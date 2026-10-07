/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 *                  About MACH-O Symbols 
 *
 * Import load commands point to import symbol sections and
 * symbols_commands point to defined symbol sections. 
 *
 * DEFINED SYMBOLS are either local, export or absolute symbols and
 * should always have one and only one of the flags
 * (SI_EXPORT_F,SI_LOCAL_F, SI_ABSOLUTE_F ) set. They should not have the
 * (SI_IMPORT_F) flag set. If the flags (SI_EXPORT_F or SI_LOCAL_F ) are
 * set then the def_val field of the si_value union is used.  If the
 * SI_LITERAL_F flag is set then the lit_val field of the si_value union
 * is used.  
 *
 * IMPORT SYMBOLS should always have the (SI_IMPORT_F ) flag
 * set. These symbols use the imp_val field in the si_value union. This
 * field is set to the symbol index. For example the tenth symbol in the
 * list would have the imp_val field set to 10; I believe the flags
 * si_data_f and si_code_def are used by import symbols to show what kind
 * of info they are expected to be importing.
 *
 * The flag SI_FORWARD_F is not used in the first revision of the language
 * tools.
 */


#include <mach_o_format.h>
#include <mach_o_header.h>
#include <mach_o_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <ldfcn.h>
#include <sys/types.h>
#include <unistd.h>
#include "dump_defs.h"
#include "rose_namegrp.h"



#define ldc_header_NULL ((ldc_header_t *)0)
#define LDC_EVERYTHING (LDC_UNDEFINED)
#define WORD_LENGTH 4
#define LINE_LENGTH 80
#define INDEX_LENGTH 5
#define XLATE_LCID(lcd) (&file_contents[cmd_map->lcm_map[lcd]])
#define IS_INVALID_LCM_ENTRY(lcd) (cmd_map->lcm_map[lcd] == LCM_INVALID_ENTRY)
#define IS_INVALID_LCID(lcd) ((lcd == MO_INVALID_PKG_INDEX) || (lcd >= cmd_map->lcm_nentries))

/*
 * Flags that act as an interface between the command line parser and
 * the dump routines.  Most of these flags are used only in the dump_rose
 * routine to select the  parts of the file that will be dumped.  Exceptions
 * are flags that affect display formatting or contents, and so apply to all
 * parts of the file:
 *   Print_header
 *   Symbolic
 */

extern char	*Current_file, *Member_name, *Nname, *Zname;

extern int	Ar_member,
		Head_sect,
		Obj_sect,
		Print_header,
		Reloc,
		Str_table,
		Sym_table,
		Symbolic,
                Prefix_names,
                Ac_component_names,
		Entry,
		Info_cmd,
		Load_map,
                All_cmds,
		File_header,
                Defined_syms,
		Export_syms,
		Import_syms,
                Pkg_names,
		Debug_syms;



char *file_contents; /* file_contents is a pointer to an array of chars */
load_cmd_map_command_t *cmd_map;
static mo_header_t header;		/* Visible to all functions in this module */
static FILE *fp;			/* Shared among all routines accessing mach-o object */

/* Information about each section.  An array of structures is allocated and
 * the entries corresponding to the meta data sections are filled when those 
 * sections are loaded.
 */

typedef struct sect_info_t {
	unsigned int 	section_len;
	char		*section_ptr;
} sect_info_t;

sect_info_t	*sip;

/*
 * Internal error flag.
 */
static int	Error_flag;

/*
 * Internal functions
 */

/*
 * Make sure the fields of a load command that point to a 
 * section look reasonable.
 */
static void
check_section_info( ldc_header_t *cmd )
{

if (cmd->ldci_section_len > 0) {
  if (cmd->ldci_section_off < header.moh_sizeofcmds + sizeof(mo_header_t))
      error(" WARNING: ldci_section_off looks too small.\n");
}
/* callers should make sure that they do something reasonable when 
 * section_len == 0.
 */

}


/* *********************remove code below ********************** 
static
assert_valid_cmd( cmd, index )
  ldc_header_t *cmd;
  int          index;
{
 switch ( cmd->ldci_cmd_type ) {
  case LDC_UNDEFINED:
  case LDC_CMD_MAP:
  case LDC_INTERPRETER:
  case LDC_STRINGS:
  case LDC_REGION:
  case LDC_RELOC:
  case LDC_PACKAGE:
  case LDC_SYMBOLS:
  case LDC_ENTRY:
  case LDC_FUNC_TABLE:
  case LDC_GEN_INFO:
    break;
  default:
    error("Warning: Unknown load command type ");
    fprintf(stderr, "command index = %d ,  command type = %d\n", index,
	    cmd->ldci_cmd_type );
    fprintf(stderr, "cmds type decimal:  %d\n", 
  }
}
 *********************** remove code above **********************/

/*
 * This routine walks the load commands, storing the length of every section
 * and loading the meta data sections.  (Relocation sections are loaded
 * only if asked for; symbol sections may also be large, but it is more
 * awkward to know whether or not they will be needed.
 */

static
load_sections(cmd_map)
  load_cmd_map_command_t *cmd_map;
{
ldc_header_t *lcmh;
int i;
mo_long_t reloc_type;

#ifdef DEBUG
printf("\n loading sections");
#endif /* DEBUG */

sip = (sect_info_t *)malloc( (sizeof (sect_info_t )) * (cmd_map->lcm_nentries));
if (Reloc) reloc_type = LDC_REGION;	/* don't skip over reloc */
else reloc_type = LDC_RELOC;

for (i=0; i < cmd_map->lcm_nentries; i++ ) {
  if (cmd_map->lcm_map[i] == LCM_INVALID_ENTRY) {
	  sip[i].section_len = 0;
	  sip[i].section_ptr = (char *)NULL;
  } else {
  lcmh = (ldc_header_t *) XLATE_LCID(i);
  sip[i].section_len = (int)lcmh->ldci_section_len;

  if ((sip[i].section_len > 0) && (lcmh-> ldci_cmd_type != LDC_REGION)
      && (lcmh-> ldci_cmd_type != reloc_type)) {
    fseek(fp, lcmh->ldci_section_off, SEEK_SET);
    sip[i].section_ptr = malloc(sip[i].section_len);
    fread( sip[i].section_ptr, sip[i].section_len, 1 ,fp);
  } else {
    sip[i].section_ptr = (char *)NULL;
  }

}
}

#ifdef DEBUG
printf("\n Done loading sections\n ");
#endif /* DEBUGi */

} /* end of load_sections */

/* 
 ********obsolete***************
 * This routine walks the load commands and stores lenth of every section.
 * It sets up this information for future consistency checking.
 */
int * section_len;

static
load_section_lengths(cmd_map )
  load_cmd_map_command_t *cmd_map;
{
ldc_header_t *lcmh;
int i;

#ifdef DEBUG
printf("\n loading section lengths.");
#endif /* DEBUG */

section_len = malloc( sizeof (int ) * cmd_map->lcm_nentries);

for (i=0; i < cmd_map->lcm_nentries; i++ ) {
  lcmh = ( ldc_header_t *) (&file_contents[cmd_map->lcm_map[i]] );
  section_len[i] = lcmh->ldci_section_len;
}

#ifdef DEBUG
printf("\n Done loading section lengths\n ");
#endif /* DEBUGi */

} /* load section lengths */


/* 
 ***************obsolete****************
 * This routine walks the load commands look for string table commands.
 * It then loads those string tables.
 */
char **st;

static
load_strings(cmd_map )
  load_cmd_map_command_t *cmd_map;
{
ldc_header_t *lcmh;
int i;

st = malloc( sizeof(char *) * (cmd_map->lcm_nentries+1));

for (i=0; i < cmd_map->lcm_nentries; i++ ) {
  lcmh = ( ldc_header_t *) (&file_contents[cmd_map->lcm_map[i]] );
  if ( lcmh->ldci_cmd_type == LDC_STRINGS ) {
    fseek(fp, lcmh->ldci_section_off, SEEK_SET);
    st[i] = malloc(lcmh->ldci_section_len);
    fread( st[i], lcmh->ldci_section_len, 1 ,fp);
  } else {
    st[i] = 0;
  }
}

} /* load_strings */

