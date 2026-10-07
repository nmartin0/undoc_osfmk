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

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <sys/types.h>
#include <sys/errno.h>
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

#include <nlist.h>

/*
 * Configuration Dependent Header Files
 */
#ifdef	USE_PTRACE
#include <sys/ptrace.h>
#endif
#ifdef	USE_VM_CALLS
#include <mach.h>
#endif

/*
 * General Macro Definitions
 */
#define	LOADER	"/sbin/loader"
#ifdef	USE_VM_CALLS
#define	NULL_SELF_TASK	((task_t)-1)
#endif
#ifdef	USE_PTRACE
#define	NBPI	sizeof(int)
#endif

#define	round(x, s) \
	(((unsigned)(x) + (unsigned)(s) - 1) & ~((unsigned)(s) - 1))
#define	trunc(x, s) \
	(((unsigned)(x)) & ~((unsigned)(s) - 1))
#define	add(a, b) \
	(((unsigned)(a)) + ((unsigned)(b)))
#define	subtract(a, b) \
	(((unsigned)(a)) - ((unsigned)(b)))

#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	FALSE
#define	FALSE	0
#endif

/*
 * Data Structures
 */
typedef struct xprocent {
	struct xprocent *x_next;
	pid_t            x_pid;
#ifdef	USE_VM_CALLS
	task_t		x_target_task;
#endif
	ldr_context     *x_ldr_process_context;
} xprocent;

static xprocent *xprocentlist;

#ifdef	USE_VM_CALLS
static task_t self_task = NULL_SELF_TASK;
#endif


/*
 *Function Prototypes
 */
static int  ldr_process_context_variable_address_get(void **);
static int  xproc_read(xprocent *, void *, void *, int, int);
static int  alloc(size_t, void **);
#ifdef	USE_VM_CALLS
static int  mach2unix(kern_return_t);
#endif

static int  ldr_process_context_get(xprocent *, ldr_context **);
static void ldr_process_context_destroy(ldr_context *);

static int  ldr_module_rec_get(xprocent *, ldr_module_rec *,
			       ldr_module_rec **);
static void ldr_module_rec_destroy(ldr_module_rec *);

static int  xpe_create(ldr_process_t);
static int  xpe_find(ldr_process_t, xprocent **);
static int  xpe_sync(xprocent *);
static void xpe_destroy(xprocent *);

/*
 * The Cross-Process Functions
 */
int
ldr_xproc_attach(ldr_process_t process)
{
	xprocent *xp;
	int       rc;

	if (xpe_find(process, &xp)) {
		/* didn't find an entry, so just create one */
		return(xpe_create(process));
	}

	/* found an entry, so just sync it */
	return(xpe_sync(xp));
}

int
ldr_xproc_detach(ldr_process_t process)
{
	xprocent *xp;
	int       rc;

	if (rc = xpe_find(process, &xp))
		return(rc);

	xpe_destroy(xp);

	return(0);
}

int
ldr_xproc_sync(ldr_process_t process)
{
	xprocent *xp;
	int       rc;

	if (rc = xpe_find(process, &xp))
		return(rc);

	return(xpe_sync(xp));
}

int
ldr_xproc_next_module(ldr_process_t process, ldr_module_t *mod_id_ptr)
{
	ldr_context_t context;
	xprocent     *xp;
	int           rc;

	if (rc = xpe_find(process, &xp))
		return(rc);

	context = (ldr_context_t)xp->x_ldr_process_context;

	return(ldr_context_next_module(context, mod_id_ptr));
}

int
ldr_xproc_inq_module(ldr_process_t process, ldr_module_t mod_id,
		     ldr_module_info_t *info, size_t info_size,
		     size_t *ret_size)
{
	ldr_context_t context;
	xprocent     *xp;
	int           rc;

	if (rc = xpe_find(process, &xp))
		return(rc);

	context = (ldr_context_t)xp->x_ldr_process_context;

	return(ldr_context_inq_module(context, mod_id, info, info_size,
				      ret_size));
}

