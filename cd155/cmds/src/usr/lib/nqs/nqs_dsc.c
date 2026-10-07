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
 
/*++ nqs_dsc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_dsc.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the 3 functions:
 *
 *		dsc_reqcom()
 *		dsc_sched()
 *		dsc_spawn()
 *
 *	which control the scheduling, and spawning of NQS device requests.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:54 $ $State: Exp $)
 * $Log: nqs_dsc.c,v $
 * Revision 1.3  1994/11/19  02:52:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:19  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:01  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:35  hender
 * Sterling version 4/22/87
 * 
 * Intergraph addition: add queue complex support
 *			add code in dsc_spawn to check queue complex limits
 *			add complex_space_4deviceq()
 *			add scheduling for when devices become
 *			 free and it is a member of a complex 
 *			 then do bsc_spawn and dsc_spawn before trying
 *			 to schedule just the device again.
 *			Modified dsc_reqcom so that if the device was
 *			 in a complex, the current device is checked
 *			 last, to cause round-robin device service
 *			Bill Mar	10/05/89 TAC
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS global variables */

extern void nqs_spawn();		/* Spawn an NQS request */
extern int strcmp();			/* String comparison */
extern void udb_qorder();		/* Update queue-ordering */


/*** dsc_reqcom
 *
 *
 *	void dsc_reqcom():
 *
 *	This function is invoked whenever a running device req completes
 *	execution.  This function is free to spawn more device reqs from
 *	ANY device queue.
 *
 *	The internal device structure AND the NQS database image of the
 *	device that was handling the request have been updated to indicate
 *	an idle state.
 *
 *	All queues and devices modified by the invocation of this procedure
 *	have had their corresponding NQS database images updated (to match
 *	their internal states) upon return from this procedure.
 */
void dsc_reqcom (device, request)
struct device *device;			/* Device that just finished */
					/* handling the request */
struct request *request;		/* The completed device request */
{
	struct request *sel_request();	/* Get next request in queue that */
					/* the specified device should serve */
	void update_devqueues();	/* Update database image of device */
					/* queues as necessary */
	register struct request *newreq;/* Ptr to new request to be spawned */
	register struct request *tagreq;/* Current tagged request */
	struct queue *queue;		/* Current queue */
	struct device *curdevice, *tagdevice;
	extern void bsc_spawn();
	void dsc_spawn();
	int  curpriority;		/* Current priority */

	if (Shutdown) return;		/* Do not spawn any new device */
					/* requests if NQS is shutting down */

	/* 
	 *  If this queue is in a complex, try to schedule 
	 *  a batch request 
	 *
	 *  Since we just finished a device request go to a 
	 *  batch request so we don't starve it.  Then, try
	 *  to spawn a device request.
	 */
	if (request->queue->v1.device.qcomplex[0] !=
		(struct qcomplex *) 0) {
		bsc_spawn();

		/* 
		 *  Since we have a complex, we must go through
		 *  a modified dsc_spawn so a particular device on the
		 *  mux will not be starved.  To do this, we must 
		 *  augment spawning so that the current device is not
		 *  selected first.  This will prevent one queue from
		 *  starving the rest.  That is why the current 
		 *  device is always checked last.  In addition,
		 *  if there is a request in a higher priority queue
		 *  then run it before the rest.  Otherwise, it will
		 *  be a round robin scheduling algorithm.
		 */

		tagreq = (struct request * ) 0;
		queue = device->curque;
		curdevice = device;
		curpriority = -1;
		do {
			device = device->next; 	/* Check the next device */
			if (device == (struct device *) 0)
				device = Devset;
			newreq = sel_request (device, queue);
			if (newreq != (struct request *) 0) {
				/* 
				 * If no requests have been assigned,
				 * then tag this request 
				 */

				if (curpriority == -1) {
					tagreq = newreq;
					tagdevice = device;
					curpriority = tagreq->queue->
							q.priority;
				}

				/* 
				 * Only tag this request if it has a
				 * higher pri.  If it is equal pri, it
				 * will be round robin.
				 */
				else if (newreq->queue->q.priority >
					curpriority) {
					tagreq = newreq;
					tagdevice = device;
					curpriority = tagreq->queue->
							q.priority;
				}
			}
		} while (device != curdevice);
		if (tagreq != (struct request *) 0) {
			/*
			 * There is a request that can be run on
			 * the device from the selected queue.  
			 * Spawn it.
			 */
			nqs_spawn (tagreq, tagdevice);
		}
		update_devqueues();   /* Update db image of queues */
	}	/* end of if queue in complex */
	else {
		newreq = sel_request (device, request->queue);
		if (newreq != (struct request *) 0) {
			/*
			 *  There is a request that can be run on 
			 *  the specified device.  Spawn it.
			 */
			nqs_spawn (newreq, device);
		}
		update_devqueues();	/* Update db image of queues */
	}
}


