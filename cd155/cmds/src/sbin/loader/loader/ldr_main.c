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
/* ldr_main.c
 * Main loop and top-level routines for loader
 *
 * This file contains the main loop for loading modules
 * and the top-level support routines for the format-
 * independent loader.  The main loop is called from the
 * various load-related system calls (such as "load" and
 * "ldr_xload", as as well as from the library pre-load
 * entry point and the loader startup routine in a
 * stand-alone loader being run during exec.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <sys/file.h>
#include <string.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "dqueue.h"
#include "ldr_errno.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"
#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"

#include "ldr_lock.h"
#include "ldr_known_pkg.h"
#include "ldr_module.h"
#include "ldr_switch.h"
#include "ldr_global_file.h"
#include "ldr_symres.h"


/* Forward procedure declarations */

static int load_regions_list(ldr_context *context);
static int relocate_modules_list(ldr_context *context);
static int initialize_modules_list(ldr_context *context);
static int cleanup_modules_list(ldr_context *context);
static void abort_loading_modules_list(ldr_context *context);
static int cleanup_exportonly_list(ldr_context *context);


/* Static data */

/* main_program_allocs is the allocation routines for a main program.
 * These routines are only used when allocating regions for a main
 * program (ie one flagged with LDR_MAIN -- this should only be used
 * by the standalone loader when loading into the process context).
 */

static ldr_region_allocs main_program_allocs = {
	alloc_abs_process_region,	/* abs allocator */
	alloc_rel_main_region,		/* rel allocator */
	dealloc_process_region		/* deallocator */
	};


int
ldr_context_load(ldr_context_t ctxt, const char *module_name,
		 ldr_load_flags_t load_flags, ldr_module_t *mod_id)

/* Load the specified module and all its required dependencies (both
 * static dependencies and dependencies to resolve import symbols)
 * into the specified loader context.  This includes calling the
 * module initialization routines for all modules, unless the load_flags
 * specified no initialization (eg. for kernel modules).  Return the
 * module ID of the loaded module in *mod_id.
 * Returns LDR_SUCCESS on success or a negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt; /* internal rep */
	ldr_module_rec		*main_rec; /* module record for main module */
	int			rc;	/* return status */

	/* Check for invalid load flags */

	if ((load_flags & (LDR_EXPORTONLY|LDR_WIRE)) != 0)
		return(LDR_EINVAL);

	/* Serialize */

	ldr_lock_context(context);

	/* Build or find the module record; put in hash table */

	if ((rc = ldr_get_module_record(context, module_name, load_flags,
					&main_rec)) != LDR_SUCCESS)
		goto unlock_exit;

	/* If the module is already loaded in this context, just return its
	 * module ID.  We don't support loading multiple instances of a single
	 * module in a given context.  If user specified no-prexist, return
	 * LDR_EEXIST error (but still return module ID).
	 */

	if (main_rec->lm_flags & LMF_LOADED) {
		*mod_id = main_rec->lm_module;
		if (load_flags & LDR_NOPREXIST)
			rc = LDR_EEXIST;
		else
			rc = LDR_SUCCESS;
		goto unlock_exit;
	}

	/* get_module_record can't put the module record on the known module
	 * list, because it doesn't know the right order to use.  So, enqueue
	 * it here.  A new main module goes at the tail of the list.
	 */

	if ( !(main_rec->lm_flags & LMF_ONLIST)) {
		dq_ins_tail(&(context->lc_known_modules), &(main_rec->lm_list));
		lm_flag_onlist(main_rec);
	}

	/* Next, get the list of the module's exported packages, and add them
	 * to the context's loaded package table.  This must be done before
	 * finding the module's dependencies, as the symbol resolution
	 * algorithm examines the loaded package table.
	 */

	if ((rc = ldr_install_lpt(context, main_rec)) != LDR_SUCCESS)
		goto cleanup;

	/* Now find all of this module's dependencies, both static dependencies
	 * and dependencies due to unresolved import symbols.  Build module
	 * records for all the dependencies and link them on the known module
	 * list in the "natural" load order (see the comments on the
	 * get_module_dependencies routine for details).
	 */

	if ((rc = ldr_get_module_dependencies(context)) != LDR_SUCCESS)
		goto cleanup;

	/* The remainder of the routines in the loading process operate on
	 * the entire known module list for this context.  They iterate through
	 * the list, applying the operation to all the modules that are
	 * being loaded in this pass (the main module and the transitive closure
	 * of all its dependencies).
	 */

	/* Assign all the region addresses and map all the regions into
	 * the process' address space, building the region records.
	 */

	if ((rc = load_regions_list(context)) != LDR_SUCCESS)
		goto cleanup;

	/* Perform all the necessary relocation */

	if ((rc = relocate_modules_list(context)) != LDR_SUCCESS)
		goto cleanup;

	/* Call all the initialization routines for all the newly-loaded
	 * modules.
	 */

	if ( !(load_flags & LDR_NOINIT) &&
	    (rc = initialize_modules_list(context)) != LDR_SUCCESS)
		goto cleanup;

	/* Cleanup the format-dependent manager information for all the
	 * newly-loaded modules, and mark them as fully loaded.
	 */

	if ((rc = cleanup_modules_list(context)) != LDR_SUCCESS)
		goto cleanup;

	/* Loading completed successfully.  Return the module ID */

	*mod_id = main_rec->lm_module;
	rc = LDR_SUCCESS;
	goto unlock_exit;

