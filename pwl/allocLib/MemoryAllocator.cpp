#include <assert.h>
#include "MemoryAllocator.h"

CAllocator::CAllocator(ADDRINT nStartAddr, ADDRINT nSize, ADDRINT nLineSizeShift=4) 
{
	m_nStartAddr = nStartAddr;
	m_nSize = nSize;  // in power		
	m_nLineSizeShift = nLineSizeShift;	
}

MemBlock *CStaticAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *newBlock = NULL;
	
#ifdef DEBUG
	cerr << "<try allocating (" << hex << obj->_nID << "," << obj->_nSize << ") from static" << endl;		
#endif

	ADDRINT nRemain = m_nSize - m_nCurrent;
	if( obj->_nSize > nRemain )
	{
		cerr << "@Error: failed to allocate " << hex << obj->_nSize << " with only " << nRemain << "bytes remaining" << endl;
		assert(false);
		return NULL;
	}

#ifdef DEBUG
	cerr << "@(" <<hex << m_nCurrent << "," << obj->_nSize << ")" << endl;
#endif	
	newBlock = new MemBlock(obj, m_nCurrent, obj->_nSize);
	obj->_block = newBlock;
	m_nCurrent += obj->_nSize;
	
	return newBlock;	
}

void CStaticAllocator::deallocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *block = obj->_block;
	assert(block != 0 );	
	
#ifdef DEBUG
	cerr << ">try deallocating (" <<hex << block->_nStartAddr << "," << block->_nSize << ") back to static area" << endl;
#endif	
	
	m_nCurrent -= obj->_nSize;
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

	ADDRINT nRemain = m_StackTop - m_nStartAddr;
	if( nRemain < obj->_nSize )
	{
		cerr << hex << nRemain << " cannot afford " << obj->_nSize << " bytes!" << endl;
		assert(false );
		return NULL;
	}

	newBlock = new MemBlock(obj, m_StackTop-obj->_nSize, obj->_nSize);
	obj->_block = newBlock;
	
	m_StackTop -= obj->_nSize;

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
	delete obj;
	delete block;
}

MemBlock* CHeapAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	assert(obj->_nSize != 0 );	

#ifdef DEBUG
	cerr << "<try allocating (" << hex << obj->_nID << "," << obj->_nSize << ") from heap" << endl;	
#endif
	
	// 1. search the free block to use
	int retv = 1;
	MemBlock *newBlock = NULL;
	MemBlock* block = m_lastP;
	do
	{		
		// if found the place
		if(!block->isUsed() && block->_nSize > obj->_nSize )
		{
			// allocate a new block
			newBlock = new MemBlock(obj, block->_nStartAddr, obj->_nSize);
			obj->_block = newBlock;
			block->_nSize -= obj->_nSize;
			block->_nStartAddr += obj->_nSize;
			
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
		cerr << "@Error: failed " << hex << obj->_nSize << endl;
		return NULL;
	}		
#ifdef DEBUG
	cerr << "@(" <<hex << newBlock->_nStartAddr << "," << newBlock->_nSize << ")" << endl;
#endif
	return newBlock;
}

void CHeapAllocator::deallocate(TraceE *traceE)
{
	MemBlock *block = traceE->_obj->_block;
#ifdef DEBUG
	cerr << ">try deallocating (" <<hex << block->_nStartAddr << "," << block->_nSize << ")" << endl;
	dumpFreeList();
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
		if( m_lastP == block ) 
		{
			cerr << "Strange situation!" << endl;
			assert(false);
			m_lastP = right2;
		}
		else if( m_lastP == right )              // ???may not achieve a good even usage, since a huge right block may be skipped
			m_lastP == right2;  			
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


