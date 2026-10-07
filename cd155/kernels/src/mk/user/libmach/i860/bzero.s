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
 * $Log: bzero.s,v $
// Revision 1.6  1994/11/18  21:06:38  mtm
// Copyright additions/changes
//
// Revision 1.5  1993/06/30  23:05:39  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.4  1993/05/17  23:09:41  andyp
// Moved some long-available i860 microkernel code (setjmp(), bcopy(), and
// bzero()) out into libmach so that the server can use it too.
//
 * Revision 1.3  1993/04/27  20:59:51  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:57  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/02/18  19:26:31  jeffreyh
 * 	[intel] moved bcopy to its own file.
 * 	[92/02/13  13:15:38  jeffreyh]
 * 
 * Revision 2.2  91/09/04  11:28:58  jsb
 * 	First checkin. Provided by Intel SSD.
 * 	[91/09/04  10:33:01  jsb]
 * 
 */

	.file	"bzero.s"
	.text
	.align	4
//
//	bzero(addr, cnt)
//
//	Make a few quick checks for best-case alignment,
//	then zero cnt bytes starting at addr.
//
_bzero::
_blkclr::				// wlb	XXX is this still needed?
	bte	r17,r0,.bzret
	adds	-1,r0,r19		// inc = -1 for bla
	or	r16,r17,r31		// tmp = src | cnt
	and	0x000f,r31,r0		// 16-byte aligned?
	bc.t	.bz16prep		//	zero 16-byte chunks,
	 shr	4,r17,r17		//	cnt = cnt / 16
	and	0x0007,r31,r0		// 8-byte aligned?
	bc.t	.bz8prep		//	zero 8-byte chunks,
	 shr	3,r17,r17		//	cnt = cnt / 8
	and	0x0003,r31,r0		// 4-byte aligned?
	bc.t	.bz4prep		//	zero 4-byte chunks,
	 shr	2,r17,r17		//	cnt = cnt / 4
	and	0x0001,r31,r0		// 2-byte aligned?
	bc.t	.bz2prep		//	zero 2-byte chunks,
	 shr	1,r17,r17		//	cnt = cnt / 2

	// oh well, zero bytes...
.bz1prep:
	adds	-1,r17,r17		// cnt -= 1
	bla	r19,r17,.bz1
	 nop
.bz1:	st.b	r0,0(r16)
	bla	r19,r17,.bz1
	 addu	1,r16,r16
.bzret:	bri	r1
	 nop

	// zero shorts...
.bz2prep:
	adds	-1,r17,r17		// cnt -= 1
	bla	r19,r17,.bz2
	 nop
.bz2:	st.s	r0,0(r16)
	bla	r19,r17,.bz2
	 addu	2,r16,r16
	bri	r1
	 nop

	// zero longs...
.bz4prep:
	adds	-1,r17,r17		// cnt -= 1
	bla	r19,r17,.bz4
	 adds	-4,r16,r16
.bz4:	bla	r19,r17,.bz4
	 fst.l	f0,4(r16)++
	bri	r1
	 nop

	// zero doubles...
.bz8prep:
	adds	-1,r17,r17		// cnt -= 1
	bla	r19,r17,.bz8
	 adds	-8,r16,r16
.bz8:	bla	r19,r17,.bz8
	 fst.d	f0,8(r16)++
	bri	r1
	 nop

	// zero quads...
.bz16prep:
	fmov.dd	f0,f16			// f16 = 0,f17 = 0
	adds	-1,r17,r17		// cnt -= 1
	fmov.dd	f0,f18			// f18 = 0,f19 = 0
	bla	r19,r17,.bz16
	 adds	-16,r16,r16
.bz16:	bla	r19,r17,.bz16
	 fst.q	f16,16(r16)++
	bri	r1
	 nop
