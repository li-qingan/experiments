#include "../allocLib/MemoryAllocator.h"
#include "../allocLib/tool.h"
#include <sstream>

int main(int argc, char *argv[])
{
	if( argc < 4 )
	{
		cerr << "Lack of args: need four args!" << endl;
		cerr << "arg1: input file" << endl;
		cerr << "arg2: power vaule of space size" << endl;
		cerr << "arg3: memory line width in bytes, 4 bytes by default" << endl;
		return -1;
	}

	string szFile = argv[1];
	
	UINT64 nSizePower, nSize;
	stringstream ss(argv[2]);
	ss >> nSizePower;	
	nSize = 1<< nSizePower;
	cerr << "Memory Size (in bytes):\t" << hex << nSize << dec << endl;	

	ADDRINT nLineSize;
	stringstream ss1(argv[3]);
	ss1 >> nLineSize;
	
	string szOutFile = string(argv[1]) + "_" + argv[2] + "_" + argv[3];
	
	//1. read trace
	readTrace(szFile);
	
	//2. initialize allocators
	CHeapAllocator *allocator = new CHeapAllocator(0, nSize, nLineSize);
	allocator->init();
	
	//3. start allocating
	list<TraceE *>::iterator I = g_Trace.begin(), E = g_Trace.end();
	for(; I != E; ++ I)
	{
		TraceE *traceE = *I;
		Object *obj = traceE->_obj;
		if(traceE->_entry)
		{
			MemBlock *block = allocator->allocate(traceE);	
			updateStats(obj, block);
		}
		else
		{
			allocator->deallocate(traceE);
		}
	}
	
	//4. print
	print(szOutFile, nSize);
	return 0;
}

