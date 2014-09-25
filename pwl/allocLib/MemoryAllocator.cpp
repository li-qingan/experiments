#include <assert.h>
#include "MemoryAllocator.h"

CAllocator::CAllocator(ADDRINT nStartAddr, ADDRINT nSize, ADDRINT nLineSize=4) 
{
	m_nStartAddr = nStartAddr;
	m_nSize = nSize;  // in power		
	m_nLineSize = nLineSize;	
}

MemBlock *CStaticAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *newBlock = NULL;
	
#ifdef DEBUG
	cerr << "<try allocating (" << hex << obj->_nID << "," << obj->_nSize << ") from static" << endl;		
#endif
	m_nCurrent = alignUp(m_nCurrent, m_nLineSize);
	UINT64 nSize = alignUp(obj->_nSize, m_nLineSize);

	ADDRINT nRemain = m_nSize - m_nCurrent;
	if(nSize > nRemain )
	{
		cerr << "@Error: failed to allocate " << hex << obj->_nSize << " with only " << nRemain << "bytes remaining" << endl;
		assert(false);
		return NULL;
	}

#ifdef DEBUG
	//cerr << "@(" <<hex << m_nCurrent << "," << obj->_nSize << ")" << endl;
#endif	
	newBlock = new MemBlock(obj, m_nCurrent, nSize);
	obj->_block = newBlock;
	m_nCurrent += nSize;
	
	return newBlock;	
}

void CStaticAllocator::deallocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *block = obj->_block;
	assert(block != 0 );	
	
#ifdef DEBUG
	//cerr << ">try deallocating (" <<hex << block->_nStartAddr << "," << block->_nSize << ") back to static area" << endl;
#endif	
	
	m_nCurrent -= block->_nSize;
	delete obj;
	delete block;
}

MemBlock *CStackAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *newBlock = NULL;
	
#ifdef DEBUG
	cerr << "<try allocating (" << hex << obj->_nID << "," << obj->_nSize << ") from stack" << endl;	
#endif

	m_StackTop = alignDown(m_StackTop, m_nLineSize);
	UINT64 nSize = alignUp(obj->_nSize, m_nLineSize);
	ADDRINT nRemain = m_StackTop - m_Bottom;
	if( nRemain < nSize )
	{
		cerr << hex << nRemain << " cannot afford " << obj->_nSize << " bytes!" << endl;
		assert(false );
		return NULL;
	}
	m_StackTop = m_StackTop-nSize;
	newBlock = new MemBlock(obj, m_StackTop, nSize);
	obj->_block = newBlock;
	

#ifdef DEBUG
	cerr << "@(" <<hex << newBlock->_nStartAddr << "," << newBlock->_nSize << ")" << endl;
#endif
	return newBlock;
}

void CStackAllocator::deallocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *block = obj->_block;
	assert(block != 0 );	
	
#ifdef DEBUG
	cerr << ">try deallocating (" <<hex << block->_nStartAddr << "," << block->_nSize << ") back to stack" << endl;
#endif	
	
	m_StackTop += block->_nSize;
	assert(m_StackTop < 0x100001 );
	delete obj;
	delete block;
}

MemBlock* CHeapAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	assert(obj->_nSize != 0 );	

#ifdef DEBUG
	//cerr << "<try allocating (" << hex << obj->_nID << "," << obj->_nSize << ") from heap" << endl;	
	//dumpFreeList();
#endif
	
	// 1. search the free block to use
	UINT64 nSize = alignUp(obj->_nSize, m_nLineSize);
	int retv = 1;
	MemBlock *newBlock = NULL;
	MemBlock* block = m_lastP;
	do
	{		
		// if found the place
		if(!block->isUsed() && block->_nSize >= nSize )
		{
			// allocate a new block
			newBlock = new MemBlock(obj, block->_nStartAddr, nSize);
			obj->_block = newBlock;
			block->_nSize = block->_nSize - nSize;
			block->_nStartAddr= block->_nStartAddr + nSize;
			
			
			// updating physical position 			
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
		else if( !block->isUsed() && block->_nSize == obj->_nSize )
		{
			newBlock = block;
			newBlock->_obj = obj;
			obj->_block = newBlock;
			newBlock->setUsed(true);
			
			// no change in physical position
			m_lastP = block->getRight();	
			
			retv = 0;
			break;
		}
		block = block->getRight();
	}while( block != m_lastP);
	
	if( retv != 0 )
	{
		cerr << "@Error: failed for (" << hex << obj->_nID << "," << obj->_nSize << ")" << endl;
		return NULL;
	}		
#ifdef DEBUG
	//cerr << "@(" <<hex << newBlock->_nStartAddr << "," << newBlock->_nSize << ")" << endl;
#endif
	return newBlock;
}

void CHeapAllocator::deallocate(TraceE *traceE)
{
	MemBlock *block = traceE->_obj->_block;
#ifdef DEBUG
	cerr << ">try deallocating (" <<hex << block->_nStartAddr << "," << block->_nSize << ")" << endl;
	//dumpFreeList();
#endif
	
	//cerr << hex << "dealloc: " << block->_nStartAddr << "--" << traceE->_nFrameSize << endl;		
	
	// release and immediate coalescing
	MemBlock *left = block->getLeft(), *right = block->getRight();
	MemBlock *right2 = right->getRight();
	if(!left->isUsed() && !right->isUsed())  // the left absorbs both right and current blocks
	{		
		// updating physical position		
		left->setRight(right2); 
		right2->setLeft(left);
		left->_nSize += block->_nSize + right->_nSize;		
		
		// updating free list				
		if( m_lastP == block || m_lastP == right) // ???may not achieve a good even usage, since a huge right block may be skipped
		{			
			m_lastP = right2;
		}		
		delete block; 
		delete right;  		
		
		
	}
	else if( !left->isUsed() ) // absorbed by the left block
	{
		//updating physical position		
		left->setRight(right); 
		right->setLeft(left);
		left->_nSize += block->_nSize;
		
		// updating last place			
		if( m_lastP == block )
		{
			m_lastP = right;						
		}		
		delete block; 	
	}
	else if( !right->isUsed() )  // absorbed by the right block
	{		
		// updating physical position		
		right->setLeft(left);
		left->setRight(right);
		right->_nSize += block->_nSize;	
		right->_nStartAddr -= block->_nSize;	
		
		// updating last place		
		if( m_lastP == block )	
			m_lastP == right;
		delete block; 
	}	
	else
	{
		// no change in physical position
		block->setUsed(false);	
	}
}

void CHeapAllocator::dumpFreeList()
{
	MemBlock *block = m_lastP;
	do
	{
		cerr << "(" << block->_nStartAddr << "," << block->_nSize << "," << block->isUsed() << ")\t->\t" ;
		block = block->getRight();
	}while (block != m_lastP);
	cerr << endl;
}

UINT64 CAllocator::alignUp(UINT64 nAddr, UINT64 nLineSize)
{
	UINT64 nFactor = (nAddr+nLineSize-1)/nLineSize;
	return nFactor * nLineSize;
}
UINT64 CAllocator::alignDown(UINT64 nSize, UINT64 nLineSize)
{
	UINT64 nFactor = nSize/nLineSize;
	return nFactor * nLineSize;
}

