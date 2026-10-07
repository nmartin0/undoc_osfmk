//
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
// 
 
//
// Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
// Santa Clara, California.
// 
//	All Rights Reserved
// 
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and that
// both the copyright notice and this permission notice appear in
// supporting documentation, and that the name of Intel not be used in
// advertising or publicity pertaining to distribution of the software
// without specific, written prior permission.
// 
// INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
// ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
// SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
// ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
// THIS SOFTWARE.
// 

//
// msgp_hw.s
//
// The critical part of the message passing interface to the NIC
//
// $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hw.s,v 1.8 1994/11/18 20:46:44 mtm Exp $
// 
#include <i860paragon/dp.h>
#include <i860paragon/msgp/msgp_asm.h>

	.file "msgp_hw.s"
	.text

#if	HANDCODE
////////////////////////////////////////////////////////////////////////
//
//	Routine:
//		mcmsg_send_hdr10
//
//	Arguments:
//		r16		route
//		r17-r26	hdr1-hdr10
//
//	Purpose:
//		Copy a header with ten components to the NIC (no EOD).
//		No data is sent.
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////
	.align	32
_mcmsg_send_hdr10::
	orh	ha%NIC, r0, r27			// Generate NIC address in r27
	ixfr	r16, f16			// Transfer route to dbl f16
	ixfr	r0,  f17
	ixfr	r17, f18
	ixfr	r18, f19
	fst.d	f16, nic_io(r27)		// Send route
	ixfr	r19, f16
	ixfr	r20, f17
	fst.d	f18, nic_io(r27)		// Send hdr1 & hdr2
	ixfr	r21, f18
	ixfr	r22, f19
	fst.d	f16, nic_io(r27)		// Send hdr3 & hdr4
	ixfr	r23, f16
	ixfr	r24, f17
	fst.d	f18, nic_io(r27)		// Send hdr5 & hdr6
	ixfr	r25, f18
	ixfr	r26, f19
	fst.d	f16, nic_io(r27)		// Send hdr7 & hdr8
	bri	r1
	fst.d	f18, nic_io(r27)		// Send hdr9 & hdr10

////////////////////////////////////////////////////////////////////////
//
//	Routine:
//		mcmsg_send_hdr10_eod
//
//	Arguments:
//		r16		route
//		r17-r26	hdr1-hdr10
//
//	Purpose:
//		Copy a header with ten components to the NIC with EOD.
//		No data is sent.
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////
	.align	32
_mcmsg_send_hdr10_eod::
	orh	ha%NIC, r0, r27			// Generate NIC address in r27
	ixfr	r16, f16			// Transfer route to dbl f16
	ixfr	r0,  f17
	ixfr	r17, f18
	ixfr	r18, f19
	fst.d	f16, nic_io(r27)		// Send route
	ixfr	r19, f16
	ixfr	r20, f17
	fst.d	f18, nic_io(r27)		// Send hdr1 & hdr2
	ixfr	r21, f18
	ixfr	r22, f19
	fst.d	f16, nic_io(r27)		// Send hdr3 & hdr4
	ixfr	r23, f16
	ixfr	r24, f17
	fst.d	f18, nic_io(r27)		// Send hdr5 & hdr6
	ixfr	r25, f20
	ixfr	r26, f21
	fst.d	f16, nic_io(r27)		// Send hdr7 & hdr8
#if	BUMPERS
	fst.d	f20, nic_io(r27)		// Send hdr9 & hdr10

	or	MCTRL_BUMP, r0, r16
	orh	0xbabe, r0,  r17
	or	0xfe9d, r17, r17
	ixfr	r16, f16
	ixfr	r17,  f17
	fst.d	f16, nic_io(r27)		// send 1st bumper

	orh	1, r0, r16
	or	MCTRL_BUMP, r16, r16
	ixfr	r16, f16
	fst.d	f16, nic_io(r27)		// send 2nd bumper

	orh	2, r0, r16
	or	MCTRL_BUMP, r16, r16
	ixfr	r16, f16
	fst.d	f16, nic_io(r27)		// send 3rd bumper

	orh	3, r0, r16
	or	MCTRL_BUMP, r16, r16
	ixfr	r16, f16
	fst.d	f16, nic_io+nic_eod(r27)	// send last bumper to EOD

