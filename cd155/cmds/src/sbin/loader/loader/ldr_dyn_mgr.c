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
/* ldr_dyn_mgr.c
 * Support for dynamic auxiliary format-dependent managers.
 *
 * This file contains the routines to support dynamically-loaded
 * auxiliary format-dependent managers.  When the builtin managers 
 * fail to recognize a module being loaded, ldr_get_module_record()
 * calls ldr_load_dyn_mgr() to attempt to load a dynamic manager.
 * The dynamic managers are simply listed in an ASCII text file;
 * they are loaded one by one until one recognizes the module or
 * all are loaded.
 *
 * Dynamic managers are loaded into their own loader context, to
 * reduce namespace pollution and avoid problems with using the
 * same manager in multiple contexts.  The dynamic manager context
 * is only created when the first dynamic manager is loaded.  It
 * is bootstrapped to contain all the symbols of the format-
 * independent loader, including loader utilities and so forth.
 * 
 * There are many restrictions on dynamic managers:
 *    they may not have exports
 *    they may only use symbols exported by the format-dependent manager
 *    they must be in a recognized format
 *
 * OSF/1 Release 1.0
 *
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

#include "ldr_lock.h"
#include "ldr_known_pkg.h"
#include "ldr_module.h"
#include "ldr_switch.h"
#include "ldr_symres.h"


/* The dynamic manager context.  All dynamic managers are loaded
 * into this context.  It is created when the first dynamic manager
 * is loaded.
 */

static ldr_context	*dyn_context;

/* The dynamic manager file.  It is mapped when the first attempt
 * is made to load a dynamic manager, and unmapped when all dynamic
 * managers have been loaded.
 */

static char		*dyn_file;
static char		*next_dyn_name;	/* next dyn mgr name to load */
static size_t		dyn_size;

/* Forward references */

static int init_dyn_context(void);
static int load_mgr(const char *dyn_name);
static int push_dyn_mgrs(ldr_context *context);
static int get_dyn_name(const char **dyn_name);


int
ldr_load_dyn_mgr(ldr_context *context)

/* Load the next dynamic manager, if any, and call its entry point
 * to get it to insert itself onto the specified context's loader
 * switch.
 */
{
	const char		*dyn_name; /* dynamic manager module name */	
	int			rc;

	if (dyn_context == NULL) {

		/* We haven't tried to load any managers yet, set things up */

		if ((rc = init_dyn_context()) != LDR_SUCCESS)
			return(rc);

	} else 	if (context->lc_dynmgr != dyn_context->lc_next_module_id) {

		/* There are currently-loaded dynamic managers that have not
		 * yet been added to this context's loader switch, so add them
		 * and return to caller.
		 */

		return(push_dyn_mgrs(context));
	}

	/* If there are any managers yet to be loaded, load one, add it
	 * to the context's switch, and return.  If the manager fails
	 * to load, keep trying until we get one loaded.
	 */

	do {
		if ((rc = get_dyn_name(&dyn_name)) != LDR_SUCCESS)
			return(rc);
		rc = load_mgr(dyn_name);
		(void)ldr_free((univ_t)dyn_name);
	} while (rc != LDR_SUCCESS);

	return(push_dyn_mgrs(context));
}


static int
init_dyn_context(void)

/* Initialize the dynamic manager context.  First, try to map the
 * dynamic manager database file.  If this succeeds, create the dynamic
 * manager context, and bootstrap it to contain the loader's exported
 * symbols.  Returns LDR_SUCCESS on success, negative error status
 * on error.
 */
{
	ldr_context_t		ctxt;
	ldr_file_t		fd;
	struct stat		buf;
	univ_t			mapaddr;
	int			rc;

	/* Try to get the dynamic manager database file mapped in */

	if ((fd = ldr_open(ldr_dyn_database, LDR_O_RDONLY)) < 0)
		return(fd);
	if ((rc = ldr_fstat(fd, &buf)) != LDR_SUCCESS)
		return(rc);

	rc = ldr_mmap(NULL, buf.st_size, LDR_PROT_READ, LDR_MAP_FILE|LDR_MAP_SHARED,
		      fd, (off_t)0, &mapaddr);
	(void)ldr_close(fd);
	if (rc != LDR_SUCCESS)
		return(rc);

	dyn_file = next_dyn_name = mapaddr;
	dyn_size = buf.st_size;

	/* Now create the dynamic manager context */

	if ((rc = ldr_context_create(LDR_NMODULES, alloc_abs_process_region,
				     alloc_rel_process_region, 
				     dealloc_process_region, &ctxt)) != LDR_SUCCESS)
		return(rc);

	/* Now, bootstrap the dynamic manager context to include the
	 * loader's symbol table.
	 */

	if ((rc = ldr_context_bootstrap(ctxt, "dummy_loader")) != LDR_SUCCESS)
		return(rc);

	dyn_context = (ldr_context *)ctxt;
	return(LDR_SUCCESS);
}


static int
load_mgr(const char *dyn_name)

