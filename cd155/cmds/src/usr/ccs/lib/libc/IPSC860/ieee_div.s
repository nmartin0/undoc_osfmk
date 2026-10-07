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
 
//
//    Copyright (c) 1990 INTEL CORPORATION
//
//    This file contains information proprietary to Intel Corporation.
//    This software is supplied under terms of a license or non-disclosure
//    agreement and should be treated as confidential in accordance with
//    this agreement.
//
// ieee_div.s 8.2 90/08/27 10:23:38
//
/*
 * HISTORY
 * $Log: ieee_div.s,v $
 * Revision 1.4  1994/11/19  02:12:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/08/04  23:19:23  stans
 *    Cleanup CVS log messages, set 'cvs admin -c" * "' CVS log message leader.
 *
 */

	.file "ieee_div.s"
	.text
	.align 32
_RDIVF::			// This entry for back compatibility
	adds	-16,sp,sp
	st.l	r1,12(sp)
	fmov.sd	f16,f8
	call	.ieee_div
	 fmov.sd f18,f10
	fmov.ds	f8,f8
	ld.l	12(sp),r1
	bri	r1
	adds	16,sp,sp

_RDIVD::			// This entry for back compatibility
	adds	-16,sp,sp
	st.l	r1,12(sp)
	fmov.dd	f16,f8
	call	.ieee_div
	fmov.dd	f18,f10
	ld.l	12(sp),r1
	bri	r1
	adds	16,sp,sp

.ieee_fp_divf::			// Single precision ABI entry
	adds	-16,sp,sp
	st.l	r1,12(sp)
	fmov.sd	f9,f10
	call	.ieee_div
	fmov.sd f8,f8
	fmov.ds f8,f8
	ld.l	12(sp),r1
	bri	r1
	adds	16,sp,sp

.ieee_fp_div::			// Double precision ABI entry
.ieee_div:

    fadd.ss   f0,      f0,   f0   // any unmasked f.p. exceptions pending?

save_registers:
    addu     -112,     r2,   r2   // make new stack frame
    fst.d     f10,      0(r2)     // save f10 - f11
    fst.q     f12,     16(r2)     // save f12 - f15
    fst.q     f16,     32(r2)     // save f16 - f19
    fst.q     f20,     48(r2)     // save f20 - f23
    st.l      r16,     64(r2)     // save r16
    st.l      r17,     68(r2)     // save r17
    st.l      r18,     72(r2)     // save r18
    st.l      r19,     76(r2)     // save r19
    st.l      r20,     80(r2)     // save r20
    st.l      r21,     84(r2)     // save r21
    st.l      r22,     88(r2)     // save r22
    st.l      r23,     92(r2)     // save r23
    st.l      r24,     96(r2)     // save r24
    
    ld.c      fsr,     r17        // r17 <-- caller's fsr
    mov       r17,     r16        // r16 <-- caller's fsr, to be saved
    andnot    0x002C,  r17,  r18  // fsr.FTE <-- 0, to disable f.p. traps
                                  // fsr.RM  <-- 00B, round to nearest
    st.c      r18,     fsr        // install modified fsr
    fadd.dd   f8,      f10,  f0   // check for invalid source operands
    ld.c      fsr,     r17        // r17 <-- fsr with source exceptions
    and       0x0100,  r17,  r0   // CC set iff fsr.SE = 0
    bnc       src_xcptn           // if fsr.SE = 1, an operand is invalid

    fxfr      f11,     r21        // r21 <-- high_32(y)
    fxfr      f9,      r17        // r17 <-- high_32(x)
    andh      0x7FF0,  r21,  r20  // r20 <-- bexp(y) * (2**20)
                                  // CC set iff bexp(y) = 0 iff y = +/- 0
    bc        is_dividend_also_zero

    andh      0x7FF0,  r17,  r19  // r19 <-- bexp(x) * (2**20)
                                  // CC set iff bexp(x) = 0 iff x = +/- 0
    bnc    dividend_not_zero
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmul.dd   f8,      f10,  f8   // f8 <-- (+/-0)*y = (+/-0)/y for y /= 0


is_dividend_also_zero:

    andh      0x7FF0,  r17,  r19  // r19 <-- bexp(x) * (2**20)
                                  // CC set iff bexp(x) = 0 iff x = +/- 0
    bnc    only_divisor_is_zero
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_zero_divide

    orh       0xFFF8,  r17,  r17  // r17      <-- high_32(INDEFINITE)
    ixfr      r17,     f9         // (f9:f8) <--  correctly signed INFINITY
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,   f0   // set fsr.SE

    
dividend_not_zero:

    andnoth   0x7FF0,  r21,  r21  // r21.bexp <-- 000_0000_0000B
    orh       0x3FF0,  r21,  r21  // r21.bexp <-- 011_1111_1111B = 0x3FF
    ixfr      r21,     f11        // (f11:f10) <-- y/(2**exp(y))
    andnoth   0x8000,  r21,  r21  // r21.sign <-- 0
    ixfr      r21,     f23        // f23 <-- high_32(|y|/2**exp(y))
    fmov.ss   f10,     f22        // f22 <-- low_32(|y|/2**exp(y))
    andnoth   0x7FF0,  r17,  r17  // r17.bexp <-- 000_0000_0000B
    orh       0x3FF0,  r17,  r17  // r17.bexp <-- 011_1111_1111B = 0x3FF
    ixfr      r17,     f9         // (f9:f8) <-- x/(2**exp(x))
    andnoth   0x8000,  r17,  r17  // r17.sign <-- 0
    ixfr      r17,     f21        // f21 <-- high_32(|x|/2**exp(x))
    fmov.ss   f8,      f20        // f20 <-- low_32(|x|/2**exp(x))
    orh       0x4000,  r0,   r17  // r17 <-- high 32 bits of 2.0D
    ixfr      r17,     f17        // f17 <-- high 32 bits of 2.0D
    fmov.ss   f0,      f16        // f16 <-- low  32 bits of 2.0D
    frcp.dd   f10,     f12        // 1st approx. to 1/y: r0 = r(y)
    fmul.dd   f10,     f12,  f14  // y*r0
    or        0x0001,  r0,   r17  // r17 <-- lsb of 1, rest 0 (1 ulp)
    fsub.dd   f16,     f14,  f14  // f14 <-- 2 - y*r0
    ixfr      r17,     f18        // f18 <-- lsb of 1, rest 0 (1 ulp)
    fmul.dd   f12,     f14,  f12  // second approx. to 1/y: r1 =  r0*(2-y*r0)
    shra      20,      r19,  r19  // r19 <-- bexp(x), rt. justified
    fmul.dd   f10,     f12,  f14  // f14 <-- y*r1
    fmov.ss   f0,      f19        // f19 <-- 0, so (f19:f18) = 00...01
    fsub.dd   f16,     f14,  f14  // f14 <-- 2 - y*r1
    orh       0x3ff0,  r0,   r17  // r17 <-- high 32 bits of 1.0D
    ixfr      r17,     f17        // f17 <-- high 32 bits of 1.0D
    fmul.dd   f12,     f14,  f12  // 3rd approx. to 1/y: r2 =  r1*(2-y*r1)
    or        0x0008,  r18,  r17  // r17.RM <-- 10B, round up
    fmul.dd   f10,     f12,  f14  // f14 <-- y*r2
    and       0x000C,  r16,  r21  // r21.RM <-- caller's rounding mode
    fsub.dd   f16,     f14,  f14  // f14 <-- 1 - y*r2
    fmul.dd   f12,     f8,   f12  // f12 <-- x*r2
    pfgt.dd   f8,      f0,   f0   // CC set iff x > 0.0
    fmul.dd   f14,     f12,  f14  // f14 <-- error = (1 - y*r2) * (x*r2)
    shra      20,      r20,  r20  // r20 <-- bexp(y), rt. justified
    fadd.dd   f12,     f14,  f12  // f12 <-- x*r2 + (1 - y*r2) * (x*r2)
                                  //         = (in effect) x*r3 = q
    btne      0,       r21,  RM_NOT_TO_NEAREST

    st.c      r17,     fsr        // fsr.RM <-- round_up
    fmul.dd   f12,     f10,  f16  // f16 <-- q*y rounded up = (q*y)UP
    fmlow.dd  f12,     f10,  f14  // (f15:f14) <-- int*8(low_53(q*y))
    bc.t      check_approx_q      // branch if x > 0.0
    fisub.dd  f0,      f14,  f14  // (f15:f14) <-- comp2(int*8(low_53(q*y)))

    fisub.dd  f0,      f18,  f18  // (f19:f18) <-- comp2(int*8(00...01))

