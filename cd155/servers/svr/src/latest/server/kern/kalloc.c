/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: kalloc.c,v $
 * Revision 1.6  1994/11/18  20:32:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/12/20  19:05:36  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: declare kfree() as void to match header definition,
 * 	so we pass 'lint'
 *  Testing: builds
 *  Module(s):
 *
 * Revision 1.4  1993/07/14  18:01:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:14  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1991/10/14  12:33:43  sjs
 * 	91/09/13  12:50:19  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 *
 * Revision 2.2  91/08/31  13:37:28  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/07  16:59:55  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  13:53:10  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:53:49  gm]
 * 
 * Revision 1.5  90/06/22  20:14:04  devrcs
 * 	Remove pageable flag from zinit argument list.
 * 	[90/06/07  15:25:36  jvs]
 * 
 * Revision 1.4  90/02/23  00:28:38  devrcs
 * 	gross hack to allow us to initialize the
 * 	kalloc zones exactly when we need to.
 * 	[90/02/12  16:16:35  jeffc]
 * 
 * Revision 1.3  90/01/02  20:05:50  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/10/26  07:50:16  gm
 * 	MACH X115 Update
 * 
 * Revision 6.2  89/08/25  16:12:49  boykin
 * 	Added table to allow per-zone sizing.
 * 
 * Revision 6.1  89/07/26  16:04:07  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.7  89/10/11  14:09:58  dlb
 * 	Rewrite kallocinit() to allow the k_zones to be individually
 * 	sized with the sizes in a static (patchable) array.  Loosely
 * 	based on code from Joe Boykin (boykin@encore).
 * 	[89/09/01  17:28:36  dlb]
 * 
 * Revision 2.6  89/08/02  08:03:28  jsb
 * 	Make all kalloc zones 8 MB big. (No more kalloc panics!)
 * 	[89/08/01  14:10:17  jsb]
 * 
 * Revision 2.4  89/04/05  13:03:10  rvb
 * 	Guarantee a zone max of at least 100 elements or 10 pages
 * 	which ever is greater.  Afs (AllocDouble()) puts a great demand
 * 	on the 2048 zone and used to blow away.
 * 	[89/03/09            rvb]
 * 
 * Revision 2.3  89/02/25  18:04:39  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.2  89/01/18  02:07:04  jsb
 * 	Give each kalloc zone a meaningful name (for panics);
 * 	create a zone for each power of 2 between MINSIZE
 * 	and PAGE_SIZE, instead of using (obsoleted) NQUEUES.
 * 	[89/01/17  10:16:33  jsb]
 * 
 * Revision 0.0  88/02/13            jks
 * 	Updated to use kmem routines instead of vmem routines.
 * 	[88/02/13            jks]
 * 
 * Revision 0.0  85/06/21            avie
 * 	Created.
 * 	[85/06/21            avie]
 * 
 * $EndLog$
 */
/*
 *	File:	kern/kalloc.c
 *	Author:	Avadis Tevanian, Jr.
 *
 *	Copyright (C) 1985, Avadis Tevanian, Jr.
 *
 *	General kernel memory allocator.  This allocator is designed
 *	to be used by the kernel to manage dynamic memory fast.
 */

#include <sys/types.h>

#include <uxkern/import_mach.h>
#include <uxkern/vm_param.h>
#include <kern/zalloc.h>
#include <kern/kalloc.h>

/*
 *	All allocations of size less than PAGE_SIZE are rounded to the
 *	next highest power of 2.  This allocator is built on top of
 *	the zone allocator.  A zone is created for each potential size
 *	that we are willing to get in small blocks.
 *
 *	We assume that PAGE_SIZE is not greater than 64K;
 *	thus 16 is a safe array size for k_zone and k_zone_name.
 */

int first_k_zone = -1;
struct zone *k_zone[16];
static char *k_zone_name[16] = {
	"kalloc.1",		"kalloc.2",
	"kalloc.4",		"kalloc.8",
	"kalloc.16",		"kalloc.32",
	"kalloc.64",		"kalloc.128",
	"kalloc.256",		"kalloc.512",
	"kalloc.1024",		"kalloc.2048",
	"kalloc.4096",		"kalloc.8192",
	"kalloc.16384",		"kalloc.32768"
};