static char *
get_string( mo_addr_t str)
{
  char * string_section;

  if (str.adr_lcid > header.moh_n_load_cmds ) {
    error("Warning: Bad Index.");
    return("");
  }

  if ((sip[str.adr_lcid].section_ptr == NULL)) {
    error("Warning: No String section found at this index.");
    return("");
  }

  return ( (char *)&sip[ str.adr_lcid].section_ptr[ str.adr_sctoff ] );
}


static void
print_string( mo_addr_t str)
{
printf( "\"%s\"", get_string( str));
}

static void
print_string_padded( mo_lcid_t section, mo_offset_t offset ) {
  mo_addr_t	str;

  str.adr_lcid = section;
  str.adr_sctoff = offset;

  printf( "\"%-14s\"", get_string(str));

} /* print_string_padded */


/*
 * Given an mo_index_t this routine will print the
 *  symbol name associated with it. This requires
 *  that the load command map and string tables have
 *  been loaded.
 */
static void
print_reloc_symbol( mo_index_t sym)
{
ldc_header_t *cmd;

cmd = ( ldc_header_t *) (XLATE_LCID(sym.adx_lcid));

if (cmd->ldci_cmd_type == LDC_SYMBOLS) {
  symbol_info_t *sym_sect;
  symbol_info_t *sym_info;
  symbols_command_t *sym_cmd;
  package_command_t *pkg_cmd;
  mo_addr_t str;
  char *string_tmp;
  
  sym_cmd = ( symbols_command_t *) cmd;
  
  sym_sect = (symbol_info_t *)sip[sym.adx_lcid].section_ptr;
                                        /* get addr of symbol section */
  sym_info = &sym_sect[ sym.adx_index ];
  if (sym_info->si_package_index != MO_INVALID_PKG_INDEX) {
    pkg_cmd = (package_command_t *) (XLATE_LCID(sym_cmd->symc_pkg_list));
    str.adr_lcid   = pkg_cmd->pkgc_strings_id;
    str.adr_sctoff = pkg_cmd->pkgc_pkg_list[ sym_info->si_package_index ].pe_pkg_name;
    string_tmp = (char *)get_string(str);
    if ((strlen( string_tmp )) > 0)
    printf( "%s:", string_tmp );
}
  str.adr_lcid   = sym_cmd->symc_strings_section;
  str.adr_sctoff = sym_info->si_symbol_name;
  string_tmp = (char *)get_string(str);
  if ((strlen( string_tmp )) > 0)
    printf( "%s   ", string_tmp );
  else printf( "<no name>   ");

  rose_flags_print((int)sym_info->si_flags, SI_FLAGS_SHORT_NAMES);
} else {
  error(" Warning: printing relocation symbol, symbol value does not refer to a symbol section.");
}

} /* end of print_reloc_symbol */


static void
print_reloc_loc (mo_addr_t loc)
{
ldc_header_t  *cmd;

printf("    %d,%#x", loc.adr_lcid, loc.adr_sctoff);

cmd = ( ldc_header_t *) (XLATE_LCID( loc.adr_lcid ));
if (cmd->ldci_cmd_type != LDC_REGION) {
	error("Warning: printing relocation target, address does not refer to a region.");
}

} /* end of print_reloc_loc */


static void
print_cmd_type ( mo_lcid_t lcid)
{
  ldc_header_t  *cmd;
  region_command_t  *regcmd;
  package_command_t  *pkgcmd;
  symbols_command_t  *symcmd;
  func_table_command_t  *fctcmd;

  if ((lcid > cmd_map->lcm_nentries)
  || (cmd_map->lcm_map[lcid] == LCM_INVALID_ENTRY)) {
    printf( "INVALID");
    return;
}
  cmd = ( ldc_header_t *) XLATE_LCID( lcid);
  switch ( cmd->ldci_cmd_type ) {
    case LDC_REGION:
      regcmd = (region_command_t *) cmd;
      printf( "%s ", get_string( regcmd->regc_region_name ));
      rose_type_print((int)regcmd->regc_usage_type, REGC_USAGE_TYPE_STRIPPED_NAMES);
      break;

    case LDC_PACKAGE:
      pkgcmd = (package_command_t *) cmd;
      rose_flags_print( (long)pkgcmd->pkgc_flags, PKGC_FLAGS_STRIPPED_NAMES);
      break;

    case LDC_SYMBOLS:
      symcmd = (symbols_command_t *) cmd;
      rose_type_print( (int)symcmd->symc_kind, SYMC_KIND_STRIPPED_NAMES);
      break;

    case LDC_FUNC_TABLE:
      fctcmd = (func_table_command_t *) cmd;
      rose_type_print((int)fctcmd->fntc_type, FNTC_TYPE_STRIPPED_NAMES);
      break;

}
    printf(" ");
    rose_type_print( (int)cmd->ldci_cmd_type, LDC_CMD_TYPE_STRIPPED_NAMES);

}	/* end print_cmd_type */


/* ****************obsolete******************/

static
ldc_header_t *
next_ldc( ldc_header_t *cmd, mo_long_t id)
{
ldc_header_t *end_cmd;

/*
 * If initial call, use the first command, else use the command after the
 * passed command
 */
if (cmd == ldc_header_NULL)
  cmd = (ldc_header_t *)
        (file_contents + header.moh_first_cmd_off);
else
  cmd = (ldc_header_t *) ((int) cmd + cmd->ldci_cmd_size);

/*
 * Find the end of the load commands
 */
end_cmd = (ldc_header_t *)
          (file_contents + header.moh_first_cmd_off + header.moh_sizeofcmds);


while (cmd != end_cmd) {
  if ((id == cmd->ldci_cmd_type) || (id == LDC_EVERYTHING))
    return cmd;
  cmd = (ldc_header_t *) ((int) cmd + cmd->ldci_cmd_size);
  }
return ldc_header_NULL;
} /* next_ldc */


static mo_lcid_t
next_ldcid( mo_lcid_t lcid, mo_long_t id)
{
mo_lcid_t    i;
ldc_header_t  *cmd;

/*
 * This routine looks for the next load command of the specified type.
 * If initial call, use the first command, else use the command after the
 * passed command
 */
  if (lcid == MO_INVALID_LCID)
    lcid = 0;
  else
    lcid++;

  if (lcid >= cmd_map->lcm_nentries) return MO_INVALID_LCID;

  for (i = lcid; i < cmd_map->lcm_nentries; i++) {
    if (IS_INVALID_LCM_ENTRY(i)) continue;
    cmd = (ldc_header_t *) XLATE_LCID(i);
    if ((id == cmd->ldci_cmd_type) || (id == LDC_EVERYTHING)) 
      return i;
}
  return MO_INVALID_LCID;
}		/* next ldcid */


static void
dump_hdr_UNDEFINED( mo_lcid_t lcid, ldc_header_t *cmd )
{

} /* dump_hdr_UNDEFIND */

static void
dump_LDC_UNDEFINED(ldc_header_t *cmd )
{

} /* dump_LDC_UNDEFINED */

static void
dump_hdr_UNRECOGNIZED( mo_lcid_t lcid, ldc_header_t *cmd)
{
  int n_on_line;
  int i,j;
  unsigned long *long_word;

#define NBYTES_IN_LONG	4

if (cmd->ldci_cmd_size > sizeof(ldc_header_t)) {	/* dump ldc in hex */
  n_on_line = 1;
  for ((i = cmd_map->lcm_map[lcid] + sizeof(ldc_header_t)), 
       j = sizeof(ldc_header_t); j < cmd->ldci_cmd_size; 
       i = i + NBYTES_IN_LONG, j = j + NBYTES_IN_LONG) {

    long_word = (unsigned long *)&file_contents[i];
    printf( "%#-10x  ", *long_word);
    if (n_on_line == 4) {
	    printf("\n");
	    n_on_line = 1;
    } else {
	    n_on_line++;
    }
}
  if (n_on_line != 1) printf("\n");
}
}

