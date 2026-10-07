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
//                          All Rights Reserved
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
// msgp_dispatch.s
//
// The critical part of the message processor dispatch loop.
//
// $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_dispatch.s,v 1.10 1994/11/18 20:46:40 mtm Exp $
// 
#include <i860paragon/msgp/msgp_asm.h>

	.file "msgp_dispatch.s"

#if	HANDCODE
#if !BIGPKTS

	.text
////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_dispatch_user_recv
//
// Arguments:
//	none
//
// Purpose:
//	Dispatch loop for 1 user process and send interrupts disabled
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_dispatch_user_recv::
	addu	-64, sp, sp			// Make a new frame
	st.l	fp, 0(sp)			//
	addu	0, sp, fp			//
	st.l	r1, 4(fp)			//
	st.l	r4, 8(fp)			//
	st.l	r5, 12(fp)			//
	st.l	r6, 16(fp)			//
	st.l	r7, 20(fp)			//
	st.l	r8, 24(fp)			//
	st.l	r9, 28(fp)			//
	st.l	r10, 32(fp)			//
	st.l	r11, 36(fp)			//
	st.l	r12, 40(fp)			//
	st.l	r13, 44(fp)			//
	st.l	r14, 48(fp)			//
	st.l	r15, 52(fp)			//
						// Load kernel index
	orh	ha%_mcmsg_kernel_post_page_out, r0, r31
	ld.l	l%_mcmsg_kernel_post_page_out(r31), r4
						// Load kernel post page pointer
	orh	ha%_mcmsg_kernel_post_page, r0, r31
	ld.l	l%_mcmsg_kernel_post_page(r31), r16
	shl	POST_SHIFT, r4, r17		//
	addu	r16, r17, r5			// r5->kernel post page entry
						// Load user index
	orh	ha%_mcmsg_user_post_page_out, r0, r31
	ld.l	l%_mcmsg_user_post_page_out(r31), r6
						// Load user post page pointer
	orh	ha%_mcmsg_user_post_page, r0, r31
	ld.l	l%_mcmsg_user_post_page(r31), r16
	shl	POST_SHIFT, r6, r17		//
	addu	r16, r17, r7			// r7->user post page entry
						//
	orh	h%_mcmsg_hw, r0, r8		//
	or	l%_mcmsg_hw, r8, r8		// r8->mcmsg_hw
	ld.l	0(r8), r16			// Read it into cache
						//
	orh	h%NIC, r0, r9			//
	or	l%NIC, r9, r9			// r9->NIC
						//
	orh	h%_mcmsg_recv_switch, r0, r10	//
	or	l%_mcmsg_recv_switch, r10, r10	// r10->mcmsg_recv_switch
						//
	orh	h%NIC_STAT_MASK, r0, r12	//
	or	l%NIC_STAT_MASK, r12, r12	// r12 = NIC_STAT_MASK
						//
	orh	h%NIC_STAT_OK, r0, r13		//
	or	l%NIC_STAT_OK, r13, r13		// r13 = NIC_STAT_OK
						//

//	call	_mcmsg_catch_regs
//	 nop

top:						//

	ld.c	epsr, r16			// Check for interrupt
	andh	h%EPSR_INT, r16, r0		//
	bc	kernel				// Branch if no interrupt
						//
	fld.d	nic_status(r9), f16		// Read NIC status
	fxfr	f16, r16			// Check desired bits
	and	r12, r16, r17			//
	btne	r17, r13, nicerr		// Jump if wierd
						//
	fld.d	nic_io(r9), f16			// Read first words of header
#if    BUMPERS
	or	NIC_RX_LOW, r0, r16		// Set normal receive enable
	st.l	r16, hw_recv_status(r8)		//
#endif BUMPERS
	fxfr	f16, r16			// Get header in arg regs
	and	MCTRL_END-1, r16, r18		// Isolate method
	shl	2, r18, r18			// Shift for offset
	ld.l	r18(r10), r18			// Get recv proc pointer

