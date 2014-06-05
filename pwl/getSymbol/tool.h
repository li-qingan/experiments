#ifndef _TOOL_H
#define _TOOL_H

#include <sys/types.h> /* For open() */
#include <sys/stat.h>  /* For open() */
#include <fcntl.h>     /* For open() */
#include <stdlib.h>     /* For exit() */
#include <unistd.h>     /* For close() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "dwarf.h"
#include "libdwarf.h"

#include <ostream>
#include <string>
#include <assert.h>


//namespace qali/
//{
typedef unsigned int uint;

class NameMangle
{
	public:
	
	NameMangle() {ResetFile(); ResetFunc();};
	
	void SetFile(Dwarf_Die die ) { m_fileDie = die;}
	void SetFunc(Dwarf_Die die) { m_funcDie = die;}
	void ResetFile() { m_fileDie = 0;};
	void ResetFunc() { m_funcDie = 0;};
	std::string GetGlobal(Dwarf_Die);
	std::string GetFile(Dwarf_Die);
	std::string GetLocal(Dwarf_Die);
	std::string GetName(Dwarf_Die, bool bExternal = true);

public:
	
	Dwarf_Die	m_fileDie;
	Dwarf_Die	m_funcDie;
	

	std::string GetDieName(Dwarf_Die die);
	
};

class CData
{
	public:
	CData(){};
	CData(std::string &szName, uint nAddr, uint nSize = 0, uint nLevel=1) 
	{
		m_szName = szName;
		m_nAddr = nAddr;
		m_nSize = nSize;
		m_nLevel = nLevel;
	};
	
	void SetFile(std::string szFile) { m_szFile = szFile; }
	
	public:
	std::string m_szName;
	uint m_nAddr;
	uint m_nSize;
	uint m_nLevel;
	
	std::string m_szFile;
};

class CTool
{
	public:
	enum ADDR
	{
		NON_ADDRESS = -1,
		NON_STATIC_ADDRESS =0
	};
	public:
	
	static Dwarf_Attribute GetAttribute(Dwarf_Die die, int nAttribute, std::string szInfo, bool bNeedAttri);
	static uint GetTypeSize(Dwarf_Debug dbg, Dwarf_Die die, std::string szInfo);
	static uint GetArrayLength(Dwarf_Debug, Dwarf_Die die, uint extLen);
	
	static Dwarf_Die GetAttrDie(Dwarf_Debug dbg, Dwarf_Die die, std::string szInfo);
	
	// Query function: from address to symbol
	static std::string QueryAddress(uint nAddr);
	
	static int DumpData(std::ostream &os);
	static std::string DupChar(uint num, char c);
};
#endif
