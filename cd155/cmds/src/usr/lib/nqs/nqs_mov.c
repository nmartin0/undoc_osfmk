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
 
/*++ nqs_mov.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_mov.c,v $
 *
 * DESCRIPTION:
 *
 *	Move a request between queues or move all requests on a queue to
 *	another queue.
 *
 *
 *	Author:
 *	-------
 *	Clayton D. Andreasen, Cray Research, Incorporated.
 *	August 8, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:02 $ $State: Exp $)
 * $Log: nqs_mov.c,v $
 * Revision 1.3  1994/11/19  02:53:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:49  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:16  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:45  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_mov.c	1.2 (nqs_mov.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "transactcc.h"			/* Transaction completion codes */
#include "nqsxvars.h"			/* Global vars */
#include "informcc.h"			/* NQS information completion */
					/* codes and masks */

extern void a2s_a2aset();		/* Add to arriving set */
extern void a2s_a2hset();		/* Add to hold set in queue */
extern void a2s_a2qset();		/* Add to queued set in queue */
extern void a2s_a2wset();		/* Add to wait set in queue */
extern void udb_qorder();		/* Update NQS database image of */
extern struct request *nqs_fndreq();	/* Find request by sequence# & mid */
extern struct queue *nqs_fndnnq();	/* Find non-network queue */
extern long nsq_enque();		/* Insert a request into a queue */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern int readreq();			/* Read the request header portion */
					/* of a control file for an NQS req */
extern time_t time();			/* Get GMT time since 0:00:00 1/1/70 */
extern int writereq();			/* Write/rewrite request header */
extern int bsc_sched();			/* Schedule a batch request */
extern void bsc_spawn();		/* MAYBE spawn a batch request */
extern int dsc_sched();			/* Schedule a device request */
extern void dsc_spawn();		/* MAYBE spawn a device request */
extern int psc_sched();			/* Schedule a pipe request */
extern void psc_spawn();		/* MAYBE spawn a pipe queue request */
extern char *strcmp();			/* String compare routine */

/*** nqs_movreq
 *
 *
 *	long nqs_movreq():
 *	Move a queued NQS request.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if the request was successfully
 *				 modified.
 *		TCML_NOSUCHQUE:	 if the specified queue does not
 *				 exist on this machine.
 *		TCML_QUEDISABL:	 if the specified queue is disabled
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist on this machine.
 *		TCML_REQRUNNING: if the specified request is running.
 *		TCML_WROQUETYP:  if the destination queue type is not
 *				 compatible with the source queue type.
 */
