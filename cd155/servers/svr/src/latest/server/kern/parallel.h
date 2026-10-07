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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: parallel.h,v $
 * Revision 1.10  1995/02/01  21:32:13  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:32:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/10/27  05:35:57  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: corrects previous checkin for the non-assertful case.
 *  Testing: EATs: controlc
 *  Module(s): see previous checkin
 *
 * Increment count in all cases in unix_master() macro, but still after lock
 * is taken.
 *
 * Revision 1.7  1994/10/25  22:31:05  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Lo
 *  Benefit or PTS #: 11128
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/i386/bsd_machdep.c
 * 	server/i386/slock.s
 * 	server/i860/bsd_machdep.c
 * 	server/i860/slock.s
 * 	server/kern/parallel.h
 * 	server/kern/sched_prim.c
 * 	server/sys/unix_defs.h
 * 	server/tnc/rvp_subr.c
 * 	server/uxkern/cred_servers.c
 * 	server/uxkern/emul_user.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/ux_server_loop.c
 *
 * For assertful servers ($CONFIG contains "test" and "MACH_ASSERT" is defined
 * to be 1 instead of 0), make taking the master lock be a subroutine call,
 * so that more error checking can be done, and so that the routine name
 * will appear in debug tracebacks.
 * For servers with lock debugging ($CONFIG contains "ldebug" and "MACH_LDEBUG"
 * is defined to be 1 instead of 0), do even more master lock consistency
 * checking, including saving the call chain of the last locker and unlocker
 * of the master lock.
 *
 * Revision 1.6  1994/06/17  23:55:30  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.5  1994/02/15  00:31:27  yazz
 *  Reviewer: cfj
 *  Risk: lo
 *  Benefit or PTS #: catch master lock mismatches sooner
 *  Testing: minimal
 *  Module(s): server/kern/parallel.h
 *
 *
 * Panic if an excess call to unix_release() is made, dropping the lock
 * count below zero.
 *
 * Revision 1.4  1993/07/14  18:01:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:55  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:13  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.9  1993/02/02  15:29:18  rabii
 * 	Added ifdef _PARALLEL_H_ so this file can be included multiple times
 * 	(rabii)
 *
 * Revision 2.8  92/06/30  22:46:44  loverso
 * 	Added TNC_unix_master() and TNC_unix_release() macros (rabii)
 * 
 * Revision 2.7  92/06/10  16:34:45  pjg
 * 	Put MACRO_BEGIN and MACRO_END around master_lock and master_unlock.
 * 
 * Revision 2.6  92/05/31  18:58:44  loverso
 * 	Undid the last change. Keep the master lock as a mutex and if
 * 	MACH_LDEBUG is set define an auxiliary structure master_mutex_aux
 * 	to record who acquired and released the lock (pjg).
 * 	Define a real syscall_on_master() (pjg).
 * 
 * Revision 2.5  92/05/27  20:04:25  pjg
 * 	Define the master lock as a simple lock if NCPUS > 1 and MACH_LDEBUG
 * 	to ease debugging of deadlocks in the master lock.
 * 
 * Revision 2.4  91/12/16  20:40:49  roy
 * 	91/10/21  18:47:40  emcmanus
 * 	Define fake syscall_on_master() used in some asserts.
 * 
 * Revision 2.3  91/10/14  12:33:56  sjs
 * 	91/10/01  14:06:57  condict
 * 	Change back to original definition of master lock, now that spl's 
 * 	are gone.
 * 
 * 	91/09/20  21:26:29  barbou
 * 	master_lock deals correctly with the interrupt handlers ipl.
 * 
 * 	91/09/18  17:46:00  condict
 * 	Fix definition of unix_master and master_lock.  Must release all the spl
 * 	locks before attempting to get the master lock, o.w. deadlock.
 * 
 * Revision 2.2  91/08/31  13:38:00  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/06/25  16:49:35  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:09:14  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  89/12/08  20:14:56  rwd
 * 	Changed master_lock to mutex
 * 	[89/10/30            rwd]
 * 
 */
/*
 * Serialization for threads within the kernel.
 */

#ifndef	_PARALLEL_H_
#define	_PARALLEL_H_
#include <kern/macro_help.h>
#include <mach_assert.h>
#include <mach_ldebug.h>

extern struct mutex master_mutex;

#if MACH_ASSERT

#define MASTER_LOCK_SANITY_MAX	7

extern struct mutex_aux {
	char	*slthread;
	int	slck_addr;
	int	sunlck_addr;
#if MACH_LDEBUG
#define MASTER_TRACE_COUNT	8
	int	slock_trace[MASTER_TRACE_COUNT];
	int	sunlock_trace[MASTER_TRACE_COUNT];
#endif /* MACH_LDEBUG */
} master_mutex_aux;

#ifndef current_thread
extern struct uthread *current_thread();
#endif


/*
 * Call subroutines so their name appears in tracebacks.
 */
#define master_lock()	do_master_lock()
#define master_unlock()	do_master_unlock()
#define	unix_master()	do_unix_master()
#define	unix_release()	do_unix_release()

#else /* !MACH_ASSERT */

#define	master_lock()	mutex_lock(&master_mutex)
#define master_unlock()	mutex_unlock(&master_mutex)

/* only alter the counter while the master lock is held. */
#define	unix_master()			\
MACRO_BEGIN				\
	if (u.uu_master_lock == 0) {	\
		master_lock();		\
	}				\
	++u.uu_master_lock;		\
MACRO_END

#define	unix_release()			\
MACRO_BEGIN				\
	if (--u.uu_master_lock == 0) {	\
		master_unlock();	\
	}				\
MACRO_END

#endif /* !MACH_ASSERT */

#if MACH_LDEBUG || MACH_ASSERT
#define syscall_on_master() u.uu_master_lock
#endif


#ifdef	TNC
#define	TNC_unix_master()	unix_master()
#define	TNC_unix_release()	unix_release()
#else	/* TNC */
#define	TNC_unix_master()
#define	TNC_unix_release()	
#endif	/* TNC */
#endif	_PARALLEL_H_
