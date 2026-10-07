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
 * $Log: syslog.h,v $
 * Revision 1.4  1994/11/18  20:41:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:04  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:13:47  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:45:39  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:54:45  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:05  gm]
 * 
 * Revision 1.5  90/06/29  13:54:08  devrcs
 * 	Removed ifdef BSD44 references (inclusive).
 * 	[90/06/26  11:26:53  gmf]
 * 
 * Revision 1.4  90/03/13  20:49:57  mbrown
 * 	put argument for openlog() back. Bad source reference.
 * 	[90/02/16  09:30:47  mbrown]
 * 
 * 	changed # of args of openlog()
 * 	[90/02/15  13:43:27  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:05:07  tom]
 * 
 * Revision 1.3  90/01/02  20:16:04  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:40:41  gm
 * 	Integrated 4.4BSD changes.
 * 	[89/12/25            noemi]
 * 
 * Revision 2.3  89/03/09  22:08:04  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:56:39  gm0w
 * 	Made MACH and CMUCS code unconditional and eliminated
 * 	non-MACH code.
 * 	[89/02/14            mrt]
 * 
 * Revision 0.0  88/01/06            jjk
 * 	Added declarations for __STDC__.
 * 	[88/01/06            jjk]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)syslog.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_SYSLOG_H_
#define _SYS_SYSLOG_H_

/*
 *  Facility codes
 */

#define LOG_KERN	(0<<3)	/* kernel messages */
#define LOG_USER	(1<<3)	/* random user-level messages */
#define LOG_MAIL	(2<<3)	/* mail system */
#define LOG_DAEMON	(3<<3)	/* system daemons */
#define LOG_AUTH	(4<<3)	/* security/authorization messages */
#define LOG_SYSLOG	(5<<3)	/* messages generated internally by syslogd */
#define LOG_LPR		(6<<3)	/* line printer subsystem */
#define LOG_NEWS	(7<<3)	/* network news subsystem */
#define LOG_UUCP	(8<<3)	/* UUCP subsystem */
#define LOG_CRON	(9<<3)	/* clock daemon */
	/* other codes through 15 reserved for system use */
#define LOG_LOCAL0	(16<<3)	/* reserved for local use */
#define LOG_LOCAL1	(17<<3)	/* reserved for local use */
#define LOG_LOCAL2	(18<<3)	/* reserved for local use */
#define LOG_LOCAL3	(19<<3)	/* reserved for local use */
#define LOG_LOCAL4	(20<<3)	/* reserved for local use */
#define LOG_LOCAL5	(21<<3)	/* reserved for local use */
#define LOG_LOCAL6	(22<<3)	/* reserved for local use */
#define LOG_LOCAL7	(23<<3)	/* reserved for local use */

#define LOG_NFACILITIES	24	/* maximum number of facilities */
#define LOG_FACMASK	0x03f8	/* mask to extract facility part */

#define LOG_FAC(p)	(((p) & LOG_FACMASK) >> 3)	/* facility of pri */

/*
 *  Priorities (these are ordered)
 */

#define LOG_EMERG	0	/* system is unusable */
#define LOG_ALERT	1	/* action must be taken immediately */
#define LOG_CRIT	2	/* critical conditions */
#define LOG_ERR		3	/* error conditions */
#define LOG_WARNING	4	/* warning conditions */
#define LOG_NOTICE	5	/* normal but signification condition */
#define LOG_INFO	6	/* informational */
#define LOG_DEBUG	7	/* debug-level messages */

#define LOG_PRIMASK	0x0007	/* mask to extract priority part (internal) */
#define LOG_PRI(p)	((p) & LOG_PRIMASK)	/* extract priority */

#define	LOG_MAKEPRI(fac, pri)	(((fac) << 3) | (pri))

#ifdef _KERNEL
#define LOG_PRINTF	-1	/* pseudo-priority to indicate use of printf */
#endif

/*
 * arguments to setlogmask.
 */
#define LOG_MASK(pri)	(1 << (pri))		/* mask for one priority */
#define LOG_UPTO(pri)	((1 << ((pri)+1)) - 1)	/* all priorities through pri */

/*
 *  Option flags for openlog.
 *
 *	LOG_ODELAY no longer does anything; LOG_NDELAY is the
 *	inverse of what it used to be.
 */
#define LOG_PID		0x01	/* log the pid with each message */
#define LOG_CONS	0x02	/* log on the console if errors in sending */
#define LOG_ODELAY	0x04	/* delay open until syslog() is called */
#define LOG_NDELAY	0x08	/* don't delay open */
#define LOG_NOWAIT	0x10	/* if forking to log on console, don't wait() */
#define	LOG_PERROR	0x20	/* log to stderr as well */

#ifndef	_KERNEL
#ifdef _NO_PROTO
extern int openlog();
extern int syslog();
extern void closelog();
extern int setlogmask();
#else
extern int openlog(const char *, int, int);
extern int syslog(int, const char *, ...);
extern void closelog(void);
extern int setlogmask(int);
#endif	/* _NO_PROTO */
#endif	/* ! _KERNEL */

#endif	/* _SYS_SYSLOG_H_ */