int
ldr_xproc_inq_region(ldr_process_t process, ldr_module_t mod_id,
		     ldr_region_t region, ldr_region_info_t *info,
		     size_t info_size, size_t *ret_size)
{
	ldr_context_t context;
	xprocent     *xp;
	int           rc;

	if (rc = xpe_find(process, &xp))
		return(rc);

	context = (ldr_context_t)xp->x_ldr_process_context;

	return(ldr_context_inq_region(context, mod_id, region, info, info_size,
				      ret_size));
}

/*
 * Get Address of ldr_process_context Variable
 */
static int
ldr_process_context_variable_address_get(value)
	void **value;
{
	static void *ldr_process_context_variable_address = (void *)0;
	struct nlist nl[2];
	int          failed;

	if (ldr_process_context_variable_address) {
		*value = ldr_process_context_variable_address;
		return(0);
	}

#ifdef	PREFIX_UNDERSCORE
	nl[0].n_name = "_ldr_process_context";
#else
	nl[0].n_name = "ldr_process_context";
#endif
	nl[0].n_value = 0;
	nl[0].n_type = 0;

	nl[1].n_name = (char *)0;
	nl[1].n_value = 0;
	nl[1].n_type = 0;

#ifdef	USE_MACHO_NLIST
	failed = macho_nlist(LOADER, nl);
#else
	failed = nlist(LOADER, nl);
#endif

	if (failed || (!nl[0].n_type) || (!nl[0].n_value))
		return(-ENOENT);

	ldr_process_context_variable_address = (void *)nl[0].n_value;

	*value = ldr_process_context_variable_address;

	return(0);
}

/*
 * Cross-Process Read Functions
 */
#ifdef	USE_VM_CALLS
static int
xproc_read(xp, from, to, size_arg, string)
	xprocent *xp;
	void     *from, *to;
	int       size_arg;
{
	vm_address_t  address;
	vm_size_t     size;
	void         *data;
	unsigned      data_count, delta;
	kern_return_t error;
	
	address = (vm_address_t)trunc(from, vm_page_size);
	delta = subtract(from, address);
	size = (vm_size_t)round(add(size_arg, delta), vm_page_size);

	if ((error = vm_read(xp->x_target_task, address, size,
	    (pointer_t *)&data, &data_count)) != KERN_SUCCESS)
		return(-mach2unix(error));

	from = (void *)add(data, delta);

	if (string)
		(void)strncpy((char *)to, (char *)from, (size_t)size_arg);
	else
		(void)memcpy(to, from, (size_t)size_arg);

	(void)vm_deallocate(self_task, (vm_address_t)data,
	    (vm_size_t)data_count);

	return(0);
}
#endif	/* USE_VM_CALLS */

#ifdef	USE_PTRACE
static int
xproc_read(xp, from_arg, to_arg, size_arg, string)
	xprocent *xp;
	void     *from_arg, *to_arg;
	int       size_arg;
{
	int      count, value, rc, *address;
	char     *from, *to;
	size_t   size;
	unsigned i, delta;

	address = (int *)trunc(from_arg, NBPI);
	delta = subtract(from_arg, address);
	size = (size_t)round(add(size_arg, delta), NBPI);
	count =  size / NBPI;

	to = (char *)to_arg;
	i = delta;
	from = (char *)add(&value, i);

	if (string) {
		while (count--) {
			errno = 0;
			value = ptrace(PT_READ_D, xp->x_pid, address);
			if (errno)
				return(-errno);

			while (i < NBPI) {
				if (!(*to++ = *from++))
					return(0);
				i++;
			}

			i = 0;
			from = (char *)&value;
			address++;
		}
	} else {
		while (count--) {
			errno = 0;
			value = ptrace(PT_READ_D, xp->x_pid, address);
			if (errno)
				return(-errno);

			while (i < NBPI) {
				if (!(size--))
					return(0);
				*to++ = *from++;
				i++;
			}

			i = 0;
			from = (char *)&value;
			address++;
		}
	}

	return(0);
}
#endif	/* USE_PTRACE */

/*
 * Memory Allocation Function
 */
