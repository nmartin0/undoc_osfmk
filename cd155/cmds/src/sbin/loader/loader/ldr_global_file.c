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
/* ldr_global_file.c
 * Routines for managing loader global data file
 *
 * The loader global data file contains all global loader data; that is,
 * all loader information shared by all processes.  Today this information
 * includes the global known package table and the preloaded library
 * cache (preloading is not yet supported).  The global data file is
 * mapped into all processes loaded by the loader at a fixed address
 * (so that it may contain pointers).  It is immutable once created,
 * and is mapped read-only by all processes.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include "ldr_types.h"
#include "ldr_lock.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "dqueue.h"
#include "ldr_errno.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"

#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_known_pkg.h"
#include "ldr_module.h"
#include "ldr_switch.h"
#include "ldr_global_file.h"
#include "ldr_preload.h"



/* Internal definition for the loader global file header.
 * The global data file header contains a magic number, heap pointer,
 * and the header for the global Known Package Table.  Eventually it
 * will also contain pre-loaded library information.
 */

typedef struct global_file_hdr {

	int			lgh_magic; /* magic number */
	int			lgh_version; /* header version number */
	size_t			lgh_mapsize; /* size of initial mapping */
	ldr_heap_t		lgh_heap; /* heap for allocations */
	ldr_kpt_header		lgh_global_kpt;	/* global KPT */
	ldr_preload_desc	lgh_preload_desc; /* preload descriptor */
} global_file_hdr;

#define LGH_MAGIC	0xe739ce73
#define	LGH_VERSION	1

/* Internal definition for the loader private data file.  The private
 * data file (inherited keep-on-exec from our parent) contains a magic
 * number, heap pointer, and the header for the private Known Package
 * Table.
 */

typedef struct private_file_hdr {

	int			lph_magic; /* magic number */
	int			lph_version; /* header version number */
	size_t			lph_mapsize; /* size of initial mapping */
	ldr_heap_t		lph_heap; /* heap for allocations */
	ldr_kpt_header		lph_private_kpt; /* private KPT */
} private_file_hdr;

#define LPH_MAGIC	0xf3cf3cf3
#define	LPH_VERSION	1


int
ldr_global_file_init(ldr_file_t fd, ldr_global_file_hdr *hdrp)

/* Initialize the specified file as a loader global data file.
 * This involves creating and initializing a global data file
 * header at the beginning of the file and filling it in, and
 * creating a heap from which the data structures that are to
 * be stored in the file can be allocated.  Note that the
 * file will always be mapped shared, read/write, not keep-on-exec.
 * Fd is an open file descriptor on the file to be initialized, open
 * for writing.  Returns pointer to the global file header in
 * *hdrp.  Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	global_file_hdr		*glob_hdr; /* the header */
	struct	addressconf	*addr_conf; /* address space config */
	int			pagesz;	/* page size */
	univ_t			mapaddr; /* where header is to live */
	univ_t			heapaddr; /* where heap is to live */
	int			mapflags; /* flags for ldr_mmap */
	ldr_heap_t		heap;	/* heap for this context */
	int			rc;

	/* Figure out where the global file is supposed to live */

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);
	mapaddr = addr_conf[AC_LDR_GLB].ac_base;
	pagesz = ldr_getpagesize();
	mapflags = LDR_MAP_FILE|LDR_MAP_SHARED|LDR_MAP_FIXED;

	/* Set the file size to one page, and map it (we assume global
	 * header is less than one page!)
	 */

	if ((rc = ldr_grow_file(fd, (off_t)pagesz)) != LDR_SUCCESS)
		return(rc);

	if ((rc = ldr_mmap(mapaddr, pagesz, LDR_PROT_READ|LDR_PROT_WRITE,
			   mapflags, fd, 0, (univ_t *)&glob_hdr)) != LDR_SUCCESS)
		return(rc);

	/* Got it.  Fill in as much as possible */

	glob_hdr->lgh_magic = LGH_MAGIC;
	glob_hdr->lgh_version = LGH_VERSION;

	/* Create the heap for allocations in the file.  Note that we
	 * start the heap at the first page of the file, assuming (as
	 * above) that the global file header is less than one page.
	 */

	heapaddr = (univ_t)((char *)mapaddr + pagesz);
	if ((rc = ldr_heap_create(heapaddr, fd, mapflags, pagesz, &heap)) != LDR_SUCCESS) {
		(void)ldr_munmap(glob_hdr, pagesz);
		return(rc);
	}
	glob_hdr->lgh_heap = heap;

	/* Now initialize the global KPT header in the global data */

	if ((rc = ldr_kpt_hdr_init(&(glob_hdr->lgh_global_kpt), heap)) != LDR_SUCCESS) {

		/* Hard to deallocate the heap here.  Assume it's one page */

		(void)ldr_munmap(glob_hdr, 2 * pagesz);
		return(rc);
	}

	/* Finally, initialize the preload descriptor in the global data */

	if ((rc = ldr_preload_init(&(glob_hdr->lgh_preload_desc), heap)) != LDR_SUCCESS) {
		/* Hard to deallocate the heap here.  Assume it's one page */

		(void)ldr_munmap(glob_hdr, 2 * pagesz);
		return(rc);
	}

	*hdrp = (ldr_global_file_hdr)glob_hdr;
	return(LDR_SUCCESS);
}


