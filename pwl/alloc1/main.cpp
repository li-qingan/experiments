#include "MemoryAllocator.h"
#include "aux.h"
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
	cerr << "Memory Size (in bytes):\t" << hex << (1<<nSizePower) << dec << endl;	

	ADDRINT nLineSizeShift;
	stringstream ss1(argv[3]);
	ss1 >> nLineSizeShift;
	
	ADDRINT nStartG;
	stringstream ss2(argv[4]);
	ss2 >> nStartG;
	
	ADDRINT nStartH;
	stringstream ss3(argv[5]);
	ss3 >> nStartH;	

	string szOutFile = argv[1] + "_" + argv[2] + "_" + argv[3];

	//1. read trace
	ReadTrace(szFile);
	
	//2. initialize allocators
	CStackAllocator *allocatorF = new CStackAllocator();
	allocatorS.init();
	CDynamicAllocator *allocatorH = new CDynamicAllocator();
	allocatorH.init();
	CStackAllocator *allocatorG = new CStackAllocator();
	allocatorG.init();
	
	//3. start allocating
	list<TraceE *>::iterator I = g_Trace.begin(), E = g_Trace.end();
	for(; I != E; ++ I)
	{
		TraceE *traceE = *I;
		if(traceE->_entry)
		{
			Object *obj = traceE->_obj;
			if( obj->_region == FRAME )
			{
				MemBlock *block = allocatorF.allocate(traceE);
				UpdateStats(obj, block);
			}
			else if( obj->_region == HEAP )
			{
				MemBlock *block = allocatorH.allocate(traceE);
				UpdateStats(obj, block);
			}
			else if( obj->_region == GLOBAL )
			{
				MemBlock *block = allocatorG.allocate(traceE);
				UpdateStats(obj, block);	
			}
		}
		else
		{
			if( obj->_region == FRAME )
			{
				allocatorF.deallocate(traceE);
			}
			else if( obj->_region == HEAP )
			{
				allocatorH.deallocate(traceE);
			}
			else if( obj->_region == GLOBAL )
				allocatorG.deallocate(traceE);
		}
	}
	
	//4. print
	print(szOutFile);
	
	//5. wear leveling compute
	wearCompute(1<<nSizePower);
	
	return 0;
}