/*** dsc_sched
 *
 *
 *	int dsc_sched():
 *
 *	This function is invoked whenever a device req must be evaluated
 *	and assigned a priority by the NQS device scheduling policies
 *	(which are implemented by this function).
 *
 *	The priority assigned to the req must be in the interval [0..32767].
 *	All device reqs with priority >= N within a given device queue, will,
 *	depending upon the precise scheduling criteria defined in:
 *
 *		dsc_spawn()  and
 *		dsc_reqcom()
 *
 *	be spawned before any device reqs in the same queue with a priority
 *	value < N.
 *	
 *	Returns:
 *		The assigned priority value for the specified device request.
 */
int dsc_sched (rawreq)
struct rawreq *rawreq;			/* The raw request structure */
					/* describing the new request */
{
	if (rawreq->rpriority == -1) {
		/*
		 *  The user did not specify a priority; assign a
		 *  default priority.
		 */
		return (Defdevpri);
	}
	return (rawreq->rpriority);	/* For now, just the intra-queue */
}					/* Priority */


/*** dsc_spawn
 *
 *
 *	void dsc_spawn():
 *
 *	This function is invoked whenever request activity indicates that
 *	it MAY be possible to spawn a device request.  It is up to the
 *	discretion of the device request scheduling/spawning algorithm
 *	implemented here to determine whether or not device req(s) should
 *	be spawned.
 *
 *	All queues and devices modified by the invocation of this procedure
 *	have had their corresponding NQS database images updated (to match
 *	their internal states) upon return from this procedure.
 */
void dsc_spawn()
{
	struct request *sel_request();	/* Get next request in queue that */
					/* the specified device should serve */
	void update_devqueues();	/* Update database image of device */
					/* queues as necessary */

	register struct device *device;	/* Ptr to device */
	register struct request *newreq;/* Ptr to new request to be spawned */

	if (Shutdown) return;		/* Do not spawn any new device */
					/* requests if NQS is shutting down */
	device = Devset;
	/*
	 *  Walk the device set looking for enabled devices which are not
	 *  doing anything.
	 */
	while (device != (struct device *) 0) {
		newreq = sel_request (device, (struct queue *) 0);
		if (newreq != (struct request *) 0) {
			/*
			 *  There is a request that can be run on the
			 *  device from the selected queue.  Spawn it.
			 */
			nqs_spawn (newreq, device);
		}
		device = device->next;	/* Check the next device */
	}
	update_devqueues();		/* Update database image of queues */
}


/*** sel_request
 *
 *
 *	struct request *sel_request():
 *
 *	Determine the request that should be served next by the specified
 *	device.
 *
 *	Returns:
 *		A pointer to the first runnable request residing in the
 *		highest priority queue that should should be served next
 *		by the specified device.  If there are no queues containing
 *		requests to be started on the specified, device, or the
 *		device is disabled, or the device is actively serving a
 *		current request, then (struct request *) 0 is returned.
 */
