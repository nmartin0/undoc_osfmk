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
//		INTEL CORPORATION PROPRIETARY INFORMATION
//
//	This software is supplied under the terms of a license
//	agreement or nondisclosure agreement with Intel Corporation
//	and may not be copied or disclosed except in accordance
//	with the terms of that agreement.
//
//	i860rt.s 7.2 %D 09:14:22
//
//
//                       Integer division routines
//
//Converts ints to doubles does a double divide, and then converts the result
//back to an int.  Note: things can be sped up when dividing by a constant
//by converting the const to floating and taking the reciprical in the compiler.
//I do this.

	.file "i860rt.s"

	     .atmp	r31


	     .data
	     .align  8

two52two31: .long   0x80000000  //I don't trust floating conversion
	    .long   0x43300000
two:        .long   0x0
	    .long   0x40000000
onepluseps: .long   0x1000
	    .long   0x3ff00000
two52:      .long   0x0
	    .long   0x43300000

	    .text


// Signed integer divide, r16=r16/r17
// uses f16-f25
.div::
_DIVI::                             // Should be obsolete
_PDIVI::                            // I should implement this someday
    fst.q       f16,-16(sp)++
    fst.q       f20,-16(sp)++
    fst.q       f24,-16(sp)++       // save two extra registers to align the sp

// Convert denominator (r17) and numerator (r16) into doubles (f18,f16)
    fld.d       two52two31,f20
    xorh        0x8000,r17,r17
    ixfr        r17,f18
    fmov.ss     f21,f19             // Make f18.f19 a valid number by loading
				    // the correct exponant
    xorh        0x8000,r16,r16      // Start the other argument
     fsub.dd     f18,f20,f18        // Now f18.f19 is correct (put here to fill gap)
    ixfr        r16,f16
    fmov.ss     f21,f17
//   fsub.dd     f16,f20,f16         // moved down a little further

// Now do the divide
    fld.d       two,f24
     fsub.dd     f16,f20,f16        // Final part of the conversion
    frcp.dd     f18,f20             // Make a guess at the reciprical of denom
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // second guess is off by 2^-15
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // third guess is off by 2^-29
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f16,f20         // guess*numerator
    fmul.dd     f22,f20,f22         // fixup by error term

// Round result a little, then convert to integer
    fld.d       onepluseps,f24      // 1+2^-40
    fmul.dd     f22,f24,f22         // force quotient to be bigger than integer
   fld.q       0(sp),f24
    ftrunc.dd   f22,f22             // convert to integer
   fld.q       32(sp),f16
    fxfr        f22,r16             // move to an integer reg

    fld.q       16(sp),f20
   bri         r1
    adds        48,sp,sp





// Unsigned integer divide, r16=r16/r17
// uses f16-f25
//   (same as above, except the conversion is easier)
.udiv::
_UDIVI::			    // Should be obsolete
    bte         1,r17,.L1           // avoid potential explosions
    fst.q       f16,-16(sp)++
    fst.q       f20,-16(sp)++
    fst.q       f24,-16(sp)++       // save two extra registers to align the sp

// Convert denominator (r17) and numerator (r16) into doubles (f18,f16)
    fld.d       two52,f20
    ixfr        r17,f18
    ixfr        r16,f16
    fmov.ss     f21,f19             // Make f18.f19 a valid number by loading
				    // the correct exponant
    fmov.ss     f21,f17
    fsub.dd     f18,f20,f18         // Now f18.f19 is correct
    fsub.dd     f16,f20,f16         // moved down a little further

// Now do the divide
    fld.d       two,f24
    frcp.dd     f18,f20             // Make a guess at the reciprical of denom
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // second guess is off by 2^-15
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // third guess is off by 2^-29
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f16,f20         // guess*numerator
    fmul.dd     f22,f20,f22         // fixup by error term

// Round result a little, then convert to integer
    fld.d       onepluseps,f24      // 1+2^-40
    fmul.dd     f22,f24,f22         // force quotient to be bigger than integer
   fld.q       0(sp),f24
    ftrunc.dd   f22,f22             // convert to integer
   fld.q       32(sp),f16
    fxfr        f22,r16             // move to an integer reg

    fld.q       16(sp),f20
   bri         r1
    adds        48,sp,sp

// The ftrunc instruction explodes on any unsigned number >= 0x80000000.
// This could happen on a divide if the numerator >= 0x80000000 and the
// denominator=1.  Since division by 1 is easy, I make a special check for
// it at the start and avoid the problem
.L1:
    bri         r1
    nop

// Signed integer remainder, r16=r16%r17
// uses f16-f25
.rem::
_MODI::				    // Should be obsolete
_PMODI::                            // I should implement this someday
    fst.q       f16,-16(sp)++
    fst.q       f20,-16(sp)++
    fst.q       f24,-16(sp)++       // save two extra registers to align the sp

