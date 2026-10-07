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
 * ldr_kernel_bootstrap.c
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

#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_switch.h"

#include "kernel_loader.h"
#include "ldr_kernel_main.h"


/* Forward declarations */

/* Object File Format Independent */
static int get_static_dep(ldr_module_handle handle, int depno, char **dep);
static int get_imports(ldr_module_handle handle, int *pkg_count,
		       ldr_package_rec **pkgs, int *sym_count,
		       ldr_symbol_rec **imports);
static int relocate(ldr_module_handle handle, int nregions,
		    ldr_region_rec regions[], int npackages,
		    ldr_package_rec import_pkgs[], int nimports,
		    ldr_symbol_rec imports[]);

static int cleanup(ldr_module_handle handle);

/* Declarations for kernel loader switch */

#include "ldr_kernel_machdep.h"

/*
 * Bootstrap the kernel loader system.  Includes building the kernel loader
 * context, containing the module and region records and exported symbol
 * list for the kernel itself.  The kernel argument is the name of
 * the kernel's object module, for building the module record.
 *
 * On error, we don't even try to free data structures; if the kernel
 * can't be bootstrapped, the kernel load server is going to exit
 * quickly anyway.
 */
int
ldr_kernel_bootstrap(char * kernel)
{
	ldr_context_t		loc_ctxt;
	ldr_module_t		mod_id;
	int			i;
	int			rc;

	if ((rc = ldr_context_create(LDR_NMODULES, alloc_abs_kernel_region,
				     alloc_rel_kernel_region, 
				     dealloc_kernel_region, &loc_ctxt)) < 0)
		return(rc);

	/*
	 * Temporarily insert the fake kernel loader switch entries
	 * in the context, while doing the fake load of the kernel
	 * module itself.  Note that the entries are added to the
	 * head of the loader switch, so they will take precedence
	 * over all other managers.
	 */

	for (i = 0; i < n_kernel_lsw_entries; i++)
		(void)ldr_switch_ins_head(loc_ctxt, (ldr_switch_t)(&kernel_loader_switch[i]));

	/* Now do the fake load of the module */

	if ((rc = ldr_context_load(loc_ctxt, kernel,
				  (LDR_NOUNLOAD|LDR_NOINIT), &mod_id)) < 0)
		return(rc);

	/*
	 * Fake load complete; now remove the dummy entries */

	for (i = 0; i < n_kernel_lsw_entries; i++)
		ldr_switch_rem_head(loc_ctxt);

	ldr_kernel_context = loc_ctxt;
	rc = LDR_SUCCESS;

	return(rc);
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
relocate(ldr_module_handle handle, int nregions,
	 ldr_region_rec regions[], int npackages,
	 ldr_package_rec import_pkgs[], int nimports,
	 ldr_symbol_rec imports[])

/* Dummy */
{
	return(LDR_SUCCESS);
}

static int
cleanup(ldr_module_handle handle)

/* Nothing to do */
{
	return(LDR_SUCCESS);
}
