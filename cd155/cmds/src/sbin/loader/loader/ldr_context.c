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
/* ldr_context.c
 * Routines for manager loader contexts and their contents
 *
 * Only the routines declared in ldr_main.h are intended to be
 * visible outside the internal format-independent loader.
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



/* Forward declarations */
static int create_context(ldr_heap_t heap, int nmodules, ldr_context **ctxt);
static ldr_module_t assign_module_id(ldr_context *context);



/* An empty module record for initialization */

static const ldr_module_rec initial_module_rec = {
	{ NULL, NULL },			/* chains */
	{ NULL, NULL, NULL },		/* hash entry */
	0,				/* module id */
	NULL,				/* switch */
	NULL,				/* handle */
	0,				/* region count */
	NULL,				/* regions */
	0,				/* import package count */
	NULL,				/* import packages */
	0,				/* import count */
	NULL,				/* imports */
	0,				/* export packages count */
	NULL,				/* export packages list */
	NULL,				/* lpt list */
	LDR_NOFLAGS,			/* load flags */
	LMF_NONE			/* flags */
};


/* An empty region record for initialization */

static const ldr_region_rec initial_region_rec = {
	LDR_REGION_VERSION,		/* version number */
	NULL,				/* name */
	0,				/* prot */
	NULL,				/* vaddr */
	NULL,				/* mapaddr */
	0,				/* size */
	LRF_NONE			/* flags */
	};


/* An empty package record for initialization */

static const ldr_package_rec initial_package_rec = {
	LDR_PACKAGE_VERSION,		/* version */
	ldr_package,			/* kind */
	NULL				/* name */
	};


/* An empty import record for initialization */

static const ldr_symbol_rec initial_symbol_rec = {
	LDR_SYMBOL_VERSION,		/* version */
	NULL,				/* name */
	-1,				/* package number */
	NULL				/* exporting module */
};


int
ldr_context_create(int nmodules, alloc_abs_region_p absp, alloc_rel_region_p
		   relp, dealloc_region_p deallocp, ldr_context_t *ctxt)

/* Allocate a loader context and initialize it.  Arguments are the expected
 * number of modules that will be loaded into the context, and the procedures
 * to be used for allocating and deallocating space for regions of modules
 * loaded into this context.  The context's loader switch is initialized,
 * by calling the entry point of each builtin manager and allowing it to
 * push its switch entry onto the context's switch.  The context is returned
 * unlocked. Returns LDR_SUCCESS on success or a negative loader status code
 * on error.
 */
{
	ldr_context		*context; /* internal representation */
	ldr_mgr_entry_p		mgr_ep;	/* manager entry point */
	int			mgrno;	/* manager number */
	int			rc;

	if ((rc = create_context(ldr_process_heap, nmodules, &context)) != LDR_SUCCESS)
		return(rc);

	context->lc_allocsp.lra_abs_alloc = absp;
	context->lc_allocsp.lra_rel_alloc = relp;
	context->lc_allocsp.lra_dealloc = deallocp;

	/* Now initialize loader switch, by calling each builtin manager.
	 * Note that the switch is built in the order of entries in the
	 * manager list.
	 */

	for (mgrno = 0; mgrno < n_ldr_mgr_entries; mgrno++) {

		mgr_ep = (ldr_mgr_entry_p)ldr_manager_entries[mgrno];
		(void)(*mgr_ep)((ldr_context_t)context);
	}

	*ctxt = context;

	return(LDR_SUCCESS);
}


static int
create_context(ldr_heap_t heap, int nmodules, ldr_context **ctxt)

