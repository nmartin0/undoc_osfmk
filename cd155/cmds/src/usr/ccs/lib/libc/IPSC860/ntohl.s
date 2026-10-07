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
 */
        .file "ntohl"

#include <i860/asm.h>

// ntohl( foo )
//
// inputs:
//      0xaabbccdd - network byte-order
//	  1 2 3 4
//
// outputs:
//      0xddccbbaa - host byte-order
//	  4 3 2 1


ENTRY(ntohl)
	mov	r16,r17
	mov	r0,r16

	and	0x00FF,r17,r18		// isolate byte 4
	shl	24,r18,r18
	or	r18,r16,r16

	and	0xFF00,r17,r18		// isolate byte 3
	shl	 8,r18,r18		// position for OR
	or	r18,r16,r16		// r16 == 4 3 0 0 

	andh	0x00FF,r17,r18		// isolate byte 2
	shr      8,r18,r18		// position for OR
	or	r18,r16,r16		// r16 == 4 3 2

	andh	0xFF00,r17,r18		// isolate byte 1
	shr	24,r18,r18		// position to LSB

	bri	r1
	  or	r18,r16,r16		// r16 == 4 3 2 1

