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
 
//      Copyright (c) 1989, Intel Corporation.
//      All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//
        .file   "strcmp.s"

#include        <i860/asm.h>
        .text

ENTRY(strcmp)
        bte     r16,r17,.equal  // Same string? - must be equal

        or      r16,r17,r18     // If addresses not divisible by 4,
        and     3,r18,r0        //   use the byte loop
        bnc     .byteloop
        orh     0x0101,r0,r18   // Build up two masks
        or      0x0101,r18,r18  // 0x01010101
        shl     7,r18,r19       // 0x80808080

.loop:
        ld.l    0(r16),r20      // This loop is unrolled 4 times
        ld.l    0(r17),r21
        subs    r20,r18,r22     // Compare two words at a time
        btne    r20,r21,.notequal       // Go if words are different
        andnot  r20,r19,r23     // These lines find 0 bytes
        and     r23,r22,r22
        bnc     .equal          // A 0 byte was found, done

        ld.l    4(r16),r20
        ld.l    4(r17),r21
        subs    r20,r18,r22
        btne    r20,r21,.notequal
        andnot  r20,r19,r23
        and     r23,r22,r22
        bnc     .equal

        ld.l    8(r16),r20
        ld.l    8(r17),r21
        subs    r20,r18,r22
        btne    r20,r21,.notequal
        andnot  r20,r19,r23
        and     r23,r22,r22
        bnc     .equal

        ld.l    12(r16),r20
        ld.l    12(r17),r21
        subs    r20,r18,r22
        btne    r20,r21,.notequal
        andnot  r20,r19,r23
        and     r23,r22,r22
        bnc     .equal

        addu    16,r16,r16
        br      .loop
        addu    16,r17,r17

.equal:                         // Done, strings match
        bri     r1
          mov   r0,r16          // Return zero (equal)

// Compare the bytes individually (once a difference in words are found)
.notequal:
        shl     24,r20,r22      // Check low order bytes first
        shl     24,r21,r23
        btne    r22,r23,.noteq
        bte     0,r22,.equal    // Check for a 0 byte
        shl     16,r20,r22      // The low order bytes are equal (we
        shl     16,r21,r23      // just checked that) so don't need
        btne    r22,r23,.noteq  // to mask them off.
        andh    0xff00,r22,r0   // Check for a 0 byte
        bc      .equal
        shl     8,r20,r22
        shl     8,r21,r23
        btne    r22,r23,.noteq
        andh    0xff00,r22,r0   // Check for a 0 byte
        bc      .equal
// The high bytes (r20, r21) must be different
        shr    24,r20,r22      // Sign extend
        shr    24,r21,r23
        bri     r1
          subu  r22,r23,r16

.noteq:
        shr    24,r22,r22      // Sign extend
        shr    24,r23,r23
        bri     r1
          subu  r22,r23,r16

// The byte at a time loop.  This is also unrolled 4 times
.byteloop:
        ld.b    0(r16),r20
        ld.b    0(r17),r21
        ld.b    1(r16),r22      // Pre-load the next byte
        btne    r20,r21,.bytenotequal
        bte     r0,r20,.equal   // End of string, and match

        ld.b    1(r17),r23
        ld.b    2(r16),r20      // Pre-load the next byte
        btne    r22,r23,.bytenoteq
        bte     r0,r22,.equal   // End of string, and match

        ld.b    2(r17),r21
        ld.b    3(r16),r22      // Pre-load the next byte
        btne    r20,r21,.bytenotequal
        bte     r0,r20,.equal   // End of string, and match

        ld.b    3(r17),r23
        btne    r22,r23,.bytenoteq
        addu    4,r16,r16
        addu    4,r17,r17
        btne    r0,r22,.byteloop        // End of string, and match
.bytenotequal:
        shl     24,r20,r20
        shl     24,r21,r21
        shr    24,r20,r20
        shr    24,r21,r21
        bri     r1
          subu  r20,r21,r16
.bytenoteq:
        shl     24,r22,r22
        shl     24,r23,r23
        shr    24,r22,r22
        shr    24,r23,r23
        bri     r1
          subu  r22,r23,r16

