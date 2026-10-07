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
 
/*++ nqs_psc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_psc.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the 3 functions:
 *
 *		psc_reqcom()
 *		psc_sched()
 *		psc_spawn()
 *
 *	which control the scheduling, and spawning of NQS pipe requests.
 *	This module can be modified to implement appropriate scheduling
 *	algorithms for a particular installation as necessary.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:07 $ $State: Exp $)
 * $Log: nqs_psc.c,v $
 * Revision 1.3  1994/11/19  02:53:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:58  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:22  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:23  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS global variables */

extern void nqs_spawn();		/* Spawn an NQS request */
extern void udb_qorder();		/* Update queue-ordering */


/*** psc_reqcom
 *
 *
 *	void psc_reqcom():
 *
 *	This function is invoked whenever a running pipe request completes
 *	execution.  This function is free to spawn more pipe requests from
 *	ANY pipe queue.
 *
 *	All queues modified by the invocation of this procedure have
 *	had their corresponding NQS database queue state image updated
 *	(to match the internal queue states) upon return from this
 *	procedure.
 */
void psc_reqcom (request)
struct request *request;		/* The completed pipe request */
{
	void psc_spawn();

	/*
	 *  For the moment, we do not use any of the knowledge imparted
	 *  to us by the request that just finished execution,  Instead,
	 *  we spawn as many pipe requests as we can, within the limits
	 *  configured.
	 */
	psc_spawn();			/* Spawn all pipe requests */
}					/* within configured boundaries */


/*** psc_sched
 *
 *
 *	int psc_sched():
 *
 *	This function is invoked whenever a pipe req must be evaluated
 *	and assigned a priority by the NQS pipe req scheduling policies
 *	(which are implemented by this function).
 *
 *	The priority assigned to the req must be in the interval [0..32767].
 *	All pipe reqs with priority >= N within a given pipe queue, will,
 *	depending upon the precise scheduling criteria defined in:
 *
 *		psc_spawn()  and
 *		psc_reqcom()
 *
 *	be spawned before any pipe reqs in the same queue with a priority
 *	value < N.
 *	
 *	Returns:
 *		The assigned priority value for the specified pipe request.
 */
int psc_sched (rawreq)
struct rawreq *rawreq;			/* The raw request structure */
					/* describing the new request */
{
	if (rawreq->rpriority == -1) {
		/*
		 *  The user did not assign a priority to the req; assign
		 *  a default priority value.
		 */
		return (MAX_RPRIORITY / 2);
	}
	return (rawreq->rpriority);	/* For now, just the intra-queue */
}


/*** psc_spawn
 *
 *
 *	void psc_spawn():
 *
 *	This function is invoked whenever request activity indicates that
 *	it MAY be possible to spawn a pipe request.  It is up to the
 *	discretion of the pipe req scheduling/spawning algorithm
 *	implemented here to determine whether or not pipe req(s) should
 *	be spawned.
 *
 *	All queues modified by the invocation of this procedure have
 *	had their corresponding NQS database queue state image updated
 *	(to match the internal queue states) upon return from this
 *	procedure.
 */
void psc_spawn()
{
	register struct queue *queue;	/* Pipe queue set walking */
	register int prevpipcount;	/* Prev loop values of Gblpipcount */

	/*
	 *  Note that we are very careful to make sure that all pipe
	 *  queues with higher priorities that also have pipe requests
	 *  that can run, get to spawn first.  This becomes critical when
	 *  the number of pipe requests that can run exceeds the maximum
	 *  number of pipe requests that are allowed to simultaneously
	 *  execute.
	 */
	if (Shutdown) return;		/* Do not spawn any requests if */
					/* NQS is shutting down */
	queue = Pripipqueset;		/* Prioritized pipe queue set */
	prevpipcount = Gblpipcount - 1;	/* Make loop go at least once */
	while (queue != (struct queue *) 0 &&
	       Maxgblpiplimit > Gblpipcount &&
	       prevpipcount != Gblpipcount) {
		/*
		 *  Spawn as many requests as are allowed for this
		 *  queue.
		 */
		while (queue->q.v1.pipe.runlimit > queue->q.runcount &&
		      (queue->q.status & QUE_RUNNING) &&
		       queue->queuedset != (struct request *) 0 &&
		       enabled_dest (queue) &&
		       Maxgblpiplimit > Gblpipcount &&
		       prevpipcount != Gblpipcount) {
			/*
			 *  There is a pipe request that can be spawned.
			 *  Try to spawn it.
			 *
			 *  Note that the spawning of a pipe request
			 *  requires the creation of a subrequest.
			 *  Furthermore, a new network queue may have
			 *  to be created.  It is therefore possible
			 *  that the successful spawning of a pipe
			 *  request would require more memory, than
			 *  there is available.  In such a situation,
			 *  the spawn quietly fails.  By watching
			 *  Gblpipcount, this procedure can tell when
			 *  such an event has happened.
			 */
			prevpipcount = Gblpipcount;
			nqs_spawn (queue->queuedset, (struct device *) 0);
		}
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  The database image of this queue needs to
			 *  be updated.
			 */
			udb_qorder (queue);	/* Update database image */
		}				/* and clear QUE_UPDATE */
		queue = queue->v1.pipe.nextpriority;	/* Next queue */
	}
}


/*** enabled_dest
 *
 *
 *	int enabled_dest():
 *
 *	Return boolean TRUE (non-zero), if there exists an enabled
 *	destination in the destination set for the specified pipe
 *	queue.
 */
static int enabled_dest (queue)
struct queue *queue;				/* Queue structure */
{
	register struct qdestmap *qdestmap;	/* Pipe destination walking */

	qdestmap = queue->v1.pipe.destset;
	while (qdestmap != (struct qdestmap *) 0) {
		if (qdestmap->pipeto->status & DEST_ENABLED) {
			/*
			 *  There exists an enabled destination for
			 *  this pipe queue.
			 */
			return (1);
		}
		qdestmap = qdestmap->next;	/* Examine next destination */
	}
	return (0);				/* No enabled destinations */
}
