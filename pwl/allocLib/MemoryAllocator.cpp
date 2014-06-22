#include <assert.h>
#include "MemoryAllocator.h"


#define DELETE_FROM_LIST(block) block->getPrev()->setNext(block->getNext()); \
								block->getNext()->setPrev(block->getPrev());
								
#define INSERT_BEFORE_LIST(block, cur) block->setPrev(cur->getPrev()); \
										block->setNext(cur); \
										cur->getPrev()->setNext(block); \
										cur->setPrev(block);
								

CAllocator::CAllocator(ADDRINT nStartAddr, ADDRINT nSize, ADDRINT nLineSizeShift=4) 
{
	m_nStartAddr = nStartAddr;
	m_nSize = nSize;  // in power		
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
	
#ifdef DEBUG
	cerr << "<try allocating (" << hex << obj->_nID << "," << obj->_nSize << ")" << endl;	
	dumpFreeList();
#endif
	
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
			
			// updating physical position 
			newBlock->setLeft(block->getLeft());
			block->getLeft()->setRight(newBlock);
			newBlock->setRight(block);
			block->setLeft(newBlock);
			
			// no change in free list
			
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
			
			// no change in physical position
			// updating free list			
			m_lastP = block->getRight();
			DELETE_FROM_LIST(block);			
			
			retv = 0;
			break;
		}
		block = block->getNext();
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
		if( m_lastP == left ) 
		{
			m_lastP = left->getNext();
			if( m_lastP == right )
				m_lastP == right->getNext(); ////////// to do!!!!
			DELETE_FROM_LIST(right);
		}
		else
		{			
			DELETE_FROM_LIST(left);	
			INSERT_BEFORE_LIST(left, m_lastP);  
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
		
		// updating free list			
		if( m_lastP == left )
		{
			m_lastP = right;						
		}
		else
		{
			DELETE_FROM_LIST(left);	
			INSERT_BEFORE_LIST(left, m_lastP); 
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
		
		// updating free list
		if( m_lastP == right )
		{			
			m_lastP = right2;			
		}
		else
		{
			DELETE_FROM_LIST(right);
			INSERT_BEFORE_LIST(right, m_lastP); // keep the latest recently used (LRU) block used much later
		}			
		delete block; 
	}	
	else
	{
		// no change in physical position
		block->setUsed(false);		
		// updating free list
		INSERT_BEFORE_LIST(block, m_lastP);
	}
}

void CHeapAllocator::dumpFreeList()
{
	MemBlock *block = m_lastP;
	do
	{
		cerr << "(" << block->_nStartAddr << "," << block->_nSize << ")\t->\t" ;
		block = block->getNext();
	}while (block != m_lastP);
	cerr << endl;
}


