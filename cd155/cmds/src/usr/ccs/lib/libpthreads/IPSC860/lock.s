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
// Mach Operating System
// Copyright (c) 1989 Carnegie-Mellon University
// All rights reserved.  The CMU software License Agreement specifies
// the terms and conditions for use and redistribution.
//
//
// HISTORY
//
//
// i860/lock.s
//
// Mutex implementation for i860.  Note that lock variables must be
// stored in non-cacheable memory to lock the bus.
//

	.text
//
//	int
//	lock_try_set(m)
//		mutex_t m;	(= int *m for our purposes)
//
//	returns:
//		0 == previous state of the lock was 'locked'.
//		1 == previous state of the lock was 'unlocked'.
//
//
	.align	4
_lock_try_set::
	lock
	ld.l	0(r16),r17
	mov	1,r18
	unlock
	st.l	r18,0(r16)	// must be AFTER unlock inst (wlb)
	bri	r1
	  xor	1,r17,r16	// invert previous lock value for ret code.


//
//	void
//	lock_unset(m)
//		mutex_t m;	(= int *m for our purposes)
//
_lock_unset::
	lock
	nop
	unlock
	bri	r1
	  st.l	r0,0(r16)	// actually unlocks the bus

