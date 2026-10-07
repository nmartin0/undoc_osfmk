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
 * $Log: zalloc.h,v $
 * Revision 1.5  1994/11/18  20:33:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:02:48  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:17:51  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:12  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1991/10/14  12:34:08  sjs
 * 	91/10/01  14:09:25  condict
 * 	Always use simple locks now, even in pageable zones (efficiency).
 *
 * 	91/09/20  12:16:12  condict
 * 	Move lock_zone macro from zalloc.c to zalloc.h to allow its use by ZALLOC
 * 	macro (fixes major zone allocation bug).  Also removed all ifdef OSF1_SERVER.
 *
 * Revision 2.2  91/08/31  13:38:27  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/07  17:01:19  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/31  13:58:16  devrcs
 * 	Add "expandable" flag to zone, see zalloc.c.
 * 	[90/10/11  08:17:18  tmt]
 * 
 * Revision 1.5  90/10/07  13:58:01  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  10:01:09  gm]
 * 
 * Revision 1.4  90/06/22  20:15:28  devrcs
 * 	Move the accounting for zones' garbage collection out of the
 * 	macros and into zone_gc, the garbage collection routine itself.
 * 	[90/06/18  13:39:14  jvs]
 * 
 * 	Some cleanup work:
 * 	    - remove #if COLLECT_ZONE_GARBAGE
 * 	    - remove the temporary zcollectable routine.
 * 	[90/06/07  15:36:35  jvs]
 * 
 * Revision 1.3  90/03/27  13:19:04  gm
 * 	Incrementing RCS number so mkds will eat this.
 * 	[90/03/20  15:35:30  jvs]
 * 
 * 	Changes to add garbage collection to zones.
 * 	[90/03/19  11:07:35  jvs]
 * 
 * 	Add the "next" field to struct zone_page_table in order to create
 * 	singly linked lists of zone_page_table entries for garbage collection.
 * 	[90/03/13  10:51:42  jvs]
 * 
 * Revision 1.2  90/01/02  20:06:33  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:36:35  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:27:53  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.8  89/05/11  14:41:36  gm0w
 * 	Added next_zone field, to link all zones onto a list.
 * 	[89/05/08  21:35:14  rpd]
 * 
 * Revision 2.7  89/03/09  20:17:58  rpd
 * 	More cleanup.
 * 
 * Revision 2.6  89/02/25  18:11:22  gm0w
 * 	Kernel code cleanup.
 * 	Put macros under #indef KERNEL.
 * 	[89/02/15            mrt]
 * 
 * Revision 2.5  89/02/07  01:06:22  mwyoung
 * 	Relocated from sys/zalloc.h
 * 
 * Revision 2.4  89/01/15  16:36:01  rpd
 * 	Use decl_simple_lock_data.
 * 	[89/01/15  15:20:28  rpd]
 * 
 * Revision 2.3  88/12/19  02:52:11  mwyoung
 * 	Use MACRO_BEGIN and MACRO_END.  This corrects a problem
 * 	in the ZGET macro under lint.
 * 	[88/12/09            mwyoung]
 * 
 * Revision 2.2  88/08/24  02:56:21  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:30:22  mwyoung]
 * 
 * Revision 0.0  88/01/08            rfr
 * 	Pageable zone lock added. NOTE ZALLOC and ZFREE macros assume
 * 	non-pageable zones.
 * 	[88/01/08            rfr]
 * 
 * Revision 0.0  87/09/30            mwyoung
 * 	Make ZALLOC, ZFREE not macros for lint.
 * 	[87/09/30            mwyoung]
 * 
 * Revision 0.0  87/09/12            avie
 * 	Modified to use a list instead of a queue - no need for expense
 * 	of queue.  Also removed warning about assembly language hacks as
 * 	there are none left that I know of.
 * 	[87/09/12            avie]
 * 
 * Revision 0.0  87/09/01            mwyoung
 * 	Added zchange() declaration; added sleepable, exhaustible flags.
 * 	[87/09/01            mwyoung]
 * 
 * Revision 0.0  87/04/18            avie
 * 	Added ZALLOC and ZGET macros, note that the arguments are
 * 	different that zalloc and zget due to language restrictions.
 * 	For consistency, also made a ZFREE macro, zfree is now always
 * 	a procedure call.
 * 	[87/04/18            avie]
 * 
 * Revision 0.0  87/04/15            avie
 * 	Added warning about implementations that inline expand zalloc
 * 	and zget.
 * 	[87/04/15            avie]
 * 
 * Revision 0.0  87/03/23            avie
 * 	Simplified zfree macro.
 * 	[87/03/23            avie]
 * 
 * Revision 0.0  87/03/09            mwyoung
 * 	Make zfree a macro: a big win on a register-based machine
 * 	with expensive procedure call; smaller change elsewhere.
 * 	[87/03/09            mwyoung]
 * 
 * Revision 0.0  87/03/03            mwyoung
 * 	Reduce include of "../h/types.h" to "../mach/machine/vm_types.h".
 * 	[87/03/03            mwyoung]
 * 
 * Revision 0.0  87/02/12            rds
 * 	Added zget.
 * 	[87/02/12            rds]
 * 
 * Revision 0.0  87/01/12            mwyoung
 * 	Eliminated use of the interlocked queue package.
 * 	[87/01/12            mwyoung]
 * 
 * Revision 0.0  85/06/09            avie
 * 	Created.
 * 	[85/06/09            avie]
 * 
 * $EndLog$
 */