static int
alloc(size, pp)
	size_t size;
	void **pp;
{
	void *mem;

	if (mem = malloc(size)) {
		*pp = mem;
		return(0);
	}
	return(-ENOMEM);
}

/*
 * Loader Data Structure Functions
 */
#define	MAXMODULES	10000

static int
ldr_process_context_get(xp, value)
	xprocent        *xp;
	ldr_context   **value;
{
	ldr_context    *lcp = (ldr_context *)0;
	ldr_context    *context;
	ldr_module_rec *lmp;
	ldr_module_rec *module;
	ldr_module_rec *module_array[MAXMODULES];
	void           *variable_address;
	int            module_count = 0;
	int            i, rc;
	ldr_module_rec **prev_pointer;
	ldr_module_rec *prev_module;

	/* get address of the ldr_process_context variable */
	if (rc = ldr_process_context_variable_address_get(&variable_address))
		goto error;

	/* ldr_process_context is a pointer; get what it points to */
	if (rc = xproc_read(xp, variable_address, (void *)&context,
	    sizeof(context), FALSE))
		goto error;

	/* sanity check the context pointer, non-NULL for now */
	if (!context) {
		rc = -EFAULT;
		goto error;
	}

	/* get a local copy of the context */
	if (rc = alloc(sizeof(*lcp), (void **)&lcp))
		goto error;
	if (rc = xproc_read(xp, (void *)context,
	    (void *)lcp, sizeof(*lcp), FALSE))
		goto error;

	/* 
	 * get all the module records and update known module
	 * list pointers to our data structures
	 */
	prev_pointer = &lcp->lc_known_modules.ll_forw;
	prev_module = (ldr_module_rec *)&lcp->lc_known_modules;
	for (module = lcp->lc_known_modules.ll_forw;
	     module != (ldr_module_rec *)&context->lc_known_modules;
	     module = lmp->lm_forw) {

		if (rc = ldr_module_rec_get(xp, module, &lmp))
			goto error;

		/* save in module array; temporary in case we must abort */
		module_array[module_count++] = lmp;
		if (module_count >= MAXMODULES) {
			rc = -ENOMEM;
			goto error;
		}

		*prev_pointer = lmp;
		lmp->lm_back = prev_module;

		prev_pointer = &lmp->lm_forw;		
		prev_module = lmp;
	}
	*prev_pointer = (ldr_module_rec *)&lcp->lc_known_modules;
	lcp->lc_known_modules.ll_back = prev_module;

	/* clear unused fields */
	lcp->lc_switch.lss_forw = (struct ldr_switch_links *)0;
	lcp->lc_switch.lss_back = (struct ldr_switch_links *)0;
	lcp->lc_exportonly_modules.ll_forw = (struct ldr_module_rec *)0;
	lcp->lc_exportonly_modules.ll_back = (struct ldr_module_rec *)0;
	lcp->lc_module_hash = (ldr_module_hashtab)0;
	lcp->lc_lpt = (ldr_kpt)0;
	lcp->lc_global_kpt = (ldr_kpt_header *)0;
	lcp->lc_private_kpt = (ldr_kpt_header *)0;
	lcp->lc_allocsp.lra_abs_alloc = (alloc_abs_region_p)0;
	lcp->lc_allocsp.lra_rel_alloc = (alloc_rel_region_p)0;
	lcp->lc_allocsp.lra_dealloc = (dealloc_region_p)0;
	lcp->lc_dynmgr = (ldr_module_t)0;

	/* return value */
	*value = lcp;
	return(0);

error:
	if (module_count)
	 	for (i = 0; i < module_count; i++)
			ldr_module_rec_destroy(module_array[i]);
	if (lcp)
		free(lcp);
	return(rc);
}

static void
ldr_process_context_destroy(lcp)
	ldr_context *lcp;
{
	ldr_module_rec *lmp;

	for (lmp = lcp->lc_known_modules.ll_forw;
	     lmp != (ldr_module_rec *)&lcp->lc_known_modules;
	     lmp = lmp->lm_forw)
		ldr_module_rec_destroy(lmp);
	free(lcp);
}

