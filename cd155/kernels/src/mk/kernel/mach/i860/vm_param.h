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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: vm_param.h,v $
 * Revision 1.6  1994/11/18  20:54:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/10/27  20:01:21  andyp
 * Initial memory expansion card support.
 *
 * Revision 1.4  1993/06/30  22:48:54  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:40:15  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:41:37  dleslie
 * First R1_0 release
 *
 * Revision 2.6  1993/02/09  18:04:35  stans
 * SSD version T8
 *
 * Revision 1.1  1992/09/30  02:31:09  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.5.2.3  92/09/15  17:55:33  jeffreyh
 * 	Unify paragon and iPSC
 * 	[92/09/15            jeffreyh]
 * 
 * Revision 2.5.2.2  92/05/27  00:46:43  jeffreyh
 * 	Changed KERNEL_STACK_SIZE to 16K (4*I860_PGBYTES)
 * 	[andyp@ssd.intel.com]
 * 	Added comments to note kernel-stack size MUST be a power of two.
 * 	Initial Paragon support.
 * 	[stans@ssd.intel.com]
 * 
 * Revision 2.5.2.1  92/03/28  10:10:58  jeffreyh
 * 	Make phystokv() a real routine.(andyp)
 * 	[92/03/20  13:22:16  jeffreyh]
 * 
 * Revision 2.5  91/12/10  16:32:48  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:46  jsb]
 * 
 * Revision 2.4  91/05/14  16:53:49  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:32:59  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:11:44  mrt]
 * 
 * Revision 2.2  90/12/04  14:51:19  jsb
 * 	First checkin.
 * 	[90/12/03  22:05:31  jsb]
 * 
 */
/* CMU_ENDHIST */
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
 */
/*
 * Machine dependent virtual memory parameters for i860.
 * Derived from i386 version.
 */

#ifndef	_MACH_I860_VM_PARAM_H_
#define _MACH_I860_VM_PARAM_H_

#define BYTE_SIZE	8	/* byte size in bits */

#define I860_PGBYTES	4096	/* bytes per 80860 page */
#define I860_PGSHIFT	12	/* number of bits to shift for pages */

/*
 *	Convert bytes to pages and convert pages to bytes.
 *	No rounding is used.
 */

#define i860_btop(x)		(((unsigned)(x)) >> I860_PGSHIFT)
#define i860_ptob(x)		(((unsigned)(x)) << I860_PGSHIFT)

/*
 *	Round off or truncate to the nearest page.  These will work
 *	for either addresses or counts.  (i.e. 1 byte rounds to 1 page
 *	bytes.
 */

#define i860_round_page(x)	((((unsigned)(x)) + I860_PGBYTES - 1) & \
					~(I860_PGBYTES-1))
#define i860_trunc_page(x)	(((unsigned)(x)) & ~(I860_PGBYTES-1))

#define VM_MIN_ADDRESS		((vm_offset_t) 0)
#define VM_MAX_ADDRESS		((vm_offset_t) 0x80000000)
#define VM_MIN_TEXT_ADDRESS     ((vm_offset_t) 0x80000000)
#define VM_MIN_KERNEL_ADDRESS	((vm_offset_t) 0x80000000)
#define VM_MAX_KERNEL_ADDRESS	((vm_offset_t) 0xffffffff)

/*
 * ***** WARNING *****
 *
 * stack size MUST be a power of 2 !!
 */
#define KERNEL_STACK_SIZE	(4*I860_PGBYTES)

/*
 *	Conversion between 80860 pages and VM pages
 */

#define trunc_i860_to_vm(p)	(atop(trunc_page(i860_ptob(p))))
#define round_i860_to_vm(p)	(atop(round_page(i860_ptob(p))))
#define vm_to_i860(p)		(i860_btop(ptoa(p)))

#endif	_MACH_I860_VM_PARAM_H_
