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
 
//
//	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/IPSC860/mcmsglib.s,v 1.16 1994/11/19 02:33:16 mtm Exp $
//

        .file "mcmsglib.s"

_mcmsg_interface::
	or	-130,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

__myphysnode::
	or	-131,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_mp_access::
	or	-132,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_flick::
	or	-133,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_boot_send::
	or	-134,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_console_open::
	or	-135,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_console_close::
	or	-136,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_console_read::
	or	-137,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_console_write::
	or	-138,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_wire::
	or	-140,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_nx_nodeinfo::
	or	-141,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_nxport_setup::
	or	-142,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_nx_local_ast::
	or	-143,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop

_mcmsg_nx_op::
	or	-144,r0,r31
	trap	r31,r31,r0
	bri	r1
	 nop


//
//	Entry points for message processor interface
//

// include kernel/i860paragon/mcmsg/mcmsg_config.h here

POST_NOP	= 1

NX_POST_BASE	= 11
POST_RESPTYPE   = (NX_POST_BASE+4)
POST_PROVIDE    = (NX_POST_BASE+5)
POST_MASKTRAP   = (NX_POST_BASE+6)
POST_NXSEND     = (NX_POST_BASE+7)
POST_NXRECVX    = (NX_POST_BASE+8)
POST_NXSENDCONT = (NX_POST_BASE+9)
POST_NXRECVCONT = (NX_POST_BASE+10)
POST_NXCANCEL   = (NX_POST_BASE+11)
POST_XSEND      = (NX_POST_BASE+12)


_mcmsg_nop::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	POST_NOP, r0, r31

_mcmsg_nx_send::
	orh	ha%_mcmsg_intf_vec8, r0, r30
	ld.l	l%_mcmsg_intf_vec8(r30), r30
	bri	r30
	 or	POST_NXSEND, r0, r31


_mcmsg_nx_recvx::
	orh	ha%_mcmsg_intf_vec8, r0, r30
	ld.l	l%_mcmsg_intf_vec8(r30), r30
	bri	r30
	 or	POST_NXRECVX, r0, r31

_mcmsg_nx_recv_continue::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	POST_NXRECVCONT, r0, r31

_mcmsg_nx_send_continue::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	POST_NXSENDCONT, r0, r31

_mcmsg_nx_reserve_ptype::
	orh	ha%_mcmsg_intf_vec2, r0, r30
	ld.l	l%_mcmsg_intf_vec2(r30), r30
	bri	r30
	 or	 POST_RESPTYPE, r0, r31

_mcmsg_provide::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	 POST_PROVIDE, r0, r31


_mcmsg_xsend::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	POST_XSEND, r0, r31

_mcmsg_nx_cancel::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	POST_NXCANCEL, r0, r31

_mcmsg_masktrap::
	orh	ha%_mcmsg_intf_vec1, r0, r30
	ld.l	l%_mcmsg_intf_vec1(r30), r30
	bri	r30
	 or	POST_MASKTRAP, r0, r31

//
// Post a request
//
// Entry:
//	r16 - r25	Arguments to be posted (preserved through call)
//	r31		Method number
// Post page characteristics:
//	4096 bytes in page
//	64   bytes per entry
//	64   entries
//	1st long is method number
//	Args start at 2nd long
//
// To post a request the method and arguments are written to an entry,
// method last. To make sure an entry is not already occupied it is necessary
// to test the method to make sure it is zero.
// Entries are always used in order.
// To avoid pulling the cache line for a request into local
// cache every time, this code checks them in batches, half the page at a time.
// The check only occurs when the index crosses from one half of the page to
// the other. The wrap-around check is be done here too.

_mcmsg_post1::
_mcmsg_post2::
_mcmsg_post3::

_mcmsg_post4::
_mcmsg_post5::
_mcmsg_post6::
_mcmsg_post7::

_mcmsg_post8::
_mcmsg_post9::
_mcmsg_post10::

	orh	ha%_mcmsg_post_page, r0, r30	//
	ld.l	l%_mcmsg_post_page(r30), r30	// r30 =  pointer to post page
	ixfr	r31, f16			// Move method to FP
	orh	ha%_mcmsg_post_page_in, r0, r29	// r29 = ptr to in index
	lock
	ld.l	l%_mcmsg_post_page_in(r29), r28	// r28 = in index
	and	31, r28, r0			// Check for half page bdry
	bc	2f				// Branch if half page bdry
	addu	1, r28, r31			// Increment index
	unlock
	st.l	r31, l%_mcmsg_post_page_in(r29)	// Store new index
	shl	6, r28, r31			// Create entry pointer
1:	addu	r30, r31, r31			//
	ixfr	r19, f20			// Move arg 4 to FP
	ixfr	r20, f21			// Move arg 5 to FP
	ixfr	r21, f22			// Move arg 6 to FP
	ixfr	r22, f23			// Move arg 7 to FP
	ixfr	r23, f24			// Move arg 8 to FP
	ixfr	r24, f25			// Move arg 9 to FP
	ixfr	r25, f26			// Move arg 10 to FP
	ixfr	r16, f17			// Move arg 1 to FP
	ixfr	r17, f18			// Move arg 2 to FP
	ixfr	r18, f19			// Move arg 3 to FP
	fst.q	f24, 32(r31)			// Post arguments
	fst.q	f20, 16(r31)			//
	fst.q	f16, 0(r31)			// Post request
	bri	r1				// Return
	 mov r0,r16

2:	unlock
	st.l	r28, l%_mcmsg_post_page_in(r29)	// Unlock index, value unchanged
	and	63, r28, r28			// Fix index for end of page
//
//	At this point, r28 is either	 0 (was 64) or	32.
//	We want to spin on index	31 or		63, respectively.
//	That will ensure that entries 0-31 or        32-63 are open,
//	so we don't have to check them individually.
//
	shl	6, r28, r31			//
	addu	r30, r31, r31			// r31 = entry pointer
3:	ld.l	1984(r31), r27			// r27 = method
	or	r27, r0, r0			// Test for zero
	bnc	3b				// Loop until method is zero
//  We need to grab index again, since another thread may have passed
//  us by in the loop above, and already incremented the index.
	lock
	ld.l	l%_mcmsg_post_page_in(r29), r28	// r28 = in index
	and	63, r28, r28			// Fix index for end of page
	addu	1, r28, r31			// Increment index
	unlock
	st.l	r31, l%_mcmsg_post_page_in(r29)	// Store new index and unlock
	br	1b				// Loop back
	 shl	6, r28, r31			// Create entry pointer
//
// Mark end of post page code
_mcmsg_post_end::

//
// Make a request through a kernel system call
//
// Entry:
//	r16 - r25	Arguments
//	r31		Method number
// Exit to system call:
//	r16		Method number
//	r17 - r26	Arguments

_mcmsg_call::
	or	r25, r0, r26			// Move arguments up
	or	r24, r0, r25
	or	r23, r0, r24
	or	r22, r0, r23
	or	r21, r0, r22
	or	r20, r0, r21
	or	r19, r0, r20
	or	r18, r0, r19
	or	r17, r0, r18
	or	r16, r0, r17
	br	_mcmsg_interface		// Make call
	 or	r31, r0, r16			// Move method down
