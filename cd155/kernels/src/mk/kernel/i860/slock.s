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
 * Copyright 1988, 1989, 1990, 1991, 1992 by Intel Corporation,
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
 *
 * i860 simple lock routines.
 *
 * $Id: slock.s,v 1.15 1995/03/14 08:04:20 lenb Exp $
 */
#include <cpus.h>
#include <mach_lock_mon.h>
#include <mach_mp_debug.h>
#include <mach_ldebug.h>

#include <i860/cpu_number.h>
#include <i860/psl.h>

#if     (MACH_LOCK_MON || MACH_LDEBUG)
#define _simple_lock     __simple_lock
#define _simple_lock_try __simple_lock_try
#define _simple_unlock   __simple_unlock

/* makes these unused and allows lock.c to interpose */
#define _simple_lock1     __simple_lock1
#define _simple_lock_try1 __simple_lock_try1
#endif  (MACH_LOCK_MON || MACH_LDEBUG)

#define VERIFY_LOCK 0	/* 1 == verify lock value is {0/1} */
#define	LA_TRIGGER 0	/* 1 == enable logic Analyzer triggers */
#define	ATOMIC_UNLOCK 0	/* 1 == lock/unlock hardware around lock unlock op */
#define	LOCK_EXPIRE 1	/* 1 == printf() when a lock has expired */

	.file	"slock.s"
	.text
	.align	32

#define	LOCK_OFFSET	0	/* real atomic lock longword */
#define	LOCK_PC		4	/* r1 of caller */
#define	UNLOCK_PC	8	/* r1 of caller */
#define	OWNER		12	/* thread id of owner */
#define	TH_FP		16	/* thread's fp */
#define	TIMESTAMP	20	/* timestamp */

/*
 *	int
 *	simple_lock_try(m)
 *		mutex_t m;	(= int *m for our purposes)
 *
 * Quick attempt to grab/lock specified lock. If it's busy then just return
 * otherwise take the lock and return.
 *
 * inputs:
 *      m       mutex address.
 *
 * outputs: (r16)
 *      1 == we own the lock
 *	0 = we DO NOT own the lock.
 *
 * side effects:
 *      
 *
 */
	.align	32
_simple_lock_try::
_simple_lock_try1::
	ld.l	LOCK_OFFSET(r16),r17	// fetch the lock cached
	btne	r0,r17,.slt	// if busy, skip the rest...
	// lock is free attempt to acquire it!
	mov	1,r18
	lock
	ld.l	LOCK_OFFSET(r16),r17
	unlock
	st.l	r18,LOCK_OFFSET(r16)	// must be AFTER unlock inst (wlb)
.slt:
	bri	r1
	  xor	1,r17,r16

/*
 *	int
 *	simple_lock(m)
 *		mutex_t m;	(= int *m for our purposes)
 *
 * loop until we acquire the specified lock. Spin watching the cached
 * lock value. When it is free lock the bus, check the lock and if
 * available set it, unlock and your outta here.....Otherwise start
 * entire sequence again.
 *
 * inputs:
 *      m       mutex address.
 *
 * outputs:
 *      none
 *
 * side effects:
 *
 */
	.data
#if	ATOMIC_UNLOCK
_sunlock::
	.long	0	// 0 == st.l without lock/unlock pair
#endif

#if	LOCK_EXPIRE
// spin countdown before a lock expires.
_bail::
	.long	9000000
#endif	LOCK_EXPIRE

	// printf(str,va,r1,phys,owner's-r1)
_slp::
#if	MACH_LDEBUG
	.string	"slock VA 0x%x\ncur-lockr: r1 0x%x 1st-lockr th 0x%x r1 0x%x\n"
#else
	.string	"slock VA 0x%x cur-locker r1 0x%x\n"
#endif
	.align 4

	.text
	.align	32
	.globl	_active_threads

_simple_lock::
_simple_lock1::

#if	LOCK_EXPIRE
	orh	h%_bail,r0,r21
	or	l%_bail,r21,r21
	ld.l	0(r21),r20	// bailout count
#endif

	ld.l	LOCK_OFFSET(r16),r17	// fetch lock contents, load cache

#if VERIFY_LOCK
	mov	1,r18
	andnot	r18,r17,r0
	bnc	spill_my_guts
#endif

#if LA_TRIGGER
	shl	24,r17,r19		// position lock value
	orh	h%0x60300000,r0,r18	// combine -> 0x603 or 0x613
	addu	r19,r18,r18
	br	.sl01
	 stio.l	r16,r18			// output phys lock adrs
