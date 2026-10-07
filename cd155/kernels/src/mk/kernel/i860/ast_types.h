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
 * $Log: ast_types.h,v $
 * Revision 2.5  1994/11/18  20:38:19  mtm
 * Copyright additions/changes
 *
 * Revision 2.4  1993/06/30  22:30:05  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.3  1992/09/22  17:03:08  SSD
 * *** empty log message ***
 *
 * Revision 2.3  91/05/14  16:03:42  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:30:46  dbg
 * 	Copied for i386.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.2  90/08/27  22:13:15  dbg
 * 	Created for VAX.
 * 	[90/07/26  17:07:26  dbg]
 * 
 * Revision 2.3  89/10/15  02:07:01  rpd
 * 	Minor cleanups.
 * 
 * Revision 2.2  89/10/12  21:37:18  dlb
 * 	Get rid of NCPUS wrapper, add multiple include support.
 * 	[89/10/12            dlb]
 * 
 * Revision 2.1  89/10/12  21:33:15  dlb
 * Created.
 * 
 */

#ifndef	_I860_AST_TYPES_H_
#define	_I860_AST_TYPES_H_

/*
 *	Data type for remote ast_check() invocation support.
 */
typedef	int	ast_check_t;

#endif	/* _I860_AST_TYPES_H_ */