check_approx_q:
    pfgt.dd   f8,      f16,  f0   // set CC if (q*y)UP < x
    bc.t      get_exponent        // branch if approx. too low
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP

    pfeq.dd   f8,      f16,  f0   // set CC if x = (q*y)UP
    bc        get_remainder       // branch if approx may be too low
// Here, approx. may be too high, so we prepare to reduce it by 1 ulp.
    or        0x0004,  r18,  r17  // r17.RM <-- round_down
    st.c      r17,     fsr        // fsr.RM <-- round_to_nearest
    fisub.dd  f0,      f18,  f18  // change sign of ONE_ULP
    fisub.dd  f0,      f14,  f14  // if x < 0, comp2(int*8(low_53(q*y)))
                                  // if x > 0, just  int*8(low_53(q*y))
    fmul.dd   f12,     f10,  f16  // f16 <-- q*y rounded down = (q*y)DOWN
    pfgt.dd   f16,     f8,   f0   // set CC if  x < (q*y)DOWN
    bc.t      get_exponent        // branch if approx. too high
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP

get_remainder:
    pfgt.dd   f22,     f20,  f0   // CC set iff |y| > |x|
    bc        double_remainder
    fxfr      f15,     r17        // r17 <-- high_32(int*8(low_53(q*y)))
    andnoth   0x0010,  r17,  r17  // r17 <-- high_32(remainder) if |x| >= |y|
    ixfr      r17,     f15        // f15 <-- high_32(remainder)

double_remainder:
    fiadd.dd  f14,     f14,  f14  // 2*remainder (left shift)
    fxfr      f11,     r17        // r17 <-- high 32 bits of y
                                  // Note that lsb of r17.bexp = 1; this bit
                                  // will be used as y's now explicit leading
                                  // significand bit 
    fxfr      f15,     r21        // r21 <-- high_32{2*remainder}
    andnoth   0xffe0,  r17,  r17  // r17 <-- high_21{signif(y)},   rt. jstfd.
    andnoth   0xffc0,  r21,  r21  // r21 <-- high_22{2*remainder}, rt. jstfd.
// Now compare sig(y) to 2*remainder.  If it "fits", change q by 1 ULP.
    subu      r21,     r17,  r0   // compare more significant bits
                                  // CC clear iff no change in q is needed
    bc        continue_rem_test

get_exponent:

    fxfr      f13,     r17        // r17 <-- high_32(q)
    subu      r19,     r20,  r21  // r21 <-- bexp(x)-bexp(y) = exp(x)-exp(y)
    andh      0x7FF0,  r17,  r18  // r18 <-- bexp(q) * (2**20)
    shr       20,      r18,  r18  // r18 <-- bexp(q)
    adds      r21,     r18,  r18  // r18 <-- bexp(x/y)
    subs      r0,      r18,  r0   // CC clear iff bexp(x/y) <= 0
    bnc   underflow               // branch not taken iff bexp(x/y) > 0
    subu      0x07FE,  r18,  r0   // CC clear iff bexp(x/y) >  0x07FE
    bnc   overflow
    shl       20,      r18,  r18  // r18 <-- bexp(x/y) * (2**20)
    andnoth   0x7FF0,  r17,  r17  // r17.bexp <-- 000_0000_0000B
    or        r18,     r17,  r17  // r17.bexp <-- bexp(x/y)
    fmov.ss   f12,     f8         // f8 <-- low_32(q)
    ixfr      r17,     f9         // (f9:f8) <-- q*(2**[exp(x)-exp(y)])
    st.c      r16,     fsr        // restore user's fsr
    fmul.dd   f12,     f22,  f0   // |q|*|y| sets inexact bits in fsr

restore_regs:
    ld.l      96(r2),  r24        // restore r24
    ld.l      92(r2),  r23        // restore r23
    ld.l      88(r2),  r22        // restore r22
    ld.l      84(r2),  r21        // restore r21
    ld.l      80(r2),  r20        // restore r20
    ld.l      76(r2),  r19        // restore r19
    ld.l      72(r2),  r18        // restore r18
    ld.l      68(r2),  r17        // restore r17
    ld.l      64(r2),  r16        // restore r16
    fld.q     48(r2),  f20        // restore f20 - f23
    fld.q     32(r2),  f16        // restore f16 - f19
    fld.q     16(r2),  f12        // restore f12 - f15
    fld.d      0(r2),  f10        // restore f10 - f11
    bri       r1
    addu      112,     r2,   r2   // restore old stack pointer

continue_rem_test:

    xor       r21,     r17,  r0   // CC set iff less significant bits must be
                                  // compared; i.e. iff more sig. bits agree.
    bc        finish_rem_test

adjust_q:
    br        get_exponent
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP

finish_rem_test:
    fxfr      f14,     r21        // r21 <-- low_32{2*remainder}
    fxfr      f10,     r17        // r17 <-- low_32{signif(y)}
    subu      r21,     r17,  r0   // compare less significant bits
                                  // CC clear iff no change in q is needed
    bnc       get_exponent
    br        get_exponent
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP

RM_NOT_TO_NEAREST:
    pfgt.ss   f13,     f0,   f0   // CC set iff q > 0.0
    bte       0x0004,  r21,  round_down
    bte       0x0008,  r21,  round_up
    bnc       round_up            // chop uses round_up for q < 0

round_down:
    bc.t      down_chk_sign_y
    fisub.dd  f0,      f18,  f18  // (f19:f18) <-- comp2(int*8(00...01))
