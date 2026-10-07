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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * $Log: lock.s,v $
 * Revision 1.6  1994/11/18  21:07:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  23:06:43  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/05/17  22:47:02  andyp
 * Cleaned up context switch code.  Killed a dormant bug at the bottom
 * of cproc_prepare() -- there was no instruction in the delay slot of the
 * bri -- it had been commented out.  Also, don't save/restore f8-f15.
 * Reorded two instructions in lock.s to have one less instruction under
 * the bus lock.
 *
 * Revision 1.3  1993/04/27  21:00:57  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:07:54  dleslie
 * First R1_0 release
 *
 * Revision 2.2  91/09/04  11:29:19  jsb
 * 	First checkin. Contributed by Intel/SSD.
 * 	[91/09/02  08:56:39  jsb]
 * 
 */
/*
 *	Mutex implementation for i860.
 */

	.text
/*
 *	int
 *	spin_try_lock(m)
 *		mutex_t m;	(= int *m for our purposes)
 */
	.align	4
_spin_try_lock::
	mov	1,r18
	lock
	ld.l	0(r16),r17
	unlock
	st.l	r18,0(r16)
	bri	r1
	 xor	1,r17,r16


/*
 *	void
 *	spin_unlock(m)
 *		mutex_t m;	(= int *m for our purposes)
 */
_spin_unlock::
	bri	r1
	 st.l	r0,0(r16)

