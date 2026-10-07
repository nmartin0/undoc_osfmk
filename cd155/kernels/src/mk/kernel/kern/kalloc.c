/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
/*
 *	$Id: kalloc.c,v 1.9 1994/11/18 20:52:11 mtm Exp $
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: kalloc.c,v $
 * Revision 1.9  1994/11/18  20:52:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/11/02  22:14:49  andyp
 * PTS #: 10452
 * Mandatory?:  No
 * Description: Don't kfree() zero-length ool points during deallocation.
 *              Added an assert to check for zero-length deallocations
 *              in both zfree() and in kfree().
 * Reviewer(s): lenb, rkl
 * Risk:        Low
 * Testing:     sats, AIM3
 * Module(s):
 *         M norma2/tx_engine.c
 *         M kern/kalloc.c
 *         M kern/zalloc.c
 *
 * Revision 1.7  1994/07/12  19:23:21  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6  1994/04/13  09:18:40  fritz
 * Undo the change to kalloc_map size. The default now is back at
 * 8 MByte. It can be changed by bootmagic KALLOC_MAP_SIZE.
 *
 * Revision 1.5  1994/03/29  13:09:46  fritz
 * Change kalloc() to *never* return failure, but panic instead.
 * Also, adjust size of kalloc_map to size of physical memory.
 *
 * Revision 1.4  1993/06/30  22:44:55  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:35:51  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:35:59  dleslie
 * First R1_0 release
 *
 * Revision 2.9  91/05/14  16:43:17  mrt
 * 	Correcting copyright
 * 
 * Revision 2.8  91/03/16  14:50:37  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 
 * Revision 2.7  91/02/05  17:27:22  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:12  mrt]
 * 
 * Revision 2.6  90/06/19  22:59:06  rpd
 * 	Made the big kalloc zones collectable.
 * 	[90/06/05            rpd]
 * 
 * Revision 2.5  90/06/02  14:54:47  rpd
 * 	Added kalloc_max, kalloc_map_size.
 * 	[90/03/26  22:06:39  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:13  dbg
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.3  89/09/08  11:25:51  dbg
 * 	MACH_KERNEL: remove non-MACH data types.
 * 	[89/07/11            dbg]
 * 
 * Revision 2.2  89/08/31  16:18:59  rwd
 * 	First Checkin
 * 	[89/08/23  15:41:37  rwd]
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
 *
 * 13-Feb-88  John Seamons (jks) at NeXT
 *	Updated to use kmem routines instead of vmem routines.
 *
 * 21-Jun-85  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Created.
 */
/*
 *	File:	kern/kalloc.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1985
 *
 *	General kernel memory allocator.  This allocator is designed
 *	to be used by the kernel to manage dynamic memory fast.
 */

#include <mach/machine/vm_types.h>
#include <mach/vm_param.h>

#include <kern/zalloc.h>
#include <kern/kalloc.h>
#include <vm/vm_kern.h>
#include <vm/vm_object.h>
#include <vm/vm_map.h>

vm_map_t kalloc_map;
vm_size_t kalloc_map_size = 8 * 1024 * 1024;
vm_size_t kalloc_max;

/*
 *	All allocations of size less than kalloc_max are rounded to the
 *	next highest power of 2.  This allocator is built on top of
 *	the zone allocator.  A zone is created for each potential size
 *	that we are willing to get in small blocks.
 *
 *	We assume that kalloc_max is not greater than 64K;
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
      4096,		/*     32 Byte  */
      4096,		/*     64 Byte  */
      4096,		/*    128 Byte  */
      4096,		/*    256 Byte  */
      1024,		/*    512 Byte  */
      1024,		/*   1024 Byte  */
      1024,		/*   2048 Byte  */
      1024,		/*   4096 Byte  */
      4096,		/*   8192 Byte  */
      64,		/*  16384 Byte  */
      64,		/*  32768 Byte  */
};


int	kalloc_aligned = 1;		/* return aligned memory */
int	kalloc_minsize = MINSIZE;	/* minimum amt allocated */


#define	KALLOC_STATS	1
#if	KALLOC_STATS
unsigned long kalloc_stats[6][16];

static void kalloc_count(addr, zindex)
	vm_offset_t	addr;		/* buffer allocated */
	int		zindex;		/* allocation zone... */
{
	if ((addr & 31) == 0)
		kalloc_stats[5][zindex]++;	/* 32-byte aligned */
	else if ((addr & 15) == 0)
		kalloc_stats[4][zindex]++;	/* 16-byte aligned */
	else if ((addr &  7) == 0)
		kalloc_stats[3][zindex]++;	/*  8-byte aligned */
	else if ((addr &  3) == 0)
		kalloc_stats[2][zindex]++;	/*  4-byte aligned */
	else if ((addr &  1) == 0)
		kalloc_stats[1][zindex]++;	/*  2-byte aligned */
	else
		kalloc_stats[0][zindex]++;	/*  unaligned */
}

