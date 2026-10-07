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

#ifndef _LDFCN_H_
#define _LDFCN_H_

#include <standards.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <mach_o_types.h>
#include <mach_o_format.h>
#include <mach_o_header.h>
#include <mach_o_header_md.h>

#define LD_FAIL -1
#define LD_SUCCESS 0

#ifdef __STDC__
#define VOID_PTR void *
#else
#define VOID_PTR char *
#endif


/* useful macros to access OSF/Rose format */
/* per-file macros */
#define SECT_PTR(x, i) (x->ld_region_list[i].ld_reg_sect_ptr)
#define REGION_CMD(x, i) (x->ld_region_list[i].ld_reg_cmd)
#define RELOC_PTR(x, i) (x->ld_region_list[i].ld_reloc)
#define SECT_LEN(x) (x != NULL? x->ldc_section_len : 0)
#define SECT_OFF(x) (x != NULL? x->ldc_section_off : 0)
#define ALIGN_SIZE(x, i) (x->ld_region_list[i].ld_reg_cmd->regc_addralign)

/* 
 * The follwing macros are written in terms of the above macros, because any
 * of text, data, bss, etc may be missing.
 */
#define STRING_SIZE(x)    (SECT_LEN(x->ld_strings_cmd))
#define RELOC_SIZE(x, i)  (SECT_LEN(x->ld_region_list[i].ld_reloc_cmd))

typedef struct ld_region  
    {
    mo_lcid_t ld_region_lcid;        /* the region load command id */
    region_command_t *ld_reg_cmd;    /* region load command */
    char *ld_reg_sect_ptr;  	     /* section in-memory pointer */
    reloc_command_t *ld_reloc_cmd;   /* region relocation */
    reloc_info_t *ld_reloc;          /* section pointer */
    struct ld_region *ld_next;       /* link for multiple regions same type */
    } ld_region;

typedef struct ldfile
    {
    char 	*ld_filename;	/* file name string */
    FILE	*ld_macho_fd;	/* File descriptor */
    mo_header_t ld_header;	/* Macho header */

    struct ldfile *ld_superfile;   /* name of library */
    struct ldfile *ld_subfiles;    /* only for archive libraries */
    off_t	ld_starting_offset; /* for an archive member...starting 
				       offset in file */
    struct ldfile *ld_chain;       /* chain of member files for archives */

    mo_long_t ld_region_count;     /* the number of regions in list */
	                           /* list of region commands */

    ld_region ld_region_list[MAX_USAGE_TYPE+1];     

    load_cmd_map_command_t *ld_map_cmd; /* ptr to load map */
    char **ld_map_vector;          /* this is a pointer to a vector of pointers
				      that can be indexed by load command id to
				      get the pointer to the load command */

    /* symbols and strings */
    char 	*ld_strings;		/* single string section */
    symbol_info_t *ld_symbols;     	/* Points to beginning of symbol sect*/
    symbol_info_t *ld_imports;		/* import symbols */
    symbol_info_t *ld_debug;		/* debug symbols */

    strings_command_t *ld_strings_cmd;  /* string load command */
    symbols_command_t *ld_def_cmd;      /* def sym load command */    
    symbols_command_t *ld_imp_cmd;      /* import sym load command */
    symbols_command_t *ld_debug_cmd;      /* debug sym load command */
    package_command_t *ld_package_import_cmd;  /* load command for packages to be imported */
    package_command_t *ld_package_export_cmd;  /* load command for packages to be exported */

    entry_command_t *ld_main_entry;      /* the main entry point load command */
    gen_info_command_t *ld_gen_info;     /* The generation info load command */
    func_table_command_t *ld_func_cmd;   
    mo_long_t	ld_symsize;              /* total size of combined symbol section */
    char ld_is_open_flag;                
    }ldfile ;

#ifdef __STDC__                 /* ANSI compilant compiler */
        typedef void *generic_ptr_t;
#else /* __STDC__ */            /* non-ANSI compiler */
        typedef char *generic_ptr_t;
#endif /* __STDC__ */

/*********************************************************/
/*
 * Exported functions (from libld)
 */
#ifdef _NO_PROTO
ldfile *ldOpen();
ldfile *ldInit();
void ldClose();
int ldIsObject();
int ldReadSyms();
int ldReadStrings();
char* ldReadSection();
ldfile *ldAlloc();
void ldFree();
int decode_mach_o_hdr ();
int encode_mach_o_hdr ();
int export_foreach ();

#else
ldfile* ldOpen (char *);
ldfile* ldInit (ldfile*);
void ldClose (ldfile *);
int ldIsObject( void *, size_t, unsigned long, mo_header_t *);
int ldReadSyms( ldfile * );		/* non-zero return implies error! */
int ldReadStrings(ldfile * );		/* non-zero return implies error! */
char* ldReadSection(ldfile *, ldc_header_t*);
ldfile* ldAlloc();
void ldFree( ldfile * );
int decode_mach_o_hdr (generic_ptr_t, size_t, unsigned long, mo_header_t*);
int encode_mach_o_hdr (mo_header_t*, generic_ptr_t, size_t);
int export_foreach (char *, void (*func)(char *));
#endif /* _NO_PROTO_ */


typedef struct arbuf_t {	/* Usable copy of archive-member header */
  char		ar_name[16];
  time_t	ar_date;
  uid_t		ar_uid;
  gid_t		ar_gid;
  mode_t	ar_mode;
  size_t	ar_size;
} arbuf_t;


#ifdef _NO_PROTO_
	typedef int (*ARfunc_t)();
	extern int ARisarchive();
	extern int ARforeach();
#else
	typedef int (*ARfunc_t)( FILE *, arbuf_t* );
	extern int ARisarchive( FILE * );
	extern int ARforeach( FILE *, ARfunc_t);
#endif /*_NO_PROTO */

#endif /* _LDFCN_H_ */




