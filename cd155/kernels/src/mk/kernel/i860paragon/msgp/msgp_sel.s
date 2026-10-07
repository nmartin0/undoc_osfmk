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
// msgp_sel.s
//
// The critical part of selection
//
// $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_sel.s,v 1.5 1994/11/18 20:47:54 mtm Exp $
// 
#include <i860paragon/msgp/msgp_asm.h>

	.file "msgp_sel.s"

#if	HANDCODE
	.text
////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_selector_lookup_si(sel, value)
//
// Arguments:
//	r16 sel:	points to selector
//	r17 value:	key value to select on
//
// Locals:
//	r16 si		select item under consideration
//	r17 value:	key value to select on
//	r30 sitail	points to tail of hash bucket circular list
//	r31 test	item value
//
// Purpose:
//	Looks up a select item in a selector
//
// Returns:
//	select item pointer or zero if not found
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_selector_lookup_si::
	btne	0, r17, 1f			// Branch if not zero
	bri	r1				// Return
	 ld.l	sel_zero(r16), r16		//  Get zero pointer

1:	and	SELECT_HASH_LEN-1, r17, r30	// Hash function
	shl	2, r30, r30			// Shift for array reference
	addu	sel_hash, r30, r30		// Add offset
	ld.l	r16(r30), r30			// sitail (r30) -> tail item
	bte	0, r30, 5f			// Return if zero
	ld.l	si_link(r30), r16		// si (r16) -> head item
	ld.l	si_value(r16), r31		// Check for matching value
	btne	r17, r31, 4f			// Branch if not equal
2:	bri	r1				// Return
	 nop					//

3:	ld.l	si_link(r16), r16		// si (r16) -> next item
	ld.l	si_value(r16), r31		// Check for matching value
	bte	r17, r31, 2b			// Branch if equal
4:	btne	r16, r30, 3b			// Branch back if not tail
5:	bri	r1				// Return
	 mov	r0, r16				//  Zero

////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_lookup_remote(mt, pid)
//
// Arguments:
//	r16 mt:		points to mcmsg_task
//	r17 pid:	key value to select on
//
// Locals:
//	r16 si		select item under consideration
//	r17 value:	key value to select on
//	r30 sitail	points to tail of hash bucket circular list
//	r31 test	item value
//
// Purpose:
//	Looks up a select item in a selector
//
// Returns:
//	select item pointer or zero if not found
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_lookup_remote::
	ld.l	mt_pid_sel(r16), r16		// r16 = mt->pid_sel
	shr	16, r17, r30			// Hash Pid
	or	r0, r30, r0			// CC = 1 if r30 zero
	bc.t	6f				// Search bucket
	 ld.l	sel_zero(r16), r30		//  of mt->pid_sel->zero
	and	SELECT_HASH_LEN-1, r30, r30	// Hash for Selector

1:	shl	2, r30, r30			// Shift for array reference
	addu	sel_hash, r30, r30		// Add offset
	ld.l	r16(r30), r30			// sitail (r30) -> tail item
6:	bte	0, r30, 5f			// Return if zero
	ld.l	si_link(r30), r16		// si (r16) -> head item
	ld.l	si_value(r16), r31		// Check for matching value
	btne	r17, r31, 4f			// Branch if not equal
2:	bri	r1				// Return
	 nop					//

3:	ld.l	si_link(r16), r16		// si (r16) -> next item
	ld.l	si_value(r16), r31		// Check for matching value
	bte	r17, r31, 2b			// Branch if equal
4:	btne	r16, r30, 3b			// Branch back if not tail
5:	bri	r1				// Return
	 mov	r0, r16				//  Zero

////////////////////////////////////////////////////////////////////////
//
//
// Routine:
//	mcmsg_lookup_sequence(source_pid, sequence)
//
// Arguments:
//	r16 source_pid:	selector search value
//	r17 sequence:	secondary search value
//
// Locals:
//	r16 si		select item under consideration
//	r17 sequence:	secondary search value
//	r28 source_pid:	selector search value
//	r29 test	item value
//	r30 sitail	points to tail of hash bucket circular list
//	r31 scratch
//
// Purpose:
//	Looks up a select item in a selector
//
// Returns:
//	select item pointer or zero if not found
//
////////////////////////////////////////////////////////////////////////

	.align	32

_mcmsg_lookup_sequence::
	orh	ha%_mcmsg_seq_sel, r0, r30	// Develop pointer to sel
	btne	0, r16, 1f			// Branch if not zero
	br	2f				// Use zero entry
	 ld.l	l%_mcmsg_seq_sel+sel_zero(r30), r16

1:	and	SELECT_HASH_LEN-1, r16, r31	// Hash function
	shl	2, r31, r31			// Shift for array reference
	addu	r30, r31, r30			// Add offset
	ld.l	l%_mcmsg_seq_sel+sel_hash(r30), r30
	mov	r16, r28			// Move search value to r28
	bte	0, r30, 5f			// Return if zero
	ld.l	si_link(r30), r16		// si (r16) -> head item
	ld.l	si_value(r16), r29		// Check for matching value
	btne	r28, r29, 4f			// Branch if not equal
2:	ld.s	si_nxrq_seq(r16), r29		// Check sequence
	and	0xFFFF, r29, r29		// Mask to short
	btne	r17, r29, 6f			// Branch if not equal
	bri	r1				// Return
	 nop					//
6:	ld.l	si_nxrq_seqlink(r16), r16	// Follow sequence link
	btne	0, r16, 2b			// Branch if not zero
	bri	r1				// Return zero
	 nop					//

3:	ld.l	si_link(r16), r16		// si (r16) -> next item
	ld.l	si_value(r16), r29		// Check for matching value
	bte	r28, r29, 2b			// Branch if equal
4:	btne	r16, r30, 3b			// Branch back if not tail
5:	bri	r1				// Return
	 mov	r0, r16				//  Zero

#endif	HANDCODE
