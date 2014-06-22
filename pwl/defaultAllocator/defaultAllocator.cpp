#include "../allocLib/MemoryAllocator.h"
#include "../allocLib/tool.h"
#include <sstream>



int main(int argc, char *argv[])
{
	if( argc < 7 )
	{
		cerr << "Lack of args: need seven args!" << endl;
		return -1;
	}

	string szFile = argv[1];
	
	ADDRINT nSizePower;
	stringstream ss(argv[2]);
	ss >> nSizePower;	
	UINT64 nSize = 1<<nSizePower;
	cerr << "Memory Size (in bytes):\t" << hex << nSize << dec << endl;	

	ADDRINT nLineSize;
	stringstream ss1(argv[3]);
	ss1 >> nLineSize;
	
	ADDRINT nStartG;
	stringstream ss2(argv[4]);
	ss2 >> nStartG;
	
	ADDRINT nStartH;
	stringstream ss3(argv[5]);
	ss3 >> nStartH;	

	string szOutFile = string( argv[1] ) + "_" + argv[2] + "_" + argv[3];

	//1. read trace
	readTrace(szFile);
	
	//2. initialize allocators
	CStackAllocator *allocatorF = new CStackAllocator( 1<<nSizePower, (nSize-nStartH)/2, nLineSize );
	allocatorF->init();
	CHeapAllocator *allocatorH = new CHeapAllocator( nStartH, (nSize-nStartH)/2, nLineSize );
	allocatorH->init();
	CStackAllocator *allocatorG = new CStackAllocator( nStartG, nStartH - nStartG, nLineSize );
	allocatorG->init();
	
	//3. start allocating
	list<TraceE *>::iterator I = g_Trace.begin(), E = g_Trace.end();
	for(; I != E; ++ I)
	{
		TraceE *traceE = *I;
		Object *obj = traceE->_obj;
		if(traceE->_entry)
		{			
			if( obj->_region == FRAME )
			{
				MemBlock *block = allocatorF->allocate(traceE);
				updateStats(obj, block);
			}
			else if( obj->_region == HEAP )
			{
				MemBlock *block = allocatorH->allocate(traceE);
				updateStats(obj, block);
			}
			else if( obj->_region == GLOBAL )
			{
				MemBlock *block = allocatorG->allocate(traceE);
				updateStats(obj, block);	
			}
		}
		else
		{
			if( obj->_region == FRAME )
			{
				allocatorF->deallocate(traceE);
			}
			else if( obj->_region == HEAP )
			{
				allocatorH->deallocate(traceE);
			}
			else if( obj->_region == GLOBAL )
				allocatorG->deallocate(traceE);
		}
	}
	
	//4. print
	print(szOutFile, nSize);
	
	//5. wear leveling compute
	wearCompute(1<<nSizePower);
	
	return 0;
}


