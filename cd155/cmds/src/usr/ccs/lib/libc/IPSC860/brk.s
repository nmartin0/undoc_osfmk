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
 */

        .file   "brk.s"

#include "SYS.h"

/* in syscall.h as SYS_obreak */
#define	SYS_brk		17

	.globl	_curbrk
	.globl	_minbrk

FENTRY(_brk)
	br	.ok
	nop

FENTRY(brk)
	mov	_minbrk,r31	// entry: brk level in r16
	ld.l	0(r31),r30
	subu	r16,r30,r0	// see if r16 >= _minbrk
	bc	.ok		// if so call 
	  nop
	mov	r30,r16		// otherwise r16 <- _minbrk

.ok:
	mov	r16,r18		// save new brk, relies on r18 preservation
	CALL(brk)
	/*
	 * return here ONLY on success, cerror otherwise ala CALL macro
	 */
	btne	r16,r0,.brk_done // if syscall OK, then store new break.
	mov	_curbrk,r30	// set address in r30
	st.l	r18,0(r30)	// set new break value.
.brk_done:
	FRET

