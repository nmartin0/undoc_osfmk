/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
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
/*
 * HISTORY
 * $Log: user.s,v $
 * Revision 2.12  1994/11/18  20:40:00  mtm
 * Copyright additions/changes
 *
 * Revision 2.11  1993/06/30  22:31:48  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.10  1993/05/17  22:58:10  andyp
 * Thanks to Koichi Yamada <koichi@gomez.intel.com>, copyin() and copyout()
 * now look a lot like the kernel bcopy() -- it batches and pipelines.
 *
 * Revision 2.9  1992/12/15  00:38:40  andyp
 * Cosmetic changes to bcopy() and eliminated an instruction
 * or two for 4-byte aligned case.
 * bzero() now will zero in 16, 8, 4, 2, or 1 byte chunks
 * copyin() and copyout() are chunkier now, too.
 * XXX bcopy(), copyin(), and copyout() may still benefit
 * XXX from batching the 4- and 8-byte cases as will as
 * XXX pipelining the 4, 8, and 16 byte cases.
 *
 * Revision 2.8  1992/12/02  19:48:00  stans
 * mo better way to get the current thread address (active_threads[]).
 *
 * Revision 2.7  1992/09/24  22:53:10  stans
 * for NCPUS > 1 include  i860/cpu_number.h
 *
 * Revision 2.6  1992/09/24  17:03:33  stans
 * use genasm labels to replace call to "C" code for setting thread->recover
 *
 * Revision 2.4.2.2  92/05/27  00:42:43  jeffreyh
 * 	Support copyin() fault recovery now returns to caller an error code.
 * 	[stans@ssd.intel.com]
 * 
 * Revision 2.4.2.1  92/04/08  15:43:42  jeffreyh
 * 	Reorder to prevent a stall; cosmetics
 * 	[92/04/08            andyp]
 * 
 * Revision 2.4  91/06/18  20:52:11  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:00:26  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:30  jsb
 * 	Changes to copyin, copyout. Also added copy{in,out}msg aliases.
 * 	[91/06/17  10:30:42  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:48  jsb
 * 	First checkin.
 * 	[90/12/03  21:35:52  jsb]
 * 
 */
#include <cpus.h>
#include "assym.s"

#if	NCPUS > 1
#include <i860/cpu_number.h>
#endif	NCPUS > 1

#define	RECOVERY 1


//
// copyin(from,to,nbytes)
//
// Copy bytes from user space to kernel space
//
// inputs:
//	(r16) from	user-space source address
//	(r17) to	kernel-space destination address
//	(r18) nbytes	number of bytes to copy in from user-space..
// outputs:
//	r16	0 == success, otherwise ERROR code.
//
// Make a few quick checks for the alignment restrictions in force,
// then copy bytes using the largest chunks possible.
//
//
_copyin::
_ALLOW_FAULT_START::		// kernel fault recovery starting here.
_copyinmsg::
	mov	r1,r30		// save return address
	//check specified user memory block is in valid user space
	orh	h%VM_MIN_KERNEL_ADDRESS,r0,r19
	or	l%VM_MIN_KERNEL_ADDRESS,r19,r19
	addu	r16,r18,r20	/* compute end address: start + size */
	subu	r20,r19,r0	/* if VM_MIN_KERNEL_ADDRESS > end ; CC is clr */
	bc.t	.cpcdone
	  subu	1,r0,r16

	// common code shared between copyin() & copyout()
.copycom:

#if RECOVERY
	// enable kernel page-fault recovery on user page access.
	// set current_thread()->recover = &_FAULT_ERROR.
#if     NCPUS > 1
	FAST_CPU_NUMBER(r20)
	shl	2,r20,r20       /* convert cpu # to longword offset */
	orh	ha%_active_threads,r20,r20
	ld.l	l%_active_threads(r20),r21	/* r21 == current_thread() */
#else	NCPUS > 1
	orh	ha%_active_threads,r0,r21
	ld.l    l%_active_threads(r21),r21      /* current_thread() */
