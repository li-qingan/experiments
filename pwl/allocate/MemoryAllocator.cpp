#include "MemoryAllocator.h"
#include <assert.h>

#define DELETE_FROM_LIST(block) block->getPrev()->setNext(block->getNext()); \
								block->getNext()->setPrev(block->getPrev());
								
#define INSERT_BEFORE_LIST(block, cur) block->setPrev(cur->getPrev()); \
										block->setNext(cur); \
										cur->getPrev()->setNext(block); \
										cur->setPrev(block);
								

CAllocator::CAllocator(ADDRINT nStartAddr, ADDRINT nSizePower, ADDRINT nLineSizeShift, string szTraceFile) 
{
	m_nStartAddr = nStartAddr;
	m_nSize = 1 << nSizePower;  // in power	
	m_nSizePower = nSizePower;
	m_nLineSizeShift = nLineSizeShift;
	m_szTraceFile = szTraceFile;
}

void CAllocator::run()
{	
	m_32Addr2WriteCount.assign(m_nSize >> m_nLineSizeShift,0.0);
	m_32Addr2FrameCount.assign(m_nSize >> m_nLineSizeShift,0);


	readTrace();

	list<TraceE *>::iterator I = m_Trace.begin(), E = m_Trace.end();
	for(; I != E; ++ I)
	{
		TraceE *traceE = *I;
		if(traceE->_entry)
		{
			int retv = allocate(traceE);
			if( retv != 0 )
			{
				cerr << "Error: memory overflow!" << endl;
				return;
			}
		}
		else
			deallocate(traceE);
	}

	dump();
}

void CAllocator::readTrace()
{
	ifstream inf;
	inf.open(m_szTraceFile.c_str());
	string szLine;
	
	// skip the prolog
	while(getline(inf, szLine) )
		if( szLine[0] == '<')
			break;

	do
	{
		Object *obj = NULL;		
		TraceE *traceE = NULL;
		Region region;
		UINT32 nID;
		bool bEntry;
		UINT32 nSize;	
		UINT32 nCount;	

		if(szLine.size() < 2)
			continue;				
		
		if(szLine[0] == '<' )   // entry
		{
			// read basic info
			if( szLine[1] == 's' )
				region = FRAME;
			else if( szLine[1] == 'h' )
				region = HEAP;
			else if( szLine[1] == 'g' )
				region = GLOBAL;
			else
				assert(false);
				
			string szInfo = szLine.substr(2);
			stringstream ss(szInfo);	
			ss >>hex>> nID >> nSize >> nCount;				
			
			obj = new Object(nID, nSize, region);  // ??? nSize + 16 for boundary tags?
			traceE = new TraceE(obj, true);	
			m_hId2Object[nID] = obj;			
			//cerr << "Entry " << nID << endl;	
			
			// read write count
			if( nCount > 0)
			{
				getline(inf, szLine);
				stringstream ss1(szInfo);
				for(int i =0; i < nCount; ++ i )
				{
					UINT32 nOffset;
					UINT64 nWrite;
					
					ss1 >> nOffset >> nWrite;			
					obj->_hOffset2W[nOffset] = nWrite;		
				}
			}
			
		}
		else if(szLine[0] == '>' ) // function exit
		{
			string szInfo = szLine.substr(2);
			stringstream ss(szInfo);	
			ss >>hex>> nID;		
			
			Object *obj = m_hId2Object[nID];			
			traceE = new TraceE(obj, false);				
			
			if( obj == NULL )
				cerr << "No entry for " << nID << endl;							
		}	
		else
			assert(false);	
		
		m_Trace.push_back(traceE);		
	}while(getline(inf, szLine));
	inf.close();
}

void CAllocator::print(string szOutFile)
{			
	
	ofstream outf;
	outf.open(szOutFile.c_str());


	ADDRINT index = 0;
	ADDRINT nLines = m_nSize >> m_nLineSizeShift;
	for(; index < nLines; ++ index )
	{		
		outf << hex << (index << m_nLineSizeShift)  << "\t" <<dec << m_32Addr2FrameCount[index] << "\t" <<dec << m_32Addr2WriteCount[index]<< endl;

	}
	

	outf.close();
}

void CStackAllocator::dump()
{
	if( !m_Blocks.empty() )	
		cerr << hex << m_Blocks.front()->_obj->_nID << " is not popped!" <<dec << endl;
	char digits[16];
	sprintf(digits, "%d", m_nSizePower);
	string szOutFile = m_szTraceFile + "_" + digits;
	sprintf(digits, "%d", 1);
	szOutFile = szOutFile + "_" + digits;
	print(szOutFile);
}



int CStackAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	assert(obj->_nSize != 0 );
	MemBlock *newBlock = NULL, *lastBlock = NULL;
	
	// stack allocation
	if( m_Blocks.empty() )
	{
		newBlock = new MemBlock(obj, m_nSize-1, obj->_nSize);
		obj->_block = newBlock;
	}
	else
	{		
		lastBlock = m_Blocks.front();
		if(lastBlock->_nStartAddr < obj->_nSize )    // ??? <= or <
		{
			cerr << hex << lastBlock->_nStartAddr << " cannot afford " << obj->_nSize << " bytes!" << endl;
			assert(false );
		}
		ADDRINT addr = lastBlock->_nStartAddr-lastBlock->_obj->_nSize;		
		
		//cerr << lastBlock->_nStartAddr << "--" << lastBlock->_nSize << endl;
		//cerr << "==allocate " << traceE->_nID << "-" << traceE->_nFrameSize << endl;
		
		newBlock = new MemBlock(obj, addr, obj->_nSize);
		obj->_block = newBlock;
	}	
	
	m_Blocks.push_front(newBlock);
	
	// 2. update write count and object count for each address
	std::map<UINT32, UINT64>::iterator w_p = obj->_hOffset2W.begin(), w_e = obj->_hOffset2W.end();
	for(; w_p != w_e; ++ w_p )
	{
		m_32Addr2WriteCount[newBlock->_nStartAddr + w_p->first] += w_p->second;
		++ m_32Addr2FrameCount[newBlock->_nStartAddr + w_p->first];
	}	
	return 0;
}