down_chk_sign_y:
    pfgt.ss   f11,     f0,   f0   // CC set iff y > 0.0
    bc.t      down_and_y_pos
    or        0x0008,  r18,  r18  // r18.RM <-- ROUND_UP
    or        0x0004,  r18,  r18  // r18.RM <-- ROUND_DOWN
    st.c      r18,     fsr        // fsr.RM <-- ROUND_DOWN
    fmul.dd   f12,     f10,  f16  // f16 <-- q*y rounded down = (q*y)DOWN
    pfle.dd   f8,      f16,  f0   // CC CLEAR if x/2**(exp(x) =< (q*y)DOWN
    bnc       get_exponent
    br        get_exponent
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP per sign of x

down_and_y_pos:
    st.c      r18,     fsr        // fsr.RM <-- ROUND_UP
    fmul.dd   f12,     f10,  f16  // f16 <-- q*y rounded up = (q*y)UP
    pfle.dd   f16,     f8,   f0   // CC CLEAR if (q*y)UP =< x/2**(exp(x)
    bnc       get_exponent
    br        get_exponent
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP per sign of x
    
round_up:
    bnc.t     up_chk_sign_y
    fisub.dd  f0,      f18,  f18  // (f19:f18) <-- comp2(int*8(00...01))
up_chk_sign_y:
    pfgt.ss   f11,     f0,   f0   // CC set iff y > 0.0
    bc.t      up_and_y_pos
    or        0x0004,  r18,  r18  // r18.RM <-- ROUND_DOWN
    or        0x0008,  r18,  r18  // r18.RM <-- ROUND_UP
    st.c      r18,     fsr        // fsr.RM <-- ROUND_UP
    fmul.dd   f12,     f10,  f16  // f16 <-- q*y rounded up = (q*y)UP
    pfle.dd   f16,     f8,   f0   // CC CLEAR if (q*y)UP =< x/2**(exp(x)
    bnc       get_exponent
    br        get_exponent
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP per sign of x

up_and_y_pos:
    st.c      r18,     fsr        // fsr.RM <-- ROUND_DOWN
    fmul.dd   f12,     f10,  f16  // f16 <-- q*y rounded down = (q*y)DOWN
    pfle.dd   f8,      f16,  f0   // CC CLEAR if x/2**(exp(x) =< (q*y)DOWN
    bnc       get_exponent
    br        get_exponent
    fiadd.dd  f12,     f18,  f12  // q <-- q +/- 1 ULP per sign of x

overflow:
    and       0x0020,  r16,  r0   // CC set iff caller's fsr.FTE = 0;
                                  // i.e.,  iff fp traps are off
    bc    masked_overflow         // branch not taken iff fp traps are on

    adds    -0x03ff,  r18,   r18  // r18 <-- bexp[(x/y)/2**(1023)]
    subu     0x07FE,  r18,   r0   // CC clear iff r18 > 0x07FE
    bc   can_do_trapped_overflow

even_rebiased_exp_out_of_range:
    orh      0xFFF8,  r0,    r17  // r17 <-- high_32(INDEF)
    ixfr     r17,     f9
    fmov.ss  f0,      f8          // (f9:f8) <-- INDEF
    st.c      r16,     fsr        // restore user's fsr
    br        restore_regs
    fmul.dd  f8,      f8,    f8   // cause source exception

can_do_trapped_overflow:
    shl       20,      r18,  r18  // r18 <-- bexp[(x/y)/2**(1023) * (2**20)
    andnoth   0x7FF0,  r17,  r17  // r17.bexp <-- 000_0000_0000B
    or        r18,     r17,  r17  // r17.bexp <-- bexp[(x/y)/2**(1023)
    fmov.ss   f12,     f8         // f8 <-- low_32(q)
    ixfr      r17,     f9         // (f9:f8) <-- q*(2**[exp(x)-exp(y)-1023])
    orh       0x7FE0,  r0,   r18  // r18 <-- high_32(2**1023)
    fmov.ss   f0,      f10
    ixfr      r18,     f11        // (f11:f10) <-- 2**1023
    st.c      r16,     fsr        // restore user's fsr
    br        restore_regs
    fmul.dd   f8,      f10,  f8   // cause overflow
     
masked_overflow:
    andnot    0x1e00,  r16,  r16  // r16 <-- caller's fsr with Mx bits cleared
    andh      0x8000,  r17,  r23  // r23.sign <-- q's sign, rest 0
    and       0x000c,  r16,  r19  // r19.RM <-- caller's fsr.RM
    bc   set_up_signed_INF        // CC set iff caller's fsr.RM = 00B
    btne      0x000c,  r19, ovflow_check_sign_of_q

set_up_signed_huge:
    orh       0x7fef,  r23,  r23
    or        0xffff,  r23,  r23
    br   finish_signed_ovflow
    or        0x0C90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MO <-- 1; r16.MI <-- 1
    
set_up_signed_INF:
    orh       0x7ff0,  r23,  r23  // r23 <-- high_32(signed_INF)

signed_INF_status_bits:
    or        0x1C90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MO <-- 1; r16.MI <-- 1; r16.MA <--1

finish_signed_ovflow:
    ixfr      r23,     f9
    fmov.ss   f0,      f8         // (f9:f8) <-- INF or HUGE with sign of q
    br        restore_regs
    st.c      r16,     fsr

ovflow_check_sign_of_q:
    andh      0x8000,  r17,  r23  // r23.sign <-- q's sign, rest 0
    bc   ovflow_is_RM_up
    btne      0x0004,  r19,  set_up_signed_huge
    br   signed_INF_status_bits
    orh       0x7ff0,  r23,  r23  // r23 <-- high_32(signed_INF)
    
ovflow_is_RM_up:
    btne      0x0008,  r19,  set_up_signed_huge
    br   signed_INF_status_bits  
    orh       0x7ff0,  r23,  r23  // r23 <-- high_32(signed_INF)


underflow:
    and       0x0020,  r16,  r0   // CC set iff caller's fsr.FTE = 0;
                                  // i.e.,  iff fp traps are off
    bc    masked_underflow        // branch not taken iff fp traps are on

    and       0x0001,  r16,  r0   // CC clear iff caller's fsr.FZ = 1;
                                  // i.e.,    iff caller wants underflows
                                  //              to be flushed to zero.
    bc    trapped_underflow       // branch taken only if caller wants
                                  // underflows to go to trap-handler
    andnot    0x1e00,  r16,  r16  // r16 <-- caller's fsr with Mx bits cleared
    andh      0x8000,  r17,  r17  // r17 <-- 0 with the sign of x/y
    fmov.ss   f0,      f8         // f8  <-- 0
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    ixfr      r17,     f9         // (f9:f8) <-- d.p. 0 with sign of x/y
    br        restore_regs
    st.c      r16,     fsr