//	call	_mcmsg_catch_regs
//	 fxfr	f17, r17

	calli	r18				// Call recv proc
	 fxfr	f17, r17			//
#if    BUMPERS
	fld.d	hw_recv_enable(r8), f16		// Clear old interrupt mask
	fst.d	f16, nic_clear(r9)		//
	ld.l	hw_recv_status(r8), r16		// Change to new mask
	orh	h%NIC_ERRORS, r16, r16		// Add error bits
	or	l%NIC_ERRORS, r16, r16		// Add error bits
	ixfr	r16, f16			//
	st.l	r16, hw_recv_enable(r8)		//
	fst.d	f16, nic_set(r9)		// Set new interrupt mask
#endif BUMPERS

#if 0
	fld.d	nic_status(r9), f16		// Read NIC status
	fxfr	f16, r16			//
	and	NIC_RX_1, r16, r0		// Check can read 1
	bc	nicerr				// Jump if can't
#endif

loopbk:	ld.l	hw_send_int_enable(r8), r16	// Check send interrupts
	bte	r16, r0, top			// Branch back if none
	br	exit				// Exit
	 nop					//
						//
kernel:						// Check for kernel requests
	ld.l	post_method(r5), r28		// Check method
	bte	r28, r0, user			// Branch if zero
	subu	POST_MAX-1, r28, r0		// Check bounds
	bnc	2f				// Branch if out of bounds
	ld.l	post_mt(r5), r16		// Get mcmsg_task pointer
#if	TRACE_CALL
	orh	h%_mcmsg_post_switch, r0, r18	// Get switch pointer
	or	l%_mcmsg_post_switch, r18, r18	//
	call	_mcmsg_exp_dispatch_call	// Make the call
	 or	r5, r0, r17			// Post page entry pointer
#else	TRACE_CALL
	shl	2, r28, r28			// Fix index
	orh	ha%_mcmsg_post_switch, r28, r28	// Develop proc pointer
	ld.l	l%_mcmsg_post_switch(r28), r28	// r28 -> proc
	ld.l	post_arg0(r5), r17		// Get arguments
	ld.l	post_arg1(r5), r18		//
	ld.l	post_arg2(r5), r19		//
	ld.l	post_arg3(r5), r20		//
	ld.l	post_arg4(r5), r21		//
	ld.l	post_arg5(r5), r22		//
	ld.l	post_arg6(r5), r23		//
	ld.l	post_arg7(r5), r24		//
	ld.l	post_arg8(r5), r25		//

//	call	_mcmsg_catch_regs
//	 nop

	calli	r28				// Call proc
	 ld.l	post_arg9(r5), r26		//
#endif	TRACE_CALL
1:	st.l	r16, post_status(r5)		// Store status
	st.l	r0, post_method(r5)		// Clear method
	addu	1, r4, r4			// Increment index
	and	POST_SLOTS-1, r4, r4		// 
						// Store kernel index
	orh	ha%_mcmsg_kernel_post_page_out, r0, r31
	st.l	r4, l%_mcmsg_kernel_post_page_out(r31)
	orh	ha%_mcmsg_user_post_tasks, r0, r31
	ld.l	l%_mcmsg_user_post_tasks(r31), r16
	btne	1, r16, exit			// Return if user tasks changed
						// Load kernel post page pointer
	orh	ha%_mcmsg_kernel_post_page, r0, r31
	ld.l	l%_mcmsg_kernel_post_page(r31), r16
	shl	POST_SHIFT, r4, r17		//
	br	loopbk				// Loop back
	 addu	r16, r17, r5			// r5->kernel post page entry
2:	br	1b				//
	 subs	1, r0, r16			// Status -1