#define	KALLOC_COUNT(addr, zindex)	kalloc_count(addr, zindex)

#include <mach_kdb.h>
#if	MACH_KDB

int db_kalloc_stats(reset)
	int	reset;
{
	int	y, x, c;

	c = 0;
	if (reset) {
		for (y = 0; y < 6; y++)
			for (x = 0; x < 16; x++)
				kalloc_stats[y][x] = 0, c++;
		return c;
	}

	for (y = 5; y >= 0; y--)
		db_printf("%8d ", 1 << y);
	db_printf("zone\n");

	for (x = 0; x < 16; x++) {
		for (y = 5; y >= 0; y--) {
			c += kalloc_stats[y][x];
			db_printf("%8d ", kalloc_stats[y][x]);
		}
		db_printf("%s\n", k_zone_name[x]);
	}

	return c;
}

#endif	/* MACH_KDB */

#else	/* KALLOC_STATS */

#define	KALLOC_COUNT(addr, zindex)

#endif	/* KALLOC_STATS */


/*
 *	Initialize the memory allocator.  This should be called only
 *	once on a system wide basis (i.e. first processor to get here
 *	does the initialization).
 *
 *	This initializes all of the zones.
 */

void kalloc_init()
{
	vm_offset_t min, max;
	vm_size_t size;
	register int i;
	extern vm_size_t mem_size;

	kalloc_map_size = getbootint("KALLOC_MAP_SIZE", kalloc_map_size);
	if (kalloc_map_size == 0)
		kalloc_map_size = mem_size;

	kalloc_map = kmem_suballoc(kernel_map, &min, &max,
				   kalloc_map_size, FALSE);

	/*
	 *	Ensure that zones up to size 8192 bytes exist.
	 *	This is desirable because messages are allocated
	 *	with kalloc, and messages up through size 8192 are common.
	 */

	if (PAGE_SIZE < 16*1024)
		kalloc_max = 16*1024;
	else
		kalloc_max = PAGE_SIZE;

	/*
	 *	Allocate a zone for each size we are going to handle.
	 *	We specify non-paged memory.
	 */
	for (i = 0, size = 1; size < kalloc_max; i++, size <<= 1) {
		if (size < kalloc_minsize) {
			k_zone[i] = 0;
			continue;
		}
		if (size == kalloc_minsize) {
			first_k_zone = i;
		}
		k_zone[i] = zinit(size, k_zone_max[i] * size, size,
				  FALSE, k_zone_name[i]);
		if (kalloc_aligned)
			zaligned(k_zone[i]);
		if (size >= PAGE_SIZE)
			zcollectable(k_zone[i]);
	}
}

vm_offset_t kalloc(size)
	vm_size_t size;
{
	register int zindex;
	register vm_size_t allocsize;
	vm_offset_t addr;

	/* compute the size of the block that we will actually allocate */

	allocsize = size;
	if (size < kalloc_max) {
		allocsize = kalloc_minsize;
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

	if (allocsize < kalloc_max) {
		addr = zalloc(k_zone[zindex]);
		KALLOC_COUNT(addr, zindex);
	} else {
		if (kmem_alloc_wired(kalloc_map, &addr, allocsize)
							!= KERN_SUCCESS)
			addr = 0;
	}
	if (!addr)
		panic("kalloc");

	return(addr);
}

vm_offset_t kget(size)
	vm_size_t size;
{
	register int zindex;
	register vm_size_t allocsize;
	vm_offset_t addr;

	/* compute the size of the block that we will actually allocate */

	allocsize = size;
	if (size < kalloc_max) {
		allocsize = kalloc_minsize;
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

	if (allocsize < kalloc_max) {
		addr = zget(k_zone[zindex]);
		KALLOC_COUNT(addr, zindex);
	} else {
		/* This will never work, so we might as well panic */
		panic("kget");
	}
	return(addr);
}

void
kfree(data, size)
	vm_offset_t data;
	vm_size_t size;
{
	register int zindex;
	register vm_size_t freesize;

	assert(data != (vm_offset_t) 0);
	freesize = size;
	if (size < kalloc_max) {
		freesize = kalloc_minsize;
		zindex = first_k_zone;
		while (freesize < size) {
			freesize <<= 1;
			zindex++;
		}
	}

	if (freesize < kalloc_max) {
		zfree(k_zone[zindex], data);
	} else {
		kmem_free(kalloc_map, data, freesize);
	}
}