trapped_underflow:
    adds      0x03FE,  r18,  r18  // r18 <-- bexp(x/y) + 0x03FE ( > 0 ?)
    subs      r0,      r18,  r0   // CC clear iff r18 =< 0
    bnc   even_rebiased_exp_out_of_range

    shl       20,      r18,  r18  // r18 <-- (bexp(x/y) + 0x03FE) * (2**20)
                                  // r18.bexp <-- bexp(x/y) + 0x03FE
    andnoth   0x7FF0,  r17,  r17  // r17.bexp <-- 000_0000_0000B
    or        r18,     r17,  r17  // r17.bexp <-- bexp(x/y) + 0x03FE
    orh       0x0010,  r0,   r18  // r18.bexp <-- 000_0000_0001B
    ixfr      r17,     f9         // f9  <-- high_32((x/y)*(2**0x03FE))
    fmov.ss   f12,     f8         // f8  <-- low_32(x/y)
    ixfr      r18,     f11        // f11 <-- high_32(2**(-0x03FE))
    fmov.ss   f0,      f10        // f10 <--  low_32(2**(-0x03FE))
    st.c      r16,     fsr        // restore user's fsr
    br        restore_regs
    fmul.dd   f8,      f10,  f8   // cause underflow


masked_underflow:
    subs      0x0001,  r18,  r18  // r18 <--  1 - bexp(x/y)  = shift_count
    andnot    0x1e00,  r16,  r16  // r16 <-- caller's fsr with Mx bits cleared
    subu      53,      r18,  r0   // CC clear iff shift_count > 53
    bnc  severe_unflow_reround    // branch not taken iff shift_count <= 53
    fxfr      f12,     r22        // r22 <-- low_32(q)
    adds     -32,      r18,  r0   // CC clear iff shift_count >= 32
    bnc  big_shift_count          // branch not taken iff shift_count < 32
    andnoth   0xfff0,  r17,  r23  // r23 <-- high_20(frac(q))
    orh       0x0010,  r23,  r23  // r23 <-- high_21(signif(q))
    subu      32,      r18,  r19  // r19 <-- 32 - shift_count
    shl       r19,     r22,  r21  // r21 <-- bits shifted out, left justified
    shr       r18,     r0,   r0   // psr.SC <-- shift_count
    shrd      r23,     r22,  r22  // r22 <--  low_32(right_shifted_signif)
    shr       r18,     r23,  r23  // r23 <-- high_21(right_shifted_signif)
    br   unflow_reround
    shl       r0,      r0,   r20  // r20 <-- 0

big_shift_count:
    adds     -32,      r18,  r18  // r18 <-- shift_count - 32
    subu      32,      r18,  r19  // r19 <-- 32 - reduced_shift_count
    shl       r19,     r22,  r20  // r20 <-- first (shift_count-32) bits
                                  //         right-shifted out
    shr       r18,     r0,   r0   // psr.SC <-- reduced_shift_count
    shrd      r23,     r22,  r21  // r21 <-- last 32 bits right-shifted out
    shr       r18,     r23,  r22  // r22 <-- low_32(right_shifted_signif)
    shl       r0,      r0,   r23  // r23 <-- high_21(right_shifted_signif)


unflow_reround:

    andnoth   0x8000,  r17,  r18  // r18 <-- high_32(|q|)
    ixfr      r18,     f13        // (f13:f12) <-- |q|
    andh      0x8000,  r17,  r17  // r17.SIGN <-- sign of q
    fmul.dd   f22,     f12,  f16  // q was exact iff fsr.MI = 0 now
                                  // f16 <-- |y|*|q|
    ld.c      fsr,     r18        // r18.MI <-- fsr.MI
    or        r17,     r23,  r23  // r23 <-- high_32(denormalized_q)
    ixfr      r23,     f9
    ixfr      r22,     f8
    and       0x0800,  r18,  r0   // CC set iff fsr.MI = 0
    bnc   unflow_second_round
    xor       r21,     r0,   r0   // CC set iff r21 = 0
    bnc   unflow_1st_round
    xor       r20,     r0,   r0   // CC set iff r20 = 0
    bnc   unflow_1st_round        // branch not taken iff
                                  // denormal result is exact - no underflow
    or        0x0010,  r16,  r16  // r16.U <-- 1
    br        restore_regs
    st.c      r16,     fsr
    
unflow_second_round:
    xor       r21,     r0,   r0   // CC set iff r21 = 0
    bnc  second_round_get_mode
    xor       r20,     r0,   r0   // CC set iff r20 = 0
    bnc  second_round_get_mode       // branch not taken iff denormalization
                                  // caused no extraordinary loss of precision
    and       0x1E80,  r18,  r18  // r18 <-- SI, Mx bits of |y|*|q| fsr; rest 0
    xor       0x1210,  r18,  r18  // r18.U <-- 1; r18.MU <-- 1;
                                  // r18.MA <-- flipped MA from |y|*|q|
    or        r18,     r16,  r16  // caller's fsr with SI, Mx bits
                                  // showing inexact multiply underflow
    pfeq.dd   f16,     f20,  f0   // CC set iff |y|*|q| = |x|
    bnc  get_MA_bit

no_xtra_lost_prcsn:
    br        restore_regs
    st.c      r16,     fsr

get_MA_bit:
    pfgt.dd   f16,     f20,  f0   // CC set iff |y|*|q| > |x|
    bc.t  no_xtra_lost_prcsn
    or        0x1000,  r16,  r16  // r16.MA <-- 1
    
    andnot    0x1000,  r16,  r16  // r16.MA <-- 0
    br        restore_regs
    st.c      r16,     fsr


second_round_get_mode:

    and       0x000c,  r16,  r19  // r19.RM <-- caller's fsr.RM
    bc   second_round_to_nearest     // CC set iff caller's fsr.RM = 00B


rounding_not_to_nearest:

    andh      0x8000,  r23,  r0   // CC set iff q was positive
    bte       0x0008,  r19,  unflow_up
    bte       0x0004,  r19,  unflow_down
                                  // branch not taken iff fsr.RM = 11B (chop)
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr

second_round_to_nearest:
    orh       0x8000,  r0,   r19  // r19 <-- 0x80000000
    subu      r21,     r19,  r0   // CC set iff r21 >= 0x80000000
    bc   is_rest_of_r21_0
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr

is_rest_of_r21_0:
    xor       r21,     r19,  r0   // CC set iff r21 = 0x80000000
    bnc  add_1_ulp
    xor       r20,     r0,   r0   // CC set iff r20 = 0
    bnc  add_1_ulp

// With the preceding branch not taken, the program appears to be in the
// tie-breaking case, but this appearance is deceptive.  The "tail" value
// 0x80000000 came about from a previous inexact rounding, so the true tail
// was more or less than 0x80000000, but not the same.  If it was less,
// then this rounding should not add 1 ulp; however, if it was more,
// then this rounding should     add 1 ulp.
     
    pfeq.dd   f16,     f20,  f0   // CC set iff |y|*|q| = |x|
    bnc  how_was_q_rounded
    xor       0x1000,  r18,  r0   // CC set iff |y|*|q| set MA bit
                                  //        iff |q| was chopped
    bnc   add_1_ulp

q_was_rounded_up:
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr
    
