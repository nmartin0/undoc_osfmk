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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log: macro_help.h,v $
 * Revision 1.5  1994/11/18  20:52:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:45:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:36:11  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:36:35  dleslie
 * First R1_0 release
 *
 * Revision 2.3  91/05/14  16:44:49  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/05  17:28:09  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:15:31  mrt]
 * 
 * Revision 2.1  89/08/03  15:53:45  rwd
 * Created.
 * 
 * Revision 2.2  88/10/18  03:36:20  mwyoung
 * 	Added a form of return that can be used within macros that
 * 	does not result in "statement not reached" noise.
 * 	[88/10/17            mwyoung]
 * 	
 * 	Add MACRO_BEGIN, MACRO_END.
 * 	[88/10/11            mwyoung]
 * 	
 * 	Created.
 * 	[88/10/08            mwyoung]
 * 
 */
/*
 *	File:	kern/macro_help.h
 *
 *	Provide help in making lint-free macro routines
 *
 */  

#ifndef	_KERN_MACRO_HELP_H_
#define	_KERN_MACRO_HELP_H_

#include <mach/boolean.h>

#ifdef	lint
boolean_t	NEVER;
boolean_t	ALWAYS;
#else	lint
#define		NEVER		FALSE
#define		ALWAYS		TRUE
#endif	lint

#define		MACRO_BEGIN	do {
#define		MACRO_END	} while (NEVER)

#define		MACRO_RETURN	if (ALWAYS) return

#endif	_KERN_MACRO_HELP_H_