#else	/* LA_TRIGGER */

	br	.sl01			// skip initial lock load, see before
	 nop

#endif	/* LA_TRIGGER */

.sl1:	// spin on cached value of lock without locking the bus
	ld.l	LOCK_OFFSET(r16),r17	// fetch lock contents, load cache
.sl01:

#if VERIFY_LOCK
	// verify the lock is {0/1}
	mov	1,r18
	andnot	r18,r17,r0
	bnc	spill_my_guts
#endif

#if	LOCK_EXPIRE
	adds	-1,r20,r20
	bnc	.sl2

	// spin count has expired.
	bte	r0,r17,.sl2		// do we care about expiration?

#if	MACH_LDEBUG
        ld.l    4(fp),r20		// PC of the 1st locker
#if LA_TRIGGER
	orh	h%0x66300000,r0,r18
	stio.l	r20,r18			// LA printf
#endif
#endif	/* MACH_LDEBUG */

#if LA_TRIGGER
	// yes - signal the world.
	orh	h%0x6f300000,r0,r18
	stio.l	r16,r18			// output lock adrs
#endif

#ifdef	MACH_LDEBUG
	// printf(
	//	r16	format-str(r16),
	//	r17	lock-VA
	//	r18	caller's-r1
	//	r19	1st-lockr-thread
	//	r20	lockr-r1 )
#else
	// printf(str(r16),va(r17),r1(r18)
#endif
	// entry:
	//	r16 == lock address
	//	r1 return address
spill_my_guts:
        addu    -16,sp,sp
        st.l    r1,0(sp)		// protect our return adrs
        st.l	r16,4(sp)		// save VA of lock
#if	MACH_LDEBUG
	ld.l	OWNER(r16),r19		// 1st locker's thread adrs
	or	r1,r0,r18		// _simple_lock() caller's PC
	ld.l	LOCK_PC(r16),r20	// PC of 1st locker
#else
	or	r16,r0,r19		// save lock's Physical Adrs
        ld.l    0(sp),r18		// our caller's PC
#endif
	or	r16,r0,r17		// VA of simple_lock structure.
	orh	h%_slp,r0,r16
	call	_db_printf		// use db_printf -- it doesn't lock...
	 or	l%_slp,r16,r16		// printf() format string

        ld.l    4(sp),r16		// VA of lock
        ld.l    0(sp),r1		// reset return address
        addu    16,sp,sp		// cleanup
	// die here
sl_die:
	nop
	br	_gimmeabreak
	nop
	br	sl_die
	  nop

	// spin count OK
.sl2:
#endif	LOCK_EXPIRE

	// != 0 imples lock is busy, check again.
	btne	r0,r17,.sl1

	// lock is now available, lock the bus and REALLY check
	lock
	ld.l	LOCK_OFFSET(r16),r17	// fetch lock contents
	nop
	or	r17,r0,r0
	or	1,r0,r18		
	unlock
	st.l	r18,LOCK_OFFSET(r16)	// must be AFTER unlock inst (wlb)

#if VERIFY_LOCK
	// no other bits except for #0 are allowed, die if others are set.
	mov	1,r18
	andnot	r18,r17,r0
	bnc	spill_my_guts
#endif

	// was lock free when we really locked it (r17 previous lock value) ?
	// if not then start again.

	btne	r0,r17,.sl1

#if	LA_TRIGGER
	orh	h%0x62300000,r0,r18	// output lock adrs & we have the lock
	stio.l	r16,r18
#endif
	bri	r1
	  nop

#if     MACH_MP_DEBUG
	.globl	_retry_simple_lock	// see kern/lock.c
slock_dbg:
	br	_retry_simple_lock
	  nop
#endif


/*
 *	int
 *	simple_lock_pause()
 *
 * inputs:
 *      none
 *
 * spin_loop until we countdown to zero.  Used in loops that are trying to
 * acquire locks out-of-order. This smells like a bad idea!! stan.
 *
 * outputs:
 *      none
 *
 * side effects:
 *      none
 *
 */
	.align	32
_simple_lock_pause::
	mov	100000,r16	// 386 used 100 with stack references
.slp:
	nop
	nop
	nop			// squander time....
	nop
	addu	-1,r16,r16
	btne	r0,r16,.slp
	bri	r1
	  nop