#endif	NCPUS > 1
	// r21 == current_thread()
	bte	r0,r21,9f			/* null thread? skip recover */
	orh	h%_FAULT_ERROR,r0,r20
	or	l%_FAULT_ERROR,r20,r20
	st.l	r20,THREAD_RECOVER(r21)
9:
#endif	RECOVERY
	or	r0,r18,r0
	bc.t	.cpc_bye
	  mov	r0,r16
	adds	-1,r0,r19		// inc = -1
	or	r16,r17,r31		// tmp = src | dst
	or	r18,r31,r31		// tmp |= cnt
	and	0x000f,r31,r0		// 16-byte aligned?
	bc.t	.cc16prep		//	copy 16-byte chunks,
	 shr	4,r18,r18		// 	cnt = cnt / 16
	and	0x0007,r31,r0		// 8-byte aligned?
	bc.t	.cc8prep		//	copy 8-byte chunks,
	 shr	3,r18,r18		// 	cnt = cnt / 8
	and	0x0003,r31,r0		// 4-byte aligned?
	bc.t	.cc4prep		//	copy 4-byte chunks,
	 shr	2,r18,r18		// 	cnt = cnt / 4
	and	0x0001,r31,r0		// 2-byte aligned?
	bc.t	.cc2prep		//	copy 2-byte chunks,
	 shr	1,r18,r18		// 	cnt = cnt / 2

	// copy bytes...
.cc1prep:
	adds	-1,r18,r18		// cnt -= 1
	bla	r19,r18,.cc1
	 nop
.cc1:
	ld.b	0(r16),r20
	addu	1,r16,r16
	st.b	r20,0(r17)		// slow if dcache miss...
	bla	r19,r18,.cc1
	 addu	1,r17,r17
	br	.cpc_bye
	 nop

	// copy shorts...
.cc2prep:
	adds	-1,r18,r18		// cnt -= 1
	bla	r19,r18,.cc2
	 nop
.cc2:
	ld.s	0(r16),r20
	addu	2,r16,r16
	st.s	r20,0(r17)		// slow if dcache miss...
	bla	r19,r18,.cc2
	 addu	2,r17,r17
	br	.cpc_bye
	 nop

	// copy longs...
.cc4prep:
	// one word lower (autopreincrement)
	addu	-4,r16,r16
	addu	-4,r17,r17

	// pipeline only when cnt >= 3 words
	addu	-3,r18,r0
	bnc	.small4		// if (cnt < 3) goto .small4;

	// prime the load pipe
	adds	-3,r18,r18	// count the 3 words in the pipe
	pfld.l	4(r16)++,f16	// ignore value returned
	adds	-16,r0,r19	// inc = -16
	pfld.l	4(r16)++,f20	// ignore value returned
	mov	r18,r20		// loop counter is r20
	bla	r19,r20,.primed4
	 pfld.l	4(r16)++,f24	// ignore value returned
.primed4:
	// pump the pipeline if at least 16 words left
	bla	r19,r20,.pump4	// taken if at least 16 words left
	 nop
	// can't batch if less than 16 words left, drain the pipe.
	br	.drain4
	 nop

	.align	32
.pump4:
	// batch read 16, then batch write 16 (64 bytes moved per loop)
	pfld.l	4(r16)++,f16
	pfld.l	4(r16)++,f17
	pfld.l	4(r16)++,f18
	pfld.l	4(r16)++,f19
	pfld.l	4(r16)++,f20
	pfld.l	4(r16)++,f21
	pfld.l	4(r16)++,f22
	pfld.l	4(r16)++,f23
	pfld.l	4(r16)++,f24
	pfld.l	4(r16)++,f25
	pfld.l	4(r16)++,f26
	pfld.l	4(r16)++,f27
	pfld.l	4(r16)++,f28
	pfld.l	4(r16)++,f29
	pfld.l	4(r16)++,f30
	pfld.l	4(r16)++,f31
	fst.l	f16,4(r17)++
	fst.l	f17,4(r17)++
	fst.l	f18,4(r17)++
	fst.l	f19,4(r17)++
	fst.l	f20,4(r17)++
	fst.l	f21,4(r17)++
	fst.l	f22,4(r17)++
	fst.l	f23,4(r17)++
	fst.l	f24,4(r17)++
	fst.l	f25,4(r17)++
	fst.l	f26,4(r17)++
	fst.l	f27,4(r17)++
	fst.l	f28,4(r17)++
	fst.l	f29,4(r17)++
	fst.l	f30,4(r17)++
	fst.l	f31,4(r17)++
	bla	r19,r20,.pump4
	 adds	-16,r18,r18	// moved 16 words
