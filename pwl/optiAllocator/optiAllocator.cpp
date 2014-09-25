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
	ofstream os;
	os.open("opti.out", ios_base::out);

	string szFile = argv[1];
	
	UINT64 nSizePower, nSize;
	stringstream ss(argv[2]);
	ss >> nSizePower;	
	nSize = 1<< nSizePower;
	cerr << "Memory Size (in bytes):\t" << hex << nSize << endl;	

	ADDRINT nLineSize;
	stringstream ss1(argv[3]);
	ss1 >> nLineSize;
	
	string szOutFile = string(argv[1]) + "." + argv[2] + "." + argv[3] + ".opt";
	
	//1. read trace
	readTrace(szFile);
	
	//2. initialize allocators
	CHeapAllocator *allocator = new CHeapAllocator(0, nSize, nLineSize);
	allocator->init();
	
	double dTotal = 0;
	//3. start allocating
	list<TraceE *>::iterator I = g_Trace.begin(), E = g_Trace.end();
	for(; I != E; ++ I)
	{
		TraceE *traceE = *I;
		Object *obj = traceE->_obj;
		if(traceE->_entry)
		{
			os <<hex << obj->_nID << ":" << writeCompute(obj->_hOffset2W) << endl;
			dTotal += writeCompute(obj->_hOffset2W);
			MemBlock *block = allocator->allocate(traceE);	
			updateStats(obj, block);
		}
		else
		{
			allocator->deallocate(traceE);
		}
	}
	
	//4. print
	print(szOutFile, nSize, nLineSize);
	
	//5. wear leveling compute
	wearCompute(nSize, nLineSize, os);
	os << "Total:\t" << dTotal << endl;
	os.close();
	return 0;
}