cleanup:
	/* An error occurred during the loading of this module or one of its
	 * dependencies.  Clean up the whole list of partially-loaded modules.
	 * This entails scanning the entire known module list, finding all
	 * modules that are in the state of being loaded, and doing the
	 * appropriate cleanup (unmapping regions, deallocating data
	 * structures, etc.) for each.  We ignore errors during cleanup
	 * so that we can return the original error status.
	 */

	abort_loading_modules_list(context);

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


static int
load_regions_list(ldr_context *context)

/* Iterate through the context's known module list.  For each module
 * being loaded (flags & LMF_LOADING), call the format-dependent
 * manager to assign addresss to and map in each region in the module and
 * to fill in the region record corresponding to each such region.
 */
{
	ldr_module_rec		*mod;	/* module being loaded */
	ldr_region_allocs	*allocsp; /* region allocator routines */
	ldr_region_rec		*regions; /* region list */
	int			count;	/* region count */
	int			rc;

	for_all_modules(context, mod) {

		if (!(mod->lm_flags & LMF_LOADING))
			continue;

		/* If this is the main module of a program being loaded,
		 * we have to use different region allocation routines
		 * (so that data and bss end up in the right place).
		 * Note that this assumes that the LDR_MAIN flag will
		 * only be set on programs being loaded into the
		 * program context.
		 */

		allocsp = (mod->lm_load_flags & LDR_MAIN) ? &main_program_allocs :
			&context->lc_allocsp;

		/* Call the format-dependent loader to map the regions and
		 * fill in the region record (key fields for us are vaddr,
		 * mapaddr, and region size).  Then save the region list and
		 * count in the module record.
		 */

		if ((rc = LSW_MAP_REGIONS(mod, allocsp, &count, &regions)) != LDR_SUCCESS)
			return(rc);

		lm_set_region_list(mod, count, regions);

	}
	return(LDR_SUCCESS);
}


static int
relocate_modules_list(ldr_context *context)

/* Iterate through all the modules in the context's known module list,
 * and relocate each module.  Relocation is a completely format-dependent
 * routine.  Before calling the module's relocation routine, attempt to
 * precompute the absolute values of all the import symbols in the
 * module's import symbol table; this is an attempt to improve performance
 * of relocation.
 */
{
	ldr_module_rec		*mod;	/* module being loaded */
	int			rc;

	for_all_modules(context, mod) {

		if (!(mod->lm_flags & LMF_LOADING))
			continue;

		ldr_precompute_imports(mod);

		if ((rc = LSW_RELOCATE(mod, mod->lm_region_count, mod->lm_regions,
				       mod->lm_import_pkg_count, mod->lm_import_pkgs,
				       mod->lm_import_count, mod->lm_imports)) != LDR_SUCCESS)
			return(rc);
	}

	return(LDR_SUCCESS);
}


static int
initialize_modules_list(ldr_context *context)

