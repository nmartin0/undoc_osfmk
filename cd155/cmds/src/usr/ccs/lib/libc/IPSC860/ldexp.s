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
 
//      Copyright (c) 1989, 1990, Intel Corporation.
//      All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//

//      ldexp takes only two arguments: val, a double passed in f9:f8,
//      and exp, a 32-bit integer passed in r16.
//      if val is +/-0.0, ldexp returns val.
//      if val is infinity or NaN, ldexp returns val;
//      else, val is neither infinity nor NaN, so ldexp returns a double
//      which is the same as val except for its biased exponent, which
//      has the eleven least significant bits of the value obtained by
//      performing signed addition of val's biased exponent to the
//      integer exp.
//

        .file   "ldexp.s"

#ifndef  _THREAD_SAFE

        .text

        .globl _ldexp
_ldexp:
ldexp:

// Code added to return proper errno value 

        pfeq.dd  f8,f8,f0
        bc .continue2

// X is a NaN

        or      33,r0,r16               // r16 = 33 = 0x21
        orh     ha%_errno,r0,r31
        bri     r1
        st.l    r16,l%_errno(r31)       // _errno = EDOM; delayed

.continue2:

// End of code change 

        fxfr    f9,r17          // r17 is high order of input f_val
        fxfr    f8,r18          // r18 is low order of input f_val
        subs    2098,r16,r0             // if r16>=2098, CC set
        bc      .overflow               //  then goto .overflow
        subs    -2098,r16,r0            // if r16<=-2098, CC clear
        bnc     .exp_add_under          //  then goto .exp_add_under
.L1:
        andh    0x7ff0,r17,r20
        bc      .denorm_or_0            // val is either denormal or zero
        shr     20,r20,r20              // extract old exponent
        xor     0x07ff,r20,r0
        bc      .special_exit           // val is either infinite or NaN
.add_int_to_bias_exp:
        adds    r16,r20,r20             // r20=exponent value
        subs    r0,r20,r0               // if r20<=0 then underflow
        bnc     .underflow
        or      0x07ff,r0,r31
        subs    r20,r31,r0              // if r20>=0x7ff then overflow
        bnc     .overflow
.insert_bias_exp:
        shl     20,r20,r20
        andnoth 0x8000,r20,r20  // clear sign bit
        andnoth 0x7ff0,r17,r17  // clear old exponent
        or      r20,r17,r17             // insert new exponent
.special_exit:
        bri     r1
        ixfr    r17,f9

.denorm_or_0:                           // exponent is known to be zero
        andnoth 0x8000,r17,r21  // make val positive
        bnc     .big_denorm             // if high-part of fraction is not zero
        xor     r18,r0,r0               // check low fraction bits
        bc      .special_exit           // fraction is zero, a true zero
.lil_denorm:                            // 20 most signif. frac bits are 0
        adds    -19,r0,r20              // bias exponent after 20-bit lshift
.lil_lf_shft:                           // loop to normalize significant:
        adds    -1,r20,r20              //      decrement biased exponent
        andh    0x8000,r18,r0           //      is most signif. bit set?
        shl     1,r18,r18               //      shift, making MSB implicit
        bc      .lil_lf_shft            //      loop if implicit bit 0
.lil_rt_shft:                           // align normalized significant:
        shr     12,r18,r21              // put high frac in proper bield
        shl     20,r18,r18              // make low frac of result
        or      r17,r21,r17             // put sign and hi frac in r17
        br      .add_int_to_bias_exp    // done, bit for biased_exp field
        ixfr    r18,f8          // set-up low frac of result

.big_denorm:                            // fraction is more than 32-bits
        or      1,r0,r20                // pre-set the biased exponent
        shl     12,r21,r21              // left justify high-part of fraction
.lf_shft:                               // loop to normalize significant:
        adds    -1,r20,r20              //      decrement biased exponent
        andh    0x8000,r21,r0           //      is most signif. bit set?
        shl     1,r21,r21               //      shift, making MSB implicit
        bc      .lf_shft                //      loop if implicit bit 0
.rt_shft:                               // align normalized significant:
        shr     12,r21,r21              // return high frac to proper field
        adds    31,r20,r22              // get count of low frac bits needed
        shr     r22,r18,r22             // rt justify low frac bits to be hi
        or      r21,r22,r21             // put low frac bits in hi frac
        andh    0x8000,r17,r22  // get sign of val
        or      r21,r22,r17             // put sign & hi frac in r17
        subs    1,r20,r22               // get lshift count for low frac
        shl     r22,r18,r18             // align low frac properly
        br      .add_int_to_bias_exp    // done, but for biased_exp field
        ixfr    r18,f8          // set-up low frac of result
