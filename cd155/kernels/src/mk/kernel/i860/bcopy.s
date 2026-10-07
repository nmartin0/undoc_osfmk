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
 * $Id: bcopy.s,v 2.14 1994/12/16 18:20:17 andyp Exp $
 */

	.file	"bcopy.s"
	.text
	.align	32

#if	PARAGON860
//
//	andyp@ssd.intel.com
//
//	void bcopy(src, dst, cnt)
//	char	*src, *dst;
//	int	cnt;
//
//	A bcopy() that has better performance characteristics
//	for transfers that are not favorably aligned.
//
_bcopy::
	//
	//	if < 48 bytes to copy, handle the small stuff
	//	in a non-pipelined loop.
	//
	adds	-48, r18, r0
	bc	.small

	//
	//	>= 48 bytes to copy.  Check for 16-byte alignment.
	//
	or	r16, r17, r31
	and	0x000f, r31, r0
	bnc	.mis8

	//
	//	pipelined copy of 16-byte aligned data.
	//
.copy16:
	addu	-16, r16, r16		// autopreincrement
	addu	-16, r17, r17		// autopreincrement
	addu	-48, r18, r18		// 3 in load pipe
	pfld.q	16(r16)++, f8
	addu	-1,  r0, r28		// increment
	pfld.q	16(r16)++, f8
	shr	 6, r18, r29		// number of loops == (cnt / 64)
	pfld.q	16(r16)++, f8
	mov	r29, r30		// save the loop count...
	bla	r28, r29, .primed16
	 nop
.primed16:
	bla	r28, r29, .pump16
	 nop
.drain16:
	pfld.d	0(r16), f16		// drain the 3 outstanding
	pfld.d	0(r16), f20
	pfld.d	0(r16), f24
	fst.q	f16, 16(r17)++
	fst.q	f20, 16(r17)++
	fst.q	f24, 16(r17)++
	addu	16, r16, r16		// undo the autopreincrement
	addu	16, r17, r17		// undo the autopreincrement
.tail16:
	bte	0, r18, .done
	adds	-16, r18, r0
	bc	.tail8
	fld.q	0(r16), f16
	addu	16, r16, r16
	addu	16, r17, r17
	adds	-16, r18, r18
	br	.tail16
	 fst.q	f16, -16(r17)
.pump16:
	pfld.q	16(r16)++, f16
	pfld.q	16(r16)++, f20
	pfld.q	16(r16)++, f24
	pfld.q	16(r16)++, f28
	fst.q	f16, 16(r17)++
	fst.q	f20, 16(r17)++
	fst.q	f24, 16(r17)++
	bla	r28, r29, .pump16
	 fst.q	f28, 16(r17)++
	shl	6, r30, r30		// subtract the number of bytes...
	br	.drain16
	 subs	r18, r30, r18		// ...moved in the loop.

	//
	//	check for 8-byte alignment
	//
.mis8:
	or	r16, r17, r31
	and	0x0007, r31, r0
	bnc	.mis4

	//
	//	there are four cases for (src | dst) & 7 == 0:
	//
	//	src=0 dst=0	-- not applicable (handled above)
	//	src=0 dst=8	-- 8-byte but can't become 16-byte aligned
	//	src=8 dst=0	-- 8-byte but can't become 16-byte aligned
	//	src=8 dst=8	-- can become 16-byte aligned
	//
	xor	r16, r17, r31
	and	0x0008, r31, r0
	bc	.one8			// might become 16-byte aligned

	//
	//	pipelined copy of 8-byte aligned data.
	//
.copy8:
	addu	-8, r16, r16		// autopreincrement
	addu	-8, r17, r17		// autopreincrement
	addu	-24, r18, r18		// 3 in load pipe
	pfld.d	8(r16)++, f8
	addu	-1,  r0, r28		// increment
	pfld.d	8(r16)++, f8
	shr	 6, r18, r29		// number of loops == (cnt / 64)
	pfld.d	8(r16)++, f8
	mov	r29, r30		// save the loop count...
	bla	r28, r29, .primed8
	 nop
.primed8:
	bla	r28, r29, .pump8
	 nop
.drain8:
	pfld.d	0(r16), f16		// drain the 3 outstanding
	pfld.d	0(r16), f18
	pfld.d	0(r16), f20
	fst.d	f16, 8(r17)++
	fst.d	f18, 8(r17)++
	fst.d	f20, 8(r17)++
	addu	8, r16, r16		// undo the autopreincrement
	addu	8, r17, r17		// undo the autopreincrement
