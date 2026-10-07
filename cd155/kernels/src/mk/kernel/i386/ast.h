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
 * $Log: ast.h,v $
 * Revision 1.5  1994/11/18  20:32:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:08  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:21:43  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:21:13  dleslie
 * First R1_0 release
 *
 * Revision 2.4.3.2  92/04/08  15:43:14  jeffreyh
 * 	Define MACHINE_AST_PER_THREAD (from David Golub).
 * 	[92/04/05            dlb]
 * 
 * Revision 2.4.3.1  92/03/03  16:14:16  jeffreyh
 * 	Pick up changes from MK68
 * 	[92/02/26  11:04:27  jeffreyh]
 * 
 * Revision 2.5  92/01/03  20:04:28  dbg
 * 	Add AST_I386_FP to handle delayed floating-point exceptions.
 * 	[91/10/29            dbg]
 * 
 * Revision 2.4  91/05/14  16:03:02  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:10:46  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:30:36  mrt]
 * 
 * Revision 2.2  90/05/03  15:24:36  dbg
 * 	Created.
 * 	[89/02/21            dbg]
 * 
 */

#ifndef	_I386_AST_H_
#define	_I386_AST_H_

/*
 * Machine-dependent AST file for machines with no hardware AST support.
 *
 * For the I386, we define AST_I386_FP to handle delayed
 * floating-point exceptions.  The FPU may interrupt on errors
 * while the user is not running (in kernel or other thread running).
 */

#define	AST_I386_FP	0x80000000
#define MACHINE_AST_PER_THREAD  AST_I386_FP


#endif	/* _I386_AST_H_ */
