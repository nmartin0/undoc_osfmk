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
/* ldr_symres.c
 * Symbol resolution algorithm for loader
 *
 * This file contains the implementation of the symbol resolution
 * algorithm for the loader.  The symbol resolution algorithm is
 * based on looking up the package for each unresolved symbol
 * in a module, and loading the object module that exports
 * the required package.  There are three known package tables:
 *  - the loaded package table
 *  - the private known package table
 *  - the global known package table
 * and a package name is looked up in this order.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "open_hash.h"
#include "dqueue.h"
#include "ldr_errno.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"
#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_switch.h"

#include "ldr_lock.h"
#include "ldr_known_pkg.h"
#include "ldr_module.h"
#include "ldr_symres.h"

/* Forward declarations */

static int get_static_dependencies(ldr_context *context, ldr_module_rec *mod);
static int resolve_import(ldr_context *context, ldr_module_rec *mod,
			  ldr_package_rec *pkg, ldr_symbol_rec *sym);



int ldr_get_module_record(ldr_context *context, const char *module_name,
			  ldr_load_flags_t flags, ldr_module_rec **mod)

/* Return a module record for the specified module.  First look up the
 * library name in the context's hash table; if found, return its module
 * record.  If not found, allocate a module record.  Call the recognizer
 * functions to determine the object file format of the module and to
 * get the format-dependent handle back.  Then call the format-dependent
 * manager to get the export list for the module.  Add the module
 * to the context's hash table (but NOT to its known module list, since
 * we don't know where in the list to link the module).  The flags
 * can indicate that the module is simply being scanned for exported
 * symbols during symbol resolution instead of being loaded (LDR_EXPORTONLY).
 * Note that an export-only module is always returned fully loaded and
 * linked on the context's export-only list.
 * Return LDR_SUCCESS on success or negative error status on error.
 */
{
	struct lm_hash_entry	*lhe;	/* for chain hash lookup */
	ldr_module_rec	*module;	/* new module record */
	ldr_file_t	fd;		/* file desc for new module */
	ldr_module_handle handle;	/* format-dependent handle */
	struct loader_switch_entry *lsw; /* temp for loader switch */
	int		rc;

	if (chain_hash_lookup((chain_hashtab_t)context->lc_module_hash,
			      (const univ_t)module_name,
			      (chain_hash_elem **)(&lhe)) == 0) {

		/* Module is known already; just return its handle */

		module = lhe->lh_rec;	/* back ptr to record */

		/* If module is currently loaded exportonly and we're
		 * requesting a full load, must upgrade module's load
		 * request to full load.  This removes the module from
		 * the export-only module list, clears its
		 * export-only flag and marks it as being loaded.
		 * Don't do this if caller required that module must
		 * already be loaded.
		 */

		if (module->lm_load_flags & LDR_EXPORTONLY) {

			if (flags & LDR_PREXIST)
				return(LDR_ENOMODULE);

			if (!(flags & LDR_EXPORTONLY))
				lm_upgrade_exportonly(module);
		}

		*mod = module;
		return(LDR_SUCCESS);
	}

	if (flags & LDR_PREXIST)

		/* User wanted only a pre-existing module.  Return error */

		return(LDR_ENOMODULE);

	/* Allocate and fill in the module record as much as possible */

	if ((rc = ldr_module_create(context, module_name, &module)) != LDR_SUCCESS)
		return(rc);

	/* Now call the format-dependent recognizers until one accepts
	 * responsibility for this module.  If we reach the end of the
	 * list, try to load a dynamic auxiliary manager and try again.
	 * Keep going until module is recognized or no auxiliary managers
	 * remain.
	 */

	fd = LDR_FILE_NONE;		/* start with no file open */
	do {

		if ((rc = ldr_recognize(context, module_name, &fd, &handle, &lsw)) != LDR_ENOEXEC)
			break;

	} while (ldr_load_dyn_mgr(context) == LDR_SUCCESS);

	if (rc != LDR_SUCCESS) {	/* module was unrecognized */

		ldr_module_destroy(context, module);
		(void)ldr_close(fd);
		return(rc);
	}

	/* Format-dependent manager recognized this module.  Fill in rest
	 * of module record and return it.  Note that manager is now
	 * responsible for fd.
	 */

	module->lm_switch = lsw;
	module->lm_handle = handle;
	lm_set_load_flags(module, flags);

	/* If this is an export-only load, the loading is now complete.  So,
	 * enqueue the module on the context's export-only list (order doesn't
	 * matter there, so we can do the enqueueing ourselves).  Mark it as on
	 * list and loaded. Otherwise flag the module as "loading" and don't
	 * put it on the list; it is the caller's responsibility to enqueue it
	 * in the right place on the known module list.
	 */

	if (flags & LDR_EXPORTONLY) {
		dq_ins_tail(&(context->lc_exportonly_modules), &(module->lm_list));
		lm_flag_onlist(module);
		lm_flag_loaded(module);
	} else
		lm_flag_loading(module);

	*mod = module;
	return(LDR_SUCCESS);
}


