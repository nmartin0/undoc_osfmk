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
/* ldr_bootstrap.c
 * Bootstrap routines and data structures for the loader
 *
 * This file contains the "ldr_bootstrap" routine and its associated
 * data structures, used for bootstrapping the loader into a new
 * process.  The ldr_bootstrap routine creates a new loader context
 * for the process, and initializes it with a module record describing
 * the loader itself.  The loader's module record exports those packages
 * and symbols that are implemented by the loader (such as "load").  There
 * is a static loader switch entry for the loader as well, used during
 * the fake loading of the loader module.
 *
 * OSF/1 Release 1.0
 */


#include <sys/types.h>
#include <string.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_lock.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "open_hash.h"
#include "dqueue.h"
#include "ldr_errno.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"
#include "ldr_windows.h"

#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_switch.h"

extern int preload_alloc_abs(univ_t vaddr, size_t size, ldr_prot_t prot, univ_t *baseaddr);
extern int preload_alloc_rel(size_t size, ldr_prot_t prot, univ_t *vaddr, univ_t *baseaddr);
extern int preload_dealloc(univ_t vaddr, univ_t mapaddr, size_t size);

/* The ldr_bootstrap_exports.tab file contains the statically-initialized
 * package and export symbol tables for the loader.  It is mechanically
 * generated from the list of loader exports.
 */

#include "ldr_bootstrap_exports.tab"


/* Forward declarations */

static int recog(const char *filename, ldr_file_t fd, ldr_module_handle *handle);
static int get_static_dep(ldr_module_handle handle, int depno, char **dep);
static int get_imports(ldr_module_handle handle, int *pkg_count,
		       ldr_package_rec **pkgs, int *sym_count,
		       ldr_symbol_rec **imports);
static int map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
		       int *reg_count, ldr_region_rec **regions);
static int get_export_pkgs(ldr_module_handle handle, int *count,
			   ldr_package_rec **packages);
static int get_exports(ldr_module_handle handle, int *sym_count,
		       ldr_symbol_rec **exports);
static int lookup_export(ldr_module_handle handle, ldr_package_rec
			 *package, ldr_symbol_rec *symbol);
static int relocate(ldr_module_handle handle, int nregions,
		    ldr_region_rec regions[], int npackages,
		    ldr_package_rec import_pkgs[], int nimports,
		    ldr_symbol_rec imports[]);
static int get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt);
static int cleanup(ldr_module_handle handle);



/* This is the fake loader switch entry used for the loader's module
 * record.  Only the minimal set of required operations are supplied.
 */ 

const struct loader_switch_entry loader_loader_switch = {
	LSW_VERSION,			/* structure version */
	LSF_NONE,			/* open not required */
	recog,				/* recognizer */
	get_static_dep,			/* get_static_dep */
	get_imports,			/* get_imports */
	map_regions,			/* map_regions */
	get_export_pkgs,		/* get export packages */
	get_exports,			/* get exports */
	lookup_export,			/* lookup export */
	relocate,			/* relocate */
	get_entry_pt,			/* get entry */
	NULL,				/* no inits */
	cleanup,			/* cleanup */
	NULL				/* unload; can't be called */
	};


int
ldr_bootstrap(const char *loader_name, ldr_context_t *ctxt)

/* Bootstrap the loader system.  Includes building the loader process
 * context, containing the module and region records and exported symbol
 * list for the loader itself.  The loader_name argument is the name of
 * the loader's object module, for building the module record.  Returns
 * the address of the newly-created context in *ctxt.
 *
 * On error, we don't even try to free data structures; if the loader
 * can't be bootstrapped, the program is going to exit quickly anyway.
 */
{
	ldr_context_t		loc_ctxt;
	ldr_module_t		mod_id;
	int			i;
	int			rc;

	if (ldr_process_heap == NULL) {
		if ((rc = ldr_heap_init()) != LDR_SUCCESS)
			return(rc);
	}

	/* Now create the loader process context */

	if ((rc = ldr_context_create(LDR_NMODULES, alloc_abs_process_region,
				     alloc_rel_process_region, 
				     dealloc_process_region, &loc_ctxt)) != LDR_SUCCESS)
		return(rc);

	/* Now bootstrap the new context with the loader's exported symbols */

	if ((rc = ldr_context_bootstrap(loc_ctxt, loader_name)) != LDR_SUCCESS)
		return(rc);

	/* Finally, try to inherit the private and global loader data
	 * files.  Ignore inheritance errors.
	 */

	(void)ldr_context_inherit(loc_ctxt);

	*ctxt = loc_ctxt;
	rc = LDR_SUCCESS;

	return(rc);
}


int
ldr_context_bootstrap(ldr_context_t ctxt, const char *loader_name)