long nqs_movreq (orig_seqno, orig_mid, destque)
long	orig_seqno;		/* Originating sequence number of request */
mid_t	orig_mid;		/* Originating machine id */
char	*destque;		/* Destination queue name */
{
	struct request *predecessor;	/* Predecessor in req set in queue */
	int state;			/* Request queue state: RQS_ */
	char path [MAX_PATHNAME+1];	/* Control file pathname */
	struct rawreq rawreq;		/* Raw request structure */
	int cfd;			/* Control file file-descriptor */
	register struct queue *queue;	/* Queue in which req is placed */
	register struct request *req;	/* Request struct allocated for req */

	if (Debug > 2) {
		printf ("D$nqs_mov().  seqno=%d; mid=%d; queue=%s\n",
			orig_seqno, orig_mid, destque);
		fflush (stdout);
	}

	/*
	 *  Locate the queue in which to place the request.
	 */
	queue = nqs_fndnnq (destque);
	if (queue == (struct queue *) 0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if (!(queue->q.status & QUE_ENABLED)) {
		return (TCML_QUEDISABL);	/* Queue is disabled */
	}

	/*
	 *  Locate the request.
	 */
	state = RQS_STAGING | RQS_QUEUED | RQS_WAITING | RQS_HOLDING
		| RQS_ARRIVING | RQS_DEPARTING | RQS_RUNNING;
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor, &state)) ==
	    (struct request *) 0) {
		return (TCML_NOSUCHREQ);	/* Request not found */
	}
	if (state != RQS_QUEUED) {		/* if request not queued */
		/*
		 *  The request is departing, staging output files,
		 *  running, or routing.  For now, we return TCML_REQRUNNING.
		 */
		return (TCML_REQRUNNING);
	}

	/*
	 *  Build the control file name and open it.
	 */
	pack6name (path, Nqs_control,
		  (int) (orig_seqno % MAX_CTRLSUBDIRS), (char *) 0,
		  (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	if ((cfd = open (path, O_RDWR)) < 0) {
		return (TCML_UNAFAILURE);
	}

	/*
	 *  Read the request header.
	 */
	if (Debug > 2) {
		printf ("D$nqs_mov(): Read request (%s).\n", path);
		fflush (stdout);
	}
	if (readreq (cfd, &rawreq) == -1) {
		close (cfd);			/* Close request file */
		return (TCML_UNAFAILURE);	/* Failure */
	}

	/*
	 *  Verify that the request type and destination queue type agree
	 *  if the queue is a batch or device queue.
	 */
	if ((queue->q.type == QUE_BATCH &&
	     rawreq.type != RTYPE_BATCH) ||
	    (queue->q.type == QUE_DEVICE &&
	     rawreq.type != RTYPE_DEVICE)) {
		close (cfd);			/* Close request file */
		return (TCML_WROQUETYP);	/* wrong queue type */
	}

	/*
	 *  Copy the destination queue name into the queue field of the
	 *  rawreq structure for the request and rewrite the request header.
	 */
	strcpy (rawreq.quename, destque);
	writereq (cfd, &rawreq);		/* Update request header */
	close (cfd);				/* Close request file */

	/*
	 *  The request has been located.  Remove the request from the
	 *  the containing queue and set the QUE_UPDATE bit.
	 */
	if (predecessor == (struct request *) 0) {
		req->queue->queuedset = req->next;
	} else {
		predecessor->next = req->next;
	}
	req->next = (struct request *)0;	/* No more requests */
	req->queue->q.queuedcount--;
	req->queue->q.status |= QUE_UPDATE;
	req->queue = (struct queue *)0;

	/*
	 *  Now, schedule the request for execution.
	 */
	switch (queue->q.type) {
	case QUE_BATCH:
		req->v1.req.priority = bsc_sched (&rawreq);
		break;
	case QUE_DEVICE:
		req->v1.req.priority = dsc_sched (&rawreq);
		break;
	case QUE_PIPE:
		req->v1.req.priority = psc_sched (&rawreq);
		break;
	}

	if (queue->q.type == QUE_PIPE) {
		/*
		 *  When a request is placed in a pipe queue, it is
		 *  routed and delivered to its destination as quickly
		 *  as possible, regardless of the -a time.  The -a
		 *  time only becomes effective when the request has
		 *  reached its final destination batch or device
		 *  queue.
		 */
		req->start_time = 0;
	} else {
		/*
		 *  The request is being placed in a batch or device
		 *  queue, and so the -a time parameter of the request
		 *  is now meaningful.
		 */
		req->start_time = rawreq.start_time;
					/* Remember start after time */
	}

	/*
	 *  Place the prioritized request into the queue.
	 */
	if ((rawreq.flags & RQF_OPERHOLD) ||
	    (rawreq.flags & RQF_USERHOLD)) {
		/*
		 *  Add the request to the hold set for this queue.
		 *  The QUE_UPDATE bit is set by a2s_a2hset().
		 */
		a2s_a2hset (req, queue);/* Add to hold set */
	} else if (rawreq.start_time > time ((time_t *) 0)) {
		/*
		 *  The request has a start time in the future.
		 *  The QUE_UPDATE bit is set by a2s_a2wset().
		 */
		a2s_a2wset (req, queue);/* Add to wait set */
	} else {
		/*
		 *  Place the request in the eligible to run set.
		 *  The QUE_UPDATE bit is set by a2s_a2qset().
		 */
		a2s_a2qset (req, queue);/* Add to queued set */
	}
	switch (queue->q.type) {
	case QUE_BATCH:
		bsc_spawn();		/* Maybe spawn some batch reqs */
		break;
	case QUE_DEVICE:
		dsc_spawn();		/* Maybe spawn some device reqs */
		break;
	case QUE_PIPE:
		psc_spawn();		/* Maybe spawn some pipe reqs */
		break;
	}
	if (queue->q.status & QUE_UPDATE) {
		/*
		 *  No requests were spawned from the queue in which
		 *  the most recent request was placed.
		 */
		udb_qorder (queue);	/* Update and clear QUE_UPDATE bit */
	}
	return (TCML_COMPLETE);		/* Return transaction code */
}


/*** nqs_movque
 *
 *
 *	long nqs_movque():
 *	Move all of the NQS requests from one queue to another.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if the request was successfully
 *				 modified.
 *		TCML_NOSUCHQUE:	 if the specified queue does not
 *				 exist on this machine.
 *		TCML_QUEDISABL:	 if the specified queue is disabled
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist on this machine.
 *		TCML_REQRUNNING: if the specified request is running.
 *		TCML_WROQUETYP:  if the destination queue type is not
 *				 compatible with the source queue type.
 */
long nqs_movque (from_que, to_que)
char	*from_que;			/* Queue to move requests from */
char	*to_que;			/* Queue to move requests to */
{
	register struct queue *queue;	/* Queue to move requests from */
	register struct request *req;	/* Request struct allocated for req */
	register struct request *next;	/* Next request */
	register long rc;		/* return code from nqs_movreq() */

	if (strcmp(from_que, to_que) == (char *)0) {	/* if same queue */
		return (TCML_COMPLETE);
	}

	/*
	 *  Locate the queue from which requests will be moved.
	 */
	queue = nqs_fndnnq (from_que);
	if (queue == (struct queue *) 0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}

	/*
	 *  Move all requests to the destination queue.
	 */
	next = queue->queuedset;
	while ((req = next) != (struct request *)0) {
		next = req->next;
		rc = nqs_movreq( req->v1.req.orig_seqno,
			req->v1.req.orig_mid, to_que);
		if (rc == TCML_COMPLETE) continue;
		return(rc);
	}
	return (TCML_COMPLETE);		/* Return transaction code */
}