.tail8:
	bte	0, r18, .done
	adds	-8, r18, r0
	bc	.tail4
	fld.d	 0(r16), f16
	addu	 8, r16, r16
	addu	 8, r17, r17
	adds	-8, r18, r18
	br	.tail8
	 fst.d	f16, -8(r17)
.pump8:
	pfld.d	8(r16)++, f16
	pfld.d	8(r16)++, f18
	pfld.d	8(r16)++, f20
	pfld.d	8(r16)++, f22
	pfld.d	8(r16)++, f24
	pfld.d	8(r16)++, f26
	pfld.d	8(r16)++, f28
	pfld.d	8(r16)++, f30
	fst.d	f16, 8(r17)++
	fst.d	f18, 8(r17)++
	fst.d	f20, 8(r17)++
	fst.d	f22, 8(r17)++
	fst.d	f24, 8(r17)++
	fst.d	f26, 8(r17)++
	fst.d	f28, 8(r17)++
	bla	r28, r29, .pump8
	 fst.d	f30, 8(r17)++
	shl	6, r30, r30		// subtract the number of bytes...
	br	.drain8
	 subs	r18, r30, r18		// ...moved in the loop.

.one8:
	//
	//	move 1 8-byte word (makes src and dst 16-byte aligned).
	//
	fld.d	0(r16), f16
	addu	8, r16, r16
	addu	8, r17, r17
	addu	-8, r18, r18
	br	_bcopy
	 fst.d	f16, -8(r17)

.mis4:
	or	r16, r17, r31
	and	0x0003, r31, r0
	bnc	.mis2

	//
	//	there are four cases for (src | dst) & 3 == 0:
	//
	//	src=0 dst=0	-- not applicable (handled above)
	//	src=0 dst=4	-- 4-byte but can't become 8-byte aligned
	//	src=4 dst=0	-- 4-byte but can't become 8-byte aligned
	//	src=4 dst=4	-- can become 8-, 16-byte aligned
	//
	xor	r16, r17, r31
	and	0x0004, r31, r0
	bc	.one4			// might become 16-byte aligned

	//
	//	pipelined copy of 4-byte aligned data
	//
.copy4:
	addu	-4, r16, r16		// autopreincrement
	addu	-4, r17, r17		// autopreincrement
	addu	-12, r18, r18		// 3 in load pipe
	pfld.l	4(r16)++, f8
	addu	-1,  r0, r28		// increment
	pfld.l	4(r16)++, f8
	shr	 6, r18, r29		// number of loops == (cnt / 64)
	pfld.l	4(r16)++, f8
	mov	r29, r30		// save the loop count...
	bla	r28, r29, .primed4
	 nop
.primed4:
	bla	r28, r29, .pump4
	 nop
.drain4:
	pfld.l	0(r16), f16		// drain the 3 outstanding
	pfld.l	0(r16), f17
	pfld.l	0(r16), f18
	fst.l	f16, 4(r17)++
	fst.l	f17, 4(r17)++
	fst.l	f18, 4(r17)++
	addu	4, r16, r16		// undo the autopreincrement
	addu	4, r17, r17		// undo the autopreincrement
.tail4:
	bte	0, r18, .done
	adds	-4, r18, r0
	bc	.tail2
	fld.l	 0(r16), f16
	addu	 4, r16, r16
	addu	 4, r17, r17
	adds	-4, r18, r18
	br	.tail4
	 fst.l	f16, -4(r17)
.pump4:
	pfld.l	4(r16)++, f16
	pfld.l	4(r16)++, f17
	pfld.l	4(r16)++, f18
	pfld.l	4(r16)++, f19
	pfld.l	4(r16)++, f20
	pfld.l	4(r16)++, f21
	pfld.l	4(r16)++, f22
	pfld.l	4(r16)++, f23
	pfld.l	4(r16)++, f24
	pfld.l	4(r16)++, f25
	pfld.l	4(r16)++, f26
	pfld.l	4(r16)++, f27
	pfld.l	4(r16)++, f28
	pfld.l	4(r16)++, f29
	pfld.l	4(r16)++, f30
	pfld.l	4(r16)++, f31
	fst.l	f16, 4(r17)++
	fst.l	f17, 4(r17)++
	fst.l	f18, 4(r17)++
	fst.l	f19, 4(r17)++
	fst.l	f20, 4(r17)++
	fst.l	f21, 4(r17)++
	fst.l	f22, 4(r17)++
	fst.l	f23, 4(r17)++
	fst.l	f24, 4(r17)++
	fst.l	f25, 4(r17)++
	fst.l	f26, 4(r17)++
	fst.l	f27, 4(r17)++
	fst.l	f28, 4(r17)++
	fst.l	f29, 4(r17)++
	fst.l	f30, 4(r17)++
	bla	r28, r29, .pump4
	 fst.l	f31, 4(r17)++
	shl	6, r30, r30		// subtract the number of bytes...
	br	.drain4
	 subs	r18, r30, r18		// ...moved in the loop.

