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
/* ldr_preload.c
 * Routines and format-dependent manager for preloaded libraries
 *
 * Pre-loading is a technique for improving startup time for typical
 * programs.  Commonly-used libraries (such as the C library) may be
 * pre-loaded into a reserved portion of the address space.  All references
 * in a pre-loaded library must be fully resolved.
 *
 * The relocated text and data of pre-loaded libraries are copied into a
 * separate preload file when the data file is created.  A pre-loaded
 * library is loaded into an application via the special pre-load format-
 * dependent manager, by mapping the preload file into the application's
 * address space.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <strings.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "open_hash.h"
#include "squeue.h"
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
#include "ldr_preload.h"


#define	PD_MAGIC	0xf9f3e7cf	/* magic number for preload desc */

/* The following static variables are used only when the preload descriptor is
 * read-only (during load of a preloaded module).
 */

static ldr_preload_desc	*preload_ptr;	/* ptr to preload desc. in data file */
static int		preload_refcount; /* refcount for mapped region */

/* The following static variables are used only when there is no current preload
 * descriptor, during preloading.
 */

static void		*preload_nextva; /* next virt. addr when preloading */
static void		*preload_baseva; /* base virt. addr when preloading */

#define	round(x, s)	(((unsigned)(x) + (unsigned)(s) - 1) & ~((unsigned)(s) - 1))
#define va2off(va, ba)	((char *)(va) - (char *)(ba))
#define off2va(off, ba)	((off) + (char *)(ba))

/* The "format-dependent" handle for a preloaded module is simply a module
 * record, copied out into the global data file heap.  We store the export
 * symbol table for the module in the format-dependent handle of the
 * copied out module record.  It's an open hash table, hashed by symbol
 * name.  The symbol hashes to a list of records of all the packages
 * (usually there will be only one) that export the symbol.
 */

typedef struct export_pkg_rec {
	struct export_pkg_rec	*ep_next; /* forward link; must be first */
	int			ep_pkgno; /* export package number */
	ldr_symval		ep_value; /* symbol value */
} export_pkg_rec;

typedef open_hashtab_t preload_exports_t;


/* Forward references */

static int int_module_unload(ldr_module_rec *module, int region_count);
static int region_copy_data(ldr_region_rec *region, ldr_file_t fd,
			    ldr_preload_desc *pr_ptr);
static int exports_copy(ldr_module_rec *mod, ldr_module_rec *new_mod,
			ldr_heap_t heap);
static void free_exports(int export_count, ldr_symbol_rec *exports);
static int build_export_hash(ldr_heap_t heap, int export_count,
			     ldr_symbol_rec *exports, preload_exports_t *hashtab);

static int preload_recog(const char *filename, ldr_file_t fd,
			     ldr_module_handle *handle);
static int preload_get_static_dep(ldr_module_handle handle, int depno,
				      char **dep);
static int preload_get_imports(ldr_module_handle handle, int *pkg_count,
				   ldr_package_rec **pkgs, int *sym_count,
				   ldr_symbol_rec **imports);
static int preload_map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
				   int *reg_count, ldr_region_rec **regions);
static int preload_get_export_pkgs(ldr_module_handle handle, int *count,
				       ldr_package_rec **packages);
static int preload_get_exports(ldr_module_handle handle, int *sym_count,
				   ldr_symbol_rec **exports);
static int preload_lookup_export(ldr_module_handle handle, ldr_package_rec *package,
				     ldr_symbol_rec *symbol);
static int preload_relocate(ldr_module_handle handle, int nregions, ldr_region_rec *regions,
				int npackages, ldr_package_rec *import_pkgs, int nimports,
				ldr_symbol_rec *imports);
static int preload_get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt);
static int preload_run_inits(ldr_module_handle handle, entry_pt_kind kind);
static int preload_cleanup(ldr_module_handle handle);
static int preload_unload(ldr_module_handle handle, ldr_region_allocs *allocsp,
			      int reg_count, ldr_region_rec *regions,
			      int ipkg_count, ldr_package_rec *import_pkgs,
			      int import_count, ldr_symbol_rec *imports,
			      int epkg_count, ldr_package_rec *export_pkgs);


