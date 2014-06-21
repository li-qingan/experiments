#include <list>
#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include "MemoryAllocator.h"

using namespace std;


extern list<TraceE*> g_Trace;  // temporarily store the function invocation/ret trace	
extern map<ADDRINT, UINT64> g_32Addr2WriteCount;
extern map<ADDRINT, UINT64> g_32Addr2FrameCount;

void ReadTrace(string szFile);
void UpdateStats(Object *obj, MemBlock *block);
void print(string szFile);
double wearCompute(UINT64 nSize);
