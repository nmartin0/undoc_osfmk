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
 * @OSF_FREE_COPYRIGHT@
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
 * $Log: zalloc.h,v $
 * Revision 1.2  1994/11/19  02:38:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/23  01:08:26  stans
 * Initial revision
 *
 * Revision 1.2  1992/05/12  14:52:01  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  08:22:08  condict]
 *
 * Revision 2.2  91/07/06  15:15:06  jsb
 * 	First checkin.
 * 
 * $EndLog$
 */

#ifndef	_KERN_ZALLOC_H_
#define _KERN_ZALLOC_H_

#include <mach.h>

typedef struct zone {
	vm_size_t	elem_size;	/* size of an element */
	char		*zone_name;	/* a name for the zone */
} *zone_t;

#define		ZONE_NULL	((zone_t) 0)

extern vm_offset_t	zalloc();
extern zone_t		zinit();
extern void		zfree();

#endif	_KERN_ZALLOC_H_
