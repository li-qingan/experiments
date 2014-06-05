#include <string>
#include <iostream>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#define UINT32 unsigned int
#define UINT64 long long 
#define ADDRINT unsigned int


struct TraceE
{
	string _szFunc;
	bool _bCallOrRet;
	ADDRINT _nFrameSize;
	UINT64 _nWriteCount;
	UINT32 _nID;

	TraceE(string szFunc, bool bCallOrRet, ADDRINT nFrameSize, UINT64 nWriteCount, UINT32 nID)
	{
		_szFunc = szFunc;
		_bCallOrRet = bCallOrRet;
		_nFrameSize = nFrameSize;
		_nWriteCount = nWriteCount;
		_nID = nID;
	}
};


struct MemBlock
{
	ADDRINT _nStartAddr;
	ADDRINT _nSize;
	UINT32 _nID;

	MemBlock(UINT32 nID, ADDRINT addr, ADDRINT nSize)
	{
		_nID = nID;
		_nStartAddr = addr;
		_nSize = nSize;
	}
};

class CAllocator
{
public:
	CAllocator(ADDRINT nSize, ADDRINT nLineSize, string szTraceFile);	
	void run();
	void readTrace();	
	void print(string szFile);
	virtual void dump(){};
	
protected:
	virtual int allocate(TraceE *traceE){ return 0;};
	virtual void deallocate(TraceE *traceE){};

protected:
	list<TraceE*> m_Trace;  // temporarily store the function invocation/ret trace	
	list<MemBlock*> m_freeBlocks; // the free list	
	

	// for output
	vector<double> m_32Addr2WriteCount;  // write count for each memory line
	vector<UINT64> m_32Addr2FrameCount;  // frame count for each memory line
	
	ADDRINT m_nSizePower;
	ADDRINT m_nSize;	// the size of the memory space
	UINT32 m_nLineSizeShift;	// the size of each memory line which consider the write count as a whole
	string m_szTraceFile; // the file name of trace	
};

class CStackAllocator: public CAllocator
{
public:
	CStackAllocator(ADDRINT nSize, ADDRINT nLineSize, string szTraceFile) : CAllocator(nSize, nLineSize, szTraceFile) 
	{
	}
private:
	virtual int allocate(TraceE *traceE);
	virtual void deallocate(TraceE *traceE);	
	virtual void dump();
private:
	list<MemBlock*>  m_Blocks; // the used memory block list
};

class CHeapAllocator: public CAllocator
{
public:
	CHeapAllocator(ADDRINT nSize, ADDRINT nLineSize, string szTraceFile) : CAllocator(nSize, nLineSize, szTraceFile)
	{
		MemBlock *block = new MemBlock(0, m_nSize-1, m_nSize);
		m_freeBlocks.push_back(block);
		m_lastPlace = m_freeBlocks.begin();
	}
private:
	virtual int allocate(TraceE *traceE);
	virtual void deallocate(TraceE *traceE);
	virtual void dump();

private:
	list<MemBlock*>::iterator m_lastPlace;	
	map<UINT32, MemBlock *> m_hId2Block; // the memory block allocated for each entry
};