// Convert denominator (r17) and numerator (r16) into doubles (f18,f16)
    fld.d       two52two31,f20
    xorh        0x8000,r17,r17
    ixfr        r17,f18
    fmov.ss     f21,f19             // Make f18.f19 a valid number by loading
				    // the correct exponant
    xorh        0x8000,r16,r16      // Start the other argument
     fsub.dd     f18,f20,f18        // Now f18.f19 is correct (put here to fill gap)
    ixfr        r16,f16
    fmov.ss     f21,f17
//   fsub.dd     f16,f20,f16         // moved down a little further

// Now do the divide
    fld.d       two,f24
     fsub.dd     f16,f20,f16        // Final part of the conversion
    frcp.dd     f18,f20             // Make a guess at the reciprical of denom
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // second guess is off by 2^-15
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // third guess is off by 2^-29
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f16,f20         // guess*numerator
    fmul.dd     f22,f20,f22         // fixup by error term

// Round result a little, then compute remainder
    xorh        0x8000,r17,r17      // Get the right value of the denominator
    xorh        0x8000,r16,r16      //  and the numerator
//It might be possible to remove the above to instructions, test this.
    ixfr        r17,f16
    fld.d       onepluseps,f24      // 1+2^-40
    fmul.dd     f22,f24,f22         // force quotient to be bigger than integer

    ftrunc.dd   f22,f22             // convert to integer
   fld.q       0(sp),f24
    fmlow.dd    f16,f22,f22         // integer mult, quotient*denominator
   fld.q       32(sp),f16
    fxfr        f22,r17             // move to an integer reg
   fld.q       16(sp),f20
    subs        r16,r17,r16         // rem=numerator-quotient*denominator

   bri         r1
    adds        48,sp,sp




// Unsigned integer remainder, r16=r16%r17
// uses f16-f25
//   (same as above, except the conversion is easier)
.urem::
_UMODI::			    // Should be obsolete
    bte         1,r17,.L2
    fst.q       f16,-16(sp)++
    fst.q       f20,-16(sp)++
    fst.q       f24,-16(sp)++       // save two extra registers to align the sp

// Convert denominator (r17) and numerator (r16) into doubles (f18,f16)
    fld.d       two52,f20
    ixfr        r17,f18
    ixfr        r16,f16
    fmov.ss     f21,f19             // Make f18.f19 a valid number by loading
				    // the correct exponant
    fmov.ss     f21,f17
    fsub.dd     f18,f20,f18         // Now f18.f19 is correct
    fsub.dd     f16,f20,f16         // moved down a little further

// Now do the divide
    fld.d       two,f24
    frcp.dd     f18,f20             // Make a guess at the reciprical of denom
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // second guess is off by 2^-15
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f22,f20         // third guess is off by 2^-29
    fmul.dd     f18,f20,f22         // guess*divisor
    fsub.dd     f24,f22,f22         // 2-guess*divisor
    fmul.dd     f20,f16,f20         // guess*numerator
    fmul.dd     f22,f20,f22         // fixup by error term

// Round result a little, then convert to integer
    ixfr        r17,f16
    fld.d       onepluseps,f24      // 1+2^-40
    fmul.dd     f22,f24,f22         // force quotient to be bigger than integer

    ftrunc.dd   f22,f22             // convert to integer
    fmlow.dd    f16,f22,f22         // integer mult, quotient*denominator

    fld.q       0(sp),f24
    fld.q       32(sp),f16
   fxfr         f22,r17             // move to an integer reg
    fld.q       16(sp),f20
   subu         r16,r17,r16         // rem=numerator-quotient*denominator
   bri         r1
    adds        48,sp,sp

// The ftrunc instruction explodes on any unsigned number >= 0x80000000.
// This could happen on a modulus if the numerator >= 0x80000000 and the
// denominator=1.  Since modulus by 1 is easy, I make a special check for
// it at the start and avoid the problem
.L2:
   bri         r1
    mov         r0,r16




// Set a range in a pascal set
// r16 - address of the set
// r17 - lowerbound of range
// r18 - upperbound of range
_SETPAIR::
    adds    -16,sp,sp
    st.l    r19,12(sp)
    st.l    r20,8(sp)

    and     31,r17,r19          //get the bit offset
    or      1,r0,r20
    shl     r19,r20,r19         //get the bit itself
    andnot  31,r17,r20
    shr     3,r20,r20           //get the byte offset
    adds    r20,r16,r16         //and increment the address by it

    subs    r17,r17,r0
    ld.l    0(r16),r20          //Fill up the gap after sub
    bc      L2
L1:
     or     r19,r20,r20         //set this bit
     shl    1,r19,r19           // move to next bit
     adds   1,r17,r17           // (increment the loop counter)
     bte    r17,r18,L2          // Are we done?
     btne   r0,r19,L1           //Did we overflow the word?
     st.l   r20,0(r16)          //Store the cached value
     adds   4,r16,r16           //next address
     or     1,r0,r19            // first bit in this address
    br      L1
     ld.l   0(r16),r20          //Get a new cached value

L2:
    st.l    r20,0(r16)          //Store the cached value
    st.l    r19,12(sp)
    st.l    r20,8(sp)
   bri      r1
    adds    16,sp,sp






