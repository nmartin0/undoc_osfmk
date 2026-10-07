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
 * $Log: miocopy.s,v $
 * Revision 0.4  1994/11/18  20:45:16  mtm
 * Copyright additions/changes
 *
 * Revision 0.3  1993/06/30  22:39:48  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.2  1992/08/25  09:44:28  andyp
 * Corrected the comment leader string (and the log message).
 *
 * Revision 0.1  92/08/25  09:39:19  andyp
 * Newer and improved specialty copy routines for copying data
 * to and from MIO static ram.
 *
 */
/*
 *	A small collection of copy routines for moving bytes between
 *	the MIO's static ram and main memory.
 *
 *	XXX Experiment:
 *	Setting PRIME_FOR_WRITEBACK will include instructions in the
 *	copyin flavor of calls to load the destination into the data
 *	cache so that the store will hit in the cache (enabling a
 *	burst writeback when evicted).
 */
#define	PRIME_FOR_WRITEBACK	1


	.file "miocopy.s"
	.text
	.align	4

//	andyp@ssd.intel.com
//
//	mio_byte_copy(src, dst, cnt)
//	vm_offset_t	src, dst;
//	int	cnt;
//
//	perform a byte-wide copy from src to dst of cnt bytes.
//
	.text
	.align	4
_mio_byte_copy::
	adds	-1,r0,r19		// loop increment
	addu	-1,r18,r18		// loop count - 1
	bla	r19,r18,.bcopy		// once to init LCC
	 nop
.bcopy:
	ld.b	0(r16),r20
	addu	1,r16,r16
	st.b	r20,0(r17)
	bla	r19,r18,.bcopy
	 addu	1,r17,r17

	bri	r1
	 nop

//	andyp@ssd.intel.com
//
//	mio_word_copy(src, dst, cnt)
//	vm_offset_t	src, dst;
//	int	cnt;
//
//	perform a 16-bit copy from src to dst of cnt shorts.
//
	.text
	.align	4
_mio_word_copy::
	adds	-1,r0,r19		// loop increment
	addu	-1,r18,r18		// loop count - 1
	bla	r19,r18,.wcopy		// once to init LCC
	 nop
.wcopy:
	ld.s	0(r16),r20
	addu	2,r16,r16
	st.s	r20,0(r17)
	bla	r19,r18,.wcopy
	 addu	2,r17,r17

	bri	r1
	 nop

//	andyp@ssd.intel.com
//
//	mio_quad_copyout(src, dst, cnt)
//	vm_offset_t	src, dst;
//	int	cnt;
//
//	copy *into* mio static ram *from* kernel ram.
//
//	Restrictions:
//
//		1. src must be 16-byte aligned.
//		2. dst must be 2-byte aligned.
//		3. cnt is the number of quads to copy (minimum of 1).
//
	.text
	.align	4
_mio_quad_copyout::
	adds	-1,r0,r19		// loop increment
	addu	-1,r18,r18		// loop count - 1
	bla	r19,r18,.qcopyout	// once to init LCC
	 addu	-16,r16,r16		// start src 1 quad lower
.qcopyout:
	// load a quad from dram
	fld.q	16(r16)++,f16

	// shovel the bytes to the iregs
	// pipeline stalls here (f16 and f17 still busy)
	fxfr	f16,r20
	fxfr	f17,r21
	fxfr	f18,r22
	fxfr	f19,r23

	// write the even shorts, shift in the shadow
	st.s	r20,0(r17)
	shr	16,r20,r20
	st.s	r21,4(r17)
	shr	16,r21,r21
	st.s	r22,8(r17)
	shr	16,r22,r22
	st.s	r23,12(r17)
	shr	16,r23,r23

	// write the odd shorts
	st.s	r20,2(r17)
	st.s	r21,6(r17)
	st.s	r22,10(r17)
	st.s	r23,14(r17)

	bla	r19,r18,.qcopyout
	 addu	16,r17,r17
	bri	r1
	 nop

//	andyp@ssd.intel.com
//
//	mio_quad_copyin(src, dst, cnt)
//	vm_offset_t	src, dst;
//	int	cnt;
//
//	copy *from* mio static ram *into* kernel ram.
//
//	Restrictions:
//
//		1. src must be 2-byte aligned.
//		2. dst must be 16-byte aligned.
//		3. cnt is the number of quads to copy (minimum of 1).
//
	.text
	.align	4