/* Iterate through the context's known module list, calling the initialization
 * routines (if any) for each newly-loaded module in the list.
 * Return LDR_SUCCESS on success or a negative status code on error.
 */
{
	ldr_module_rec		*mod;	/* module being initialized */
	int			rc;

	for_all_modules(context, mod) {

		if (!(mod->lm_flags & LMF_LOADING) ||
		    (mod->lm_load_flags & LDR_NOINIT) )
			continue;

		if ((rc = LSW_RUN_INITS(mod, init_routines)) != LDR_SUCCESS)
			return(rc);
	}

	return(LDR_SUCCESS);
}


static int
cleanup_modules_list(ldr_context *context)

/* Iterate through the context's known module list, cleaning up the
 * loading for each module.  This means at least setting the "loading
 * completed" flag and freeing the import list (at least for modules
 * with no unresolved references).  It also means calling the format-
 * dependent manager's "cleanup" entry point, to close any open
 * files and free up any data structures that are no longer needed now
 * that the module is loaded.  And, finally, it means unloading all
 * the modules that were loaded exportonly during the loading of
 * this module, to reclaim the resources (such as open file descriptors)
 * that are used by the export-only modules.
 * Return LDR_SUCCESS on success or a negative error status on error.
 */
{
	ldr_module_rec		*mod;	/* module being initialized */
	int			rc;

	for_all_modules(context, mod) {

		if (!(mod->lm_flags & LMF_LOADING))
			continue;

		if ((rc = LSW_CLEANUP(mod)) != LDR_SUCCESS)
			return(rc);

		lm_flag_loaded(mod);
	}

	if ((rc = cleanup_exportonly_list(context)) != LDR_SUCCESS)
		return(rc);

	return(LDR_SUCCESS);
}


static void
abort_loading_modules_list(ldr_context *context)

/* An error occurred during the loading of a module.  Clean up the
 * whole list of partially-loaded modules.  This entails scanning the
 * entire known module list, finding all modules that are in the state
 * of being loaded, and doing the appropriate cleanup (unmapping
 * regions, deallocating data structures, etc.) for each.  We ignore
 * errors during cleanup so that we can return the original error
 * status.
 */
{
	ldr_module_rec		*mod;	/* module record being cleaned up */
	ldr_module_rec		*next;	/* next module record */

	for (mod = context->lc_known_modules.ll_forw;
	     mod != (ldr_module_rec *)&(context->lc_known_modules);
	     mod = next) {

		next = mod->lm_forw;
		if (!(mod->lm_flags & LMF_LOADING))
			continue;

		(void)ldr_internal_module_unload(context, mod);
	}

	(void)cleanup_exportonly_list(context);
}


static int
cleanup_exportonly_list(ldr_context *context)

/* Iterate through the list of modules loaded export-only for this
 * context, and unload each.  The export-only list is cleaned up
 * at the end of each explicit load.
 */
{
	ldr_module_rec		*mod;	/* module record being cleaned up */
	ldr_module_rec		*next;	/* next module record */
	int			rc, rrc;

	rc = LDR_SUCCESS;
	for (mod = context->lc_exportonly_modules.ll_forw;
	     mod != (ldr_module_rec *)&(context->lc_exportonly_modules);
	     mod = next) {

		next = mod->lm_forw;
		rrc = ldr_internal_module_unload(context, mod);
		if (rc == LDR_SUCCESS) rc = rrc;
	}
	return(rc);
}


int
ldr_internal_module_unload(ldr_context *context, ldr_module_rec *mod)

