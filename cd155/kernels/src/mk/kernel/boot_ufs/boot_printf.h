/*
 * 
 * $Copyright
 * Copyright 1991, 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log: boot_printf.h,v $
 * Revision 1.5  1994/11/18  20:24:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:16:04  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:13:35  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:06  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/05/14  15:21:02  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:00:29  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:25  mrt]
 * 
 * Revision 2.2  90/01/11  11:40:58  dbg
 * 	Created.
 * 	[89/12/20            dbg]
 * 
 */

/*
 * Redefine print routines for bootstrap task to use the boot_ print
 * routines.
 */

#define	printf	boot_printf
#define	gets	boot_gets
#define	panic	boot_panic
