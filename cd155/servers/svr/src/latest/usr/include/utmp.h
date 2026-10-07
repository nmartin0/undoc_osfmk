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
 * HISTORY
 * $Log: utmp.h,v $
 * Revision 1.3  1994/11/18  21:11:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:45  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:22:00  nandy
 * Initial revision
 *
 * Revision 2.7  92/04/13  14:40:20  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.7  90/10/07  20:39:05  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:02:50  gm]
 * 
 * Revision 2.6  90/08/09  14:28:59  devrcs
 * 	Changes for reentrant C library
 * 	[90/08/02  09:58:39  sp]
 * 
 * Revision 2.5  90/07/27  10:44:21  devrcs
 * 	More gcc changes.
 * 	[90/07/16  01:37:27  gm]
 * 
 * 	Add function prototypes for all the calls that manipulate the
 * 	utmp file.
 * 	[90/07/12  08:17:48  sp]
 * 
 * Revision 2.4  90/06/22  21:52:54  devrcs
 * 	Fixed the run level message.
 * 	[90/06/11  16:26:53  tom]
 * 
 * 	Removed BSD_COM macro so everyone uses the real utmp structure.
 * 	Increased hostname length to match sys/param.h.
 * 	Increased username length.
 * 	[90/05/31  17:39:54  tom]
 * 
 * Revision 2.3  90/03/13  21:24:07  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  18:04:13  tom]
 * 
 * Revision 2.2  90/01/02  19:15:55  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)utmp.h	1.8  com/inc,3.1,8943 10/11/89 17:45:38 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)utmp.h	5.1 (Berkeley) 5/30/85
 */
#ifndef _UTMP_H_
#define _UTMP_H_

#define UTMP_FILE       "/var/adm/utmp"
#define WTMP_FILE       "/var/adm/wtmp"

#include <sys/types.h>	/* for pid_t, time_t */

/*
 * Structure of utmp and wtmp files.
 *
 * Assuming these numbers is unwise.
 */


#define ut_name ut_user 		/* compatability */
struct utmp
  {
	char ut_user[32] ;		/* User login name */
	char ut_id[14] ;		/* /etc/inittab id- IDENT_LEN in init */
	char ut_line[32] ;		/* device name (console, lnxx) */
	short ut_type ; 		/* type of entry */
	pid_t ut_pid ;			/* process id */
	struct exit_status
	  {
	    short e_termination ;	/* Process termination status */
	    short e_exit ;		/* Process exit status */
	  }
	ut_exit ;			/* The exit status of a process
					 * marked as DEAD_PROCESS.
					 */
	time_t ut_time ;		/* time entry was made */
	char ut_host[64] ;		/* host name same as MAXHOSTNAMELEN */
  } ;


/*	Definitions for ut_type						*/

#define	EMPTY		0
#define	RUN_LVL		1
#define	BOOT_TIME	2
#define	OLD_TIME	3
#define	NEW_TIME	4
#define	INIT_PROCESS	5	/* Process spawned by "init" */
#define	LOGIN_PROCESS	6	/* A "getty" process waiting for login */
#define	USER_PROCESS	7	/* A user process */
#define	DEAD_PROCESS	8
#define	ACCOUNTING	9

#define	UTMAXTYPE	ACCOUNTING	/* Largest legal value of ut_type */

/*	Special strings or formats used in the "ut_line" field when	*/
/*	accounting for something other than a process.			*/
/*	No string for the ut_line field can be more than 11 chars +	*/
/*	a NULL in length.						*/

#define RUNLVL_MSG      "run-level %c"
#define	BOOT_MSG	"system boot"
#define	OTIME_MSG	"old time"
#define	NTIME_MSG	"new time"


#ifdef	_NO_PROTO
struct utmp	*getutent();
struct utmp	*getutid();
struct utmp	*getutline();
struct utmp	*pututline();
void		setutent();
void		endutent();
void		utmpname();
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern	int	getutent_r();
extern	int	getutid_r();
extern	int	getutline_r();
extern	int	pututline_r();
#endif
#else	/* _NO_PROTO */
struct utmp	*getutent(void);
struct utmp	*getutid(struct utmp *);
struct utmp	*getutline(struct utmp *);
struct utmp	*pututline(struct utmp *);
void		setutent(void);
void		endutent(void);
void		utmpname(char *);
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern	int	getutent_r(struct utmp *);
extern	int	getutid_r(struct utmp *, struct utmp  *);
extern	int	getutline_r(struct utmp *, struct utmp *);
extern	int	pututline_r(struct utmp *);
#endif
#endif	/* _NO_PROTO */
#endif  /* _UTMP_H_ */