/* Unload the specified module.  Clean up all its data structures
 * (import and export lists), unload all its regions, clean up
 * the format-dependent data.  If all operations succeed, return
 * LDR_SUCCESS; if any fail, return the status code of the first
 * failing operation but continue on and try to complete the
 * unload anyway.
 */
{
	ldr_region_rec		*reg;	/* region being cleaned up */
	int			regid;	/* region id */
	int		rc;
	int		rrc;

	rc = LDR_SUCCESS;

	/* First, remove the module from the loaded package table lists */

	rrc = ldr_remove_lpt(context, mod);
	if (rc == LDR_SUCCESS) rc = rrc;

	/* Tell the format-dependent manager to unload the specified
	 * module.  It should unmap all mapped regions, close the open
	 * file, free all the data structures, etc.  After this call,
	 * the module handle, region lists, import and export lists
	 * are all invalid.
	 */

	rrc = LSW_UNLOAD(mod, &context->lc_allocsp,
			 mod->lm_region_count, mod->lm_regions,
			 mod->lm_import_pkg_count, mod->lm_import_pkgs,
			 mod->lm_import_count, mod->lm_imports,
			 mod->lm_export_pkg_count, mod->lm_export_pkgs);
	if (rc == LDR_SUCCESS) rc = rrc;

	/* Finally, pull it off the known module list and the
	 * hash chains, and free the module record itself.
	 */

	ldr_module_destroy(context, mod);
	return(rc);
}


int
ldr_context_get_entry_pt(ldr_context_t ctxt, ldr_module_t mod_id,
			 ldr_entry_pt_t *entry)

/* Return the entry point for the module named by the specified module ID
 * in the specified loader context.  Returns LDR_SUCCESS on success or
 * a negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*module; /* module record for this module */
	ldr_entry_pt_t		ent;	/* its entry point */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	if ((rc = translate_module_id(context, mod_id, &module)) != LDR_SUCCESS)
		goto unlock_exit;

	if ((rc = LSW_GET_ENTRY_PT(module, &ent)) != LDR_SUCCESS)
		goto unlock_exit;

	*entry = ent;
	rc = LDR_SUCCESS;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_unload(ldr_context_t ctxt, ldr_module_t mod_id)

/* Unload the specified module.  Don't do anything to any of this module's
 * dependencies.
 * Return LDR_SUCCESS on success or a negative error status on error (EINVAL
 * for invalid module ID).
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*module; /* module record for this module */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	if ((rc = translate_module_id(context, mod_id, &module)) != LDR_SUCCESS)
		goto unlock_exit;

	/* Make sure module can really be unloaded; don't allow unloading
	 * the loader module, for example.
	 */

	if (module->lm_load_flags & LDR_NOUNLOAD) {
		rc = LDR_EINVAL;
		goto unlock_exit;
	}

	if (!(module->lm_load_flags & LDR_NOINIT)) {

		/* Run the module's termination routines */

		if ((rc = LSW_RUN_INITS(module, term_routines)) != LDR_SUCCESS)
			goto unlock_exit;
	}

	/* Finally, do the unload */

	rc = ldr_internal_module_unload(context, module);

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_lookup(ldr_context_t ctxt, ldr_module_t mod_id, char *symbol_name,
		   void **value)

/* Look up the specified symbol in the symbols exported by the specified
 * module ID, and return the absolute value of the symbol in *value.
 * Return LDR_SUCCESS on success or negative error status on error (ERANGE
 * if symbol value cannot be represented as a void *).
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*module; /* module record for this module */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	/* find module corresponding to this ID */

	if ((rc = translate_module_id(context, mod_id, &module)) != LDR_SUCCESS)
		goto unlock_exit;

	/* WHAT DOES IT MEAN TO LOOK UP A SYMBOL WITHOUT A PACKAGE NAME??? */

	rc = LDR_EINVAL;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_lookup_package(ldr_context_t ctxt, char *package,
			   char *symbol_name, void **value)