int
ldr_global_file_inherit(ldr_file_t fd, ldr_global_file_hdr *hdrp)

/* Inherit the specified open file as a loader global file.  This
 * basically involves error-checking.  We inherit the contents of
 * the file (the heap and KPT header).  Returns pointer to the
 * inherited global file header.  Returns LDR_SUCCESS on success
 * or negative error status on error.
 */
{
	global_file_hdr		*glob_hdr; /* the header */
	struct	addressconf	*addr_conf; /* address space config */
	univ_t			mapaddr; /* address to map at */
	size_t			mapsize; /* size to map including heap */
	size_t			pagesz;
	int			mapflags;
	int			rc;

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);
	mapaddr = (ldr_kpt_header *)addr_conf[AC_LDR_GLB].ac_base;
	pagesz = ldr_getpagesize();
	mapflags = LDR_MAP_FILE|LDR_MAP_SHARED|LDR_MAP_FIXED;

	/* Try to map enough of the file to cover the file header */

	if ((rc = ldr_mmap(mapaddr, pagesz, LDR_PROT_READ, mapflags, fd, 0,
			   (univ_t *)&glob_hdr)) != LDR_SUCCESS)
		return(rc);


	/* Check magic number and header version */

	if (glob_hdr->lgh_magic != LGH_MAGIC ||
	    glob_hdr->lgh_version != LGH_VERSION) {
		(void)ldr_munmap(glob_hdr, pagesz);
		return(LDR_EVERSION);
	}

	/* Now remove this mapping, and remap to cover entire heap */

	mapsize = glob_hdr->lgh_mapsize;
	(void)ldr_munmap(glob_hdr, pagesz);

	if ((rc = ldr_mmap(mapaddr, mapsize, LDR_PROT_READ, mapflags, fd, 0,
			   (univ_t *)&glob_hdr)) != LDR_SUCCESS)
		return(rc);

	/* OK, inherit the loader heap */

	if ((rc = ldr_heap_inherit(glob_hdr->lgh_heap, LDR_PROT_READ)) != LDR_SUCCESS)
		goto unmap_exit;

	/* Inherit the global KPT */

	if ((rc = ldr_kpt_hdr_inherit(&(glob_hdr->lgh_global_kpt))) != LDR_SUCCESS)
		goto unmap_exit;

	/* Inherit the preloaded libraries */

	if ((rc = ldr_preload_inherit(&(glob_hdr->lgh_preload_desc))) != LDR_SUCCESS)
		goto unmap_exit;

	/* Success; return global header address */

	*hdrp = (ldr_global_file_hdr)glob_hdr;
	return(LDR_SUCCESS);