/*
 *	void
 *	simple_unlock(m)
 *		mutex_t m;	(= int *m for our purposes)
 *
 * Atomic lock release.
 *
 * inputs:
 *      m       mutex address.
 *
 * outputs:
 *      none
 *
 * side effects:
 *      none
 *
 */
sul_fail:
	br	_gimmeabreak
	nop

	.align	32
_simple_unlock::

#if VERIFY_LOCK
	// verify the lock is {0/1}
	ld.l	LOCK_OFFSET(r16),r17	// get lock contents
	mov	1,r18
	andnot	r18,r17,r0
	bnc	spill_my_guts
#endif

#if LA_TRIGGER
#if	MACH_LDEBUG
        ld.l    4(fp),r20		// PC of unlocker
#else
        or	r1,r0,r20		// PC of unlocker
#endif
	orh	h%0x67300000,r0,r18
	stio.l	r20,r18			// LA printf
#endif	/* LA_TRIGGER */

#if	ATOMIC_UNLOCK
	orh	ha%_sunlock,r0,r20
	ld.l	l%_sunlock(r20),r21
	btne	r0,r21,sun.1
#endif


#if LA_TRIGGER
	shl	24,r17,r19		// position lock value
	orh	h%0x63300000,r0,r18	// output lock value to 633 or 643
	addu	r19,r18,r18
	stio.l	r16,r18
#endif
	bri	r1
	 st.l	r0,LOCK_OFFSET(r16)

#if	ATOMIC_UNLOCK
/*
 * perform an atomic lock clear operation
 */
sun.1:
	lock
	ld.l	LOCK_OFFSET(r16),r17
	nop
	nop
	or	r17,r0,r0
	nop
	unlock
	st.l	r0,LOCK_OFFSET(r16)
#if LA_TRIGGER
	shl	24,r17,r19		// position lock value
	orh	h%0x63300000,r0,r18	// output lock value to 633 or 643
	addu	r19,r18,r18
	stio.l	r16,r18
#endif
	bri	r1
	  nop
#endif	/* ATOMIC_UNLOCK */


/*
 *	void
 *	i_bit_set( bit_number, lock_word_adrs )
 *		int	bit_number;
 *		int	*lock_word_adrs;
 *
 *	atomic bit set in specified longword; limited to 1 longword [0..31]. 
 *
 * inputs:
 *      bit_number	0..31
 *	lock_word_adrs	address of longword which gets a bit set.
 *
 * outputs:
 *      previous value of the lock word
 *
 * side effects:
 *      on return specified bit in longword has been set.
 *
 */
	.align	32
_i_bit_set::
#if	DEBUG
	// bit number must be in range [0...31]
	subu	31,r16,r18	// if (bit_number > 31) then branch
	bnc	ib_dead
	adds	r0,r16,r18
	bc	ib_dead
#endif
	ld.l	0(r17),r21	// force coherency between CPU caches.
	lock
	ld.l	0(r17),r20	// fetch bit lock longword
	mov	1,r18		// start with bit #0
	shl	r16,r18,r19	// shift left to correct bit position
	or	r19,r20,r21	// set specified bit
	unlock
	st.l	r21,0(r17)	// reset lock word
	bri	r1
	  or	r20,r0,r16	// return previous lock word value

#if	DEBUG
	.data
ib_msg:
	.string "i_bit_{set/clear}() invalid bit number in r16"
	.align	4

	.text
	.globl	_panic
ib_dead:
	orh	h%ib_msg,r0,r16
	br	_panic
	  or	l%ib_msg,r16,r16
#endif
	
/*
 *	void
 *	i_bit_clear( bit_number, lock_word_adrs)
 *		int	bit_number;
 *		int	*lock_word_adrs;
 *
 *	atomic clear of specified bit in specified longword.
 *
 * inputs:
 *      bit_number	0..31
 *	lock_word_adrs	address of longword which gets specified bit cleared.
 *
 * outputs:
 *      previous value of lock longword
 *
 * side effects:
 *      on return specified bit in longword has been set.
 *
 */
	.align	32
_i_bit_clear::
#if	DEBUG
	// bit number must be in range [0...31]
	subu	31,r16,r18	// if (bit_number > 31) then branch
	bnc	ib_dead
	adds	r0,r16,r18
	bc	ib_dead