static
dump_hdr_CMD_MAP( mo_lcid_t lcid, ldc_header_t *cmd )
{
  load_cmd_map_command_t *the_map;
  int i;


the_map = ( load_cmd_map_command_t *) cmd;

printf("\nlcm_ld_cmd_strings:  %#x " , the_map->lcm_ld_cmd_strings );
printf("\nlcm_nentries:        %d (%#x) " , the_map->lcm_nentries );
for (i=0; i < the_map->lcm_nentries; i++ ) 
  printf("\n%3d   %#x ", i, the_map->lcm_map[i] );


} /* dump_hdr_CMD_MAP */

/* *********** obsolete ************ */

static char *
xlate_cmd_type( mo_long_t cmd_type )
{
  static char * ctyp[] = { "UNDEFINED"
		         , "CMD_MAP"
			 , "INTERPRETER"
			 , "STRINGS"
			 , "REGION"
			 , "RELOC"
			 , "PACKAGE"
			 , "SYMBOLS"
			 , "MAIN_ENTRY"
			 , "FUNC_TABLE"
			 , "GEN_INFO" };


  if( (unsigned) cmd_type > LDC_GEN_INFO )
    return (ctyp[0]);

  return (ctyp[cmd_type] );
}
/* ************************************** */



static void
dump_LDC_CMD_MAP(mo_lcid_t lcid)
{
  ldc_header_t  *Acmd;
  load_cmd_map_command_t *the_map;
  int i;
  char	cmd_type_buf[50];
  char	*cmd_type = cmd_type_buf;

  the_map = ( load_cmd_map_command_t *) XLATE_LCID(lcid);
  
  
  if (Print_header) {
  printf("\n\t\tOBJECT MAP\n\t\t----------\n");

  printf("Idx  Cmd Type              FileOff    Size      SectOff      SectLen\n");
  printf("---  --------              -------    ----      -------      -------\n");
}
  
  for (i=0; i < the_map->lcm_nentries; i++ ) {
    Acmd = (ldc_header_t *) XLATE_LCID (i);
    rose_type_get((int)Acmd->ldci_cmd_type, LDC_CMD_TYPE_STRIPPED_NAMES, cmd_type);
    printf("%3d  %-20s  %#-10x %#-8x  %#-10x   %#-10x\n"
	   , i
	   , cmd_type
	   , the_map->lcm_map[i]
	   , Acmd->ldci_cmd_size
	   , Acmd->ldci_section_off
	   , Acmd->ldci_section_len);
  }
  
} /* dump_LDC_CMD_MAP */


static
dump_hdr_INTERPRETER( mo_lcid_t lcid, ldc_header_t *cmd )
{

} /* dump_hdr_INTERPRETER */

static
dump_LDC_INTERPRETER( cmd )
  ldc_header_t *cmd;
{

} /* dump_LDC_INTERPRETER */


static void
dump_hdr_STRINGS( mo_lcid_t lcid, ldc_header_t *cmd )
{
  char c;
  strings_command_t *string_cmd;
  
  check_section_info( cmd );
  
  string_cmd = ( strings_command_t *) cmd;
  
  printf("\nstrc_flags:  %#x\n", string_cmd->strc_flags );
}

static
dump_LDC_STRINGS( lcid )
  mo_lcid_t  lcid;
{
ldc_header_t *cmd;
int  i, cookie;
char c;
strings_command_t *string_cmd;
char  *string_section_char;

cmd = (ldc_header_t *) XLATE_LCID( lcid );
check_section_info( cmd );

string_cmd = ( strings_command_t *) cmd;

if (Print_header)
	printf("\nSTRING SECTION CONTENTS FOR CMD %d\n", lcid);
printf("%#10x: ", 0);

for (i=0, string_section_char = (char *)&*sip[lcid].section_ptr; 
     i<cmd->ldci_section_len; i++, string_section_char++ ) {
    c =  *string_section_char;
    if (c) {
      if (isprint(c))
	putchar( c );
      else
	printf("\\%03o",c);
    } else 
      printf("\n%#10x: ", (i+1) );
  }
printf("<end of strings>\n");

} /* dump_LDC_STRINGS */

static char *
alignment(mo_long_t align) {
  static char nbuf[12];
  switch (align) {
  case 1:	return "BYTE";
  case 2:	return "WORD";
  case 4:	return "LONG";
  case 1024:	return "1KB";
  default:	sprintf(nbuf,"%#-4x",align);
    		return nbuf;
  }
}

/*
 * static char *
 * protection( mo_short_t prot ) {
 * static char pbuf[5];
 * char *p = pbuf;
 *
 * if (prot && MO_PROT_READ) *p++ = 'R';
 * if (prot && MO_PROT_WRITE) *p++ = 'W';
 * if (prot && MO_PROT_EXECUTE) *p++ = 'E';
 * if (prot == MO_PROT_NONE) *p++ = '-';
 *
 * *p = '\0';
 * return pbuf;
 *}
 */

static
dump_hdr_REGION( mo_lcid_t lcid, ldc_header_t *cmd )
{
region_command_t *reg_cmd;
int i;
int c;
char  addr_buf[20];
char  *addr_string = addr_buf;
char  usage_buf[50];
char *reg_usage = usage_buf;
char  prot_buf[80];
char *reg_prot = prot_buf;
char *reg_align;
static int init = 0;

/* do not call check_section_info for regions */

reg_cmd = (region_command_t *) cmd;

/* odump used to display the region load commands all together after the
 * other load commands.  It printed the header "Region Load Commands" and
 * the column headers only once, if the init variable was 0.  This mad
 * a nice compact table.
 *
 *  printf("Name           VAddr      Size       Flags   Reloc   Align  Usage  Prot\n"
 *	 "----           -----      ----       -----   -----   -----  -----  ----\n");
 */
/* omit header? prob shd print strings for flags, align and prot only if 
 * symbolic;  do vm_addr and rel_addr differently
 */

if (reg_cmd->regc_flags & REG_ABS_ADDR_F) {
	sprintf( addr_string, "%#-14x", reg_cmd->regc_addr.vm_addr);
} else if (reg_cmd->regc_flags & REG_REL_ADDR_F) {
	sprintf( addr_string, "%3d,%#-10x", 
		reg_cmd->regc_addr.rel_addr.adrl_lcid,
		reg_cmd->regc_addr.rel_addr.adrl_reloff);
} else 			/* should be 0, but print vm_addr in case it isn't */
	sprintf( addr_string, "%#-14x", reg_cmd->regc_addr.vm_addr);

if (!Symbolic) {
/*
  printf("Name:  %2d,%#-8x Address:  %13x Size:  %#-8x    Flags:  %#-8x  ", 
	 reg_cmd->regc_region_name.adr_lcid,
	 reg_cmd->regc_region_name.adr_sctoff, addr_string,
	 reg_cmd->regc_vm_size, reg_cmd->regc_flags);

  printf( "\nReloc cmd:  %2d  Align:  %#-4x  Usage:  %2d  Prot:  %#x",
	 reg_cmd->regc_reloc_addr, reg_cmd->regc_addralign, 
	 reg_cmd->regc_usage_type, reg_cmd->regc_initprot);
 */
  printf("\n  Name       Address          Size        Flags     Reloc  Align  Usage Prot\n");
  printf("%2d,%#-8x  %-15s  %#-10x  %#-10x   %2d   %#-4x   %2d   %#x", 
	 reg_cmd->regc_region_name.adr_lcid,
	 reg_cmd->regc_region_name.adr_sctoff, addr_string,
	 reg_cmd->regc_vm_size, reg_cmd->regc_flags,
	 reg_cmd->regc_reloc_addr, reg_cmd->regc_addralign, 
	 reg_cmd->regc_usage_type, reg_cmd->regc_initprot);

} else {			/* symbolic */
  reg_align = alignment( reg_cmd->regc_addralign );
  rose_type_get( (int)reg_cmd->regc_usage_type, REGC_USAGE_TYPE_STRIPPED_NAMES, 
		reg_usage );
  rose_flags_get( (long)reg_cmd->regc_initprot, REGC_INITPROT_SHORT_NAMES, 
		 reg_prot);
  printf("\n  Name          Address          Size      Reloc   Align  Usage  Prot    Flags\n");
  printf("%-14s  %-15s  %#-10x   %2d   %-6s %-5s %-7s " 
	 , get_string( reg_cmd->regc_region_name)
	 , addr_string
	 , reg_cmd->regc_vm_size
	 , reg_cmd->regc_reloc_addr
	 , reg_align
	 , reg_usage
	 , reg_prot );
  rose_flags_print( (long)reg_cmd->regc_flags, REGC_FLAGS_STRIPPED_NAMES );

}

} /* dump_hdr_REGION */