//-------------- case ( underflow ) ----------------
.underflow:
        shr     31,r17,r27              // r27 = value of sign bit
        shl     31,r27,r29              // r29 = sign bit pattern
        ld.c    fsr,r25         // load fsr into r25
        shl     28,r25,r25
        shr     30,r25,r25              // r25 = the value of rounding mode
        subs    -53,r20,r0              // if r20>-53
        bc      .make_denorm            //  then goto .make_denorm
        bte     1,r25,.udf_01           // else return min_val or zero
        bte     2,r25,.udf_10           // in these cases,
        bte     3,r25,.udf_11           // true underflow
//br    .min_val                // Round to nearest or even
// sas change so that zero is returned
        br      .d_zero         // Round to nearest or even
        nop                             //  return +-MIN
.udf_01:                                // Round down
        bte     1,r27,.min_val          //  if(sign==1) then return -MIN
        br      .d_zero                 //  else return zero
        nop
.udf_10:                                // Round up
        bte     1,r27,.d_zero           //  if(sign==1) then return zero
        br      .min_val                //  else return +MIN
        nop
.udf_11:                                // Chop
        br      .d_zero                 //  return +-zero
        nop

.min_val:
        or      0x0001,r0,r18           // r18 = 0x00000001 : low order ret val.
        ixfr    r18,f8          // set low order of return val
        bri     r1                      // r29 = 0x80000000 or 0x00000000
        ixfr    r29,f9          // set high order of return val

.d_zero:

// sas added so errno is set
        or      34,r0,r16               // r16 = 33 = 0x21
        orh     ha%_errno,r0,r31
        st.l    r16,l%_errno(r31)       // _errno = EDOM; delayed

        ixfr    r0,f8                   // set low order to 0x00000000
        bri     r1
        ixfr    r29,f9          // make signed zero into f8-f9

.make_denorm:                           // r17:r18 = 53bit significant
        shr     31,r17,r31
        shl     31,r31,r31              // r31 = sign bit
        andnoth 0xfff0,r17,r17  // r17 = high order of significant
        orh     0x0010,r17,r17  // set default msb (53bit)
        mov     r0,r19          // r17:r18:r19:r20 = 53+64bit v_signific
        or      0x0001,r0,r24           // r24 = 0x00000001 for lsb mask
        shl     31,r17,r21              // r21.31b = LSB of r17
        shr     1,r17,r17               // r17 = 1>>r17
        shl     31,r18,r22              // r22.31b = LSB of r18
        shr     1,r18,r18               // r18 = 1>>r18
        or      r18,r21,r18             // set r18.31b
        or      r19,r22,r19             // set r19.31b
        mov     r20,r26         // r26 = counter
        mov     r0,r20          // clear r20
.m_loop:                                //
        bte     r0,r26,.m_lp_end        // if (counter=0) then end loop
        shl     31,r17,r21              // r21.31b = LSB of r17
        shr     1,r17,r17               // r17 = 1>>r17
        shl     31,r18,r22              // r22.31b = LSB of r18
        shr     1,r18,r18               // r18 = 1>>r18
        or      r18,r21,r18             // set r18.31b
        shl     31,r19,r21              // r21.31b = LSB of r19
        shr     1,r19,r19               // r19 = 1>>r19
        or      r19,r22,r19             // set r19.31b
        shr     1,r20,r20               // r20 = 1>>r20
        or      r20,r21,r20             // set r20.31b
.m_d_2:
        br      .m_loop                 // loop to .m_loop
        adds    1,r26,r26               // r26 = r26 + 1 : delayed

.m_lp_end:
        bte     1,r25,.deno_01
        bte     2,r25,.deno_10
        bte     3,r25,.deno_11
//                                      // Round to nearest or even
        shr     31,r19,r21              // r21.01b = MSB of r19
        or      r0,r21,r0               // if r21==0 (means r19.31b == 0)
        bc      .deno_end               //  goto .deno_end
        shl     1,r19,r21               // r21 = 1<<r19
        or      r0,r21,r0               // if r21 != 0 (means not even case)
        bnc     .m_pls_one              //  goto .m_pls_one
        or      r0,r20,r0               // else if r20 != 0 (means not even)
        bnc     .m_pls_one              //  goto .m_pls_one