int
ldr_recognize(ldr_context *context, const char *module_name, ldr_file_t *fd,
	      ldr_module_handle *handle, struct loader_switch_entry **lswp)

/* Run the recognizers of the managers in the loader switch for the
 * specified loader context.  Run them in turn until one recognizes
 * the module, or until all have returned failure.  On success, return
 * the manager's handle in *handle, and the loader switch entry in
 * *lsw.  The fd argument is an in/out argument containing an open
 * file descriptor for the module; if it is not equal LDR_FILE_NONE
 * and the manager demands that the file be opened, we will open
 * it here.  Return LDR_SUCCESS on success or negative error status
 * on error.
 */
{
	struct ldr_switch_links *lsl;	/* temp for loader switch */
	struct loader_switch_entry *lsw; /* temp for loader switch */
	ldr_file_t		ifd;	/* internal fd */
	int			rc;

	ifd = *fd;
	for_all_switch_entries(&context->lc_switch, lsl) {

		lsw = lsl->lsl_entry;

		/* See whether the manager requires the file to be
		 * opened, and if so, open it here.  Note that on a
		 * successful recognize, we don't close the file here;
		 * it becomes the responsibility of the format-
		 * dependent manager.  Also notice that we don't
		 * support execute-only files.
		 */

		if ((ifd == LDR_FILE_NONE) && (lsw->lsw_flags & LSF_MUSTOPEN) &&
		    ((ifd = ldr_open(module_name, LDR_O_RDONLY)) < 0)) {

			ldr_log("ldr_recognize: error opening file %s: %E\n", module_name, ifd);
			return(ifd);
		}

		*fd = ifd;
		if ((rc = (*(lsw->lsw_recog))(module_name, ifd, handle)) ==
		    LDR_SUCCESS) {

			/* Recognized; return loader switch entry */

			*lswp = lsw;
			return(LDR_SUCCESS);
		}
	}

	/* No one accepted this module as his own.  Return failure */

	return(LDR_ENOEXEC);
}


int
ldr_install_lpt(ldr_context *context, ldr_module_rec *mod)

/* Get the list of all the packages exported by the specified module
 * and add them to the loaded package table for this context.  Once in
 * the loaded package table, they are available for resolution of
 * unresolved import symbols.
 *
 * Returns LDR_SUCCESS on success and negative error status on error.
 */
{
	int			npkgs;	/* number of exported packages */
	ldr_package_rec		*pkgs;	/* export package list */
	ldr_kpt_rec		*kpt_list; /* kpt list */
	int			rc;

	if ((rc = LSW_GET_EXPORT_PKGS(mod, &npkgs, &pkgs)) != LDR_SUCCESS)
		return(rc);

	/* Now, construct the lpt list from the exported package list. */

	if ((rc = ldr_kpt_list_create(ldr_process_heap, npkgs, &kpt_list)) != LDR_SUCCESS) {

		/* Must leave module record in a consistent state for
		 * later cleanup.
		 */

		lm_set_kpt_list(mod, npkgs, pkgs, NULL);
		return(rc);
	}

	lm_set_kpt_list(mod, npkgs, pkgs, kpt_list);

	/* Now point the lpt list entries at their corresponding package
	 * records, and chain them into the lpt.
	 */

	if ((rc = ldr_kpt_insert(context->lc_lpt, mod)) != LDR_SUCCESS)
		return(rc);

	return(LDR_SUCCESS);
}


int
ldr_remove_lpt(ldr_context *context, ldr_module_rec *mod)

/* Remove the specified module's loaded packages from the loaded package
 * table for this context, as part of unloading the module.  First, unhash
 * the module's lpt records from the context's loaded package table; then
 * free the lpt list.  Returns LDR_SUCCESS on success or negative error
 * status on error.
 */
{
	int		rc;

	if (mod->lm_kpt_list == NULL)
		return(LDR_SUCCESS);

	if ((rc = ldr_kpt_list_remove(context->lc_lpt, mod)) != LDR_SUCCESS)
		return(rc);

	return(ldr_kpt_list_free(ldr_process_heap, mod->lm_export_pkg_count,
				 mod->lm_kpt_list));
}