/* Create and initialize a loader context from the specified heap.
 * Return LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_context		*context;
	ldr_module_hashtab	tab;	/* this context's hash table */
	ldr_kpt			lpt;	/* this context's loaded pkg table */
	int			rc;

	if ((rc = ldr_heap_malloc(heap, sizeof(*context), LDR_CONTEXT_T,
				  (univ_t *)&context)) != LDR_SUCCESS)
		return(rc);

	dq_init(&(context->lc_known_modules));
	dq_init(&(context->lc_exportonly_modules));
	context->lc_next_module_id = (ldr_module_t)(1);
	context->lc_dynmgr = LDR_NULL_MODULE;
	lsw_init(&context->lc_switch);

	if ((rc = chain_hash_create_heap(heap, nmodules, (ldr_hash_p)hash_string,
					 (ldr_hash_compare_p)strcmp,
					 (chain_hashtab_t *)&tab)) != LDR_SUCCESS) {
		ldr_heap_free(heap, context);
		return(rc);
	}

	context->lc_module_hash = tab;

	if ((rc = chain_hash_create_heap(heap, nmodules, (ldr_hash_p)hash_string,
					 (ldr_hash_compare_p)strcmp,
					 (chain_hashtab_t *)&lpt)) != LDR_SUCCESS) {

		chain_hash_destroy(context->lc_module_hash);
		ldr_heap_free(heap, context);
		return(rc);
	}
	context->lc_lpt = lpt;

	context->lc_global_kpt = NULL;	/* initialized when needed */
	context->lc_private_kpt = NULL;	/* initialized when needed */

	*ctxt = context;
	return(LDR_SUCCESS);
}


int
ldr_switch_ins_head(ldr_context_t ctxt, ldr_switch_t swtch)

/* Insert the specifed loader switch entry at the head of the loader
 * switch in the specified context.  Only a loader context created from
 * the default heap should ever have loader switch entries, as the
 * loader switch links are always allocated from the default heap.
 * This routine is mostly useful for special tasks such as
 * loader bootstrapping.  Returns LDR_SUCCESS on success or negative
 * error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	struct loader_switch_entry *lsw = (struct loader_switch_entry *)swtch;
	struct ldr_switch_links	*lsl;
	int			rc;
	
	if (lsw->lsw_version != LSW_VERSION) /* bad manager version */
		return(LDR_EVERSION);

	if ((rc = ldr_malloc(LDR_SWITCH_LINKS_T, sizeof(struct ldr_switch_links),
			     (univ_t)&lsl)) != LDR_SUCCESS)
		return(rc);
	dq_init(lsl);
	lsl->lsl_entry = lsw;

	lsw_ins_head(&context->lc_switch, lsl);
	return(LDR_SUCCESS);
}


void
ldr_switch_rem_head(ldr_context_t ctxt)

/* Remove the first loader switch entry from the head of the loader
 * switch in the specified context.
 * Used to undo the effects of a ldr_switch_ins_head.  Can't fail.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	
	lsw_rem_head(&context->lc_switch);
}


int
ldr_switch_ins_tail(ldr_context_t ctxt, ldr_switch_t swtch)

/* Insert the specifed loader switch entry at the tail of the loader
 * switch in the specified context.  Only a loader context created from
 * the default heap should ever have loader switch entries, as the
 * loader switch links are always allocated from the default heap.
 * This routine is called by each format-dependent manager during
 * manager initialization, to declare the manager's switch entries
 * to the format-independent manager.  Returns LDR_SUCCESS on success
 *  or negative error status on error.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	struct loader_switch_entry *lsw = (struct loader_switch_entry *)swtch;
	struct ldr_switch_links	*lsl;
	int			rc;
	
	if (lsw->lsw_version != LSW_VERSION) /* bad manager version */
		return(LDR_EVERSION);

	if ((rc = ldr_malloc(LDR_SWITCH_LINKS_T, sizeof(struct ldr_switch_links),
			     (univ_t)&lsl)) != LDR_SUCCESS)
		return(rc);
	dq_init(lsl);
	lsl->lsl_entry = lsw;

	lsw_ins_tail(&context->lc_switch, lsl);
	return(LDR_SUCCESS);
}


void
ldr_switch_rem_tail(ldr_context_t ctxt)

/* Remove the last loader switch entry from the tail of the loader
 * switch in the specified context.
 * Used to undo the effects of a ldr_switch_ins_tail.  Can't fail.
 */
{
	ldr_context		*context = (ldr_context *)ctxt;
	
	lsw_rem_tail(&context->lc_switch);
}