static int
ldr_module_rec_get(xp, module, value)
	xprocent        *xp;
	ldr_module_rec  *module;
	ldr_module_rec **value;
{
	ldr_module_rec  *lmp = (ldr_module_rec *)0;
	char            *module_name = (char *)0;
	ldr_region_rec  *regions = (ldr_region_rec *)0;
	char           **region_names = (char **)0;
	int              i, size, rc;

	/* get the module record */
	if (rc = alloc(sizeof(*lmp), (void **)&lmp))
		goto error;
	if (rc = xproc_read(xp, (void *)module,
	    (void *)lmp, sizeof(*lmp), FALSE))
		goto error;
	lmp->lm_hash.lh_rec = lmp;

	/* get the module name */
	if (rc = alloc(PATH_MAX, (void **)&module_name))
		goto error;
	if (rc = xproc_read(xp, (void *)lmp->lm_hash.lh_name,
	    (void *)module_name, PATH_MAX, TRUE)) 
		goto error;
	lmp->lm_hash.lh_name = module_name;

	/* get region records */
	if (lmp->lm_region_count) {
		size = lmp->lm_region_count * sizeof(*regions);
		if (rc = alloc(size, (void **)&regions))
			goto error;
		if (rc = xproc_read(xp, (void *)lmp->lm_regions,
		    (void *)regions, size, FALSE))
			goto error;
		lmp->lm_regions = regions;

		/* get region record names */
		size = lmp->lm_region_count * sizeof(*region_names);
		if (rc = alloc(size, (void **)&region_names))
			goto error;
		for (i = 0; i < lmp->lm_region_count; i++)
			region_names[i] = (char *)0;
		for (i = 0; i < lmp->lm_region_count; i++) {
			if (rc = alloc(PATH_MAX, (void **)&region_names[i]))
				goto error;
			if (rc = xproc_read(xp, (void *)regions[i].lr_name,
			    (void *)region_names[i], PATH_MAX, TRUE))
				goto error;
			regions[i].lr_name = region_names[i];
		}
		free(region_names);
	} else
		lmp->lm_regions = (ldr_region_rec *)0;

	/* clear unused fields */
	lmp->lm_hash.lh_next = (struct lm_hash_entry *)0;
	lmp->lm_switch = (struct loader_switch_entry *)0;
	lmp->lm_handle = (univ_t)0;
	lmp->lm_import_pkg_count = 0;
	lmp->lm_imports = (ldr_symbol_rec *)0;
	lmp->lm_import_count = 0;
	lmp->lm_imports = (ldr_symbol_rec *)0;
	lmp->lm_export_pkg_count = 0;
	lmp->lm_export_pkgs = (ldr_package_rec *)0;
	lmp->lm_kpt_list = (ldr_kpt_rec	*)0;

	/* return the module record */
	*value = lmp;
	return(0);

error:
	if (region_names) {
		for (i = 0; i < lmp->lm_region_count; i++)
			if (region_names[i])
				free(region_names[i]);
		free(region_names);
	}
	if (regions)
		free(regions);
	if (module_name)
		free(module_name);
	if (lmp)
		free(lmp);
	return(rc);
}

static void
ldr_module_rec_destroy(lmp)
	ldr_module_rec *lmp;
{
	int i;

	for (i = 0; i < lmp->lm_region_count; i++)
		free(lmp->lm_regions[i].lr_name);
	free(lmp->lm_regions);
	free(lmp->lm_hash.lh_name);
	free(lmp);
}

/*
 * Cross-Process Data Structure Functions
 */