int
ldr_get_module_dependencies(ldr_context *context)

/* Build module records for all the static dependencies (and,
 * recursively, all their static dependencies, etc.) of modules being
 * loaded in the specified context.  Then resolve all unresolved
 * import symbols in the list of modules to be loaded, by querying the
 * format-dependent manager to get the information on the unresolved
 * symbol (including symbol name and package), and then looking up the
 * package and symbol in the installed library table.  The module
 * records are enqueued on the (global) module list.  The noures flag
 * indicates that unresolved symbols are not allowed.
 *
 * Note that this loop is very careful to keep the static dependencies
 * in their "natural" load order (pre-order depth-first tree walk).
 * This is necessary so that the ELF format-dependent manager can
 * simulate the behavior of the standard System V Release 4 symbol
 * resolution policy, which depends on load order (sigh).
 */
{
	ldr_module_rec		*mod;
	int			rc;

	for_all_modules(context, mod) {

		if (!(mod->lm_flags & LMF_LOADING))
			continue;

		if (!(mod->lm_flags & LMF_STATIC_DEP_DONE))

			/* Get static dependencies for this module, if any.
			 * They must be enqueued in "depth-first" order
			 * immediately after this module on the list.
			 */

			if ((rc = get_static_dependencies(context, mod)) != LDR_SUCCESS)
				return(rc);

		/* Next, do import symbol resolution */

		if ((rc = ldr_get_import_list(mod)) != LDR_SUCCESS)
			 return(rc);

		if ((rc = ldr_resolve_imports(context, mod)) != LDR_SUCCESS)
			return(rc);
	}

	return(LDR_SUCCESS);
}


static int
get_static_dependencies(ldr_context *context, ldr_module_rec *mod)

/* Build module records for all the static dependencies of this module,
 * enqueueing them on the known module list just before the module's
 * successor.  See the comment on the get_module_dependencies routine
 * about the importance of maintaining the proper order of the
 * known module list.  Return LDR_SUCCESS on success or negative error
 * status on error.
 */
{
	char		*dep;
	int		depno;
	ldr_module_rec	*dep_mod;
	ldr_module_rec	*next_mod;
	int		rc;

	next_mod = mod->lm_forw;

	for (depno = 0; (rc = LSW_GET_STATIC_DEP(mod, depno, &dep)) == LDR_SUCCESS;
	     depno++) {

		/* Get module record for this dependency.  If
		 * it's not already on the list, enqueue it in
		 * the right place.  Also, install its list of
		 * exported packages to the loaded package
		 * table (static dependencies behave like
		 * explicitly loaded modules in that all their
		 * exported packages are installed in the loaded
		 * package table).
		 */

		rc = ldr_get_module_record(context, dep,
					   (mod->lm_load_flags & LDR_FLAGS_PROPAGATE),
					   &dep_mod);
		(void)ldr_free(dep);
		if (rc != LDR_SUCCESS)
			return(rc);

		if ( !(dep_mod->lm_flags & LMF_ONLIST)) {
			dq_ins_before(&(dep_mod->lm_list), &(next_mod->lm_list));
			lm_flag_onlist(dep_mod);

			/* Install in lpt */

			if ((rc = ldr_install_lpt(context, dep_mod)) != LDR_SUCCESS)
				return(rc);
		}
	}

	lm_flag_static_dep_done(mod);

	if (rc == LDR_EAGAIN)
		rc = LDR_SUCCESS;
	return(rc);
}


int
ldr_get_import_list(ldr_module_rec *mod)

/* Build the imported packages list and imported symbols list for the
 * specified module.  Allocate the lists, then call the format-dependent 
 * manager to fill them in.  All import symbols are initially unresolved.
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	int			pkg_count; /* number of packages */
	ldr_package_rec		*pkg_list; /* package list */
	int			sym_count; /* number of symbols */
	ldr_symbol_rec		*import_list; /* import list */
	int			rc;

	if ((rc = LSW_GET_IMPORTS(mod, &pkg_count, &pkg_list, &sym_count,
				  &import_list)) != LDR_SUCCESS)
		return(rc);

	lm_set_imports(mod, pkg_count, pkg_list, sym_count, import_list);

	return(LDR_SUCCESS);
}


int
ldr_resolve_imports(ldr_context *context, ldr_module_rec *mod)