//                                      // else even case
        shl     31,r18,r21
        shr     31,r21,r21              // r21 = LSB of r18
        or      r0,r21,r0               // if r21 == 0
        bc      .deno_end               //  goto .deno_end
        br      .m_pls_one              // else goto .m_pls_one
        nop

.deno_01:                               // Round down
        or      r0,r19,r0               // if r19 != 0
        bnc     .deno_01_1              //  goto .deno_01_1
        or      r0,r20,r0               // if r20 != 0
        bnc     .deno_01_1              //  goto .deno_01_1
        br      .deno_end               // else
        nop                             //  goto .deno_end
.deno_01_1:
        bte     1,r27,.m_pls_one        //  if(sign) then (r17:r18)+=r28
        br      .deno_end               //  else return
        nop

.deno_10:                               // Round up
        or      r0,r19,r0               // if r19 != 0
        bnc     .deno_10_1              //  goto .deno_10_1
        or      r0,r20,r0               // if r20 != 0
        bnc     .deno_10_1              //  goto .deno_10_1
        br      .deno_end               // else
        nop                             //  goto .deno_end
.deno_10_1:
        bte     1,r27,.deno_end // if(sign) then return
        br      .m_pls_one              // else (r17:r18)+=r28
        nop

.deno_11:                               // Chop
        br      .deno_end               //  return
        nop

.m_pls_one:
        addu    1,r18,r18               // r18 = r18 + 1
        bnc     .deno_end               // if (CC==1) then
        addu    0x1,r17,r17             //  r17 = r17 + 1

.deno_end:
        or      r31,r17,r17             // set sign bit
        ixfr    r18,f8          // set low order of return val
        bri     r1                      // return
        ixfr    r17,f9          // set high order of return val

//-------------- case ( overflow ) -----------------
.overflow:
        shr     31,r17,r27              // r27 = value of sign bit
        shl     31,r27,r29              // r29 = sign bit pattern
        ld.c    fsr,r28         // load fsr into r28
        shl     28,r28,r28              // fsr(low16) = 0bXXXXXXXXXXXXRMxx
        shr     30,r28,r28              // r28 = the value of rounding mode
        bte     1,r28,.ovf_01           //  round down
        bte     2,r28,.ovf_10           //  round up
        bte     3,r28,.ovf_11           //  chop

//.ovf_00                               // round to nearest or even
//      br      .max_val                // return +-MAX
// sas change so inf is returned by default
        br      .infin                  // return +-MAX
        nop

.ovf_01:                                // Round down (RM=01)
        bte     1,r27,.infin            // if(sign==1) then return -infinity
        br      .max_val                // else return +MAX
        nop
.ovf_10:                                // Round up (RM=10)
        bte     1,r27,.max_val          // if(sign==1) then return -MAX
        br      .infin                  // else rerurn +infinity
        nop
.ovf_11:                                // Chop (RM=11)
        br      .max_val                // return +-MAX
        nop

.max_val:
        orh     0xffff,r0,r18
        or      0xffff,r18,r18  // r18 = 0xffffffff : low order ret val
        shr     12,r18,r19              // r19 = 0x000fffff : high order
        orh     0x7fe0,r19,r19  // add max exponent, r19=0x7fefffff
        or      r29,r19,r19             // add sign bit pattern
        ixfr    r18,f8          // set low order of return val
// sas added so errno is set
        or      34,r0,r16               // r16 = 33 = 0x21
        orh     ha%_errno,r0,r31
        st.l    r16,l%_errno(r31)       // _errno = ERANGE; delayed
        bri     r1
        ixfr    r19,f9          // set high order of return val : delayed

.infin:
        orh     0x7ff0,r0,r20           // set all exponent to 1
        or      r29,r20,r19             // add sign bit to make +-infinity
        ixfr    r0,f8                   // set low order of return val
// sas added so errno is set
        or      34,r0,r16               // r16 = 33 = 0x21
        orh     ha%_errno,r0,r31
        st.l    r16,l%_errno(r31)       // _errno = EDOM; delayed
        bri     r1
        ixfr    r19,f9          // set high order of return val : delayed