/* The loader switch entry for the preload manager */

const struct loader_switch_entry preload_switch_entry = {
	LSW_VERSION,
	LSF_NONE,
	preload_recog,
	preload_get_static_dep,
	preload_get_imports,
	preload_map_regions,
	preload_get_export_pkgs,
	preload_get_exports,
	preload_lookup_export,
	preload_relocate,
	preload_get_entry_pt,
	preload_run_inits,
	preload_cleanup,
	preload_unload,
};


/* Entry points for preload file creation and management */

int
ldr_preload_init(ldr_preload_desc *pr_ptr, ldr_heap_t heap)

/* Initialize the specified preload descriptor.  Initially there is
 * no preload context, and so no base address or data file.
 * This routine is only to be called from ldr_global_file_init
 * during global data file creation.
 */
{
	struct 	addressconf	*addr_conf;
	int	rc;

	pr_ptr->pd_magic  = PD_MAGIC;
	pr_ptr->pd_context = NULL;
	if (preload_baseva == NULL) {
		if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
			return(rc);
		preload_baseva = preload_nextva = addr_conf[AC_LDR_PRELOAD].ac_base;
	}
	pr_ptr->pd_baseaddr = preload_baseva;
	pr_ptr->pd_fname = NULL;
	pr_ptr->pd_size = 0;
	pr_ptr->pd_heap = heap;

	return(LDR_SUCCESS);
}


int
ldr_preload_copyout(ldr_preload_desc *pr_ptr, ldr_context *context)

/* Copy out the specified loader context to the preload file,
 * using the preload descriptor initialized by the previous call to
 * preload_init.  First, get a unique preload data file name and
 * create the preload data file.  (The preload data file is always
 * created in the same directory as ldr_global_data_file).  Next, copy
 * out the context and its consitutent module records (and their contents)
 * to the heap in the global file.  Then walk the copied context
 * and modules, copying the data from its current location (mapped
 * into the process' VA space) into the preload data file, and fixing
 * up the virtual and mapped addresses in the region records.
 * Finally, close the preload data file.
 * Returns LDR_SUCCESS on success, negative error status on error.
 * If we fail, we leave the preload data file in an indeterminate state.
 * This routine is only to be called from ldr_global_file_init
 * during global data file creation.
 */
{
	ldr_file_t		fd;
	char			*fname;
	ldr_context		*new_context;
	ldr_module_rec		*mod;
	ldr_module_rec		*new_mod;
	ldr_region_rec		*reg;
	int			regno;
	ldr_package_rec		*pkgs;
	int			pkg_count;
	int			rc;

	/* First, need to get the export package table for each module
	 * being copied out, if necessary (demand-loaded libraries don't
	 * normally have export package tables, since they're usually
	 * not needed).
	 */

	for_all_modules(context, mod) {

		if (mod->lm_export_pkgs == NULL) {

			if ((rc = LSW_GET_EXPORT_PKGS(mod, &pkg_count, &pkgs)) != LDR_SUCCESS)
				return(rc);

			lm_set_kpt_list(mod, pkg_count, pkgs, NULL);
		}
	}

	/* Create and open the preload data file, in the same directory
	 * as the ldr_global_data_file.
	 */

	if ((rc = ldr_maketemp(ldr_global_data_file, (S_IRUSR|S_IRGRP|S_IROTH),
			       &fd, &fname)) != LDR_SUCCESS)
		return(rc);

	/* Now copy the context and all its modules to the heap */

	if ((rc = ldr_context_copy(pr_ptr->pd_heap, context, &new_context)) != LDR_SUCCESS)
		return(rc);

	/* At this point we walk the known modules of the new and old
	 * contexts in parallel, to get the exported symbol tables.
	 * For each module, we get the exported symbols, build a hash
	 * table of them, and save the hash table as the "module handle"
	 * for the copy.
	 */

	for (mod = context->lc_known_modules.ll_forw,
	     new_mod = new_context->lc_known_modules.ll_forw;
	     mod != (ldr_module_rec *)&(context->lc_known_modules);
	     mod = mod->lm_forw) {

		/* NOTE overloading of NOUNLOAD flag here -- NOUNLOAD
		 * modules were not copied out to the global data file.
		 */

		if (mod->lm_load_flags & LDR_NOUNLOAD)
			continue;

		if ((rc = exports_copy(mod, new_mod, pr_ptr->pd_heap)) != LDR_SUCCESS)
			return(rc);

		new_mod = new_mod->lm_forw;
	}

	/* Now prepare to write region data to the preload file */

	for_all_modules(new_context, new_mod) {

		for_all_regions(new_mod, regno, reg) {

			if ((rc = region_copy_data(reg, fd, pr_ptr)) != LDR_SUCCESS)
				return(rc);
		}
	}

	/* Save remaining info in preload descriptor */

	pr_ptr->pd_fname = ldr_heap_strdup(pr_ptr->pd_heap, fname);
	(void)ldr_free(fname);
	pr_ptr->pd_size = round(((char *)preload_nextva - (char *)preload_baseva), ldr_getpagesize());
	pr_ptr->pd_context = new_context;
	return(LDR_SUCCESS);
}