void CStackAllocator::deallocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	assert(obj->_nSize != 0 );
	//cerr << "==deallocate " << traceE->_nID << "-" << traceE->_nFrameSize << endl;
	if( m_Blocks.empty() )
		cerr << "No entry for traceE->_nID! " << endl;
	MemBlock *topBlock = m_Blocks.front();
	if( topBlock->_obj != obj )
		cerr << topBlock->_obj->_nID << " doesn't match " << obj->_nID << endl;
	delete obj;
	delete topBlock;
	m_Blocks.pop_front();
}

void CHeapAllocator::dump()
{
	char digits[16];
	sprintf(digits, "%d", m_nSizePower);
	string szOutFile = m_szTraceFile + "_" + digits;
	sprintf(digits, "%d", 0);
	szOutFile = szOutFile + "_" + digits;
	print(szOutFile);
}

int CHeapAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	assert(obj->_nSize != 0 );
	
	// 1. search the free block to use
	int retv = 1;
	MemBlock *newBlock = NULL;
	MemBlock* block = m_lastP;
	do
	{		
		// if found the place
		if(block->_nSize > obj->_nSize )
		{
			// allocate a new block
			newBlock = new MemBlock(obj, block->_nStartAddr, obj->_nSize);
			obj->_block = newBlock;
			block->_nSize -= obj->_nSize;
			block->_nStartAddr += obj->_nSize;
			
			newBlock->setLeft(block->getLeft());
			block->getLeft()->setRight(newBlock);
			newBlock->setRight(block);
			block->setLeft(newBlock);
			
			//cerr << hex << "alloc: " << block->_nStartAddr << "--" << traceE->_nFrameSize << endl;		

			// update last place
			m_lastP = block;			
			retv = 0;
			break;
		}
		else if( block->_nSize == obj->_nSize )
		{
			newBlock = block;
			newBlock->_obj = obj;
			obj->_block = newBlock;
			newBlock->setUsed(true);
			
			m_lastP = block->getRight();
			DELETE_FROM_LIST(block);			
			
			retv = 0;
			break;
		}
		block = block->getNext();
	}while( block != m_lastP);
	
	if( retv != 0 )
	{
		cerr << "While allocating an object of " << hex << obj->_nSize << endl;
		return retv;
	}	
	

	// 2. update write count and object count for each address
	std::map<UINT32, UINT64>::iterator w_p = obj->_hOffset2W.begin(), w_e = obj->_hOffset2W.end();
	for(; w_p != w_e; ++ w_p )
	{
		m_32Addr2WriteCount[newBlock->_nStartAddr + w_p->first] += w_p->second;
		++ m_32Addr2FrameCount[newBlock->_nStartAddr + w_p->first];
	}	
	return 0;
}

void CHeapAllocator::deallocate(TraceE *traceE)
{
	MemBlock *block = traceE->_obj->_block;
	assert(block->_nSize != 0 );
	
	//cerr << hex << "dealloc: " << block->_nStartAddr << "--" << traceE->_nFrameSize << endl;		
	
	// release and immediate coalescing
	MemBlock *left = block->getLeft(), *right = block->getRight();
	if(!left->isUsed() && !right->isUsed())
	{
		MemBlock *right2 = right->getRight();
		
		// physical relation
		left->setRight(right2); // for left
		left->_nSize += block->_nSize + right->_nSize;		
		
		DELETE_FROM_LIST(left);		// keep the latest recently used (LRU) block be used much later
		if( m_lastP == left || m_lastP == right ) 
			m_lastP = right2;
		else
			INSERT_BEFORE_LIST(left, m_lastP);  
		DELETE_FROM_LIST(block);	// for current
		delete block; 
		DELETE_FROM_LIST(right);	// for right;
		delete right;  		
		
		
	}
	else if( !left->isUsed() )
	{
		left->setRight(right); // for left
		left->_nSize += block->_nSize;
		
		DELETE_FROM_LIST(left);		// keep the latest recently used (LRU) block be used much later
		if( m_lastP == left )
			m_lastP = right;
		else
			INSERT_BEFORE_LIST(left, m_lastP); 
		
		DELETE_FROM_LIST(block);	// for current	
		delete block; 	
	}
	else if( !right->isUsed() )
	{		
		MemBlock *right2 = right->getRight();
		block->setRight(right2); // for current
		block->_nSize += right->_nSize;
		block->setUsed(false);
		
		DELETE_FROM_LIST(block);		// keep the latest recently used (LRU) block be used much later
		if( m_lastP == right )
			m_lastP = right2;
		else
			INSERT_BEFORE_LIST(block, m_lastP); 
		
		DELETE_FROM_LIST(right);	// for right
		delete right; 
	}	
}