_mio_quad_copyin::
	adds	-1,r0,r19		// loop increment
	addu	-1,r18,r18		// loop count - 1
	bla	r19,r18,.qcopyin	// once to init LCC
	 addu	-16,r17,r17		// start dst 1 quad lower
.qcopyin:
#if	PRIME_FOR_WRITEBACK
	ld.l	16(r17),r0		// read dst into cache so fst will hit
#endif	PRIME_FOR_WRITEBACK
	//
	//	load 1 quad's worth of shorts
	//
	ld.s	 0(r16),r20
	// stall?
	ld.s	 2(r16),r21
	and	0xffff,r20,r20
	ld.s	 4(r16),r22
	shl	16,r21,r21
	ld.s	 6(r16),r23
	or	r20,r21,r28	// r28 = (r21 << 16) | (r20 & 0xffff)
	and	0xffff,r22,r22
	ixfr	r28,f16		// f16 = r28
	ld.s	 8(r16),r24
	shl	16,r23,r23
	ld.s	10(r16),r25
	or	r22,r23,r29	// r29 = (r23 << 16) | (r22 & 0xffff)
	and	0xffff,r24,r24
	ixfr	r29,f17		// f17 = r29
	ld.s	12(r16),r26
	shl	16,r25,r25
	ld.s	14(r16),r27
	or	r24,r25,r30	// r30 = (r25 << 16) | (r24 & 0xffff)
	and	0xffff,r26,r26
	ixfr	r30,f18		// f18 = r30
	shl	16,r27,r27
	or	r26,r27,r31	// r31 = (r27 << 16) | r26
	// stall
	ixfr	r31,f19		// f19 = r31

	addu	16,r16,r16
	bla	r19,r18,.qcopyin
	 fst.q	f16,16(r17)++

	bri	r1
	 nop


//	andyp@ssd.intel.com
//
//	mio_long_copyout(src, dst, cnt)
//	vm_offset_t	src, dst;
//	int	cnt;
//
//	copy *into* mio static ram *from* kernel ram.
//
//	Restrictions:
//
//		1. src must be 4-byte aligned.
//		2. dst must be 2-byte aligned.
//		3. cnt is the number of longs to copy (minimum of 1).
//
	.text
	.align	4
_mio_long_copyout::
	adds	-1,r0,r19		// loop increment
	addu	-1,r18,r18		// loop count - 1
	bla	r19,r18,.lcopyout	// once to init LCC
	 nop
.lcopyout:
	ld.l	0(r16),r20
	addu	4,r16,r16
	st.s	r20,0(r17)
	shr	16,r20,r20
	addu	4,r17,r17
	bla	r19,r18,.lcopyout
	 st.s	r20,-2(r17)
	bri	r1
	 nop

//	andyp@ssd.intel.com
//
//	mio_long_copyin(src, dst, cnt)
//	vm_offset_t	src, dst;
//	int	cnt;
//
//	copy *from* mio static ram *into* kernel ram.
//
//	Restrictions:
//
//		1. src must be 2-byte aligned.
//		2. dst must be 4-byte aligned.
//		3. cnt is the number of longs to copy (minimum of 1).
//
	.text
	.align	4
_mio_long_copyin::
	adds	-1,r0,r19		// loop increment
	addu	-1,r18,r18		// loop count - 1
	bla	r19,r18,.lcopyin	// once to init LCC
	 nop
.lcopyin:
#if	PRIME_FOR_WRITEBACK
	ld.l	0(r17),r0		// read dst into cache so st.l will hit
#endif	PRIME_FOR_WRITEBACK
	ld.s	0(r16),r20
	addu	4,r17,r17
	ld.s	2(r16),r21
	and	0xffff,r20,r20
	shl	16,r21,r21
	addu	4,r16,r16
	or	r20,r21,r20
	bla	r19,r18,.lcopyin
	 st.l	r20,-4(r17)
	bri	r1
	 nop
//
//	as above, but with less confusion:
//
//	ld.s	0(r16),r20		// lo = *r16
//	ld.s	2(r16),r21		// hi = *(r16 + 2)
//	addu	4,r16,r16		// r16 += 4
//	and	0xffff,r20,r20		// lo &= 0xffff
//	shl	16,r21,r21		// hi <<= 16
//	or	r20,r21,r20		//
//	st.l	r20,0(r17)		// *r17 = lo | hi
//	bla	r19,r18,.lcopyin
//	 addu	4,r17,r17		// r17 += 4
//	bri	r1
//	 nop