int
ldr_preload_remove(ldr_preload_desc *desc)

/* Remove the current preload descriptor, so that future installs or
 * loads will not use the preload cache.  This is intended to be used
 * only during removal of the loader global file, as part of a new
 * global library install.  This includes deleting the old preload
 * file.  This is safe, because:
 *	- any process already having it mapped will hold it 
 *	- a process racing to open it (down in the recognizer) may
 *	  fail to open it, but that's OK -- it will just end up 
 *	  using the non-preloaded version of the library
 *	- no process will see the new preload file until the global
 *	  data file is atomically renamed into place
 */
{
	if (desc != preload_ptr)	/* garbage */
		return(LDR_EINVAL);

	if (preload_refcount != 0)	/* currently in use! */
		return(LDR_ETXTBSY);

	(void)ldr_unlink(preload_ptr->pd_fname); /* remove old preload file */

	preload_ptr = NULL;		/* no more preloading */
	return(LDR_SUCCESS);
}


int
ldr_preload_inherit(ldr_preload_desc *desc)

/* Inherit the specified preload descriptor.  Mostly just
 * includes error checking; also sets up the static data
 * used by the format-dependent manager.
 */
{
	int			rc;

	preload_refcount = 0;
	preload_ptr = NULL;
	if (desc == NULL || (desc->pd_context == NULL))	/* no preload desc */
		return(LDR_SUCCESS);

	if (desc->pd_magic != PD_MAGIC)
		return(LDR_EVERSION);

	/* Try to inherit the heap */

	if ((rc = ldr_heap_inherit(desc->pd_heap, LDR_PROT_READ)) != LDR_SUCCESS)
		return(LDR_EVERSION);

	/* Finally, inherit the context */

	if ((rc = ldr_context_inherit_ctxt(desc->pd_context)) != LDR_SUCCESS)
		return(LDR_EVERSION);

	/* Could be paranoid here and inherit the module records too,
	 * but with the current scheme they "cant' fail" (only thing
	 * to inherit is the open hash tables for the export symbol
	 * tables, and we know they're OK.
	 */

	/* OK; stash the pointers for later use by preload manager below */

	preload_ptr = desc;
	return(LDR_SUCCESS);
}


static int
region_copy_data(ldr_region_rec *region, ldr_file_t fd, ldr_preload_desc *pr_ptr)

/* Copy the data of the the specified region from its current mapped
 * address to the loader global data file, at the correct file offset.
 * In the process, "fix up" the region record so that mapaddr == vaddr
 * (where vaddr is the final address the region will run at).
 * Return LDR_SUCCESS on success, negative error status on error.
 */
{
	off_t			off;
	int			rc;

	off = va2off(region->lr_vaddr, pr_ptr->pd_baseaddr);
	if ((rc = ldr_lseek(fd, off, LDR_L_SET)) != off)
		return(rc);

	if ((rc = ldr_write(fd, region->lr_mapaddr, region->lr_size)) != region->lr_size)
		return(rc);

	region->lr_mapaddr = region->lr_vaddr;
	return(LDR_SUCCESS);
}


static int
exports_copy(ldr_module_rec *mod, ldr_module_rec *new_mod, ldr_heap_t heap)

