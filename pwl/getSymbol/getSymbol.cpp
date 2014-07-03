
/*  getSymbol.cpp
    This file is used to obtain the global data symbol infos from an object file with DWARF debug infos

    The --names 
    option adds some extra printing.

    To use, try
        make
        ./getSymbol getSymbol
*/
#include <sys/types.h> /* For open() */
#include <sys/stat.h>  /* For open() */
#include <fcntl.h>     /* For open() */
#include <stdlib.h>     /* For exit() */
#include <unistd.h>     /* For close() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "dwarf.h"
#include "libdwarf.h"
#include "tool.h"

using namespace std;

#define bool int
#define true 1
#define false 0

struct srcfilesdata {
    char ** srcfiles;
    Dwarf_Signed srcfilescount;
    int srcfilesres;
};

FILE *g_outFile;
int g_counter = 0;
const char *g_curFunc;

int Error(int res, const char *szInfo, bool bWarn);
int printIndent(char c, int nTab);
bool IsGoodTag( Dwarf_Half tag);
bool VerifiedUserFunc( Dwarf_Die die);
unsigned int GetTypeSize(Dwarf_Debug dbg, Dwarf_Die die, string szInfo);


Dwarf_Unsigned BlockValue(Dwarf_Block *block, Dwarf_Half form);
int dump(const char *varName, int addr, bool bGlobal, unsigned int nSize );


static void read_cu_list(Dwarf_Debug dbg);
static void print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,int level,
   struct srcfilesdata *sf);
static void DepthFirst(Dwarf_Debug dbg, Dwarf_Die in_die,int in_level,
   struct srcfilesdata *sf);
static void resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf);

static int namesoptionon = 0;

static FILE *g_userFunc;

int 
main(int argc, char **argv)
{

    Dwarf_Debug dbg = 0;
    int fd = -1;
    const char *filepath = "<stdin>";
    int res = DW_DLV_ERROR;
    Dwarf_Error error;
    Dwarf_Handler errhand = 0;
    Dwarf_Ptr errarg = 0;
	char szOutFile[128];

    if(argc < 2) {
        fd = 0; /* stdin */
    } else {
        int i = 0;
        for(i = 1; i < (argc-1) ; ++i) {
            if(strcmp(argv[i],"--names") == 0) {
                namesoptionon=1;
            } else {
                printf("Unknown argument \"%s\" ignored\n",argv[i]);
            }
        }
        filepath = argv[i];
        fd = open(filepath,O_RDONLY);
    }
    if(argc > 2) {
    }
    if(fd < 0) {
        printf("Failure attempting to open \"%s\"\n",filepath);
    }

	g_userFunc = fopen("userfunc", "w+");
	// initalize 
    res = dwarf_init(fd,DW_DLC_READ,errhand,errarg, &dbg,&error);
    if(res != DW_DLV_OK) {
        printf("Giving up, cannot do DWARF processing\n");
        exit(1);
    }

	
	// main work
	strncpy(szOutFile, filepath, 127);
	strcat(szOutFile, ".symbol");
	g_outFile = fopen(szOutFile, "w+");
	fprintf(g_outFile, "#Demangled name\t\t#Address\t#Size\n\n");
    read_cu_list(dbg);
	fclose(g_outFile);
	fclose(g_userFunc);

	//finalize
    res = dwarf_finish(dbg,&error);
    if(res != DW_DLV_OK) {
        printf("dwarf_finish failed!\n");
    }
    close(fd);
    return 0;
}

