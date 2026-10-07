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
 * $Log: wait.h,v $
 * Revision 1.4  1994/11/18  20:42:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:31:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:40:50  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:56  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:39  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/10/17  18:26:31  cfj
 * IPD & Allocator Support.
 *
 * Revision 2.3  1992/10/16  18:16:37  cfj
 * Modification so that NX programs can run.
 *
 * Revision 2.2  1991/08/31  14:17:15  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/08/07  17:17:30  jose
 *  Unchanged version from osc1.0.2
 * 
 * Revision 1.9.2.3  90/12/20  15:02:05  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.9.2.2  90/12/14  15:58:36  dwm
 * 	POSIX bug 1678 - name space pollution
 * 	[90/12/13  13:14:19  dwm]
 * 
 * Revision 1.9  90/10/07  14:57:28  devrcs
 * 	fixed name-space pollution reported by VSX in POSIX-Only mode
 * 	[90/10/04  07:49:01  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:50:28  gm]
 * 
 * Revision 1.8  90/07/17  11:42:51  devrcs
 * 	Updated to use bsd4.4 style W* macros.
 * 	[90/07/07  15:39:23  gm]
 * 
 * Revision 1.7  90/06/22  20:54:53  devrcs
 * 	correct WIF undefs
 * 	[90/05/30  14:54:58  mbrown]
 * 
 * Revision 1.6  90/05/24  23:06:54  devrcs
 * 	Fixed WIFSTOPPED macros.
 * 	[90/05/06            gm]
 * 
 * Revision 1.5  90/04/27  19:28:28  devrcs
 * 	Made BSD-compatible definitions of WIF* macros invisible in the kernel.
 * 	[90/04/23  15:03:56  coren]
 * 
 * 	Made BSD-compatible structure definition ANSI-compliant.
 * 	Took definition of 'union wait' out of enclosing #ifdef _BSD".
 * 	Added parentheses around macro arguments.
 * 	[90/04/19  08:24:11  coren]
 * 
 * Revision 1.4  90/03/13  20:50:33  mbrown
 * 	prototyping removal of BSD wait()
 * 	[90/02/25  17:26:42  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:08:31  tom]
 * 
 * Revision 1.3  90/02/16  16:53:08  devrcs
 * 	Upgraded for POSIX 1003.1 compliance.
 * 	[90/01/25  18:54:38  coren]
 * 
 * Revision 1.2  90/01/02  20:16:24  gm
 * 	     Fixes for first snapshot.
 * 
 * $EndLog$
 */
/* @(#)wait.h   1.13  com/inc/sys,3.1,9013 1/26/90 16:14:52 */
/*
 * NAME:  Waitpid, wait, and wait3 system call constants and definitions 
 *
 * ORIGIN: BSD, IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */

#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <standards.h>

#include <sys/types.h>
#include <machine/endian.h>

/*
 * This file holds definitions relevent to the waitpid(), wait(), and wait3()
 * system calls.  The rusage option is only available with the wait3() call.
 * The options field in wait3() and waitpid() determines the behavior of the
 * call, while the process ID field in the waitpid() call determines which
 * group of children to search.
 */

/*
 * POSIX requires that certain values be included in wait.h.  It also
 * requires that when _POSIX_SOURCE is defined only those standard
 * specific values are present.  This header includes all the POSIX
 * required entries.
 */
#ifdef _POSIX_SOURCE

/*
 * If the user defines _BSD, they are obviously not looking for
 * POSIX definitions with respect to wait, so give 'em the BSD
 * interface.
 *
 */
#ifdef _BSD			 /* BSD definition of wait() */
extern int wait();
#else			 	/* POSIX definition of wait() */
#ifdef _NO_PROTO
extern pid_t wait();
#else
extern pid_t wait(int *);
#endif /* _NO_PROTO */
#endif /* _BSD */

/* 
 * waitpid is not in "old" BSD so that goes here 
 */

#ifdef _NO_PROTO
extern pid_t waitpid();
#else
extern pid_t waitpid(pid_t , int *, int );
#endif /* _NO_PROTO */

/*
 * The option field for wait3() and waitpid() is defined as follows:
 * WNOHANG causes the wait to not hang if there are no stopped or terminated
 * processes, rather returning an error indication in this case (pid==0).
 * WUNTRACED indicates that the caller should receive status about untraced
 * children which stop due to signals.  If children are stopped and a wait
 * without this option is done, it is as though they were still running...
 * nothing about them is returned.
 *
 */