int
ldr_context_copy(ldr_heap_t heap, ldr_context *ctxt, ldr_context **new_ctxt)

/* Create a copy of the specified loader context from the specified heap.
 * The new loader context is allocated from the heap and initialized to a copy
 * of selected contents of the original.  Since this routine is intended to
 * be used only during pre-loading, only those fields important for preloading
 * are copied.  These are:
 *  the known module list
 *  the module name hash table
 * Note in particular that the copy will have no loaded or known
 * package tables, and no loader switch.
 * Returns LDR_SUCCESS on success or negative status on error.
 */
{
	ldr_context		*new_context;
	ldr_module_rec		*mod;
	ldr_module_rec		*new_mod;
	chain_hash_elem		*elem;
	int			rc;

	if ((rc = create_context(heap, LDR_NMODULES, &new_context)) != LDR_SUCCESS)
		return(rc);

	for_all_modules(ctxt, mod) {

		/* NOTE overloading of NOUNLOAD flag here -- we take it to
		 * mean that module should NOT be copied to new heap either.
		 * This is used eg. for the loader's own module record.
		 */

		if (mod->lm_load_flags & LDR_NOUNLOAD)
			continue;

		if ((rc = ldr_module_copy(heap, mod, &new_mod)) != LDR_SUCCESS)
			return(rc);

		elem = (chain_hash_elem *)&new_mod->lm_hash;
		if ((rc = chain_hash_insert((chain_hashtab_t)new_context->lc_module_hash,
					    elem)) != LDR_SUCCESS)
			return(rc);

		dq_ins_tail(&(new_context->lc_known_modules),
			    &(new_mod->lm_list));
		lm_flag_onlist(new_mod);
		lm_flag_loaded(new_mod);
	}

	*new_ctxt = new_context;
	return(LDR_SUCCESS);
}
		

int
ldr_context_inherit_ctxt(ldr_context *context)

/* Attempt to inherit the specified loader context, presumably from a
 * keep-on-exec region or mapped file.  Currently only does error
 * checking on the inherited components.
 */
{
	int	rc;

	if ((rc = chain_hash_inherit(context->lc_module_hash,
				     (ldr_hash_p)hash_string,
				     (ldr_hash_compare_p)strcmp)) != LDR_SUCCESS)
		return(rc);

	if ((rc = chain_hash_inherit(context->lc_lpt,
				     (ldr_hash_p)hash_string,
				     (ldr_hash_compare_p)strcmp)) != LDR_SUCCESS)
		return(rc);

	return(LDR_SUCCESS);
}


/* Routines for managing module and region records */


int
ldr_module_create(ldr_context *context, const char *name,
		  ldr_module_rec **mod)

/* Create a module record for the specified module name, and return it.
 * The module record is initialized and installed on the context's hash
 * chains, but is NOT yet linked into the known (or export-only) module list.
 * Returns LDR_SUCCESS on success or negative status on error.
 */
{
	ldr_module_rec		*module;
	int			rc;
	chain_hash_elem		*elem;

	if ((rc = ldr_malloc(sizeof(ldr_module_rec), LDR_MODULE_REC_T,
			     (univ_t *)&module)) != LDR_SUCCESS)
		return(rc);

	*module = initial_module_rec;	/* initialize it */
	dq_init(&module->lm_list);
	sq1_init(&module->lm_next);
	module->lm_rec = module;	/* back ptr to module record */
	if ((module->lm_name = ldr_strdup(name)) == NULL) {
		(void)ldr_free(module);
		return(LDR_EALLOC);
	}
	module->lm_module = assign_module_id(context);

	elem = (chain_hash_elem *)&module->lm_hash;
	if ((rc = chain_hash_insert((chain_hashtab_t)context->lc_module_hash,
				    elem)) != LDR_SUCCESS) {
		(void)ldr_free((univ_t)(module->lm_name));
		(void)ldr_free(module);
		return(rc);
	}

	*mod = module;
	return(LDR_SUCCESS);
}