static struct request *sel_request (device, tie_loser)
struct device *device;			/* Device */
struct queue *tie_loser;		/* If two equal priority */
					/* queues are tied for highest*/
					/* priority, then do NOT spawn*/
					/* a request from this queue so */
					/* that we have round-robin */
					/* scheduling between queues at */
					/* the same priority. */
					/* (This argument can be NULL) */
{
	register struct qdevmap *map;	/* Pointer to queue/device mapping */
	register struct queue *highest;	/* Highest priority queue */
					/* served by device */
	register struct queue *queue;	/* Device queue set walking */
	register short is_a_tie;	/* BOOLEAN non-zero if tie for first */
	register int priority;		/* Winning high priority value */
	register struct qdevmap *map_loser;
					/* Ptr to map entry of loser for */
					/* the device */
	register struct request *wreq;	/* Used to walk request set for queue */
	struct request *newreq;		/* Chosen request to be run next */
	char *cp;			/* Pointer to forms name */

	if ((device->status & DEV_ENABLED) == 0 ||
	    (device->status & DEV_ACTIVE)) return ((struct request*) 0);
	/*
	 *  The device is enabled, but is inactive.
	 */
	newreq = (struct request *) 0;	/* Not yet known */
	highest = (struct queue *) 0;	/* Not yet known */
	map_loser = (struct qdevmap *)0;/* Not yet known */
	map = device->dqmaps;		/* First mapping */
	/*
	 *  Loop over all device/queue mappings, to find the highest
	 *  priority queue served by this device, that also has a
	 *  request that can be run on the specified device.
	 */
	while (map != (struct qdevmap *) 0) {
	    queue = map->queue;
	    if (queue == tie_loser) map_loser = map;
	    if (highest == (struct queue *) 0) {
		if (queue->q.queuedcount &&
		   (queue->q.status & QUE_RUNNING &&
		    complex_space_4deviceq(queue))) {
		    wreq = queue->queuedset;
		    while (wreq != (struct request *) 0) {
			/*
			 *  Loop over queued requests.
			 */
			if (wreq->v1.req.v2.dev.forms [0] != '\0') {
			    cp = wreq->v1.req.v2.dev.forms;
			}
			else cp = Defprifor;	/* Default */
			if ((*cp=='\0') || (strcmp (device->forms, cp) == 0)) {
			    /*
			     *  There exists a request in the
			     *  queue that can run on the
			     *  specified device.
			     */
			    is_a_tie = 0;	/* No ties */
			    highest = queue;
			    newreq = wreq;
			    priority = queue->q.priority;
			    wreq = (struct request *) 0;
						/* Exit loop */
			}
			else wreq = wreq->next;
		    }
		}
	    }
	    else if (queue->q.queuedcount && 
		    (queue->q.status & QUE_RUNNING &&
		     complex_space_4deviceq(queue))) {
		/*
		 *  This queue has requests that could run.
		 */
		if (queue->q.priority > priority) {
		    /*
		     *  This queue has a higher priority than
		     *  the current chosen queue.
		     */
		    wreq = queue->queuedset;
		    while (wreq != (struct request *) 0) {
			/*
			 *  This request is not blocked from
			 *  execution.
			 */
			if (wreq->v1.req.v2.dev.forms [0] != '\0') {
			    cp = wreq->v1.req.v2.dev.forms;
			}
			else cp = Defprifor;	/* Default */
			if ((*cp=='\0') || (strcmp (device->forms, cp) == 0)) {
			    /*
			     *  There exists a request in the
			     *  queue that can run on the
			     *  specified device.
			     */
			    is_a_tie = 0;	/* No ties */
			    highest = queue;
			    newreq = wreq;
			    priority = queue->q.priority;
			    wreq = (struct request *) 0;
						/* Exit loop */
			}
			else wreq = wreq->next;
		    }
		}
		else if (priority == queue->q.priority) {
		    /*
		     *  This queue has a priority equal to
		     *  the currently chosen queue.
		     */
		    wreq = queue->queuedset;
		    while (wreq != (struct request *) 0) {
			/*
			 *  This request is not blocked from
			 *  execution.
			 */
			if (wreq->v1.req.v2.dev.forms [0] != '\0') {
			    cp = wreq->v1.req.v2.dev.forms;
			}
			else cp = Defprifor;	/* Default */
			if ((*cp=='\0') || (strcmp (device->forms, cp) == 0)) {
			    /*
			     *  There exists a request in the
			     *  queue that can run on the
			     *  specified device.
			     */
			    is_a_tie = 1;	/* Tie for 1st */
			    wreq = (struct request *) 0;
						/* Exit loop */
			}
			else wreq = wreq->next;
		    }
		}
	    }
	    map = map->nextdq;	/* Examine next device/queue mapping */
	}
	if (highest != (struct queue *) 0 &&
	    map_loser != (struct qdevmap *) 0) {
	    /*
	     *  A queue exists with runable request(s), and we may have to
	     *  break a tie by implementing "round-robin" scheduling.
	     */
	    if (tie_loser->q.priority == priority && is_a_tie) {
		/*
		 *  Tie break required; do "round-robin" scheduling.
		 */
		map = map_loser;
		newreq = (struct request *) 0;	/* Request unknown */
		do {
		    /*
		     *  Locate the next queue whose priority is
		     *  equal to the loser queue, and whose status
		     *  is running, with requests in the queued set.
		     */
		    do {
			map = map->nextdq;
			if (map == (struct qdevmap *) 0) map = device->dqmaps;
			highest = map->queue;
		    } while (highest->q.priority != priority ||
			    (highest->q.status & QUE_RUNNING) == 0 ||
			     highest->q.queuedcount == 0);
		    /*
		     *  Now, determine if the queue has request(s)
		     *  that can run on the specified device.
		     */
		    wreq = highest->queuedset;
		    while (wreq != (struct request *) 0) {
			/*
			 *  This queue has a request that is ready
			 *  to run.
			 */
			if (wreq->v1.req.v2.dev.forms [0]) {
			    cp = wreq->v1.req.v2.dev.forms;
			}
			else cp = Defprifor;	/* Default */
			if ((*cp=='\0') || (strcmp (device->forms, cp) == 0)) {
			    /*
			     *  There exists a request in the
			     *  queue that can run on the
			     *  specified device.
			     */
			    newreq = wreq;	/* Ptr to req*/
			    wreq = (struct request *) 0;
						/* Exit loop */
			}
			else wreq = wreq->next;
		    }
		    /*
		     *  Keep looking until we've found the proper
		     *  queue and request.
		     */
		} while (newreq == (struct request *) 0);
	    }
	}
	return (newreq);
}


