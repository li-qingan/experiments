#include "../allocLib/MemoryAllocator.h"
#include "../allocLib/tool.h"
#include <sstream>



int main(int argc, char *argv[])
{
	if( argc < 6 )
	{
		cerr << "Lack of args: need seven args!" << endl;
		cerr << "arg1: input file" << endl;
		cerr << "arg2: space size (10 means 1k)" << endl;
		cerr << "arg3: memory line width in bytes, 4 bytes by default" << endl;
		cerr << "arg4: start address of heap data " << endl;
		cerr << "arg5: space size reserved for heap data " << endl;
		return -1;
	}
	ofstream os;
	os.open("default.out", ios_base::out);
	string szFile = argv[1];
	
	ADDRINT nSize;
	stringstream ss(argv[2]);
	ss >> nSize;	
	nSize = 1<<nSize;	
	
	ADDRINT nLineSize;
	stringstream ss1(argv[3]);
	ss1 >> nLineSize;	
		
	ADDRINT nStartH;
	stringstream ss2(argv[4]);
	ss2 >> nStartH;	
	//nStartH = 1<<nStartH;
	
	ADDRINT nHeapSize;
	stringstream ss3(argv[5]);
	ss3 >> nHeapSize;
	//nHeapSize = 1<<nHeapSize;

	string szOutFile = string( argv[1] ) + "." + argv[2] + "." + argv[3] + ".default";
	
	cerr << hex << "Memory size (global, heap, stack)"<<  endl;	
	cerr << hex << nSize << "\t(0\t" << nStartH << "\t" << nStartH + nHeapSize << ")" << endl;
	//os << hex << "Memory size (global, heap, stack)"<<  endl;	
	//os << hex << nSize << "\t(0\t" << nStartH << "\t" << nStartH + nHeapSize << ")" << endl;	

	//1. read trace
	readTrace(szFile);
	
	//2. initialize allocators
	CStackAllocator *allocatorF = new CStackAllocator( nStartH+nHeapSize, nSize-nStartH-nHeapSize, nLineSize );
	allocatorF->init();
	CHeapAllocator *allocatorH = new CHeapAllocator( nStartH, nHeapSize, nLineSize );
	allocatorH->init();
	CStaticAllocator *allocatorG = new CStaticAllocator( 0, nStartH, nLineSize );
	allocatorG->init();
	
	//3. start allocating
	double dTotal = 0;
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
				//os <<hex << obj->_nID << ":" << writeCompute(obj->_hOffset2W) << endl;
				//dTotal += writeCompute(obj->_hOffset2W);
			}
			else if( obj->_region == HEAP )
			{
				MemBlock *block = allocatorH->allocate(traceE);
				updateStats(obj, block);
				//os <<hex << obj->_nID << ":" << writeCompute(obj->_hOffset2W) << endl;
				//dTotal += writeCompute(obj->_hOffset2W);
			}
			else if( obj->_region == GLOBAL )
			{
				MemBlock *block = allocatorG->allocate(traceE);
				updateStats(obj, block);	
				//os <<hex << obj->_nID << ":" << writeCompute(obj->_hOffset2W) << endl;
				//dTotal += writeCompute(obj->_hOffset2W);
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
	print(szOutFile, nSize, nLineSize);
	
	//5. wear leveling compute
	wearCompute(nSize, nLineSize, os);
	os << "Total:\t" << dTotal << endl;
	os.close();
	return 0;
}


