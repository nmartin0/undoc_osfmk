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
 * Copyright (c) 1992 Carnegie Mellon University
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
 * Copyright 1992 by Intel Corporation,
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
 * $Log: dcm.s,v $
// Revision 1.3  1994/11/18  20:40:45  mtm
// Copyright additions/changes
//
// Revision 1.2  1993/06/30  22:34:46  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.1  1992/09/22  18:11:17  regnier
// Initial revision
//
 * Revision 2.1.2.2  92/05/27  00:42:51  jeffreyh
 * 	Added some experimental, special case pipelined fifo routines.
 * 	_dcm_piped_fifo_in() appears to work.  _dcm_piped_fifo_out()
 * 	bypasses the cache and may push stale data out the wire.
 * 	_dcm_quad_fifo_out() was not tested in any substantial way.
 * 	None of these routines are currently called.
 * 	[andyp@ssd.intel.com]
 * 
 * Revision 2.1.2.1  92/04/08  15:43:51  jeffreyh
 * 	Experimental fifo load/drain.
 * 	[92/04/08            andyp]
 * 
 *
 */
	.file	"dcm.s"
	.text
	.align	16

//	andyp@ssd.intel.com
//
//	_dcm_fifo_out(src, fifo, nwords)
//	unsigned long	*src, *fifo;
//	int nwords;
//
//	XXX Could be made faster
//
__dcm_fifo_out::
	adds	-1,r0,r19	// loop increment
	addu	-1,r18,r18	// loop count - 1
	bla	r19,r18,.push	// once to initialize LCC
	 addu	-4,r16,r16	// start src 1 word lower
.push:
	fld.l	4(r16)++,f16
	bla	r19,r18,.push
	 fst.l	f16,0(r17)
	bri	r1
	 nop

//	andyp@ssd.intel.com
//
//	_dcm_fifo_in(dst, fifo, nwords)
//	unsigned long	*dst, *fifo;
//	int nwords;
//
//	XXX Could be made faster
//
__dcm_fifo_in::
	adds	-1,r0,r19	// loop increment
	addu	-1,r18,r18	// loop count - 1
	bla	r19,r18,.pull	// once to initialize LCC
	 addu	-4,r16,r16	// start dst 1 word lower
.pull:
	fld.l	0(r17),f16
	bla	r19,r18,.pull
	 fst.l	f16,4(r16)++
	bri	r1
	 nop

//	andyp@ssd.intel.com (with inspiration from regnier@ssd.intel.com)
//
//	_dcm_piped_fifo_in(fifo, buf, nblocks)
//	unsigned long	*fifo, *buf, nblocks;
//
//	(1 block == 64 bytes)
//
//	Restrictions:
//		XXX Can only be used for multiples of 64-bytes > 128 bytes
//		XXX "buf" must be 16-byte aligned
//
__dcm_piped_fifo_in::
	pfld.l	0(r16),f0	// 1st stage
	adds	-1,r0,r19	// loop inc
	pfld.l	0(r16),f0	// 2nd stage
	addu	-2,r18,r20	// loop count - 2
	pfld.l	0(r16),f0	// 3rd stage
	bla	r19,r20,.pipe_in	// initialize lcc
	 addu	-16,r17,r17	// one quad lower for dst

        // load pipe is now primed

.pipe_in:
	// pull 16 longs from the fifo
	pfld.l	0(r16),f16
	pfld.l	0(r16),f17
	pfld.l	0(r16),f18
	pfld.l	0(r16),f19
	pfld.l	0(r16),f20
	pfld.l	0(r16),f21
	pfld.l	0(r16),f22
	pfld.l	0(r16),f23
	pfld.l	0(r16),f24
	pfld.l	0(r16),f25
	pfld.l	0(r16),f26
	pfld.l	0(r16),f27
	pfld.l	0(r16),f28
	pfld.l	0(r16),f29
	pfld.l	0(r16),f30
	pfld.l	0(r16),f31

	// write 4 quads to memory
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bla	r19,r20,.pipe_in
	 fst.q	f28,16(r17)++

	//
	// last 64-byte block is special
	//
	// 13 transfers (+ 3 more still in the pipe)
	//
	pfld.l	0(r16),f16
	pfld.l	0(r16),f17
	pfld.l	0(r16),f18
	pfld.l	0(r16),f19
	pfld.l	0(r16),f20
	pfld.l	0(r16),f21
	pfld.l	0(r16),f22
	pfld.l	0(r16),f23
	pfld.l	0(r16),f24
	pfld.l	0(r16),f25
	pfld.l	0(r16),f26
	pfld.l	0(r16),f27
	pfld.l	0(r16),f28
	pfld.l	0(sp),f29	// drain remnants of the load pipe
	pfld.l	0(sp),f30
	pfld.l	0(sp),f31

	// store the last 4 quads
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bri	r1
	 fst.q	f28,16(r17)++