unmap_exit:
	(void)ldr_munmap(glob_hdr, mapsize);
	return(rc);
}


int
ldr_global_file_set_size(ldr_global_file_hdr glob_hdr)

/* Compute and set the map size of the specified global file header.
 * The map size includes the global file header and the entire heap
 * including its header.  It is computed assuming that the file is
 * mapped into contiguous virtual address space.  Returns LDR_SUCCESS
 * on success, LDR_ERANGE on error.
 */
{
	global_file_hdr		*hdr = (global_file_hdr *)glob_hdr;
	size_t			heapsize;

	if ((heapsize = ldr_heap_size(hdr->lgh_heap)) == 0)
		return(LDR_ERANGE);

	/* Assume heap starts a first page of file (as above) */

	hdr->lgh_mapsize = ldr_getpagesize() + heapsize;
	return(LDR_SUCCESS);
}


ldr_kpt_header *
ldr_global_file_kpt(ldr_global_file_hdr glob_hdr)

/* Return the global KPT header from the specified loader global file.
 * Can't fail.  If no global KPT header, return NULL.
 */
{
	global_file_hdr		*hdr = (global_file_hdr *)glob_hdr;

	if (hdr == NULL)
		return(NULL);
	return(&(hdr->lgh_global_kpt));
}


int
ldr_global_file_preload(ldr_global_file_hdr glob_hdr, ldr_context *ctxt)

/* Copy any preloaded libraries from the specified loader context
 * into the specified loader global data file.  The data file
 * must have been initialized in ldr_global_file_init above.
 * Also, the context should have been created with the correct
 * preload region-allocation procedures.
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	global_file_hdr		*hdr = (global_file_hdr *)glob_hdr;

	return(ldr_preload_copyout(&hdr->lgh_preload_desc, ctxt));
}


int
ldr_global_file_remove(void)

/* Remove the loader global file from the address space.  All uses of
 * the loader global file should already have been removed.  This is
 * intended to be used only from the global library installation
 * program prior to initializing a new loader global file.
 * Returns LDR_SUCCESS on success or negative error status on error.
 */
{
	global_file_hdr		*glob_hdr; /* the header */
	struct	addressconf	*addr_conf; /* address space config */
	size_t			mapsize; /* size mapped */
	int			rc;

	/* Figure out where the global file is supposed to live */

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);
	glob_hdr = (global_file_hdr *)addr_conf[AC_LDR_GLB].ac_base;

	/* Remove the preload library information, so we won't see
	 * the preload cache during subsequent installs or loads.
	 */

	if ((rc = ldr_preload_remove(&glob_hdr->lgh_preload_desc)) != LDR_SUCCESS)
		return(rc);


	mapsize = glob_hdr->lgh_mapsize;
	return(ldr_munmap(glob_hdr, mapsize));
}


int
ldr_private_file_init(ldr_private_file_hdr *priv_hdr)