static
dump_file_formatted(len, endian)
  unsigned int len;
  unsigned int endian;
{
unsigned char dbuf[LINE_LENGTH];
int ibuf;
int nword;
int startbuf;
int lenbuf;
int ifile;
int i;
int c;

ifile = 0;
ibuf = 2*WORD_LENGTH + 1;
nword = 1;

printf("\n");

/*
 * Determine the length of the effective line buffer. 
 * The effective line allows for blanks between words, and
 * line indexing information at the end.  Note that a blank is always
 * between the index information and the line.
 */
do {
  lenbuf = ibuf;
  nword += nword;
  ibuf += ibuf;
} while (ibuf <= LINE_LENGTH - INDEX_LENGTH);
nword = nword / 2;


/*
 * Display the file one word at a time.
 */
while (ifile < len) {
  startbuf = ifile;

  /*
   * Read each character a byte at a time into the data buffer, zero-filling
   * the data buffer.
   */
  for (i = 0; i < nword*WORD_LENGTH; i++) {

    c =  fgetc( fp );
    ifile++;

    if (endian == BO_LSB)
      ibuf = (nword*WORD_LENGTH - 1) - i;
    else
      ibuf = i;

    if ((c == EOF) || (ifile > len))
      dbuf[ibuf] = 0;
    else            
      dbuf[ibuf] = c;
  }

  /*
   * Print out each buffer.
   */
  if (endian == BO_LSB) {
    for (i = 0; i < nword; i++) {
      for (ibuf = 0; ibuf < WORD_LENGTH; ibuf++)
        printf("%02x", dbuf[i*WORD_LENGTH + ibuf]);
      printf(" ");
    }
    printf(":%04x\n", startbuf);
  } else {
    printf("%04x:", startbuf);
    for (i = 0; i < nword; i++) {
      printf(" ");
      for (ibuf = 0; ibuf < WORD_LENGTH; ibuf++)
        printf("%02x", dbuf[i*WORD_LENGTH + ibuf]);
      }
    printf("\n");
  }
}
} /* dump_file_formatted */

static
dump_LDC_REGION( lcid )
  mo_lcid_t  lcid;
{
  ldc_header_t *cmd;
int i;
int c;

cmd = (ldc_header_t *) XLATE_LCID( lcid);;


if ( cmd->ldci_section_off) {
    if (Print_header) {
	    printf("\n\tREGION CONTENTS FOR CMD %d - ", lcid);
	    print_cmd_type( lcid );
    }
    printf ("\n\n");
    fseek(fp, cmd->ldci_section_off, SEEK_SET);

    /*
     * Dump appropriately to big or little endian if formatting flag
     * is set.
     */

    dump_file_formatted(cmd->ldci_section_len, header.moh_byte_order);
    printf("\n<end of section>\n");
}

printf("\n");

} /* dump_LDC_REGION */


static
dump_hdr_RELOC( mo_lcid_t lcid, ldc_header_t *cmd )
{

reloc_command_t *reloc_cmd;
reloc_info_t reloc_info;

reloc_cmd = ( reloc_command_t *) cmd;

printf("\nrelc_nentries:       %#x ", reloc_cmd->relc_nentries );
printf("\nrelc_owner_section:  %d ", reloc_cmd->relc_owner_section);

printf("\n\n");
} /* dump_hdr_RELOC */


static
dump_LDC_RELOC( mo_lcid_t lcid)
{
  int  i;
  reloc_command_t *reloc_cmd;
  reloc_info_t *relip;
  char	type_buf[50];
  char	*size_type = type_buf;

  reloc_cmd = ( reloc_command_t *) XLATE_LCID( lcid);

  if (Print_header) {
  printf("\n  RELOCATION SECTION FOR CMD %d, OWNER IS CMD %d - ", 
	 lcid, reloc_cmd->relc_owner_section);
  print_cmd_type( reloc_cmd->relc_owner_section );
  printf( "\n  -------------------------------------------------------------------\n");
  printf( "\t\t(%d entries)\n\t\t------------\n", reloc_cmd->relc_nentries );
}

  if (Symbolic == 0) {
    if (Print_header) {
  printf("Offset      Flags   Type   Sect,Off\n");
  printf("------      -----   ----   --------\n");
}

  for (i=0, relip = (reloc_info_t *)sip[lcid].section_ptr; 
       i < reloc_cmd->relc_nentries; i++, relip++) {

    printf("%#-9x   %#04x  %4d    "
	   , relip->ri_roffset
	   , relip->ri_flags
	   , relip->ri_size_type
	   );
    if (relip->ri_flags & RI_SYMBOL_F)
      printf( "%d,%d\n"
           , relip->ri_value.symbol_index.adx_lcid
           , relip->ri_value.symbol_index.adx_index
           );
    else printf("%d,%#x\n"
	   , relip->ri_value.loc_addr.adr_lcid
	   , relip->ri_value.loc_addr.adr_sctoff
	   );
}
} else {			/* print symbolic representation */
    if (Print_header) {
    printf("Offset     Size-Type     Sect,Offset   Flags        Target\n");
    printf("------     ---------     -----------   -----        ------\n");
}
  for (i=0, relip = (reloc_info_t *)sip[lcid].section_ptr; 
       i < reloc_cmd->relc_nentries; i++, relip++) {

    rose_type_get( (int)relip->ri_size_type, RI_SIZE_TYPE_STRIPPED_NAMES, size_type );
    printf("%#-9x  %-10s  "
	   , relip->ri_roffset
	   , size_type
	   );
    if (relip->ri_flags & RI_SYMBOL_F)
      printf( "%3d,%-7d     "
           , relip->ri_value.symbol_index.adx_lcid
           , relip->ri_value.symbol_index.adx_index
           );
    else printf("%3d,%#-10x  "
	   , relip->ri_value.loc_addr.adr_lcid
	   , relip->ri_value.loc_addr.adr_sctoff
	   );
    rose_flags_print( relip->ri_flags, RI_FLAGS_SHORT_NAMES);
    printf( "  ");

    if (relip->ri_flags & RI_SYMBOL_F) {
    print_reloc_symbol( relip->ri_symbol_index );
}   else if (relip->ri_flags & RI_LOC_F) {
	print_reloc_loc(relip->ri_loc_addr);
}
    printf("\n");
}
}
  printf("\n\t<end of relocation entries for cmd %d>\n", lcid);

} /* dump_LDC_RELOC */


/* in this case put ...ldc... before ...hdr... because latter calls former */

static
dump_LDC_PACKAGE( lcid )
  mo_lcid_t  lcid;
{
  ldc_header_t *cmd;
  int	i;
  mo_addr_t	packname;
  char	flags_buf[80];
  char	*flags_string = flags_buf;

  package_command_t *pcmd = (package_command_t *)XLATE_LCID( lcid );

  if (Print_header) {
    rose_flags_get( (long)pcmd->pkgc_flags, PKGC_FLAGS_STRIPPED_NAMES, flags_string );
  printf("\n\t%s PACKAGE LIST FOR CMD %d\n\t-------------------------------\n\n", flags_string, lcid );

  printf("Index   Package Name         Version Info\n");
  printf("-----   ------------         ------------\n");
}
  packname.adr_lcid = pcmd->pkgc_strings_id;

  for(i=0; i<pcmd->pkgc_nentries; i++) {

    packname.adr_sctoff = pcmd->pkgc_pkg_list[i].pe_pkg_name;

    printf("%5d   %-20s   %d, %#x\n"
	   , i
	   , get_string(packname)
	   , pcmd->pkgc_pkg_list[i].pe_version_addr.adr_lcid
	   , pcmd->pkgc_pkg_list[i].pe_version_addr.adr_sctoff);
  }
  if (Print_header) printf("\n<end of packages for cmd %d>\n", lcid);

} /* dump_LDC_PACKAGE */