how_was_q_rounded:
    pfgt.dd   f16,     f20,  f0   // CC set iff |y|*|q| > |x|
    bc    q_was_rounded_up

add_1_ulp:
    or        0x0001,  r0,   r19  // r19 <-- 0x00000001
    ixfr      r19,     f18
    fmov.ss   f0,      f19        // (f19:f18) <-- 1 ulp
    fiadd.dd  f8,      f18,  f8   // significand of (f9:f8) increased by 1 ulp
    or        0x1A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1; r16.MA <-- 1
    br        restore_regs
    st.c      r16,     fsr


unflow_up:
    bc   add_1_ulp
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr
    

unflow_down:
    bnc  add_1_ulp
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr
    

unflow_1st_round:
    and       0x000c,  r16,  r19  // r19.RM <-- caller's fsr.RM
    bnc  rounding_not_to_nearest  // CC set iff caller's fsr.RM = 00B
    orh       0x8000,  r0,   r19  // r19 <-- 0x80000000
    subu      r21,     r19,  r0   // CC set iff r21 >= 0x80000000
    bc   first_is_rest_of_r21_0
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr

first_is_rest_of_r21_0:
    xor       r21,     r19,  r0   // CC set iff r21 = 0x80000000
    bnc  add_1_ulp
    xor       r20,     r0,   r0   // CC set iff r20 = 0
    bnc  add_1_ulp

// Here, program really is in tie-breaking situation.  It breaks the tie
// in the way that clears the Least Significant Bit of the (not necessarily
// denormalized) result.

    and       0x0001,  r22,  r0   // CC set iff LSB is 0
    bnc  add_1_ulp
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr


severe_unflow_reround:
    and       0x000c,  r16,  r19  // r19.RM <-- caller's fsr.RM
    bc   set_up_signed_0          // CC set iff caller's fsr.RM = 00B
    btne      0x000c,  r19, check_sign_of_q
set_up_signed_0:
    andh      0x8000,  r17,  r23  // r23.sign <-- q's sign, rest 0
    ixfr      r23,     f9
finish_signed_0:
    fmov.ss   f0,      f8         // (f9:f8) <-- 0 with sign of q
    or        0x0A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1;
    br        restore_regs
    st.c      r16,     fsr

check_sign_of_q:
    andh      0x8000,  r17,  r23  // r23.sign <-- q's sign, rest 0
    ixfr      r23,     f9
    bc   is_RM_up
    btne      0x0004,  r19,  finish_signed_0
    or        0x0001,  r0,   r19  // r19 <-- 0x00000001
return_tiny_denorm:
    ixfr      r19,     f8
    or        0x1A90,  r16,  r16  // r16.U  <-- 1; r16.SI <-- 1;
                                  // r16.MU <-- 1; r16.MI <-- 1; r16.MA <-- 1
    br        restore_regs
    st.c      r16,     fsr

is_RM_up:
    btne      0x0008,  r19,  finish_signed_0
    br   return_tiny_denorm    
    or        0x0001,  r0,   r19  // r19 <-- 0x00000001


src_xcptn:
    fadd.dd   f8,      f0,   f0   // Is f8 (x) an invalid source operand?
    ld.c      fsr,     r17        // r17 <-- fsr with source exceptions
    and       0x0100,  r17,  r0   // CC set iff fsr.SE = 0
    bc        only_y_invalid      // if fsr.SE = 1, dividend x is invalid
    
    fadd.dd   f10,      f0,   f0  // Is f10 (y) also an invalid source operand?
    ld.c      fsr,     r17        // r17 <-- fsr with source exceptions
    and       0x0100,  r17,  r0   // CC set iff fsr.SE = 0
    bc        only_x_invalid      // if fsr.SE = 1, divisor y is also invalid

    fxfr      f9,      r17        // r17 <-- high_32(x)
    andh      0x7FF0,  r17,  r0   // CC set iff bexp(x) = 0 iff x = +/- denorm
    bc        at_least_x_denorm   // branch not taken iff bexp(x) = 0x7FF

    andnoth   0xFFF0,  r17,  r21  // if CC clear, frac(x) /= 0, so x /= +/- INF
    fxfr      f8,      r20        // r20 <-- low_32(x)
    bnc       at_least_x_NaN      // branch taken only if x is NaN
    btne      r20,     r0,   at_least_x_SNaN
                                  // branch     taken only if x is    SNaN
                                  // branch not taken only if x is +/- INF
    fxfr      f11,     r19        // r19 <-- high_32(y)
    andh      0x7FF0,  r19,  r0   // CC set iff bexp(y) = 0 iff y = +/- denorm
    bnc       x_inf_y_not_denorm  // branch not taken iff
                                  // y is denormal and x is +/- INF
    andh      0x8000,  r19,  r19  // r19.sign <-- sign(y), r19.rest <-- 0
    xor       r17,     r19,  r17  // r17 <-- high_32(INF),
                                  // with sign = xor of operands' signs
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    ixfr      r17,     f9         // INF/denorm =
                                  // (sign(INF) xor sign(denorm)) * |INF|
                                      
x_inf_y_not_denorm:

    andnoth   0xFFF0,  r19,  r23  // if CC clear, frac(y) /= 0, so y /= +/- INF
    fxfr      f10,     r22        // r22 <-- low_32(y)
    bnc       x_inf_y_NaN         // branch taken only if y is NaN
    btne      r22,     r0,   x_inf_y_SNaN
                                  // branch     taken only if y is SNaN
                                  // branch not taken only if y is +/- INF
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_infinite_operands

    orh       0x8008,  r17,  r17  // Set sign and leading frac bits of
                                  // high_32(x) = high_32(signed INF) to
                                  // produce high_32(QNaN INDEF) in r17
    ixfr      r17,     f9         // make (f9:f8) = INDEF
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,   f0   // set fsr.SE
    
trap_infinite_operands:
    andh      0x8000,  r19,  r23  // r23 <-- 0 with sign of y
    ixfr      r23,     f11        // (f11:f10) <-- 0 with the sign of y
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmul.dd   f8,      f10,  f8   // cause source exception

x_inf_y_NaN:
    andh      0x0008,  r19,  r0   // CC set iff most signif. bit of frac(y) = 0
    bc        x_inf_y_SNaN        // branch not taken only if y is QNaN
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmov.dd   f10,     f8         // return QNaN y

x_inf_y_SNaN:
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_SNaN_operand

    orh       0x0008,  r19,  r19  // Set leading frac bit of
                                  // high_32(y) = high_32(SNaN) to
                                  // produce high_32(quietized SNaN) in r19
    ixfr      r19,     f9         // make (f9:f8) = quietized SNaN
    fmov.ss   f10,     f8
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,   f0   // set fsr.SE

    
trap_SNaN_operand:

    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmul.dd   f8,      f10,  f8   // cause source exception
    

