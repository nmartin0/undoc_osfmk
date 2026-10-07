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
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log: cthreads.h,v $
 * Revision 1.6  1994/11/18  21:07:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  23:06:41  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/24  18:45:52  andyp
 * Added volatile to the typedef for spin_lock_t.
 *
 * Revision 1.3  1993/04/27  21:00:54  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:07:52  dleslie
 * First R1_0 release
 *
 * Revision 2.2  91/09/04  11:29:17  jsb
 * 	First checkin. Derived from i386 sources by Intel/SSD.
 * 	[91/09/02  08:56:06  jsb]
 * 
 */

#ifndef _MACHINE_CTHREADS_H_
#define _MACHINE_CTHREADS_H_

typedef volatile int spin_lock_t;
#define SPIN_LOCK_INITIALIZER 0
#define spin_lock_init(s) *(s)=0
#define spin_lock_locked(s) (*(s) != 0)

#endif _MACHINE_CTHREADS_H_
