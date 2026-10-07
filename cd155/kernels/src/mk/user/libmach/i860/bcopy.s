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
 * Copyright (c) 1991 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: bcopy.s,v $
// Revision 1.6  1994/11/18  21:06:35  mtm
// Copyright additions/changes
//
// Revision 1.5  1993/06/30  23:05:37  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.4  1993/05/17  23:09:33  andyp
// Moved some long-available i860 microkernel code (setjmp(), bcopy(), and
// bzero()) out into libmach so that the server can use it too.
//
 * Revision 1.3  1993/04/27  20:59:49  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:54  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.2  92/05/27  10:05:54  jeffreyh
 * 	memcpy now returns correct value
 * 
 * Revision 2.2.2.1  92/02/18  19:26:21  jeffreyh
 * 	[intel] provide assembly language version.
 * 	[92/02/13  13:14:08  jeffreyh]
 * 
 * Revision 2.2  91/09/04  11:28:57  jsb
 * 	First checkin. Provided by Intel SSD.
 * 	[91/09/04  10:32:49  jsb]
 * 
 */

/*
 * char *memcopy(to,from,nbytes)
 * void bcopy(from,to,nbytes)
 *
 * inputs:
 *	to	destination address
 *	from	source address
 *	nbytes	# of bytes to copy
 *
 * outputs:
 *	bcopy() return void
 *	memcopy() returns (char *)to
 *
 *	XXX these loops don't handle overlapping regions (which require
 *	XXX the loops to run "backwards").
 */

	.file	"bcopy.s"
	.text
	.align	32
	.globl	_memcpy
	.globl	_bcopy

_memcpy::
	mov	r16,r30			// swap r16 and r17 -- to is also in r30
	mov	r17,r16
	mov	r30,r17
_bcopy::
	bte	r18,r0,.exit		// return if no bytes to copy
	or	r16,r17,r31		// tmp = src | dst
	or	r18,r31,r31		// tmp |= cnt
	and	0x000f,r31,r0		// 16-byte aligned?
	bc.t	.copy16			//	copy 16-byte chunks,
	 shr	4,r18,r18		// 	cnt = cnt / 16
	and	0x0007,r31,r0		// 8-byte aligned?
	bc.t	.copy8			//	copy 8-byte chunks,
	 shr	3,r18,r18		// 	cnt = cnt / 8
	and	0x0003,r31,r0		// 4-byte aligned?
	bc.t	.copy4			//	copy 4-byte chunks,
	 shr	2,r18,r18		// 	cnt = cnt / 4
	and	0x0001,r31,r0		// 2-byte aligned?
	bc.t	.copy2			//	copy 2-byte chunks,
	 shr	1,r18,r18		// 	cnt = cnt / 2

	//
	// do it the slow way...
	//
	// copy bytes...
.copy1:
	adds	-1,r0,r19		// inc = -1
	adds	-1,r18,r18		// cnt = cnt - 1
	bla	r19,r18,.drip1
	 nop
.drip1:
	ld.b	0(r16),r20
	addu	1,r16,r16
	st.b	r20,0(r17)		// slow if dcache miss...
	bla	r19,r18,.drip1
	 addu	1,r17,r17
.exit:
	bri	r1
	 mov	r30,r16

	// copy shorts...
.copy2:
	adds	-1,r0,r19		// inc = -1
	adds	-1,r18,r18		// cnt = cnt - 1
	bla	r19,r18,.drip2
	 nop
.drip2:
	ld.s	0(r16),r20
	addu	2,r16,r16
	st.s	r20,0(r17)		// slow if dcache miss...
	bla	r19,r18,.drip2
	 addu	2,r17,r17
	bri	r1
	 mov	r30,r16

	// copy longs...
