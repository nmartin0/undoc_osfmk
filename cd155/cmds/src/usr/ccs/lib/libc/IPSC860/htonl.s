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
        .file   "htonl.s"

#include <i860/asm.h>

//
// Convert host-byte-order (little-endian) to network byte order (BIG endian)
//
// inputs:
//	0xddccbbaa - host byte-order
//
// outputs:
//	0xaabbccdd - network byte-order

ENTRY(htonl)

        shr     16,r0,r0        // rotate right 16 bits
        shrd    r16,r16,r16
        shl     8,r16,r17       // shift left 8 bits and
        andnoth 255,r17,r17     // mask with 0xFF00FF00
        andnot  255,r17,r17
        shr     8,r16,r16       // shift original value right 8
        andnoth 65280,r16,r16   // mask with 0x00FF00FF
        andnot  65280,r16,r16
        bri     r1
          or      r16,r17,r16     // combine partial results