.drain4:
	// drain 3 remaining words in the pipe
	pfld.l	0(r16),f16
	pfld.l	0(r16),f17
	pfld.l	0(r16),f18
	fst.l	f16,4(r17)++
	fst.l	f17,4(r17)++
	fst.l	f18,4(r17)++
	bte	0,r18,.ret4

	// 1 <= cnt < 16 words left
.small4:
	adds	-1,r0,r19	// inc = -1
	adds	-1,r18,r18	// cnt -= 1
	bla	r19,r18,.drip4
	 nop
.drip4:
	fld.l	4(r16)++,f16
	bla	r19,r18,.drip4
	 fst.l	f16,4(r17)++
.ret4:
	br	.cpc_bye
	 nop


	// copy doubles...
.cc8prep:
#if	!defined(i860XP)
.xr:
#endif	!defined(i860XP)
	// one double lower (autopreincrement)
	addu	-8,r16,r16
	addu	-8,r17,r17

	// pipeline only when cnt >= 3 doubles
	addu	-3,r18,r0
	bnc	.small8		// if (cnt < 3) goto .small8;

	// prime the load pipe
	adds	-3,r18,r18	// count the 3 doubles in the pipe
	pfld.d	8(r16)++,f16	// ignore value returned
	adds	-8,r0,r19	// inc = -8
	pfld.d	8(r16)++,f20	// ignore value returned
	mov	r18,r20		// loop counter is r20
	bla	r19,r20,.primed8
	 pfld.d	8(r16)++,f24	// ignore value returned
.primed8:
	// pump the pipeline if at least 8 doubles left
	bla	r19,r20,.pump8	// taken if at least 8 doubles left
	 nop
	// can't batch if less than 8 doubles left, drain the pipe.
	br	.drain8
	 nop

	.align	32
.pump8:
	// batch read 8, then batch write 8 (64 bytes moved per loop)
	pfld.d	8(r16)++,f16
	pfld.d	8(r16)++,f18
	pfld.d	8(r16)++,f20
	pfld.d	8(r16)++,f22
	pfld.d	8(r16)++,f24
	pfld.d	8(r16)++,f26
	pfld.d	8(r16)++,f28
	pfld.d	8(r16)++,f30
	fst.d	f16,8(r17)++
	fst.d	f18,8(r17)++
	fst.d	f20,8(r17)++
	fst.d	f22,8(r17)++
	fst.d	f24,8(r17)++
	fst.d	f26,8(r17)++
	fst.d	f28,8(r17)++
	fst.d	f30,8(r17)++
	bla	r19,r20,.pump8
	 adds	-8,r18,r18	// moved 8 doubles
.drain8:
	// drain 3 remaining doubles in the pipe
	pfld.d	0(r16),f16
	pfld.d	0(r16),f18
	pfld.d	0(r16),f20
	fst.d	f16,8(r17)++
	fst.d	f18,8(r17)++
	fst.d	f20,8(r17)++
	bte	0,r18,.ret8

	// 1 <= cnt < 8 doubles left
.small8:
	adds	-1,r0,r19	// inc = -1
	adds	-1,r18,r18	// cnt -= 1
	bla	r19,r18,.drip8
	 nop
.drip8:
	fld.d	8(r16)++,f16
	bla	r19,r18,.drip8
	 fst.d	f16,8(r17)++
.ret8:	
	br	.cpc_bye
	 nop

	// copy quads...
.cc16prep:

#if	!defined(i860XP)
	//
	//	don't try to pfld.q on an i860XR
	//
	ld.c	epsr,r31
	and	0x0002,r31,r0
	bc.t	.xr
	 shl	1,r18,r18	// correct r18 for 8-byte transfers