/* Bootstrap the specified loader context, by constructing a module
 * record for the loader itself and filling it in with the loader's
 * exports.  We bootstrap the context by doing a "fake load" on the
 * specified loader file, using a private loader switch as a temporary
 * switch so the recognizer will not try to open the file.  Returns
 * LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_module_t		mod_id;
	int			rc;

	/* Temporarily push the fake loader switch entry onto the context,
	 * while doing the fake load of the loader module itself.  Note
	 * that we push the entry on the HEAD of the switch, so it will
	 * take precedence over all other managers.
	 */

	(void)ldr_switch_ins_head(ctxt, (ldr_switch_t)&loader_loader_switch);

	/* Now do the fake load of the module.
	 * NOTE that the NOUNLOAD flag here has an overloaded meaning -- it also
	 * tells the preload routines NOT to copy out this module record to
	 * the preload context during preloading.
	 */

	if ((rc = ldr_context_load(ctxt, loader_name, (LDR_NOUNLOAD|LDR_NOINIT),
				   &mod_id)) != LDR_SUCCESS)
		return(rc);

	/* Fake load complete; pop our dummy switch entry back off */

	ldr_switch_rem_head(ctxt);
	return(LDR_SUCCESS);
}


static int
recog(const char *filename, ldr_file_t fd, ldr_module_handle *handle)

/* This function acts as the recognizer during the fake loading of the
 * loader module.  It has nothing to do; it always succeeds.
 */
{
	*handle = NULL;
	return(LDR_SUCCESS);
}


static int
get_export_pkgs(ldr_module_handle handle, int *count, ldr_package_rec **packages)

/* Return the loader's package list.  The package list is statically
 * initialized in the bootstrap exports file.  Always succeeds.
 */
{
	*count = loader_packages_count;
	*packages = loader_packages;
	return(LDR_SUCCESS);
}


static int
lookup_export(ldr_module_handle handle, ldr_package_rec *package,
	      ldr_symbol_rec *symbol)

/* Look up the specified package name and symbol name in the loader's
 * exported symbol table and  fill in its value in the import symbol
 * record.  The export symbol table is statically initialized in the
 * bootstrap exports file.
 * Can use the following fields in the import record:
 *  - symbol name
 * Must fill in the following fields in the import symbol record:
 *  - symbol value
 * Return LDR_SUCCESS on success or negative error status on error.
 */
{
	open_hashtab_t		table = (open_hashtab_t)&loader_exports;
	struct export_symbol	*sym;
	int			rc;

	if ((rc = open_hash_lookup(table, (const univ_t)symbol->ls_name,
				   (univ_t *)&sym)) != LDR_SUCCESS)
		return(LDR_ENOSYM);

	if (strcmp(package->lp_name, loader_packages[sym->se_pkgno].lp_name) != 0)
		return(LDR_ENOSYM);

	symbol->ls_value = sym->se_value;
	return(LDR_SUCCESS);
}


static int
get_exports(ldr_module_handle handle, int *sym_count, ldr_symbol_rec **exports)

/* Get the list of symbols exported by the loader proper and return
 * it.
 */
{
	ldr_symbol_rec	*exp_list;
	struct export_symbol *sym;
	int		export_count;
	int		i, j;
	int		rc;

	export_count = loader_exports.h_header.hh_nelem;
	if ((rc = ldr_symbols_create(export_count, LDR_SYMBOL_VERSION,
				     &exp_list)) != LDR_SUCCESS)
		return(rc);

	for (i = 0, j = 0; i < loader_exports.h_header.hh_maxelem; i++) {
		if (loader_exports.h_entries[i].key == NULL)
			continue;
		sym = (struct export_symbol *)loader_exports.h_entries[i].value;
		exp_list[j].ls_name = ldr_strdup(sym->se_name);
		exp_list[j].ls_packageno = sym->se_pkgno;
		exp_list[j].ls_value = sym->se_value;
		j++;
	}

	*sym_count = export_count;
	*exports =  exp_list;
	return(LDR_SUCCESS);
}


/* Dummy routines to fill in the remaining slots in the loader switch */

static int
get_static_dep(ldr_module_handle handle, int depno, char **dep)

/* Dummy for fake loading */
{
	return(LDR_EAGAIN);
}


static int
get_imports(ldr_module_handle handle, int *pkg_count,
	    ldr_package_rec **pkgs, int *sym_count,
	    ldr_symbol_rec **imports)

/* Dummy for fake loading */
{
	*pkg_count = *sym_count = 0;
	*pkgs = NULL;
	*imports =  NULL;
	return(LDR_SUCCESS);
}


static int
map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
		       int *reg_count, ldr_region_rec **regions)

/* Build a dummy region list describing the loader itself.  For now,
 * we just return an empty list.  Eventually, could change to use
 * getaddrconf() and/or etext,edata,end.
 */
{
	*reg_count = 0;
	*regions = NULL;
	return(LDR_SUCCESS);
}


static int
relocate(ldr_module_handle handle, int nregions,
	 ldr_region_rec regions[], int npackages,
	 ldr_package_rec import_pkgs[], int nimports,
	 ldr_symbol_rec imports[])

/* Dummy */
{
	return(LDR_SUCCESS);
}


static int
get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt)

/* Routine to get the loader's entry point.  Loader doesn't have an
 * entry point, so return NULL address with LDR_SUCCESS status.
 */
{
	*entry_pt = NULL;
	return(LDR_SUCCESS);
}


static int
cleanup(ldr_module_handle handle)

/* Nothing to do */
{
	return(LDR_SUCCESS);
}
