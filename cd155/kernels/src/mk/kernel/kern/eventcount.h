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
 * $Log: eventcount.h,v $
 * Revision 1.5  1994/11/18  20:51:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:44:27  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:35:23  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:35:08  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/03  16:19:59  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:54:15  jeffreyh]
 * 
 * Revision 2.3  92/01/03  20:40:10  dbg
 * 	Added id field to make user-safe.  Since we have not
 * 	decided what the user-interface will look like (and
 * 	we are using it nonetheless) do not define anything.
 * 	[91/12/27            af]
 * 
 * Revision 2.2  91/12/13  14:54:47  jsb
 * 	Created.
 * 	[91/11/01            af]
 * 
 */
/*
 *	File:	eventcount.c
 *	Author:	Alessandro Forin
 *	Date:	10/91
 *
 *	Eventcounters, for user-level drivers synchronization
 *
 */

#ifndef	_KERN_EVENTCOUNT_H_
#define	_KERN_EVENTCOUNT_H_	1

/* kernel visible only */

typedef struct evc {
	unsigned int	count;
	thread_t	waiting_thread;
	unsigned int	ev_id;
	struct evc	*sanity;
	decl_simple_lock_data(,	lock)
} *evc_t;

extern	void	evc_init(/* evc_t ev */),
		evc_destroy(/* evc_t ev */),
		evc_signal(/* evc_t ev */);

/* kernel and user visible */

extern	kern_return_t	evc_wait(/* unsigned ev_id */);

#endif	/* _KERN_EVENTCOUNT_H_ */