/* Resolve each of the import symbols in the specified module's
 * import list to a value as exported by some other module.  Find
 * the module exporting the symbol by looking up the symbol's packge
 * in the various known package tables.  Then build an module record
 * (possibly export-only) for the module, and look up the symbol in
 * its export list to ensure that it's really there.  Iterate until
 * you find a symbol resolution.
 *
 * If no resolution is found for the symbol, call a machine-dependent
 * "unresolved symbol" routine to take care of it.  This routine
 * may return a value for the symbol, or it may indicate that an
 * error is to be returned.  If the module flags indicate no unresolveds
 * allowed, don't do machine-dependent handling, just return an error.
 */
{
	int			symno;	/* import symbol number */
	ldr_package_rec		*pkg;	/* symbol's package */
	ldr_symbol_rec		*sym;	/* symbol */
	int			rc;

	for_all_imports(mod, symno, pkg, sym)

		if ((rc = resolve_import(context, mod, pkg, sym)) != LDR_SUCCESS)
			return(rc);

	return(LDR_SUCCESS);
}


static int
resolve_import(ldr_context *context, ldr_module_rec *mod, ldr_package_rec *pkg,
	       ldr_symbol_rec *sym)

/* Resolve the specified import package and symbol, being imported by the
 * specified module in the specified context.  If necessary, the module
 * exporting the symbol will be marked for loading.  Note that the symbol
 * is resolved to the extent of determining which module exports the
 * symbol and getting the most complete symbol value available for the
 * symbol at this time.  The symbol value may not be fully known at this
 * time -- eg. because the module's regions have not yet been assigned
 * addresses.  The absolute symbol values will be computed during relocation.
 */
{
	ldr_load_flags_t	flags;	/* load flags for resolution */
	ldr_module_rec		*next_mod = mod->lm_forw;
	ldr_module_rec		*dep_mod; /* module exporting symbol */
	int			rc;

	/* Try to resolve the import symbol.  Specify export-only for
	 * loading (we will upgrade to full loading status on success).
	 */

	flags = (mod->lm_load_flags & LDR_FLAGS_PROPAGATE) | LDR_EXPORTONLY;
	if ((rc = ldr_resolve_symbol(context, pkg, sym, flags)) != LDR_SUCCESS)
		return(rc);

	/* We now know the symbol's exporting module and value, so
	 * set up to load the module if necessary.  If the module is
	 * loaded exportonly, upgrade it to loading; then put it on
	 * the known module list in the right place.
	 */

	dep_mod = sym->ls_module;
	if (dep_mod->lm_load_flags & LDR_EXPORTONLY)

		lm_upgrade_exportonly(dep_mod);

	if ( !(dep_mod->lm_flags & LMF_ONLIST)) {
		dq_ins_before(&(dep_mod->lm_list), &(next_mod->lm_list));
		lm_flag_onlist(dep_mod);
	}

	return(LDR_SUCCESS);
}


int
ldr_resolve_symbol(ldr_context *context, ldr_package_rec *pkg,
		   ldr_symbol_rec *sym, ldr_load_flags_t flags)

