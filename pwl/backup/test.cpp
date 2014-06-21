#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

//int main(int argc, char **argv)
//{
//	if( argc < 2 )
//	{
//		cerr << "Lack args!" << endl;
//		return 1;
//	}
//	string szFile = argv[1];
//	
//	ifstream inf;
//	inf.open(szFile.c_str());
//	
//	szFile += ".dec";
//	ofstream outf;
//	outf.open(szFile.c_str());
//	string szLine;
//	while(getline(inf, szLine) )
//	{
//		if( szLine.size() < 3 )
//			continue;
//		stringstream ss(szLine);
//		long long nWrites, nFuncs;
//		ss >> hex >> nWrites >> nWrites >> nFuncs;
//		outf << dec << nWrites << "\t" << nFuncs << endl;
//	}
//	outf.close();
//	inf.close();
//	return 0;
//}

int main(int argc, char **argv)
{
	if( argc < 2 )
	{
		cerr << "Lack args!" << endl;
		return 1;
	}
	string szDir = argv[1];
	
	string szCmd = "cd ";
	szCmd += szDir;
	//system(szCmd.c_str());	
	
	string szOutf = "stack.out";

	ifstream inf;
		
	
	ofstream outf;
	outf.open(szOutf.c_str());

	string szLine;

	string szInf = "stack.out_3--0";
	inf.open(szInf.c_str());
	while(getline(inf, szLine) )
	{
		outf << szLine;
	}	
	inf.close();
	szInf = "stack.out_3--1";
	inf.open(szInf.c_str());
	while(getline(inf, szLine) )
	{
		outf << szLine;
	}	
	inf.close();
	szInf = "stack.out_3--2";
	inf.open(szInf.c_str());
	while(getline(inf, szLine) )
	{
		outf << szLine;
	}	
	inf.close();

	outf.close();


	szCmd = "cd ..";	
	//system(szCmd.c_str());
	return 0;
}