/*
 *	File:	zalloc.h
 *	Author:	Avadis Tevanian, Jr.
 *	Copyright (C) 1985, Avadis Tevanian, Jr.
 *
 */

#ifndef	_KERN_ZALLOC_H_
#define _KERN_ZALLOC_H_

#include <uxkern/import_mach.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/macro_help.h>

/*
 *	A zone is a collection of fixed size blocks for which there
 *	is fast allocation/deallocation access.  Kernel routines can
 *	use zones to manage data structures dynamically, creating a zone
 *	for each type of data structure to be managed.
 *
 */

typedef struct zone {
	decl_simple_lock_data(,lock)	/* generic lock */
	int		count;		/* Number of elements used now */
	vm_offset_t	free_elements;
	vm_size_t	cur_size;	/* current memory utilization */
	vm_size_t	max_size;	/* how large can this zone grow */
	vm_size_t	elem_size;	/* size of an element */
	vm_size_t	alloc_size;	/* size used for more memory */
	boolean_t	doing_alloc;	/* is zone expanding now? */
	char		*zone_name;	/* a name for the zone */
	unsigned int
	/* boolean_t */	pageable :1,	/* zone pageable? */
	/* boolean_t */	sleepable :1,	/* sleep if empty? */
	/* boolean_t */ exhaustible :1,	/* merely return if empty? */
	/* boolean_t */	collectable :1,	/* garbage collect empty pages */
	/* boolean_t */	expandable :1;	/* expand zone (with message)? */
	lock_data_t	complex_lock;	/* Lock for pageable zones */
	struct zone *	next_zone;	/* Link for all-zones list */
} *zone_t;

#define		ZONE_NULL	((zone_t) 0)


#if	(MACH_LDEBUG || MACH_LTRACKS)
/*
 * Some of this is useless given that we call the
 *  macro from within the functions below and we
 *  don't know who called the function, but the
 *  thread info should help that out.
 */
#define lock_zone(zone)						\
MACRO_BEGIN							\
	simple_lock(&zone->lock);				\
	/* zone->complex_lock.lck_addr = getpc(); */		\
	zone->complex_lock.thread = (char *)cthread_self();	\
MACRO_END

#define unlock_zone(zone)					\
MACRO_BEGIN							\
	zone->complex_lock.lck_addr |= 0x80000000;		\
	/* zone->complex_lock.unlck_addr = getpc(); */		\
	zone->complex_lock.thread = (char *)current_thread();	\
	simple_unlock(&zone->lock);				\
MACRO_END

#else	/* (MACH_LDEBUG || MACH_LTRACKS) */
#define lock_zone(zone)				\
MACRO_BEGIN					\
	simple_lock(&zone->lock);		\
MACRO_END

#define unlock_zone(zone)			\
MACRO_BEGIN					\
		simple_unlock(&zone->lock);	\
MACRO_END

#endif	/* (MACH_LDEBUG || MACH_LTRACKS) */

extern vm_offset_t	zalloc();
extern vm_offset_t	zget();
extern zone_t		zinit();
extern void		zfree();
extern void		zchange();

#define ADD_TO_ZONE(zone, element)					\
MACRO_BEGIN								\
		*((vm_offset_t *)(element)) = (zone)->free_elements;	\
		(zone)->free_elements = (vm_offset_t) (element);	\
		(zone)->count--;					\
MACRO_END

#define REMOVE_FROM_ZONE(zone, ret, type)				\
MACRO_BEGIN								\
	(ret) = (type) (zone)->free_elements;				\
	if ((ret) != (type) 0) {					\
		(zone)->count++;					\
		(zone)->free_elements = *((vm_offset_t *)(ret));	\
	}								\
MACRO_END

#define ZFREE(zone, element)		\
MACRO_BEGIN				\
	lock_zone(zone);		\
	ADD_TO_ZONE(zone, element);	\
	unlock_zone(zone);		\
MACRO_END

#define ZALLOC(zone, ret, type)			\
MACRO_BEGIN					\
	register zone_t	z = (zone);		\
						\
	lock_zone(z);				\
	REMOVE_FROM_ZONE(z, ret, type);		\
	unlock_zone(z);				\
	if ((ret) == (type)0)			\
		(ret) = (type)zalloc(z);	\
MACRO_END

#define ZGET(zone, ret, type)			\
MACRO_BEGIN					\
	register zone_t	z = (zone);		\
						\
	lock_zone(z);				\
	REMOVE_FROM_ZONE(z, ret, type);	\
	unlock_zone(z);				\
MACRO_END

extern void		zcram();
extern void		zone_init();

#endif	_KERN_ZALLOC_H_