/* Look up the specified symbol in the symbols exported from the specified
 * package, and return the absolute value of the symbol in *value.
 * Return LDR_SUCCESS on success or negative error status on error (ERANGE
 * if symbol value cannot be represented as a void *).
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_package_rec		pkg;	/* temp package record */
	ldr_symbol_rec		sym;	/* temp import symbol record */
	ldr_load_flags_t	flags;	/* load flags for lookup */
	ldr_module_rec		*mod;	/* module exporting symbol */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	/* Construct dummy package record and import symbol record for this
	 * symbol, for call to resolve_symbol.  Flags for call will indicate
	 * that module must already be loaded, and NOT just loaded export-only
	 * (this prevents incorrectly finding a symbol declaration in
	 * a module we haven't yet loaded, and also prevents this call from
	 * accidentally loading a module!
	 */

	pkg.lp_version = LDR_PACKAGE_VERSION;
	pkg.lp_kind = ldr_package;
	pkg.lp_name = package;

	sym.ls_version = LDR_SYMBOL_VERSION;
	sym.ls_name = symbol_name;
	sym.ls_packageno = 0;		/* unused */
	ldr_symval_make_unres(&sym.ls_value);

	flags = LDR_PREXIST | LDR_NOUNREFS;

	if ((rc = ldr_resolve_symbol(context, &pkg, &sym, flags)) != LDR_SUCCESS)
		goto unlock_exit;

	/* We have a value back, but it may not be an absolute symbol
	 * value.  Try to convert it to absolute, if possible.  If
	 * it can't be converted to an absolute value, it's a range error.
	 */

	mod = sym.ls_module;
	ldr_symval_cvt_abs(&sym.ls_value, mod->lm_regions, mod->lm_region_count);
	if (!ldr_symval_is_abs(&sym.ls_value)) {

		/* Couldn't convert to absolute; return range error */

		rc = LDR_ERANGE;
		goto unlock_exit;
	}

	/* Got it; return the value */

	*value = ldr_symval_abs(&sym.ls_value);
	rc = LDR_SUCCESS;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_next_module(ldr_context_t ctxt, ldr_module_t *mod_id_ptr)

/* Iterator through the module IDs for all modules currently loaded
 * in the specified context.  To initialize the iterator, set
 * *mod_id_ptr to LDR_MODULE_NULL.  The next call to this routine
 * should be made using the handle returned from this call and so on.
 * After the last module, the handle returned is the LDR_NULL_MODULE.
 * On success the call returns a zero, on failure it returns a
 * negative error number.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*module; /* module record for this module */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	if (*mod_id_ptr == LDR_NULL_MODULE) {

		/* Initializing iterator; start with head of known module list */

		module = context->lc_known_modules.ll_forw;
	} else {

		/* find module corresponding to this ID, step to next */

		if ((rc = translate_module_id(context, *mod_id_ptr, &module)) != LDR_SUCCESS)
			goto unlock_exit;
		module = module->lm_forw;
	}

	if (module == (ldr_module_rec *)&(context->lc_known_modules))

		/* End of known module list; quit */

		*mod_id_ptr = LDR_NULL_MODULE;
	else
		*mod_id_ptr = module->lm_module;

	rc = LDR_SUCCESS;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_inq_module(ldr_context_t ctxt, ldr_module_t mod_id,
		       ldr_module_info_t *info, size_t info_size,
		       size_t *ret_size)

/* Return module information about the module with the specified ID
 * in the specified context, into the info buffer supplied by the
 * caller.  info_size is the size of the buffer provided.  Returns the
 * actual size of the returned structure on success, or a negative
 * error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*module; /* module record for this module */
	ldr_module_info_t	linfo;	/* local copy of info record */
	size_t			size;	/* size to copy */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	/* find module corresponding to this ID */

	if ((rc = translate_module_id(context, mod_id, &module)) != LDR_SUCCESS)
		goto unlock_exit;

	/* Fill in info record */

	linfo.lmi_modid = mod_id;
	linfo.lmi_nregion = module->lm_region_count;
	linfo.lmi_flags = module->lm_load_flags;
	strcpy(linfo.lmi_name, module->lm_name);

	size = (info_size >= sizeof(linfo) ? sizeof(linfo) : info_size);
	bcopy(&linfo, info, size);
	*ret_size = size;
	rc = LDR_SUCCESS;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_inq_region(ldr_context_t ctxt, ldr_module_t mod_id,
		       ldr_region_t region, ldr_region_info_t *info,
		       size_t info_size, size_t *ret_size)

