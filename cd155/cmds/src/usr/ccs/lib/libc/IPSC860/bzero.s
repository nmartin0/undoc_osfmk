// 
// $Copyright
// Copyright 1993, 1994, 1995  Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
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
// Revision 2.4  1994/11/19  02:10:37  mtm
// Copyright additions/changes
//
// Revision 2.3  1994/02/01  17:05:55  jlitvin
// Fixes PTS #7968.  Byte manipulations are too slow!  Take Andy's bzero() code
// from libmach.a and use it for the NULL case in memset().
//
//  Reviewer: cfj
//  Risk: Medium
//  Benefit or PTS #: 7968
//  Testing: Prudential
//  Module(s): libc/memset.c libc/IPSC860/bzero.s
//
// Revision 2.2.16.1  1994/02/01  01:38:15  jlitvin
// Fixes PTS #7968.  Byte manipulations are too slow!  Take Andy's bzero() code
// from libmach.a and use it for the NULL case in memset().
//
//  Reviewer: cfj
//  Risk: Medium
//  Benefit or PTS #: 7968
//  Testing: Prudential
//  Module(s): libc/memset.c libc/IPSC860/bzero.s
//
// Revision 2.2  1992/04/27  14:02:18  stans
// "OSF/1.0.4"
//
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
