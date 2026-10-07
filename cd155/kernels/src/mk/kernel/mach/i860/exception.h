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
 * $Log: exception.h,v $
 * Revision 1.5  1994/11/18  20:54:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:48:46  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:40:08  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:41:27  dleslie
 * First R1_0 release
 *
 * Revision 2.5  1993/02/09  18:01:52  stans
 * SSD version (T8).
 *
 * Revision 1.1  1992/09/30  02:31:02  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4.1.1  92/05/27  00:46:36  jeffreyh
 * 	More FPE support, integer overflow
 * 	[stans@ssd.intel.com]
 * 
 * Revision 2.4  91/05/14  16:53:17  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:32:41  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:11:06  mrt]
 * 
 * Revision 2.2  90/12/04  14:51:08  jsb
 * 	First checkin.
 * 	[90/12/03  22:01:48  jsb]
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
 * Codes and subcodes for i860 exceptions. Derived from i386 version.
 */

/*
 *	EXC_BAD_INSTRUCTION
 */

#ifndef	_MACH_I860_EXCEPTION_H_
#define _MACH_I860_EXCEPTION_H_


/*
 *	EXC_ARITHMETIC
 */
#define	EXC_i860_OVERFLOW		1

/*
 *	EXC_SOFTWARE
 */

/*
 *	EXC_BAD_ACCESS
 */
#define EXC_i860_PRIV			1
#define EXC_i860_IAT			2

/*
 *	EXC_BREAKPOINT
 */

#endif	_MACH_I860_EXCEPTION_H_
