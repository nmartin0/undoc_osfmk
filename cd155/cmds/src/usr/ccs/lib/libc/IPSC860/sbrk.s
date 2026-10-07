/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1988, 1989, 1990, 1991, 1992, by Intel Corporation,
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
 *
 * $Id: sbrk.s,v 1.4 1994/11/19 02:13:31 mtm Exp $
 *
 * HISTORY:
 *
 * $Log: sbrk.s,v $
 * Revision 1.4  1994/11/19  02:13:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/08/04  23:19:15  stans
 *    Cleanup CVS log messages, set 'cvs admin -c" * "' CVS log message leader.
 *
 * Revision 1.2  1993/04/28  22:07:25  shala
 * Support Stack frame.
 * 
 * Revision 1.1  1992/04/27  14:00:45  stans
 * "OSF/1.0.4"
 */
        .file "sbrk.s"

#include "SYS.h"

	.globl	_end

//
// _curbrk & _minbrk are utilized by libmach.a(sbrk.o) rtn
//
	.data
	.align	4
_minbrk::	.long _end
_curbrk::	.long _end

	.text

#define LD_BUG_FIXED 0

FENTRY(sbrk)

#if LD_BUG_FIXED
	orh	ha%_curbrk,r0,r31
	ld.l	l%_curbrk(r31),r18	// relies on r18 preservation
#else
	mov	_curbrk,r31
	ld.l	0(r31),r18		// relies on r18 preservation
#endif
	bte	r16,r0,1f		// sbrk(0) returns curbrk.
	adds	r16,r18,r16
	or	SYS_obreak,r0,r31	// sbrk() implemented via brk()
	mov	r16,r18			// save new curbrk
	trap	r31,r31,r0
	bc	_cerror
#if LD_BUG_FIXED
	orh	ha%_curbrk,r0,r31
	ld.l	l%_curbrk(r31),r16	// return old curbrk
	br	.sb_exit
	 st.l	r18,l%_curbrk(r31)	// update curbrk
#else
	mov	_curbrk,r31
	ld.l	0(r31),r16		// return old curbrk
	br	.sb_exit
	 st.l	r18,0(r31)		// update curbrk
#endif

1:
	or	r18,r0,r16		// return curbrk.
.sb_exit:
	FRET
