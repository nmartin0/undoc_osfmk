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
 * $Log: _setjmp.s,v $
// Revision 2.8  1994/11/18  20:38:14  mtm
// Copyright additions/changes
//
// Revision 2.7  1994/07/12  19:18:12  andyp
// Merge of the NORMA2 branch back to the mainline.
//
// Revision 2.6.8.1  1994/02/04  08:13:50  andyp
// Purged some ancient PGI vs. Greenhills cruft.
//
// Revision 2.6  1993/06/30  22:29:58  dleslie
// Adding copyright notices required by legal folks
//
// Revision 2.5  1992/09/22  16:56:52  SSD
// *** empty log message ***
//
 * Revision 2.5  91/12/10  16:30:08  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:23  jsb]
 * 
 * Revision 2.4  91/08/28  11:12:03  jsb
 * 	From Intel SSD: new implementation.
 * 	[91/08/26  16:01:36  jsb]
 * 
 * Revision 2.3  91/06/18  20:50:41  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:48:26  jsb]
 * 
 * Revision 2.2  90/12/04  14:47:51  jsb
 * 	First checkin.
 * 	[90/12/03  21:24:07  jsb]
 * 
 */

	.file	"_setjmp.s"

	.dsect
u_r0:	.long		// not restored (but saved)
u_r1:	.long		// return address
u_sp:	.long		// stack pointer
u_fp:	.long		// frame pointer
u_r4:	.long		// non-volatile
u_r5:	.long		// non-volatile
u_r6:	.long		// non-volatile
u_r7:	.long		// non-volatile
u_r8:	.long		// non-volatile
u_r9:	.long		// non-volatile
u_r10:	.long		// non-volatile
u_r11:	.long		// non-volatile
u_r12:	.long		// non-volatile
u_r13:	.long		// non-volatile
u_r14:	.long		// non-volatile
u_r15:	.long		// non-volatile
u_f0:	.long		// not restored (but saved)
u_f1:	.long		// not restored (but saved)
u_f2:	.long		// non-volatile
u_f3:	.long		// non-volatile
u_f4:	.long		// non-volatile
u_f5:	.long		// non-volatile
u_f6:	.long		// non-volatile
u_f7:	.long		// non-volatile
	.end

	//
	//	If we could make promises about alignment, then
	//	doubles and/or quads could be used.  We can't,
	//	so we won't.
	//
	.text
	.align	4
__setjmp::
	st.l	r0,u_r0(r16)		// always 0
	st.l	r1,u_r1(r16)		// return address
	st.l	sp,u_sp(r16)		// stack pointer
	st.l	fp,u_fp(r16)		// frame pointer
	st.l	r4,u_r4(r16)
	st.l	r5,u_r5(r16)
	st.l	r6,u_r6(r16)
	st.l	r7,u_r7(r16)
	st.l	r8,u_r8(r16)
	st.l	r9,u_r9(r16)
	st.l	r10,u_r10(r16)
	st.l	r11,u_r11(r16)
	st.l	r12,u_r12(r16)
	st.l	r13,u_r13(r16)
	st.l	r14,u_r14(r16)
	st.l	r15,u_r15(r16)
	fst.l	f0,u_f0(r16)
	fst.l	f1,u_f1(r16)
	fst.l	f2,u_f2(r16)
	fst.l	f3,u_f3(r16)
	fst.l	f4,u_f4(r16)
	fst.l	f5,u_f5(r16)
	fst.l	f6,u_f6(r16)
	fst.l	f7,u_f7(r16)
	bri	r1
	 mov	r0,r16			// return 0

	.text
	.align	4
__longjmp::
	mov	r17,r18			// return v of _longjmp(e,v)
	//ld.l	u_r0(r16),r0		// always 0
	ld.l	u_r1(r16),r1		// return address
	ld.l	u_sp(r16),sp		// stack pointer
	ld.l	u_fp(r16),fp		// frame pointer
	ld.l	u_r4(r16),r4
	ld.l	u_r5(r16),r5
	ld.l	u_r6(r16),r6
	ld.l	u_r7(r16),r7
	ld.l	u_r8(r16),r8
	ld.l	u_r9(r16),r9
	ld.l	u_r10(r16),r10
	ld.l	u_r11(r16),r11
	ld.l	u_r12(r16),r12
	ld.l	u_r13(r16),r13
	ld.l	u_r14(r16),r14
	ld.l	u_r15(r16),r15
	//fld.l	u_f0(r16),f0
	//fld.l	u_f1(r16),f1
	fld.l	u_f2(r16),f2
	fld.l	u_f3(r16),f3
	fld.l	u_f4(r16),f4
	fld.l	u_f5(r16),f5
	fld.l	u_f6(r16),f6
	fld.l	u_f7(r16),f7
	bri	r1
	 mov	r18,r16			// return v
