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
 
/*++ nqs_reqexi.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_reqexi.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains the procedure:  nqs_reqexi(), which
 *	is invoked by the shepherd process created in nqs_spawn(),
 *	when a request has ended execution.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 14, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:53:13 $ $State: Exp $)
 * $Log: nqs_reqexi.c,v $
 * Revision 1.5  1994/11/19  02:53:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/02  23:01:35  mwan
 * Fixed PTS #7838 and #7858
 *
 *  Reviewer: jkearns
 *  Risk: L
 *  Benefit or PTS #: 7838,7858
 *  Testing:
 *  Module(s): usr/include/nqs/requestcc.h, usr/include/nqs/buddy.h,
 *                    usr/ccs/lib/libnqs/rcmmsgs.c,
 *                    usr/lib/nqs/nqs_reqser.c, usr/lib/nqs/nqs_reqexi.c,
 *                    usr/lib/nqs/nqs_reqcom.c, usr/lib/nqs/nqs_spawn.c
 * 		   usr/lib/macs_rootp.c
 *
 * Revision 1.3  1993/05/28  19:58:21  mwan
 * Change to mail bad dedicated time slot message
 *
 * Revision 1.2  1992/10/09  22:26:10  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:30  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:52  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_reqexi.c	1.2 (nqs_reqexi.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqspacket.h"			/* NQS local message packet types */
#include "nqsmail.h"			/* NQS mail definitions */
#include "informcc.h"			/* NQS information completion */
					/* codes and masks */
#include "requestcc.h"			/* NQS request completion codes */

#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <sys/times.h>			/* Get definition of struct tms */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#include <sys/resource.h>		/* Get definition of struct rusage */
#else
BAD SYSTEM TYPE
#endif
#endif

/*
 *	#defines local to this module.
 */
#define	VBAT	000001			/* Valid RCM_ for batch queue */
#define	VDEV	000002			/* Valid RCM_ for device queue */
#define	VNET	000004			/* Valid RCM_ for network queue */
#define	VPIP	000010			/* Valid RCM_ for pipe queue */


/*
 *
 *	External functions and variables.
 */
extern void analyzercm();		/* Analyze request completion code */
extern char *asciierrno();		/* Errno to ASCII string conversion */
extern char *fmtmidname();		/* Format name for machine-id */
extern int inter();			/* Send message packet */
extern void interclear();		/* Clear message packet */
extern void interw32i();		/* Write integer datum into packet */
extern void interwbytes();		/* Write byte string into packet */
extern void interwstr();		/* Write string into packet */
extern int mai_outfiles();		/* Boolean return true if mail */
					/* should be sent because the */
					/* disposition of batch request */
					/* output files deserve comment */
extern int mai_send();			/* Send mail concerning request */
extern int tra_setstate();		/* Set request transaction state */


/*** nqs_reqexi
 *
 *
 *	void nqs_reqexi():
 *
 *	This module contains the procedure:  nqs_reqexi(), which
 *	is invoked by the shepherd process created in nqs_spawn(),
 *	when a request has ended execution.
 */
void nqs_reqexi (request, transaction, rawreq, requestlog, device)
struct request *request;		/* Request structure for request */
struct transact *transaction;		/* Transaction for request */
register struct rawreq *rawreq;		/* Ptr to raw request structure */
					/* for request */