//	andyp@ssd.intel.com (with inspiration from regnier@ssd.intel.com)
//
//	_dcm_piped_fifo_out(buf, fifo, nblocks)
//	unsigned long	*buf, *fifo, nblocks;
//
//	(1 block == 64 bytes)
//
//	Restrictions:
//		XXX Can only be used for multiples of 64-bytes > 128 bytes
//		XXX "buf" must be 16-byte aligned
//
__dcm_piped_fifo_out::
	pfld.d	0(r16),f0	// 1st stage
	adds	-1,r0,r19	// loop inc
	pfld.d	8(r16)++,f0	// 2nd stage
	addu	-2,r18,r20	// loop count - 2
	bla	r19,r20,.pipe_out	// initialize lcc
	 pfld.d	8(r16)++,f0	// 3rd stage

        // load pipe is now primed

.pipe_out:
	// read 8 doubles from memory
	pfld.d	8(r16)++,f16
	pfld.d	8(r16)++,f18
	pfld.d	8(r16)++,f20
	pfld.d	8(r16)++,f22
	pfld.d	8(r16)++,f24
	pfld.d	8(r16)++,f26
	pfld.d	8(r16)++,f28
	pfld.d	8(r16)++,f30

	// push 16 longs out the fifo
	fst.l	f16,0(r17)
	fst.l	f17,0(r17)
	fst.l	f18,0(r17)
	fst.l	f19,0(r17)
	fst.l	f20,0(r17)
	fst.l	f21,0(r17)
	fst.l	f22,0(r17)
	fst.l	f23,0(r17)
	fst.l	f24,0(r17)
	fst.l	f25,0(r17)
	fst.l	f26,0(r17)
	fst.l	f27,0(r17)
	fst.l	f28,0(r17)
	fst.l	f29,0(r17)
	fst.l	f30,0(r17)
	bla	r19,r20,.pipe_out
	 fst.l	f31,0(r17)

	//
	// last 64-byte block is special
	//
	// 5 transfers (+ 3 more still in the pipe)
	//
	pfld.d	0(r16)++,f16
	pfld.d	0(r16)++,f18
	pfld.d	0(r16)++,f20
	pfld.d	0(r16)++,f22
	pfld.d	0(r16)++,f24
	pfld.d	0(r16),f26
	pfld.d	0(r16),f28
	pfld.d	0(r16),f30

	// dump the last 16 longs
	fst.l	f16,0(r17)
	fst.l	f17,0(r17)
	fst.l	f18,0(r17)
	fst.l	f19,0(r17)
	fst.l	f20,0(r17)
	fst.l	f21,0(r17)
	fst.l	f22,0(r17)
	fst.l	f23,0(r17)
	fst.l	f24,0(r17)
	fst.l	f25,0(r17)
	fst.l	f26,0(r17)
	fst.l	f27,0(r17)
	fst.l	f28,0(r17)
	fst.l	f29,0(r17)
	fst.l	f30,0(r17)
	bri	r1
	 fst.l	f31,0(r17)

//	andyp@ssd.intel.com
//
//	_dcm_quad_fifo_out(buf, fifo, nblocks)
//	unsigned long	*buf, *fifo, nblocks;
//
//	(1 block == 64 bytes)
//
//	Restrictions:
//		XXX Can only be used for multiples of 64-bytes
//		XXX "buf" must be 8-byte aligned
//
__dcm_quad_fifo_out::
	adds	-1,r0,r19	// loop inc
	addu	-1,r18,r20	// loop count - 1
	bla	r19,r20,.quad_out	// initialize lcc
	 addu	-8,r16,r16	// start one double lower

.quad_out:
	// read 8 doubles from memory
	fld.d	8(r16)++,f16
	fld.d	8(r16)++,f18
	fld.d	8(r16)++,f20
	fld.d	8(r16)++,f22
	fld.d	8(r16)++,f24
	fld.d	8(r16)++,f26
	fld.d	8(r16)++,f28
	fld.d	8(r16)++,f30

	// push 16 longs out the fifo
	fst.l	f16,0(r17)
	fst.l	f17,0(r17)
	fst.l	f18,0(r17)
	fst.l	f19,0(r17)
	fst.l	f20,0(r17)
	fst.l	f21,0(r17)
	fst.l	f22,0(r17)
	fst.l	f23,0(r17)
	fst.l	f24,0(r17)
	fst.l	f25,0(r17)
	fst.l	f26,0(r17)
	fst.l	f27,0(r17)
	fst.l	f28,0(r17)
	fst.l	f29,0(r17)
	fst.l	f30,0(r17)
	bla	r19,r20,.quad_out
	 fst.l	f31,0(r17)

	bri	r1
	 nop