#else	BUMPERS
#if	!BURST
	bri	r1
#endif	/* !BURST */
	fst.d	f20, nic_io+nic_eod(r27)	// send last bumper to EOD

#endif	BUMPERS

#if	BURST
	adds	NIC_TX_GO, r0, r16		// Load xmit release in f16
	ixfr	r16, f16
	ixfr	r0,  f17
	fst.d	f16, nic_reset_test(r27)	// Release transmit
#endif	BURST
	
	bri	r1
	nop

#if !BIGPKTS

	.text
////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_send_pkt2(mt, dummy, bp1, bp2, pkt, route, hdr1, hdr2)
//
// Arguments:
//	r16 mt:		mcmsg_task pointer
//	r17 dummy:	unused
//	r18 bp1:	Physical address of buffer
//	r19 bp2:	Physical address of last byte
//	r20 pkt:	Byte count (unrounded)
//	r21 route:	HW route
//	r22 hdr1:	Header word
//	r23 hdr2:	Header word
//
// Purpose:
//	Copies from a buffer to the NIC transmit FIFO
//	Assumes data is the last part of the packet so it appends EOD
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_send_pkt2::
	ixfr	r21, f16			// Transfer route to dbl f16
	ixfr	r22, f18			// Transfer hdr1 to dbl f18
	ixfr	r23, f19			// Transfer hdr2 to dbl f18
	orh	ha%NIC, r0, r27			// Generate NIC address in r27
	fst.d	f16, nic_io(r27)		// Send route
	adds	FIFO_ALIGN-1, r20, r29		// Round count up
	andnot	FIFO_ALIGN-1, r29, r20		// Round count off
	fst.d	f18, nic_io(r27)		// Send header
	andnot	MSG_PAGE_SIZE-1, r19, r28	// Mask for bp1 page
	andnot	MSG_PAGE_SIZE-1, r18, r21	// Mask for bp2 page
	orh	ha%_mcmsg_ltu_enable, r0, r31	// Generate LTU enable address
	ld.l	l%_mcmsg_ltu_enable(r31), r31	// LTU enable in r31
send_pkt:					// Entry from mcmsg_send_pkt*
	btne	r28, r21, 3f			// Branch if 2 pages
	addu	-LTU_MIN, r20, r0		// Check count
	bnc	2f				// Branch if too small for LTU
	and	LTU_ALIGN-1, r20, r0		// Check count
	bnc	2f				// Branch if not exact for LTU
	and	LTU_ALIGN-1, r18, r0		// Check address
	bnc	2f				// Branch if not aligned for LTU
	bte	r31, r0, 2f			// Branch if no LTU
	adds	4, r18, r16			// Calculate LTU address
	andnoth DP_LTU_ADDR_CLR, r16, r16		//
	shr	5, r20, r24			// Calculate LTU data
	adds	-1, r24, r25			//
	andh	DP_LTU_MX_MSK, r18, r17	// Select base or expansion mem, EOD
	or		r17, r25, r17
#if	BUMPERS
	orh		DP_LTU_NOEOD, r17, r17	// No EOD
#endif	BUMPERS
	stio.l	r17, r16			// Start LTU
1:
	orh	h%DP_STATUS_HI, r0, r28		// Wait for LTU
	or	l%DP_STATUS_HI, r28, r28	//
	ldio.l	r28, r30			// Load status
	and	DP_ISTAT_LTU1_CNT, r30, r0	// Check if busy
	bnc	7f				// Branch if done
#if	BUMPERS
	adds	MCTRL_BUMP, r0, r29		// Load bumper hdr in f20
	ixfr	r29, f20			//
#endif	BUMPERS
#if	BURST
	adds	NIC_TX_GO, r0, r30		// Load xmit release in f16
	ixfr	r30, f16			//
	ixfr	r0, f17				//
#endif	BURST
	orh	h%DP_LTU1_CLEAR_CNT, r0, r31	//
	nop					// Let the LTU get the bus
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
8:	ldio.l	r28, r30			// Load status
	and	DP_ISTAT_LTU1_CNT, r30, r0	// Check if busy
	bc	8b				// Branch back if busy
	ldio.l	r31, r30			// Clear count bit