/*
 *  Max number of elements per zone.  zinit rounds things up correctly
 *  Doing things this way permits each zone to have a different maximum size
 *  based on need, rather than just guessing; it also
 *  means its patchable in case you're wrong!
 */
unsigned long k_zone_max[16] = {
      1024,		/*      1 Byte  */
      1024,		/*      2 Byte  */
      1024,		/*      4 Byte  */
      1024,		/*      8 Byte  */
      1024,		/*     16 Byte  */
      1024,		/*     32 Byte  */
      1024,		/*     64 Byte  */
      1024,		/*    128 Byte  */
      4096,		/*    256 Byte  */
      256,		/*    512 Byte  */
      128,		/*   1024 Byte  */
      128,		/*   2048 Byte  */
      64,		/*   4096 Byte  */
      64,		/*   8192 Byte  */
      64,		/*  16384 Byte  */
      64,		/*  32768 Byte  */
};

/*
 *	Initialize the memory allocator.  This should be called only
 *	once on a system wide basis (i.e. first processor to get here
 *	does the initialization).
 *
 *	This initializes all of the zones.
 */
/* kallocinitialized is a gross hack to allow us to initialize the 
   kalloc zones exactly when we need to. This depends on being called prior
   to going multithreaded. */
static int kallocinitialized = 0;

kallocinit()
{
	register int i, size;

	if (kallocinitialized) return;
	kallocinitialized++;

	/*
	 *	Allocate a zone for each size we are going to handle.
	 *	We specify non-paged memory.
	 */
	for (i = 0, size = 1; size < PAGE_SIZE; i++, size <<= 1) {
		if (size < MINSIZE) {
			k_zone[i] = 0;
			continue;
		}
		if (size == MINSIZE) {
			first_k_zone = i;
		}
		k_zone[i] = zinit(size, k_zone_max[i] * size, PAGE_SIZE,
			k_zone_name[i]);
	}
}

caddr_t kalloc(size)
{
	register zindex, allocsize;
	caddr_t addr;

	if (!kallocinitialized) kallocinit();

	/* compute the size of the block that we will actually allocate */

	allocsize = size;
	if (size < PAGE_SIZE) {
		allocsize = MINSIZE;
		zindex = first_k_zone;
		while (allocsize < size) {
			allocsize <<= 1;
			zindex++;
		}
	}

	/*
	 * If our size is still small enough, check the queue for that size
	 * and allocate.
	 */

	if (allocsize < PAGE_SIZE) {
		addr = (caddr_t) zalloc(k_zone[zindex]);
	} else {
		if (vm_allocate(mach_task_self(), (vm_offset_t *)&addr, allocsize, TRUE)
		    != KERN_SUCCESS)
			addr = NULL;
	}
	return(addr);
}

caddr_t kget(size)
{
	register zindex, allocsize;
	caddr_t addr;

	/* compute the size of the block that we will actually allocate */

	allocsize = size;
	if (size < PAGE_SIZE) {
		allocsize = MINSIZE;
		zindex = first_k_zone;
		while (allocsize < size) {
			allocsize <<= 1;
			zindex++;
		}
	}

	/*
	 * If our size is still small enough, check the queue for that size
	 * and allocate.
	 */

	if (allocsize < PAGE_SIZE) {
		addr = (caddr_t) zget(k_zone[zindex]);
	} else {
		/* This will never work, so we might as well panic */
		panic("kget: oversized request");
	}
	return(addr);
}

void
kfree(data, size)
	caddr_t	data;
	long	size;
{
	register freesize, zindex;

	freesize = size;
	if (size < PAGE_SIZE) {
		freesize = MINSIZE;
		zindex = first_k_zone;
		while (freesize < size) {
			freesize <<= 1;
			zindex++;
		}
	}

	if (freesize < PAGE_SIZE) {
		zfree(k_zone[zindex], data);
	} else {
		(void) vm_deallocate(mach_task_self(), (vm_offset_t) data, freesize);
	}
}