/* Return module information about the specified region of the
 * module with the specified ID in the specified context, into the
 * info buffer supplied by the caller.  info_size is the size of the
 * buffer provided.  Returns the actual size of the returned structure in
 * *ret_size.  Return 0 on success, or a negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*module; /* module record for this module */
	ldr_region_info_t	linfo;	/* local copy of info record */
	size_t			size;	/* size to copy */
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	/* find module corresponding to this ID */

	if ((rc = translate_module_id(context, mod_id, &module)) != LDR_SUCCESS)
		goto unlock_exit;

	if (region >= module->lm_region_count) { /* region out of range? */
		rc = LDR_EINVAL;
		goto unlock_exit;
	}

	/* Fill in info record */

	linfo.lri_region_no = region;
	linfo.lri_prot = module->lm_regions[region].lr_prot;
	linfo.lri_vaddr = module->lm_regions[region].lr_vaddr;
	linfo.lri_mapaddr = module->lm_regions[region].lr_mapaddr;
	linfo.lri_size = module->lm_regions[region].lr_size;
	if (module->lm_regions[region].lr_name != NULL &&
	    strlen(module->lm_regions[region].lr_name) < (LDR_REGION_NAME_MAX-1))
		strcpy(linfo.lri_name, module->lm_regions[region].lr_name);
	else
		linfo.lri_name[0] = '\0';

	size = (info_size >= sizeof(linfo) ? sizeof(linfo) : info_size);
	bcopy(&linfo, info, size);
	*ret_size = size;
	rc = LDR_SUCCESS;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_install(ldr_context_t ctxt, const char *module_name)

/* Install the specified module in the private known package table
 * of the specified context.  If the private known package table does
 * not yet exist, create it.  The module must not duplicate any currently-
 * installed package names in the private known package table.  Returns
 * LDR_SUCCESS on success or negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_private_file_hdr	priv_hdr;
	ldr_module_rec		*mod;
	int			npkgs;
	int			rc;

	/* Serialize */

	ldr_lock_context(context);

	/* Initialize context's private data file, if necessary */

	if (context->lc_private_kpt == NULL) {

		if ((rc = ldr_private_file_init(&priv_hdr)) != LDR_SUCCESS)
			goto unlock_exit;
		context->lc_private_kpt = ldr_private_file_kpt(priv_hdr);
	}

	/* Build or find the module record for this module. */

	if ((rc = ldr_get_module_record(context, module_name, LDR_EXPORTONLY,
					&mod)) != LDR_SUCCESS)
		goto unlock_exit;

	/* Construct installed module record from the exportonly module
	 * record we just got.  This involves copying the relevant fields
	 * of the exportonly module record into a newly-allocated module
	 * record in the heap of the kpt, then getting its export
	 * package table and creating its kpt list.  Finally, the new
	 * installed module record is linked on the kpt header's list.
	 * It's an error if the module record already exists.
	 */
 
	rc = ldr_install_module(context->lc_private_kpt, mod);

	/* Install done; if module was not already loaded, clean up */

cleanup:
	(void)cleanup_exportonly_list(context);

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_remove(ldr_context_t ctxt, const char *module_name)

/* Remove the specified module from the private known package table
 * of the specified context.  Returns LDR_SUCCESS on success or
 *  negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*mod;
	int			rc;
	int			rrc;

	/* Serialize */

	ldr_lock_context(context);

	/* Private KPT must exist */

	if (context->lc_private_kpt == NULL) {

		rc = LDR_ENOMODULE;
		goto unlock_exit;
	}

	/* Find installed module record. */
 
	if ((rc = ldr_lookup_installed_module(context->lc_private_kpt,
					      module_name, &mod)) != LDR_SUCCESS)
		goto unlock_exit;

	/* Remove module's packages from the kpt */

	rrc = ldr_kpt_list_remove(context->lc_private_kpt->lkh_kpt, mod);
	if (rc == LDR_SUCCESS) rc = rrc;

	/* Finally, remove installed module record */

	rrc = ldr_remove_module(context->lc_private_kpt, mod);
	if (rc == LDR_SUCCESS) rc = rrc;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_atexit(ldr_context_t ctxt)

/* Run any per-module termination routines for modules loaded into the
 * specified context.  This is intended to be called from the exit()
 * procedure during normal process exit.  Returns LDR_SUCCESS on success,
 * or negative error status on error (but tries to run all termination
 * routines even if one or more return errors).
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_module_rec		*mod;
	int			rc;
	int			rrc;

	/* Serialize */

	ldr_lock_context(context);

	rc = LDR_SUCCESS;
	for_all_modules(context, mod) {

		if (mod->lm_load_flags & LDR_NOINIT)
			continue;

		rrc = LSW_RUN_INITS(mod, term_routines);
		if (rc == LDR_SUCCESS) rc = rrc;
	}

	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_inherit(ldr_context_t ctxt)