.one4:
	//
	//	move just 1 4-byte word and try again from the top
	//	(making 8-, and perhaps 16-byte alignment)
	//
	fld.l	0(r16), f16
	addu	4, r16, r16
	addu	4, r17, r17
	addu	-4, r18, r18
	br	_bcopy
	 fst.l	f16, -4(r17)

.mis2:
	or	r16, r17, r31
	and	0x0001, r31, r0
	bnc	.mis1

	//
	//	there are four cases for (src | dst) & 1 == 0:
	//
	//	src=0 dst=0	-- not applicable (handled above)
	//	src=0 dst=2	-- 2-byte but can't become 4-byte aligned
	//	src=2 dst=0	-- 2-byte but can't become 4-byte aligned
	//	src=2 dst=2	-- can become 4-, 8-, 16-byte aligned
	//
	xor	r16, r17, r31
	and	0x0002, r31, r0
	bc	.one2			// might become 16-byte aligned

	//
	//	without resorting to additional shifting,
	//	this is the best we can do, moving 2-bytes
	//	at a time.  we do know, however, that there
	//	are >= 48 bytes to move.  (it's convenient
	//	to move only 24 bytes per iteration).
	//
	ld.s	 0(r16), r20
.copy2:
	ld.s	 2(r16), r21
	ld.s	 4(r16), r22
	ld.s	 6(r16), r23
	ld.s	 8(r16), r24
	ld.s	10(r16), r25
	ld.s	12(r16), r26
	ld.s	14(r16), r27
	ld.s	16(r16), r28
	ld.s	18(r16), r29
	ld.s	20(r16), r30
	ld.s	22(r16), r31
	ld.s	 0(r17), r0		// st.s dcache miss is sloooow
	addu	24, r16, r16
	st.s	r20,  0(r17)
	st.s	r21,  2(r17)
	st.s	r22,  4(r17)
	st.s	r23,  6(r17)
	st.s	r24,  8(r17)
	st.s	r25, 10(r17)
	st.s	r26, 12(r17)
	st.s	r27, 14(r17)
	st.s	r28, 16(r17)
	st.s	r29, 18(r17)
	st.s	r30, 20(r17)
	st.s	r31, 22(r17)
	addu	24, r17, r17
	adds	-24, r18, r18
	bc.t	.copy2
	 ld.s	 0(r16), r20
.tail2:
	bte	0, r18, .done
	adds	-2, r18, r0
	bc	.tail1
	ld.s	 0(r16), r20
	addu	 2, r16, r16
	ld.s	 0(r17), r0		// st.s dcache miss is slow
	addu	 2, r17, r17
	adds	-2, r18, r18
	br	.tail2
	 st.s	r20, -2(r17)
.one2:
	ld.s	0(r16), r20
	addu	2, r16, r16
	addu	2, r17, r17
	addu	-2, r18, r18
	br	_bcopy
	 st.s	r20, -2(r17)

.mis1:
	xor	r16, r17, r31
	and	0x0001, r31, r0
	bc	.one1			// might become 16-byte aligned

	ld.b	 0(r16), r20
.copy1:
	ld.b	 1(r16), r21
	ld.b	 2(r16), r22
	ld.b	 3(r16), r23
	ld.b	 4(r16), r24
	ld.b	 5(r16), r25
	ld.b	 6(r16), r26
	ld.b	 7(r16), r27
	ld.b	 8(r16), r28
	ld.b	 9(r16), r29
	ld.b	10(r16), r30
	ld.b	11(r16), r31
	ld.b	 0(r17), r0		// st.b dcache miss is sloooow
	addu	12, r16, r16
	st.b	r20,  0(r17)
	st.b	r21,  1(r17)
	st.b	r22,  2(r17)
	st.b	r23,  3(r17)
	st.b	r24,  4(r17)
	st.b	r25,  5(r17)
	st.b	r26,  6(r17)
	st.b	r27,  7(r17)
	st.b	r28,  8(r17)
	st.b	r29,  9(r17)
	st.b	r30, 10(r17)
	st.b	r31, 11(r17)
	addu	12, r17, r17
	adds	-12, r18, r18
	bc.t	.copy1
	 ld.b	 0(r16), r20