/* Try to resolve the specified package/symbol pair in the specified
 * loader context.  If successful, fill in the symbol value structure
 * (including the identity of the exporting module) with the best
 * currently known representation of the symbol value.
 *
 * The "package" may be a real package name, in which case it is looked up
 * in the various known package tables to determine the module to be loaded.
 * Alternatively, it may be a full module name, in which case the symbol
 * must be exported by the specified module; this is the hook which allows
 * format-dependent managers (such as ELF) to control their own symbol
 * resolution policies.
 *
 * The module flags may indicate that no unresolved symbols are allowed; if so
 * and it can't be resolved, don't even try to do any machine-dependent handing
 * for unresolved symbols.
 */
{
	ldr_module_rec		*dep_mod; /* module exporting symbol */
	ldr_kpt_rec		*kpte;
	enum {
		try_done,		/* nothing more to do */
		try_lpt,		/* first lookup in lpt */
		try_private_kpt,	/* then private kpt */
		try_global_kpt,		/* then global kpt */
		try_unres,		/* try unresolved handler */
		try_module		/* if module-name style package */
		} this_loop, next_loop; /* what are we trying now? */
	int			pkg_found; /* true if found pkg name */
	int			rc;

	pkg_found = 0;

	if (pkg->lp_kind == ldr_package_module) {

		/* This is a module name-style package, so symbol must
		 * be exported by that module. Don't try package tables at all.
		 */
		this_loop = try_module;
	} else {

		/* Normal module; try package tables */
		this_loop = try_lpt;
	}

	for ( ; this_loop != try_done; this_loop = next_loop) {

		switch (this_loop) {

		      case try_module:
			
			next_loop = try_done;

			/* This is a module name-style package, so symbol must
			 * be exported by that module.  Get the module record,
			 * exportonly to make it easier to clean up on failure.
			 */

			if ((rc = ldr_get_module_record(context, pkg->lp_name,
							flags, &dep_mod)) != LDR_SUCCESS)
				return(rc);

			break;

		      case try_lpt:
						
			next_loop = try_private_kpt;

			/* Look up in loaded package table */

			if ((rc = ldr_kpt_lookup(context->lc_lpt, pkg, &kpte)) !=
			    LDR_SUCCESS)
				continue;

			/* Found package in loaded package table */

			pkg_found = 1;
			dep_mod = kpte->lkp_module;

			break;

		      case try_private_kpt:

			next_loop = try_global_kpt;

			if ((context->lc_private_kpt == NULL) ||
			    ((rc = ldr_kpt_lookup(context->lc_private_kpt->lkh_kpt,
						  pkg, &kpte)) != LDR_SUCCESS) )
				continue;

			/* Found installed module record; need to get real
			 * module record now.
			 */

			pkg_found = 1;
			if ((rc = ldr_get_module_record(context, kpte->lkp_module->lm_name,
							flags, &dep_mod)) != LDR_SUCCESS)

				continue;

			break;

		      case try_global_kpt:

			next_loop = try_unres;

			if ((context->lc_global_kpt == NULL) ||
			    ((rc = ldr_kpt_lookup(context->lc_global_kpt->lkh_kpt,
						  pkg, &kpte)) != LDR_SUCCESS) )
				continue;

			/* Found installed module record; need to get real
			 * module record now.
			 */

			pkg_found = 1;
			if ((rc = ldr_get_module_record(context, kpte->lkp_module->lm_name,
							flags, &dep_mod)) != LDR_SUCCESS)

				continue;
				
			break;

		      case try_unres:

			next_loop = try_done;

			/* Symbol was unresolved -- either the package
			 * was unknown, or the module exporting the
			 * package didn't supply this symbol.  In
			 * either case, we have a problem.  Call a
			 * machine-dependent routine to try to do
			 * something with this symbol.  If the
			 * machine-dependent routine returns success,
			 * assume that it managed to cobble up a value
			 * for the symbol somehow, and use it;
			 * otherwise, bomb out.  (If we were told to
			 * disallow any unresolved symbols, don't even
			 * try the machine-dependent routine, just
			 * give up now).
			 */

			if (flags & LDR_NOUNREFS)
				continue;
#ifdef NOTDEF
			if ((rc = ldr_handle_unres(context, pkg, sym, flags)) != LDR_SUCCESS)
			    continue;
			break;
#else /* NOTDEF */
			continue;
#endif /* NOTDEF */

		}

		/* Have a module to try; look up symbol in it */

		if ((rc = LSW_LOOKUP_EXPORT(dep_mod, pkg, sym)) == LDR_SUCCESS) {

			/* Got it; fill in exporting package and return */

			sym->ls_module = dep_mod;
			return(LDR_SUCCESS);
		}

		/* Not found; try next loop */
	}

	if (!pkg_found) {

		ldr_log("ldr_resolve_symbol: error looking up package %s: %E\n", pkg->lp_name, rc);
		rc = LDR_ENOPKG;
	} else {
		ldr_log("ldr_resolve_symbol: error looking up package %s symbol %s: %E\n",
			pkg->lp_name, sym->ls_name, rc);
	}
		
	return(rc);
}



void
ldr_precompute_imports(ldr_module_rec *mod)

/* Precompute the absolute values of the symbols in the specified module's
 * import symbol table.  This routine deals with converting region-relative
 * symbol values to absolute symbol values, or (theoretically) with other
 * machine-dependent computations.  It is not an error for this routine to
 * do nothing; any symbol that can't be precomputed is assumed to be
 * handled by the format-dependent manager during relocation, possibly
 * in a machine-dependent way.
 */
{
	ldr_package_rec		*pkg;
	ldr_symbol_rec		*sym;
	ldr_module_rec		*smod;
	int			symno;

	for_all_imports(mod, symno, pkg, sym) {

		/* If it's already absolute, don't do anything */

		if (!ldr_symval_is_abs(&sym->ls_value)) {

			/* Try to convert to absolute; no error on failure */

			smod = (ldr_module_rec *)(sym->ls_module);
			ldr_symval_cvt_abs(&sym->ls_value, smod->lm_regions,
					   smod->lm_region_count);

		}
	}
}