/* Check to see whether a loader private data file has been inherited from
 * our parent process, and if so, inherit it and get from it the private 
 * KPT of the specified loader context.  Then, try to open the loader
 * global data file and inherit it, setting up the global KPT
 * of the specified context.  Called only during loader
 * bootstrapping.  Returns LDR_SUCCESS on success, negative error
 * status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_private_file_hdr	priv_hdr;
	ldr_file_t		fd;
	ldr_global_file_hdr	glob_hdr;
	int			rc;

	ldr_lock_context(context);

	/* First inherit private kpt */

	if ((rc = ldr_private_file_inherit(&priv_hdr)) != LDR_SUCCESS)
		goto unlock_exit;

	/* No error; if we actually got a private file, set up
	 * private KPT header
	 */

	context->lc_private_kpt = ldr_private_file_kpt(priv_hdr);

	rc = LDR_SUCCESS;

	/* Now try global data file */

	if ((fd = ldr_open(ldr_global_data_file, LDR_O_RDONLY)) < 0) {

		/* No global data file; just treat as success */

		goto unlock_exit;
	}

	if (ldr_global_file_inherit(fd, &glob_hdr) == LDR_SUCCESS) {

		/* Successful.  If we got a global file, set global KPT address */

		context->lc_global_kpt = ldr_global_file_kpt(glob_hdr);
	}

	(void)ldr_close(fd);

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_global_file_init(ldr_context_t ctxt, ldr_file_t fd)

/* The procedure is intended to be called only from the global library
 * installation program.  It initializes the loader global data file,
 * which contains the global known package table and all
 * pre-loaded libraries (not yet supported).  The global known package
 * table is initialized to a copy of the private installed package
 * table from the specified loader context.  When pre-loading is
 * supported, the pre-loaded libraries will be initialized from the
 * known module list of the specified context as well.
 *
 * This routine constructs the global data file header, initializes
 * the heap in the global data file, and copies the private KPT
 * from the context into the global data file's heap.  Arguments are
 * the loader context containing the private KPT and pre-loaded libraries
 * to initialize the global data file from, and a file descriptor open
 * for writing on the file to be initialized.
 *
 * Note that for this routine to be successful, the calling process
 * must not currently be using the global data file.  A caller
 * should call ldr_context_global_file_remove() before calling
 * this routine.
 *
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	ldr_global_file_hdr	glob_hdr;
	int			rc;

	ldr_lock_context(context);

	if ((rc = ldr_global_file_init(fd, &glob_hdr)) != LDR_SUCCESS)
		goto unlock_exit;

	context->lc_global_kpt = ldr_global_file_kpt(glob_hdr);
	if (context->lc_private_kpt != NULL) {

		if ((rc = ldr_kpt_copy(context->lc_private_kpt,
				       context->lc_global_kpt)) != LDR_SUCCESS)
			goto unlock_exit;
	}

	/* Now copy out any preloaded libraries */

	if (!dq_empty(&context->lc_known_modules)) {

		if ((rc = ldr_global_file_preload(glob_hdr, context)) != LDR_SUCCESS)
			goto unlock_exit;
	}

	(void)ldr_global_file_set_size(glob_hdr);

	rc = LDR_SUCCESS;

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}


int
ldr_context_global_file_remove(ldr_context_t ctxt)

/* The procedure is intended to be called only from the global library
 * installation program.  It removes all dependencies on the loader global
 * data file from the specified loader context, and unmaps the
 * loader global data file from the address space.  It is intended
 * to be used in preparation for a call to ldr_context_global_file_init,
 * which must map the new global file into the same region of the
 * address space that the current global file is using.
 * Note that the calling program must not be using any pre-loaded libraries.
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	int			rc;

	ldr_lock_context(context);

	if (context->lc_global_kpt == NULL) { /* must not be one */
		rc = LDR_SUCCESS;
		goto unlock_exit;
	}

	context->lc_global_kpt = NULL;	/* break dependency */

	rc = ldr_global_file_remove();

unlock_exit:
	ldr_unlock_context(context);
	return(rc);
}