static
dump_hdr_PACKAGE( mo_lcid_t lcid, ldc_header_t *cmd )
{
  int	save_print_header;
  package_command_t *pcmd = (package_command_t *)cmd;

  printf("\nflags: \t");
  rose_flags_print((long)pcmd->pkgc_flags, PKGC_FLAGS_STRIPPED_NAMES);
  printf("\nnentries:\t%d\n", pcmd->pkgc_nentries);
  printf("strings_id:\t%d (%#4x)\n", pcmd->pkgc_strings_id, pcmd->pkgc_strings_id);
  printf("package list:\n");
  save_print_header = Print_header;
  Print_header = 0;
  dump_LDC_PACKAGE( lcid );
  Print_header = save_print_header;

} /* dump_hdr_PACKAGE */

static
dump_hdr_SYMBOLS( mo_lcid_t lcid, ldc_header_t * cmd )
{
  symbols_command_t *sym_cmd;


  check_section_info(  cmd );
  
  sym_cmd = ( symbols_command_t *) cmd;
  
  printf("\nsymc_kind:            %#x ",  sym_cmd->symc_kind );
  rose_type_print ((int)sym_cmd->symc_kind, SYMC_KIND_STRIPPED_NAMES);

  printf("\nsymc_short_reserved:  %#x", sym_cmd->symc_short_reserved);
  printf("\nsymc_nentries:        %d ", sym_cmd->symc_nentries );
  printf("\nsymc_pkg_list:        %d (%#x) ", sym_cmd->symc_pkg_list,
	 sym_cmd->symc_pkg_list);
  printf("\nsymc_strings_section: %d (%#x) ", sym_cmd->symc_strings_section,
	 sym_cmd->symc_strings_section);
  printf("\nsymc_reloc_addr:      %d (%#x) ", sym_cmd->symc_reloc_addr, 
	sym_cmd->symc_reloc_addr);

  printf ("\n");
  
  } /* dump_hdr_SYMBOLS */


static void
get_symbol_name( sym_info, string_addr, string_buf, name_string)
  symbol_info_t *sym_info;
  mo_addr_t string_addr;
  char *string_buf;
  char **name_string;
{

  if (sym_info->si_name.symbol_name > sip[string_addr.adr_lcid].section_len) {
      *name_string = string_buf;
      strcpy( *name_string, "INVALID NAME OFFSET");
} else {
      string_addr.adr_sctoff = sym_info->si_name.symbol_name;
      *name_string = (char *)get_string(string_addr);
}
}	/* end of get_symbol_name */


static void
get_package_name( sym_info, pkg_cmd, string_addr, string_buf, name_string)
  symbol_info_t *sym_info;
  package_command_t *pkg_cmd;
  mo_addr_t string_addr;
  char *string_buf;
  char **name_string;
{
    if (sym_info->si_package_index == MO_INVALID_PKG_INDEX) {
	    *name_string = string_buf;
	    strcpy(*name_string, "<no pkg>");
    } else if (sym_info->si_package_index >= pkg_cmd->pkgc_nentries) {
	    *name_string = string_buf;
	    strcpy(*name_string, "<invalid pkg>");
    } else {
	    string_addr.adr_sctoff = pkg_cmd->pkgc_pkg_list[ sym_info->si_package_index ].pe_pkg_name;
	    *name_string = (char *)get_string(string_addr);
    }
}	/* end of get_package_name */


static void
get_symbol_value ( sym_info, field_size, value_string)
  symbol_info_t *sym_info;
  int field_size;
  char *value_string;
{
  char temp_buf[30];
  char *temp_string = temp_buf;
  int string_size;

    if ((sym_info->si_flags & SI_LITERAL_F)
	|| (sym_info->si_flags & SI_COMMON_F))
	    sprintf(temp_string, "%#x", sym_info->si_lit_val );

    else if (sym_info->si_flags & SI_IMPORT_F) 
	    sprintf(temp_string, "%d(%#x)", sym_info->si_imp_val,
		   sym_info->si_imp_val);
    else if (sym_info->si_flags & SI_ABSOLUTE_VALUE_F)
	    sprintf(temp_string, "%#x", sym_info->si_abs_val);
    else /* assume def_val */
	    sprintf(temp_string, "%d,%#x", sym_info->si_def_val.adr_lcid,
		   sym_info->si_def_val.adr_sctoff);

    string_size = strlen(temp_string);
    if (string_size <= (field_size-1))
	    sprintf(value_string, "%s%*s", temp_string, 
		    (field_size-string_size), " ");
    else sprintf(value_string, "%s ", temp_string);	/* shouldn't happen */

}	/* end of get_symbol_value */


static void
dump_LDC_SYMBOLS( mo_lcid_t lcid)
{
  ldc_header_t *cmd;
  int  i;
  symbols_command_t *sym_cmd;
  symbol_info_t *sym_info;
  package_command_t *pkg_cmd;
  mo_addr_t  name_addr;
  mo_addr_t  pkg_addr;
  char symbol_name_buf[30];
  char *symbol_name;
  char pkg_name_buf[30];
  char *pkg_name;
  char value_buf[30];
  char *value_string = value_buf;
  int string_size;
  int have_packages = 0;
  int nv_name_field_size = 33;
  int v_name_field_size = 20;
  int value_field_size = 15;

  cmd = (ldc_header_t *)XLATE_LCID( lcid );
  check_section_info(  cmd );

  sym_cmd = ( symbols_command_t *) cmd;
  name_addr.adr_lcid = sym_cmd->symc_strings_section;
  
    if (Print_header) {
  printf("\n\tSYMBOL SECTION CONTENTS FOR CMD %d - ", lcid);
  print_cmd_type ( lcid );
  printf( "\n\t----------------------------------------------------\n");
  printf( "\t\t\t(%d symbols)\n", sym_cmd->symc_nentries);
  printf( "\t\t\t------------\n");
}

  if (!Symbolic) {

  if (Print_header) {
  printf(" idx   name                          pkge    value        flags type rsv sc_type\n");
  printf("-----------------------------------------------------------------------------\n");
}
  for (i=0, sym_info = (symbol_info_t *)sip[lcid].section_ptr; 
       i < sym_cmd->symc_nentries; i++, sym_info++ ) {

	  get_symbol_name( sym_info, name_addr, symbol_name_buf, &symbol_name);
	  get_symbol_value( sym_info, value_field_size, value_string);

  /* print the name */

  printf( "%4d %s", i, symbol_name);
	  string_size = strlen( symbol_name);
  if (string_size <= (nv_name_field_size - 1))
	    printf( "%*s", (nv_name_field_size-string_size), " ");
  else printf( "\n%4d %*s", i, nv_name_field_size, " ");

  /* print everything else */

    pkg_name = pkg_name_buf;
    if (sym_info->si_package_index == MO_INVALID_PKG_INDEX)
      sprintf( pkg_name, "<none>");
    else sprintf( pkg_name, "%-6d", sym_info->si_package_index);
    
  printf( "%6s %s%-#4x %#-4x %#-2x %#-2x\n"
	 , pkg_name
	 , value_string
	 , sym_info->si_flags
	 , sym_info->si_type
	 , sym_info->si_reserved_byte
	 , sym_info->si_sc_type);


  }
} else {		/* Symbolic */

  if (IS_INVALID_LCID (sym_cmd->symc_pkg_list)) {
	  have_packages = 0;
	  pkg_name = pkg_name_buf;
	  strcpy(pkg_name, "<no pkg>");
  }  else {
	  have_packages = 1;
	  pkg_cmd = (package_command_t *) (XLATE_LCID(sym_cmd->symc_pkg_list));
	  pkg_addr.adr_lcid   = pkg_cmd->pkgc_strings_id;
  }

  if (Print_header) {
    printf( " idx  name                package\n idx            value           type sc_type rsv   flags\n");
    printf( "--------------------------------------------------------------------------\n");
}

  for (i=0, sym_info = (symbol_info_t *)sip[lcid].section_ptr; 
       i < sym_cmd->symc_nentries; i++, sym_info++ ) {

	  get_symbol_name( sym_info, name_addr, symbol_name_buf, &symbol_name);
          if (have_packages) 
		  get_package_name( sym_info, pkg_cmd, pkg_addr, pkg_name_buf, &pkg_name);
	  get_symbol_value( sym_info, value_field_size, value_string);
   
  /* print the symbol name and package name */

  printf( "%4d %s", i, symbol_name);
	  string_size = strlen( symbol_name);
  if (string_size <= (v_name_field_size - 1))
	    printf( "%*s", (v_name_field_size-string_size), " ");
  else printf( "\n%4d %*s", i, v_name_field_size, " ");

  printf("%s\n", pkg_name);

  /* print everything else */

  printf( "%4d            %s %#-4x  %#-2x    %#-2x  "
	   , i
	   , value_string
	   , sym_info->si_type
	   , sym_info->si_sc_type
	   , sym_info->si_reserved_byte);

  rose_flags_print((long)sym_info->si_flags, SI_FLAGS_SHORT_NAMES);
  printf("\n");
}

}	/* end of Symbolic */

  printf("\n\t<end of symbols for cmd %d>\n\n", lcid);

}	/* end of dump_LDC_SYMBOLS */


