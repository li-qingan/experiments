/*
This file is used to compute the VoC for PCM wear levelling.
*/


#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <math.h>
#include <sstream>

using namespace std;


#define UINT32 unsigned int
#define UINT64 long long 
#define ADDRINT unsigned int

void read(string szFile, list<double> &writes)
{
	ifstream inf;
	inf.open(szFile.c_str());
	string szLine;

	UINT32 nCount = 0;
	while(getline(inf, szLine))
	{
		double dWrite = 0.0;
		UINT64 nAddr = 0;

		if(szLine.size() < 1)
			continue;		
		++ nCount;
	
		stringstream ss(szLine);
		ss >>hex >> nAddr >> dec >> dWrite >> dWrite;
		writes.push_back(dWrite); 	
		
	}
	inf.close();
}

double compute(string szFile, UINT64 nLen)
{
	list<double> g_writes;
	read(szFile, g_writes);
	
	UINT64 nSize = g_writes.size();
	for(UINT64 i = nSize; i < nLen; ++ i )
	{
		g_writes.push_back(0.0);
	}

	// 1. compute the expected value
	double sum = 0.0;
	list<double>::iterator I = g_writes.begin(), E= g_writes.end();
	for(; I != E; ++ I )
		sum += *I;	

	double exp = sum/g_writes.size()/4;
	cerr << "sum/exp:\t" << sum << "/" << exp << endl;

	// 2. compute sum of variance
	double sumVariance = 0.0;
	I = g_writes.begin();
	for(; I != E; ++ I )
	{
		//cerr << sumVariance << ":\t";
		double d = *I/4;
		sumVariance += pow(d-exp, 2);
		//cerr << " + power of " << d << "-" << exp << "=" << sumVariance << endl;
		
	}
	
	//cerr << "sumVariance:\t" << sumVariance << endl;

	// 3. compute standard deviation
	double stdDev = sumVariance*4/(g_writes.size()*4-1);
	double wear = sqrt(stdDev)/exp;
	cerr << "wear:\t" << wear << endl;
	
	return wear;
}

int main(int argc, char **argv)
{
	
	if( argc < 3)
	{
		cerr << "Need three args!" << endl;
		return 1;
	}
	
	string szBench = argv[1];
	string szPrefix = "stack.out_5";
	string szFlag = argv[2];

	string szOutFile = szPrefix + "--" + szFlag;
	ofstream outf;
	outf.open(szOutFile.c_str());
	
	outf << szBench << "\t";
	char digits[16];
	for(int i = 13; i < 24; ++ i)
	{
		int j = i - 5;
		UINT64 nLen = 1 << j;
		if( szFlag == "2" )
			sprintf(digits, "%d", 23);
		else
			sprintf(digits, "%d", i );
		string szFile = szPrefix + "_" + digits + "_" + szFlag;
	 
		double wear = compute(szFile, nLen);
		outf << wear << "\t";		
	}
	outf << endl;
	outf.close();


}