at_least_x_NaN:

    andh      0x0008,  r17,  r0   // CC set iff most signif. bit of frac(x) = 0
                                  //        iff x is a signaling NaN
    fxfr      f11,     r19        // r19 <-- high_32(y)
    bc        at_least_x_SNaN     // branch not taken iff x is QNaN
    
    andh      0x7FF0,  r19,  r0   // CC set iff bexp(y) = 0 iff y = +/- denorm
    bnc       y_too_may_be_NaN    // branch not taken iff bexp(y) = 0x000

    br        restore_regs
    st.c      r16,     fsr        // restore caller's fsr


y_too_may_be_NaN:

    andnoth   0xFFF0,  r19,  r23  // if CC clear, frac(y) /= 0, so y /= +/- INF
    fxfr      f10,     r22        // r22 <-- low_32(y)
    bnc       y_too_NaN           // branch taken only if y is NaN

    btne      r22,     r0,   x_QNaN_y_SNaN
                                  // branch     taken only if y is SNaN
                                  // branch not taken only if y is +/- INF; 
return_QNaN_x:
    br        restore_regs
    st.c      r16,     fsr        // restore caller's fsr


y_too_NaN:
    andh      0x0008,  r19,  r0   // CC set iff most signif. bit of frac(y) = 0
                                  //        iff y is a signaling NaN
    bc        x_QNaN_y_SNaN       // branch not taken iff y is also QNaN 

    subu      r23,     r21,  r0   // CC clear iff
                                  //        high_20(frac(x)) > high_20(frac(y))
    bnc       return_QNaN_x

    bte       r23,     r21,  cmp_lo_fracs

    fmov.dd   f10,     f8         // return QNaN y
    br        restore_regs
    st.c      r16,     fsr        // restore caller's fsr


cmp_lo_fracs:

    fxfr      f8,      r20        // r20 <-- low_32(frac(x))
    fxfr      f10,     r22        // r22 <-- low_32(frac(y))
    subu      r20,     r22,   r0  // CC set iff
                                  //        low_32(frac(x)) >= low_32(frac(y))
    bc        return_QNaN_x
    fmov.dd   f10,     f8         // return QNaN y
    br        restore_regs
    st.c      r16,     fsr        // restore caller's fsr

x_QNaN_y_SNaN:
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_SNaN_operand

    orh       0x0008,  r19,  r19  // Set leading frac bit of
                                  // high_32(y) = high_32(SNaN) to
                                  // produce high_32(quietized SNaN) in r19
    orh       0x0008,  r23,  r23  // Set leading frac bit of high_20(frac(y))

    subu      r23,     r21,  r0   // CC clear iff
                                  //        high_20(frac(x)) > high_20(frac(y))
    bc  check_equality

    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,    f0  // cause output source exception


check_equality:

    bte       r23,     r21,  cmp_lo_NaN_fracs


return_quietized_y:

    ixfr      r19,     f9
    fmov.ss   f10,     f8         // return quietized(SNaN y)
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,    f0  // cause output source exception


cmp_lo_NaN_fracs:
    fxfr      f8,      r20        // r20 <-- low_32(frac(x))
    fxfr      f10,     r22        // r22 <-- low_32(frac(y))
    subu      r20,     r22,   r0  // CC set iff
                                  //        low_32(frac(x)) >= low_32(frac(y))
    bnc       return_quietized_y
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,    f0  // return QNaN x and cause source exception


at_least_x_SNaN:
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_SNaN_operand
    orh       0x0008,  r17,  r17  // Set leading frac bit of
                                  // high_32(x) = high_32(SNaN x) to
                                  // produce high_32(quietized SNaN x) in r17

    andh      0x7FF0,  r19,  r0   // CC set iff y is denormal
    bc  only_x_SNaN
    
    andnoth   0xFFF0,  r19,  r23  // if CC clear, frac(y) /= 0, so y /= +/- INF
    fxfr      f10,     r22        // r22 <-- low_32(y)
    bnc  maybe_y_SNaN_too         // branch taken only if y is NaN

    btne      r22,     r0,   x_SNaN_y_SNaN
                                  // branch     taken only if y is SNaN
                                  // branch not taken only if y is +/- INF; 
only_x_SNaN:
    ixfr      r17,     f9         // quietize SNaN x
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs        // return quietized SNaN x
    fsub.ss   f9,      f9,    f0  // and cause output source exception


maybe_y_SNaN_too:
    andh      0x0008,  r19,  r0   // CC set iff most signif. bit of frac(y) = 0
                                  //        iff y is a signaling NaN
    bnc       only_x_SNaN         // branch taken iff y is a QNaN 


x_SNaN_y_SNaN:
    orh       0x0008,  r19,  r19  // Set leading frac bit of
                                  // high_32(y) = high_32(SNaN) to
                                  // produce high_32(quietized SNaN) in r19
    subu      r23,     r21,  r0   // CC clear iff
                                  //        high_20(frac(x)) > high_20(frac(y))
    bnc  only_x_SNaN

    btne      r23,     r21,  return_quietized_y

    fxfr      f8,      r20        // r20 <-- low_32(frac(x))
    fxfr      f10,     r22        // r22 <-- low_32(frac(y))
    subu      r20,     r22,   r0  // CC set iff
                                  //        low_32(frac(x)) >= low_32(frac(y))
    bnc       return_quietized_y
    ixfr      r17,     f9         // quietize SNaN x
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs        // return quietized SNaN x
    fsub.ss   f9,      f9,    f0  // and cause output source exception


at_least_x_denorm:
    fxfr      f11,     r19        // r19 <-- high_32(y)
    andh      0x7FF0,  r19,  r0   // CC set iff bexp(y) = 0 iff y = +/- denorm
    bnc  x_denorm_y_inf_or_NaN    // branch not taken iff
                                  // x is denormal and y is denormal
    shl       r0,      r19,  r21  // r21 <-- high_32(y)
    fxfr      f8,      r22        // r22 <-- low_32(frac(x))
	andnoth   0x8000,  r17,  r23  // eradicate sign bit
	bc        x_lil_denorm        // branch taken iff high_20(frac(x)) = 0

	or	      1,       r0,   r19  // pre-set the biased exponent
	shl	      12,      r23,  r23  // left justify high_20(frac(x))


x_lf_shft:                        // loop to normalize significand:
	adds	 -1,       r19,  r19  // decrement biased exponent
	andh	  0x8000,  r23,  r0   // is Most Significant Bit set?
	shl       1,       r23,  r23  // shift, making MSB implicit
	bc    x_lf_shft               // loop if implicit bit 0

x_rt_shft:                        // align normalized significand:
	shr	      12,      r23,  r23  // return high frac to proper field
	adds      31,      r19,  r24  // get count of low frac bits to stay low
	shr       r24,     r22,  r24  // rt justify low frac bits to become hi 
	or        r23,     r24,  r23  // put low frac bits in hi frac
	subs      1,       r19,  r24  // get lshift count for low frac
	shl       r24,     r22,  r22  // align low frac properly
	br	fin_norm_x_b4_norm_y      // done, but for biased_exp field
	andh      0x8000,  r17,  r17  // get sign of x

x_lil_denorm:                     // 20 most signif. frac bits are 0 
    adds     -19,      r0,   r19  // biased exponent after 20-bit lshift