.copy4:
	// one word lower (autopreincrement)
	addu	-4,r16,r16
	addu	-4,r17,r17

	// pipeline only when cnt >= 3 words
	addu	-3,r18,r0
	bnc	.small4		// if (cnt < 3) goto .small4;

	// prime the load pipe
	adds	-3,r18,r18	// count the 3 words in the pipe
	pfld.l	4(r16)++,f16	// ignore value returned
	adds	-16,r0,r19	// inc = -16
	pfld.l	4(r16)++,f20	// ignore value returned
	mov	r18,r20		// loop counter is r20
	bla	r19,r20,.primed4
	 pfld.l	4(r16)++,f24	// ignore value returned
.primed4:
	// pump the pipeline if at least 16 words left
	bla	r19,r20,.pump4	// taken if at least 16 words left
	 nop
	// can't batch if less than 16 words left, drain the pipe.
	br	.drain4
	 nop
	.align	32
.pump4:
	// batch read 16, then batch write 16 (64 bytes moved per loop)
	pfld.l	4(r16)++,f16
	pfld.l	4(r16)++,f17
	pfld.l	4(r16)++,f18
	pfld.l	4(r16)++,f19
	pfld.l	4(r16)++,f20
	pfld.l	4(r16)++,f21
	pfld.l	4(r16)++,f22
	pfld.l	4(r16)++,f23
	pfld.l	4(r16)++,f24
	pfld.l	4(r16)++,f25
	pfld.l	4(r16)++,f26
	pfld.l	4(r16)++,f27
	pfld.l	4(r16)++,f28
	pfld.l	4(r16)++,f29
	pfld.l	4(r16)++,f30
	pfld.l	4(r16)++,f31
	fst.l	f16,4(r17)++
	fst.l	f17,4(r17)++
	fst.l	f18,4(r17)++
	fst.l	f19,4(r17)++
	fst.l	f20,4(r17)++
	fst.l	f21,4(r17)++
	fst.l	f22,4(r17)++
	fst.l	f23,4(r17)++
	fst.l	f24,4(r17)++
	fst.l	f25,4(r17)++
	fst.l	f26,4(r17)++
	fst.l	f27,4(r17)++
	fst.l	f28,4(r17)++
	fst.l	f29,4(r17)++
	fst.l	f30,4(r17)++
	fst.l	f31,4(r17)++
	bla	r19,r20,.pump4
	 adds	-16,r18,r18	// moved 16 words
.drain4:
	// drain 3 remaining words in the pipe
	pfld.l	0(r16),f16
	pfld.l	0(r16),f17
	pfld.l	0(r16),f18
	fst.l	f16,4(r17)++
	fst.l	f17,4(r17)++
	fst.l	f18,4(r17)++
	bte	0,r18,.ret4

	// 1 <= cnt < 16 words left
.small4:
	adds	-1,r0,r19	// inc = -1
	adds	-1,r18,r18	// cnt -= 1
	bla	r19,r18,.drip4
	 nop
.drip4:
	fld.l	4(r16)++,f16
	bla	r19,r18,.drip4
	 fst.l	f16,4(r17)++
.ret4:
	bri	r1
	 mov	r30,r16

	// copy doubles...
.copy8:
.xr:
	// one double lower (autopreincrement)
	addu	-8,r16,r16
	addu	-8,r17,r17

	// pipeline only when cnt >= 3 doubles
	addu	-3,r18,r0
	bnc	.small8		// if (cnt < 3) goto .small8;

	// prime the load pipe
	adds	-3,r18,r18	// count the 3 doubles in the pipe
	pfld.d	8(r16)++,f16	// ignore value returned
	adds	-8,r0,r19	// inc = -8
	pfld.d	8(r16)++,f20	// ignore value returned
	mov	r18,r20		// loop counter is r20
	bla	r19,r20,.primed8
	 pfld.d	8(r16)++,f24	// ignore value returned
.primed8:
	// pump the pipeline if at least 8 doubles left
	bla	r19,r20,.pump8	// taken if at least 8 doubles left
	 nop
	// can't batch if less than 8 doubles left, drain the pipe.
	br	.drain8
	 nop
	.align	32
