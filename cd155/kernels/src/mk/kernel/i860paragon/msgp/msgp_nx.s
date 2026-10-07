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
// msgp_nx.s
//
// The critical part of the message passing interface to the NIC
//
// $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_nx.s,v 1.4 1994/12/06 17:53:44 tnt Exp $
// 
#include <i860paragon/msgp/msgp_asm.h>

	.file "msgp_nx.s"

#if	HANDCODE
#if !BIGPKTS
	.text
////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_recv_nxn(hdr1, hdr2)
//
// Arguments:
//	r16 hdr1:	Header word
//	r17 hdr2:	Header word
//
// Locals:
//	r31 scratch
//	r30 scratch
//	f31 save return
//	f18-9 NIC interface
//	f20-1 NIC interface
//
// Output: to mcmsg_recv_nxn_rq or mcmsg_recv_nxn_xm
//	r16 si
//	r17 mt
//	r18 offset
//	r19 pkt
//	r20 give
//	r21 pid_si
//	r22 source_pid
//
// Purpose:
//	Critical portion of receive NXN packet
//
// Returns:
//	none
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_recv_nxn::
	orh	ha%NIC+nic_io, r0, r31		// Develop NIC pointer
	fld.d	l%NIC+nic_io(r31), f18		// Read source_pid and offset
	and	0xFFFF, r17, r19		// Extract pkt
	ixfr	r1, f31				// save r1
	fld.d	l%NIC+nic_io(r31), f20		// Read filler
	shr	11, r16, r20			// Extract give
	andnot	0x1F, r20, r20			//
	fxfr	f18, r16			// Extract source_pid
	call	_mcmsg_lookup_sequence		// Lookup si (uses r28-r31)
	 shr	16, r17, r17			// Extract sequence
	bte	0, r16, recv_nxn_flush		// Branch if not found
	ld.l	si_nxrq_pidsi(r16), r21		// Load pid_si
	fxfr	f18, r22			// Extract source_pid
	ld.l	si_ppid_savail(r21), r31	// Update send_avail
	fxfr	f19, r18			// Extract offset
	adds	r20, r31, r31			//  add give to send_avail
	ld.l	si_nxrq_count(r16), r30		// Get message size
	st.l	r31, si_ppid_savail(r21)	//  save send_avail
	ld.s	si_method(r16), r31		// Get si method
	ld.l	si_mcmsg_task(r16), r17		// Get mcmsg_task pointer (mt)
	subu	SELMETH_RECV_XMSG, r31, r31	// Check for buffered receive
	bte	0, r31, recv_nxn_xm		// Branch if buffered
	br	_mcmsg_recv_nxn_rq		// Go on
	 fxfr	f31, r1				// restore r1

recv_nxn_xm:
	br	_mcmsg_recv_nxn_xm		// Go on
	 fxfr	f31, r1				// restore r1

recv_nxn_flush:
	mov	r19, r16				// pkt length
	br	_mcmsg_fifo_flush		// Flush the packet
	 fxfr	f31, r1				// restore r1

#endif !BIGPKTS
#endif	HANDCODE