// Register save and restore routines
// update sp by number of registers
// save registers r4-?(r15, max)
rsave07::
    adds        -28,sp,sp
    st.l        r4,0(sp)
    st.l        r5,4(sp)
    st.l        r6,8(sp)
    st.l        r7,12(sp)
    st.l        r8,16(sp)
    st.l        r9,20(sp)
   bri         r1
    st.l        r10,24(sp)

rrest07::
    ld.l        0(sp),r4
    ld.l        4(sp),r5
    ld.l        8(sp),r6
    ld.l        12(sp),r7
    ld.l        16(sp),r8
    ld.l        20(sp),r9
    ld.l        24(sp),r10
   bri         r1
    adds        28,sp,sp

rsave08::
    adds        -32,sp,sp
    st.l        r4,0(sp)
    st.l        r5,4(sp)
    st.l        r6,8(sp)
    st.l        r7,12(sp)
    st.l        r8,16(sp)
    st.l        r9,20(sp)
    st.l        r10,24(sp)
   bri         r1
    st.l        r11,28(sp)

rrest08::
    ld.l        0(sp),r4
    ld.l        4(sp),r5
    ld.l        8(sp),r6
    ld.l        12(sp),r7
    ld.l        16(sp),r8
    ld.l        20(sp),r9
    ld.l        24(sp),r10
    ld.l        28(sp),r11
   bri         r1
    adds        32,sp,sp

rsave09::
    adds        -36,sp,sp
    st.l        r4,0(sp)
    st.l        r5,4(sp)
    st.l        r6,8(sp)
    st.l        r7,12(sp)
    st.l        r8,16(sp)
    st.l        r9,20(sp)
    st.l        r10,24(sp)
    st.l        r11,28(sp)
   bri         r1
    st.l        r12,32(sp)

rrest09::
    ld.l        0(sp),r4
    ld.l        4(sp),r5
    ld.l        8(sp),r6
    ld.l        12(sp),r7
    ld.l        16(sp),r8
    ld.l        20(sp),r9
    ld.l        24(sp),r10
    ld.l        28(sp),r11
    ld.l        32(sp),r12
   bri         r1
    adds        36,sp,sp

rsave10::
    adds        -40,sp,sp
    st.l        r4,0(sp)
    st.l        r5,4(sp)
    st.l        r6,8(sp)
    st.l        r7,12(sp)
    st.l        r8,16(sp)
    st.l        r9,20(sp)
    st.l        r10,24(sp)
    st.l        r11,28(sp)
    st.l        r12,32(sp)
   bri         r1
    st.l        r13,36(sp)

rrest10::
    ld.l        0(sp),r4
    ld.l        4(sp),r5
    ld.l        8(sp),r6
    ld.l        12(sp),r7
    ld.l        16(sp),r8
    ld.l        20(sp),r9
    ld.l        24(sp),r10
    ld.l        28(sp),r11
    ld.l        32(sp),r12
    ld.l        36(sp),r13
   bri         r1
    adds        40,sp,sp

rsave11::
    adds        -44,sp,sp
    st.l        r4,0(sp)
    st.l        r5,4(sp)
    st.l        r6,8(sp)
    st.l        r7,12(sp)
    st.l        r8,16(sp)
    st.l        r9,20(sp)
    st.l        r10,24(sp)
    st.l        r11,28(sp)
    st.l        r12,32(sp)
    st.l        r13,36(sp)
   bri         r1
    st.l        r14,40(sp)

rrest11::
    ld.l        0(sp),r4
    ld.l        4(sp),r5
    ld.l        8(sp),r6
    ld.l        12(sp),r7
    ld.l        16(sp),r8
    ld.l        20(sp),r9
    ld.l        24(sp),r10
    ld.l        28(sp),r11
    ld.l        32(sp),r12
    ld.l        36(sp),r13
    ld.l        40(sp),r14
   bri         r1
    adds        44,sp,sp

rsave12::
    adds        -48,sp,sp
    st.l        r4,0(sp)
    st.l        r5,4(sp)
    st.l        r6,8(sp)
    st.l        r7,12(sp)
    st.l        r8,16(sp)
    st.l        r9,20(sp)
    st.l        r10,24(sp)
    st.l        r11,28(sp)
    st.l        r12,32(sp)
    st.l        r13,36(sp)
    st.l        r14,40(sp)
   bri         r1
    st.l        r15,44(sp)

rrest12::
    ld.l        0(sp),r4
    ld.l        4(sp),r5
    ld.l        8(sp),r6
    ld.l        12(sp),r7
    ld.l        16(sp),r8
    ld.l        20(sp),r9
    ld.l        24(sp),r10
    ld.l        28(sp),r11
    ld.l        32(sp),r12
    ld.l        36(sp),r13
    ld.l        40(sp),r14
    ld.l        44(sp),r15
   bri         r1
    adds        48,sp,sp
// i860rt.s 7.2 90/05/30 09:14:22