user:						// Check for user requests
	ld.l	post_method(r7), r28		// Check method
	bte	r28, r0, top			// Branch if zero
	subu	POST_MAX-1, r28, r0		// Check bounds
	bnc	2f				// Branch if out of bounds
						// Get mcmsg_task pointer
	orh	ha%_mcmsg_user_mcmsg_task, r0, r31
	ld.l	l%_mcmsg_user_mcmsg_task(r31), r16
	shl	2, r28, r28			// Fix index
	orh	ha%_mcmsg_user_switch, r28, r28	// Develop proc pointer
	ld.l	l%_mcmsg_user_switch(r28), r28	// r28 -> proc
	ld.l	post_arg0(r7), r17		// Get arguments
	ld.l	post_arg1(r7), r18		//
	ld.l	post_arg2(r7), r19		//
	ld.l	post_arg3(r7), r20		//
	ld.l	post_arg4(r7), r21		//
	ld.l	post_arg5(r7), r22		//
	ld.l	post_arg6(r7), r23		//
	ld.l	post_arg7(r7), r24		//
	ld.l	post_arg8(r7), r25		//

//	call	_mcmsg_catch_regs
//	 nop

	calli	r28				// Call proc
	 ld.l	post_arg9(r7), r26		//
1:	st.l	r16, post_status(r7)		// Store status
	st.l	r0, post_method(r7)		// Clear method
	addu	1, r6, r6			// Increment index
	and	POST_SLOTS-1, r6, r6		// 
						// Store user index
	orh	ha%_mcmsg_user_post_page_out, r0, r31
	st.l	r6, l%_mcmsg_user_post_page_out(r31)
						// Load user post page pointer
	orh	ha%_mcmsg_user_post_page, r0, r31
	ld.l	l%_mcmsg_user_post_page(r31), r16
	shl	POST_SHIFT, r6, r17		//
	br	loopbk				// Loop back
	 addu	r16, r17, r7			// r7->user post page entry
2:	br	1b				//
	 subs	1, r0, r16			// Status -1

exit:						//
	ld.l	52(fp), r15			// Destroy frame
	ld.l	48(fp), r14			//
	ld.l	44(fp), r13			//
	ld.l	40(fp), r12			//
	ld.l	36(fp), r11			//
	ld.l	32(fp), r10			//
	ld.l	28(fp), r9			//
	ld.l	24(fp), r8			//
	ld.l	20(fp), r7			//
	ld.l	16(fp), r6			//
	ld.l	12(fp), r5			//
	ld.l	8(fp), r4			//
	ld.l	4(fp), r1			//
	ld.l	0(fp), r31			//
	addu	64, sp, sp			//
	bri	r1				// Return
	 or	r31, r0, fp			//

nicerr:
	orh	ha%_mcmsg_nic_err_stat, r0, r31	//
	st.l	r16, l%_mcmsg_nic_err_stat(r31)	//
	call	_mcmsg_catch_regs		//
	 nop					//
x:	br	x
	 nop

#endif !BIGPKTS

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_recv_even
//
// Arguments:
//	none
//
// Purpose:
//	Receives the "even-up" word before a bumper
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

_mcmsg_recv_even::				//
//	bri	r1				// return
//	 nop					//
	orh	ha%_mcmsg_even_count, r0, r31
	ld.l	l%_mcmsg_even_count(r31), r30
	adds	1, r30, r30
	bri	r1
	 st.l	r30, l%_mcmsg_even_count(r31)

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_recv_bump
//
// Arguments:
//	none
//
// Purpose:
//	Receives the bumper
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

_mcmsg_recv_bump::				//
NICio	=	0xFFFF0020			// NIC data
NIC_EOD_IN =	0x00000400			// 
#if	BUMPERS
	orh	ha%NICio, r0, r31		// 
	fld.d	l%NICio(r31), f30		// bumper 1
	or	NIC_EOD_IN, r0, r29		// new enable
	orh	ha%_mcmsg_hw+0, r0, r30		// 
	st.l	r29, l%_mcmsg_hw+0(r30)		// set it
	fxfr	f30, r26			// (quiesce bus)
	fld.d	l%NICio(r31), f30		// bumper 2
						// clear byte count
	orh	ha%_mcmsg_recv_byte_count, r0, r28
	st.l	r0, l%_mcmsg_recv_byte_count(r28)
	fxfr	f30, r26			// (quiesce bus)
	bri	r1				// return
	 fld.d	l%NICio(r31), f30		// bumper 3