/* Get the export symbol table for the specified module.  Build a hash
 * table (in the "standard" format for the pre-load manager) from the
 * export symbols, and save it in the format-dependent module handle of
 * the new (copied) module record.  Returns LDR_SUCCESS on success,
 * negative error status on error.
 */
{
	ldr_symbol_rec		*exports;
	int			export_count;
	int			symno;
	preload_exports_t	hashtab;
	int			rc;

	/* First, get the export symbol table */

	if ((rc = LSW_GET_EXPORTS(mod, &export_count, &exports)) != LDR_SUCCESS)
		return(rc);

	/* If no exports, just return a NULL handle */

	if (export_count == 0) {
		new_mod->lm_handle = NULL;
		goto free_exit;
	}

	/* Crunch the values here, to ensure that each symbol value
	 * is absolute.  It's an error if any symbol values can't
	 * be converted.
	 */

	for (symno = 0; symno < export_count; symno++) {

		/* If it's already absolute, don't do anything */

		if (!ldr_symval_is_abs(&exports[symno].ls_value))

			/* Try to convert to absolute; no error on failure */

			ldr_symval_cvt_abs(&exports[symno].ls_value,
					   mod->lm_regions, mod->lm_region_count);

		if (!ldr_symval_is_abs(&exports[symno].ls_value))
			return(LDR_ERANGE);
	}

	/* Build standard format hash table from the exports */

	if ((rc = build_export_hash(heap, export_count, exports, &hashtab)) != LDR_SUCCESS)
		goto free_exit;

	new_mod->lm_handle = (univ_t)hashtab;

free_exit:
	free_exports(export_count, exports);
	return(rc);
}


static void
free_exports(int export_count, ldr_symbol_rec *exports)

/* Free the specified export list.  First walk the list freeing the
 * components of the records (the name strings), then free the
 * list.
 */
{
	int		symno;

	for (symno = 0; symno < export_count; symno++)

		(void)ldr_free(exports[symno].ls_name);

	(void)ldr_symbols_free(export_count, exports);
}


static int
build_export_hash(ldr_heap_t heap, int export_count, ldr_symbol_rec *exports,
		  preload_exports_t *hashtab)

/* Build an open hash table from the specified list of export symbols.
 * The table hashes each symbol to a chain of the packages exporting
 * the symbol (usually there will be only one) and the value of
 * that (package, symbol) pair.  Build the table from the specified heap.
 */
{
	preload_exports_t	tab;
	export_pkg_rec		*rec;
	export_pkg_rec		*val;
	univ_t			key;
	int			symno;
	int			rc;

	if ((rc = open_hash_create_heap(heap, export_count,
					(ldr_hash_p)hash_string,
					(ldr_hash_compare_p)strcmp,
					(open_hash_flags_t)0,
					(open_hashtab_t *)(&tab))) != LDR_SUCCESS)
		return(rc);

	for (symno = 0; symno < export_count; symno++) {

		if ((rc = ldr_heap_malloc(heap, sizeof(export_pkg_rec),
					  MALLOC_T, (univ_t *)&rec)) != LDR_SUCCESS)
			return(rc);

		rec->ep_next = NULL;
		rec->ep_pkgno = exports[symno].ls_packageno;
		rec->ep_value = exports[symno].ls_value;

		key = ldr_heap_strdup(heap, exports[symno].ls_name);
		val = rec;
		if ((rc = open_hash_search(tab, key, (univ_t *)&val,
					   (LDR_HASH_LOOKUP|LDR_HASH_INSERT))) != LDR_SUCCESS)
			return(rc);

		if (val != rec)		/* name collision; chain new after old */
			sq1_ins_after(rec, val);
	}

	*hashtab = tab;
	return(LDR_SUCCESS);
}


/* Entry points for the pre-load format-dependent manager */

int
ldr_preload_entry(ldr_context_t ctxt)

/* The manager entry point is called with a pointer to a loader context.
 * It is responsible for linking its switch entry into the context's
 * switch (by calling ldr_switch_ins_tail()).  This procedure
 * allows dynamically-loaded auxiliary managers to be initialized in
 * the same way as statically-linked managers.
 */
{
	return(ldr_switch_ins_tail(ctxt, (ldr_switch_t)&preload_switch_entry));
}


