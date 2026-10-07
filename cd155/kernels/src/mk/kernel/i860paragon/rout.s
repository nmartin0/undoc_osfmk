// 
// $Copyright
// Copyright 1993, 1994, 1995 Intel Corporation
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
        .file "rout.s"

/*
 * We are call'ed from a function compiled with "-p". Here we save some things
 * and call trace_in() "see time_trace.c".
 *
 * inputs:
 *
 *      r30 == REAL routine return-adrs
 *      r29 == adrs of PGI allocated memory area, 28 bytes.
 */
        .text
        .align  32
        .extern _trace_in
        .globl  ___rouent
___rouent:
        addu    -64,sp,sp

        st.l    r1,0(sp)        // save rtn adrs.

        st.l    r16,4(sp)
        st.l    r17,8(sp)
        st.l    r18,12(sp)
        st.l    r19,16(sp)
        st.l    r20,20(sp)
        st.l    r21,24(sp)
        st.l    r22,28(sp)
        st.l    r23,32(sp)
        st.l    r24,36(sp)
        st.l    r25,40(sp)
        st.l    r26,44(sp)
        st.l    r27,48(sp)
        st.l    r30,52(sp)      // save REAL function's return adrs.

        /* trace_in( REAL-ret-adrs, PGI mem, previous caller ) */
        or      r1,r0,r16
        or	r29,r0,r17
	or	r30,r0,r18
        call    _trace_in
	 addu	4,sp,r19

        ld.l    0(sp),r30       // restore rouent() callers return adrs.
        ld.l    4(sp),r16
        ld.l    8(sp),r17
        ld.l    12(sp),r18
        ld.l    16(sp),r19
        ld.l    20(sp),r20
        ld.l    24(sp),r21
        ld.l    28(sp),r22
        ld.l    32(sp),r23
        ld.l    36(sp),r24
        ld.l    40(sp),r25
        ld.l    44(sp),r26
        ld.l    48(sp),r27
        ld.l    52(sp),r1       // REAL function's return address

        bri     r30             // return to REAL function after _rouent() call
          addu  64,sp,sp

/*
 * called by PGI C compiler "-p" function epilogue
 * inputs:
 *
 *      r30 == REAL routine return-adrs
 */
        .globl  ___rouret
        .extern _trace_out
___rouret:
        addu    -64,sp,sp

        st.l    r1,0(sp)        // save rtn adrs.

        st.l    r16,4(sp)
        st.l    r17,8(sp)
        st.l    r18,12(sp)
        st.l    r19,16(sp)
        st.l    r20,20(sp)
        st.l    r21,24(sp)
        st.l    r22,28(sp)
        st.l    r23,32(sp)
        st.l    r24,36(sp)
        st.l    r25,40(sp)
        st.l    r26,44(sp)
        st.l    r27,48(sp)
        st.l    r30,52(sp)      // save REAL function's return adrs.

        /* trace_out( REAL-ret-adrs, return value ) */
        or      r1,r0,r16
        or	r16,r0,r17
        call    _trace_out
	  or	r30,r0,r18

        ld.l    0(sp),r30       // restore rouent() callers return adrs.
        ld.l    4(sp),r16
        ld.l    8(sp),r17
        ld.l    12(sp),r18
        ld.l    16(sp),r19
        ld.l    20(sp),r20
        ld.l    24(sp),r21
        ld.l    28(sp),r22
        ld.l    32(sp),r23
        ld.l    36(sp),r24
        ld.l    40(sp),r25
        ld.l    44(sp),r26
        ld.l    48(sp),r27
        ld.l    52(sp),r1       // REAL function's return address
        bri     r30             // return to REAL function after _rouent() call
          addu  64,sp,sp


        .globl  __trace_in_calibrate
__trace_in_calibrate:
	.data
	.align  8
PGI_PROFENT1:
	.long 0,0,0,0,0,0,0
	.long __trace_in_calibrate,0,0
	.text
	mov r1, r30
	orh h%PGI_PROFENT1, r0, r29
	call ___rouent
	or l%PGI_PROFENT1, r29, r29
	bri r1
	nop

        .globl  __trace_out_calibrate
__trace_out_calibrate:
	mov r1, r30
	call ___rouret
        nop
	bri r1
	nop
