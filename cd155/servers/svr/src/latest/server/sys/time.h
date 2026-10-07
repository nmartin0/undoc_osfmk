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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: time.h,v $
 * Revision 1.8  1995/02/01  21:38:28  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:41:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/08/31  22:46:57  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.4.8.1  1994/08/13  01:45:56  paul
 * Reviewer: mjl jlitvin
 * Risk:M
 * Benefit or PTS #: 8836
 * Testing: tested on Plymouth across diag station reboots
 * Module(s):
 *     sys/time.h
 *     uxkern/rpm_clock.c
 *     uxkern/boot_config.c
 *
 * Changes to fix bug 8836 9521 RTI - Handle an RPM which stops counting
 *     This change halts a system when the RPM stops counting either due
 *     to a hardware failure, or a reboot of the diagnostic station. It
 *     also includes a boot magic variable to override use of the RPM.
 *
 * Revision 1.4  1993/10/29  11:57:23  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.3  1993/07/14  18:29:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:19  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  18:08:44  roy
 * 	91/10/28  11:10:04  condict
 * 	Change update_time_var to copy the Mach time repeatedly until it finds
 * 	that seconds didn't change (avoids race with the Mach thread that
 * 	updates *mtime).
 *
 * 	91/10/23  16:38:25  condict
 * 	Moved the MAP_TIME definitions here from sys/kernel.h, because they
 * 	involve changes to the TIME LOCK macros, now.
 *
 * Revision 2.2  91/08/31  14:14:27  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/31  15:46:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:55:12  devrcs
 * 	Removed unused definitions that cause -DDEC Motif builds to complain.
 * 	[90/10/03  12:42:12  jeffc]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:51  gm]
 * 
 * Revision 1.4  90/06/22  20:54:26  devrcs
 * 	nags merge
 * 
 * 	Condensed ancient history (reverse chronology):
 * 	Turned on time locks for OSF/1.			nags@encore.com
 * 	make tv_sec and tv_usec longs			mbrown@osf.org
 * 	AIX merge first cut.				tom@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.			gm@osf.org
 * 	Early Mach 2.5 merge with MMAX_MP yields 0.6.	alan@encore.com
 * 	MMAX_MP:  time lock for mp environment.		boykin@encore.com
 * 	More cleanup.					rpd@cmu.edu
 * 	Made all code dependent on CMUCS always true.	mrt@cmu.edu
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Added declarations for __STDC__.		jjk@cmu.edu
 * 	Prevent multiple inclusions of time.h.		mwyoung@cmu.edu
 * 	Restored "struct tm" definition for kern_clock.	avie@cmu.edu
 * 	[90/06/12  21:39:31  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)time.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_TIME_H_
#define _SYS_TIME_H_

#ifdef	KERNEL
#include <map_time.h>
#endif
#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

#include <sys/types.h>

#ifdef  _KERNEL
/*
 * Structure returned by gmtime and localtime calls (see ctime(3)).
 */
struct tm {
        int     tm_sec;
        int     tm_min;
        int     tm_hour;
        int     tm_mday;
        int     tm_mon;
        int     tm_year;
        int     tm_wday;
        int     tm_yday;
        int     tm_isdst;
};
#else   /* _KERNEL  */
#include <time.h>
#endif  /*  _KERNEL  */

/*
 * The rest of this file is the interface to the BSD timer services.
 * Most of these services are implemented as subroutines that convert
 * the interface to the corresponding POSIX timer service.
 */

/*
 * The following are the BSD labels for the timer types.
 */
#define	ITIMER_REAL		0	/* Real time */
#define	ITIMER_VIRTUAL		1	/* Per-process time */
#define	ITIMER_PROF		2	/* Per-process user time */

struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* microseconds */
};

/*
 * Operations on timevals.
 *
 * Note that timercmp only works for cmp values of !=, >, and <.
 */
#define	timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timerclear(tvp)		(tvp)->tv_sec = (tvp)->tv_usec = 0
#define	timercmp(tvp, fvp, cmp)						\
	((tvp)->tv_sec cmp (fvp)->tv_sec ||				\
	 (tvp)->tv_sec == (fvp)->tv_sec &&				\
	 (tvp)->tv_usec cmp (fvp)->tv_usec)

struct	itimerval {
	struct		timeval it_interval; /* timer interval */
	struct		timeval it_value; /* current value */
};

/*
 * The following provides a way to convert the current time in GMT
 * to a local time.
 */
struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

#define DST_NONE	0	/* not on dst */
#define DST_USA		1	/* USA style dst */
#define DST_AUST	2	/* Australian style dst */
#define DST_WET		3	/* Western European dst */
#define DST_MET		4	/* Middle European dst */
#define DST_EET		5	/* Eastern European dst */
#define DST_CAN		6	/* Canada */

#ifndef _KERNEL

#ifdef _NO_PROTO
extern int adjtime();
extern int getitimer();
extern int setitimer();
extern int gettimeofday();
extern int settimeofday();
extern int utimes();
#else
extern int adjtime(struct timeval *, struct timeval *);
extern int getitimer(int, struct itimerval *);
extern int setitimer(int, struct itimerval *, struct itimerval *);
extern int gettimeofday(struct timeval *, struct timezone *);
extern int settimeofday(struct timeval *, struct timezone *);
extern int utimes(const char *, struct timeval *);
#endif  /* _NO_PROTO */
#endif  /* _KERNEL */

#ifdef	_KERNEL
extern struct	timeval time;	/* Must only be accessed inside TIME LOCK's or
				 * it will be out of date -- see time.h
				 */
#if	KERNEL && MAP_TIME
/* The struct mtime is mapped into the place where the Mach kernel
 * updates the time automatically.  Mach does not respect the time
 * locks.  Therefore, we only copy the time from the mapped time when
 * the lock is taken.  Also, we have to copy it again, if the seconds
 * field changed during the time we copied the microseconds field:
 */
extern time_value_t *mtime;
#if defined(__i860__) && defined(TNC)
#define update_time_var()	rpm_update_time_var()
extern int rpm_error;
#define update_time_var_post()	if(rpm_error) rpm_check_for_error()

#define update_time_var_from_mach()		\
				do {		\
	time.tv_sec = mtime->seconds;		\
	time.tv_usec = mtime->microseconds;	\
} while (time.tv_sec != mtime->seconds);

#else

#define update_time_var()	do {		\
	time.tv_sec = mtime->seconds;		\
	time.tv_usec = mtime->microseconds;	\
} while (time.tv_sec != mtime->seconds);

#define update_time_var_post()

#endif /* defined(__i860__) && defined(TNC) */

#else	KERNEL && MAP_TIME

void get_mach_time();
#define update_time_var()	get_mach_time()

#endif	KERNEL && MAP_TIME

udecl_simple_lock_data(,time_lock)
#define	TIME_LOCK_INIT()	usimple_lock_init(&time_lock)
#define	TIME_READ_LOCK()	{ usimple_lock(&time_lock); update_time_var(); }
#define	TIME_WRITE_LOCK()	{ usimple_lock(&time_lock); update_time_var(); }
#define	TIME_READ_UNLOCK()	{ usimple_unlock(&time_lock) ; \
					update_time_var_post() ; }
#define	TIME_WRITE_UNLOCK()	{ usimple_unlock(&time_lock) ; \
					update_time_var_post() ; }
#endif

#endif	/* _SYS_TIME_H_ */
