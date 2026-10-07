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
 * $Log: zalloc.c,v $
 * Revision 1.8  1994/11/18  20:32:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  18:02:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:17:47  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:06:18  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/01/09  00:04:23  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.4  1992/12/18  17:15:37  nandy
 * The zone expansion log messages are now under #if MACH_ASSERT
 *
 * Revision 1.3  1992/12/15  00:02:32  nandy
 * ifdefed the zalloc log messages.
 *
 * Revision 1.2  1992/11/30  22:23:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:16:20  cfj
 * Bump major revision number.
 *
 * Revision 2.7  1992/04/06  19:08:08  klh
 * For OSF merge, update version # to match LCC#
 *
 * Revision 2.6  92/04/05  16:54:35  pjg
 * 	Round the allocation size to a page boundary in zinit (durriya).
 * 
 * 	Add kernel return value in panic message inside zalloc(). (roman)
 * 
 * Revision 2.5  92/02/21  16:18:42  durriya
 * 	set attribute 'expandable' of zones to be TRUE by default. 
 * 
 * Revision 2.4  91/12/16  20:52:29  roy
 * 	91/10/18  17:15:52  jose
 * 	Changed place where uxkern/vm_param.h is included
 * 
 * Revision 2.3  91/10/14  12:34:04  sjs
 * 	91/10/01  14:08:15  condict
 * 	Fix bug: lock_zone_init should always use simple locks, like lock_zone.