static int
preload_recog(const char *filename, ldr_file_t fd, ldr_module_handle *handle)

/* The recognizer routine checks to see whether the specified file
 * is of an object file format supported by this format-dependent
 * manager.  It returns LDR_SUCCESS on success or a negative loader error
 * status on failure.  On success, the format-dependent manager's handle
 * is left in the handle variable.  Also, after a successful recognition,
 * the open file descriptor is the resposibility of the format-dependent
 * manager; it is never used again by the format-independent manager.
 *
 * For the preload manager, the "must-open" switch is OFF in the loader
 * switch, indicating that the file need not be opened before calling the
 * recognizer.  In this case the file descriptor will be LDR_NOFILE.
 * The preload recognizer simply looks up the pathname in the loader
 * context found via the preload descriptor.  If found, it immediately
 * maps all the regions of the library, from the preload data file,
 * into the process at the "correct" virtual address (if the region is
 * already mapped, it simply bumps the reference count on the region).
 * This is necessary to validate that mapping can succeed, since
 * otherwise we can't use the preloaded version, and want to use the 
 * normal version instead.  If the mapping succeeds, we return the
 * module record from the preload data as the handle.
 */
{
	struct lm_hash_entry	*lhe;	/* for chain hash lookup */
	ldr_module_rec		*module; /* preload module record */
	ldr_region_rec		*reg;	/* region record */
	ldr_file_t		preload_fd; /* fd for preload data file */
	univ_t			vaddr;
	int			regno;
	int			rc;

	if (preload_ptr == NULL)	/* no preload desc */
		return(LDR_ENOEXEC);

	if (chain_hash_lookup((chain_hashtab_t)preload_ptr->pd_context->lc_module_hash,
			      (const univ_t)filename,
			      (chain_hash_elem **)(&lhe)) != LDR_SUCCESS)
		return(LDR_ENOEXEC);
	module = lhe->lh_rec;	/* back ptr to record */

	/* Found the module.  Now try to map the entire preload data portion of
	 * the loader global data file, if it's not already mapped (if it is
	 * already mapped, just bump the refcount).
	 */

	if (preload_refcount == 0) {

		/* Need to map.  We map the whole preload data file here, for
		 * performance and to avoid potential problems with the user
		 * attempting to override the resolution of a symbol in
		 * a preloaded library.  We just map the whole area for
		 * write, since we want to map it all in one map call.
		 * (Could re-protect text for read later, but doesn't
		 * seem worthwhile now).
		 */

		if ((preload_fd = ldr_open(preload_ptr->pd_fname, LDR_O_RDONLY)) < 0 )
			return(LDR_ENOEXEC);

		rc = ldr_mmap(preload_ptr->pd_baseaddr, preload_ptr->pd_size,
			      LDR_PROT_READ|LDR_PROT_WRITE|LDR_PROT_EXEC,
			      LDR_MAP_FILE|LDR_MAP_PRIVATE|LDR_MAP_FIXED,
			      preload_fd, (off_t)0, &vaddr);
		(void)ldr_close(preload_fd);

		if (rc != LDR_SUCCESS)
			return(LDR_ENOEXEC);

	}
	preload_refcount++;

	/* Success.  Return module record as handle.  Close fd open on
	 * module, if it's open.
	 */

	if (fd != LDR_FILE_NONE)
		(void)ldr_close(fd);

	*handle = (univ_t)module;
	return(LDR_SUCCESS);
}

static int
preload_get_static_dep(ldr_module_handle handle, int depno, char **dep)

/* Iterator returing the pathnames of the static dependencies of the object
 * module with the specified format-dependent handle.  depno is the index of
 * the dependency to be found, starting at zero.  Return pointer to pathname
 * of static dependency (as a ldr_strdup'ed string; caller will ldr_free it)
 * in *dep.  Returns LDR_SUCCESS on success, a negative loader error
 * status on error (including LDR_EAGAIN to indicate the end of the
 * dependencies). 
 *
 * Static dependencies are not supported for pre-loaded libraries (it's not
 * impossible, but there's little justification for the added complexity).
 */
{
	return(LDR_EAGAIN);
}

