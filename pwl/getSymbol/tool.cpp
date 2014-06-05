#include "tool.h"
#include <map>
#include <list>


std::map<uint, std::list<CData *> > g_DataMap;
uint g_nNumOfGlobal;
uint g_nNumOfLocal;
uint g_nNumOfFile;
//using namespace qali;


using namespace std;


string NameMangle::GetGlobal(Dwarf_Die die)
{	
	assert( m_fileDie != 0 );
	string szName = GetDieName(die);
	string szFile = GetDieName(m_fileDie);
	return szName =/* szFile+ ": " +*/ szName;
	return GetDieName(die);
}

string NameMangle::GetName(Dwarf_Die die, bool bExt)
{
	
if( m_funcDie != 0)
	
{	
	
return GetLocal(die);	
	
}
	
if( !bExt)
	
{	
	
return GetFile(die);
	
}
	
return GetGlobal(die);
}


string NameMangle::GetFile(Dwarf_Die die)
{
	
assert( m_fileDie != 0 );
  string szName = GetDieName(die);
  string szFile = GetDieName(m_fileDie);
  
  return szName = /*szFile+ ": " + */szFile + "%%" + szName;
}


string NameMangle::GetLocal(Dwarf_Die die)
{
  assert( m_fileDie != 0 && m_funcDie != 0);
  string szName = GetDieName(die);
   string szFunc = GetDieName(m_funcDie);
  string szFile = GetDieName(m_fileDie);
  //string szFile = GetDieName(m_fileDie);
  
  return szName =  /*szFile+ ": " +*/ szFunc + "##" + szName;
}


string NameMangle::GetDieName(Dwarf_Die die)
{
  Dwarf_Error error = 0;   
  string szName;
  char *name;  
   Dwarf_Half tag = 0;
  int res = dwarf_diename(die,&name,&error);  


  if(res == DW_DLV_ERROR) {
    printf("Error in dwarf_diename\n");
    exit(1);
  }
  if(res == DW_DLV_NO_ENTRY) {
    name = "NoName";
  res = dwarf_tag(die,&tag,&error);  // get the tag number
  if(res != DW_DLV_OK) {
    printf("Error in dwarf_tag\n");
    exit(1);
  }
    
  printf("Waring:\t no name for tag %d\n", tag);
  }
  szName = name;   
  return szName;
}


Dwarf_Attribute CTool::GetAttribute(Dwarf_Die die, int nAttri, string szInfo, bool bNeedAttri=false)
{
  Dwarf_Error error = 0;
  int res;
  Dwarf_Bool bRet;
  res = dwarf_hasattr(die,nAttri, &bRet, &error);  // no attribute
  if( res != DW_DLV_OK || !bRet) 
  {
  if(bNeedAttri)
  {
    printf("Error in no %s attribute \n", szInfo.c_str()); 
    exit(1);
  }
  return NULL;
  }  
  
  Dwarf_Attribute attr;
  res = dwarf_attr(die, nAttri, &attr, &error );  // error for getting location
  if(res != DW_DLV_OK ) {
    printf("Error in getting %s attribute\n", szInfo.c_str());
  exit(1);
  }
  return attr;
}


uint CTool::GetTypeSize(Dwarf_Debug dbg, Dwarf_Die die, string szInfo)
{
  Dwarf_Error error = 0;
  Dwarf_Half tag = 0;
  uint size = 1;
  
  int res = dwarf_tag(die, &tag, &error);
  if( res != DW_DLV_OK )
  {
  printf("Error in tag\n");
  exit(1);
  }  
  
  Dwarf_Attribute attr;
  if( tag == DW_TAG_array_type )
  {
  // get original type of array-type
  Dwarf_Die typeDie = CTool::GetAttrDie(dbg, die, string("type of array-type for ")+szInfo);  
  uint oriSize = GetTypeSize(dbg, typeDie, string("size of type of array type for ") + szInfo );
  
  // the child(ren) of an array-type is a subrange-type
  Dwarf_Die child;
  res = dwarf_child(die,&child,&error);  
  if(res != DW_DLV_OK) 
  {
    printf("Error in getting a child die of array-type\n");
    exit(1);
  }
  res = dwarf_tag(child, &tag, &error);
  if( res != DW_DLV_OK || tag != DW_TAG_subrange_type )
  {
    printf("Error a non-subgrange child of a subrange_type\n");
    exit(1);
  }  
  return oriSize * CTool::GetArrayLength(dbg, child, 1);
  }
  else if( tag == DW_TAG_typedef)
  {
  Dwarf_Die typeDie = CTool::GetAttrDie(dbg, die, string("type of typedef")+szInfo);
  
  return GetTypeSize(dbg, typeDie, string("size of type of typedef") + szInfo);
  }
  else if ( tag == DW_TAG_const_type )
  {
  Dwarf_Die typeDie = CTool::GetAttrDie(dbg, die, string("type of const")+szInfo);
  
  return GetTypeSize(dbg, typeDie, string("size of type of const") + szInfo);
  }
  else
  {
  //assert(tag == DW_TAG_base_type || tag == DW_TAG_structure_type);
  attr = GetAttribute(die, DW_AT_byte_size, string("byte_size for ") + szInfo, true );
  Dwarf_Unsigned size = 0;
  res = dwarf_formudata(attr, &size, &error);
  if( res != DW_DLV_OK) 
  {
    printf("Error in getting size of %s \n", szInfo.c_str());  
    exit(0);
  }
  return size;
  }
}


