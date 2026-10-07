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
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * 15-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Added fields in time_out struct for softclock propagation on MPs.
 *
 * $Log: time_out.h,v $
 * Revision 1.5  1994/11/18  20:53:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:46:09  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:37:12  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:38:15  dleslie
 * First R1_0 release
 *
 * Revision 2.5  91/07/31  17:51:15  dbg
 * 	Fix race condition.
 * 	[91/07/30  17:08:00  dbg]
 * 
 * Revision 2.4  91/05/14  16:49:27  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:30:51  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:20:31  mrt]
 * 
 * Revision 2.2  90/11/05  14:32:00  rpd
 * 	Changed untimeout to return boolean.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.1  89/08/03  15:57:24  rwd
 * Created.
 * 
 * 14-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */

#ifndef	_KERN_TIME_OUT_H_
#define	_KERN_TIME_OUT_H_

/*
 * Mach time-out and time-of-day facility.
 */

#include <mach/boolean.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/zalloc.h>
#include <kern/processor.h>

/*
 *	Timers in kernel:
 */
extern unsigned long elapsed_ticks; /* number of ticks elapsed since bootup */
extern int	hz;		/* number of ticks per second */
extern int	tick;		/* number of usec per tick */

/*
 *	Time-out element.
 */
struct timer_elt {
	queue_chain_t	chain;		/* chain in order of expiration */
	int		(*fcn)();	/* function to call */
	char *		param;		/* with this parameter */
	unsigned long	ticks;		/* expiration time, in ticks */
	int		set;		/* unset | set | allocated */
#if	NCPUS > 1
	int		count;		/* Approximate count of next */
					/* elements with same ticks value */
	processor_t	bound_processor; /* Must be executed on this proc. */
#endif	NCPUS > 1
};
#define	TELT_UNSET	0		/* timer not set */
#define	TELT_SET	1		/* timer set */
#define	TELT_ALLOC	2		/* timer allocated from pool */

typedef	struct timer_elt	timer_elt_data_t;
typedef	struct timer_elt	*timer_elt_t;

/* for 'private' timer elements */
extern void		set_timeout();
extern boolean_t	reset_timeout();

/* for public timer elements */
extern void		timeout();
extern boolean_t	untimeout();

#define	set_timeout_setup(telt,fcn,param,interval)	\
	((telt)->fcn = (fcn),				\
	 (telt)->param = (param),			\
	 (telt)->private = TRUE,			\
	 set_timeout((telt), (interval)))

#define	reset_timeout_check(t) \
	MACRO_BEGIN \
	if ((t)->set) \
	    reset_timeout((t)); \
	MACRO_END

#endif	_KERN_TIME_OUT_H_
