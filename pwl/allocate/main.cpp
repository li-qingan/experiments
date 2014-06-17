#include "MemoryAllocator.h"
#include <sstream>

list<TraceE*> g_Trace;  // temporarily store the function invocation/ret trace	
// for output the stats
vector<ADDRINT> g_32Addr2WriteCount;  // write count for each memory line
vector<ADDRINT> g_32Addr2FrameCount;  // object count for each memory line

void ReadTrace(string szFile);

int main(int argc, char *argv[])
{
	if( argc < 5 )
	{
		cerr << "Lack of args: need three args!" << endl;
		return -1;
	}

	ADDRINT nSizePower;
	stringstream ss(argv[1]);
	ss >> nSizePower;	
	cerr << "Memory Size (in bytes):\t" << hex << (1<<nSizePower) << dec << endl;	

	string szFile = argv[2];

	ADDRINT nLineSizeShift;
	stringstream ss1(argv[3]);
	ss1 >> nLineSizeShift;

	bool bStackAllocator = true;
	stringstream ss2(argv[4]);
	ss2 >> bStackAllocator;	

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
				allocatorF.allocate(traceE);
			}
			else if( obj->_region == HEAP )
			{
				allocatorH.allocate(traceE);
			}
			else if( obj->_region == GLOBAL )
				allocatorG.allocate(traceE);	
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
	allocatorF.print(
	return 0;
}

void readTrace(string szTraceFile)
{
	ifstream inf;
	inf.open(szTraceFile.c_str());
	string szLine;
	
	std::map<UINT32, Object *> hId2Object;
	// skip the prolog
	while(getline(inf, szLine) )
		if( szLine[0] == '<')
			break;

	do
	{
		Object *obj = NULL;		
		TraceE *traceE = NULL;
		Region region;
		UINT32 nID;
		bool bEntry;
		UINT32 nSize;	
		UINT32 nCount;	

		if(szLine.size() < 2)
			continue;				
		
		if(szLine[0] == '<' )   // entry
		{
			// read basic info
			if( szLine[1] == 's' )
				region = FRAME;
			else if( szLine[1] == 'h' )
				region = HEAP;
			else if( szLine[1] == 'g' )
				region = GLOBAL;
			else
				assert(false);
				
			string szInfo = szLine.substr(2);
			stringstream ss(szInfo);	
			ss >>hex>> nID >> nSize >> nCount;				
			
			obj = new Object(nID, nSize, region);  // ??? nSize + 16 for boundary tags?
			traceE = new TraceE(obj, true);	
			hId2Object[nID] = obj;			
			//cerr << "Entry " << nID << endl;	
			
			// read write count
			if( nCount > 0)
			{
				getline(inf, szLine);
				stringstream ss1(szInfo);
				for(int i =0; i < nCount; ++ i )
				{
					UINT32 nOffset;
					UINT64 nWrite;
					
					ss1 >> nOffset >> nWrite;			
					obj->_hOffset2W[nOffset] = nWrite;		
				}
			}
			
		}
		else if(szLine[0] == '>' ) // function exit
		{
			string szInfo = szLine.substr(2);
			stringstream ss(szInfo);	
			ss >>hex>> nID;		
			
			Object *obj = hId2Object[nID];			
			traceE = new TraceE(obj, false);				
			
			if( obj == NULL )
				cerr << "No entry for " << nID << endl;							
		}	
		else
			assert(false);	
		
		g_Trace.push_back(traceE);		
	}while(getline(inf, szLine));
	inf.close();
}