static int
preload_get_imports(ldr_module_handle handle, int *pkg_count,
		    ldr_package_rec **pkgs, int *sym_count,
		    ldr_symbol_rec **imports)

/* Return the lists of import packages and import symbols for the
 * specified object module.  The callee allocates the lists and their
 * contents, and will be responsible for freeing them.  The callee must
 * fill in the following fields of each package record:
 *  - structure version number (compatibility check)
 *  - import package name
 *  - import package kind
 * and the following fields of each symbol record:
 *  - structure version number (compatibility check)
 *  - symbol name
 *  - import package number
 * Returns the number of packages in the package list in *pkg_count, and
 * the number of symbols in the import list in *sym_count.
 * Return LDR_SUCCESS on success or negative error status on error.
 *
 * Simply returns the import lists from the preload module record.
 */
{
	ldr_module_rec		*module = (ldr_module_rec *)handle;

	*pkg_count = module->lm_import_pkg_count;
	*pkgs = module->lm_import_pkgs;
	*sym_count = module->lm_import_count;
	*imports = module->lm_imports;
	return(LDR_SUCCESS);
}

static int
preload_map_regions(ldr_module_handle handle, ldr_region_allocs *allocsp,
		    int *reg_count, ldr_region_rec **regions)

/* Map the regions of the object file into the process' address space.
 * The callee allocates the list of mapped regions and its contents,
 * and will be responsible for freeing it.  The callee must fill in
 * these fields of the region descriptor record for each region mapped:
 *   - structure version number
 *   - region name (may be NULL if no region name is known)
 *   - region kind
 *   - region protection
 *   - the address it is to ultimately live at in the destination process'
 *     address space (vaddr)
 *   - the address it is currently mapped at in this process (mapaddr)
 *   - region size
 * 
 * allocsp is pointer to structure holding address allocation and deallocation
 * procedures to use; see ldr_types.h for description.
 * Returns the number of regions in the region list in *reg_count.
 * Return LDR_SUCCESS on success or negative error status on error.
 *
 * Just returns a pointer to the region list from the preloaded module.
 */
{
	ldr_module_rec		*module = (ldr_module_rec *)handle;

	*reg_count = module->lm_region_count;
	*regions = module->lm_regions;
	return(LDR_SUCCESS);
}

static int
preload_get_export_pkgs(ldr_module_handle handle, int *count,
			ldr_package_rec **packages)

/* Return the list of packages exported by this object module.  The
 * callee allocates the list and its contents, and will be responsible
 * for freeing it. The calle must fill in the following fields of each
 * package record:
 *  - structure version number
 *  - export package name
 *  - export package kind
 * Returns the number of exported packages in *count.
 * Return LDR_SUCCESS on success or negative error status on error.
 *
 * Just return a pointer to the export package list from the preloaded module.
 */
{
	ldr_module_rec		*module = (ldr_module_rec *)handle;

	*count = module->lm_export_pkg_count;
	*packages = module->lm_export_pkgs;
	return(LDR_SUCCESS);
}

static int
preload_get_exports(ldr_module_handle handle, int *sym_count,
		    ldr_symbol_rec **exports)

/* Return the list of exported symbols for the
 * specified object module.  The callee allocates the list and its
 * contents (the list MUST be allocated by calling ldr_symbols_create()),
 * but the CALLER is responsible for freeing the list.  The caller must
 * have previously called the get_export_pkgs_p call to get the list of
 * packages exported by this module.  The callee must
 * fill in the following fields of each symbol record:
 *  - structure version number (compatibility check)
 *  - symbol name
 *  - export package number (in previously-obtained export pkg list)
 *  - symbol value
 * Returns the number of symbols in the export list in *sym_count.
 * Return LDR_SUCCESS on success or negative error status on error.
 *
 * This routine is not called by the format-independent manager in normal
 * module loading.  It is intended for use only when pre-loading modules,
 * and possibly to allow format-dependent managers such as ELF to implement
 * their own symbol resolution algorithms.
 *
 * We don't implement this; just return error.
 */
{
	return(LDR_EINVAL);
}

