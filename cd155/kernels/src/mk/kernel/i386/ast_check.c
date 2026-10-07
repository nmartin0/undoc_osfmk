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
 * $Log: ast_check.c,v $
 * Revision 1.5  1994/11/18  20:32:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:10  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:21:45  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:21:16  dleslie
 * First R1_0 release
 *
 * Revision 2.3.9.2  92/04/30  11:49:41  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.3.9.1  92/02/18  18:43:26  jeffreyh
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.3  91/05/14  16:03:21  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:30:25  dbg
 * 	Copied for i386.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.2  90/08/27  22:13:09  dbg
 * 	Created.
 * 	[90/07/18            dbg]
 * 
 */
#include <cpus.h>
#include <platforms.h>

#if	NCPUS > 1 && !AT386

/*
 * Handle signalling ASTs on other processors.
 *
 * Initial i386 implementation does nothing.
 */

#include <kern/processor.h>

/*
 * Initialize for remote invocation of ast_check.
 */
init_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif lint
}

/*
 * Cause remote invocation of ast_check.  Caller is at splsched().
 */
cause_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif lint
}

#endif	/* NCPUS > 1 && !AT386 */