static void
print_export_line( sym_info, name_addr, idx )
  symbol_info_t *sym_info;
  mo_addr_t name_addr;
  int idx;
{
  char symbol_name_buf[30];
  char *symbol_name;
  char value_buf[30];
  char *value_string = value_buf;
  int string_size;
  int value_field_size = 15;

#define ENAME_FIELD_SIZE	38

	    get_symbol_name( sym_info, name_addr, symbol_name_buf, &symbol_name);
	    get_symbol_value( sym_info, value_field_size, value_string);

	    printf( "  %4d  %s", idx, symbol_name);
	    string_size = strlen( symbol_name );
	    if (string_size <= (ENAME_FIELD_SIZE-1)) 
		    printf( "%*s", ENAME_FIELD_SIZE-string_size, " ");
	    else printf("\n%4d  %*s", idx, ENAME_FIELD_SIZE, " ");

	    printf("%s", value_string);

	    rose_flags_print((long)sym_info->si_flags, SI_FLAGS_SHORT_NAMES);
	    printf("\n");

}	/* end of print_export_line */


static
dump_EXPORTS( mo_lcid_t lcid )
{
  ldc_header_t *cmd;
  int  i;
  int  k;
  symbols_command_t *sym_cmd;
  symbol_info_t *sym_info;
  package_command_t *pkg_cmd;
  mo_addr_t  name_addr;
  mo_addr_t  pkg_addr;
  char pkg_name_buf[30];
  char *pkg_name;
  char string_buf[50];
  char * string_tmp;
  int nexports = 0;
  int nprinted = 0;
  int name_not_printed = 1;
  int have_packages = 0;


  cmd = (ldc_header_t *)XLATE_LCID( lcid );
  check_section_info(  cmd );

  sym_cmd = ( symbols_command_t *) cmd;
  name_addr.adr_lcid = sym_cmd->symc_strings_section;
  if (IS_INVALID_LCID (sym_cmd->symc_pkg_list)) have_packages = 0;
  else {
	  have_packages = 1;
	  pkg_cmd = (package_command_t *) (XLATE_LCID(sym_cmd->symc_pkg_list));
	  pkg_addr.adr_lcid   = pkg_cmd->pkgc_strings_id;
  }
  
    if (Print_header) {
  printf("\n\t\tEXPORT LIST FOR CMD %d");
  printf( "\n\t\t------------------------\n");
  printf( "\t\t    (%d symbols)\n", sym_cmd->symc_other.n_exported_symb);
  printf( "\t\t    --------------\n");
  printf("  idx  name                                   value          flags\n");
  printf("--------------------------------------------------------------------------\n");
}
  
  nexports = sym_cmd->symc_other.n_exported_symb;
  nprinted = 0;

  if (have_packages) {
  for (k=0; k < pkg_cmd->pkgc_nentries; k++) {
    name_not_printed = 1;
    pkg_addr.adr_sctoff = pkg_cmd->pkgc_pkg_list[k].pe_pkg_name;

  for (i=0, sym_info = (symbol_info_t *)sip[lcid].section_ptr; 
       (i < sym_cmd->symc_nentries) && (nprinted < nexports); 
       i++, sym_info++ ) {
    
    if ((sym_info->si_flags & SI_EXPORT_F)
	&& (sym_info->si_package_index == k)) {
	    if (name_not_printed) {
		    printf( "\nPACKAGE %s :\n", (char *)get_string(pkg_addr));
		    name_not_printed = 0;
	    }	    
	    print_export_line( sym_info, name_addr, i);
	    nprinted++;
    }
}
}	/* end of package k */
}	/* end of package processing */

  if (nprinted < nexports) {
  printf("\nEXPORTS NOT IN A (VALID) PACKAGE:\n");
  for (i=0, sym_info = (symbol_info_t *)sip[lcid].section_ptr; 
       (i < sym_cmd->symc_nentries) && (nprinted < nexports); 
       i++, sym_info++ ) {
    
    if ((sym_info->si_flags & SI_EXPORT_F)
	&& ((sym_info->si_package_index == MO_INVALID_PKG_INDEX)
	    || (have_packages && (sym_info->si_package_index >= pkg_cmd->pkgc_nentries)))) {

	    print_export_line( sym_info, name_addr, i);
	    nprinted++;
    }
}
}	/* end of leftover exports */

  printf("\n\t<end of exports for cmd %d>\n", lcid);
}


static
dump_IMPORTS( mo_lcid_t lcid )
{
  ldc_header_t *cmd;
  int  i;
  symbols_command_t *sym_cmd;
  symbol_info_t *sym_info;
  package_command_t *pkg_cmd;
  mo_addr_t  name_addr;
  mo_addr_t  pkg_addr;
  char symbol_name_buf[30];
  char *symbol_name;
  char pkg_name_buf[30];
  char *pkg_name;
  char string_buf[50];
  char * string_tmp;
  int string_size;
  int have_packages = 0;
#define INAME_FIELD_SIZE	25
#define IPKG_FIELD_SIZE		25
#define INAME_AND_PKG_SIZE	(INAME_FIELD_SIZE + IPKG_FIELD_SIZE)


  cmd = (ldc_header_t *)XLATE_LCID( lcid );
  check_section_info(  cmd );

  sym_cmd = ( symbols_command_t *) cmd;
  name_addr.adr_lcid = sym_cmd->symc_strings_section;
  if (IS_INVALID_LCID (sym_cmd->symc_pkg_list)) {
	  have_packages = 0;
	  pkg_name = pkg_name_buf;
	  strcpy(pkg_name, "<no pkg>");
  }  else {
	  have_packages = 1;
	  pkg_cmd = (package_command_t *) (XLATE_LCID(sym_cmd->symc_pkg_list));
	  pkg_addr.adr_lcid   = pkg_cmd->pkgc_strings_id;
  }
  
    if (Print_header) {
  printf("\n\t\tIMPORT LIST FOR CMD %d", lcid);
  printf( "\n\t\t------------------------\n");
  printf( "\t\t    (%d symbols)\n", sym_cmd->symc_nentries);
  printf( "\t\t    --------------\n");
  printf(" idx  name                     package                  flags\n");
  printf("------------------------------------------------------------------\n");
}
  
  for (i=0, sym_info = (symbol_info_t *)sip[lcid].section_ptr; i < sym_cmd->symc_nentries; i++, sym_info++ ) {
    
    get_symbol_name( sym_info, name_addr, symbol_name_buf, &symbol_name);
    if (have_packages) 
	    get_package_name( sym_info, pkg_cmd, pkg_addr, pkg_name_buf, &pkg_name);

    printf( "%4d  %s", i, symbol_name);
    string_size = strlen( symbol_name );
    if (string_size <= (INAME_FIELD_SIZE-1)) 
	      printf( "%*s", INAME_FIELD_SIZE-string_size, " ");
    else printf("\n%4d  %*s", i, INAME_FIELD_SIZE, " ");

    printf("%s", pkg_name);
    string_size = strlen(pkg_name);
    if (string_size <= (IPKG_FIELD_SIZE-1)) 
	      printf( "%*s", IPKG_FIELD_SIZE-string_size, " ");
    else printf("\n%4d  %*s", i, INAME_AND_PKG_SIZE, " ");

    rose_flags_print((long)sym_info->si_flags, SI_FLAGS_SHORT_NAMES);
    printf("\n");
  }

  printf("\n\t<end of imports for cmd %d>\n", lcid);
}