//-------------- case ( r16 <= -2100 ) ---------------
.exp_add_under:                         // adjust the value and return
        br      .L1
        adds    -2098,r0,r16            // set r16 to minmam exponent number.
//

#else   /* _THREAD_SAFE */

.extern _seterrno

        .text

        .globl _ldexp
_ldexp:
ldexp:

// Code added to return proper errno value 

        pfeq.dd  f8,f8,f0
        bc .continue2

// X is a NaN

//  New code added for set errno
        addu -16,sp,sp
        st.l  r1,0(sp)
        fst.d f8,8(sp) 
        call    _seterrno
          or      33,r0,r16

        ld.l  0(sp),r1
        fld.d 8(sp),f8

        addu 16,sp,sp
        bri     r1
          nop

.continue2:

// End of code change 

        fxfr    f9,r17          // r17 is high order of input f_val
        fxfr    f8,r18          // r18 is low order of input f_val
        subs    2098,r16,r0             // if r16>=2098, CC set
        bc      .overflow               //  then goto .overflow
        subs    -2098,r16,r0            // if r16<=-2098, CC clear
        bnc     .exp_add_under          //  then goto .exp_add_under
.L1:
        andh    0x7ff0,r17,r20
        bc      .denorm_or_0            // val is either denormal or zero
        shr     20,r20,r20              // extract old exponent
        xor     0x07ff,r20,r0
        bc      .special_exit           // val is either infinite or NaN
.add_int_to_bias_exp:
        adds    r16,r20,r20             // r20=exponent value
        subs    r0,r20,r0               // if r20<=0 then underflow
        bnc     .underflow
        or      0x07ff,r0,r31
        subs    r20,r31,r0              // if r20>=0x7ff then overflow
        bnc     .overflow
.insert_bias_exp:
        shl     20,r20,r20
        andnoth 0x8000,r20,r20  // clear sign bit
        andnoth 0x7ff0,r17,r17  // clear old exponent
        or      r20,r17,r17             // insert new exponent
.special_exit:
        bri     r1
        ixfr    r17,f9

.denorm_or_0:                           // exponent is known to be zero
        andnoth 0x8000,r17,r21  // make val positive
        bnc     .big_denorm             // if high-part of fraction is not zero
        xor     r18,r0,r0               // check low fraction bits
        bc      .special_exit           // fraction is zero, a true zero
.lil_denorm:                            // 20 most signif. frac bits are 0
        adds    -19,r0,r20              // bias exponent after 20-bit lshift
.lil_lf_shft:                           // loop to normalize significant:
        adds    -1,r20,r20              //      decrement biased exponent
        andh    0x8000,r18,r0           //      is most signif. bit set?
        shl     1,r18,r18               //      shift, making MSB implicit
        bc      .lil_lf_shft            //      loop if implicit bit 0
.lil_rt_shft:                           // align normalized significant:
        shr     12,r18,r21              // put high frac in proper bield
        shl     20,r18,r18              // make low frac of result
        or      r17,r21,r17             // put sign and hi frac in r17
        br      .add_int_to_bias_exp    // done, bit for biased_exp field
        ixfr    r18,f8          // set-up low frac of result

.big_denorm:                            // fraction is more than 32-bits
        or      1,r0,r20                // pre-set the biased exponent
        shl     12,r21,r21              // left justify high-part of fraction
.lf_shft:                               // loop to normalize significant:
        adds    -1,r20,r20              //      decrement biased exponent
        andh    0x8000,r21,r0           //      is most signif. bit set?
        shl     1,r21,r21               //      shift, making MSB implicit
        bc      .lf_shft                //      loop if implicit bit 0
.rt_shft:                               // align normalized significant:
        shr     12,r21,r21              // return high frac to proper field
        adds    31,r20,r22              // get count of low frac bits needed
        shr     r22,r18,r22             // rt justify low frac bits to be hi
        or      r21,r22,r21             // put low frac bits in hi frac
        andh    0x8000,r17,r22  // get sign of val
        or      r21,r22,r17             // put sign & hi frac in r17
        subs    1,r20,r22               // get lshift count for low frac
        shl     r22,r18,r18             // align low frac properly
        br      .add_int_to_bias_exp    // done, but for biased_exp field
        ixfr    r18,f8          // set-up low frac of result
