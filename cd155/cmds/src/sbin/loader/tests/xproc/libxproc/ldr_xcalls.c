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
/* ldr_xcalls.c
 * Implementations of the exported loader system calls
 *
 * This file contains the implementations of the loader system calls
 * exported to clients of the loader.  In general these routines do
 * not know the internal representations of loader data structures.
 * This file implements the cross-process loader routines; ldr_syscalls.c
 * implements the routines which operate on the current process.
 *
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ldr_xcalls.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:45:41 $";
#endif

#include <sys/types.h>
#include <sys/errno.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>


#define	RETURN(rc)	return((rc < 0) ? -ldr_status_to_errno(rc) : rc)

/* Functions for information and debugging */


int
ldr_next_module(ldr_process_t process, ldr_module_t *mod_id_ptr)

/* Iterator through the module IDs for all modules currently loaded
 * in the specified process.  To initialize the iterator, set
 * *mod_id_ptr to LDR_MODULE_NULL.  The next call to this routine
 * should be made using the handle returned from this call and so on.
 * After the last module, the handle returned is the LDR_NULL_MODULE.
 * On success the call returns a zero, on failure it returns a
 * negative error number.
 */
{
	int	rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_next_module(ldr_process_context, mod_id_ptr);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_next_module(mod_id_ptr);
	} else {
		rc = ldr_xproc_next_module(process, mod_id_ptr);
	}
	RETURN(rc);
}


int
ldr_inq_module(ldr_process_t process, ldr_module_t mod_id,
	       ldr_module_info_t *info, size_t info_size, size_t *ret_size)

/* Return module information about the module with the specified ID
 * in the specified process, into the info buffer supplied by the
 * caller.  info_size is the size of the buffer provided.  Returns the
 * actual size of the returned structure in *ret_size.  Return 0 on success,
 * or a negative error status on error.
 */
{
	int	rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_inq_module(ldr_process_context, mod_id,
					    info, info_size, ret_size);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_inq_module(mod_id, info, info_size, ret_size);
	} else {
		rc = ldr_xproc_inq_module(process, mod_id, info, info_size,
					  ret_size);
	}
	RETURN(rc);
}


int
ldr_inq_region(ldr_process_t process, ldr_module_t mod_id, ldr_region_t region,
	       ldr_region_info_t *info, size_t info_size, size_t *ret_size)

/* Return module information about the specified region of the
 * module with the specified ID in the specified process, into the
 * info buffer supplied by the caller.  info_size is the size of the
 * buffer provided.  Returns the actual size of the returned structure in
 * *ret_size.  Return 0 on success, or a negative error status on error.
 */
{
	int	rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_inq_region(ldr_process_context, mod_id,
					    region, info, info_size, ret_size);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_inq_region(mod_id, region, info, info_size, ret_size);
	} else {
		rc = ldr_xproc_inq_region(process, mod_id, region, info,
					  info_size, ret_size);
	}
	RETURN(rc);
}

/* Functions for cross-load operations */

int
ldr_xattach(ldr_process_t process)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = 0;
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_ipc_connect_to_server();
	} else {
		rc = ldr_xproc_attach(process);
	}
	RETURN(rc);
}

int
ldr_xdetach(ldr_process_t process)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = 0;
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_ipc_disconnect_from_server();
	} else {
		rc = ldr_xproc_detach(process);
	}
	RETURN(rc);
}

int
ldr_xsync(ldr_process_t process)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = 0;
	} else if (process == ldr_kernel_process()) {
		rc = 0;
	} else {
		rc = ldr_xproc_sync(process);
	}
	RETURN(rc);
}

int
ldr_xload(ldr_process_t process, char *file_pathname,
	  ldr_load_flags_t load_flags, ldr_module_t *mod_id_ptr)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_load(ldr_process_context, file_pathname, load_flags,
				      mod_id_ptr);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_load(file_pathname, load_flags, mod_id_ptr);
	} else {
		rc = ESRCH;
	}
	RETURN(rc);
}

int
ldr_xunload(ldr_process_t process, ldr_module_t mod_id)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_unload(ldr_process_context, mod_id);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_unload(mod_id);
	} else {
		rc = ESRCH;
	}
	RETURN(rc);
}

int
ldr_xentry(ldr_process_t process, ldr_module_t mod_id,
	   ldr_entry_pt_t *entry_ptr)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_get_entry_pt(ldr_process_context,
					   mod_id, entry_ptr);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_entry(mod_id, entry_ptr);
	} else {
		rc = ESRCH;
	}
	RETURN(rc);
}

int
ldr_xlookup(ldr_process_t process, ldr_module_t mod_id,
	    char *symbol_name, void **symbol_addr_ptr)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_lookup(ldr_process_context, mod_id,
					symbol_name, symbol_addr_ptr);
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_lookup(mod_id, symbol_name,
				       symbol_addr_ptr);
	} else {
		rc = ESRCH;
	}
	RETURN(rc);
}

int
ldr_xlookup_package(ldr_process_t process, char *package_name,
	char *symbol_name, void **symbol_addr_ptr)
{
	int rc;

	if (process == ldr_my_process()) {
		rc = ldr_context_lookup_package(ldr_process_context,
						package_name,
						symbol_name,
						symbol_addr_ptr); 
	} else if (process == ldr_kernel_process()) {
		rc = kls_client_lookup_package(package_name,
					       symbol_name,
					       symbol_addr_ptr);
	} else {
		rc = ESRCH;
	}
	RETURN(rc);
}


ldr_process_t
ldr_my_process(void)

/* Return the loader_process_t for the current process.  Can't fail.
 */
{
	return((ldr_process_t)getpid());
}


ldr_process_t
ldr_kernel_process(void)

/* Return the loader_process_t for the kernel.  Can't fail.
 */
{
	return((ldr_process_t)(-1));
}
