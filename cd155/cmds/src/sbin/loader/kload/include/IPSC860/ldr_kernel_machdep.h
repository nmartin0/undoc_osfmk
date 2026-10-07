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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */

/* Note that this include file allocates static storage, and hence must
 * only be included by ldr_kernel_bootstrap.c
 */

/* a.out */
int kls_aout_recog(const char *filename, ldr_file_t fd,
		   ldr_module_handle *handle);
int kls_aout_map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
			 int *reg_count, ldr_region_rec **regions);
int kls_aout_get_export_pkgs(ldr_module_handle handle, int *count,
			     ldr_package_rec **packages);
int kls_aout_get_exports(ldr_module_handle handle, int *countp,
			 ldr_symbol_rec **exports);
int kls_aout_lookup_export(ldr_module_handle handle,
			   ldr_package_rec *package, ldr_symbol_rec *symbol);
int kls_aout_get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt);

/* OSF/Mach-O */
int kls_macho_recog(const char *filename, ldr_file_t fd,
		   ldr_module_handle *handle);
int kls_macho_map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
			 int *reg_count, ldr_region_rec **regions);
int kls_macho_get_export_pkgs(ldr_module_handle handle, int *count,
			     ldr_package_rec **packages);
int kls_macho_get_exports(ldr_module_handle handle, int *countp,
			  ldr_symbol_rec **exports);
int kls_macho_lookup_export(ldr_module_handle handle,
			    ldr_package_rec *package, ldr_symbol_rec *symbol);
int kls_macho_get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt);

/* COFF */
int kls_coff_recog(const char *filename, ldr_file_t fd,
		   ldr_module_handle *handle);
int kls_coff_map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
			 int *reg_count, ldr_region_rec **regions);
int kls_coff_get_export_pkgs(ldr_module_handle handle, int *count,
			     ldr_package_rec **packages);
int kls_coff_get_exports(ldr_module_handle handle, int *countp,
			 ldr_symbol_rec **exports);
int kls_coff_lookup_export(ldr_module_handle handle,
			   ldr_package_rec *package, ldr_symbol_rec *symbol);
int kls_coff_get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt);

/* This is the fake kernel loader switch entry used for the loader's module
 * record.  Only the minimal set of required operations are supplied.
 */ 

static struct loader_switch_entry kernel_loader_switch[] = {
    {
	LSW_VERSION,			/* structure version */
	LSF_MUSTOPEN,			/* open required */
	kls_aout_recog,			/* recognizer */
	get_static_dep,			/* get_static_dep */
	get_imports,			/* get_imports */
	kls_aout_map_regions,		/* map_regions */
	kls_aout_get_export_pkgs,	/* get export packages */
	kls_aout_get_exports,		/* get exports */
	kls_aout_lookup_export,		/* lookup export */
	relocate,			/* relocate */
	kls_aout_get_entry_pt,		/* get entry */
	NULL,				/* no inits */
	cleanup,			/* cleanup */
	NULL				/* unload; can't be called */
    },
    {
	LSW_VERSION,			/* structure version */
	LSF_MUSTOPEN,			/* open required */
	kls_macho_recog,		/* recognizer */
	get_static_dep,			/* get_static_dep */
	get_imports,			/* get_imports */
	kls_macho_map_regions,		/* map_regions */
	kls_macho_get_export_pkgs,	/* get export packages */
	kls_macho_get_exports,		/* get exports */
	kls_macho_lookup_export,	/* lookup export */
	relocate,			/* relocate */
	kls_macho_get_entry_pt,		/* get entry */
	NULL,				/* no inits */
	cleanup,			/* cleanup */
	NULL				/* unload; can't be called */
    },
    {
	LSW_VERSION,			/* structure version */
	LSF_MUSTOPEN,			/* open required */
	kls_coff_recog,			/* recognizer */
	get_static_dep,			/* get_static_dep */
	get_imports,			/* get_imports */
	kls_coff_map_regions,		/* map_regions */
	kls_coff_get_export_pkgs,	/* get export packages */
	kls_coff_get_exports,		/* get exports */
	kls_coff_lookup_export,		/* lookup export */
	relocate,			/* relocate */
	kls_coff_get_entry_pt,		/* get entry */
	NULL,				/* no inits */
	cleanup,			/* cleanup */
	NULL				/* unload; can't be called */
    }
};

int n_kernel_lsw_entries = 
	sizeof(kernel_loader_switch)/sizeof(kernel_loader_switch[0]);
