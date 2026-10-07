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
/* ldr_syscalls.c
 * Implementations of the exported loader system calls
 *
 * This file contains the implementations of the loader system calls
 * exported to clients of the loader.  In general these routines do
 * not know the internal representations of loader data structures.
 * This file implements the routines which implicitly operate on the
 * current process; ldr_xcalls.c implements the cross-process routines.
 *
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ldr_syscalls.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:48 $";
#endif

#include <sys/types.h>
#include <sys/errno.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>


ldr_module_t
load(char *file_pathname, ldr_load_flags_t load_flags)

/* Load the specified module in the current process loader context,
 * and return its module ID.  Returns module ID on success, or 0
 * on failure with error number in errno.
 */
{
	ldr_module_t		mod_id;	/* module ID */
	int			rc;

	rc = ldr_context_load(ldr_process_context, file_pathname, load_flags,
			      &mod_id);
	if (rc != 0) {
		errno = ldr_status_to_errno(rc);
		return(0);
	}
	return(mod_id);
}


ldr_entry_pt_t
ldr_entry(ldr_module_t mod_id)

/* Look up and return the entry point of the loaded module named by the
 * specified module ID.  Returns the entry point on success, or NULL on error.
 */
{
	int			rc;
	ldr_entry_pt_t		entry;

	rc = ldr_context_get_entry_pt(ldr_process_context, mod_id, &entry);
	if (rc != 0) {
		errno = ldr_status_to_errno(rc);
		return(NULL);
	}
	return(entry);
}


int
unload(ldr_module_t mod_id)

/* Unload the loaded module named by the specified module ID.  Returns zero on
 * success, -1 on error.  NOTE: does no checking for possible references to
 * the module being unloaded.
 */
{
	int			rc;

	rc = ldr_context_unload(ldr_process_context, mod_id);
	if (rc != 0) {
		errno = ldr_status_to_errno(rc);
		return(-1);
	}
	return(0);
}


void *
ldr_lookup(ldr_module_t mod_id, char *symbol_name)

/* Look up the specified symbol in the symbols exported by the specified
 * module ID, and return the absolute value of the symbol.  On error,
 * return NULL with error status in errno.
 */
{
	int			rc;
	void			*value;

	rc = ldr_context_lookup(ldr_process_context, mod_id, symbol_name,
				&value);
	if (rc != 0) {
		errno = ldr_status_to_errno(rc);
		return(NULL);
	}
	return(value);
}


void *
ldr_lookup_package(char *package, char *symbol_name)

/* Look up the specified symbol in the symbols exported from the specified
 * package, and return the absolute value of the symbol.  On error,
 * return NULL with error status in errno.
 */
{
	int			rc;
	void			*value;

	rc = ldr_context_lookup_package(ldr_process_context, package,
					symbol_name, &value);
	if (rc != 0) {
		errno = ldr_status_to_errno(rc);
		return(NULL);
	}
	return(value);
}


int
ldr_install(const char *module_name)

/* Install the specified module in the current process' private known
 * package table.  The private KPT is inherited copy-on-write by
 * this process' children.  Returns 0 on success, or negative
 * error status on error.
 */
{
	int			rc;

	rc = ldr_context_install(ldr_process_context, module_name);
	if (rc != 0)
		errno = ldr_status_to_errno(rc);
	return(rc);
}


int
ldr_remove(const char *module_name)

/* Remove the specified module from the current process' private known
 * package table.  Returns 0 on success, or negative
 * error status on error.
 */
{
	int			rc;

	rc = ldr_context_remove(ldr_process_context, module_name);
	if (rc != 0)
		errno = ldr_status_to_errno(rc);
	return(rc);
}