.tail1:
	adds	-1, r18, r0
	bc	.done
	ld.b	 0(r16), r20
	addu	 1, r16, r16
	ld.b	 0(r17), r0		// st.b dcache miss is slow
	addu	 1, r17, r17
	adds	-1, r18, r18
	br	.tail1
	 st.b	r20, -1(r17)
.one1:
	ld.b	0(r16), r20
	addu	1, r16, r16
	addu	1, r17, r17
	addu	-1, r18, r18
	br	_bcopy			// try again -- with better alignment
	 st.b	r20, -1(r17)

.small:
	bte	0, r18, .done
	or	r16, r17, r31
	and	0x000f, r31, r0	
	bc.t	.tail16
	 nop
	and	0x0007, r31, r0
	bc.t	.tail8
	 nop
	and	0x0003, r31, r0
	bc.t	.tail4
	 nop
	and	0x0001, r31, r0
	bc.t	.tail2
	 nop
	br	.tail1
	 nop
.done:
	bri	r1
	 nop

#else	PARAGON860

	/*
	 *	XXX andyp@ssd.intel.com
	 *
	 *	XXX Some older steppings of the XR seem to have troubles
	 *	XXX with the above version of bcopy().  This needs to
	 *	XXX get resolved.
	 */

#include <machine/psl.h>

#define	BCOPY_REG r30

//
//	bcopy(src, dst, nbytes)
//

_bcopy::
	bte	r18,r0,.bcdone

	adds	-1,r0,BCOPY_REG		// r30 = -1 for bla
	or	r16,r17,r19
	or	r18,r19,r19
	and	3,r19,r0
	bc	.bcdolongs		// long aligned addr and 4*x bcnt?

.bcblp_start:
	adds	-1,r18,r18
	bla	BCOPY_REG,r18,.bcblp
	 nop
.bcblp:
	ld.b	0(r16),r29
	addu	1,r16,r16
	and	0x00FF,r29,r29
	st.b	r29,0(r17)
	bla	BCOPY_REG,r18,.bcblp
	 addu	1,r17,r17
	bri	r1
	 nop

.bcdolongs:
	shr	2,r18,r18
	adds	-1,r18,r18
	bla	BCOPY_REG,r18,.bcllp
	 nop
.bcllp:
	ld.l	0(r16),r29
	addu	4,r16,r16
	st.l	r29,0(r17)
	bla	BCOPY_REG,r18,.bcllp
	 addu	4,r17,r17
.bcdone:
	bri	r1
	 nop

#endif	PARAGON860

//	andyp@ssd.intel.com
//
//	void quad_copy(src, dst, nquads)
//	vm_offset_t	src, dst;
//	int		nquads;
//
//	Copy 16-byte-sized chunks of memory using quad-word stores.
//	DANGER: src and dst must be quad-word aligned.
//
//	XXX Were this routine to use pfld.d (or pfld.q on the i860XP),
//	    it wouldn't thrash the on-chip data cache so badly.
//
//	XXX A further optimization would be to load several quads
//	    into the registers (and stay in the same DRAM page)
//	    before issuing the store; ie, take deep breaths of memory.
//
	.text
	.align	4
_quad_copy::
	adds	-1,r0,r19	// loop increment
	addu	-1,r18,r18	// loop count - 1
	addu	-16,r16,r16	// start src 1 quad lower
	bla	r19,r18,.qcopy	// once to initialize LCC
	 addu	-16,r17,r17	// start dst 1 quad lower
.qcopy:
	fld.q	16(r16)++,f16	// load 16 bytes into f16..f19
	bla	r19,r18,.qcopy
	 fst.q	f16,16(r17)++	// store 16 bytes from f16..f19

	bri	r1
	 nop

#if	i860XP

//	andyp@ssd.intel.com
//
//	i860XP verison (pfld.q)
//
//	piped_page_copy(src, dst)
//	vm_offset_t src, dst;
//
//	XXX FOR i860XP ONLY
//
//	Called to copy a physical page when it would *not* be advantageous
//	to fill the data cache with data that will not be used.
//

	.text
	.align	4
_piped_page_copy::
	pfld.q	0(r16),f16	// 1st stage (use f16 in case a quad pops out)
	adds	-1,r0,r19	// loop inc
	pfld.q	16(r16)++,f20	// 2nd stage (use f20 in case a quad pops out)
	addu	62,r0,r20	// loop count - 1
	pfld.q	16(r16)++,f24	// 3rd stage (use f24 in case a quad pops out)
	bla	r19,r20,.inhale	// initialize lcc
	 addu	-16,r17,r17	// one quad lower for dst