#define WNOHANG		0x1	/* dont hang in wait			 */
#define WUNTRACED	0x2	/* tell about stopped, untraced children */
#define WNOWAIT         0x4     /* Get status but leave the process such
                                   that wait can be called again and
                                   receive the same status. */



/*
 * Stopped process status.  Returned only for traced children unless requested
 * with the WUNTRACED option bit.  Lower byte gives the reason, next byte is
 * the last signal received, i.e. p->p_cursig.
 */
#define	_W_STOPPED	0177	/* bit set if stopped		*/

/*
 * MACRO defines for application interfacing to waitpid(), wait(), and wait3()
 */
#ifdef _BSD
#define	_W_INT(w)	(*(int *)&(w))	/* convert union wait to int */
#else
#define	_W_INT(i)	(i)
#endif

#define	_WSTATUS(x)	(_W_INT(x) & _W_STOPPED)
/* evaluates to a non-zero value if status returned for a stopped child	*/
#define WIFSTOPPED(x)	(_WSTATUS(x) == _W_STOPPED)
/* evaluates to the number of the signal that caused the child to stop	*/
#define WSTOPSIG(x)	(WIFSTOPPED(x) ? ((_W_INT(x) >> 8) & 0177) : -1)
/* evaluates to a non-zero value if status returned for normal termination */
#define WIFEXITED(x)	(_WSTATUS(x) == 0)
/* evaluates to the low-order 8 bits of the child exit status	*/
#define WEXITSTATUS(x)	(WIFEXITED(x) ? ((_W_INT(x) >> 8) & 0377) : -1)
/* evaluates to a non-zero value if status returned for abnormal termination */
#define WIFSIGNALED(x)	(_WSTATUS(x) != _W_STOPPED && _WSTATUS(x) != 0)
/* evaluates to the number of the signal that caused the child to terminate */
#define WTERMSIG(x)	(WIFSIGNALED(x) ? _WSTATUS(x) : -1)

#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE

#include <sys/param.h>

/*
 * Option for wait3(), mach extention.
 *
 */
#define WLOGINDEV       0x8000  /* tell about login device for child */

#ifdef _BSD

/* define for BSD compatibility					*/

#define WCOREFLAG       0200

#define WCOREDUMP(x)    ((x) & WCOREFLAG)

#define W_EXITCODE(ret, sig)    ((ret) << 8 | (sig))
#define W_STOPCODE(sig)		((sig) << 8 | _W_STOPPED)

#define	WSTOPPED	_W_STOPPED

/*
 * Tokens for special values of the "pid" parameter to wait4.
 */
#define WAIT_ANY	(-1)    /* any process */
#define WAIT_MYPGRP     0       /* any process in my process group */

/*
 * Use of this union is deprecated
 */
union wait
{
	int	w_status;		/* used in syscall		*/

	struct				/* terminated process status	*/
	{
#if     BYTE_ORDER == BIG_ENDIAN
		unsigned short  w_PAD16;
                unsigned        w_Retcode:8;    /* exit code if w_termsig==0 */
                unsigned        w_Coredump:1;   /* core dump indicator */
                unsigned        w_Termsig:7;    /* termination signal */
#else
                unsigned 	w_Termsig:7;    /* termination signal */
                unsigned 	w_Coredump:1;   /* core dump indicator */
                unsigned 	w_Retcode:8;    /* exit code if w_termsig==0 */
                unsigned short  w_PAD16;
#endif
	} w_T;
	/*
         * Stopped process status.  Returned
         * only for traced children unless requested
         * with the WUNTRACED option bit.
         */
        struct {
#if     BYTE_ORDER == BIG_ENDIAN
                unsigned short  w_PAD16;
                unsigned        w_Stopsig:8;    /* signal that stopped us */
                unsigned        w_Stopval:8;    /* == _W_STOPPED if stopped */
#else
                unsigned	w_Stopval:8;    /* == _W_STOPPED if stopped */
                unsigned	w_Stopsig:8;    /* signal that stopped us */
                unsigned short  w_PAD16;
#endif
	} w_S;
};

#define	w_termsig	w_T.w_Termsig
#define w_coredump	w_T.w_Coredump
#define w_retcode	w_T.w_Retcode
#define w_stopval	w_S.w_Stopval
#define w_stopsig	w_S.w_Stopsig

#ifndef _NO_PROTO
#include <sys/resource.h>   /* contains rusage struct for wait3 */
extern int wait3(union wait *, int, struct rusage *);
#else    
extern int wait3();
#endif  /* !_NO_PROTO */

#endif /* _BSD */

#endif /* _OSF_SOURCE */

#endif /* _SYS_WAIT_H_ */
