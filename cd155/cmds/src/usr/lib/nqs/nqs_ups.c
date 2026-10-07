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
 
/*++ nqs_ups.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_ups.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS shutdown state update module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	June 16, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:30 $ $State: Exp $)
 * $Log: nqs_ups.c,v $
 * Revision 1.3  1994/11/19  02:53:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:51  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:52  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:20  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_ups.c	1.4 (nqs_ups.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <signal.h>
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>		/* LOCK_UN */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif

#include <errno.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */


extern void nqs_aboque();	/* Abort all reqs in specified queue */


/*** ups_shutdown
 *
 *
 *	long ups_shutdown():
 *
 *	Prepare to shutdown.
 *
 *	Non-Berkeley based UNIX implementations must disable group and
 *	others write-access on the named-pipe used to accept message
 *	packets from other processes.  The disabling of group and other
 *	write-access is critical for NQS to shutdown properly, rebuffing
 *	new message packets from client processes (see fstat() call in
 *	../lib/inter.c).
 *
 *	Berkeley UNIX based implementations release the advisory exclusive
 *	access lock on the Queuefile to accomplish the same thing.
 *
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_SHUTERROR:	if some error has prevented
 *				the shutdown sequence from
 *				being started.
 */
long ups_shutdown (pid, ruid, grace_period)
int pid;				/* Pid of requesting process.	*/
uid_t ruid;				/* Real UID of requesting process. */
int grace_period;			/* Number of seconds warning to */
					/* each running request. */
{
	register struct queue *queue;	/* Used to queue walking */

	if (Shutdown) return (TCML_COMPLETE);
					/* Ignore multiple shutdown */
					/* requests.  */
	/*
	 *  Record who asked us to stop.
	 */
	printf ("I$Received shutdown request from process %1d.\n", pid);
	printf ("I$Process real user-id is: %1d.\n", ruid);
	fflush (stdout);
	/*
	 *  Broadcast SIGTERM to all NQS shepherd processes, and to the
	 *  NQS network daemon (if one is present).  It is CRITICAL that
	 *  all NQS shepherd processes be notified of the impending
	 *  shutdown PRIOR to aborting all queues (see the loops below).
	 *  Otherwise, the shepherd processes will not know if the
	 *  signal death of the request server process or shell is
	 *  because of an NQS shutdown.
	 */
#if	SGI | SYS52 | UTS | UNICOS | OSF
	
	if (0 != kill (-getpid(), SIGTERM)) {
		printf ("E$ kill(%d,%d) failed, errno=%d\n",
		-getpid(), SIGTERM, errno);      /* Signal process-group */
	}
	/*
	 *  Turn off ALL write permissions to the NQS pipe "mailbox" so
	 *  that other processes can tell that the NQS daemon is shutting
	 *  down.
	 */
	if (chmod (Nqs_ffifo, 00600) == -1) {
		/*
		 *  We were unable to change the mode of the NQS
		 *  daemon's request pipe!
		 */
		printf ("E$Shutdown request error.\n");
		printf ("I$Unable to chmod() on the NQS request FIFO.\n");
		printf ("I$File=%s.\n", Nqs_ffifo);
		printf ("I$%s.\n", asciierrno());
		fflush (stdout);
		return (TCML_SHUTERROR);	/* Shutdown error */
	}
#else
#if	BSD42 | BSD43
	killpg (getpid(), SIGTERM);	/* Signal process-group */
	/*
	 *  Release the advisory exclusive access lock on the Queuefile,
	 *  so that other NQS client processes can tell that NQS is shutdown,
	 *  or is in the process of doing so.
	 *
	 *  The advisory exclusive access lock on the Qmapfile however,
	 *  must be retained until the bloody end (see ../src/nqs_boot.c).
	 */
	flock (Queuefile->fd, LOCK_UN);	/* Release advisory exclusive lock */
#else
#if	ULTRIX
	killpg (getpid(), SIGTERM);	/* Signal process-group */
	/*
	 *  Turn off ALL write permissions to the NQS pipe "mailbox", and
	 *  release the advisory exclusive access lock on the Queuefile, so
	 *  that other processes can tell that the NQS daemon is shutting
	 *  down.
	 */
	if (chmod (Nqs_ffifo, 00600) == -1) {
		/*
		 *  We were unable to change the mode of the NQS
		 *  daemon's request pipe!
		 */
		printf ("E$Shutdown request error.\n");
		printf ("I$Unable to chmod() on the NQS request FIFO.\n");
		printf ("I$File=%s.\n", Nqs_ffifo);
		printf ("I$%s.\n", asciierrno());
		fflush (stdout);
		return (TCML_SHUTERROR);	/* Shutdown error */
	}
	/*
	 *  The advisory exclusive access lock on the Qmapfile however,
	 *  must be retained until the bloody end (see ../src/nqs_boot.c).
	 */
	flock (Queuefile->fd, LOCK_UN);	/* Release advisory exclusive lock */
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
	close (Write_fifo);		/* Close the file descriptor we had */
					/* open to prevent exiting when no  */
					/* req. activity was taking place.  */
					/* We do blocking reads for requests*/
					/* until ALL other processes close  */
					/* their write-descriptors on the   */
					/* mailbox request pipe.	    */
	/*
	 *  For each queue, perform the equivalent of an ABort Queue command,
	 *  with the exception that requests are to be requeued.
	 */
	Shutdown = 1;			/* Signal that no more reqs are to */
					/* be spawned.  We are shutting down*/
	queue = Nonnet_queueset;	/* Walk the non-network queues */
	while (queue != (struct queue *) 0) {
		nqs_aboque (queue, grace_period, 1);	/* Requeue req if */
		queue = queue->next;			/* possible */
	}
	queue = Net_queueset;		/* Walk the network queues */
	while (queue != (struct queue *) 0) {
		nqs_aboque (queue, grace_period, 1);	/* Requeue req if */
		queue = queue->next;			/* possible */
	}
	return (TCML_COMPLETE);		/* Return successful completion */
}
