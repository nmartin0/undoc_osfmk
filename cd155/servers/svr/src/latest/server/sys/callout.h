/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: callout.h,v $
 * Revision 1.4  1994/11/18  20:40:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:39  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:45  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/12/16  12:50:18  roy
 * 	Initial check-in.
 *
 * Revision 3.0  91/10/17  15:44:04  condict
 * Original version from OSF/1 Release 1.0.2; modified struct callout.
 * 
 * Revision 1.3  90/10/07  14:49:14  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:37:22  gm]
 * 
 * Revision 1.2  90/01/02  20:15:38  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:47:55  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:15:22  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.6  89/03/09  22:02:36  rpd
 * 	More cleanup.
 * 
 * Revision 2.5  89/02/25  17:52:11  gm0w
 * 	Got rid of conditional on CMUCS and MACH and the code that
 * 	was defined for non-CMUCS case
 * 	[89/02/13            mrt]
 * 
 * Revision 2.4  89/01/15  16:33:36  rpd
 * 	Use decl_simple_lock_data.
 * 	[89/01/15  15:14:27  rpd]
 * 
 * Revision 2.3  88/11/14  14:33:35  gm0w
 * 	Placed kernel includes under #ifdef KERNEL.
 * 	[88/11/14            gm0w]
 * 
 * Revision 2.2  88/09/25  22:15:52  rpd
 * 	Added copyright, includes, etc.
 * 	Declared variables extern when CMUCS.
 * 	[88/09/09  04:49:09  rpd]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)callout.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_CALLOUT_H_
#define _SYS_CALLOUT_H_

#include <sys/types.h>
#include <sys/time.h>

/*
 * The callout structure is for
 * a routine arranging
 * to be called by the clock interrupt
 * (clock.c) with a specified argument,
 * in a specified amount of time.
 * Used, for example, to time tab
 * delays on typewriters.
 */

struct	callout {
	struct callout	*c_next;
	struct callout	*c_prev;
	struct timeval	c_time;		/* absolute time */
	caddr_t		c_arg;		/* argument to routine */
	int		(*c_func)();	/* routine */
	int		c_special;	/* special flag */
};
typedef struct callout	*callout_t;

#ifdef	KERNEL
#include <kern/lock.h>

extern struct callout *callfree, *callout, calltodo;
extern int ncallout;
struct mutex callout_lock;

#endif	KERNEL
#endif	_SYS_CALLOUT_H_
