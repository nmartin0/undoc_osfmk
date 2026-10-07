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
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * vp_call_deliver( function_arg, function_addr )
 *			r16		r17
 * issues: [1-30-92] stan
 *	where do we return to?? is it r1 on entry? From code reading this
 *	routine is used to kill pthreads only hence it never returns back
 *	to here.
 *
 * returns 0
 *
 */

	.text
	.align	4

_vp_call_deliver::

	addu	-16,sp,sp	// allocate stack frame for return adrs
	st.l	r1,0(sp)	// save return-adrs
	calli	r17		// call the REAL function
	  nop			// r16 has function arg
	ld.l	0(sp),r1	// restore return-adrs
	addu	16,sp,sp
	bri	r1
	  mov	r0,r16		// return 0
