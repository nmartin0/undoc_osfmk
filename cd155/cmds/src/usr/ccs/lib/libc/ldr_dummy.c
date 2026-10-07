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
 *
 * OSF/1 Release 1.0
 */
/* ldr_dummy.c
 * Dummies of the exported loader system calls, for static libc
 *
 * This file contains dummy versions of the exported loader system
 * calls.  It is intended to be included in a statically-linked
 * version of the C library, from which the loader functions are
 * not available.
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ldr_dummy.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:45 $";
#endif

#include <sys/types.h>
#include <errno.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>


/* Load the specified module and all its required dependencies (both
 * static dependencies and dependencies to resolve import symbols)
 * into the specified loader context.  This includes calling the
 * module initialization routines for all modules, unless the load_flags
 * specified no initialization (eg. for kernel modules).  Return the
 * module ID of the loaded module in *mod_id.
 * Returns LDR_SUCCESS on success or a negative error status on error.
 */

int
ldr_context_load(ldr_context_t ctxt, const char *module_name,
		     ldr_load_flags_t load_flags, ldr_module_t *mod_id)
{
	return(-ENOSYS);
}


/* Return the entry point for the module named by the specified module ID
 * in the specified loader context.  Returns LDR_SUCCESS on success or
 * a negative error status on error.
 */

int
ldr_context_get_entry_pt(ldr_context_t ctxt, ldr_module_t mod_id,
			     ldr_entry_pt_t *entry)
{
	return(-ENOSYS);
}


/* Unload the specified module.  Don't do anything to any of this module's
 * dependencies.
 * Return LDR_SUCCESS on success or a negative error status on error (EINVAL
 * for invalid module module ID).
 */

int
ldr_context_unload(ldr_context_t ctxt, ldr_module_t mod_id)
{
	return(-ENOSYS);
}


/* Look up the specified symbol in the symbols exported by the specified
 * module ID, and return the absolute value of the symbol in *value.
 * Return LDR_SUCCESS on success or negative error status on error (ERANGE
 * if symbol value cannot be represented as a void *).
 */

int
ldr_context_lookup(ldr_context_t ctxt, ldr_module_t mod_id, char *symbol_name,
		       void **value)
{
	return(-ENOSYS);
}


/* Look up the specified symbol in the symbols exported from the specified
 * package, and return the absolute value of the symbol in *value.
 * Return LDR_SUCCESS on success or negative error status on error (ERANGE
 * if symbol value cannot be represented as a void *).
 */

int
ldr_context_lookup_package(ldr_context_t ctxt, char *package,
			       char *symbol_name, void **value)
{
	return(-ENOSYS);
}


/* Iterator through the module IDs for all modules currently loaded
 * in the specified context.  To initialize the iterator, set
 * *mod_id_ptr to LDR_MODULE_NULL.  The next call to this routine
 * should be made using the handle returned from this call and so on.
 * After the last module, the handle returned is the LDR_NULL_MODULE.
 * On success the call returns a zero, on failure it returns a
 * negative error number.
 */

int ldr_context_next_module(ldr_context_t ctxt, ldr_module_t
				       *mod_id_ptr)

{
	return(-ENOSYS);
}


/* Return module information about the module with the specified ID
 * in the specified context, into the info buffer supplied by the
 * caller.  info_size is the size of the buffer provided.  Returns the
 * actual size of the returned structure on success, or a negative
 * error status on error.
 */

int ldr_context_inq_module(ldr_context_t ctxt, ldr_module_t mod_id,
				      ldr_module_info_t *info, size_t info_size,
				      size_t *ret_size)
{
	return(-ENOSYS);
}



/* Return module information about the specified region of the
 * module with the specified ID in the specified context, into the
 * info buffer supplied by the caller.  info_size is the size of the
 * buffer provided.  Returns the actual size of the returned structure in
 * *ret_size.  Return 0 on success, or a negative error status on error.
 */

int ldr_context_inq_region(ldr_context_t ctxt, ldr_module_t mod_id,
				      ldr_region_t region, ldr_region_info_t *info,
				      size_t info_size, size_t *ret_size)
{
	return(-ENOSYS);
}


/* Install the specified module in the private known package table
 * of the specified context.  If the private known package table does
 * not yet exist, create it.  The module must not duplicate any currently-
 * installed package names in the private known package table.  Returns
 * LDR_SUCCESS on success or negative error status on error.
 */

int
ldr_context_install(ldr_context_t ctxt, const char *module_name)
{
	return(-ENOSYS);
}


/* Remove the specified module from the private known package table
 * of the specified context.  Returns LDR_SUCCESS on success or
 *  negative error status on error.
 */

int
ldr_context_remove(ldr_context_t ctxt, const char *module_name)
{
	return(-ENOSYS);
}


/* Allocate a loader context and initialize it.  Arguments are the expected
 * number of modules that will be loaded into the context.  The context
 * is initialized to use the default loader switch.  Returns LDR_SUCCESS
 * on success or a negative loader status code on error.
 */

int
ldr_context_create (int nmodules, alloc_abs_region_p absp, alloc_rel_region_p
			relp, dealloc_region_p deallocp, ldr_context_t *ctxt)
{
	return(-ENOSYS);
}


/* Check to see whether a loader private data file has been inherited from
 * our parent process, and if so, inherit it and get from it the private 
 * KPT of the specified loader context.  Then, try to open the loader
 * global data file and inherit it, setting up the global KPT
 * of the specified context.  Called only during loader
 * bootstrapping.  Returns LDR_SUCCESS on success, negative error
 * status on error.
 */

int
ldr_context_inherit(ldr_context_t ctxt)
{
	return(-ENOSYS);
}


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
 * from the context into the global data file's heap.
 *
 * Note that for this routine to be successful, the calling process
 * must not currently be using the global data file.  A caller
 * should call ldr_context_remove_global_file() before calling
 * this routine.
 *
 * Returns LDR_SUCCESS on success, negative error status on error.
 */

int
ldr_context_global_file_init(ldr_context_t ctxt, ldr_file_t fd)
{
	return(-ENOSYS);
}


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

int
ldr_context_global_file_remove(ldr_context_t ctxt)
{
	return(-ENOSYS);
}


/* Bootstrap the loader system.  Includes building the loader process
 * context, containing the module and region records and exported symbol
 * list for the loader itself.  The loader_name argument is the name of
 * the loader's object module, for building the module record.  Returns
 * the address of the newly-created context in *ctxt.
 */

int ldr_bootstrap(const char *loader_name, ldr_context_t *ctxt)
{
	return(-ENOSYS);
}


/* Bootstrap the specified loader context, by constructing a module
 * record for the loader itself and filling it in with the loader's
 * exports.  We bootstrap the context by doing a "fake load" on the
 * specified loader file, using a private loader switch as a temporary
 * switch so the recognizer will not try to open the file.  Returns
 * LDR_SUCCESS on success, negative error status on error.
 */

int
ldr_context_bootstrap(ldr_context_t ctxt, const char *loader_name)
{
	return(-ENOSYS);
}


/* Install the locking functions to be used by the loader.  This is
 * intended to be run from pthreads_init() or other thread package
 * initialization routines.
 */

void
ldr_declare_lock_functions(lib_lock_functions_t *funcs)
{
}