static int
xpe_create(process)
	ldr_process_t process;
{
	xprocent     *xp;
	pid_t         pid;
	int           rc;
#ifdef	USE_VM_CALLS
	kern_return_t error;
#endif


	/* allocate space for the entry */
	if (rc = alloc(sizeof(*xp), (void **)&xp))
		return(rc);

	/* get pid of target process */
	pid = (pid_t)process;
	xp->x_pid = pid;

#ifdef	USE_VM_CALLS
	/* get a port to ourself */
	if (self_task == NULL_SELF_TASK)
		self_task = task_self();

	/* get a port to the target task */
	if ((error = task_by_unix_pid(self_task, xp->x_pid,
	    &xp->x_target_task)) != KERN_SUCCESS) {
		if (error == KERN_PROTECTION_FAILURE)
			rc = -EACCES;
		else
			rc = -mach2unix(error);
		free(xp);
		return(rc);
	}
#endif

	/* get the ldr_process_context for the target process */
	if (rc = ldr_process_context_get(xp, &xp->x_ldr_process_context)) {
		free(xp);
		return(rc);
	}

	/* link into list */
	xp->x_next = xprocentlist;
	xprocentlist = xp;

	return(0);
}

static int
xpe_find(process, value)
	ldr_process_t process;
	xprocent    **value;
{
	xprocent *xp;
	pid_t     pid;
	int       rc;

	pid = (pid_t)process;
	for (xp = xprocentlist; xp; xp = xp->x_next)
		if (xp->x_pid == pid) {
			*value = xp;
			return(0);
		}
	return(-ESRCH);
}

static int
xpe_sync(xp)
	xprocent *xp;
{
	ldr_process_context_destroy(xp->x_ldr_process_context);

	return(ldr_process_context_get(xp, &xp->x_ldr_process_context));
}

static void
xpe_destroy(entry)
	xprocent *entry;
{
	xprocent *xp, **pp;
	int       rc;

	/* destroy the ldr_process_context */
	ldr_process_context_destroy(entry->x_ldr_process_context);

	/* remove entry from list */
	pp = &xprocentlist;
	for (xp = xprocentlist; xp; xp = xp->x_next) {
		if (xp == entry) {
			*pp = xp->x_next;
			break;
		}
		pp = &xp->x_next;
	}

	/* destroy the entry */
	free(entry);
}

#ifdef	USE_VM_CALLS
/*
 * Convert a Mach kern_return_t to a UNIX errno
 */
static int
mach2unix(mach_error)
	kern_return_t mach_error;
{
	int unix_errno;

	switch (mach_error) {

	default:
	case KERN_INVALID_ARGUMENT:
		unix_errno = EINVAL;
		break;

	case KERN_SUCCESS:
		unix_errno = 0;
		break;

	case KERN_INVALID_ADDRESS:
	case KERN_PROTECTION_FAILURE:
		unix_errno = EFAULT;
		break;

	case KERN_NO_SPACE:
	case KERN_RESOURCE_SHORTAGE:
		unix_errno = ENOMEM;
		break;

	case KERN_NOT_RECEIVER:
	case KERN_NO_ACCESS:
		unix_errno = EACCES;
		break;

	case KERN_FAILURE:
	case KERN_MEMORY_FAILURE:
	case KERN_MEMORY_ERROR:
	case KERN_ABORTED:
		unix_errno = EIO;
		break;

	case KERN_ALREADY_IN_SET:
	case KERN_NAME_EXISTS:
		unix_errno = EEXIST;
		break;

	case KERN_NOT_IN_SET:
		unix_errno = ENOENT;
		break;
	}

	return(unix_errno);
}
#endif	/* USE_VM_CALLS */

/*
 * Debugging Functions
 */