// getting the (type) attribute as a die
Dwarf_Die CTool::GetAttrDie(Dwarf_Debug dbg, Dwarf_Die die, string szInfo)
{
  Dwarf_Error error = 0;
  Dwarf_Half tag = 0;
  Dwarf_Attribute attr;
  attr = GetAttribute(die, DW_AT_type, string("type for ")+szInfo, true );
  Dwarf_Off offset;
  int res = dwarf_global_formref(attr, &offset, &error);
  if( res != DW_DLV_OK) 
  {
  printf("Error in getting type value of type for %s\n", szInfo.c_str()); 
  exit(1);  
  }  
  
  // the type die of this type attribute
  Dwarf_Die typeDie = 0;
  res = dwarf_offdie(dbg, offset, &typeDie, &error);
  if( res != DW_DLV_OK)
  {
  printf("Error in getting die of type of type-type for %s\n", szInfo.c_str()); 
  exit(1);
  }
  return typeDie;
}


uint CTool::GetArrayLength(Dwarf_Debug dbg, Dwarf_Die die, uint extLen)
{
  Dwarf_Error error = 0;
  Dwarf_Half tag = 0;
  Dwarf_Die sible;
  uint len = 0;
  
  // get bound
  Dwarf_Attribute attr;
  attr = GetAttribute(die, DW_AT_upper_bound, string("upper bound"), true);
  Dwarf_Unsigned bound = 0;
  int res = dwarf_formudata(attr, &bound, &error);
  if( res != DW_DLV_OK )
  {  
  printf("Error in getting upper bounoned value of subrange_type\n");
  exit(1);  
  }
  len = bound + 1;
  
  // test siblings
  while( dwarf_siblingof(dbg,die,&sible,&error) != DW_DLV_NO_ENTRY)
  {
  // exist   
  if(res == DW_DLV_ERROR) 
  {
    printf("Error in getting a child die \n");
    exit(1);
  }
  
  // subrange-type 
  res = dwarf_tag(sible, &tag, &error);
  if( res != DW_DLV_OK || tag != DW_TAG_subrange_type )
  {
    printf("Error a non-subgrange child of a subrange_type\n");
    exit(1);
  }  
  
  // uppper bound 
  attr = GetAttribute(sible, DW_AT_upper_bound, string("upper bound"), true);
  int res = dwarf_formudata(attr, &bound, &error);
  if( res != DW_DLV_OK )
  {  
    printf("Error in getting upper bound value of subrange_type\n");
    exit(1);  
  }
  len *= bound + 1;
  die = sible;
  }
  return len;
}


string CTool::QueryAddress(uint nAddr)
{
  string symbol = "";
  map<uint, list<CData *> >::iterator u2d_p = g_DataMap.begin(), u2d_e = g_DataMap.end(), next_p;
  
  if( nAddr < u2d_p->first )
  return symbol;
  
  bool flag = false;
  for(; u2d_p != u2d_e; ++ u2d_p)
  {
  next_p = u2d_p;
  ++ next_p;
  if( nAddr == u2d_p->first)
  {
    symbol = u2d_p->second.front()->m_szName;
    flag = true;
    break;
  }
  else if( nAddr > u2d_p->first)
  {    
    CData *pData = u2d_p->second.front();
    if( nAddr <= pData->m_nAddr + pData->m_nSize - 1)
    {
     symbol = u2d_p->second.front()->m_szName;
     flag = true;
     break;
    }    
  }
  else
  {
    flag = false;
    break;
  }
  }
  return symbol;
}


int CTool::DumpData(std::ostream &os)
{
  uint nTotal = g_nNumOfGlobal + g_nNumOfFile + g_nNumOfLocal;
  os << "Number of global:\t" << g_nNumOfGlobal << "(" << g_nNumOfGlobal*100/nTotal<< "%)\n";
  os << "Number of file-static:\t" << g_nNumOfFile << "(" << g_nNumOfFile*100/nTotal<< "%)\n";
  os << "Number of Local:\t" << g_nNumOfLocal << "(" << g_nNumOfLocal*100/nTotal<< "%)\n";
  
  std::map<uint, list<CData *> >::iterator u2d_p = g_DataMap.begin(), u2d_e = g_DataMap.end();
  for(; u2d_p != u2d_e; ++ u2d_p)
  {
  std::list<CData *>::iterator d_p = u2d_p->second.begin(), d_e = u2d_p->second.end();
  for( ; d_p != d_e; ++ d_p)
  {
    CData *pData = *d_p;
    uint nlen = pData->m_szFile.size() + pData->m_szName.size() + 3;
    os << pData->m_szFile << ": " << pData->m_szName << ":" << DupChar(6-nlen/8, '\t');
    if(pData->m_nAddr == CTool::NON_STATIC_ADDRESS)
     os << hex << "0x       " << "------0x       (" << dec << pData->m_nSize << ")" << endl;
    else if (pData->m_nAddr == CTool::NON_ADDRESS)
     os << hex << "0x#######" << "------0x#######(" << dec << pData->m_nSize << ")" << endl;
    else
     os << hex << "0x" << pData->m_nAddr << "------0x" << pData->m_nAddr + pData->m_nSize - 1 <<  dec << "(" << pData->m_nSize << ")" << endl;
  }
  }
  return 0;
}


string CTool::DupChar(uint num, char c)
{
  string str = "";
  for(int i = 0; i < num ; ++i)
  str += c;
  return str;
}
