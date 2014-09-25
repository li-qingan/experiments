#include "tool.h"

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
		if( szLine[0] == '@')
			break;

	do
	{
		Object *obj = NULL;		
		TraceE *traceE = NULL;
		Region region;
		UINT32 nID;	
		UINT32 nSize;
		bool bEntry;	
		UINT32 nCount;	

		if(szLine.size() < 2)
			continue;				
		
		if(szLine[0] == '@' )   
		{
			// read basic info
			if( szLine[1] == 's' )
				region = FRAME;
			else if( szLine[1] == 'h' )
				region = HEAP;
			else if( szLine[1] == 'g' )
				region = GLOBAL;
			else
			{
				cerr << "Error in " << szLine << endl;
				assert(false);
			}
				
			string szInfo = szLine.substr(2);
			stringstream ss(szInfo);	
			ss >>hex>> nID >> nSize >> bEntry >> nCount;				
			
			if( bEntry )
			{
				obj = new Object(nID, nSize, region);  // ??? nSize + 16 for boundary tags?
				traceE = new TraceE(obj, bEntry);	
				hId2Object[nID] = obj;			
				//cerr << "Entry " << nID << endl;	
			
				// read write count
				if( nCount > 0)
				{
					getline(inf, szLine);
					stringstream ss1(szLine);
					for(UINT32 i =0; i < nCount; ++ i )
					{
						UINT32 nOffset;
						UINT64 nWrite;
					
						ss1 >> nOffset >> nWrite;			
						obj->_hOffset2W[nOffset] = nWrite;		
					}
				}
			}
			else if(!bEntry)
			{
				Object *obj = hId2Object[nID];			
				traceE = new TraceE(obj, bEntry);				
			
				if( obj == NULL )
					cerr << "Error!: No entry for " << nID << endl;		
			}			
		}
		
		else		
			assert(false);			
		g_Trace.push_back(traceE);		
	}while(getline(inf, szLine));
	inf.close();
	if(g_Trace.empty() )
		cerr << "The trace is empty!" << endl;
}

// 2. update write count and object count for each address
void updateStats(Object *obj, MemBlock *block)
{
	std::map<UINT32, UINT64>::iterator w_p = obj->_hOffset2W.begin(), w_e = obj->_hOffset2W.end();
	for(; w_p != w_e; ++ w_p )
	{	
		ADDRINT nAddr = block->_nStartAddr + w_p->first;
		if( nAddr >= 0x100001 )
			cerr << "Error!" << endl;
		g_32Addr2WriteCount[nAddr] += w_p->second;
		++ g_32Addr2FrameCount[nAddr];
	}	
}

void print(string szOutFile, UINT64 nSize, UINT32 nLineSize)
{			
	
	ofstream outf;
	string szFileDense = szOutFile + "_dense";
	outf.open(szFileDense.c_str(), ios_base::out);
	std::map<ADDRINT, UINT64>::iterator I_p = g_32Addr2FrameCount.begin(), I_e = g_32Addr2FrameCount.end();
	for(; I_p != I_e; ++ I_p )	
	{		
		outf << hex << I_p->first  << "\t" << g_32Addr2FrameCount[I_p->second] << "\t" << g_32Addr2WriteCount[I_p->second]<< endl;
	}	
	outf.close();
	
	string szFileSparse = szOutFile + "_sparse";
	outf.open(szFileSparse.c_str(), ios_base::out);
	for( UINT64 i = 0; i < nSize; i=i+nLineSize )
	{
		outf << hex << i  << "\t" << g_32Addr2FrameCount[i] << "\t" << g_32Addr2WriteCount[i]<< endl;
	}
	outf.close();
}

double wearCompute(UINT64 nSize, UINT32 nLineSize, ofstream &outf)
{
	UINT64 nEntries = nSize/nLineSize;
	vector<UINT64> writes(nEntries, 0);
	UINT64 worst = 0;
	
	double dTotal = 0;
	for( UINT64 i = 0; i < nEntries; ++i )
	{
		writes[i] = g_32Addr2WriteCount[i*nLineSize];
		dTotal += writes[i];
		
		if(writes[i] > worst)
			worst = writes[i];
	}
	//cerr << "Totol in wear:\t" << dTotal << endl;
	//cerr << "Total in count:\t" << writeCompute(g_32Addr2WriteCount);
	
	// 1. compute the expected value
	double sum = 0.0;
	vector<UINT64>::iterator I = writes.begin(), E= writes.end();
	for(; I != E; ++ I )
		sum += *I;	

	double exp = sum/writes.size();
	cerr << "sum/exp:\t" << sum << "/" << exp << endl;
	outf << "sum/exp:\t" << sum << "/" << exp << endl;

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
	double stdDev = sumVariance/(nEntries-1);
	//???
	double wear = sqrt(stdDev)/exp;
	cerr << "worst:\t" << worst << endl;
	cerr << "wear:\t" << wear << endl;
	outf << "worst:\t" << worst << endl;
	outf << "wear:\t" << wear << endl;
	
	return wear;
}

double writeCompute(std::map<ADDRINT, UINT64> &m)
{
	double dWrites = 0;
	std::map<UINT32, UINT64>::iterator w_p = m.begin(), w_e = m.end();
	for(; w_p != w_e; ++ w_p )
	{
		if( w_p->first >= 0x100000 )
			cerr << w_p->first << " over " << w_p->second << endl;
		dWrites += w_p->second;
	}
	return dWrites;
}