void
ldr_module_destroy(ldr_context *context, ldr_module_rec *mod)

/* Free the specified module record.  Remove any packages exported by
 * this module from the context's loaded package table.  Dequeue it
 * from the module list it's on (if any), delete it from the
 * hash chains, and free it.
 */
{
	if (mod->lm_flags & LMF_ONLIST)
		dq_rem_elem(&mod->lm_list);
	(void)chain_hash_delete(context->lc_module_hash, mod->lm_name);
	(void)ldr_free(mod->lm_name);
	(void)ldr_free(mod);
}


int
ldr_module_copy(ldr_heap_t heap, ldr_module_rec *orig, ldr_module_rec **mod)

/* Create a copy of the specified module record from the specified heap.
 * The new module record is allocated from the heap and initialized to a copy
 * of the original (including copies of the original's package and region
 * tables, also allocated from the heap).
 * The copy is NOT installed on any hash chains or known module lists.
 * Returns LDR_SUCCESS on success or negative status on error.
 */
{
	ldr_module_rec		*module;
	ldr_region_rec		*regions;
	ldr_package_rec		*pkgs;
	ldr_symbol_rec		*imports;
	int			rc;

	if ((rc = ldr_heap_malloc(heap, sizeof(ldr_module_rec), LDR_MODULE_REC_T,
			     (univ_t *)&module)) != LDR_SUCCESS)
		return(rc);

	*module = *orig;		/* copy everything */
	dq_init(&module->lm_list);
	sq1_init(&module->lm_next);
	module->lm_rec = module;	/* back ptr to module record */
	if ((module->lm_name = ldr_heap_strdup(heap, orig->lm_name)) == NULL) {
		(void)ldr_heap_free(heap, module);
		return(LDR_EALLOC);
	}
	module->lm_switch = NULL;
	module->lm_handle = NULL;

	if (orig->lm_regions != NULL) {
		if ((rc = ldr_regions_copy(heap, orig->lm_region_count,
					  orig->lm_regions, &regions)) != LDR_SUCCESS)
			goto free_err;
		module->lm_regions = regions;
	}

#ifdef	NOTDEF
/* Nothing currently needs import packages or symbols in a module record
 * copy, so NULL them out.  This is how the code would look if they
 * were needed.
 */
	if (orig->lm_import_pkgs != NULL) {
		if ((rc = ldr_packages_copy(heap, orig->lm_import_pkg_count,
					   orig->lm_import_pkgs, &pkgs)) != LDR_SUCCESS)
			goto free_err;
		module->lm_import_pkgs = pkgs;
	}
	if (orig->lm_imports != NULL) {
		if ((rc = ldr_symbols_copy(heap, orig->lm_import_count,
					   orig->lm_imports, &imports)) != LDR_SUCCESS)
			goto free_err;
		module->lm_imports = imports;
	}
#else /* NOTDEF */
	module->lm_import_pkg_count = 0;
	module->lm_import_pkgs = NULL;
	module->lm_import_count = 0;
	module->lm_imports = NULL;
#endif /* NOTDEF */

	if (orig->lm_export_pkgs != NULL) {
		if ((rc = ldr_packages_copy(heap, orig->lm_export_pkg_count,
					   orig->lm_export_pkgs, &pkgs)) != LDR_SUCCESS)
			goto free_err;
		module->lm_export_pkgs = pkgs;
	}
	module->lm_kpt_list = NULL;

	*mod = module;
	return(LDR_SUCCESS);

free_err:
	(void)ldr_module_free_copy(heap, module);
	return(rc);
}


void
ldr_module_free_copy(ldr_heap_t heap, ldr_module_rec *mod)