#else	BUMPERS
	bri	r1
	 nop
#endif	BUMPERS

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_validate_real(address, dirbase)
//
// Arguments:
//	address:	Address to validate
//	dirbase:	Directore base
//
// Purpose:
//	Translate the virtual address to physical
//
// Returns:
//	Physical address if valid, else 0
//
////////////////////////////////////////////////////////////////////////
_mcmsg_validate_real::				//
	shr	20, r16, r28			//
	and	0xffc, r28, r30			//
	ld.l	r17(r30), r18			//
	shr	10, r16, r28			//
	and	0xffc, r28, r30			//
	and	1, r18, r0			//
	bc	4f				//
	andnot	0xfff, r18, r19			//
	adds	r30, r19, r20			//
	ld.l	r0(r20), r18			//
	and	0xfff, r16, r28			//
	and	1, r18, r0			//
	bc	4f				//
	andnot	0xfff, r18, r29			//
	bri	r1				//
	or	r28, r29, r16			//
4:	bri	r1				//
	 mov	r0, r16				//

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_validate_rw1(address, count, dirbase, rw_valid)
//
// Arguments:
//	address:	Address of buffer to validate
//	count:		Length of buffer
//	dirbase:	Directory base
//	rw_valid:	Type of access to verify.  Bit 0 = present.
//				Bit 1 = write access.
//
// Purpose:
//	Translate the virtual address to physical.
//	Verify present.  Verify write access if bit set in rw_valid.
//	Checks the end point also.
//
// Returns:
//	Physical address of beginning of buffer if valid, else 0.
//
////////////////////////////////////////////////////////////////////////

_mcmsg_validate_rw1::				//
	shr	20, r16, r28			// r26 = dirtab offset of buf
	and	0xffc, r28, r26			//
	ld.l	r18(r26), r20			// r20 = dirtab entry
	adds	-1, r17, r17			//  r17 = count-1
	shr	10, r16, r28			// r30 = pagetab offset of buf
	and	0xffc, r28, r30			//
	and	1, r20, r0			// Test present bit in dirtab
	bc	7f				// Branch if not present
						//
	andnot	0xfff, r20, r22			// Isolate address of pagetab
	adds	r30, r22, r23			// Add pagetab offset
	ld.l	r0(r23), r29			// r29 = pagetab entry
	and	MSG_PAGE_SIZE-1, r16, r28	//  Isolate vpage offset of buf
	adds	r17, r28, r31			//  Add count-1
	andnot	MSG_PAGE_SIZE-1, r31, r30	//  Check for cross to next vpg
	adds	r17, r16, r24			//  r24 = buf+count-1
	and		r19, r29, r31			// Check present|writeable bits in pagetab
	btne	r19, r31, 7f			// Branch if not present|writeable
						//
	and	0xfff, r16, r28			// Isolate page offset of buf
	andnot	0xfff, r29, r22		// Isolate phys page address
	or	r28, r22, r16			// r16 = phys address of buf
	btne	0, r30, 3f			// Branch if v. page crossing
	bri	r1				// Return
	 adds	r16, r17, r27			//  r27 = phys buf + count-1
						//
3:	shr	20, r24, r28			// r21 = dirtab offset
	and	0xffc, r28, r21			//
	btne	r21, r26, 6f			// Branch if different dir entry
						//
4:	shr	10, r24, r28			// r30 = pagetab offset
	and	0xffc, r28, r30			//
	andnot	0xfff, r20, r22			// Isolate address of pagetab
	adds	r30, r22, r23			// Add pagetab offset
	ld.l	r0(r23), r29			// r29 = pagetab entry
	and		r19, r29, r31			// Check present|writable bits
	btne	r19, r31, 7f			// Branch if not present|writeable
						//
5:	and	0xfff, r24, r28			// Isolate page offset
	andnot	0xfff, r29, r31			// Isolate phys page address
	bri	r1				// Return
	 or	r28, r31, r27			//  r27 = phys address
						//
