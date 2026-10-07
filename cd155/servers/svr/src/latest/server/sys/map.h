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
 * $Log: map.h,v $
 * Revision 1.4  1994/11/18  20:40:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:25:45  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:36  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:07  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:08:10  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:43:36  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:52:01  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:57  gm]
 * 
 * Revision 1.2  90/01/02  20:15:58  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:18  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:19:19  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.5  89/03/09  22:05:30  rpd
 * 	More cleanup.
 * 
 * Revision 2.4  89/02/25  17:54:36  gm0w
 * 	Got rid of MACH conditionals and the non-MACH code.
 * 	[89/02/13            mrt]
 * 
 * Revision 2.3  88/08/24  02:34:31  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:16:39  mwyoung]
 * 
 * Revision 0.0  87/08/17            dbg
 * 	MACH: Replaced user_pt_map by kernel_pageable_map.
 * 	Removed swapmap, nswapmap, argmap (not used).
 * 	[87/08/17            dbg]
 * 
 * Revision 0.0  86/02/25            avie
 * 	Installed VM changes.
 * 	[86/02/25            avie]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)map.h	7.1 (Berkeley) 6/4/86
 */

/*
 * Resource Allocation Maps.
 *
 * Associated routines manage sub-allocation of an address space using
 * an array of segment descriptors.  The first element of this array
 * is a map structure, describing the arrays extent and the name
 * of the controlled object.  Each additional structure represents
 * a free segment of the address space.
 *
 * A call to rminit initializes a resource map and may also be used
 * to free some address space for the map.  Subsequent calls to rmalloc
 * and rmfree allocate and free space in the resource map.  If the resource
 * map becomes too fragmented to be described in the available space,
 * then some of the resource is discarded.  This may lead to critical
 * shortages, but is better than not checking (as the previous versions
 * of these routines did) or giving up and calling panic().  The routines
 * could use linked lists and call a memory allocator when they run
 * out of space, but that would not solve the out of space problem when
 * called at interrupt time.
 *
 * N.B.: The address 0 in the resource address space is not available
 * as it is used internally by the resource map routines.
 */

#ifndef	_SYS_MAP_H_
#define _SYS_MAP_H_

struct map {
	struct	mapent *m_limit;	/* address of last slot in map */
	char	*m_name;		/* name of resource */
/* we use m_name when the map overflows, in warning messages */
};
struct mapent
{
	int	m_size;		/* size of this segment of the map */
	int	m_addr;		/* resource-space addr of start of segment */
};

#ifndef	OSF1_SERVER
#ifdef	KERNEL
#include <vm/vm_map.h>

extern vm_map_t	kernel_pageable_map;
extern vm_map_t	mb_map;

#endif	KERNEL
#endif	OSF1_SERVER
#endif	_SYS_MAP_H_