/* Load the specified dynamic manager into the dynamic manager context.  This
 * is a stripped-down version of ldr_context_load(), specialized for loading
 * dynamic managers (we can't just call ldr_context_load() recursively
 * because there's no way to terminate the recursion, and because most
 * of the data structures are not prepared for it).  We assume that the
 * manager does not preexist in the context, and that it has no static
 * dependencies or unresolved symbols that do not resolve to the
 * standalone loader.  It also is assumed not to have any export packages
 * or symbols.
 * Return LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_module_rec			*mod;
	ldr_file_t			fd;
	ldr_module_handle		handle;
	struct loader_switch_entry	*lsw;
	int				regcount;
	ldr_region_rec			*regions;
	int				rc;

	ldr_lock_context(dyn_context);

	/* Get a module record, and try recognizing it */

	if ((rc = ldr_module_create(dyn_context, dyn_name, &mod)) != LDR_SUCCESS) {
		ldr_unlock_context(dyn_context);
		return(rc);
	}

	fd = LDR_FILE_NONE;		/* start with no file open */
	if ((rc = ldr_recognize(dyn_context, dyn_name, &fd, &handle, &lsw)) != LDR_SUCCESS) {
		ldr_module_destroy(dyn_context, mod);
		ldr_unlock_context(dyn_context);
		return(rc);
	}

	/* Recognized; fill in rest of module ID and link on context's list */

	mod->lm_switch = lsw;
	mod->lm_handle = handle;
	lm_set_load_flags(mod, LDR_NOINIT|LDR_NOUNREFS|LDR_NOUNLOAD);
	dq_ins_tail(&(dyn_context->lc_known_modules), &(mod->lm_list));
	lm_flag_loading(mod);
	lm_flag_onlist(mod);

	/* Next, do import symbol resolution */

	if ((rc = ldr_get_import_list(mod)) != LDR_SUCCESS)
		goto err_exit;

	if ((rc = ldr_resolve_imports(dyn_context, mod)) != LDR_SUCCESS)
		goto err_exit;

	/* Now load its regions, relocate it, and clean up */

	if ((rc = LSW_MAP_REGIONS(mod, &dyn_context->lc_allocsp, &regcount, &regions)) != LDR_SUCCESS)
		goto err_exit;
	lm_set_region_list(mod, regcount, regions);

	ldr_precompute_imports(mod);

	if ((rc = LSW_RELOCATE(mod, mod->lm_region_count, mod->lm_regions,
			       mod->lm_import_pkg_count, mod->lm_import_pkgs,
			       mod->lm_import_count, mod->lm_imports)) != LDR_SUCCESS)
		goto err_exit;

	if ((rc = LSW_CLEANUP(mod)) != LDR_SUCCESS)
		goto err_exit;
	lm_flag_loaded(mod);

	/* Module loading successful.  Unlock the context and return.
	 * NOTE: if we want to allow a dynamic manager to be in a format
	 * supported by another dynamic manager, call the newly-loaded
	 * manager's entry point here, passing it the dynamic context.
	 */

	ldr_unlock_context(dyn_context);
	return(LDR_SUCCESS);

	/* Load failed; unload the partially-loaded module here */

err_exit:
	ldr_log("load_mgr: error loading dynamic manager %s: %E\n", dyn_name, rc);
	(void)ldr_internal_module_unload(dyn_context, mod);
	ldr_unlock_context(dyn_context);
	return(rc);
}

static int
push_dyn_mgrs(ldr_context *context)

/* Iterate through the list of dynamic managers loaded into the dynamic
 * manager context.  For each manager, if the manager has not already
 * been added to the loader switch in the specified context, add it
 * (by calling the manager's entry point).  Returns LDR_SUCCESS on
 * success or negative error status on error.
 */
{
	ldr_module_rec		*mod;
	ldr_mgr_entry_p		entry;
	int			rc;

	for_all_modules(dyn_context, mod) {

		/* See if manager has already been added to switch */

		if (context->lc_dynmgr >= mod->lm_module)
			continue;

		/* No, add it */

		if ((rc = LSW_GET_ENTRY_PT(mod, (ldr_entry_pt_t *)&entry)) != LDR_SUCCESS)
			return(rc);

		if (entry == NULL)	/* can't have that... */
			continue;

		if ((rc = (*entry)(context)) != LDR_SUCCESS)
			return(rc);
	}

	/* Flag all managers as added */

	context->lc_dynmgr = dyn_context->lc_next_module_id;
	return(LDR_SUCCESS);
}


static int
get_dyn_name(const char **dyn_name)

/* Scan the dynamic manager database file to find the next manager name
 * to be loaded.  Format is one manager name per line; '#' at start of
 * line is comment character.  Return ldr_strdup'ed name in *dyn_name,
 * LDR_SUCCESS on success, negative error status on error (including
 * LDR_EAGAIN to indicate no more dynamic managers to load).
 */
{
	char		*p;
	char		*start, *fin;
	char		*str;
	int		rc;

	if (next_dyn_name == NULL)
		return(LDR_EAGAIN);

	for (p = next_dyn_name; p < (dyn_file + dyn_size); ) {

		if (*p == '#') {
			while ((p < (dyn_file + dyn_size)) &&
			       *p++ != '\n')
				;
			continue;
		}

		start = p;
		while ((p < (dyn_file + dyn_size)) &&
		       *p++ != '\n')
			;
		fin = next_dyn_name = p;

		if ((rc = ldr_malloc(fin - start, LDR_STRING_T, (univ_t)&str)) != LDR_SUCCESS)
			return(rc);

		bcopy(start, str, fin - start - 1);
		str[fin - start - 1] = '\0';
		*dyn_name = str;
		return(LDR_SUCCESS);
	}

	next_dyn_name = NULL;
	return(LDR_EAGAIN);
}
