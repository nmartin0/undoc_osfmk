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
 * $Log: ktss.c,v $
 * Revision 1.5  1994/11/18  20:33:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:24:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:53  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:39  dleslie
 * First R1_0 release
 *
 * Revision 2.5  91/05/14  16:10:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:12:39  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:35:28  mrt]
 * 
 * Revision 2.3  90/08/27  21:57:07  dbg
 * 	New selector names from new seg.h.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.2  90/05/03  15:33:35  dbg
 * 	Created.
 * 	[90/02/15            dbg]
 * 
 */

/*
 * Kernel task state segment.
 *
 * We don't use the i386 task switch mechanism.  We need a TSS
 * only to hold the kernel stack pointer for the current thread.
 *
 * XXX multiprocessor??
 */
#include <i386/tss.h>
#include <i386/seg.h>

struct i386_tss	ktss = {
	0,				/* back link */
	0,				/* esp0 */
	KERNEL_DS,			/* ss0 */
	0,				/* esp1 */
	0,				/* ss1 */
	0,				/* esp2 */
	0,				/* ss2 */
	0,				/* cr3 */
	0,				/* eip */
	0,				/* eflags */
	0,				/* eax */
	0,				/* ecx */
	0,				/* edx */
	0,				/* ebx */
	0,				/* esp */
	0,				/* ebp */
	0,				/* esi */
	0,				/* edi */
	0,				/* es */
	0,				/* cs */
	0,				/* ss */
	0,				/* ds */
	0,				/* fs */
	0,				/* gs */
	0,				/* ldt */
	0,				/* trace_trap */
	0x0FFF				/* IO bitmap offset -
					   beyond end of TSS segment,
					   so no bitmap */
};

