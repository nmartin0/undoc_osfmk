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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: synch.h,v $
 * Revision 1.5  1994/11/18  20:41:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:28:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:45  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:20:15  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:44:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:52  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:55  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:42:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  1993/04/08  11:32:44  loverso
 * 	Include kern/parallel.h because the spl macro def can use unix_master.
 * 	[1992/06/04  14:16:40  condict]
 *
 * Revision 2.6  92/05/24  14:38:09  pjg
 * 	Revision 3.4  92/03/23  18:03:57  condict
 * 	Move curr_ipl into the thread structure (u.uu_ipl), so it can be saved
 * 	across sleeps (e.g. in ux_server_thread_blocking/unblocking).
 * 
 * Revision 2.5  91/12/16  18:02:00  roy
 * 	91/10/17  15:45:23  condict
 * 	Add an alternate version of spl_n that can be used by imported modules
 * 	that depend on spl's for mutual exclusion..
 * 
 * Revision 2.4  91/10/17  11:19:11  sjs
 * fixed RCS messages
 * 
 * Revision 2.3  91/10/14  13:09:34  sjs
 * 	91/10/01  14:10:30  condict
 * 	Remove extern declaration of spl_n function.  It is now a no-op macro.
 * 
 * 	91/09/27  12:01:39  emcmanus
 * 	Added splstr() macro.
 * 
 * Revision 2.2  91/08/31  14:13:24  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:07:49  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.1  89/08/04  14:44:04  rwd
 * Created.
 * 
 *  8-May-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */

/*
 * SPL level definitions.
 */
#define	SPL_COUNT	6

#define	SPL0		0
				/* placeholder - not used */
#define	SPLSOFTCLOCK	1
				/* level '0x8' */
#define	SPLNET		2
				/* level '0xc' */
#define	SPLTTY		3
				/* level '0x15' */
#define	SPLBIO		3
				/* level '0x15' */
#define	SPLIMP		4
				/* level '0x16' */
#define	SPLHIGH		5
				/* level '0x18' */


/* Some imported modules may not be parallelized to work on a multi-processor,
 * i.e. they may depend on the use of spl for locking.  The following alternate
 * definition of spl_n allows for this, if these source files define
 * USE_SPL_LOCKING to 1 before including this file.  Note that USE_SPL_LOCKING
 * is a module-dependent macro, not a global option from the configuration file.
 * Note also that it is required that all such modules include this file,
 * directly or indirectly, in order to avoid making calls to the spl functions,
 * which are no-ops.
 */

/* The following is used to define the body of the spl_n macro when using gcc,
 * or the body of the spl_n_lock function, in case we are not using gcc:
 */
#define SPL_N_LOCK_BODY(x, prev_ipl) do {	\
	unix_master();				\
	prev_ipl = u.uu_ipl;			\
	u.uu_ipl = (x);				\
	/* Already had it, don't lock again: */	\
	if (prev_ipl > 0) unix_release();	\
	if (u.uu_ipl == 0) unix_release();	\
} while (0)

#ifdef	USE_SPL_LOCKING

#include <kern/parallel.h>		/* For unix_master/unix_release */

#ifdef	__GNUC__
#define spl_n(x) ({				\
	int prev_ipl;				\
	SPL_N_LOCK_BODY(x, prev_ipl);		\
	prev_ipl;				\
})
#else	__GNUCC__
#define spl_n	spl_n_lock
#endif	__GNUCC__

#else	USE_SPL_LOCKING

#define spl_n(x)	0

#endif	USE_SPL_LOCKING

#define	spl0()		(spl_n(SPL0))
#define	splsoftclock()	(spl_n(SPLSOFTCLOCK))
#define	splnet()	(spl_n(SPLNET))
#define	splbio()	(spl_n(SPLBIO))
#define	spltty()	(spl_n(SPLTTY))
#define	splimp()	(spl_n(SPLIMP))
#define	splhigh()	(spl_n(SPLHIGH))
#define	splx(s)		(spl_n(s))
#define splsched()	(spl_n(SPLHIGH))
#define splvm()		(spl_n(SPLIMP))
#define splstr()	(spl_n(SPLHIGH))

extern void	interrupt_enter();
extern void	interrupt_exit();
