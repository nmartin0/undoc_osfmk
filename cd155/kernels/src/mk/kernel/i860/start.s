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
 * $Log: start.s,v $
 * Revision 2.16  1994/11/18  20:39:35  mtm
 * Copyright additions/changes
 *
 * Revision 2.15  1994/07/12  19:18:45  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 2.14.2.2  1994/02/10  23:02:30  andyp
 * Removed the need for intel_read_fault(); now support the use of
 * the WP bit in the epsr to trap read-only kernel mappings.
 *
 * Revision 2.14.2.1  1994/01/12  19:38:03  andyp
 * Simplified the i860XP flush_tlb() and the i860xp_flush_and_ctxsw()
 * routines.
 *
 * Revision 2.14  1993/12/14  22:58:07  steved
 * Redesigned the trap handler to jumps greater than 128Meg could
 * happen. Added a panic if an slock timed out.
 *
 * Revision 2.13  1993/06/30  22:31:27  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.12  1992/11/16  18:46:50  andyp
 * Reduced the bootstrap stack from 16KB to 4KB for
 * i860XP.  Still needs to be checked on the iPSC/860.
 *
 * Revision 2.11  1992/11/12  02:26:33  stans
 * moved clearing of "bear" register to after the initialization of the
 * exception handling trampoline code. Covers the case if clearing the "bear"
 * releases an exception we are in a state where it can be handled.
 *
 * Revision 2.10  1992/09/30  23:29:58  andyp
 * Unlatch "bear" to enable parity and bus error traps.
 *
 * Revision 2.9  1992/09/30  23:27:00  andyp
 * Corrected the comment leader string and the log.
 *
 * Revision 2.8  1992/09/30  23:25:31  andyp
 * Set the DRAM pagesize field of dirbase for Paragons.
 *
 * Revision 2.7  1992/09/26  21:34:43  SSD
 * OSF norma13 version.
 *
 * Revision 2.6.2.5  92/09/15  17:20:08  jeffreyh
 * 	Paragon and NCPUS > 1 work.
 * 	[92/09/10            andyp@ssd.intel.com]
 * 
 * Revision 2.6.2.4  92/05/28  18:17:54  jeffreyh
 * 	Conditionaliezed out new flush code. It does not
 * 	work with versions of the i860XR berfore C1.
 * 
 * Revision 2.6.2.3  92/05/27  00:41:44  jeffreyh
 * 	Long-standing linker weirdness resolved; the code was wrong.
 * 	Initial Paragon support including startup code, and a data
 * 	cache flush routine suitable for an i860XP.  Sometime later
 * 	we'll remove the static allocation of both the flush area
 * 	and the initial kernel stack space.
 * 	Changed "ha%" --> "h%" to fix Greg R. link problems.
 * 	[andyp@ssd.intel.com]
 * 
 * Revision 2.6.2.2  92/03/28  10:08:14  jeffreyh
 * 	kdpe_page is now allocated in pmax.c
 * 	[92/03/20            andyp]
 * 
 * Revision 2.6.2.1  92/02/18  19:00:51  jeffreyh
 * 	[andyp] take better care of interrupt stack, added message to asm panic,
 * 	track global mmu data structures.
 * 	[92/02/13  12:41:35  jeffreyh]
 * 
 * Revision 2.6  91/12/10  16:31:03  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:56  jsb]
 * 
 * Revision 2.5  91/08/28  11:12:25  jsb
 * 	From Intel SSDL: always come up single user; forcibly turn interrupts
 * 	off in thread_bootstrap.
 * 	[91/08/26  16:32:10  jsb]
 * 
 * Revision 2.4  91/06/18  20:51:48  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:58:42  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:03  jsb
 * 	Use r26 in place of r20.
 * 	[91/06/17  10:15:36  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:22  jsb
 * 	First checkin.
 * 	[90/12/03  21:33:38  jsb]
 * 
 */

#include <cpus.h>

	.file	"start.s"
	.text
_stext::
pstart::
	st.c	r0,psr
	or	0x4000,r0,r31		// Write-Protect mode
	st.c	r31,epsr
#if	PARAGON860
	st.c	r0,fsr
	st.c	r0,dirbase
	mov	r0,fp
	// privileged regs p0...p3 be a good citizen Mr. Cane....
	st.c	r0,p0
	st.c	r0,p1
	st.c	r0,p2
	st.c	r0,p3
#endif	PARAGON860
	orh	h%_boot_stack_hi,r0,r31		// Set stack pointer
	or	l%_boot_stack_hi,r31,sp

//
//	Here we store a set of instructions at a special address (V 0xFFFFFF00)
//	which the i860 jumps to (automatically) whenever a trap or interrupt
//	occurs. Basically 8 nops followed by a branch to the trap handler
//	(alltraps()). It is important to map this Virtual address carefully
//	as otherwise strange and unwonderful things will happen! 
//
//	Equivalent functionality is duplicated in pmap_bootstrap(); doing it
//	here makes it possible to take a trap (ie, use the kernel debugger)
//	before pmap_bootstrap() has been run.
//
	orh	0xffff,r0,r26		// Form a relative branch instruction.
	or	0xff00,r26,r26		// (V 0xFFFFFF00 - V alltraps())

#if     i860XP
 
        // This is the new code to allow a branch to the trap handler.
        // It uses the 'bri' instruction to branch anywhere in memory.
        // The older method using a 'br' instruction limited a branch
        // to 128meg.
        orh     0x3920,r0,r29           //
        or      0x6800,r29,r29          //
        st.l    r29,0(r26)              // st.c r13, p1
        orh     0x3940,r0,r29           //
        or      0x7000,r29,r29          //
        st.l    r29,4(r26)              // st.c r14, p2
        orh     0x3960,r0,r29           //
        or      0x7800,r29,r29          //
        st.l    r29,8(r26)              // st.c r15, p3
        orh     0x302F,r0,r29           //
        or      0x0000,r29,r29          //
        st.l    r29,12(r26)             // ld.c psr, r15
        orh     0x3820,r0,r29           //
        or      0x0000,r29,r29          //
        st.l    r29,16(r26)             // st.c r0, psr
        orh     0xEC0D,r0,r29           //
        or      h%_alltraps,r29,r29     //
        st.l    r29,20(r26)             // orh h%_alltraps, r0, r13
        orh     0xE5AD,r0,r29           //
        or      l%_alltraps,r29,r29     //
        st.l    r29,24(r26)             // or l%_alltraps, r13, r13
        orh     0x4000,r0,r29           //
        or      0x6800,r29,r29          //
        st.l    r29,28(r26)             // bri r13
        orh     0xa000,r0,r29           //
        st.l    r29,32(r26)             // nop
 
	// This is the old trap handler. It's limited to a branch of no more
	// than 128 Meg.
#else
        orh     h%_alltraps,r0,r27      // The address of _alltraps is
        or      l%_alltraps,r27,r27     // now in r27
 
        subu    r27,r26,r28             //      here is the offset
 
        shr     2,r28,r28               // **   CHIP BUG WORKAROUND
        adds    -9,r28,r28              // ** adust offset for 8 NOPS, +1
 
        andnoth 0xfc00,r28,r28          // form br alltraps
        orh     0x6800,r28,r28          //
 
        orh     0xa000,r0,r29           // ** form nop
        st.l    r29,0(r26)              // ** chip bug NOP
        st.l    r29,4(r26)              // ** chip bug NOP
        st.l    r29,8(r26)              // ** chip bug NOP
        st.l    r29,12(r26)             // ** chip bug NOP
        st.l    r29,16(r26)             // ** chip bug NOP
        st.l    r29,20(r26)             // ** chip bug NOP
        st.l    r29,24(r26)             // ** chip bug NOP
        st.l    r29,28(r26)             // ** chip bug NOP
        st.l    r28,32(r26)             // br alltraps
        st.l    r29,36(r26)             // delayed NOP
        st.l    r29,40(r26)             // delayed NOP
#endif


#if	PARAGON860
	nop
	nop
	// unlatch bear to catch future parity or bus errors...
	// moved code here to cover the case if we do get an exception we are
	// ready to handle it.
	//
	ld.c	bear,r0
	nop
	nop
#endif

#if	iPSC860
	/*
	 *  arg1 = basemem
	 *  arg2 = physnode
	 *  arg3 = slot
	 *  arg4 = boothow
	 *  arg5 = end_of_sym
	 *  arg6 = bootenv
	 *  arg7 = entry
	 */
	
	addu	2,r0,r19	// Come up in single user for now.
				// XXX cfj
#endif	iPSC860
#if	PARAGON860
	/*
	 *  arg1(r16) = end_of_symbol table
	 *  arg2(r17) = ram disk address
	 *  arg3(r18) = size of ram disk (0 if not loaded)
	 *  arg4(r19) = environment string adrs
	 *  arg5(r20) = server start adrs
	 *  arg6(r21) = server size
	 *  arg5(r20) = emulator start adrs
	 *  arg6(r21) = emulator size
	 */
#endif	PARAGON860
	call	_machine_startup	// run C code
	 nop

.tight::	br	.tight
	 nop

////////////////////////////////////////////////////////////////////////
//
// _flush
//   flush the caches of the cpu remaining in the current process.
//   _flush was put here in order to insure that the code that changes
//   the dirbase and the following 6 nop's are within the same page.
//
// _flush_and_ctxsw
//   flush the caches of the cpu and change the dirbase.  At entry, r16
//   contains the new dirbase.
//
// _flush_tlb
//  flush TLB and instruction cache only.
//
//	Register dependencies:
//
//	r30	- dirbase temporary
//	r29	- bla loop increment
//	r28	- flush data pointer
//	r27	- bla loop count
//	r26	- holds return address for the three entry points
//	r25	- dirbase with RB and RC cleared
//	r24	- epsr temporary (to check size of dcache)
//	r23	- FTE bit at entry
//	r22	- fsr temporary
//	r21	- IM bit at entry
//	r20	- psr temporary
//	r19	- flush area
//	r18	-
//	r17	-
//	r16	- new dirbase (for flush_and_ctxsw())
//
////////////////////////////////////////////////////////////////////////
//
//	dram page sizes:
//
//	 4KB == 1<<12, DPS = 0
//	 8KB == 1<<13, DPS = 1
//	16KB == 1<<14, DPS = 2
//	32KB == 1<<15, DPS = 3
//
#if	i860XP
/*
 * XXX
 * XXX The code below will not run on a iPSC with i860's below rev C1. 
 * XXX
 */
#if	PARAGON860
#define	DPS	2
#endif	PARAGON860
#if	iPSC860
#define	DPS	0
#endif	iPSC860

#define	FLUSH_STATS	1

_i860_reboot::
	mov	r16,r1		// return will go to the bootstrap
	mov	r17,sp		// get onto the bootstrap stack
	mov	r0,r16		// zap dirbase; disable ATE
	br	.do_flush	// pull the chain
	 mov	r16,r30		// set up for calling flush()

_i860xp_flush_and_ctxsw::
#if	FLUSH_STATS
	orh	h%_flush_stats_switch,r0,r30
	or	l%_flush_stats_switch,r30,r30
	ld.l	0(r30),r31
	addu	1,r31,r31
	st.l	r31,0(r30)
#endif	/* FLUSH_STATS */
	andnot	0x0fff,r16,r16	// clear the low 12 bits of the new dirbase
	or	0x21,r16,r16	// set ITI and ATE 
	or	DPS<<1,r16,r16	// DPS - used by cpu to assert next_near
	bri	r1		// return
	 st.c	r16,dirbase	//  and switch in the shadow

	/* -=- */

_flush_tlb::
#if	FLUSH_STATS
	orh	h%_flush_stats_tlb,r0,r30
	or	l%_flush_stats_tlb,r30,r30
	ld.l	0(r30),r31
	addu	1,r31,r31
	st.l	r31,0(r30)
#endif	/* FLUSH_STATS */
	ld.c	dirbase,r16	// get current dirbase
	or      0x20,r16,r16    // set ITI
	bri	r1		// return
	 st.c    r16,dirbase	//  and invalidate in the shadow

	/* -=- */

_flush_and_ctxsw::
	andnot	0x0fff,r16,r16
	or	0x01,r16,r16	// ATE - address translation enable
	or	DPS<<1,r16,r16	// DPS - used by cpu to assert next_near
	br      .do_flush
	 ld.c    dirbase,r30
_flush::
#if	FLUSH_STATS
	orh	h%_flush_stats_flush,r0,r30
	or	l%_flush_stats_flush,r30,r30
	ld.l	0(r30),r31
	addu	1,r31,r31
	st.l	r31,0(r30)
#endif	/* FLUSH_STATS */
	ld.c    dirbase,r30
	mov     r30,r16
.do_flush:
	ld.c    psr,r20         // disable interrupts
	and     0x0010,r20,r21  //
	andnot  0x0010,r20,r20  //
	st.c    r20,psr         //

	mov     r1,r26          // save r1 for return

	//	flush dcache
	adds    -1,r0,r29
	andnot  0x0f00,r30,r25	// clear out RC and RB
	mov	_flush_area-32,r19

	or      0x0800,r25,r30	// RC=2, RB=0 to flush 1st way
	call    D_FLUSH
	 st.c    r30,dirbase
	or      0x0900,r25,r30	// RC=2, RB=1 to flush 2nd way
	call    D_FLUSH
	 st.c    r30,dirbase

	// more than 8K?
	ld.c	epsr,r24	// epsr <21:18> is dcs <3:0>
	shr	19,r24,r24	// discard bit 0
	and	0x07,r24,r24	// isolate dcs <3:1>
	bc	.do_dirbase	// taken if data cache is 8K or less

	or	0x0a00,r25,r30	// RC=2, RB=2 to flush 3rd way
	call	D_FLUSH
	 st.c	r30,dirbase

	or	0x0b00,r25,r30	// RC=2, RB=3 to flush 4th way
	call	D_FLUSH
	 st.c	r30,dirbase

        // flush icache and tlb (dirbase must be in r16)
.do_dirbase:
	or      0x20,r16,r16    // raise ITI bit in order to
	st.c    r16,dirbase     // flush the I and TLB caches
	nop			// drain the instruction pre-fetch queue
	nop
	nop
	nop
	nop
	nop
	ld.c    psr,r20		// enable interrupts (perhaps)
	or      r20,r21,r20	// 
	st.c    r20,psr		//
	bri     r26
	 nop
D_FLUSH:
	or      127,r0,r27
	bla     r29,r27,D_FLUSH_LOOP
	 mov	r19,r28
D_FLUSH_LOOP::
	bla     r29,r27,D_FLUSH_LOOP    // execute next instruction
	 flush  32(r28)++               // for 128 lines in cache block
	bri     r1
	 nop

#else	i860XP
/*
 *XXX
 *XXX Here is the old flush code that will run on older i860's
 *XXX
 */
_flush_tlb::

        ld.c    psr,r20         // B2/B1 errata # 23 clear IM
        and     0x0010,r20,r21  // B2/B1 errata # 23 clear IM
        andnot  0x0010,r20,r20  // B2/B1 errata # 23 clear IM
        st.c    r20,psr         // B2/B1 errata # 23 clear IM
        ld.c    fsr,r22         // B2/B1 errata # 23 clear FTE
        and     0x0010,r22,r23  // B2/B1 errata # 23 clear FTE
        andnot  0x0010,r22,r22  // B2/B1 errata # 23 clear FTE
        st.c    r22,fsr         // B2/B1 errata # 23 clear FTE

        mov     r1,r26          // save r1 for return

	br	do_dirbase
	ld.c	dirbase,r16

_flush_and_ctxsw::
_set_cr3::			// makes pmap.h "simpler"

	andnot	0x0FFF, r16, r16
	or	0x01, r16, r16	// set virtual mode bit
        br      do_flush
        ld.c    dirbase,r30
        
_flush::

        ld.c    dirbase,r30
        mov     r30,r16

do_flush:
        ld.c    psr,r20         // B2/B1 errata # 23 clear IM
        and     0x0010,r20,r21  // B2/B1 errata # 23 clear IM
        andnot  0x0010,r20,r20  // B2/B1 errata # 23 clear IM
        st.c    r20,psr         // B2/B1 errata # 23 clear IM
        ld.c    fsr,r22         // B2/B1 errata # 23 clear FTE
        and     0x0010,r22,r23  // B2/B1 errata # 23 clear FTE
        andnot  0x0010,r22,r22  // B2/B1 errata # 23 clear FTE
        st.c    r22,fsr         // B2/B1 errata # 23 clear FTE

        mov     r1,r26          // save r1 for return

        // flush data caches

        andnot  0x0f00,r30,r30
        or      0x0800,r30,r30  // RC=2, RB=0 to flush first set
        adds    -1,r0,r29
        call    D_FLUSH
        st.c    r30,dirbase

        or      0x900,r30,r30   // RC=2, RB=1 to flush second set
        call    D_FLUSH
        st.c    r30,dirbase

//      xor     0x900,r30,r30   // clear RC and RB

        // flush I and TLB caches

do_dirbase:
        or      0x20,r16,r16    // raise ITI bit in order to
        st.c    r16,dirbase     // flush the I and TLB caches
        nop     // do 6 nop's after writing the ITI bit in the DIRBASE.
        nop
        nop
        nop
        nop
        nop

        ld.c    psr,r20         // B2/B1 errata # 23 reset IM
        or      r20,r21,r20     // B2/B1 errata # 23 reset IM
        st.c    r20,psr         // B2/B1 errata # 23 reset IM
        ld.c    fsr,r22         // B2/B1 errata # 23 reset FTE
        or      r22,r23,r22     // B2/B1 errata # 23 reset FTE
        st.c    r22,fsr         // B2/B1 errata # 23 reset FTE

        bri     r26
        nop

D_FLUSH:
        orh     h%_flush_area-32,r0,r28 // reserved page address - 32
        or      l%_flush_area-32,r28,r28
        or      127,r0,r27
        ld.l    32(r28),r31     // B1/B2 errata #10 do load twice
        ld.l    32(r28),r31     // ld with interlock clears pending writebacks
        shl     0,r31,r31
        bla     r29,r27,D_FLUSH_LOOP
         nop
         .align 32                      // B1/B2 errata #23 align
D_FLUSH_LOOP::
        ixfr    r0,f0                   // B1/B2 errata #23 sync 
        bla     r29,r27,D_FLUSH_LOOP    // execute next instruction
         flush  32(r28)++               // for 128 lines in cache block
        ixfr    r0,f0                   // B1/B2 errata #23 sync 
        bri     r1
         ld.l   -512(r28),r0
#endif	i860XP

////////////////////////////////////////////////////////////////////////////

	.data
	.align	32
//
// XXX	All of the following static data will go away sometime soon
//	and be allocated from free pages.  The stack will also come
//	from free pages and will be reclaimed after threads start
//	running.
//
_sdata::
_flush_area::	.byte	[4096]0
_boot_stack_lo::
#if	i860XP
	.byte	[4096]0
#else	i860XP
	//
	// XXX	Someone should check this on the cube; the 4K bootstrap
	// XXX	stack works on Paragons, probably works fine on the cube.
	//
	.byte	[16384]0
#endif	i860XP
_boot_stack_hi::

