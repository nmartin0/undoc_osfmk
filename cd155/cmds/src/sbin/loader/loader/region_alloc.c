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
/* region_alloc.c
 * This file contains the default routines for allocating and
 * deallocating virtual address space for regions loaded into a
 * process.  These routines are potentially machine-dependent, so they
 * are in a separate file to allow them to be overridden by a
 * machine-dependent version.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include "ldr_types.h"
#include "ldr_errno.h"
#include "ldr_sys_int.h"


int
alloc_abs_process_region(univ_t vaddr, size_t size, ldr_prot_t prot,
			 univ_t *baseaddr)

/* Region allocator for absolute regions to be loaded into a process.
 * This is the "default" absolute region allocator used in
 * initializing the process loader context.  The absolute region
 * allocator procedure is called by the format-dependent map_region
 * routine to determine the base address at which the region is to be
 * mapped.  Arguments are the virtual address at which the region must
 * run, region size and protection.  On return, baseaddr is set to the
 * best-guess starting address at which the region is to be mapped.
 * Returns LDR_SUCCESS on success, negative error status on error.
 *
 * For loading an absolute region into a process, the base address for mapping
 * is always the virtual address at which the code is to run.  This informs our
 * caller that he is to use the LDR_MAP_FIXED flag when mapping the region.
 */
{
	*baseaddr = vaddr;
	return(LDR_SUCCESS);
}


int
alloc_rel_process_region(size_t size, ldr_prot_t prot, univ_t *vaddr,
			 univ_t *baseaddr)

/* Region allocator for relocatable regions to be loaded into a
 * process.  This is the "default" relocatable region allocator used
 * in initializing the process loader context.  The relocatable region
 * allocator procedure is called by the format-dependent map_region
 * routine to determine the virtual address at which the region is to
 * be run and the base address at which the region is to be mapped.
 * Arguments are the region size and protection.  On return, *vaddr
 * is set to the address at which the code is to be relocated to run,
 * and *baseaddr is set to the best-guess starting address at which
 * the region is to be mapped; a NULL value for *vaddr means that the
 * code is to be relocated to run at whatever address it ends up being
 * mapped at.  Returns LDR_SUCCESS on success, negative error status
 * on error.
 * 
 * For the default region allocator, we determine the desired base
 * address for the region by parsing the address configuration record
 * obtained from the kernel.  We always return *vaddr equal to NULL,
 * to ensure that the code is relocated to the address it is mapped
 * at.  Return LDR_SUCCESS on success, negative error status on error.
 */
{
	int		rc;
	struct addressconf *addr_conf;

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);

	/* We will use the following policy:
	 * - all regions with write access go in the mapped file data
	 *   area until we pass a flag to distinguish anonymous memory
	 *   which we'd like to go in the mapped file bss area
	 * - all regions without write access go in the mapped file
	 *   text area
	 * Other plausible policies can easily be imagined.
	 */

	if (prot & LDR_W)
		*baseaddr = addr_conf[AC_MMAP_DATA].ac_base;
	else
		*baseaddr = addr_conf[AC_MMAP_TEXT].ac_base;

	/* Always return *vaddr equal to NULL */

	*vaddr = NULL;
	return(LDR_SUCCESS);
}


int
dealloc_process_region(univ_t vaddr, univ_t mapaddr, size_t size)

/* Region deallocator for regions loaded into a process.  This is
 * the "default" deallocator used in initializing the process loader context.
 * The region deallocator procedure is the inverse to the region allocator
 * procedure; it is called by the format-dependent unmap_region routine to
 * deallocate any storage allocated by the alloc_region_p procedure.
 * mapaddr argument is the actual address to which the region was mapped;
 * vaddr is vaddr returned from alloc_region_p if it was non-NULL, or
 * equal to mapaddr otherwise; size is argument passed to alloc_region_p.
 * Returns LDR_SUCCESS on success or negative error status on error.
 */
{
	/* For the process allocator, no special deallocation work is needed */

	return(LDR_SUCCESS);
}


int
alloc_rel_main_region(size_t size, ldr_prot_t prot, univ_t *vaddr,
		      univ_t *baseaddr)

/* Region allocator for relocatable regions to be loaded into the main
 * program of a process.  This routine is only used for the main program
 * itself.  The main program is different from all other modules in that
 * its data and bss regions must be loaded into the main program area,
 * so that there is room for the break area afterward.
 *
 * Arguments are the region size and protection.  On return, *vaddr
 * is set to the address at which the code is to be relocated to run,
 * and *baseaddr is set to the best-guess starting address at which
 * the region is to be mapped; a NULL value for *vaddr means that the
 * code is to be relocated to run at whatever address it ends up being
 * mapped at.  Returns LDR_SUCCESS on success, negative error status
 * on error.
 * 
 * For the default region allocator, we determine the desired base
 * address for the region by parsing the address configuration record
 * obtained from the kernel.  We always return *vaddr equal to NULL,
 * to ensure that the code is relocated to the address it is mapped
 * at.  Return LDR_SUCCESS on success, negative error status on error.
 */
{
	int		rc;
	struct addressconf *addr_conf;

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);

	/* We will use the following policy:
	 * - all regions with write access go in the main program data
	 *   area until we pass a flag to distinguish anonymous memory
	 *   which we'd like to go in the mapped file bss area
	 * - all regions without write access go in the main program
	 *   text area
	 */

	if (prot & LDR_W)
		*baseaddr = addr_conf[AC_DATA].ac_base;
	else
		*baseaddr = addr_conf[AC_TEXT].ac_base;

	/* Always return *vaddr equal to NULL */

	*vaddr = NULL;
	return(LDR_SUCCESS);
}