6:	ld.l	r18(r21), r20			// r20 = dirtab entry
	and	1, r20, r0			// Check present bit
	bnc	4b				// Branch if present
7:	bri	r1				// Return, invalid address
	 mov	r0, r16				// Return 0
						//


////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_validate2()
//
// Arguments:
//	Must be called directly after mcmsg_validate_rw1
//
// Purpose:
//	Finishes mcmsg_validate_rw1
//
// Returns:
//	Physical address of end of buffer
//
////////////////////////////////////////////////////////////////////////

_mcmsg_validate2::				//
	bri	r1				//
	 mov	r27, r16			//
#endif	HANDCODE

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_send()
//
// Arguments:
//
// Purpose:
//	Send a packet
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

_mcmsg_send::					//
	orh	h%_mcmsg_send_switch, r0, r31	// Load thru
	or	l%_mcmsg_send_switch, r31, r31	// send switch
	shl	2, r17, r30			// using method
	ld.l	r31(r30), r17			//

#if BIGPKTS
	orh 	ha%_mcmsg_send_waiting, r0, r31	// Check send waiting
	ld.l	l%_mcmsg_send_waiting(r31), r28
	btne	r28, r0, 4f						// exit if non-zero
#endif BIGPKTS

	orh	ha%_mcmsg_send_store_out, r0, r31
	ld.b	l%_mcmsg_send_store_out(r31), r28
	orh	ha%_mcmsg_send_store_in, r0, r31
	ld.b	l%_mcmsg_send_store_in(r31), r30
	btne	r28, r30, 4f			// Exit if in != out
	orh	h%NIC, r0, r31			// Get NIC stat
	fld.d	nic_status(r31), f16		// thru f16
	fxfr	f16, r30			// in r30
#if	BURST
	and	NIC_TX_EMPTY, r30, r0		// Check NIC_TX_EMPTY
	bc	2f				// Exit if not
	adds	NIC_TX_STOP, r0, r30		// Stop xmit
1:	ixfr	r30, f18			//  FIFO
	fst.d	f18, nic_reset_test(r31)	//
	nop					// This nop makes it go faster?
#else	BURST
#if	BUMPERS
	and	NIC_TX_EMPTY, r30, r0		// Check NIC_TX_EMPTY
#else	BUMPERS
	and	NIC_TX_NOT_ALMOST_FULL, r30, r0		// Check NOT ALMOST FULL
#endif	BUMPERS
	bc	2f				// Exit if not
1:						//
#endif	BURST
	bri	r17				//
	 nop					//
						//
2:
#if 1
	orh	h%_mcmsg_hw, r0, r29		// Access mcmsg_hw
	or	l%_mcmsg_hw, r29, r29		//
	ld.l	hw_recv_status(r29), r28	// Get recv test bit
	and	r28, r30, r0			// Test in status
	bnc	4f				// Branch on recv
	ld.l	hw_send_loop(r29), r27		// Get send loop count
	adds	-1, r0, r29			// Set increment
	bla	r29, r27, 3f			// Start loop
	 fld.d	nic_status(r31), f16		// Get NIC stat
3:	fxfr	f16, r30			// in r30
#if	BURST
	and	NIC_TX_EMPTY, r30, r0		// Check NIC_TX_EMPTY
	bnc.t	1b				// Exit if empty
	 adds	NIC_TX_STOP, r0, r30		// Stop xmit
#else	BURST
	and	NIC_TX_EMPTY, r30, r0		// Check NIC_TX_EMPTY
	bnc.t	1b				// Exit if not
	 nop					//
#endif	BURST
	and	r28, r30, r0			// Test recv status
	bnc	4f				//
	bla	r29, r27, 3b			// Start loop
	 fld.d	nic_status(r31), f16		// Get NIC stat
#endif

4:	br	_mcmsg_save_send		// Exit if recv ready
	 nop					//
						//