static void 
read_cu_list(Dwarf_Debug dbg)
{
	Dwarf_Bool is_info = 1;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half address_size = 0;
	Dwarf_Half offset_size = 0;
	Dwarf_Half extension_size = 0;
	Dwarf_Sig8 signature ;
	Dwarf_Unsigned typeoffset = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Error error;
    int cu_number = 0;

    for(;;++cu_number) {
        struct srcfilesdata sf;
        sf.srcfilesres = DW_DLV_ERROR;
        sf.srcfiles = 0;
        sf.srcfilescount = 0;
        Dwarf_Die no_die = 0;
        Dwarf_Die cu_die = 0;
        int res = DW_DLV_ERROR;
		bool noEntry = false;
		// get the info about the next compilation unit header in .debug_info section
        res = dwarf_next_cu_header_c(dbg, is_info, &cu_header_length, &version_stamp, 
			&abbrev_offset, &address_size, &offset_size,
			&extension_size, &signature,   &typeoffset,
		    &next_cu_header, &error);
        noEntry = Error(res, "SEARCHING CU die", true);
		if( noEntry )
			return;

        /* The CU will have a single sibling, a cu_die. */
		// with no_die==NULL as input, this function will get the first die of the cu
        res = dwarf_siblingof_b(dbg, no_die, is_info, &cu_die,&error);
		res = Error( res, "Get the first die within CU", false);
		
		// get the name for CU from the first die of CU
		char *dieName = NULL;
		dwarf_diename(cu_die,&dieName,&error);
		Error(res, "SEARCHING CU die name", true);
		if( dieName )
			printf("===========CU die name:\t%s==============\n", dieName);		

        DepthFirst(dbg,cu_die,0,&sf);
        dwarf_dealloc(dbg,cu_die,DW_DLA_DIE);
        resetsrcfiles(dbg,&sf);
    }
}

// visit into cu in a Depth-First fashion
static void
DepthFirst(Dwarf_Debug dbg, Dwarf_Die in_die,int in_level,
   struct srcfilesdata *sf)
{
    int res = DW_DLV_ERROR;
    Dwarf_Die cur_die=in_die;
    Dwarf_Die child = 0;
    Dwarf_Error error;
	Dwarf_Half tag;
   
	for(;;) {
		Dwarf_Die sib_die = 0;

		// focus on dies of interested tags
		res = dwarf_tag(cur_die, &tag,&error);
		Error(res, "SEARCHING die tag", false);
		if( tag == DW_TAG_compile_unit )
		{
			//print_die_data(dbg,cur_die,in_level,sf);
			res = dwarf_child(cur_die,&child,&error);
			Error(res, "SEARCHING DFS of CU", true);  
		
			// DFS
		    if(res == DW_DLV_OK) {
		        DepthFirst(dbg,child,in_level+1,sf);
		    }
		}
		else if( tag == DW_TAG_subprogram )
		{
			if( VerifiedUserFunc( cur_die ) )
			{
				print_die_data(dbg,cur_die,in_level,sf);
				res = dwarf_child(cur_die,&child,&error);
				Error(res, "SEARCHING DFS of CU", true);  
		
				// DFS
				if(res == DW_DLV_OK) {
				    DepthFirst(dbg,child,in_level+1,sf);
				}
			}	
		}
		else if( tag == DW_TAG_variable)
		{
			print_die_data(dbg,cur_die,in_level,sf);
		}
        
		// WFS
        res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
		Error(res, "SEARCHING WFS of CU", true);
        if(res == DW_DLV_NO_ENTRY) {
            /* Done at this level. */
            break;
        }

        /* res == DW_DLV_OK */
        if(cur_die != in_die) {
            dwarf_dealloc(dbg,cur_die,DW_DLA_DIE);
        }
        cur_die = sib_die;
        //print_die_data(dbg,cur_die,in_level,sf);
    }
    return;
}

static void
resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf)
{
    Dwarf_Signed sri = 0;
    for (sri = 0; sri < sf->srcfilescount; ++sri) {
        dwarf_dealloc(dbg, sf->srcfiles[sri], DW_DLA_STRING);
    }
    dwarf_dealloc(dbg, sf->srcfiles, DW_DLA_LIST);
    sf->srcfilesres = DW_DLV_ERROR;
    sf->srcfiles = 0;
    sf->srcfilescount = 0;
}

static void
print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,int level,
    struct srcfilesdata *sf)
{
    char *name = 0;
    Dwarf_Error error = 0;
    Dwarf_Half tag = 0;
    const char *tagname = 0;
    int localname = 0;
	bool bDeclared;
	Dwarf_Bool bAttr;
	Dwarf_Attribute attr;