/* Free the specified module record copy into the specified heap.
 * This includes freeings its region, import, and package table
 * copies as well.  Module must have been removed from the known
 * package table already.  Dequeue it from the module list it's on
 * (if any), free its contents, and free it.
 */
{
	if (mod->lm_flags & LMF_ONLIST)
		dq_rem_elem(&mod->lm_list);
	/* Not needed until we have pre-loading */
	(void)ldr_regions_free_copy(heap, mod->lm_region_count, mod->lm_regions);
	(void)ldr_packages_free_copy(heap, mod->lm_import_pkg_count,
				     mod->lm_import_pkgs);
	(void)ldr_symbols_free_copy(heap, mod->lm_import_count, mod->lm_imports);
	(void)ldr_packages_free_copy(heap, mod->lm_export_pkg_count,
				     mod->lm_export_pkgs);
	(void)ldr_heap_free(heap, mod->lm_name);
	(void)ldr_heap_free(heap, mod);
}


static ldr_module_t
assign_module_id(ldr_context *context)

/* Assign a module ID for the specified module that is unique within
 * the specified context.  Initially, we just count up starting at 1.
 * If we ever wrap, then we have to start checking for uniqueness (this
 * will "never happen", but it doesn't cost much to check).
 */
{
	ldr_module_t	mod_id;		/* module ID to be returned */

	if ((mod_id = ++(context->lc_next_module_id)) > 0)
		return(mod_id);

	/* Wrapped somehow.  For now we just panic in this case, as
	 * it's surely a bug that we actually got here.  However, if
	 * someone ever comes up with a system that stays up long
	 * enough to load 4 billion modules into a single context,
	 * this will have to be changed to iterate through the
	 * known module list and find an unused ID.
	 */

	ldr_msg("loader: module ID's wrapped, no way to continue\n");
	ldr_abort();
	/*NORETURN*/
}


int
translate_module_id(ldr_context *context, ldr_module_t mod_id,
			ldr_module_rec **module)

/* Translate the specified module ID into a pointer to the module
 * record for the module.  Return LDR_SUCCESS on success, LDR_EINVAL if the
 * module record is not found.
 */
{
	ldr_module_rec		*mod;	/* iterate through module records */

	for_all_modules(context, mod) {

		if (mod->lm_module == mod_id) {
			*module = mod;
			return(LDR_SUCCESS);
		}
	}

	return(LDR_EINVAL);
}


/* The following macros are prototypes for standard functions to allocate,
 * free, copy, and free copies of the standard loader data structures.
 */

/* The allocator function allocates space for a list of count elements
 * of the specified type, and initializes them to the contents of the
 * specified initializer record.  If count is 0, no allocation is
 * done; a NULL pointer is returned instead.
 */

#define	decl_struct_allocator(name, type, tag, vers, initializer) \
int \
name (int count, int version, type **retval) \
{ \
	type	*tmp; \
	int	i; \
	int	rc; \
\
	if (count == 0) { \
		*retval = NULL; \
		return(LDR_SUCCESS); \
	} \
\
	if (version != vers) \
		return(LDR_EVERSION); \
\
	if ((rc = ldr_malloc(count * sizeof(type), tag, (univ_t *)&tmp)) != LDR_SUCCESS) \
		return(rc); \
\
	for (i = 0; i < count; i++) /* initialize list */ \
		tmp[i] =  initializer; \
\
	*retval = tmp; \
	return(LDR_SUCCESS); \
}
		

/* The freer function frees a list as allocated by the allocator function.
 * NULL is a valid argument.
 */

#define decl_struct_freer(name, type) \
int \
name (int count, type *val) \
{ \
	if (count == 0) \
		return(LDR_SUCCESS); \
	return(ldr_free(val)); \
}


/* The copier function copies a list of the specified type into a list
 * allocated from a specified heap.  If count is 0, no allocation is
 * done; a NULL pointer is returned instead.
 */

#define decl_struct_copier(name, type, tag, field_copier) \
int \
name (ldr_heap_t heap, int count, type *orig, type **retval) \
{ \
	type	*tmp; \
	int	i; \
	int	rc; \
\
	if (count == 0) { \
		*retval = NULL; \
		return(LDR_SUCCESS); \
	} \
\
	if ((rc = ldr_heap_malloc(heap, count * sizeof(type), tag, \
				  (univ_t *)&tmp)) != LDR_SUCCESS) \
		return(rc); \
\
	for (i = 0; i < count; i++) { /* initialize list */ \
		tmp[i] = orig[i]; \
		field_copier(heap, &tmp[i]); \
	} \
\
	*retval = tmp; \
	return(LDR_SUCCESS); \
}


