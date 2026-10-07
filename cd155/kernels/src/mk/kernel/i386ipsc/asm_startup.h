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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: asm_startup.h,v $
 * Revision 1.5  1994/11/18  20:37:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:04  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:31  dleslie
 * First R1_0 release
 *
 * Revision 2.6  91/12/10  16:29:47  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:05  jsb]
 * 
 * Revision 2.5  91/06/18  20:49:58  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  20:05:19  jsb]
 * 
 * Revision 2.4  91/06/17  15:44:04  jsb
 * 	Enabled support for new boot protocol, and eliminated support for old.
 * 	[91/06/17  10:00:10  jsb]
 * 
 * Revision 2.3  91/06/06  17:04:13  jsb
 * 	Added support for Paul's new bootstrapper.
 * 	[91/05/13  16:58:04  jsb]
 * 
 * Revision 2.2  91/05/08  12:45:07  dbg
 * 	Created (from old i386/start.s).
 * 	[91/04/26  14:40:58  dbg]
 * 
 */

/*
 * Get parameters for iPSC386.
 */
	popl	_boottype+KVTOPHYS	/ get boottype
	popl	_ipsc_basemem+KVTOPHYS	/ physical memory in K
	popl	_ipsc_physnode+KVTOPHYS	/ physical node number
	popl	_ipsc_slot+KVTOPHYS	/ physical slot number
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

	popl	_tmp_bootenv+KVTOPHYS	/ bootenv
	popl	_md_size+KVTOPHYS	/ get size