/* Initialize a loader private file for this process.
 * This involves creating an anonymous memory region to hold
 * the private file data, initializing the heap contained in the
 * private file for future allocations, and initializing the
 * constituent data structures (currently a KPT header).
 * Note that the loader private file is always
 * located at a fixed address in the process' address space,
 * found through the address configuration record.
 *
 * This routine will only be called in a process which has not
 * inherited a private file from its parent (because of the address
 * conflict that would otherwise result).
 *
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	private_file_hdr	*hdr;	/* the header */
	struct	addressconf	*addr_conf; /* address space config */
	int			pagesz;	/* page size */
	univ_t			mapaddr; /* where header is to live */
	univ_t			heapaddr; /* where heap is to live */
	int			mapflags; /* flags for ldr_mmap */
	ldr_heap_t		heap;	/* heap for this context */
	int			rc;

	/* Figure out where the private file is supposed to live */

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);
	mapaddr = addr_conf[AC_LDR_PRIV].ac_base;
	pagesz = ldr_getpagesize();
	mapflags = LDR_MAP_ANON|LDR_MAP_PRIVATE|LDR_MAP_INHERIT|LDR_MAP_FIXED;

	/* Now try to map one page for the header (we assume header is
	 * less than one page!)
	 */

	if ((rc = ldr_mmap(mapaddr, pagesz, LDR_PROT_READ|LDR_PROT_WRITE,
			   mapflags, LDR_FILE_NONE, 0, (univ_t *)&hdr)) != LDR_SUCCESS)
		return(rc);

	/* Got it.  Fill in as much as possible */

	hdr->lph_magic = LPH_MAGIC;
	hdr->lph_version = LPH_VERSION;

	/* Create the heap for allocations in this region */

	heapaddr = (univ_t)((char *)mapaddr + pagesz);
	if ((rc = ldr_heap_create(heapaddr, LDR_FILE_NONE, mapflags, 0, &heap)) != LDR_SUCCESS) {
		(void)ldr_munmap(hdr, pagesz);
		return(rc);
	}
	hdr->lph_heap = heap;

	/* Now initialize the KPT header */

	if ((rc = ldr_kpt_hdr_init(&(hdr->lph_private_kpt), heap)) != LDR_SUCCESS) {

		/* Hard to deallocate heap here.  Assume it's one page */

		(void)ldr_munmap(hdr, 2 * pagesz);
		return(rc);
	}

	*priv_hdr = (ldr_private_file_hdr)hdr;
	return(LDR_SUCCESS);
}


int
ldr_private_file_inherit(ldr_private_file_hdr *priv_hdr)

/* Check to see whether a private file has been inherited from our parent
 * process.  If so, inherit it.  This basically involves error-checking.
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	private_file_hdr	*hdr;	/* the header */
	struct	addressconf	*addr_conf; /* address space config */
	int			rc;

	if ((rc = ldr_getaddressconf(&addr_conf)) != LDR_SUCCESS)
		return(rc);
	hdr = (private_file_hdr *)addr_conf[AC_LDR_PRIV].ac_base;

	/* First, see if the private file header is there */

	if ((rc = ldr_mvalid((univ_t)hdr, sizeof(private_file_hdr),
			     LDR_PROT_READ|LDR_PROT_WRITE)) != LDR_SUCCESS) {

		/* Evidently not there.  Just return success, with a 
		 * NULL kpt_hdr.
		 */

		*priv_hdr = NULL;
		return(LDR_SUCCESS);
	}

	/* OK.  Inherit the loader heap. */

	if ((rc = ldr_heap_inherit(hdr->lph_heap, LDR_PROT_READ|LDR_PROT_WRITE)) != LDR_SUCCESS) {

		/* NOW WHAT?  Should probably try to unmap whatever's
		 * there, but how much do we unmap?  Better for now
		 * to just return NULL.  Any future install will fail
		 * with no space.
		 */

		*priv_hdr = NULL;
		return(LDR_SUCCESS);
	}

	/* Finally, inherit the KPT itself */

	if ((rc = ldr_kpt_hdr_inherit((&hdr->lph_private_kpt))) != LDR_SUCCESS) {

		*priv_hdr = NULL;
		return(LDR_SUCCESS);
	}

	*priv_hdr = (ldr_private_file_hdr)hdr;
	return(LDR_SUCCESS);

}


ldr_kpt_header *
ldr_private_file_kpt(ldr_private_file_hdr priv_hdr)

/* Return the private KPT header from the specified loader private file.
 * Can't fail.  If no global KPT header, return NULL.
 */
{
	private_file_hdr	*hdr = (private_file_hdr *)priv_hdr;

	if (hdr == NULL)
		return(NULL);
	return(&(hdr->lph_private_kpt));
}
