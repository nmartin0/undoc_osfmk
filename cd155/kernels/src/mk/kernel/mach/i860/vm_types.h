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
 * $Log: vm_types.h,v $
 * Revision 1.5  1994/11/18  20:54:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:48:57  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:40:18  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:41:39  dleslie
 * First R1_0 release
 *
 * Revision 2.4  1993/02/09  18:05:24  stans
 * SSD version T8
 *
 * Revision 1.1  1992/09/30  02:31:10  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4  91/05/14  16:54:01  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:33:03  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:11:55  mrt]
 * 
 * Revision 2.2  90/12/04  14:51:24  jsb
 * 	First checkin.
 * 	[90/12/03  22:06:15  jsb]
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
 *	VM data type definitions for i860. Derived from i386 version.
 */

#ifndef	_MACH_I860_VM_TYPES_H_
#define _MACH_I860_VM_TYPES_H_

#ifdef	ASSEMBLER
#else	ASSEMBLER
typedef	unsigned int	vm_offset_t;
typedef	unsigned int	vm_size_t;
#endif	ASSEMBLER

#endif	_MACH_I860_VM_TYPES_H_
