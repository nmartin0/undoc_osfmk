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
 *	$Id: misc.s,v 1.11 1994/11/18 20:39:11 mtm Exp $
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
 * $Log: misc.s,v $
 * Revision 1.11  1994/11/18  20:39:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/12  19:18:38  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/06/15  19:41:28  lenb
 *         cleanup: delete dead code in spl_enable() & splon()
 *
 * Revision 1.8  1994/05/27  22:14:39  flb
 *  Reviewer: Terry
 *  Risk: low
 *  Benefit or PTS #: 9612  panic: Misaligned data access trap
 *  Testing: message & misc EATS, system & parallel sats
 *  Module(s): misc.s
 *  Description:
 * 	i860_callstack modified to prevent following
 * 	non-aligned frame pointer or recording non-aligned
 * 	return addresses.
 *
 * Revision 1.7  1994/05/25  22:17:04  flb
 *  Reviewer: Terry
 *  Risk: low
 *  Benefit or PTS #:8433 (pmap_expand panic)
 *  Testing:
 * 	Tested and passed "exec.run", the test routine referenced in PTS# 8433.
 * 	Tested and passed message & misc EATS.
 * 	Tested and passed parallel & system sats.
 *  Module(s): misc.s
 *  Description:
 * 	Modified the i860_callback routine to prevent access to
 * 	non-kernel memory (the source of this panic) and to eliminate
 * 	the need for the near-duplicated routine i860_callstack.
 *
 * 	i860_callback was modified to implement the following features:
 * 		Only valid kernel return addresses are recorded.
 * 		Only valid kernel frame pointers are followed.
 * 		If the trace-back buffer is not completely filled, the last
 * 		recorded return address is followed by a NULL address (0x00000000).
 * 		Note: A valid kernel address is 0xF0000000 through 0xFFFFFFFF.
 *
 * Revision 1.6  1994/05/18  21:16:31  flb
 *  Reviewer: flb
 *  Risk: low
 *  Benefit or PTS #: improved ability to debug zone_checkit panics
 *  Testing: sats, eats
 *  Module(s): misc.s
 *  Desription:	added i860_callback, Similar to i860_callstack,
 * 		but only traces the kernel stack.
 *
 * Revision 1.5  1993/12/14  22:58:03  steved
 * Redesigned the trap handler to jumps greater than 128Meg could
 * happen. Added a panic if an slock timed out.
 *
 * Revision 1.4  1993/09/28  17:59:36  andyp
 * Update for R1.2 release.
 *
 *
 *	Trace kernel call stack back to user-land, dumping
 *	the results in a caller-supplied buffer.  [andyp@ssd.intel.com]
 *
 * Revision 1.3  1993/07/29  05:22:08  andyp
 * Added some extra stack bounds assertions for tail recursion debugging.
 * Debug kernel now uses the same mechanism for tail recursion as
 * the optimized version of the kernel.
 * Also -- modified delay() to use a deterministic two-clock spin loop
 * (see i860/misc.s and i860paragon/model_dep.c).
 *
 * Revision 1.2  1993/06/30  22:31:07  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/08/26  15:34:44  stans
 * Initial revision
 *
 * Revision 2.2.1.1  92/05/27  00:40:46  jeffreyh
 * 	Added get_rpc
 * 	[92/05/26            bernard]
 * 
 * Revision 2.2  91/12/10  16:30:49  jsb
 * 	New files from Intel
 * 	[91/12/10  16:11:30  jsb]
 * 
 * $Id: misc.s,v 1.11 1994/11/18 20:39:11 mtm Exp $ 
 */

#include <machine/psl.h>

	.file	"misc.s"
	.text
	.align	4
//
//	routines to get various control registers
//
_get_psr::
	bri	r1
	 ld.c	psr,r16

_get_dirbase::
	bri	r1
	 ld.c	dirbase,r16

_get_db::
	bri	r1
	 ld.c	db,r16

_get_epsr::
	bri	r1
	 ld.c	epsr,r16

_get_sp::
	bri	r1
	 mov	sp,r16

_get_fp::
	bri	r1
	 mov	fp,r16

