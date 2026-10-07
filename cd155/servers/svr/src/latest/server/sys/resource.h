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
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: resource.h,v $
 * Revision 1.4  1994/11/18  20:41:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:59  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:10:45  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:44:39  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:53:15  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:08:48  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:43:48  gm]
 * 
 * 	fix definition of RLIM_INFINITY
 * 	[90/09/19  08:08:45  garyf]
 * 
 * Revision 1.4  90/03/13  20:49:41  mbrown
 * 	AIX merge first cut.
 * 	[90/02/09  18:03:33  tom]
 * 
 * Revision 1.3  90/01/03  12:43:54  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:36  gm]
 * 
 * Revision 1.2  89/12/26  10:37:58  gm
 * 	BSD4.4 changes.
 * 	[89/12/25            gm]
 * 
 * Revision 6.1  89/07/26  14:22:23  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * 	Merged MMAX_MP with Release 2.5.
 * 	Encore contributors to date:
 * 		Joseph Boykin		(boykin@encore.com)
 * 
 * Revision 2.7  89/03/09  22:07:10  rpd
 * 	More cleanup.
 * 
 * Revision 2.6  89/02/25  17:55:54  gm0w
 * 	Made CMUCS conditionals always true.
 * 	[89/02/14            mrt]
 * 
 * Revision 2.5  88/10/11  12:08:31  rpd
 * 	Added include of time.h since this file uses
 * 	struct timeval.
 * 	[88/10/11  11:41:22  rpd]
 * 
 * Revision 2.4  88/08/24  02:41:41  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:21:30  mwyoung]
 * 
 * Revision 2.3  88/08/22  21:29:44  mja
 * 	Add RUSAGE_NODEV definition.
 * 	[88/08/11  19:18:02  mja]
 * 
 * Revision 2.2  88/07/15  15:59:57  mja
 * 	Condensed local conditionals.
 * 
 * Revision 0.0  88/01/06            jjk
 * 	Made file reentrant.  Added declarations for __STDC__.
 * 	[88/01/06            jjk]
 * 
 * Revision 0.0  87/01/28            mja
 * 	CMUCS: Defined new rusage_dev structure for special
 * 	wait3() option;
 * 	CMUCS: Added RPAUSE definitions.
 * 	[ V5.1(F1) ]
 * 	[87/01/28            mja]
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
 *	@(#)resource.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#include <sys/time.h>
#include <sys/types.h>

/*
 * Process priority specifications to get/setpriority.
 */
#define PRIO_MIN	-20
#define PRIO_MAX	20

#define PRIO_PROCESS	0
#define PRIO_PGRP	1
#define PRIO_USER	2

/*
 * Resource utilization information.
 */

#define RUSAGE_SELF	0
#define RUSAGE_CHILDREN	-1

struct	rusage {
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
	long	ru_maxrss;
#define ru_first	ru_ixrss
	long	ru_ixrss;		/* integral shared memory size */
	long	ru_idrss;		/* integral unshared data " */
	long	ru_isrss;		/* integral unshared stack " */
	long	ru_minflt;		/* page reclaims */
	long	ru_majflt;		/* page faults */
	long	ru_nswap;		/* swaps */
	long	ru_inblock;		/* block input operations */
	long	ru_oublock;		/* block output operations */
	long	ru_msgsnd;		/* messages sent */
	long	ru_msgrcv;		/* messages received */
	long	ru_nsignals;		/* signals received */
	long	ru_nvcsw;		/* voluntary context switches */
	long	ru_nivcsw;		/* involuntary " */
#define ru_last		ru_nivcsw
};

/*
 * Resource limits
 */
#define RLIMIT_CPU	0		/* cpu time in milliseconds */
#define RLIMIT_FSIZE	1		/* maximum file size */
#define RLIMIT_DATA	2		/* data size */
#define RLIMIT_STACK	3		/* stack size */
#define RLIMIT_CORE	4		/* core file size */
#define RLIMIT_RSS	5		/* resident set size */

#define RLIM_NLIMITS	6		/* number of resource limits */

#define	RLIM_INFINITY	0x7fffffff

struct rlimit {
	unsigned int	rlim_cur;	/* current (soft) limit */
	unsigned int	rlim_max;	/* maximum value for rlim_cur */
};


/*
 *  Special rusage structure returned with WLOGINDEV option to wait3().
 */

struct rusage_dev {
	struct rusage ru_rusage;
	dev_t	      ru_dev;
};

#define RUSAGE_NODEV	((dev_t)-1)	/* same as NODEV */


/*
 *  Resource pause system call definitions
 */

#define RPAUSE_SAME	0		/* leave state unchanged */
#define RPAUSE_DISABLE	1		/* disable pause on error type(s) */
#define RPAUSE_ENABLE	2		/* enable pause on error type(s) */

#define RPAUSE_ALL	0x7fffffff	/* all error number types */

#ifndef _KERNEL
#ifdef 	_NO_PROTO
extern int getpriority();
extern int setpriority();
extern int getrlimit();
extern int setrlimit();
extern int getrusage();
#else	/* _NO_PROTO */
extern int getpriority(int, int);
extern int setpriority(int, int, int);
extern int getrlimit(int, struct rlimit *);
extern int setrlimit(int, struct rlimit *);
extern int getrusage(int, struct rusage *);
#endif	/* _NO_PROTO */
#endif	/* _KERNEL */

#endif	/* _SYS_RESOURCE_H_ */