//-------------- case ( underflow ) ----------------
.underflow:
        shr     31,r17,r27              // r27 = value of sign bit
        shl     31,r27,r29              // r29 = sign bit pattern
        ld.c    fsr,r25         // load fsr into r25
        shl     28,r25,r25
        shr     30,r25,r25              // r25 = the value of rounding mode
        subs    -53,r20,r0              // if r20>-53
        bc      .make_denorm            //  then goto .make_denorm
        bte     1,r25,.udf_01           // else return min_val or zero
        bte     2,r25,.udf_10           // in these cases,
        bte     3,r25,.udf_11           // true underflow
//br    .min_val                // Round to nearest or even
// sas change so that zero is returned
        br      .d_zero         // Round to nearest or even
        nop                             //  return +-MIN
.udf_01:                                // Round down
        bte     1,r27,.min_val          //  if(sign==1) then return -MIN
        br      .d_zero                 //  else return zero
        nop
.udf_10:                                // Round up
        bte     1,r27,.d_zero           //  if(sign==1) then return zero
        br      .min_val                //  else return +MIN
        nop
.udf_11:                                // Chop
        br      .d_zero                 //  return +-zero
        nop

.min_val:
        or      0x0001,r0,r18           // r18 = 0x00000001 : low order ret val.
        ixfr    r18,f8          // set low order of return val
        bri     r1                      // r29 = 0x80000000 or 0x00000000
        ixfr    r29,f9          // set high order of return val

.d_zero:

//  New code added for set errno
        addu -8,sp,sp
        st.l  r1,0(sp)
        st.l  r29,4(sp)

        call    _seterrno
          or      34,r0,r16

        ld.l  0(sp),r1
        ld.l  4(sp),r29
        addu 8,sp,sp
//
        ixfr    r0,f8                   // set low order to 0x00000000
        bri     r1
        ixfr    r29,f9          // make signed zero into f8-f9

.make_denorm:                           // r17:r18 = 53bit significant
        shr     31,r17,r31
        shl     31,r31,r31              // r31 = sign bit
        andnoth 0xfff0,r17,r17  // r17 = high order of significant
        orh     0x0010,r17,r17  // set default msb (53bit)
        mov     r0,r19          // r17:r18:r19:r20 = 53+64bit v_signific
        or      0x0001,r0,r24           // r24 = 0x00000001 for lsb mask
        shl     31,r17,r21              // r21.31b = LSB of r17
        shr     1,r17,r17               // r17 = 1>>r17
        shl     31,r18,r22              // r22.31b = LSB of r18
        shr     1,r18,r18               // r18 = 1>>r18
        or      r18,r21,r18             // set r18.31b
        or      r19,r22,r19             // set r19.31b
        mov     r20,r26         // r26 = counter
        mov     r0,r20          // clear r20
.m_loop:                                //
        bte     r0,r26,.m_lp_end        // if (counter=0) then end loop
        shl     31,r17,r21              // r21.31b = LSB of r17
        shr     1,r17,r17               // r17 = 1>>r17
        shl     31,r18,r22              // r22.31b = LSB of r18
        shr     1,r18,r18               // r18 = 1>>r18
        or      r18,r21,r18             // set r18.31b
        shl     31,r19,r21              // r21.31b = LSB of r19
        shr     1,r19,r19               // r19 = 1>>r19
        or      r19,r22,r19             // set r19.31b
        shr     1,r20,r20               // r20 = 1>>r20
        or      r20,r21,r20             // set r20.31b
.m_d_2:
        br      .m_loop                 // loop to .m_loop
        adds    1,r26,r26               // r26 = r26 + 1 : delayed

.m_lp_end:
        bte     1,r25,.deno_01
        bte     2,r25,.deno_10
        bte     3,r25,.deno_11
//                                      // Round to nearest or even
        shr     31,r19,r21              // r21.01b = MSB of r19
        or      r0,r21,r0               // if r21==0 (means r19.31b == 0)
        bc      .deno_end               //  goto .deno_end
        shl     1,r19,r21               // r21 = 1<<r19
        or      r0,r21,r0               // if r21 != 0 (means not even case)
        bnc     .m_pls_one              //  goto .m_pls_one
        or      r0,r20,r0               // else if r20 != 0 (means not even)
        bnc     .m_pls_one              //  goto .m_pls_one
//                                      // else even case
        shl     31,r18,r21
        shr     31,r21,r21              // r21 = LSB of r18
        or      r0,r21,r0               // if r21 == 0
        bc      .deno_end               //  goto .deno_end
        br      .m_pls_one              // else goto .m_pls_one
        nop