//
//	clear the IM bit of the psr and return the previous value
//
_sploff::
	ld.c	psr,r17		// r17 = psr
	andnot	PSR_IM,r17,r16	// r16 = ~PSR_IM & r17  --- clear it
	st.c	r16,psr		// psr = r16
	bri	r1
	 and	PSR_IM,r17,r16	// return old IM bit

//
//	conditionally enable the IM bit of the psr depending on parameter.
//
_splon::
	and	PSR_IM,r16,r16	// r16 &= PSR_IM
	ld.c	psr,r17		// r17 = psr
	andnot	PSR_IM,r17,r17	// r17 &= ~PSR_IM  --- clear it
	or	r16,r17,r17	// r17 |= r16	   --- enable it...maybe
	st.c	r17,psr		// psr = r17
splon_return:
	bri	r1
	 and	PSR_IM,r17,r16

//
//	read_no_cache(adr)
//
//	read from adr without caching in data cache
//
_read_no_cache::
	pfld.d	0(r16), f8	// protect f2, f3 from unexpected quads
	pfld.l	0(sp), f8
	pfld.l	0(sp), f8
	pfld.l	0(sp), f16
	bri	r1
	 fxfr	f16, r16

//
//	pfldd(double *src, double *dst)
//
//	Read one 64-bit word w/o cacheing, and write it
//	to the destination.
//
_pfldd::
	pfld.d	0(r16), f8
	pfld.d	0(sp), f8
	pfld.d	0(sp), f8
	pfld.d	0(sp), f16
	bri	r1
	 fst.d	f16, 0(r17)

//
//	gimmeabreak()
//
//	Invoke the debugger.
//
_gimmeabreak::
	trap	r0,r0,r0
	bri	r1
	 nop
	nop

//
//      get_rpc()
//
//      Return callers return pc
//
_get_rpc::
        bri     r1
        ld.l      4(fp),r16
        nop

//
//	ffs(arg)
//	
//	Return cardinal count of first bit set, moving from LSB.  0
//	indicates an error.
//
_ffs::
	mov	r0,r17
	bte	r0,r16,ffs_done		// watch for zero value
ffs_loop:
	addu	1,r17,r17
	and	1,r16,r0
	bc.t	ffs_loop
	 shr	1,r16,r16
ffs_done:
	bri	r1
	 mov	r17,r16


//
//	void _spin(iterations)
//	unsigned int iterations;
//
//	Spin in a tight loop for the specified number of iterations.
//	Each iteration is 2 clock cycles plus 6 clocks for setup
//	and return.
//
__spin::
	adds	-1,r0,r19
	adds	-1,r16,r16
	bla	r19,r16,.spin
	 nop
.spin:
	bla	r19,r16,.spin
	 nop
	bri	r1
	 nop


//
//	i860_callback(buf, cnt)
//	int	*buf, cnt;
//
//	Record up to "cnt" return addresses from the current stack in "buf."
//	Only records return addresses and follow frame pointers
//	that are in kernel space (addresses that match 0xf???????).
//	For stack traces less than "cnt", a 0x0 is placed in "buf".
//
_i860_callstack::
	mov	fp,r18
.walk1:
	bte	r0,r17,.deadend1	// max items recorded?
	bte	r0,r18,.clearend	// dead-end of frame?

	and	0x3,r18,r19	// only follow frames w/ aligned address
	btne	0x0,r19,.clearend

	shr	28,r18,r19	// only follow frames w/addr 0xf???????
	btne	0xf,r19,.clearend

	ld.l	0(r18),r20	// old fp
	ld.l	4(r18),r21	// return address

	shr	28,r21,r19	// only capture addresses of 0xf???????
	btne	0xf,r19,.clearend
 
	and	0x3,r21,r19	// only capture if aligned address
	btne	0x0,r19,.clearend

	addu	-1,r17,r17	// decrement the count
	st.l	r21,0(r16)	// store the r1 from the frame
	addu	4,r16,r16	// increment the buf pointer
 
	br	.walk1
	 mov	r20,r18		// set up for next frame

.clearend:
	st.l	r0,0(r16)	// indicate end of trace buffer
//	fall-through
.deadend1:
	bri	r1
	 nop