    int res = dwarf_tag(print_me,&tag,&error);
	Error(res, "SEARCHING die tag", false);
	if( tag == DW_TAG_lexical_block || tag == DW_TAG_compile_unit)
		return;	

    res = dwarf_get_TAG_name(tag,&tagname);
	Error(res, "SEARCHIGN die tag name", false);	

	res = dwarf_diename(print_me,&name,&error);
    Error(res, "SEARCH die name", true); 
    if(res == DW_DLV_NO_ENTRY) {
        name = "<no DW_AT_name attr>";
        localname = 1;
	return;
    }
	if( tag == DW_TAG_subprogram )
		g_curFunc = name;

	// skip decalared but not defined objects
	res = dwarf_hasattr( print_me, DW_AT_declaration, &bAttr, &error);
	Error(res, "SEARCHING declaration attribute1", true);	
	if( res == DW_DLV_OK && bAttr == true)
	{
		res = dwarf_attr(print_me, DW_AT_declaration, &attr, &error);
		Error(res, "SEARCHING declaration attribute2", false);
		res = dwarf_formflag( attr, &bDeclared, &error );
		Error(res, "SEARCHIGN declaration attribute3", false);
		if( bDeclared )
			return;
	}

		

	{
		//printf("\n");
		//printIndent('-', level);
		//printf("<%d> tag: %d %s  name: \"%s\"",level,tag,tagname,name);
		if( tag == DW_TAG_subprogram )
			fprintf(g_userFunc, "%d\t%s\n", ++g_counter, name);	
		if( tag == DW_TAG_variable )
		{
			// If external or local variable	
			bool bExternal = false;					
			
			res = dwarf_hasattr( print_me, DW_AT_external, &bAttr, &error);
			Error(res, "SEARCHING external attribute1", true);
			
			if( res == DW_DLV_OK && bAttr == true)
			{
				res = dwarf_attr(print_me, DW_AT_external, &attr, &error);
				Error(res, "SEARCHING external attribute2", false);
				res = dwarf_formflag(attr, &bExternal, &error);
				Error(res, "SEARCHING external attribute3", false);
			}
			
			if( !bExternal )
				return ; // skip non-global variables
			printIndent('-', level);
			printf("<%d> tag: %d %s  name: \"%s\"",level,tag,tagname,name);
			//else
			{	
				// 1. obtain the size
				Dwarf_Off offset;
				res = dwarf_attr(print_me, DW_AT_type, &attr, &error);
				Error(res, "SEARCHING type attribute", true);
				res = dwarf_global_formref(attr, &offset, &error);
				Error(res, "SEARCHING value of type attribute", true);	
				Dwarf_Die typeDie = 0;
				res = dwarf_offdie(dbg, offset, &typeDie, &error);
				Error(res, "SEARCHING die of type value", true);	
				unsigned int nSize = CTool::GetTypeSize(dbg, typeDie, "");
				printf(":\t[%d]@", nSize);

				// 2. obtain the location
				res = dwarf_hasattr( print_me, DW_AT_location, &bAttr, &error);
				Error(res, "SEARCHING location attribute1", true);
				if( res == DW_DLV_OK && bAttr == true)
				{
					Dwarf_Signed nLenth;
					Dwarf_Locdesc **llbuf;
					Dwarf_Signed offset;					
	
					res = dwarf_attr(print_me, DW_AT_location, &attr, &error);
					Error(res, "SEARCHING location attribute2", true);
				
					// There is a unified way to get the location of symbols
					res = dwarf_loclist_n(attr, &llbuf, &nLenth, &error);
					Error(res, "PARSER location list", false);
			
					assert(nLenth == 1);
					Dwarf_Locdesc *locdesc = llbuf[0];
					assert(locdesc->ld_cents == 1 );
					Dwarf_Loc *loc = locdesc->ld_s;
					if( loc->lr_atom == DW_OP_fbreg )
					{
						//offset = _dwarf_decode_s_leb128( (unsigned char *)(&loc->lr_number), NULL );
						printIndent(' ', level);
						printf(":\tesb[%d] @@location\n", (int)offset );
						dump(name, offset, false, nSize);
					}
					else if( loc->lr_atom == DW_OP_breg5 )
					{
						//offset = _dwarf_decode_s_leb128( (unsigned char *)(&loc->lr_number), NULL );
						printIndent(' ', level);
						printf(":\tebp[%d] @@location\n", (int)offset );
						dump(name, offset, false, nSize);
					}
					else if( loc->lr_atom == DW_OP_addr )
					{
						offset = loc->lr_number;
						printIndent(' ', level);
						printf(":\t0x%x @@location\n", (int)offset );
						dump(name, offset, true, nSize);
					}					
					else
						printf("\n****%d is not DW_OP_fbreg\n", loc->lr_atom);				
					
				}
				else
					printf("\n****No location attribute\n");				
			}
			
		}
		else 
			printf(" ");
	}