/*** update_devqueues
 *
 *
 *	void update_devqueues():
 *	Update database image of device queues as necessary.
 */
static void update_devqueues()
{
	register struct queue *queue;	/* Walk queue set */

	queue = Nonnet_queueset;	/* Non-network queue set */
	while (queue != (struct queue *) 0) {
		/*
		 *  Update NQS database image of queue states as
		 *  necessary.
		 */
		if (queue->q.type == QUE_DEVICE &&
		   (queue->q.status & QUE_UPDATE)) {
			/*
			 *  This queue is a device queue, and the NQS
			 *  database image of the queue must be updated.
			 */
			udb_qorder (queue);
		}
		queue = queue->next;
	}
}

/*** complex_space_4deviceq
 *
 *	
 *    int complex_space_4deviceq():
 *
 *    This function checks to see if there is room in the complex
 *    for another spawned request.
 */
int complex_space_4deviceq(queue)
 register struct queue *queue; /* queue to make room for */
{
       struct qcomplex *qcomplex;
       int complexno;

       for (complexno = 0; complexno < MAX_COMPLXSPERQ &&
               (qcomplex = queue->v1.device.qcomplex[complexno]) !=
               (struct qcomplex *) 0; complexno++) {
               if (qcomplex->runlimit == qcomplex->runcount)
                       return (0);     /* A complex this queue is a member */
                                       /* of has reached its run limit */
       }
       return (1);     /* We've looked at all complexes */
                       /* and there is room */
}                  
 