.inhale:
	pfld.q	16(r16)++,f16
	pfld.q	16(r16)++,f20
	pfld.q	16(r16)++,f24
	pfld.q	16(r16)++,f28
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bla	r19,r20,.inhale
	 fst.q	f28,16(r17)++

	pfld.q	16(r16)++,f16	// one load left + 3 in the pipe
	pfld.d	0(r16),f20	// leave doubles in the load pipe...
	pfld.d	0(r16),f24	// leave doubles in the load pipe...
	pfld.d	0(r16),f28	// leave doubles in the load pipe...
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bri	r1
	 fst.q	f28,16(r17)++

#else	i860XP

//	andyp@ssd.intel.com
//
//	piped_page_copy(src, dst)
//	vm_offset_t src, dst;
//
//	XXX FOR i860XR or i860XP
//
	.text
	.align	4
_piped_page_copy::
	pfld.d	0(r16),f0	// 1st stage
	adds	-1,r0,r19	// loop inc
	pfld.d	8(r16)++,f0	// 2nd stage
	addu	62,r0,r20	// loop count - 1
	pfld.d	8(r16)++,f0	// 3rd stage
	bla	r19,r20,.inhale	// initialize lcc
	 addu	-16,r17,r17	// one quad lower for dst

        // load pipe is now primed

	//
	// If the external memory were perfect, and we weren't limited
	// to one pfld every other clock, and there were no tlb or icache
	// misses, the following loop would transfer 64 bytes about
	// every 13 clocks, or ~197MB/sec at 40MHz.  In practice, it
	// should hit 45MB/sec on even sleezy memory systems.  Even
	// so, we're still external-memory bandwidth limited.
	//
	//
.inhale:
	// inhale 8 doubles
	pfld.d	8(r16)++,f16
	pfld.d	8(r16)++,f18	// internal stalls from here on
	pfld.d	8(r16)++,f20	// but we're still in the same DRAM page...
	pfld.d	8(r16)++,f22	// and only causing CAS (rather than RAS+CAS).
	pfld.d	8(r16)++,f24
	pfld.d	8(r16)++,f26
	pfld.d	8(r16)++,f28
	pfld.d	8(r16)++,f30

	// exhale 4 quads
	fst.q	f16,16(r17)++	// this takes RAS+CAS+precharge ns (usually)
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bla	r19,r20,.inhale
	 fst.q	f28,16(r17)++

	//
	// last block is special
	//

	// 5 transfers (+ 3 more still in the pipe)
	pfld.d	8(r16)++,f16
	pfld.d	8(r16)++,f18
	pfld.d	8(r16)++,f20
	pfld.d	8(r16)++,f22
	pfld.d	8(r16)++,f24
	pfld.d	0(r16),f26	// don't overshoot the src page!
	pfld.d	0(r16),f28
	pfld.d	0(r16),f30

	// store the last 4 quads
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bri	r1
	 fst.q	f28,16(r17)++

#endif	i860XP


//	andyp@ssd.intel.com
//
//	scopy(src, dst, cnt)
//	short	*src, *dst;
//	int	cnt;
//
//	src and dst must be 16-bit aligned, cnt is
//	the number of shorts to copy.
//
	.text
	.align	4
_scopy::
	mov	r18,r23
	adds	-4,r0,r24
	bla	r24,r23,.scopyprime
	 nop
.scopyprime:
	bla	r24,r23,.scopy4
	 nop
	br	.scopyrest
	 nop
.scopy4:
	ld.s	0(r16),r19
	ld.s	2(r16),r20
	ld.s	4(r16),r21
	ld.s	6(r16),r22
	adds	8,r16,r16
	st.s	r19,0(r17)
	st.s	r20,2(r17)
	st.s	r21,4(r17)
	st.s	r22,6(r17)
	adds	8,r17,r17
	bla	r24,r23,.scopy4
	 adds	-4,r18,r18
.scopyrest:
	bte	1,r18,.scopy1
	bte	2,r18,.scopy2
	bte	3,r18,.scopy3
	bri	r1
	 nop
.scopy3:			// copy 3
	ld.s	0(r16),r19
	ld.s	2(r16),r20
	ld.s	4(r16),r21
	st.s	r19,0(r17)
	st.s	r20,2(r17)
	bri	r1
	 st.s	r21,4(r17)
.scopy2:			// copy 2
	ld.s	0(r16),r19
	ld.s	2(r16),r20
	st.s	r19,0(r17)
	bri	r1
	 st.s	r20,2(r17)
.scopy1:			// copy 1
	ld.s	0(r16),r19
	bri	r1
	 st.s	r19,0(r17)
