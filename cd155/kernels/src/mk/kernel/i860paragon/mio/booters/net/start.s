// 
// $Copyright
// Copyright 1993, 1994 , 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
//  
	.file	"start.s"
	.text
_start::
	orh	h%_the_stack_hi,r0,r31		// Set stack pointer
	or	l%_the_stack_hi,r31,sp
	andnot	15,sp,sp
	mov	r0,fp
	call	_main				// run C code
	 nop

.tight:	br	.tight
	 nop

_get_sp::
	bri	r1
	mov	sp,r16
	
////////////////////////////////////////////////////////////////////////
//
// _flush
//   flush the caches of the cpu remaining in the current process.
//   _flush was put here in order to insure that the code that changes
//   the dirbase and the following 6 nop's are within the same page.
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
#define	DPS	0

_flush::
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
        nop			// drain the pre-fetch instruction queue
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

////////////////////////////////////////////////////////////////////////////

	.data
	.align	32
_sdata::
_flush_area::	.byte	[4096]0
_the_stack_lo::
		.byte	[16384]0
_the_stack_hi::