#endif
	ld.l	0(r17),r21	// force coherency between CPU caches.
	lock
	ld.l	0(r17),r20	// fetch bit lock word
	mov	1,r18		// start with bit #0
	shl	r16,r18,r19	// shift left to correct bit position
	andnot	r19,r20,r21	// clear specified bit
	unlock
	st.l	r21,0(r17)	// reset lock word
	bri	r1
	  or	r20,r0,r16	// return previous value of lock word


/*
 *	void
 *	bit_lock( int bit_num, int *lock)
 *
 * assumptions:
 *	bitVector address is longword aligned!!
 *
 * inputs:
 *      bit_num	  bit # in a possibly large bit vector which spans many longs
 *	lock	  start address of bit vector.
 *
 *	set specified bit in bit vector
 *
 * outputs:
 *      none
 *
 * side effects:
 *      on return we own the lock, loop internally until we do!
 *
 */
	.align	32
_bit_lock::
	// bitvector must be word aligned!
	and	3,r17,r18
	btne	r0,r18,bit_l_die

	// determine which longword contains desired bit.
	shr	5,r16,r18	// bit position in # of longwords offset from 0
	shl	2,r18,r19	// convert to bytes
	addu	r19,r17,r18	// address of word within bit vector
	ld.l	0(r18),r21	// force coherency between CPU caches.
	lock
	ld.l	0(r18),r20	// fetch word with bit to operate on
	and	0x1f,r16,r19	// position with a 32-bit word 0..31
	mov	1,r31
	shl	r19,r31,r31	// position mask
	or	r31,r20,r20	// set the bit
	unlock
	st.l	r20,0(r18)	// reset lock word & release bus
	bri	r1
	  nop


/*
 *	void
 *	bit_unlock( int bit_num, int *lock)
 *
 *	clear specified bit in bit vector
 *
 * assumptions:
 *	bitVector address is word aligned!!
 *
 * inputs:
 *      bit_num	bit # in a possibly VERY large bit vector which spans words
 *	lock	start address of bit vector.
 *
 * outputs:
 *      none
 *
 * side effects:
 *
 */
	.align	32
_bit_unlock::
	// bitvector must be word aligned!
	and	3,r17,r18
	btne	r0,r18,bit_ul_die

	// determine which long contains the requested bit.
	// 
	shr	5,r16,r18	// bit position in # of longs offset from 0
	shl	2,r18,r19	// convert to bytes
	addu	r19,r17,r18	// address of word within bit vector
	ld.l	0(r18),r21	// force coherency between CPU caches.
	lock
	ld.l	0(r18),r20	// fetch word with bit to operate on
	and	0x1f,r16,r19	// position with a 32-bit word 0..31
	mov	1,r31
	shl	r19,r31,r31	// position mask
	andnot	r31,r20,r20	// clear the bit
	unlock
	st.l	r20,0(r18)	// reset lock word & release bus
	bri	r1
	  nop

	.data
bul_align_death:
	.string "bit_unlock() bit vector NOT word aligned, adrs in r17"
bl_align_death:
	.string "bit_lock() bit vector NOT word aligned, adrs in r17"
	.align	4

	.text
	.globl	_panic

bit_l_die:
	orh	h%bl_align_death,r0,r16
	br	_panic
	  or	l%bl_align_death,r16,r16
	
bit_ul_die:
	orh	h%bul_align_death,r0,r16
	br	_panic
	  or	l%bul_align_death,r16,r16
	


/*
 *	void
 *	atomic_set( adrs, new_value )
 *		int	*adrs;
 *		int	new_value;
 *
 *	atomic read then write op
 *
 * inputs:
 *	adrs		address of longword which gets written
 *      new_value	value written after read.
 *
 * outputs:
 *      old value
 *
 * side effects:
 *      on return specified longword has been reset.
 *
 */
	.align	32
_atomic_set::
	ld.l	0(r16),r21	// force coherency on all cached copies
	nop
	or	r21,r0,r0
	lock
	ld.l	0(r16),r18	// fetch old value
	nop
	or	r18,r0,r0
	unlock
	st.l	r17,0(r16)	// write new value
	bri	r1
	  or	r18,r0,r16	// return old value

/*
 * input:
 *	r16 == lock address
 * output:
 *	r16 boolean_t
 */

 _is_this_port_locked::
	ld.l    LOCK_OFFSET(r16),r16
	bri     r1
	  nop

/*
 * return my caller's return address
 */
 _my_caller::
	ld.l    4(fp),r16
	bri     r1
	  nop