x_lil_lf_shft:                    // loop to normalize significand:
    adds     -1,       r19,  r19  // decrement biased exponent
    andh      0x8000,  r22,  r0   // is Most Significant Bit set?
    shl       1,       r22,  r22  // shift, making MSB implicit
    bc   x_lil_lf_shft            // loop if implicit bit 0

x_lil_rt_shft:                    // align normalized significand:
    shr       12,      r22,  r23  // put high frac in proper field
    shl       20,      r22,  r22  // make low frac of result

fin_norm_x_b4_norm_y:
    or        r17,     r23,  r17  // put sign and hi frac in r17
    ixfr      r22,     f8         // set-up low frac of result
    shl       20,      r19,  r19  // r19 <-- bexp(normalized_x) * 2**20

    fxfr      f10,     r22        // r22 <--  low_32(frac(y))
	andnoth   0x8000,  r21,  r23  // r23 <-- high_20(frac(|y|), rt. justified
	bc        y_lil_denorm        // branch taken iff high_20(frac(y)) = 0

	or	      1,       r0,   r20  // pre-set the biased exponent
	shl	      12,      r23,  r23  // left justify high_20(frac(|y|))

y_lf_shft:                        // loop to normalize significand:
	adds	 -1,       r20,  r20  // decrement biased exponent
	andh	  0x8000,  r23,  r0   // is Most Significant Bit set?
	shl       1,       r23,  r23  // shift, making MSB implicit
	bc    y_lf_shft               // loop if implicit bit 0

y_rt_shft:                        // align normalized significand:
	shr	      12,      r23,  r23  // return high frac to proper field
	adds      31,      r20,  r24  // get count of low frac bits to stay low
	shr       r24,     r22,  r24  // rt justify low frac bits to become hi 
	or        r23,     r24,  r23  // put low frac bits in hi frac
	subs      1,       r20,  r24  // get lshift count for low frac
	shl       r24,     r22,  r22  // align low frac properly
	br	denorm_prep_reentry       // done, but for reentry preparation
	andh      0x8000,  r21,  r21  // get sign of y

y_lil_denorm:                     // 20 most signif. frac bits are 0 
    adds     -19,      r0,   r20  // biased exponent after 20-bit lshift

y_lil_lf_shft:                    // loop to normalize significand:
    adds     -1,       r20,  r20  // decrement biased exponent
    andh      0x8000,  r22,  r0   // is Most Significant Bit set?
    shl       1,       r22,  r22  // shift, making MSB implicit
    bc   y_lil_lf_shft            // loop if implicit bit 0

y_lil_rt_shft:                    // align normalized significand:
    shr       12,      r22,  r23  // put high frac in proper field
    shl       20,      r22,  r22  // make low frac of result

denorm_prep_reentry:
    or        r21,     r23,  r21  // put sign and hi frac in r21
    ixfr      r22,     f10        // set-up low frac of result
    br   dividend_not_zero
    shl       20,      r20,  r20  // r20 <-- bexp(normalized_y) * (2**20)

x_denorm_y_inf_or_NaN:
    andnoth   0xFFF0,  r19,  r23  // if CC clear, frac(y) /= 0, so y /= +/- INF
    fxfr      f10,     r22        // r22 <-- low_32(y)
    bnc  x_inf_y_NaN              // branch taken only if y is NaN

    btne      r22,     r0,   x_inf_y_SNaN
                                  // branch     taken only if y is SNaN
                                  // branch not taken only if y is +/- INF; 
    andh      0x8000,  r19,  r19  // r19.sign <-- sign(y), rest of r19 <-- 0
    andh      0x8000,  r17,  r17  // r17.sign <-- sign(y), rest of r17 <-- 0
    xor       r19,     r17,  r17  // r17.sign <-- sign(x) XOR  sign(y)
                                  // rest of r17 <-- 0
    ixfr      r17,     f9
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmov.ss   f0,      f8         // (f9:f8) <-- signed 0

only_y_invalid:                   // f8 may be any normal numeral, inc. 0
    fxfr      f11,     r19        // r19 <-- high_32(y)
    fxfr      f9,      r17        // r17 <-- high_32(x)
    andh      0x7FF0,  r19,  r0   // CC set iff bexp(y) = 0 iff y = +/- denorm
    bnc       inval_y_inf_or_NaN  // branch not taken iff
                                  // y is denormal
    shl       r0,      r19,  r21  // r21 <-- high_32(y)
    andh      0x7FF0,  r17,  r19  // r19 <-- bexp(x) * (2**20)
                                  // CC set iff bexp(x) = 0 iff x = +/- 0 here
    bnc  inval_y_denorm           // branch not taken iff x = +/- 0

dividend_is_zero:
    andh      0x8000,  r21,  r21  // r21.sign <-- y's sign, rest of r21 <-- 0
    xor       r21,     r17,  r21  // r21.sign <-- XOR of operands' signs
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    ixfr      r21,     f9         // (f9:f8) <-- 0 with XOR of operands' signs
    
inval_y_denorm:
    fxfr      f10,     r22        // r22 <--  low_32(frac(y))
	andnoth   0x8000,  r21,  r23  // r23 <-- high_20(frac(|y|), rt. justified
	bc        inval_y_lil_denorm  // branch taken iff high_20(frac(y)) = 0

	or	      1,       r0,   r20  // pre-set the biased exponent
	shl	      12,      r23,  r23  // left justify high_20(frac(|y|))

inval_y_lf_shft:                  // loop to normalize significand:
	adds	 -1,       r20,  r20  // decrement biased exponent
	andh	  0x8000,  r23,  r0   // is Most Significant Bit set?
	shl       1,       r23,  r23  // shift, making MSB implicit
	bc    inval_y_lf_shft         // loop if implicit bit 0

inval_y_rt_shft:                  // align normalized significand:
	shr	      12,      r23,  r23  // return high frac to proper field
	adds      31,      r20,  r24  // get count of low frac bits to stay low
	shr       r24,     r22,  r24  // rt justify low frac bits to become hi 
	or        r23,     r24,  r23  // put low frac bits in hi frac
	subs      1,       r20,  r24  // get lshift count for low frac
	shl       r24,     r22,  r22  // align low frac properly
	br	denor_y_prep_reentry      // done, but for reentry preparation
	andh      0x8000,  r21,  r21  // get sign of y

inval_y_lil_denorm:               // 20 most signif. frac bits are 0 
    adds     -19,      r0,   r20  // biased exponent after 20-bit lshift

inval_y_lil_lf_shft:              // loop to normalize significand:
    adds     -1,       r20,  r20  // decrement biased exponent
    andh      0x8000,  r22,  r0   // is Most Significant Bit set?
    shl       1,       r22,  r22  // shift, making MSB implicit
    bc   inval_y_lil_lf_shft      // loop if implicit bit 0

inval_y_lil_rt_shft:              // align normalized significand:
    shr       12,      r22,  r23  // put high frac in proper field
    shl       20,      r22,  r22  // make low frac of result

