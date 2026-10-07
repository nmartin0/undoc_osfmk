/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*++ nqs_complt.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_complt.c,v $
 *
 * DESCRIPTION:
 *
 *	Complete a local intra-machine NQS transaction, by sending
 *	a transaction completion code to the requesting process.
 *
 *	Let's talk for a moment, shall we?  This module could have
 *	been implemented using named-pipes, or on some systems, by a
 *	socket or socket like connection, or even by datagrams.
 *
 *	However, I'm trying very hard to keep this thing PORTABLE.
 *	Also, this implementation has one principal advantage:
 *
 *		The burden of creating and opening
 *		the file is placed on the client.
 *		If the simple inter-process communication
 *		file cannot be created and opened
 *		by the client, then the client 
 *		simply does not try to perform any
 *		transactions with the NQS daemon.
 *		We find out up front, whether or
 *		not we can talk (as a local client
 *		process), to the local NQS daemon.
 *		If the supporting system is short of
 *		open file table entries, we find this
 *		right away.
 *
 *
 *	This scheme is very sleazy, and I apologize here.  If you
 *	have a better idea that is still portable, then please
 *	step forward and tell me about it!
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 8, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:50 $ $State: Exp $)
 * $Log: nqs_complt.c,v $
 * Revision 1.3  1994/11/19  02:52:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:05  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:55  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:59  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_complt.c	1.2 (nqs_complt.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqs.h"		/* Packet size definition */
#include "nqsxvars.h"		/* NQS external vars */
#include <stdio.h>
#include <errno.h>
#include <signal.h>		/* Defines signals and int (*signal())(); */
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
struct utimbuf {
	time_t acctime;		/* Access time */
	time_t modtime;		/* Modification time */
};
#else
BAD SYSTEM TYPE
#endif
#endif

extern int errno;		/* System call error# */
extern void nqs_abort();	/* NQS abort */
extern time_t time();		/* Get time */

/*** nqs_complt
 *
 *
 *	void nqs_complt():
 *
 *	Complete a local intra-machine NQS transaction, by sending
 *	a transaction completion code to the requesting process.
 */
void nqs_complt (completion_code, pid)
long completion_code;			/* Transaction completion code */
int pid;				/* Process-id of requester */
{
	static char incomplete[]
	= "W$Nqs_complt() unable to send completion code.\n";

#if	BSD42 | BSD43 | ULTRIX
	struct timeval utimbuf [2];	/* Utimes() buffer */
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	struct utimbuf utimbuf;		/* utime() buffer */
#else
BAD SYSTEM TYPE
#endif
#endif
	char path [MAX_PATHNAME+1];	/* Inter-process communication */
					/* file name */

	sprintf (path, "%s/%1d", Nqs_inter, pid);
#if	BSD42 | BSD43 | ULTRIX
	time (&utimbuf [0].tv_sec);	/* Access time = current time */
	utimbuf [0].tv_usec = 0;
	utimbuf [1].tv_sec = completion_code;
	utimbuf [1].tv_usec = 0;
	if (utimes (path, utimbuf) == -1) {
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	time (&utimbuf.acctime);	/* Access time = current time */
	utimbuf.modtime = completion_code;
	if (utime (path, &utimbuf) == -1) {
#else
BAD SYSTEM TYPE
#endif
#endif
		/*
		 *  We were not successful.
		 */
		if (errno != ENOENT) {
			/*
			 *  A very serious error has occurred; either
			 *
			 *	EPERM, ENOTDIR, EACCES, EROFS, EFAULT, or
			 *	ELOOP (Berkeley).
			 */
#if	BSD42 | BSD43 | ULTRIX
			printf ("F$Utimes() call error in nqs_complt().\n");
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
			printf ("F$Utime() call error in nqs_complt().\n");
#else
BAD SYSTEM TYPE
#endif
#endif
			nqs_abort();	/* Abort execution */
		}
		/*
		 *  The inter-process communication file did not exist.
		 *  Check to see if the client process still exists.
		 *  If the client does not exist, then we do not report
		 *  an error, and instead assume that the client has
		 *  exited prematurely, without removing their inter-
		 *  process communication file.
		 */
		if (kill (pid, 0) != -1) {
			/*
			 *  The client process still exists, but the
			 *  inter-process communication file isn't there,
			 *  or we were given the wrong process-id.
			 */
			printf ("E$Wrong pid or missing ");
			printf ("inter-process comm. file.\n");
			printf (incomplete);
			fflush (stdout);
		}
	}
	else if (kill (pid, SIGALRM) == -1) {
		/*
		 *  We successfully altered the access and modification
		 *  times of the inter-process communication file, but
		 *  were unable to signal the client process that we had
		 *  completed the transaction.
		 */
		if (errno == EINVAL) {
			printf ("E$Invalid pid given to nqs_complt().\n");
			printf (incomplete);
			fflush (stdout);
		}
		else if (errno == ESRCH) {
			/*
			 *  The client process no longer exists to
			 *  listen to our results.
			 */
			unlink (path);	/* Remove the inter-process */
					/* communication file for the */
					/* exited process. */
		}
		else if (errno == EPERM) {
			printf ("F$NQS not running as root in ");
			printf ("nqs_complt().\n");
			nqs_abort();
		}
	}
}