#ifdef	DEBUG
static
print_ldr_context(lcp)
	ldr_context *lcp;
{
	printf("ldr_context.lc_switch.lss_forw=%#x\n",
		lcp->lc_switch.lss_forw);
	printf("ldr_context.lc_switch.lss_back=%#x\n",
		lcp->lc_switch.lss_back);

	printf("ldr_context.lc_known_modules.ll_forw=%#x\n",
		lcp->lc_known_modules.ll_forw);
	printf("ldr_context.lc_known_modules.ll_back=%#x\n",
		lcp->lc_known_modules.ll_back);

	printf("ldr_context.lc_exportonly_modules.ll_forw=%#x\n",
		lcp->lc_exportonly_modules.ll_forw);
	printf("ldr_context.lc_exportonly_modules.ll_back=%#x\n",
		lcp->lc_exportonly_modules.ll_back);

	printf("ldr_context.lc_module_hash=%#x\n", lcp->lc_module_hash);
	printf("ldr_context.lc_lpt=%#x\n", lcp->lc_lpt);
	printf("ldr_context.lc_global_kpt=%#x\n", lcp->lc_global_kpt);
	printf("ldr_context.lc_private_kpt=%#x\n", lcp->lc_private_kpt);

	printf("ldr_context.lc_allocsp.lra_abs_alloc=%#x\n",
		lcp->lc_allocsp.lra_abs_alloc);
	printf("ldr_context.lc_allocsp.lra_rel_alloc=%#x\n",
		lcp->lc_allocsp.lra_rel_alloc);
	printf("ldr_context.lc_allocsp.lra_dealloc=%#x\n",
		lcp->lc_allocsp.lra_dealloc);

	printf("ldr_context.lc_next_module_id=%#x\n",
		lcp->lc_next_module_id);
	printf("ldr_context.lc_dynmgr=%#x\n", lcp->lc_dynmgr);
}

static
print_ldr_module_rec(lmp)
	ldr_module_rec *lmp;
{
	char buffer[1024];
	int  i;

	printf("ldr_module_rec.lm_list.le_forw=%#x\n",
		lmp->lm_list.le_forw);
	printf("ldr_module_rec.lm_list.le_back=%#x\n",
		lmp->lm_list.le_back);

	printf("ldr_module_rec.lm_hash.lh_next=%#x\n",
		lmp->lm_hash.lh_next);
	printf("ldr_module_rec.lm_hash.lh_name=\"%s\"\n",
		lmp->lm_hash.lh_name);
	printf("ldr_module_rec.lm_hash.lh_rec=%#x\n",
		lmp->lm_hash.lh_rec);

	printf("ldr_module_rec.lm_switch=%#x\n",
		lmp->lm_switch);
	printf("ldr_module_rec.lm_handle=%#x\n",
		lmp->lm_handle);

	printf("ldr_module_rec.lm_region_count=%d\n",
		lmp->lm_region_count);
	printf("ldr_module_rec.lm_regions=%#x\n",
		lmp->lm_regions);

	printf("ldr_module_rec.lm_import_pkg_count=%d\n",
		lmp->lm_import_pkg_count);
	printf("ldr_module_rec.lm_import_pkgs=%#x\n",
		lmp->lm_import_pkgs);

	printf("ldr_module_rec.lm_import_count=%d\n",
		lmp->lm_import_count);
	printf("ldr_module_rec.lm_imports=%#x\n",
		lmp->lm_imports);

	printf("ldr_module_rec.lm_export_pkg_count=%d\n",
		lmp->lm_export_pkg_count);
	printf("ldr_module_rec.lm_export_pkgs=%#x\n",
		lmp->lm_export_pkgs);

	printf("ldr_module_rec.lm_kpt_list=%#x\n",
		lmp->lm_kpt_list);
	printf("ldr_module_rec.lm_load_flags=%#x\n",
		lmp->lm_load_flags);
	printf("ldr_module_rec.lm_flags=%#x\n",
		lmp->lm_flags);

	for (i = 0; i < lmp->lm_region_count; i++) {
		sprintf(buffer, "ldr_module_rec.lm_regions[%d].", i);
		print_ldr_region_rec(buffer, &lmp->lm_regions[i]);
	}
}

print_ldr_region_rec(prefix, lrp)
	char *prefix;
	ldr_region_rec *lrp;
{
	printf("%slr_version=%d\n", prefix,
		lrp->lr_version);
	printf("%slr_name=\"%s\"\n", prefix,
		lrp->lr_name);
	printf("%slr_prot=%#x\n", prefix,
		lrp->lr_prot);
	printf("%slr_vaddr=%#x\n", prefix,
		lrp->lr_vaddr);
	printf("%slr_mapaddr=%#x\n", prefix,
		lrp->lr_mapaddr);
	printf("%slr_size=%d(%#x)\n", prefix,
		lrp->lr_size, lrp->lr_size);
	printf("%slr_flags=%#x\n", prefix,
		lrp->lr_flags);
}
#endif	/* DEBUG */