denor_y_prep_reentry:
    or        r21,     r23,  r21  // put sign and hi frac in r21
    ixfr      r22,     f10        // set-up low frac of result
    br   dividend_not_zero
    shl       20,      r20,  r20  // r20 <-- bexp(normalized_y) * (2**20)

                                      
inval_y_inf_or_NaN:

    andnoth   0xFFF0,  r19,  r23  // if CC clear, frac(y) /= 0, so y /= +/- INF
    fxfr      f10,     r22        // r22 <-- low_32(y)
    bnc       inval_y_NaN         // branch taken only if y is NaN
    btne      r22,     r0,   inval_y_SNaN
                                  // branch     taken only if y is SNaN
                                  // branch not taken only if y is +/- INF
    andnoth   0x7FF0,  r19,  r23  // r23 <-- 0 with sign of y
    ixfr      r23,     f11        // (f11:f10) <-- 0 with sign of y
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmul.dd   f8,      f10,   f8
    
inval_y_NaN:
    andh      0x0008,  r19,  r0   // CC set iff most signif. bit of frac(y) = 0
    bc        inval_y_SNaN        // branch not taken only if y is QNaN
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fmov.dd   f10,     f8         // return QNaN y

inval_y_SNaN:
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_SNaN_operand

    orh       0x0008,  r19,  r19  // Set leading frac bit of
                                  // high_32(y) = high_32(SNaN) to
                                  // produce high_32(quietized SNaN) in r19
    ixfr      r19,     f9         // make (f9:f8) = quietized SNaN
    fmov.ss   f10,     f8
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,   f0   // set fsr.SE
    
only_x_invalid:                   // f10 may be any normal numeral, inc. 0
    fxfr      f9,      r17        // r17 <-- high_32(x)
    fxfr      f11,     r19        // r19 <-- high_32(y)
    andh      0x7FF0,  r17,  r0   // CC set iff bexp(x) = 0 iff x = +/- denorm
    bnc       inval_x_inf_or_NaN  // branch not taken iff
                                  // y is denormal
    shl       r0,      r19,  r21  // r21 <-- high_32(y)
    andh      0x7FF0,  r19,  r20  // CC set iff bexp(y) = 0 iff y = +/- 0 here
    bnc  inval_x_denorm           // branch not taken iff y = +/- 0

only_divisor_is_zero:
    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_zero_divide

    andh      0x8000,  r17,  r17  // r17.sign <-- x's sign, rest of r17 <-- 0
    xor       r17,     r21,  r17  // r17.sign <-- x's sign XOR y's sign
    orh       0x7FF0,  r17,  r17  // r17 <-- INFINITY with XOR of
                                  //         operands' signs
    ixfr      r17,     f9         // (f9:f8) <--  correctly signed INFINITY
    fmov.ss   f10,     f8
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,   f0   // set fsr.SE

trap_zero_divide:
    orh       0x7FF0,  r21,  r21  // r19 <-- high_32(INF with y's sign)
    ixfr      r21,     f11        // (f11:f10) <-- INF with y's sign
    st.c      r16,     fsr
    br        restore_regs
    fmul.dd   f8,      f10,  f8   // cause source exception   

inval_x_denorm:
    fxfr      f8,      r22        // r22 <--  low_32(frac(x))
	andnoth   0x8000,  r17,  r23  // r23 <-- high_20(frac(|x|), rt. justified
	bc        inval_x_lil_denorm  // branch taken iff high_20(frac(x)) = 0

	or	      1,       r0,   r19  // pre-set the biased exponent
	shl	      12,      r23,  r23  // left justify high_20(frac(|x|))

inval_x_lf_shft:                  // loop to normalize significand:
	adds	 -1,       r19,  r19  // decrement biased exponent
	andh	  0x8000,  r23,  r0   // is Most Significant Bit set?
	shl       1,       r23,  r23  // shift, making MSB implicit
	bc    inval_x_lf_shft         // loop if implicit bit 0

inval_x_rt_shft:                  // align normalized significand:
	shr	      12,      r23,  r23  // return high frac to proper field
	adds      31,      r19,  r24  // get count of low frac bits to stay low
	shr       r24,     r22,  r24  // rt justify low frac bits to become hi 
	or        r23,     r24,  r23  // put low frac bits in hi frac
	subs      1,       r19,  r24  // get lshift count for low frac
	shl       r24,     r22,  r22  // align low frac properly
	br	denor_x_prep_reentry      // done, but for reentry preparation
	andh      0x8000,  r17,  r17  // get sign of x

inval_x_lil_denorm:               // 20 most signif. frac bits are 0 
    adds     -19,      r0,   r19  // biased exponent after 20-bit lshift

inval_x_lil_lf_shft:              // loop to normalize significand:
    adds     -1,       r19,  r19  // decrement biased exponent
    andh      0x8000,  r22,  r0   // is Most Significant Bit set?
    shl       1,       r22,  r22  // shift, making MSB implicit
    bc   inval_x_lil_lf_shft      // loop if implicit bit 0

inval_x_lil_rt_shft:              // align normalized significand:
    shr       12,      r22,  r23  // put high frac in proper field
    shl       20,      r22,  r22  // make low frac of result

denor_x_prep_reentry:
    or        r17,     r23,  r17  // put sign and hi frac in r17
    ixfr      r22,     f8         // set-up low frac of result
    br   dividend_not_zero
    shl       20,      r19,  r19  // r19 <-- bexp(normalized_x) * (2**20)

                                      
inval_x_inf_or_NaN:

    andnoth   0xFFF0,  r17,  r23  // if CC clear, frac(x) /= 0, so x /= +/- INF
    fxfr      f8,      r22        // r22 <-- low_32(x)
    bnc       inval_x_NaN         // branch taken only if x is NaN
    btne      r22,     r0,   inval_x_SNaN
                                  // branch     taken only if x is SNaN
                                  // branch not taken only if x is +/- INF
    andh      0x8000,  r19,  r19  // r19 <-- 0 with sign of y
    xor       r19,     r17,  r17  // r17 <-- high_32(INF) with XOR signs
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    ixfr      r17,     f9         // (f9:f8) <-- correctly signed INF
    

inval_x_NaN:

    andh      0x0008,  r17,  r0   // CC set iff most signif. bit of frac(x) = 0
    bc        inval_x_SNaN        // branch not taken only if x is QNaN
    br        restore_regs
    st.c      r16,     fsr        // restore caller's fsr


inval_x_SNaN:

    and       0x0020,  r16,  r0   // Is caller's fsr.FTE = 1 (fp traps are on)
                                  // CC set iff fsr.FTE = 0 (fp traps are off)
    bnc trap_SNaN_operand

    orh       0x0008,  r17,  r17  // Set leading frac bit of
                                  // high_32(x) = high_32(SNaN) to
                                  // produce high_32(quietized SNaN) in r17
    ixfr      r17,     f9         // make (f9:f8) = quietized SNaN
    st.c      r16,     fsr        // restore caller's fsr
    br        restore_regs
    fsub.ss   f9,      f9,   f0   // set fsr.SE