register struct requestlog *requestlog;	/* Mail request log record */
struct device *device;			/* Device used to satisfy request */
					/* (can be NIL for non-device */
					/*  requests) */
{
	void reqcom();			/* Send request completion packet */
	void shoreqmsg();		/* Show request message on log */
	void stopentity();		/* Requeue request and stop device */
					/* or queue as appropriate */

	static short valid[] = {
		VBAT,				/* RCM_2MANYENVARS */
			VDEV |	VNET |	VPIP,	/* RCM_2MANYSVARGS */
		VBAT |	VDEV,			/* RCM_ABORTED */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_BADCDTFIL */
			VDEV |	VNET |	VPIP,	/* RCM_BADSRVARG */
/*
 *	For the current ****TRIAGE**** implementation, pipe queue
 *	servers are permitted to return RCM_DELIVERED, RCM_DELIVEREXP,
 *	RCM_DELIVERFAI, and RCM_DELIVERRETX.
 */
				VNET |	VPIP,	/* RCM_DELIVERED */
				VNET |	VPIP,	/* RCM_DELIVEREXP */
				VNET |	VPIP,	/* RCM_DELIVERFAI */
				VNET |	VPIP,	/* RCM_DELIVERRETX */
			VDEV,			/* RCM_DEVOPEFAI */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_ENFILERUN */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_ENOSPCRUN */
		0,				/* RCM_EXECUTING */
		VBAT |	VDEV,			/* RCM_EXITED */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_INSUFFMEM */
				VNET |	VPIP,	/* RCM_INTERRUPTED */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_MIDUNKNOWN */
				VNET,		/* RCM_NETREQDEL */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_NOACCAUTH */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_NOMOREPROC */
				VNET |	VPIP,	/* RCM_NONSECPORT */
		0,				/* RCM_NORESTART */
			VDEV |	VNET |	VPIP,	/* RCM_NOSVRETCODE */
					VPIP,	/* RCM_PATHLEN */
					VPIP,	/* RCM_PIPREQDEL */
		0,				/* RCM_REBUILDFAI */
					VPIP,	/* RCM_REQCOLLIDE */
				VNET |	VPIP,	/* RCM_RETRYLATER */
					VPIP,	/* RCM_ROUTED */
					VPIP,	/* RCM_ROUTEDLOC */
					VPIP,	/* RCM_ROUTEEXP */
					VPIP,	/* RCM_ROUTEFAI */
					VPIP,	/* RCM_ROUTERETX */
			VDEV |	VNET |	VPIP,	/* RCM_SERBRKPNT */
			VDEV |	VNET |	VPIP,	/* RCM_SEREXEFAI */
				VNET |	VPIP,	/* RCM_SERVESIGERR */
		VBAT,				/* RCM_SHEXEF2BIG */
		VBAT |	VDEV,			/* RCM_SHUTDNABORT */
		VBAT |	VDEV,			/* RCM_SHUTDNREQUE */
		VBAT,				/* RCM_SSHBRKPNT */
		VBAT,				/* RCM_SSHEXEFAI */
				VNET,		/* RCM_STAGEOUT */
				VNET,		/* RCM_STAGEOUTBAK */
				VNET,		/* RCM_STAGEOUTFAI */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_UNABLETOEXE */
		VBAT |	VDEV |	VNET |	VPIP,	/* RCM_UNAFAILURE */
		VBAT,				/* RCM_UNCRESTDERR */
		VBAT,				/* RCM_UNCRESTDOUT */
		0,				/* RCM_UNDEFINED */
		VBAT,				/* RCM_USHBRKPNT */
#ifndef SDSC
		VBAT				/* RCM_USHEXEFAI */
#else
		VBAT,				/* RCM_USHEXEFAI */
		VBAT |			VPIP,	/* RCM_BADTSCHED */
		VBAT 				/* RCM_NQSABORT */
#endif
	};

	static char putin_failed[]="E$Placing request in failed directory.\n";
	static char being_deleted[]="I$Request being deleted.\n";
	static char stopallque[]
		= "I$Stopping all queues to conserve remaining resources.\n";


	register short reason;		/* Reason bits of completion code */
	register short endmail;		/* Send mail on request end flag */
	register struct queue *serverq;	/* Queue in which request was running*/


	/*
	 *  Begin:
	 *  ------
	 *  Validate completion code.
	 */
	serverq = request->queue;	/* Server queue */
	reason = (requestlog->svm.rcm & XCI_REASON_MASK);
	if (reason < 0 || reason > (RCM_MAXRCM & XCI_REASON_MASK) ||
	   (requestlog->svm.rcm & XCI_TRANSA_MASK)) {
		/*
		 *  The request completion code is invalid.
		 */
		reason = (RCM_UNDEFINED & XCI_REASON_MASK);
		requestlog->svm.rcm = RCM_UNDEFINED;
	}
	endmail = (rawreq->flags & RQF_ENDMAIL);
	switch (serverq->q.type) {
	case QUE_BATCH:
		if (!(valid [reason] & VBAT)) {
			requestlog->svm.rcm = RCM_UNDEFINED;
		}
		if (!endmail) {
			/*
			 *  See if there are any other reasons for why we
			 *  should send mail concerning the request.
			 */
			if (!requestlog->reqrcmknown) {
				/*
				 *  A system crash has destroyed our knowledge
				 *  of the batch request completion status, so
				 *  we send mail to the user to confess our
				 *  bumbling incompetence.
				 */
				endmail = 1;
			}
			else {
				/*
				 *  At the very least, the request completion
				 *  code is known.
				 *
				 *  Determine if any output file disposition
				 *  deserves comment.
				 */
				endmail = mai_outfiles (rawreq, requestlog);
			}
		}
		break;
	case QUE_DEVICE:
		if (!(valid [reason] & VDEV)) {
			requestlog->svm.rcm = RCM_UNDEFINED;
		}
		break;
	case QUE_NET:
		if (!(valid [reason] & VNET)) {
			requestlog->svm.rcm = RCM_UNDEFINED;
		}
		break;
	case QUE_PIPE:
		if (!(valid [reason] & VPIP)) {
			requestlog->svm.rcm = RCM_UNDEFINED;
		}
		break;
	}
	/*
	 *  Adjust request transaction state (if necessary).
	 */
	if (transaction->state == RTS_EXECUTING &&
	   !(request->status & RQF_WASEXE)) {
		/*
		 *  If the request was not restarting, then there are
		 *  certain conditions under which we must restore the
		 *  transaction state of the request to RTS_STASIS.
		 */
		switch (requestlog->svm.rcm) {
		case RCM_2MANYSVARGS:
		case RCM_BADSRVARG:
		case RCM_DEVOPEFAI:
		case RCM_ENFILERUN:
		case RCM_ENOSPCRUN:
		case RCM_INSUFFMEM:
		case RCM_INTERRUPTED:
		case RCM_NOMOREPROC:
		case RCM_NONSECPORT:
		case RCM_RETRYLATER:
		case RCM_SERBRKPNT:
		case RCM_SEREXEFAI:
		case RCM_SERVESIGERR:
		case RCM_SSHEXEFAI:
			/*
			 *  For every request completion code that indicates
			 *  a requeueable condition that is NOT identical to
			 *  RCM_SHUTDNREQUE (in which case the transaction
			 *  state should be left alone), we must return the
			 *  transaction state to RTS_STASIS.
			 */
			transaction->state = RTS_STASIS;
			if (tra_setstate (request->v1.req.trans_id,
					  transaction) == -1) {
				/*
				 *  An error occurred modifying the request
				 *  transaction state.
				 */
				printf ("E$Error setting transaction state ");
				printf ("in nqs_reqexi().\n");
				printf ("I$Errno: %s.\n", asciierrno());
				printf ("I$Transaction-id: %1d.\n",
					request->v1.req.trans_id);
				fflush (stdout);
			}
			break;
		}
	}
	switch (requestlog->svm.rcm & XCI_FULREA_MASK) {
	case RCM_2MANYENVARS:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_2MANYSVARGS:
		printf ("E$Too many server arguments");
		stopentity (device);	/* Requeue request and stop device */
					/* or queue as appropriate */
	case RCM_ABORTED:
		if (endmail) mai_send (rawreq, requestlog, MSX_ABORTED);
		reqcom (request, 000);	/* Delete the request */
	case RCM_BADCDTFIL:
		shoreqmsg (rawreq, requestlog, "E$RCM_BADCDTFIL");
		printf (putin_failed);
		reqcom (request, 001);	/* Place request in failed directory */
					/* exit (001) */
	case RCM_BADSRVARG:
		printf ("E$Bad argument to server");
		stopentity (device);	/* Requeue request and stop device */
					/* or queue as appropriate */
	case RCM_DELIVERED:
		if (rawreq->flags & RQF_TRANSMAIL) {
			mai_send (rawreq, requestlog, MSX_DELIVERED);
		}
		reqcom (request, 000);	/* Delete the request from this */
					/* machine */
	case RCM_DELIVEREXP:
	case RCM_DELIVERFAI:
	case RCM_DELIVERRETX:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete the request from this */
					/* machine */
	case RCM_DEVOPEFAI:
		printf ("W$Device: %s has failed.\n", device->name);
		reqcom (request, 012);	/* Requeue request and stop device */
					/* exit (012) */
	case RCM_ENFILERUN:
		printf ("E$Insufficient number of free ");
		printf ("file descriptors to spawn request.\n");
		printf (stopallque);
		reqcom (request, 042);	/* Requeue request and stop all */
					/* queues; exit (042) */
	case RCM_ENOSPCRUN:
		printf ("E$Insufficient file system resources to spawn ");
		printf ("request.\n");
		printf (stopallque);
		reqcom (request, 042);	/* Requeue request and stop all */
					/* queues; exit (042) */
	case RCM_EXITED:
		if (endmail) mai_send (rawreq, requestlog, MSX_ENDED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_INSUFFMEM:
		printf ("I$Insufficient memory to spawn request.\n");
		reqcom (request, 042);	/* requeue request */
					/* and stop all queues */
	case RCM_INTERRUPTED:
		reqcom (request, 002);	/* Requeue request */
	case RCM_MIDUNKNOWN:
		shoreqmsg (rawreq, requestlog, "W$RCM_MIDUNKNOWN");
		printf (being_deleted);
		/*
		 *  We cannot send mail to the user since the originating
		 *  machine is no longer known to us!
		 */
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_NETREQDEL:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_NOACCAUTH:
		shoreqmsg (rawreq, requestlog, "E$RCM_NOACCAUTH");
		printf (being_deleted);
		printf ("I$Request original user-id was: %1d.\n",
			request->v1.req.uid);
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_NOMOREPROC:
		printf ("E$Insufficient number of processes to run request.\n");
		printf (stopallque);
		reqcom (request, 042);	/* Requeue request and stop all */
					/* queues; exit (042) */
	case RCM_NONSECPORT:
		printf ("E$Server bound to non-secure port");
		stopentity (device);	/* Requeue request and stop device */
					/* or queue as appropriate */
	case RCM_NOSVRETCODE:
		printf ("E$No completion code returned");
		if (serverq->q.type == QUE_DEVICE) {
			printf (" for device: %s.\n", device->name);
			printf (putin_failed);
			mai_send (rawreq, requestlog, MSX_FAILED);
			reqcom (request, 011);
					/* Place request in failed directory */
					/* and stop device */
					/* exit (011) */
		}
		if (serverq->q.type == QUE_NET) {
			printf (" for network\nE$queue: %s.\n",
				fmtmidname(serverq->q.namev.to_destination));
		}
		else printf (" for queue: %s.\n", serverq->q.namev.name);
		printf (putin_failed);
		mai_send (rawreq, requestlog, MSX_FAILED);
		reqcom (request, 021);	/* Place request in failed directory */
					/* and stop queue */
					/* exit (021) */
	case RCM_PATHLEN:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_PIPREQDEL:
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_REQCOLLIDE:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_RETRYLATER:
 		reqcom (request, 002);	/* Requeue request for retry */
					/* exit (002) */
	case RCM_ROUTED:
		/*
		 *  In this ****TRIAGE**** implementation, RCM_ROUTED
		 *  is equivalent to RCM_DELIVERED.
		 */
		if (rawreq->flags & RQF_TRANSMAIL) {
			mai_send (rawreq, requestlog, MSX_DELIVERED);
		}
		reqcom (request, 000);	/* Delete the request from this */
					/* machine */
	case RCM_ROUTEDLOC:
		if (rawreq->flags & RQF_TRANSMAIL) {
			mai_send (rawreq, requestlog, MSX_DELIVERED);
		}
		reqcom (request, 003);	/* Release request from arriving set */
	case RCM_ROUTEEXP:
	case RCM_ROUTEFAI:
	case RCM_ROUTERETX:
		/*
		 *  In this ****TRIAGE**** implementation, these codes
		 *  are analogous to the DELIVER codes.
		 */
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete the request from this */
					/* machine */
	case RCM_SERBRKPNT:
		printf ("E$Server breakpoint reached");
		stopentity (device);	/* Requeue request and stop device */
					/* or queue as appropriate */
	case RCM_SEREXEFAI:
		printf ("E$Server execve() failed");
		stopentity (device);	/* Requeue request and stop device */
					/* or queue as appropriate */
	case RCM_SERVESIGERR:
		printf ("E$Server killed by unforseen signal");
		stopentity (device);	/* Requeue request and stop device */
					/* or queue as appropriate */
	case RCM_SHEXEF2BIG:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_SHUTDNABORT:
		/*
		 *  Server terminated by receipt of signal that resulted
		 *  from an NQS shutdown SIGTERM/SIGKILL sequence, AND
		 *  the request is NOT restartable.
		 *
		 *  The serious student will note that we have an ambiguous
		 *  situation here.  NQS cannot know when shutting down, if
		 *  the signal death of a request was caused because of the
		 *  NQS SIGTERM/SIGKILL shutdown signal sequence, or because
		 *  a user decided to signal their request when NQS was also
		 *  SIMULTANEOUSLY shutting down.  Thus, this code is not
		 *  perfect.
		 *
		 *  Sigh.
		 */
		mai_send (rawreq, requestlog, MSX_ABORTSHUTDN);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_SHUTDNREQUE:
		/*
		 *  Server terminated by receipt of signal that resulted
		 *  from an NQS shutdown SIGTERM/SIGKILL sequence, AND
		 *  the request is restartable.
		 *
		 *  The serious student will note that we have an ambiguous
		 *  situation here.  NQS cannot know when shutting down, if
		 *  the signal death of a request was caused because of the
		 *  NQS SIGTERM/SIGKILL shutdown signal sequence, or because
		 *  a user decided to signal their request when NQS was also
		 *  SIMULTANEOUSLY shutting down.  Thus, this code is not
		 *  perfect.
		 *
		 *  Sigh.
		 */
		reqcom (request, 004);	/* Requeue request killed by signal */
					/* exit (004) */
	case RCM_SSHBRKPNT:
		shoreqmsg (rawreq, requestlog,
			   "I$Shell breakpoint encountered");
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 020);	/* Delete request; stop queue */
					/* exit (020) */
	case RCM_SSHEXEFAI:
		printf ("E$Shell execve() failed for batch\n");
		printf ("E$queue: %s.\n", serverq->q.namev.name);
		printf ("I$%s.\n", requestlog->svm.mssg);
		mai_send (rawreq, requestlog, MSX_REQUEUED);
		reqcom (request, 022);	/* Requeue request; stop queue */
					/* exit (022) */
	case RCM_STAGEOUT:
		/* MOREHERE later */
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_STAGEOUTBAK:
		/* MOREHERE later */
		mai_send (rawreq, requestlog, MSX_ENDED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_STAGEOUTFAI:
		/* MOREHERE later */
		mai_send (rawreq, requestlog, MSX_ENDED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_UNABLETOEXE:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_UNAFAILURE:
		shoreqmsg (rawreq, requestlog,
			   "I$Server reports RCM_UNAFAILURE");
		printf (putin_failed);
		mai_send (rawreq, requestlog, MSX_FAILED);
		reqcom (request, 001);	/* Place request in failed directory */
					/* exit (001) */
	case RCM_UNCRESTDERR:
		mai_send (rawreq, requestlog, MSX_ENDED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_UNCRESTDOUT:
		mai_send (rawreq, requestlog, MSX_ENDED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_UNDEFINED:
		/*
		 *  Invalid completion code reported.
		 */
		shoreqmsg (rawreq, requestlog, "E$Invalid completion code");
		printf ("I$Completion code text follows:\n");
		fflush (stdout);
		analyzercm (requestlog->svm.rcm, stdout, "I$");
		printf (putin_failed);
		fflush (stdout);
		requestlog->svm.rcm = RCM_UNDEFINED;
		requestlog->svm.mssg [0] = '\0';
		mai_send (rawreq, requestlog, MSX_FAILED);
		if (serverq->q.type == QUE_DEVICE) {
			reqcom (request, 011);
					/* Place request in failed */
		}			/* directory and stop dev  */
		reqcom (request, 021);	/* Place request in failed */
	case RCM_USHBRKPNT:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
	case RCM_USHEXEFAI:
		mai_send (rawreq, requestlog, MSX_DELETED);
		reqcom (request, 000);	/* Delete request */
					/* exit (000) */
#ifdef SDSC
	case RCM_BADTSCHED:
		/*
		 */
		mai_send (rawreq, requestlog, MSX_BADTSCHED);
		reqcom (request, 000);	/* Delete the request from this */
					/* machine */
	case RCM_NQSABORT:
		/*
		 */
		reqcom (request, 0102);	/* Requeue and Abort NQS */
#endif
	}
}


/*** reqcom
 *
 *
 *	void reqcom():
 *	Send a request-complete message to the NQS daemon, and exit().
 */
static void reqcom (request, exitcode)
register struct request *request;	/* Ptr to request structure */
int exitcode;				/* Exit code to NQS daemon */
{
#if	UNICOS | SGI | SYS52 | UTS | OSF
	struct tms tms;			/* Process and child execution times */

	fflush (stdout);		/* Force writing of any log */
	fflush (stderr);		/* Messages */
	interclear();
	times (&tms);			/* Get process and child times */
	/*
	 *  Since it is known absolutely that this message is not going
	 *  across an inter-machine boundary, we send the structure
	 *  directly.
	 */
	interwbytes ((char *) &tms, sizeof (struct tms));
#else
#if	BSD42 | BSD43 | ULTRIX
	struct rusage rusage_self;	/* Process resource consumption */
	struct rusage rusage_children;	/* for self and children */

	interclear();
	getrusage (RUSAGE_CHILDREN, &rusage_children);
	getrusage (RUSAGE_SELF, &rusage_self);
	rusage_self.ru_utime.tv_usec += rusage_children.ru_utime.tv_usec;
	if (rusage_self.ru_utime.tv_usec > 1000000) {
		rusage_self.ru_utime.tv_usec -= 1000000;
		rusage_self.ru_utime.tv_sec += 1;
	}
	rusage_self.ru_utime.tv_sec += rusage_children.ru_utime.tv_sec;
	rusage_self.ru_stime.tv_usec += rusage_children.ru_stime.tv_usec;
	if (rusage_self.ru_stime.tv_usec > 1000000) {
		rusage_self.ru_stime.tv_usec -= 1000000;
		rusage_self.ru_stime.tv_sec += 1;
	}
	rusage_self.ru_stime.tv_sec += rusage_children.ru_stime.tv_sec;
	rusage_self.ru_maxrss += rusage_children.ru_maxrss;
	rusage_self.ru_ixrss += rusage_children.ru_ixrss;
	rusage_self.ru_idrss += rusage_children.ru_idrss;
	rusage_self.ru_isrss += rusage_children.ru_isrss;
	rusage_self.ru_minflt += rusage_children.ru_minflt;
	rusage_self.ru_majflt += rusage_children.ru_majflt;
	rusage_self.ru_nswap += rusage_children.ru_nswap;
	rusage_self.ru_inblock += rusage_children.ru_inblock;
	rusage_self.ru_oublock += rusage_children.ru_oublock;
	rusage_self.ru_msgsnd += rusage_children.ru_msgsnd;
	rusage_self.ru_msgrcv += rusage_children.ru_msgrcv;
	rusage_self.ru_nsignals += rusage_children.ru_nsignals;
	rusage_self.ru_nvcsw += rusage_children.ru_nvcsw;
	rusage_self.ru_nivcsw += rusage_children.ru_nivcsw;
	/*
	 *  Since it is known absolutely that this message is not going
	 *  across an inter-machine boundary, we send the structure
	 *  directly.
	 */
	interwbytes ((char *) &rusage_self, sizeof (struct rusage));
#else
BAD SYSTEM TYPE
#endif
#endif
	interw32i (request->v1.req.orig_seqno);	/* Request sequence# */
	interw32i (request->v1.req.orig_mid);	/* Request mid */
	interw32i (exitcode);		/* Disposition code */
	interwstr (request->queue->q.namev.name);
					/* Containing queue name */
	inter (PKT_REQCOM);		/* Send completion notification */
	exit (exitcode);		/* Exit with code to daemon */
}


/*** shoreqmsg
 *
 *
 *	void shoreqmsg():
 *	Show request message displaying request-id.
 */
static void shoreqmsg (rawreq, requestlog, text)
register struct rawreq *rawreq;		/* Raw request struct for request */
register struct requestlog *requestlog;	/* Mail request log record */
register char *text;			/* Text */
{
	printf ("%s for request %1ld.%s.\n", text, (long) rawreq->orig_seqno,
		fmtmidname (rawreq->orig_mid));
	fflush (stdout);
	if (requestlog->svm.mssg [0] != '\0') {
		printf ("I$%s\n", requestlog->svm.mssg);
		fflush (stdout);
	}
}


/*** stopentity
 *
 *
 *	void stopentity():
 *	Requeue request and stop device or queue as appropriate.
 */
static void stopentity (device)
register struct device *device;			/* Involved device */
{
	register struct request *request;	/* Current request */
	register struct queue *serverq;		/* Server queue */

	request = device->curreq;	/* Current request */
	serverq = request->queue;	/* Server queue */
	if (serverq->q.type == QUE_DEVICE) {
		printf (" for device: %s.\n", device->name);
		reqcom (request, 012);	/* Requeue request and stop device */
					/* exit (012) */
	}
	if (serverq->q.type == QUE_NET) {
		printf (" for network\nE$queue: %s.\n",
			fmtmidname (serverq->q.namev.to_destination));
	}
	else printf (" for queue: %s.\n", serverq->q.namev.name);
	reqcom (request, 022);		/* Requeue request and stop queue */
}					/* exit (022) */
