#ifndef TOOL_H
#define TOOL_H

#include "MemoryAllocator.h"
#include <list>
#include <iostream>
#include <map>
#include <vector>
#include <math.h>


using namespace std;


extern list<TraceE*> g_Trace;  // temporarily store the function invocation/ret trace	
extern map<ADDRINT, UINT64> g_32Addr2WriteCount;
extern map<ADDRINT, UINT64> g_32Addr2FrameCount;

void readTrace(string szFile);
void updateStats(Object *obj, MemBlock *block);
void print(string szFile, UINT64 nSize);
double wearCompute(UINT64 nSize, ofstream &outf);

#endif