static
dump_hdr_ENTRY( mo_lcid_t lcid, ldc_header_t * cmd )
{
entry_command_t *ent_cmd;

ent_cmd = ( entry_command_t *) cmd;

printf("\nentc_flags:\t\t%#x ", ent_cmd->entc_flags);
rose_flags_print( (long)ent_cmd->entc_flags, ENTC_FLAGS_STRIPPED_NAMES );
printf("\nentc_short_reserved:\t%#x ", ent_cmd->entc_short_reserved);
printf("\nentc_absaddr:\t\t%#x ", ent_cmd->entc_absaddr);
printf("\nentc_entry_pt:\t\t%d,%#x ", ent_cmd->entc_entry_pt.adr_lcid,
ent_cmd->entc_entry_pt.adr_sctoff);
printf("\n");

} /* dump_hdr_ENTRY */


static
dump_LDC_ENTRY( mo_lcid_t lcid )
{
  ldc_header_t *cmd;

  cmd = (ldc_header_t *)XLATE_LCID( lcid);
  if (Print_header)
    printf("\n\tENTRY INFORMATION FROM CMD %d\n", lcid);
    printf("\t-------------------------------\n");
  dump_hdr_ENTRY( lcid, cmd );

} /* dump_LDC_ENTRY */


static
dump_hdr_FUNC_TABLE( mo_lcid_t lcid, ldc_header_t *cmd )
{
  printf("\n(func_table command not implemented yet)\n");
} /* dump_hdr_FUNC_TABLE */

static
dump_LDC_FUNC_TABLE( mo_lcid_t lcid )
{

} /* dump_LDC_FUNC_TALBE*/


static void
dump_hdr_GEN_INFO( mo_lcid_t lcid, ldc_header_t *cmd )
{
  char	timebuf[100];
  gen_info_command_t *info_cmd;
  
  info_cmd = (gen_info_command_t *) cmd;

  strftime( timebuf, sizeof timebuf, "%x %X", localtime( &info_cmd->genc_obj_create_time) );
  printf("CREATED: \t\t%s\n", timebuf);

  printf("CREATOR: \t\t%s\n", get_string( info_cmd->genc_creator_name));

  strftime( timebuf, sizeof timebuf, "%x %X", localtime( &info_cmd->genc_creator_time));
  printf("CREATOR CREATED: \t%s\n", timebuf);

  printf("CREATOR VERSION: \t%s\n", get_string( info_cmd->genc_creator_version));

  printf("OPTIONS: \t\t%s\n", get_string(info_cmd->genc_options_to_creator));

} /* dump_hdr_GEN_INFO */



static
dump_LDC_GEN_INFO( mo_lcid_t lcid )
{
  ldc_header_t *cmd;

  cmd = (ldc_header_t *)XLATE_LCID( lcid);
  if (Print_header)
    printf("\n\tGENERATION INFORMATION FROM CMD %d\n", lcid);
    printf("\t------------------------------------\n");
  dump_hdr_GEN_INFO( lcid, cmd );

} /* dump_LDC_GEN_INFO */



static void
dump_ldc_header( lcid )
  mo_lcid_t  lcid;
{

  ldc_header_t *cmd;
  char	type_buf[50];
  char	*cmd_type = type_buf;

  cmd = (ldc_header_t *) XLATE_LCID( lcid );

  rose_type_get ((int)cmd->ldci_cmd_type, LDC_CMD_TYPE_STRIPPED_NAMES, cmd_type );
  printf("\nLOAD COMMAND %d \t\tType: %d (%#x) %s\n", lcid, cmd->ldci_cmd_type,
	 cmd->ldci_cmd_type, cmd_type);
  printf("Cmd Size: %#x \tSection Offset: %#x   Section Length: %#x\n",
	 cmd->ldci_cmd_size, cmd->ldci_section_off, cmd->ldci_section_len);

  switch( cmd->ldci_cmd_type) {
  case LDC_UNDEFINED:      dump_hdr_UNDEFINED( lcid, cmd ); break;
  case LDC_CMD_MAP:        dump_hdr_CMD_MAP ( lcid, cmd ); break;
  case LDC_INTERPRETER:    dump_hdr_INTERPRETER( lcid, cmd ); break;
  case LDC_STRINGS:        dump_hdr_STRINGS( lcid, cmd ); break;
  case LDC_REGION:	   dump_hdr_REGION( lcid, cmd ); break;
  case LDC_RELOC:          dump_hdr_RELOC( lcid, cmd ); break;
  case LDC_PACKAGE:        dump_hdr_PACKAGE( lcid, cmd ); break;
  case LDC_SYMBOLS:        dump_hdr_SYMBOLS( lcid, cmd ); break;
  case LDC_ENTRY:          dump_hdr_ENTRY( lcid, cmd ); break;
  case LDC_FUNC_TABLE:     dump_hdr_FUNC_TABLE( lcid, cmd ); break;
  case LDC_GEN_INFO:       dump_hdr_GEN_INFO( lcid, cmd ); break;
  default: 		   dump_hdr_UNRECOGNIZED( lcid, cmd ); break;
  }
  printf("\n");

} /* dump_ldc_header */



static
dump_header(void)
{ 
  char	type_buf1[50];
  char	type_buf2[50];
  char	*type_string1 = type_buf1;
  char	*type_string2 = type_buf2;

if (Print_header) 
printf("\n\tFILE HEADER\n -----------\n\n");

printf("magic:\t%s(%#x)\n",  (header.moh_magic == MOH_MAGIC)? "MOH_MAGIC":"UNKNOWN", header.moh_magic );

rose_type_get( (int)header.moh_byte_order, MOH_BYTE_ORDER_STRIPPED_NAMES, type_string1 );
rose_type_get ((int)header.moh_cpu_type, MOH_CPU_TYPE_STRIPPED_NAMES, type_string2);
printf("Byte order:\t%s (%#x)\t\t\tCPU Type:\t%s(%#x)\n", type_string1,
       header.moh_byte_order, type_string2, header.moh_cpu_type );

rose_type_get ((int)header.moh_data_rep_id, MOH_DATA_REP_ID_NAMES, type_string1 );
rose_type_get ((int)header.moh_cpu_subtype, MOH_CPU_SUBTYPE_STRIPPED_NAMES, type_string2);
printf("Data Rep ID:\t%s (%#x)\t\tCPU Subtype:\t%s(%#x)\n"
       , type_string1
       , header.moh_data_rep_id
       , type_string2
       , header.moh_cpu_subtype);

rose_type_get( (int)header.moh_vendor_type, MOH_VENDOR_TYPE_STRIPPED_NAMES, type_string1 );
printf("Object Format Version:  %d.%d\n", header.moh_major_version, header.moh_minor_version);
printf("Vendor:\t\t   %s (%#x)\n"
       , type_string1
       , header.moh_vendor_type );

printf("Flags:\t\t   (%#x)  ", header.moh_flags);
rose_flags_print( header.moh_flags, MOH_FLAGS_STRIPPED_NAMES);
printf("\n");

printf("load_map_cmd_off:  %#x\n", header.moh_load_map_cmd_off);
printf("first_cmd_off:\t   %#x\n", header.moh_first_cmd_off);
printf("sizeofcmds:\t   %#x\n", header.moh_sizeofcmds);
printf("n_load_cmds:\t   %#x\n", header.moh_n_load_cmds);
printf("max_page_size:\t   %#x\n", header.moh_max_page_size);
printf("reserved[0]:\t   %#x      [1]:  %#x\n", header.moh_reserved[0],
       header.moh_reserved[1]);

printf("\n");

if (header.moh_sizeofcmds < header.moh_n_load_cmds * sizeof(ldc_header_t))
   error("Warning: moh_sizeofcmds looks too small.\n");

}

/*
 * Global functions
 */

/*
 * dump_rose formats the open OSF/ROSE file specified by the file parameter.
 */

