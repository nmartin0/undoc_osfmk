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
 * $Log: kalloc.h,v $
 * Revision 1.5  1994/11/18  20:32:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:01:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:20  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:31  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1991/10/14  12:33:48  sjs
 * 	91/09/13  12:50:31  sp
 * 	remove hackery for PAGE_SIZE and vm_pageable
 *
 * Revision 2.2  91/08/31  13:37:33  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/27  14:40:07  sp
 * Add compatibility macro for vm_pageable. This is temporary.
 * 
 * Revision 3.1  91/08/07  16:59:59  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  13:53:14  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:57:25  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:53:56  gm]
 * 
 * Revision 1.2  90/01/02  20:05:51  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:35:34  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:18:13  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.10  89/03/09  20:13:03  rpd
 * 	More cleanup.
 * 
 * Revision 2.9  89/02/25  18:04:45  gm0w
 * 	Kernel code cleanup.
 * 	Put entire file under #ifdef KERNEL
 * 	[89/02/15            mrt]
 * 
 * Revision 2.8  89/02/07  01:01:53  mwyoung
 * 	Relocated from sys/kalloc.h
 * 
 * Revision 2.7  89/01/18  02:10:51  jsb
 * 	Fixed log.
 * 	[88/01/18            rpd]
 * 
 * Revision 2.2  89/01/18  01:16:25  jsb
 * 	Use MINSIZE of 16 instead of 64 (mostly for afs);
 * 	eliminate NQUEUES (see kalloc.c).
 * 	[89/01/13            jsb]
 * 
 * 	26-Oct-87 Peter King (king) at NeXT, Inc.
 * 	Created.
 * 
 * $EndLog$
 */

#ifndef	_KERN_KALLOC_H_
#define _KERN_KALLOC_H_

#include <sys/types.h>

#define MINSIZE 16

extern caddr_t kalloc();
extern caddr_t kget();

#ifdef	lint
extern void kfree();
#endif	lint

#endif	_KERN_KALLOC_H_
