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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * 23-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Support for MACH_LDEBUG
 *
 * $Log: lock.h,v $
 * Revision 1.5  1994/11/18  20:33:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:24:19  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:59  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:43  dleslie
 * First R1_0 release
 *
 * Revision 2.4.4.1  92/02/18  18:47:03  jeffreyh
 * 	Do not use simple lock macros if MACH_MP_DEBUG or
 * 	MACH_LOCK_MON are set.
 * 	[91/12/06            bernadat]
 * 
 * 	Dont use inline macros in case of debug
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.4  91/11/12  11:50:47  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.3  91/05/14  16:11:00  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:39:08  dbg
 * 	Created.
 * 	[91/03/21            dbg]
 * 
 */

#include <mach_mp_debug.h>
#include <mach_lock_mon.h>

/*
 * Machine-dependent simple locks for the i386.
 */
#ifndef	_I386_LOCK_H_
#define	_I386_LOCK_H_

/*
 *	All of the locking routines are built from calls on
 *	a locked-exchange operation.  Values of the lock are
 *	0 for unlocked, 1 for locked.
 *
 *	Do not use these macros for debugging (dead lock detection)
 */



#if	defined(__GNUC__) && !MACH_MP_DEBUG

/*
 * If lock minitoring is on macros, simple lock routines 
 * must not point to these macros
 */

#if	(!MACH_LOCK_MON && !MACH_LDEBUG)

#define simple_lock	_simple_lock
#define simple_lock_try _simple_lock_try
#define simple_unlock	_simple_unlock
#endif	(!MACH_LOCK_MON && !MACH_LDEBUG)

/*
 *	The code here depends on the GNU C compiler.
 */

#define	_simple_lock_xchg_(lock, new_val) \
    ({	register int _old_val_; \
	asm volatile("xchgl %0, %2" \
		    : "=r" (_old_val_) \
		    : "0" (new_val), "m" (*(lock)) \
		    ); \
	_old_val_; \
    })

#define	simple_lock_init(l) \
	((l)->lock_data = 0)

#define	_simple_lock(l) \
    ({ \
	while(_simple_lock_xchg_(l, 1)) \
	    while (*(volatile int *)&(l)->lock_data) \
		continue; \
	0; \
    })

#define	_simple_unlock(l) \
	(_simple_lock_xchg_(l, 0))

#define	_simple_lock_try(l) \
	(!_simple_lock_xchg_(l, 1))

/*
 *	General bit-lock routines.
 */
#define	bit_lock(bit, l) \
    ({ \
	asm volatile("	jmp	1f	\n\
		    0:	btl	%0, %1	\n\
			jb	0b	\n\
		    1:	lock		\n\
			btsl	%0, %1	\n\
			jb	0b" \
		    : \
		    : "r" (bit), "m" (*(volatile int *)(l))); \
	0; \
    })

#define	bit_unlock(bit, l) \
    ({ \
	asm volatile("	lock		\n\
			btrl	%0, %1" \
		    : \
		    : "r" (bit), "m" (*(volatile int *)(l))); \
	0; \
    })

/*
 *	Set or clear individual bits in a long word.
 *	The locked access is needed only to lock access
 *	to the word, not to individual bits.
 */
#define	i_bit_set(bit, l) \
    ({ \
	asm volatile("	lock		\n\
			btsl	%0, %1" \
		    : \
		    : "r" (bit), "m" (*(l)) ); \
	0; \
    })

#define	i_bit_clear(bit, l) \
    ({ \
	asm volatile("	lock		\n\
			btrl	%0, %1" \
		    : \
		    : "r" (bit), "m" (*(l)) ); \
	0; \
    })

#endif	/* defined(__GNUC__) && !MACH_MP_DEBUG */

extern void simple_lock_pause();

#endif	/* _I386_LOCK_H_ */