#endif	!defined(i860XP)

	// one quad lower (autopreincrement)
	addu	-16,r16,r16
	addu	-16,r17,r17

	// pipeline only when cnt >= 3 quads
	addu	-3,r18,r0
	bnc	.small16		// if (cnt < 3) goto .small16;

	// prime the load pipe
	adds	-3,r18,r18	// count the 3 quads in the pipe
	adds	-4,r0,r19	// inc = -4
	mov	r18,r20		// loop counter is r20
	pfld.q	16(r16)++,f16	// ignore value returned
	pfld.q	16(r16)++,f20	// ignore value returned
	bla	r19,r20,.cc16prime
	 pfld.q	16(r16)++,f24	// ignore value returned
.cc16prime:
	bla	r19,r20,.cc16		// taken if >= 4
	 nop
	br	.cc16drain		// less than 4, move 1 at a time
	 nop
	
	.align	32
.cc16:
	pfld.q	16(r16)++,f16		// batch read 4 quads
	pfld.q	16(r16)++,f20
	pfld.q	16(r16)++,f24
	pfld.q	16(r16)++,f28
	fst.q	f16,16(r17)++		// batch write 4 quads
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	fst.q	f28,16(r17)++
	bla	r19,r20,.cc16
	 adds	-4,r18,r18
.cc16drain:
	// drain 3 remaining quads in the pipe
	pfld.d	0(r16),f16	// drain w/ doubles in the pipe
	pfld.d	0(r16),f20	// drain w/ doubles in the pipe
	pfld.d	0(r16),f24	// drain w/ doubles in the pipe
	fst.q	f16,16(r17)++
	fst.q	f20,16(r17)++
	fst.q	f24,16(r17)++
	bte	0,r18,.cpc_bye

.small16:
	adds	-1,r0,r19	// inc = -1
	adds	-1,r18,r18	// cnt -= 1
	bla	r19,r18,.drip16
	 nop
.drip16:
	fld.q	16(r16)++,f16
	bla	r19,r18,.drip16
	 fst.q	f16,16(r17)++
.cpc_bye:

#if	RECOVERY
	// clear current_thread()->recover
#if	NCPUS > 1
	FAST_CPU_NUMBER(r20)
	shl	2,r20,r20       /* convert cpu # to longword offset */
	orh	ha%_active_threads,r20,r20
	ld.l	l%_active_threads(r20),r21	/* r21 == current_thread() */
#else	NCPUS > 1
	orh	ha%_active_threads,r0,r21
	ld.l    l%_active_threads(r21),r21      /* current_thread() */
#endif	NCPUS > 1
	// r21 == current_thread()
	bte	r0,r21,8f			/* null thread? skip recover */
	st.l	r0,THREAD_RECOVER(r21)
8:
#endif	RECOVERY
	mov	r0,r16

	// r16 == return code
.cpcdone:
	bri	r30			// return success to caller.
	 nop

//
//	If a paging fault happens in copycom, control is returned here.
//
// inputs:
//	r30	copy{in/out} return address
// outputs:
//	r16	-1	error indicator.
//
_FAULT_ERROR::
	bri	r30
	 subu	1,r0,r16


//
// copyout(from,to,nbytes)
//
// Copy bytes from kernel space to user space
//
// inputs:
//	r16	from kernel adrs
//	r17	to user adrs
//	r18	byte count
// output:
//	r16	0 == success, otherwise ERROR
//
_copyout::
_copyoutmsg::
	mov	r1,r30		// save return address

	//verify specified user memory block is in valid user space
	orh	h%VM_MIN_KERNEL_ADDRESS,r0,r29
	or	l%VM_MIN_KERNEL_ADDRESS,r29,r29
	addu	r17,r18,r31     /* compute end address: start + size */
	subu	r31,r29,r0      /* if VM_MIN_KERNEL_ADDRESS > end ; CC is clr */
	bc.t	.cpcdone
	  subu	1,r0,r16

	br	.copycom
	  nop

_ALLOW_FAULT_END::