_mcmsg_send_tail::				//
	fld.l	4(fp), f31			// Get old r1
	orh	h%_mcmsg_send_continue, r0, r31	// Replace it
	or	l%_mcmsg_send_continue, r31, r31
	bri	r1				// Exec ret code
	 st.l	r31, 4(fp)			//
_mcmsg_send_continue::				//
	br	_mcmsg_send			// Go mcmsg_send
	 fxfr	f31, r1				// with old r1

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_eager_send()
//
// Arguments:
//
// Purpose:
//	Send a packet
//	Don't check the FIFO yet
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////


_mcmsg_eager_send::					//
	orh	h%_mcmsg_send_switch, r0, r31	// Load thru
	or	l%_mcmsg_send_switch, r31, r31	// send switch
	shl	2, r17, r30			// using method
	ld.l	r31(r30), r17			//
	orh	ha%_mcmsg_send_store_out, r0, r31
	ld.b	l%_mcmsg_send_store_out(r31), r28
	orh	ha%_mcmsg_send_store_in, r0, r31
	ld.b	l%_mcmsg_send_store_in(r31), r30
	btne	r28, r30, 4f			// Exit if in != out
	bri	r17				//
	 nop					//
						//
4:	br	_mcmsg_save_send		// Exit if recv ready
	 nop					//
						//
_mcmsg_eager_send_tail::			//
	ld.l	4(fp), r30			// Get old r1
	orh	h%_mcmsg_eager_send_continue, r0, r31	// Replace it
	or	l%_mcmsg_eager_send_continue, r31, r31
	bri	r1				// Exec ret code
	 st.l	r31, 4(fp)			//
_mcmsg_eager_send_continue::			//
	br	_mcmsg_eager_send		// Go mcmsg_eager_send
	 or	r30, r0, r1			// with old r1

////////////////////////////////////////////////////////////////////////
//
// Routine:
//	mcmsg_catch_regs
//
// Arguments:
//	registers, implicitly
//
// Purpose:
//	Dumps all the registers (including r1 return address to ID caller)
//	in mcmsg_regs for debugging.
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

_mcmsg_catch_regs::				//
	addu	-16, sp, sp			// Make a little space on stack
	st.l	r31, 0(sp)			// Save two registers
	st.l	r30, 4(sp)			//
	orh	h%_mcmsg_regs, r0, r31		// Develop pointer to
	or	l%_mcmsg_regs, r31, r31		// mcmsg_regs
	st.l	r0,    0(r31)			// Save registers
	st.l	r1,    4(r31)			//
	addu	16, sp, r30			// Use entry sp value
	st.l	r30,   8(r31)			//
	st.l	r3,   12(r31)			//
	st.l	r4,   16(r31)			//
	st.l	r5,   20(r31)			//
	st.l	r6,   24(r31)			//
	st.l	r7,   28(r31)			//
	st.l	r8,   32(r31)			//
	st.l	r9,   36(r31)			//
	st.l	r10,  40(r31)			//
	st.l	r11,  44(r31)			//
	st.l	r12,  48(r31)			//
	st.l	r13,  52(r31)			//
	st.l	r14,  56(r31)			//
	st.l	r15,  60(r31)			//
	st.l	r16,  64(r31)			//
	st.l	r17,  68(r31)			//
	st.l	r18,  72(r31)			//
	st.l	r19,  76(r31)			//
	st.l	r20,  80(r31)			//
	st.l	r21,  84(r31)			//
	st.l	r22,  88(r31)			//
	st.l	r23,  92(r31)			//
	st.l	r24,  96(r31)			//
	st.l	r25, 100(r31)			//
	st.l	r26, 104(r31)			//
	st.l	r27, 108(r31)			//
	st.l	r28, 112(r31)			//
	st.l	r29, 116(r31)			//
	ld.l	4(sp), r30			// Get original r30 and r31
	st.l	r30, 120(r31)			// from stack
	ld.l	0(sp), r30			//
	st.l	r30, 124(r31)			//
	ld.l	0(sp), r31			// Restore registers
	ld.l	4(sp), r30			//
	bri	r1				// Return
	 addu	16, sp, sp			// Fix stack