    if(!localname) {
        dwarf_dealloc(dbg,name,DW_DLA_STRING);
    }
}

int Error(int res, const char *szInfo, bool bWarn)
{
	if(res == DW_DLV_ERROR) {
            printf("Error for %s \n", szInfo);
            exit(1);
        }
    if(res == DW_DLV_NO_ENTRY) {
        if(!bWarn)
		{
        	printf("no entry for %s \n", szInfo);
        	exit(1);
		}
		return 1;
    }
	return 0;
}

int dump(const char *varName, int addr, bool bGlobal, unsigned int nSize )
{
	if( bGlobal )
		fprintf(g_outFile, "%s\t\t%x\t%x\n", varName, addr, nSize );
	else
		fprintf(g_outFile, "%s::%s\t\t%x\t%x\n", g_curFunc, varName, addr, nSize );
	return 0;
}

int printIndent(char c, int nTab)
{
	int i = 0, j;
	for(; i < nTab-1; ++ i)
		for(j =0 ; j < 3; ++ j)
			printf("%c", c);
	return 0;
}

bool IsGoodTag(Dwarf_Half tag)
{
	//printf("%d\n", tag);
//////////////////////
	if( tag == DW_TAG_subprogram )
		return true;
	return false;
///////////////////////
	if( tag == DW_TAG_base_type ||
		tag == DW_TAG_pointer_type || 
		tag == DW_TAG_const_type ||
		tag == DW_TAG_array_type ||
		tag == DW_TAG_structure_type || 
		tag == DW_TAG_typedef ||
		//tag == DW_TAG_lexical_block ||
		tag == DW_TAG_formal_parameter)
		return false;
	return true;
}


bool VerifiedUserFunc( Dwarf_Die cur_die)
{
	// declared, artificial, inlined
	bool bDeclared;
	Dwarf_Error error;
	Dwarf_Bool bAttr;
	Dwarf_Attribute attr;
	int res;
	// skip decalared but not defined objects
	res = dwarf_hasattr( cur_die, DW_AT_declaration, &bAttr, &error);
	Error(res, "SEARCHING declaration attribute1", true);	
	if( res == DW_DLV_OK && bAttr == true) // has declareation attribute
	{
		res = dwarf_attr(cur_die, DW_AT_declaration, &attr, &error);
		Error(res, "SEARCHING declaration attribute2", false);
		res = dwarf_formflag( attr, &bDeclared, &error );
		Error(res, "SEARCHIGN declaration attribute3", false);
		if( bDeclared )
			return false;
	}
	res = dwarf_hasattr( cur_die, DW_AT_artificial, &bAttr, &error );  // has artificial attribute
	Error(res, "SEARCHING artificial attribute1", true);
	if( res == DW_DLV_OK && bAttr )
	{
		res = dwarf_attr(cur_die, DW_AT_artificial, &attr, &error);
		Error(res, "SEARCHING DW_AT_artificial attribute2", false);
		res = dwarf_formflag( attr, &bDeclared, &error );
		Error(res, "SEARCHIGN DW_AT_artificial attribute3", false);
		if( bDeclared )
			return false;
	}
	return true;
}