#if	BUMPERS
	fst.d	f20, nic_io(r27)		// Send bumper
	fst.d	f20, nic_io(r27)		//
	fst.d	f20, nic_io(r27)		//
	fst.d	f20, nic_io+nic_eod(r27)	//
#endif	BUMPERS
	bri	r1				// Return
#if	BURST
	 fst.d	f16, nic_reset_test(r27)	// Release transmit
#else	BURST
	nop
#endif	BURST
						//
7:
#if	BUMPERS
	adds	MCTRL_BUMP, r0, r29		// Load bumper hdr in f20
	ixfr	r29, f20			//
#endif	BUMPERS
#if	BURST
	adds	NIC_TX_GO, r0, r30		// Load xmit release in f16
	ixfr	r30, f16			//
	ixfr	r0, f17				//
#endif	BURST
	orh	h%DP_LTU1_CLEAR_CNT, r0, r31	//
	ldio.l	r31, r30			// Clear count bit
#if	BUMPERS
	fst.d	f20, nic_io(r27)		// Send bumper
	fst.d	f20, nic_io(r27)		//
	fst.d	f20, nic_io(r27)		//
	fst.d	f20, nic_io+nic_eod(r27)	//
#endif	BUMPERS
	bri	r1				// Return
#if	BURST
	 fst.d	f16, nic_reset_test(r27)	// Release transmit
#else	BURST
	nop
#endif
						//
2:	mov	r18, r16			// Bail out - too hard
	br	_mcmsg_fifo_out_eod		//
	 mov	r20, r17			//
						//
3:	adds	MSG_PAGE_SIZE, r21, r21		// Calculate first page count
	subs	r21, r18, r21			//
	addu	-LTU_MIN, r21, r0		// Check first page count
	bnc	4f				// Branch if too small for LTU
	subs	r20, r21, r16			// Calculate remaining count
	addu	-LTU_MIN, r16, r0		// Check remaining count
	bnc	4f				// Branch if too small for LTU
	and	LTU_ALIGN-1, r16, r0		// Check remaining count
	bnc	4f				// Branch if not exact for LTU
	and	LTU_ALIGN-1, r18, r0		// Check buffer alignment
	bnc	4f				// Branch if not LTU aligned
	adds	4, r18, r16			// Calculate LTU address
	andnoth DP_LTU_ADDR_CLR, r16, r16		//
	shr	5, r21, r24			// Calculate LTU data
	adds	-1, r24, r24			//
	andh	DP_LTU_MX_MSK, r18, r17	// Select base or expansion mem, EOD
	or		r17, r24, r17		//
	orh		DP_LTU_NOEOD, r17, r17	// No EOD
	stio.l	r17, r16			// Start LTU
	orh	h%DP_STATUS_HI, r0, r29		// Wait for LTU
	or	l%DP_STATUS_HI, r29, r29	//
	ldio.l	r29, r30			// Load status
	and	DP_ISTAT_LTU1_CNT, r30, r0	// Check if busy
	bnc	7f				// Branch if done
	adds	4, r28, r16			// Calculate second LTU address
	andnoth DP_LTU_ADDR_CLR, r16, r16		//
	subs	r20, r21, r24			// Calculate second LTU data
	shr	5, r24, r24			//
	adds	-1, r24, r24			//
	andh	DP_LTU_MX_MSK, r28, r17	// Select base or expansion mem, EOD
	or		r17, r24, r17
#if	BUMPERS
	orh		DP_LTU_NOEOD, r17, r17	// No EOD
#endif	BUMPERS
	orh	h%DP_LTU1_CLEAR_CNT, r0, r31	// Clear count bit
	nop					// Let the LTU get the bus
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
8:	ldio.l	r29, r30			// Load status
	and	DP_ISTAT_LTU1_CNT, r30, r0	// Check if busy
	bc	8b				// Branch back if busy
	ldio.l	r31, r30			//
	br	1b				//
	 stio.l	r17, r16			// Start LTU
7:	adds	4, r28, r16			// Calculate second LTU address
	andnoth DP_LTU_ADDR_CLR, r16, r16		//
	subs	r20, r21, r24			// Calculate second LTU data
	shr	5, r24, r24			//
	adds	-1, r24, r24			//
	andh	DP_LTU_MX_MSK, r28, r17	// Select base or expansion mem, EOD
	or		r17, r24, r17
