#include "aux.h"

list<TraceE*> g_Trace;  // temporarily store the function invocation/ret trace	
map<ADDRINT, UINT64> g_32Addr2WriteCount;
map<ADDRINT, UINT64> g_32Addr2FrameCount;

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
				for(UINT32 i =0; i < nCount; ++ i )
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

// 2. update write count and object count for each address
void UpdateStats(Object *obj, MemBlock *block)
{
	std::map<UINT32, UINT64>::iterator w_p = obj->_hOffset2W.begin(), w_e = obj->_hOffset2W.end();
	for(; w_p != w_e; ++ w_p )
	{
		g_32Addr2WriteCount[block->_nStartAddr + w_p->first] += w_p->second;
		++ g_32Addr2FrameCount[block->_nStartAddr + w_p->first];
	}	
}

void print(string szOutFile, ADDRINT nSize)
{			
	
	ofstream outf;
	outf.open(szOutFile.c_str(), ios_base::out);
	std::map<ADDRINT, UINT64>::iterator I_p = g_32Addr2FrameCount.begin(), I_e = g_32Addr2FrameCount.end();
	for(; I_p != I_e; ++ I_p )	
	{		
		outf << hex << I_p->first  << "\t" << g_32Addr2FrameCount[I_p->second] << "\t" << g_32Addr2WriteCount[I_p->second]<< endl;
	}	
	outf.close();
	
	string szFileSparse = szOutFile + "_sparse";
	outf.open(szFileSparse.c_str(), ios_base::out);
	for( UINT64 i = 0; i < nSize; ++ i )
	{
		outf << hex << I_p->first  << "\t" << g_32Addr2FrameCount[I_p->second] << "\t" << g_32Addr2WriteCount[I_p->second]<< endl;
	}
	outf.close();
}

double wearCompute(UINT64 nSize)
{
	vector<UINT64> writes(nSize,0);
	UINT64 worst = 0;
	
	for( UINT64 i = 0; i < nSize; ++ i )
	{
		writes[i] = g_32Addr2WriteCount[i];
		if(writes[i] > worst)
			worst = writes[i];
	}
	
	
	// 1. compute the expected value
	double sum = 0.0;
	vector<UINT64>::iterator I = writes.begin(), E= writes.end();
	for(; I != E; ++ I )
		sum += *I;	

	double exp = sum/writes.size();
	cerr << "sum/exp:\t" << sum << "/" << exp << endl;

	// 2. compute sum of variance
	double sumVariance = 0.0;
	I = writes.begin();
	for(; I != E; ++ I )
	{
		//cerr << sumVariance << ":\t";
		double d = *I;
		sumVariance += pow(d-exp, 2);
		//cerr << " + power of " << d << "-" << exp << "=" << sumVariance << endl;		
	}
	
	//cerr << "sumVariance:\t" << sumVariance << endl;

	// 3. compute standard deviation
	double stdDev = sumVariance/(nSize-1);
	//???
	double wear = sqrt(stdDev)/exp;
	cerr << "worst:\t" << worst << endl;
	cerr << "wear:\t" << wear << endl;
	
	return wear;
}

