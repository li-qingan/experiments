#include <assert.h>
#include "MemoryAllocator.h"


#define DELETE_FROM_LIST(block) block->getPrev()->setNext(block->getNext()); \
								block->getNext()->setPrev(block->getPrev());
								
#define INSERT_BEFORE_LIST(block, cur) block->setPrev(cur->getPrev()); \
										block->setNext(cur); \
										cur->getPrev()->setNext(block); \
										cur->setPrev(block);
								

CAllocator::CAllocator(ADDRINT nStartAddr, ADDRINT nSizePower, ADDRINT nLineSizeShift=4) 
{
	m_nStartAddr = nStartAddr;
	m_nSize = 1 << nSizePower;  // in power	
	m_nSizePower = nSizePower;
	m_nLineSizeShift = nLineSizeShift;	
}

MemBlock *CStackAllocator::allocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *newBlock = NULL;
	
	// stack allocation
	if(m_StackTop->_nSize < obj->_nSize )
	{
		cerr << hex << m_StackTop->_nSize << " cannot afford " << obj->_nSize << " bytes!" << endl;
		assert(false );
		return NULL;
	}
	
	newBlock = new MemBlock(obj, m_StackTop->_nStartAddr-obj->_nSize+1, obj->_nSize);
	obj->_block = newBlock;
	
	m_StackTop->_nStartAddr -= obj->_nSize;
	
	return newBlock;
}

void CStackAllocator::deallocate(TraceE *traceE)
{
	Object *obj = traceE->_obj;
	MemBlock *block = obj->_block;
	assert(block != 0 );	
	
	m_StackTop->_nStartAddr += block->_nSize;
	m_StackTop->_nSize += block->_nSize;
	delete obj;
	delete block;
}

MemBlock* CHeapAllocator::allocate(TraceE *traceE)
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
		return NULL;
	}		

	return newBlock;
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