#if	BUMPERS
	orh		DP_LTU_NOEOD, r17, r17	// No EOD
#endif	BUMPERS
	orh	h%DP_LTU1_CLEAR_CNT, r0, r31	// Clear count bit
	ldio.l	r31, r30			//
	br	1b				//
	 stio.l	r17, r16			// Start LTU
						//
4:	mov	r18, r16			// Give up, too complicated
	mov	r19, r17			//
	br	_mcmsg_send_buf			//
	 mov	r20, r18			//

////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_send_pkt4(mt, dummy, bp1, bp2, pkt,
//			route, hdr1, hdr2, hdr3, hdr4)
//	mcmsg_send_pkt6(mt, dummy, bp1, bp2, pkt,
//			route, hdr1, hdr2, hdr3, hdr4, hdr5, hdr6)
//
// Arguments:
//	r16 mt:		mcmsg_task pointer
//	r17 dummy:	unused
//	r18 bp1:	Physical address of buffer
//	r19 bp2:	Physical address of last byte
//	r20 pkt:	Byte count (unrounded)
//	r21 route:	HW route
//	r22 hdr1:	Header word
//	r23 hdr2:	Header word
//	r24 hdr3:	Header word
//	r25 hdr4:	Header word
//	r26 hdr5:	Header word
//	r27 hdr6:	Header word
//
// Purpose:
//	Copies from a buffer to the NIC transmit FIFO
//	Assumes data is the last part of the packet so it appends EOD
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_send_pkt6::
	ixfr	r26, f22			// Transfer hdr5 to dbl f22
	ixfr	r27, f23			// Transfer hdr6 to dbl f22
_mcmsg_send_pkt4::
	ixfr	r21, f16			// Transfer route to dbl f16
	ixfr	r22, f18			// Transfer hdr1 to dbl f18
	ixfr	r23, f19			// Transfer hdr2 to dbl f18
	orh	ha%NIC, r0, r27			// Generate NIC address in r27
	fst.d	f16, nic_io(r27)		// Send route
	ixfr	r24, f20			// Transfer hdr3 to dbl f20
	fst.d	f18, nic_io(r27)		// Send header
	ixfr	r25, f21			// Transfer hdr4 to dbl f20
	adds	FIFO_ALIGN-1, r20, r29		// Round count up
	andnot	FIFO_ALIGN-1, r29, r20		// Round count off
	fst.d	f20, nic_io(r27)		// Send header
	andnot	MSG_PAGE_SIZE-1, r19, r28	// Mask for bp1 page
	andnot	MSG_PAGE_SIZE-1, r18, r21	// Mask for bp2 page
	fst.d	f22, nic_io(r27)		// Send header
	orh	ha%_mcmsg_ltu_enable, r0, r31	// Generate LTU enable address
	br	send_pkt			// Do the rest in common
	 ld.l	l%_mcmsg_ltu_enable(r31), r31	// LTU enable in r31

////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_recv_buf_even(bp1, bp2, pkt)
//
// Arguments:
//	r16 bp1:	Physical address of buffer
//	r17 bp2:	Physical address of last byte
//	r18 pkt:	Byte count (unrounded)
//
// Purpose:
//	Copies from the NIC receive FIFO to a buffer
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

	.align	32
_mcmsg_recv_buf_even::
	andnot	MSG_PAGE_SIZE-1, r17, r28	// Mask for bp1 page
	andnot	MSG_PAGE_SIZE-1, r16, r21	// Mask for bp2 page
	orh	ha%_mcmsg_ltu_enable, r0, r31	// Generate LTU enable address
	ld.l	l%_mcmsg_ltu_enable(r31), r31	// LTU enable in r31
	btne	r28, r21, 3f			// Branch if 2 pages
	addu	-LTU_MIN, r18, r0		// Check count
	bnc	2f				// Branch if too small for LTU
	and	LTU_ALIGN-1, r18, r0		// Check count
	bnc	2f				// Branch if not exact for LTU
	and	LTU_ALIGN-1, r16, r0		// Check address
	bnc	2f				// Branch if not aligned for LTU
	bte	r31, r0, 2f			// Branch if no LTU
	andnoth DP_LTU_ADDR_CLR, r16, r20		// Calculate LTU address
	shr	5, r18, r24			// Calculate LTU data
	adds	-1, r24, r25			//
	andh	DP_LTU_MX_MSK, r16, r27 // Select base or expansion memory
	or		r27, r25, r27
	stio.l	r27, r20			// Start LTU