.deno_01:                               // Round down
        or      r0,r19,r0               // if r19 != 0
        bnc     .deno_01_1              //  goto .deno_01_1
        or      r0,r20,r0               // if r20 != 0
        bnc     .deno_01_1              //  goto .deno_01_1
        br      .deno_end               // else
        nop                             //  goto .deno_end
.deno_01_1:
        bte     1,r27,.m_pls_one        //  if(sign) then (r17:r18)+=r28
        br      .deno_end               //  else return
        nop

.deno_10:                               // Round up
        or      r0,r19,r0               // if r19 != 0
        bnc     .deno_10_1              //  goto .deno_10_1
        or      r0,r20,r0               // if r20 != 0
        bnc     .deno_10_1              //  goto .deno_10_1
        br      .deno_end               // else
        nop                             //  goto .deno_end
.deno_10_1:
        bte     1,r27,.deno_end // if(sign) then return
        br      .m_pls_one              // else (r17:r18)+=r28
        nop

.deno_11:                               // Chop
        br      .deno_end               //  return
        nop

.m_pls_one:
        addu    1,r18,r18               // r18 = r18 + 1
        bnc     .deno_end               // if (CC==1) then
        addu    0x1,r17,r17             //  r17 = r17 + 1

.deno_end:
        or      r31,r17,r17             // set sign bit
        ixfr    r18,f8          // set low order of return val
        bri     r1                      // return
        ixfr    r17,f9          // set high order of return val

//-------------- case ( overflow ) -----------------
.overflow:
        shr     31,r17,r27              // r27 = value of sign bit
        shl     31,r27,r29              // r29 = sign bit pattern
        ld.c    fsr,r28         // load fsr into r28
        shl     28,r28,r28              // fsr(low16) = 0bXXXXXXXXXXXXRMxx
        shr     30,r28,r28              // r28 = the value of rounding mode
        bte     1,r28,.ovf_01           //  round down
        bte     2,r28,.ovf_10           //  round up
        bte     3,r28,.ovf_11           //  chop

//.ovf_00                               // round to nearest or even
//      br      .max_val                // return +-MAX
// sas change so inf is returned by default
        br      .infin                  // return +-MAX
        nop

.ovf_01:                                // Round down (RM=01)
        bte     1,r27,.infin            // if(sign==1) then return -infinity
        br      .max_val                // else return +MAX
        nop
.ovf_10:                                // Round up (RM=10)
        bte     1,r27,.max_val          // if(sign==1) then return -MAX
        br      .infin                  // else rerurn +infinity
        nop
.ovf_11:                                // Chop (RM=11)
        br      .max_val                // return +-MAX
        nop

.max_val:
        orh     0xffff,r0,r18
        or      0xffff,r18,r18  // r18 = 0xffffffff : low order ret val
        shr     12,r18,r19              // r19 = 0x000fffff : high order
        orh     0x7fe0,r19,r19  // add max exponent, r19=0x7fefffff
        or      r29,r19,r19             // add sign bit pattern
        ixfr    r18,f8          // set low order of return val

//  New code added for set errno
        addu -16,sp,sp
        st.l  r1,0(sp)
        st.l  r19,4(sp)
        fst.l  f8,8(sp)

        call    _seterrno
          or      34,r0,r16

        ld.l  0(sp),r1
        ld.l  4(sp),r19
        fld.l 8(sp),f8
        addu 16,sp,sp

        bri     r1
           ixfr    r19,f9          // set high order of return val : delayed

.infin:
        orh     0x7ff0,r0,r20           // set all exponent to 1
        or      r29,r20,r19             // add sign bit to make +-infinity
        ixfr    r0,f8                   // set low order of return val

//  New code added for set errno
        addu -16,sp,sp
        st.l  r1,0(sp)
        st.l  r19,4(sp)
        fst.l  f8,8(sp)

        call    _seterrno
          or      34,r0,r16

        ld.l  0(sp),r1
        ld.l  4(sp),r19
        fld.l 8(sp),f8
        addu 16,sp,sp

        bri     r1
           ixfr    r19,f9          // set high order of return val : delayed



//-------------- case ( r16 <= -2100 ) ---------------
.exp_add_under:                         // adjust the value and return
        br      .L1
        adds    -2098,r0,r16            // set r16 to minmam exponent number.
//

#endif  /* _THREAD_SAFE */