static int
preload_lookup_export(ldr_module_handle handle, ldr_package_rec *package,
		      ldr_symbol_rec *symbol)

/* Look up the specified import symbol from the specified packge in
 * the specified object module, and fill in its value in the import
 * symbol record.  Can use the following fields in the import record:
 *  - symbol name
 * Must fill in the following fields in the import symbol record:
 *  - symbol value
 * Return LDR_SUCCESS on success or negative error status on error.
 *
 * We look up the export symbol in the open hash table of symbols,
 * which we stored in the loader handle in the copied-out module
 * record.
 */
{
	ldr_module_rec		*mod = (ldr_module_rec *)handle;
	export_pkg_rec		*rec;
	int			pkgno;
	char			*pkg_name;
	int			rc;

	if (mod->lm_handle == NULL)
		return(LDR_ENOSYM);

	if ((rc = open_hash_lookup((open_hashtab_t)mod->lm_handle,
				   symbol->ls_name, (univ_t *)&rec)) != LDR_SUCCESS)
		return(rc);

	/* Found a record.  Chain down list checking packages until match */

	if (rec->ep_pkgno >= mod->lm_export_pkg_count) /* shouldn't happen */
		return(LDR_ENOSYM);
	pkg_name = mod->lm_export_pkgs[rec->ep_pkgno].lp_name;

	for ( ; rec != NULL; rec = rec->ep_next) {

		if (strcmp(package->lp_name, pkg_name) == 0) {
			   

			/* Found.  Return value */

			symbol->ls_value = rec->ep_value;
			return(LDR_SUCCESS);
		}
	}

	return(LDR_ENOSYM);
}

static int
preload_relocate(ldr_module_handle handle, int nregions, ldr_region_rec *regions,
		 int npackages, ldr_package_rec *import_pkgs, int nimports,
		 ldr_symbol_rec *imports)

/* Relocate all the relocatable addresses everywhere in the specified
 * object module.  regions is the array of nregions region description
 * records describing the regions mapped from this object module, as
 * returned from the lsw_map_regions call.  import_pkgs and imports
 * are arrays on npackages package records and nimports import records
 * (respectively) describing the packages and symbols imported by this
 * object module, as returned by the lsw_get_imports call.  All
 * symbols have been resolved to a symbol value.  Return LDR_SUCCESS
 * on success or negative error status on error.
 *
 * Nothing to do here, since relocation has already been done for
 * preloaded modules.
 */
{
	return(LDR_SUCCESS);
}

static int
preload_get_entry_pt(ldr_module_handle handle, ldr_entry_pt_t *entry_pt)

/* Return the address of the entry point of the specified module, if
 * any, in *entry_pt.  Return LDR_SUCCESS on success or negative
 * error status on error.
 *
 * For now, preloaded libraries can't have entry points.  It's not
 * impossible, but it's not worth the work right now.
 */
{
	return(LDR_ENOMAIN);
}

static int
preload_run_inits(ldr_module_handle handle, entry_pt_kind kind)

/* Run the specified module's initialization or termination entry points,
 * as specified by the kind flag, if any.  Return LDR_SUCCESS on success
 * or negative error status on error.
 *
 * For now, the init routines are run at the time the library is
 * preloaded, not at the time the preloaded library is used.  This
 * could be changed, but it's not worth the effort right now.
 */
{
	return(LDR_SUCCESS);
}

static int
preload_cleanup(ldr_module_handle handle)

/* Complete the loading of the specified module, clean up open files,
 * temporary data structures, etc.  Return LDR_SUCCESS on success or
 * negative error status on error.
 *
 * There is no work to do in cleanup.
 */
{
	return(LDR_SUCCESS);
}

static int
preload_unload(ldr_module_handle handle, ldr_region_allocs *allocsp,
	       int reg_count, ldr_region_rec *regions,
	       int ipkg_count, ldr_package_rec *import_pkgs,
	       int import_count, ldr_symbol_rec *imports,
	       int epkg_count, ldr_package_rec *export_pkgs)