1:
	orh	h%DP_STATUS_HI, r0, r28		// Wait for LTU
	or	l%DP_STATUS_HI, r28, r28	//
	ldio.l	r28, r30			// Load status
	and	DP_ISTAT_LTU0_CNT, r30, r0	// Check if busy
	bnc	7f				// Branch if done
	orh	h%DP_LTU0_CLEAR_CNT, r0, r31	//
	nop					// Let the LTU get the bus
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
8:	ldio.l	r28, r30			// Load status
	and	DP_ISTAT_LTU0_CNT, r30, r0	// Check if busy
	bc	8b				// Branch back if busy
	bri	r1				// Return
	 ldio.l	r31, r30			// Clear count bit
						//
7:	orh	h%DP_LTU0_CLEAR_CNT, r0, r31	//
	bri	r1				// Return
	 ldio.l	r31, r30			// Clear count bit
						//
2:	br	_mcmsg_fifo_in			// Bail out - too hard
	 mov	r18, r17			//
						//
3:	adds	MSG_PAGE_SIZE, r21, r21		// Calculate first page count
	subs	r21, r16, r21			//
	addu	-LTU_MIN, r21, r0		// Check first page count
	bnc	4f				// Branch if too small for LTU
	subs	r18, r21, r20			// Calculate remaining count
	addu	-LTU_MIN, r20, r0		// Check remaining count
	bnc	4f				// Branch if too small for LTU
	and	LTU_ALIGN-1, r20, r0		// Check remaining count
	bnc	4f				// Branch if not exact for LTU
	and	LTU_ALIGN-1, r16, r0		// Check buffer alignment
	bnc	4f				// Branch if not LTU aligned
	andnoth DP_LTU_ADDR_CLR, r16, r20		// Calculate LTU address
	shr	5, r21, r24			// Calculate LTU data
	adds	-1, r24, r24			//
	andh	DP_LTU_MX_MSK, r16, r27	// Select base or expansion memory
	or		r27, r24, r27
	stio.l	r27, r20			// Start LTU
	orh	h%DP_STATUS_HI, r0, r29		// Wait for LTU
	or	l%DP_STATUS_HI, r29, r29	//
	ldio.l	r29, r30			// Load status
	and	DP_ISTAT_LTU0_CNT, r30, r0	// Check if busy
	bnc	7f				// Branch if done
	andnoth DP_LTU_ADDR_CLR, r28, r20		// Calculate second LTU address
	subs	r18, r21, r24			// Calculate second LTU data
	shr	5, r24, r24			//
	adds	-1, r24, r24			//
	andh	DP_LTU_MX_MSK, r28, r27	// Select base or expansion memory
	or		r27, r24, r27
	orh	h%DP_LTU0_CLEAR_CNT, r0, r31	// Clear count bit
	nop					// Let the LTU get the bus
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
	nop					//
8:	ldio.l	r29, r30			// Load status
	and	DP_ISTAT_LTU0_CNT, r30, r0	// Check if busy
	bc	8b				// Branch back if busy
	ldio.l	r31, r30			//
	br	1b				//
	 stio.l	r27, r20			// Start LTU
7:	
	andnoth DP_LTU_ADDR_CLR, r28, r20		// Calculate second LTU address
	subs	r18, r21, r24			// Calculate second LTU data
	shr	5, r24, r24			//
	adds	-1, r24, r24			//
	andh	DP_LTU_MX_MSK, r28, r27	// Select base or expansion memory
	or		r27, r24, r27
	orh	h%DP_LTU0_CLEAR_CNT, r0, r31	// Clear count bit
	ldio.l	r31, r30			//
	br	1b				//
	 stio.l	r27, r20			// Start LTU
						//
4:	br	_mcmsg_recv_buf			// Give up, too complicated
	 nop					//

#endif !BIGPKTS
#endif	HANDCODE