/* The copy freer frees a copy made by the copier function.
 * A NULL pointer is a valid argument type.
 */

#define decl_struct_copy_freer(name, type, field_freer) \
int \
name (ldr_heap_t heap, int count, type *val) \
{ \
	int	i; \
\
	if (count == 0) \
		return(LDR_SUCCESS); \
	for (i = 0; i < count; i++) \
		field_freer(heap, &val[i]); \
	return(ldr_heap_free(heap, val)); \
}


static void
copy_region_fields(ldr_heap_t heap, ldr_region_rec *reg)

/* Copy the fields of the specified region record copy into
 * storage allocated from the specified heap.
 */
{
	if (reg->lr_name != NULL)
		reg->lr_name = ldr_heap_strdup(heap, reg->lr_name);
}

static void
free_region_fields(ldr_heap_t heap, ldr_region_rec *reg)

/* Free the fields of the specified package record copy to
 * the specified heap.
 */
{
	if (reg->lr_name != NULL)
		(void)ldr_heap_free(heap, reg->lr_name);
}

static void
copy_package_fields(ldr_heap_t heap, ldr_package_rec *pkg)

/* Copy the fields of the specified package record copy into
 * storage allocated from the specified heap.
 */
{
	pkg->lp_name = ldr_heap_strdup(heap, pkg->lp_name);
}

static void
free_package_fields(ldr_heap_t heap, ldr_package_rec *pkg)

/* Free the fields of the specified package record copy to
 * the specified heap.
 */
{
	(void)ldr_heap_free(heap, pkg->lp_name);
}

static void
copy_symbol_fields(ldr_heap_t heap, ldr_symbol_rec *sym)

/* Copy the fields of the specified symbol record copy into
 * storage allocated from the specified heap.
 */
{
	sym->ls_name = ldr_heap_strdup(heap, sym->ls_name);
}

static void
free_symbol_fields(ldr_heap_t heap, ldr_symbol_rec *sym)

/* Free the fields of the specified symbol record copy to
 * the specified heap.
 */
{
	(void)ldr_heap_free(heap, sym->ls_name);
}


/* Standard function declarations for the ldr_region_rec type */

decl_struct_allocator(ldr_regions_create, ldr_region_rec, LDR_REGION_REC_T,
		      LDR_REGION_VERSION, initial_region_rec)
decl_struct_freer(ldr_regions_free, ldr_region_rec)
decl_struct_copier(ldr_regions_copy, ldr_region_rec, LDR_REGION_REC_T,
		   copy_region_fields)
decl_struct_copy_freer(ldr_regions_free_copy, ldr_region_rec,
		       free_region_fields)

/* Standard function declarations for the ldr_symbol_rec type */

decl_struct_allocator(ldr_symbols_create, ldr_symbol_rec, LDR_SYMBOL_REC_T,
		      LDR_SYMBOL_VERSION, initial_symbol_rec)
decl_struct_freer(ldr_symbols_free, ldr_symbol_rec)
decl_struct_copier(ldr_symbols_copy, ldr_symbol_rec, LDR_SYMBOL_REC_T,
		   copy_symbol_fields)
decl_struct_copy_freer(ldr_symbols_free_copy, ldr_symbol_rec,
		       free_symbol_fields)

/* Standard function declarations for the ldr_package_rec type */

decl_struct_allocator(ldr_packages_create, ldr_package_rec, LDR_PACKAGE_REC_T,
		      LDR_PACKAGE_VERSION, initial_package_rec)
decl_struct_freer(ldr_packages_free, ldr_package_rec)
decl_struct_copier(ldr_packages_copy, ldr_package_rec, LDR_PACKAGE_REC_T,
		   copy_package_fields)
decl_struct_copy_freer(ldr_packages_free_copy, ldr_package_rec,
		       free_package_fields)