/* Unload the specified object module.  allocsp is pointer to
 * structure holding address allocation and deallocation procedures to
 * use; see ldr_types.h for description.  The region list describes
 * the address and size of each mapped region; the callee is
 * responsible for freeing this list.  The imports, import_pkgs, and
 * export_pkgs lists are also passed in to this procedure in case they
 * are needed during unloading; the callee is also responsible for
 * freeing them.  On return, the module handle, and the region,
 * import, import package, and export package lists are dead and
 * cannot be used further by the caller.
 *
 * Since the package, import, and region records are all simply pointers
 * into the global data file, we don't have to do anything with them.
 * All we need to do is to decrement the refcount on the data area,
 * and unmap it if we're the last user.
 */
{
	if (--preload_refcount == 0)
		return(ldr_munmap(preload_ptr->pd_baseaddr, preload_ptr->pd_size));
	else
		return(LDR_SUCCESS);
}


/* Region allocation routines for preload context */

int
preload_alloc_abs(univ_t vaddr, size_t size, ldr_prot_t prot, univ_t *baseaddr)

/* The alloc_abs_region_p procedure is called by the format-dependent
 * map_region routine to decide what base address to use in mapping an
 * absolute region.  Arguments are the virtual address at which the
 * region is relocated to run, the region size, and the protection for
 * the region.  On return, baseaddr is set to the best-guess starting
 * address at which the region is to be mapped; if baseaddr == vaddr
 * on return, the region is to be mapped using the LDR_MAP_FIXED flag,
 * otherwise, the baseaddr is just a hint to ldr_mmap.  Returns
 * LDR_SUCCESS on success, negative error status on error.
 *
 * The absolute allocator just returns an error for pre-loading -- we
 * can't preload a non-relocatable module.
 */
{
	return(LDR_ENOMEM);
}

int
preload_alloc_rel(size_t size, ldr_prot_t prot, univ_t *vaddr, univ_t *baseaddr)

/* The alloc_rel_region_p procedure is called by the format-dependent
 * map_region routine to decide what virtual address a relocatable
 * region is to be relocated to run at, and what base address to use
 * in mapping the region.  Arguments are the region size and the
 * protection for the region.  On return, vaddr is set to the address
 * to which the region is to be relocated, and baseaddr is set to the
 * best-guess starting address at which the region is to be mapped.
 * If vaddr == NULL on return, the region is to be relocated to run
 * at whatever address it ends up being mapped at.  In either case,
 * baseaddr is to be used as a hint to ldr_mmap.  Returns LDR_SUCCESS
 * on success, negative error status on error.
 *
 * The preload relative region allocator just allocates the next chunk
 * of space from the space reserved for the preload file and returns it
 * as the virtual address.  It returns the base of the standard mmap
 * region as the base address at which the region is to be mapped.
 * Note that it must be possible to call this procedure before
 * the loader global file is created.
 */
{
	struct 	addressconf	*addr_conf;
	int	pgsize = ldr_getpagesize();
	int	rc;

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);

	if (preload_nextva == NULL)	/* first call */

		preload_baseva = preload_nextva = addr_conf[AC_LDR_PRELOAD].ac_base;

	*baseaddr = addr_conf[AC_MMAP_DATA].ac_base;
	*vaddr = preload_nextva;
	(unsigned)preload_nextva += round(size, pgsize);
	return(LDR_SUCCESS);
}


int
preload_dealloc(univ_t vaddr, univ_t mapaddr, size_t size)

/* The dealloc_region_p procedure is the inverse to the
 * alloc_xxx_region_p procedures; it is called by the format-dependent
 * unmap_region routine to deallocate any storage allocated by either
 * of the alloc_xxx_region_p procedures.  The mapaddr argument is the
 * actual address to which the region was mapped.  The vaddr is the
 * virtual address to which the region was relocated to run (equal to
 * the vaddr passed in to alloc_abs_region_p, or the vaddr returned
 * from alloc_rel_region_p if non-NULL, or equal to the mapaddr if
 * alloc_rel_region_p returned a NULL vaddr).  The size is the
 * argument passed to alloc_region_p.  Returns LDR_SUCCESS on success
 * or negative error status on error.
 *
 * It doesn't have to do anything for preloaded libraries.
 */
{
	return(LDR_SUCCESS);
}
