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
 
/*++ nqs_fndreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_fndreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Find an NQS request in one of the local queues by sequence
 *	number, and machine-id only.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:58 $ $State: Exp $)
 * $Log: nqs_fndreq.c,v $
 * Revision 1.3  1994/11/19  02:52:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:34  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:08  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:06:14  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* Global vars */

/*** nqs_fndreq
 *
 *
 *	struct request *nqs_fndreq():
 *
 *	Find an NQS request in one of the local queues by original
 *	sequence number, and original machine-id.
 *
 *	Returns:
 *		A pointer to the request structure for the request,
 *		if the request was located in one of the local NQS
 *		queues.  Otherwise, a NIL request pointer is returned.
 */
struct request *nqs_fndreq (orig_seqno, orig_mid, predecessor, state)
register long orig_seqno;		/* Original req sequence number. */
register mid_t orig_mid;		/* Original machine-id */
struct request **predecessor;		/* If the request is located, then */
					/* we return a pointer to the */
					/* predecessor request structure */
					/* for the request.  If the request */
					/* is the first member in the set, */
					/* then *predecessor upon return */
					/* will be (struct request *) 0. */
int *state;				/* This parameter is a "value/return"*/
					/* parameter.  *State initially */
					/* identifies the set of request */
					/* queue states in which the request */
					/* may currently reside (RQS_). */
					/* Upon completion, *state has a */
					/* single RQS_ state value, the */
					/* state of the located request */
					/* (if the request is indeed found) */
{
	register struct queue *queue;	/* Queue set to traverse */
	register struct request *req;
	register struct request *pred;
	register short set;		/* Request set [0..6] */
	int setstate;			/* State type for search set */

	queue = Nonnet_queueset;	/* The request will always be */
					/* located in a non-network queue */
					/* if it exists (note that subre- */
					/* quests are never ever placed in */
					/* non-network queues). Subrequests*/
					/* are only found in network queues*/
					/* and they ALWAYS have a parent in*/
					/* a non-network queue */
	while (queue != (struct queue *) 0) {
		set = 0;
		while (set <= 6) {
			/*
			 *  Search the next set for the request.
			 */
			req = (struct request *) 0;
			switch (set) {
			case 0:	if (*state & RQS_DEPARTING) {
					setstate = RQS_DEPARTING;
					req = queue->departset;
				}
				break;
			case 1:	if (*state & RQS_RUNNING) {
					setstate = RQS_RUNNING;
					req = queue->runset;
				}
				break;
			case 2: if (*state & RQS_STAGING) {
					setstate = RQS_STAGING;
					req = queue->stageset;
				}
				break;
			case 3: if (*state & RQS_QUEUED) {
					setstate = RQS_QUEUED;
					req = queue->queuedset;
				}
				break;
			case 4: if (*state & RQS_WAITING) {
					setstate = RQS_WAITING;
					req = queue->waitset;
				}
				break;
			case 5: if (*state & RQS_HOLDING) {
					setstate = RQS_HOLDING;
					req = queue->holdset;
				}
				break;
			case 6: if (*state & RQS_ARRIVING) {
					setstate = RQS_ARRIVING;
					req = queue->arriveset;
				}
				break;
			}
			pred = (struct request *) 0;
			while (req != (struct request *) 0 &&
			      (req->v1.req.orig_seqno != orig_seqno ||
			       req->v1.req.orig_mid != orig_mid)) {
				pred = req;
				req = req->next;
			}
			if (req != (struct request *) 0) {
				/*
				 *  We found the request.
				 */
				*predecessor = pred;
				*state = setstate;
				return (req);
			}
			set++;		/* Examine the next set (state) */
		}
		/*
		 *  Examine the next queue in the queue set.
		 */
		queue = queue->next;
	}
	return ((struct request *) 0);	/* Not found */
}
