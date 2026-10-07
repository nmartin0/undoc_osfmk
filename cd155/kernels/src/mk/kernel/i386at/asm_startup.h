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
 * $Log: asm_startup.h,v $
 * Revision 1.5  1994/11/18  20:34:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:25:56  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:24:40  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:24:24  dleslie
 * First R1_0 release
 *
 * Revision 2.3.10.1  92/02/18  18:50:48  jeffreyh
 * 	Support for Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.3  91/05/14  16:19:14  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:44:00  dbg
 * 	Created (from old i386/start.s).
 * 	[91/04/26  14:40:03  dbg]
 * 
 */

/*
 * Startup code for an i386 on an AT.
 * Kernel is loaded starting at 1MB.
 * Protected mode, paging disabled.
 *
 * %esp ->	boottype
 *		size of extended memory (K)
 *		size of conventional memory (K)
 *		boothowto
 *		esym (if KDB set up)
 *
 */
#include <platforms.h>

	popl	_boottype+KVTOPHYS	/ get boottype
	popl	_extmem+KVTOPHYS	/ extended memory, in K
	popl	_cnvmem+KVTOPHYS	/ conventional memory, in K
	popl	%eax			/ boothowto
	movl	%eax,_boothowto+KVTOPHYS / save

#if	MACH_KDB
	testl	$RB_KDB,%eax		/ did boot load symbols?
	je	0f			/ if so:
	popl	%eax			/ get esym
	jmp	1f
0:
	xorl	%eax,%eax		/ no symbols loaded
1:
#else	MACH_KDB
	xorl	%eax,%eax		/ no symbols loaded
#endif	MACH_KDB

#ifdef	CBUS
#include <cbus/cbus.h>
	orl	%eax,%eax		/ test it
	je	2f
	addl	$CBUS_START, %eax
2:
#endif	CBUS