xo * 
 * 	91/09/20  12:15:14  condict
 * 	Move lock_zone macro from zalloc.c to zalloc.h to allow its use by 
 * 	ZALLOC macro (fixes major zone allocation bug).  
 * 	Also removed all ifdef OSF1_SERVER.
 * 
 * 	91/09/13  12:50:56  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  13:38:21  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/07  17:00:49  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/31  13:58:11  devrcs
 * 	Remove assumption in zinit that pointers are 4 bytes. Allow
 * 	"expandable" zones to grow after logging, instead of panicking
 * 	system. Zones are created this way by default.
 * 	[90/10/11  08:16:47  tmt]
 * 
 * Revision 1.8  90/10/07  13:57:56  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  10:01:01  gm]
 * 
 * Revision 1.7  90/09/23  15:51:41  devrcs
 * 	When zchange() changes the pageability of zone, need to call
 * 	lock_zone_init() again, since we will henceforth be using
 * 	a different lock.
 * 	[90/09/06  12:37:11  ers]
 * 
 * Revision 1.6  90/06/22  20:15:24  devrcs
 * 	Move the accounting for zones' garbage collection out of the
 * 	macros and into zone_gc, the garbage collection routine itself.
 * 	[90/06/18  13:38:54  jvs]
 * 
 * 	nags merge
 * 	[90/06/12  21:29:07  gmf]
 * 
 * 	Some cleanup work:
 * 	    - remove #ifdef GARBAGE_COLLECTION
 * 	    - remove pageable flag from zinit argument list.
 * 	    - remove the temporary zcollectable routine.
 * 	    - add a collectable flag to zchange.
 * 	    - add comments.
 * 	[90/06/07  15:35:13  jvs]
 * 
 * 	Condensed relevant history:
 * 	Zone garbage collection:			jvs@osf.org
 * 	  Change the loop in the second half of zone_gc that actually frees
 * 	  gargbage collected pages to walk through a singly linked list of
 * 	  zone_page_table entries.  This is a small performance optimization
 * 	  to keep the number times through this loop down to that required
 * 	  to free the pages.
 * 
 * 	This fixes three bugs:
 * 		1. missing zone_page_alloc call in zalloc.
 * 		2. "memory leak" caused by incorrect setting of
 * 		    cur_size in zcram.
 * 		3. off by one error caused by incorrect value passed to
 * 		   zone_page_init in zone_init.  (ZONE_PAGE_UNUSED (==-1)
 * 		   was used.  It should be ZONE_PAGE_USED (==0).
 * 
 * 	Added collectable zones.  This version boots on both the pmax
 * 	and the mmax but, except for initialization, garbage collection
 * 	is not getting exercised.
 * 
 * 	Use 8MB for zone_init instead of 16MB.		noemi@osf.org
 * 	Mach Release 2.5 (preliminary)			alan@encore.com
 * 	Fixed 2 bugs in the package:			boykin@encore.com
 * 	  If the kernel runs out of space
 * 	  the 'cur_size' field got upped anyway and we roached the contents
 * 	  of location zero (presuming we didn't panic due to protection
 * 	  failure). The second problem was that the "doing_alloc" field
 * 	  was never used, the result is that two memory allocations could
 * 	  have been done instead of just one.  This could push a
 * 	  zone over its max allocation (no great tragedy).
 * 	Added thread, lock, and unlock addresses.	boykin@encore.com
 * 	Fixed a problem run out of space in select zone boykin@encore.com
 * 	Free the lock before the call to printf.	boykin@encore.com
 * 	to avoid duplicate memory allocation in zalloc 	dlb@cs.cmu.edu
 * 	Use new vm_map_copy technology.			mwyoung@cs.cmu.edu
 * 	Make zalloc storage pointers external.		rvb@cs.cmu.edu
 * 	Keep all zones on a list that host_zone_info can traverse.
 * 							rpd@cs.cmu.edu
 * 	Added host_zone_info (under MACH_DEBUG).	rpd@cs.cmu.edu
 * 	Fixed zget to increase cur_size when the space	rpd@cs.cmu.edu
 * 	  comes from zget_space. 	Use MACRO_BEGIN/MACRO_END,
 * 	  decl_simple_lock_data where appropriate.
 * 	Document zget_space.  Elim MACH_XP conditional.	mwyoung@cs.cmu.edu
 * 	Allocate zone memory from a separate kernel submap, to avoid
 * 
 * 	Ancient contributors:
 * 	rfr@cs.cmu.edu			mrt@cs.cmu.edu
 * 	mwyoung@cs.cmu.edu		sanzi@cs.cmu.edu
 * 	avie@cs.cmu.edu			dbg@cs.cmu.edu
 * 	rds@cs.cmu.edu
 * 	[90/06/07  16:17:47  jvs]
 * 
 * $EndLog$
 */
/*
 *	File:	kern/zalloc.c
 *	Author:	Avadis Tevanian, Jr.
 *
 *	Zone-based memory allocator.  A zone is a collection of fixed size
 *	data blocks for which quick allocation/deallocation is possible.
 */

#include <kern/zalloc.h>
#include <uxkern/import_mach.h>
#include <uxkern/vm_param.h>
#include <sys/syslog.h>

zone_t		zone_zone;	/* this is the zone containing other zones */

boolean_t	zone_ignore_overflow = FALSE;

vm_offset_t	zdata;
vm_size_t	zdata_size;

#define lock_zone_init(zone)			\
MACRO_BEGIN					\
	simple_lock_init(&zone->lock);		\
MACRO_END

/*
 *	zinit initializes a new zone.  The zone data structures themselves
 *	are stored in a zone, which is initially a static structure that
 *	is initialized by zone_init.
 */
zone_t zinit(size, max, alloc, name)
	vm_size_t	size;		/* the size of an element */
	vm_size_t	max;		/* maximum memory to use */
	vm_size_t	alloc;		/* allocation size */
	char		*name;		/* a name for the zone */
{
	register zone_t		z;

	if (zone_zone == ZONE_NULL)
		z = (zone_t) zdata;
	else if ((z = (zone_t) zalloc(zone_zone)) == ZONE_NULL)
		return(ZONE_NULL);

	if (size == 0)
		size = sizeof(z->free_elements);
 	if (alloc == 0)
		alloc = (PAGE_SIZE > round_page(size)) ? PAGE_SIZE :
							 round_page(size);

	/*
	 *	Round off all the parameters appropriately.
	 */

	if ((max = round_page(max)) < (alloc = round_page(alloc)))
		max = alloc;

	z->free_elements = 0;
	z->cur_size = 0;
	z->max_size = max;
	z->elem_size = ((size-1) + sizeof(z->free_elements)) -
			((size-1) % sizeof(z->free_elements));
	z->alloc_size = alloc;
	z->zone_name = name;
	z->count = 0;
	z->doing_alloc = FALSE;
	z->pageable = TRUE;
	z->exhaustible = FALSE;
	z->sleepable = FALSE;
	z->collectable = FALSE;
	z->expandable = TRUE;
	lock_zone_init(z);

	return(z);
}

/*
 *	Cram the given memory into the specified zone.
 */
void zcram(zone, newmem, size)
	register zone_t		zone;
	vm_offset_t		newmem;
	vm_size_t		size;
{
	register vm_size_t	elem_size;

	if (newmem == (vm_offset_t) 0) {
		panic("zcram - memory at zero");
	}
	elem_size = zone->elem_size;

	lock_zone(zone);
	while (size >= elem_size) {
		ADD_TO_ZONE(zone, newmem);
		zone->count++;	/* compensate for ADD_TO_ZONE */
		size -= elem_size;
		newmem += elem_size;
		zone->cur_size += elem_size;
	}
	unlock_zone(zone);
}


/*
 *	Initialize the "zone of zones" which uses fixed memory allocated
 *	earlier in memory initialization.  zone_bootstrap is called
 *	before zone_init.
 */
void zone_bootstrap()
{
	zone_zone = ZONE_NULL;
	zone_zone = zinit(sizeof(struct zone), sizeof(struct zone), 0, "zones");
	zchange(zone_zone, TRUE, FALSE, FALSE, FALSE);
	zcram(zone_zone, (vm_offset_t)(zone_zone + 1),
		(zdata + zdata_size) - (vm_offset_t)(zone_zone + 1));
}

void zone_init()
{
	zdata_size = round_page(64 * sizeof(struct zone));
	(void) vm_allocate(mach_task_self(), &zdata, zdata_size, TRUE);
	zone_bootstrap();
}
	

/*
 *	zalloc returns an element from the specified zone.
 */
vm_offset_t zalloc(zone)
	register zone_t	zone;
{
	register vm_offset_t	addr;
	vm_address_t		alloc_addr;
	int			kr;

	if (zone == ZONE_NULL)
		panic ("zalloc: null zone");

	lock_zone(zone);
	REMOVE_FROM_ZONE(zone, addr, vm_offset_t);
	while (addr == 0) {
		/*
 		 *	If nothing was there, try to get more
		 */
		if (zone->doing_alloc) {
			/*
			 *	Someone is allocating memory for this zone.
			 *	Wait for it to show up, then try again.
			 */
			assert_wait((int)&zone->doing_alloc, TRUE);
			/* XXX say wakeup needed */
			unlock_zone(zone);
			thread_block();
			lock_zone(zone);
		}
		else {
			if ((zone->cur_size + (zone->pageable ?
				zone->alloc_size : zone->elem_size)) >
			    zone->max_size) {
				if (zone->exhaustible)
					break;
				/*
				 * Printf calls logwakeup, which calls
				 * select_wakeup which will do a zfree
				 * (which tries to take the select_zone
				 * lock... Hang.  Release the lock now
				 * so it can be taken again later.
				 * NOTE: this used to be specific to
				 * the select_zone, but for
				 * cleanliness, we just unlock all
				 * zones before this.
				 */
				if (zone->expandable) {
					/*
					 * We're willing to overflow certain
					 * zones, but not without complaining.
					 *
					 * This is best used in conjunction
					 * with the collecatable flag. What we
					 * want is an assurance we can get the
					 * memory back, assuming there's no
					 * leak. 
					 */
					zone->max_size += (zone->max_size >> 1);
					unlock_zone(zone);
#if MACH_ASSERT
					log(LOG_WARNING,
					 "zalloc: zone \"%s\" expanded to %d\n",
						zone->zone_name,zone->max_size);
#endif /* MACH_ASSERT */
					lock_zone(zone);
				} else if (!zone_ignore_overflow) {
					unlock_zone(zone);
					printf("zone \"%s\" empty.\n",
						zone->zone_name);
					panic("zalloc");
				}
			}

			if (zone->pageable)
				zone->doing_alloc = TRUE;
			unlock_zone(zone);

			if ((kr = vm_allocate(mach_task_self(),
					&alloc_addr,
					zone->alloc_size,
					TRUE))
				!= KERN_SUCCESS) {

			    if (zone->exhaustible)
				break;
			    panic("zalloc:%d", kr);
			}
			if (!zone->pageable) {
			    (void) vm_pageable(mach_task_self(),
						alloc_addr,
						zone->alloc_size,
						VM_PROT_READ|VM_PROT_WRITE);
			}
			zcram(zone, alloc_addr, zone->alloc_size);
			lock_zone(zone);
			zone->doing_alloc = FALSE;
			/* XXX check before doing this */
			thread_wakeup((int)&zone->doing_alloc);
			REMOVE_FROM_ZONE(zone, addr, vm_offset_t);
		}
	}

	unlock_zone(zone);
	return(addr);
}

/*
 *	zget returns an element from the specified zone
 *	and immediately returns nothing if there is nothing there.
 *
 *	This form should be used when you can not block (like when
 *	processing an interrupt).
 */
vm_offset_t zget(zone)
	register zone_t	zone;
{
	register vm_offset_t	addr;

	if (zone == ZONE_NULL)
		panic ("zalloc: null zone");

	lock_zone(zone);
	REMOVE_FROM_ZONE(zone, addr, vm_offset_t);
	unlock_zone(zone);

	return(addr);
}

/*	Free an element back to a zone.
 */
void zfree(zone, elem)
	register zone_t	zone;
	vm_offset_t	elem;
{
	lock_zone(zone);
	ADD_TO_ZONE(zone, elem);
	unlock_zone(zone);
}


/*	Change a zone's flags.
 *	This routine must be called immediately after zinit.
 */
void zchange(zone, pageable, sleepable, exhaustible, collectable)
	zone_t		zone;
	boolean_t	pageable;
	boolean_t	sleepable;
	boolean_t	exhaustible;
	boolean_t	collectable;
{
	zone->pageable = pageable;
	zone->sleepable = sleepable;
	zone->exhaustible = exhaustible;
	zone->collectable = collectable;
	lock_zone_init(zone);
}
