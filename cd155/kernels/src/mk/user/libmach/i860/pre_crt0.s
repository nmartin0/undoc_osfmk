// 
// $Copyright
// Copyright 1991 , 1994, 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
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
 * Copyright 1991 by Intel Corporation,
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
 * $Log: pre_crt0.s,v $
// Revision 1.5  1994/11/18  21:06:41  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  23:05:48  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:59:57  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  19:06:05  dleslie
// First R1_0 release
//
 * Revision 2.2.2.2  92/04/08  15:47:12  jeffreyh
 * 	moved __set_cthread_stack to crt0.c
 * 	[92/04/06            jeffreyh]
 * 
 * Revision 2.2.2.1  92/03/28  10:23:35  jeffreyh
 * 	cthread stack botch fix.
 * 	[92/03/26            cfj]
 * 
 * Revision 2.2  91/12/10  16:33:05  jsb
 * 	New files from Intel
 * 	[91/12/10  16:10:50  jsb]
 * 
 */ 

/*
 *	C start up routine.
 *
 *      Intel SSD i860 entry assumptions: [ Wed Sep 11 13:44:22 PDT 1991 ]
 *
 *      User`s stack (grows down in memory) high memory here.
 *
 *      =================
 *      |     <null>    |
 *      =================
 *      |    envp[1]    |
 *      =================
 *      |    envp[0]    |<--    r18     address of envp[0]
 *      =================
 *      |     <null>    |
 *      =================
 *      |  argv[argc-1] |
 *      =================
 *      |     argv[0]   |<--    r17     address of argv[0]
 *      =================
 *      |      argc     |<--    r16     contents of top-of-stack (argc)
 *      =================
 *
 *  This module is used to launch the OSF1AD server in such a manner so that we
 *  can call the PGI crt0.c module in the same manner as applications!!
 * PGI crt0.c module assumes it was called like a normal i860 C function:
 * (argc,argv,envp) in registers r16,r17,r18. What we do here is to load the
 * appro. registers and then call the crt0 startup routine. Someday the mk
 * should do this for us!
 *
 * inputs:
 *	User stack is valid with argc, argv & envp args on it.
 *	sp --> points to argc (top-of-stack).
 */
	.text
	.align	4
	.globl	start
	.globl	__start
	nop					/* Pad first page */
	nop
	nop
	nop
start::	
__start::
	ld.l	0(sp),r16			/* argc	*/
	addu	4,sp,r17			/* argv */
	shl	2,r16,r31
	addu	4,r31,r31
	addu	r31,r17,r18			/* envp */
	call	__crt0_start
	  nop
	bri	r1				/* return to mk?? */
	  nop


