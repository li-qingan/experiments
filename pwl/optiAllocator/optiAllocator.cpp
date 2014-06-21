#include "../allocate/MemoryAllocator.h"
#include <sstream>

list<TraceE*> g_Trace;  // temporarily store the function invocation/ret trace	
map<ADDRINT, UINT64> g_32Addr2WriteCount;
map<ADDRINT, UINT64> g_32Addr2FrameCount;

void ReadTrace(string szFile);

int main(int argc, char *argv[])
{
	if( argc < 4 )
	{
		cerr << "Lack of args: need four args!" << endl;
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
	
	string szOutFile = string(argv[1]) + "_" + argv[2] + "_" + argv[3];
	
	//1. read trace
	ReadTrace(szFile);
	
	//2. initialize allocators
	CHeapAllocator *allocator = new CHeapAllocator(0,1<<nSizePower, nLineSizeShift);
	allocator->init();
	
	//3. start allocating
	list<TraceE *>::iterator I = g_Trace.begin(), E = g_Trace.end();
	for(; I != E; ++ I)
	{
		TraceE *traceE = *I;
		if(traceE->_entry)
		{
			allocator->allocate(traceE);	
		}
		else
		{
			allocator->deallocate(traceE);
		}
	}
	
	//4. print
	print(szOutFile);
	return 0;
}