dump_rose(fileptr)
	FILE *fileptr;
{
	register struct obj_format *p;
	int	magic1,
		magic2;


	char  raw_hdr_buf[MO_SIZEOF_RAW_HDR];

	int i, ok;
        ldc_header_t *cmd;
	mo_lcid_t   ldcid;
	long File_origin;
	int  conv_error;
	char  hdr_error_buf[50];
	char  * hdr_error = hdr_error_buf;
	int cmd_count = 0;

	fp = fileptr;				/* Copy to shared-state */
	File_origin = ftell(fp);


	

	ok = fread( &raw_hdr_buf, sizeof(raw_hdr_buf), 1 ,fp);

	if (!ok) error("\n Warning: Problem reading object file.\n");

	if ( (conv_error = decode_mach_o_hdr( (void *)&raw_hdr_buf, (size_t)sizeof(raw_hdr_buf), (unsigned long)MOH_HEADER_VERSION, &header)) != MO_HDR_CONV_SUCCESS) {
	  rose_type_get(conv_error, MO_ERROR_MESSAGES, hdr_error);
	  error( hdr_error );
	  return;
  }

	fseek(fp, 0l, SEEK_SET);
	file_contents = (char *) malloc( header.moh_sizeofcmds + header.moh_first_cmd_off );
	ok = fread( file_contents, header.moh_sizeofcmds + header.moh_first_cmd_off, 1, fp );
	if (!ok) error("\n Warning: Problem reading object file.\n");

	cmd_map = (load_cmd_map_command_t *) (file_contents + header.moh_load_map_cmd_off);

	if (cmd_map->ldc_cmd_type != LDC_CMD_MAP) { 
	  error("\nCorrupt file: load map offset does not point to a load map.\n");
	  /* eventually, search the file for LDC_CMD_MAP, but set an error flag */
	  ++Error_flag;
        }

	/* get the last command and check the size of the load commands section. */
	cmd = ( ldc_header_t *) (XLATE_LCID( cmd_map->lcm_nentries-1 ));
	if ( cmd_map->lcm_map[cmd_map->lcm_nentries-1] + cmd->ldci_cmd_size
             - header.moh_first_cmd_off  != header.moh_sizeofcmds )
          error("Warning: sizeofcmds may be wrong.\n");


	/* Load the meta-data sections into memory.  (The relocation sections
	 * are only loaded if requested; the others are all loaded because
	 * it is harder to determine whether or not they will be needed.
	 * The following call fills in sip, a global array of structures.
	 */
	load_sections( cmd_map );

	/*
	 * The heart of the dump_rose routine is the flag driven dumping of each
	 * specified part of the file.  In order to provide a deterministic presentation
	 * of the output, the parts of the file are presented in a somewhat arbitry fixed order,
	 * and not in the order by which they are in the file.
	 */
	
	/* Ignore Ar_member and Ar_globals in a MACH-O file. */
	
	/* Display the file header */
	if (File_header)
	  dump_header();
	
	/* Display the load map.  It may not be the first load-command */

	if (Load_map) {
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_CMD_MAP);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_CMD_MAP( ldcid);
	  ldcid = next_ldcid( ldcid, LDC_CMD_MAP);
	}
}

	if (Head_sect) {
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_EVERYTHING);
	while (ldcid != MO_INVALID_LCID) {
	  dump_ldc_header( ldcid);
	  ldcid = next_ldcid( ldcid, LDC_EVERYTHING);
	}
}
	
	/* Display the entry point information */
	if (Entry) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_ENTRY);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_ENTRY( ldcid);
	  cmd_count++;
	  ldcid = next_ldcid( ldcid, LDC_ENTRY);
	  }
	if (cmd_count == 0) 
		printf("\nNo entry point information.\n");
	}
	
	/* Display the generation information about the file */   
	if (Info_cmd) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_GEN_INFO);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_GEN_INFO( ldcid);
	  cmd_count++;
	  ldcid = next_ldcid( ldcid, LDC_GEN_INFO);
	  }
	if (cmd_count == 0) 
		printf("\nNo generation information.\n");
	}


	/* Dump the packages */

	if (Pkg_names) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_PACKAGE);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_PACKAGE( ldcid);
	  cmd_count++;
	  ldcid = next_ldcid( ldcid, LDC_PACKAGE);
          }
	if (cmd_count == 0) 
		printf("\nNo package lists.\n");
        }
	
	/* display the symbol tables */
	if (Sym_table) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_SYMBOLS);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_SYMBOLS( ldcid);
	  cmd_count++;
	  ldcid = next_ldcid( ldcid, LDC_SYMBOLS);
	  }
	if (cmd_count == 0) 
		printf("\nNo symbol sections.\n");
	}

	/* display the defined symbol tables if not already displayed */
	if (Defined_syms && !Sym_table) {
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_SYMBOLS);
	while (ldcid != MO_INVALID_LCID) {
	    symbols_command_t *sym_cmd;
	    sym_cmd = ( symbols_command_t *) XLATE_LCID(ldcid);
	    if (sym_cmd->symc_kind == SYMC_DEFINED_SYMBOLS)
	      dump_LDC_SYMBOLS( ldcid);
	  ldcid = next_ldcid( ldcid, LDC_SYMBOLS);
	  }
	}
	
	/* display the export symbol tables if not already displayed */
	if (Export_syms && !Sym_table) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_SYMBOLS);
	while (ldcid != MO_INVALID_LCID) {
	    symbols_command_t *sym_cmd;
	    sym_cmd = ( symbols_command_t *) XLATE_LCID(ldcid);
	    if ((sym_cmd->symc_kind == SYMC_DEFINED_SYMBOLS)
		&& (sym_cmd->symc_other.n_exported_symb > 0)) {
	      dump_EXPORTS( ldcid);
	      cmd_count++;
            }
	  ldcid = next_ldcid( ldcid, LDC_SYMBOLS);
	  }
	if (cmd_count == 0) 
		printf("\nNo exported symbols.\n");
	}
	
	/* Display the import symbol tables if not already displayed */
	if (Import_syms && !Sym_table) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_SYMBOLS);
	while (ldcid != MO_INVALID_LCID) {
	    symbols_command_t *sym_cmd;
	    sym_cmd = ( symbols_command_t *) XLATE_LCID(ldcid);
	    if ((sym_cmd->symc_kind == SYMC_IMPORTS)
		&& (sym_cmd->symc_nentries > 0)) {
	      dump_IMPORTS( ldcid);
	      cmd_count++;
            }
	  ldcid = next_ldcid( ldcid, LDC_SYMBOLS);
	  }
	if (cmd_count == 0) 
		printf("\nNo imported symbols.\n");
	}
	
	/* Display the debug symbol tables if not already displayed */
	if (Debug_syms && !Sym_table) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_SYMBOLS);
	while (ldcid != MO_INVALID_LCID) {
	    symbols_command_t *sym_cmd;
	    sym_cmd = ( symbols_command_t *) XLATE_LCID(ldcid);
	    if (sym_cmd->symc_kind == SYMC_STABS) {
	      dump_LDC_SYMBOLS( ldcid);
	      cmd_count++;
    }
	  ldcid = next_ldcid( ldcid, LDC_SYMBOLS);
	  }
	if (cmd_count == 0) 
		printf("\nNo debug symbols (stabs).\n");
	}
	
	/* Display the relocations */
	if (Reloc) {
	cmd_count = 0;
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_RELOC);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_RELOC( ldcid);
	      cmd_count++;
	  ldcid = next_ldcid( ldcid, LDC_RELOC);
	  }
	if (cmd_count == 0) 
		printf("\nNo relocation information.\n");
	}
	
	/* Display the string table(s) */
	if (Str_table) {
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_STRINGS);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_STRINGS( ldcid);
	  ldcid = next_ldcid( ldcid, LDC_STRINGS);
	  }
	}
	
	/* Display the region sections */
	if (Obj_sect) {
	ldcid = next_ldcid( MO_INVALID_LCID, LDC_REGION);
	while (ldcid != MO_INVALID_LCID) {
	  dump_LDC_REGION( ldcid);
	  ldcid = next_ldcid( ldcid, LDC_REGION);
  }
}	
printf("\n");
      } /* dump_rose */