.pump8:
	// batch read 8, then batch write 8 (64 bytes moved per loop)
	pfld.d	8(r16)++,f16
	pfld.d	8(r16)++,f18
	pfld.d	8(r16)++,f20
	pfld.d	8(r16)++,f22
	pfld.d	8(r16)++,f24
	pfld.d	8(r16)++,f26
	pfld.d	8(r16)++,f28
	pfld.d	8(r16)++,f30
	fst.d	f16,8(r17)++
	fst.d	f18,8(r17)++
	fst.d	f20,8(r17)++
	fst.d	f22,8(r17)++
	fst.d	f24,8(r17)++
	fst.d	f26,8(r17)++
	fst.d	f28,8(r17)++
	fst.d	f30,8(r17)++
	bla	r19,r20,.pump8
	 adds	-8,r18,r18	// moved 8 doubles
.drain8:
	// drain 3 remaining doubles in the pipe
	pfld.d	0(r16),f16
	pfld.d	0(r16),f18
	pfld.d	0(r16),f20
	fst.d	f16,8(r17)++
	fst.d	f18,8(r17)++
	fst.d	f20,8(r17)++
	bte	0,r18,.ret8

	// 1 <= cnt < 8 doubles left
.small8:
	adds	-1,r0,r19	// inc = -1
	adds	-1,r18,r18	// cnt -= 1
	bla	r19,r18,.drip8
	 nop
.drip8:
	fld.d	8(r16)++,f16
	bla	r19,r18,.drip8
	 fst.d	f16,8(r17)++
.ret8:
	bri	r1
	 mov	r30,r16

	// copy quads...
.copy16:
// BEGIN OF i860XR SUPPORT
	//
	//	don't use pfld.q on the i860XR...
	//
	//	XXX this check could be removed for i860XP
	//
	ld.c	epsr,r31
	and	0x0002,r31,r0
	bc.t	.xr
	 shl	1,r18,r18	// correct r18 for 8-byte transfers
.xp:
// END OF i860XR SUPPORT
	// one quad lower (autopreincrement)
	addu	-16,r16,r16
	addu	-16,r17,r17

	// pipeline only when cnt >= 3 quads
	addu	-3,r18,r0
	bnc	.small16		// if (cnt < 3) goto .small16;

	// prime the load pipe
	adds	-3,r18,r18	// count the 3 quads in the pipe
	adds	-4,r0,r19	// inc = -4
	mov	r18,r20		// loop counter is r20
	pfld.q	16(r16)++,f16	// ignore value returned
	pfld.q	16(r16)++,f20	// ignore value returned
	bla	r19,r20,.primed16
	 pfld.q	16(r16)++,f24	// ignore value returned
.primed16:
	// pump the pipeline if at least 4 quads left
	bla	r19,r20,.pump16	// taken if at least 4 quads left
	 nop
	// can't batch if less than 4 quads left, drain the pipe.
	br	.drain16
	 nop
	.align	32
.pump16:
	// batch read 4, then batch write 4 (64 bytes moved per loop)
	pfld.q	16(r16)++,f16
	pfld.q	16(r16)++,f20
	pfld.q	16(r16)++,f24
	pfld.q	16(r16)++,f28
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	fst.q	f28,16(r17)++
	bla	r19,r20,.pump16
	 adds	-4,r18,r18	// moved 4 quads
.drain16:
	// drain 3 remaining quads in the pipe
	pfld.d	0(r16),f16	// drain w/ doubles in the pipe
	pfld.d	0(r16),f20	// drain w/ doubles in the pipe
	pfld.d	0(r16),f24	// drain w/ doubles in the pipe
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bte	0,r18,.ret16

	// 1 <= cnt < 4 quads left
.small16:
	adds	-1,r0,r19	// inc = -1
	adds	-1,r18,r18	// cnt -= 1
	bla	r19,r18,.drip16
	 nop
.drip16:
	fld.q	16(r16)++,f16
	bla	r19,r18,.drip16
	 fst.q	f16,16(r17)++
.ret16:
	bri	r1
	 mov	r30,r16
