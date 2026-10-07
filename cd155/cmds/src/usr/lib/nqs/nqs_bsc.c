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
 
/*++ nqs_bsc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_bsc.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the 3 functions:
 *
 *		bsc_reqcom()
 *		bsc_sched()
 *		bsc_spawn()
 *
 *	which control the scheduling, and spawning of NQS batch requests.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1995/03/17 18:21:17 $ $State: Exp $)
 * $Log: nqs_bsc.c,v $
 * Revision 1.4  1995/03/17  18:21:17  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 9765
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_bsc.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_vtimer.c
 * 	cmds_libs/src/usr/lib/nqs/macs_sched.c
 * 	cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.3  1994/11/19  02:52:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:03  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:53  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:49  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS global variables */
 
#ifdef SDSC
#include "buddyxvar.h"
extern int macs_sched ();
extern short sModeChange;
#endif
 
extern void udb_qorder();		/* Update queue-ordering */
extern void nqs_spawn();		/* Spawn an NQS request */


/*** bsc_reqcom
 *
 *
 *	void bsc_reqcom():
 *
 *	This function is invoked whenever a running batch request completes
 *	execution.  This function is free to spawn more batch requests from
 *	ANY batch queue.
 *
 *	All queues modified by the invocation of this procedure have
 *	had their corresponding NQS database queue state image updated
 *	(to match the internal queue states) upon return from this
 *	procedure.
 */
void bsc_reqcom (request)
struct request *request;		/* The completed batch request */
{
	void bsc_spawn();
	extern	void	dsc_spawn();	/* Intergraph addt */

	/*
	 *  For the moment, we do not use any of the knowledge imparted
	 *  to us by the request that just finished execution.  Instead,
	 *  we spawn as many batch requests as we can, within the limits
	 *  configured.
	 */
	bsc_spawn();			/* Spawn all batch requests */

	/*
	 *	Intergraph addition:
	 *	should we spawn a device request 
	 */
	if (request->queue->v1.batch.qcomplex[0] != (struct qcomplex *) 0)
		dsc_spawn();
}					/* within configured boundaries */


/*** bsc_sched
 *
 *
 *	int bsc_sched():
 *
 *	This function is invoked whenever a batch req must be evaluated
 *	and assigned a priority by the NQS batch req scheduling policies
 *	(which are implemented by this function).
 *
 *	The priority assigned to the req must be in the interval [0..32767].
 *	All batch reqs with priority >= N within a given batch queue, will,
 *	depending upon the precise scheduling criteria defined in:
 *
 *		bsc_spawn()  and
 *		bsc_reqcom()
 *
 *	be spawned before any batch reqs in the same queue with a priority
 *	value < N.
 *	
 *	Returns:
 *		The assigned priority value for the specified batch request.
 */
int bsc_sched (rawreq)
struct rawreq *rawreq;			/* The raw request structure */
					/* describing the new request */
{
	if (rawreq->rpriority == -1) {
		/*
		 *  The user did not specify a priority; assign a
		 *  default value.
		 */
		return (Defbatpri);
	}
	return (rawreq->rpriority);	/* For now, just the intra-queue */
}					/* req priority */

#ifdef SDSC
void bsc_spawnMode()
{
  sModeChange = 1;
  bsc_spawn();
}
#endif

/*** bsc_spawn
 *
 *
 *	void bsc_spawn():
 *
 *	This function is invoked whenever request activity indicates that
 *	it MAY be possible to spawn a batch request.  It is up to the
 *	discretion of the batch request scheduling/spawning algorithm
 *	implemented here to determine whether or not batch req(s) should
 *	be spawned.
 *
 *	All queues modified by the invocation of this procedure have
 *	had their corresponding NQS database queue state image updated
 *	(to match the internal queue states) upon return from this
 *	procedure.
 */
void bsc_spawn()
{
	register struct queue *queue;	/* Batch queue set walking */
	register struct qcomplex *qcomplex;	/* Queue complex walking */
	register int prevbatcount;	/* Prev loop value of Gblbatcount */
	register int full;		/* Some qcomplex is full flag */
	register int i;

	/*
	 *  Note that we are very careful to make sure that all batch
	 *  queues with higher priorities that also have batch requests
	 *  that can run, get to spawn first.  This becomes critical when
	 *  the number of batch requests that can run exceeds the maximum
	 *  number of batch requests that are allowed to simultaneously
	 *  execute.
	 */
	if (Shutdown) return;		/* Do not spawn any requests if */
					/* NQS is shutting down */
#ifdef SDSC
	/* Call the macs scheduler */

        if (macs_sched () == 0) {      /* scheduling already done */
                queue = Pribatqueset;   /* Prioritized batch queue set */
                while (queue != (struct queue *) 0) {
                        if (queue->q.status & QUE_UPDATE) {
                                /*
                                 *  The database image of this queue must be
                                 *  updated.
                                 */
                                udb_qorder (queue);     /* Update image */
                        }
                        queue = queue->v1.batch.nextpriority;  /* Next queue */
                }
        }
#endif
	queue = Pribatqueset;		/* Prioritized batch queue set */
	prevbatcount = Gblbatcount - 1;	/* Make loop go at least once */
	while (queue != (struct queue *) 0 &&
	       Maxgblbatlimit > Gblbatcount &&
	       prevbatcount != Gblbatcount) {
#ifdef SDSC
		/* this loop only take care of jobs with ncpus = 0 */

		if (queue->q.v1.batch.prncpus > 0) {
			queue = queue->v1.batch.nextpriority;  /* Next queue */
			break;
		}
#endif 
		/*
		 *  Spawn as many batch requests as we can for this queue.
		 */
		while (queue->q.v1.batch.runlimit > queue->q.runcount &&
		      (queue->q.status & QUE_RUNNING) &&
		       queue->queuedset != (struct request *) 0 &&
		       Maxgblbatlimit > Gblbatcount &&
		       prevbatcount != Gblbatcount) {
			full = 0;
			for (i = MAX_COMPLXSPERQ; --i >= 0;) {
				qcomplex = queue->v1.batch.qcomplex[i];
				if (qcomplex == (struct qcomplex *)0) continue;
				if (qcomplex->runlimit <= qcomplex->runcount) {
					full = 1;
					break;
				}
			}
			if (full) break;
			/*
			 *  There is a batch request that can be spawned.
			 *  Try to spawn it.
			 *
			 *  Note that when spawning a batch request,
			 *  subrequests must be created and queued in
			 *  the appropriate network queues--which
			 *  themselves may have to be created.  It is
			 *  therefore possible that the successful
			 *  spawning of a batch request would require
			 *  more memory, than there is available.  In
			 *  such a situation, the spawn quietly fails.
			 *  By watching Gblbatcount, this procedure
			 *  can tell when such an event has occurred....
			 */
			prevbatcount = Gblbatcount;
			nqs_spawn (queue->queuedset, (struct device *) 0);
		}
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  The database image of this queue must be
			 *  updated.
			 */
			udb_qorder (queue);	/* Update image and clear */
		}				/* the QUE_UPDATE bit */
		queue = queue->v1.batch.nextpriority;	/* Next queue */
	}
}
