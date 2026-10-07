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
 
/*++ nqs_upq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upq.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS queue state update module.
 *	The resource limits "LIM_PRDRIVES" and "LIM_PRNCPUS" are
 *		not handled.
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
 * REVISION HISTORY: ($Revision: 1.7 $ $Date: 1995/02/24 23:45:50 $ $State: Exp
$)
 * $Log: nqs_upq.c,v $
 * Revision 1.7  1995/02/24  23:45:50  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 5134
 *  Testing: Developer testing
 *  Module(s):      ./cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_main.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_packet.c
 *                 ./cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 *                 ./cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 *                 ./cmds_libs/src/usr/include/nqs/nqspacket.h
 *                 ./cmds_libs/src/usr/include/nqs/nqsstruct.h
 *                 ./cmds_libs/src/usr/lib/nqs/macs_sched.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_ldconf.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_main.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_upq.
 *
 * Revision 1.6  1994/11/19  02:53:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/11/02  00:57:52  mwan
 * R1.2 mods
 *
 * Revision 1.3  1992/12/16  23:26:41  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:26:45  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:49  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:12  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_upq.c	1.3 (nqs_upq.c OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <errno.h>
#include "nqs.h"
#include "nqsdeflim.h"		/* Default batch queue resource limits */
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */

#ifdef SDSC
#include "buddyxvar.h"
#endif

extern int errno;		/* System call error number */

extern void bsc_spawn();	/* Maybe spawn some batch reqs */
extern void bytezero();		/* Zero fill */
extern int cpulimhigh();	/* Answers "Is time limit too high?" */
extern int cpulimlow();		/* Answers "Is time limit too low?" */
extern void dsc_spawn();	/* Maybe spawn some device reqs */
extern void free();		/* Deallocate memory */
extern int getrreq();		/* Get a raw request */
extern char *malloc();		/* Allocate memory */
extern void nqs_aboque();	/* Abort all reqs in specified queue */
extern void nqs_abort();	/* Exit */
extern void nqs_disreq();	/* Dispose of request */
extern int nqs_enfile();	/* Does errno have a fatal value? */
extern struct queue *nqs_fndnnq();
				/* Find non-network queue by name */
extern void psc_spawn();	/* Maybe spawn some pipe reqs */
extern int quolimhigh();	/* Answers "Is quota limit too high?" */
extern int quolimlow();		/* Answers "Is quota limit too low?" */
extern int strcmp();		/* String comparison */
extern char *strcpy();		/* String copy */
extern void udb_creque();	/* Add a new queue definition to the */
				/* NQS database image */
extern void udb_delque();	/* Delete a queue descriptor and any */
				/* associated queue-ordering or queue- */
				/* access file from the NQS database */
extern void udb_delquedes();	/* Delete a pipe-queue to destination */
				/* mapping from the NQS database */
extern void udb_qaccess();	/* Update queue access file */
extern void udb_queue();	/* Update the state for a queue as */
				/* stored in the NQS database (the */
				/* request ordering in the queue remains */
				/* unchanged in the NQS database). */
extern void udb_qorder();	/* Update queue ordering file */
extern void upd_deldes();	/* Delete a pipe queue destination */


/*** upq_aboque
 *
 *
 *	long upq_aboque():
 *
 *	Abort all running requests in the named local batch, device,
 *	or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if the named local queue does not exist.
 */
long upq_aboque (que_name, wait_time)
char *que_name;				/* Name of queue to be aborted */
int wait_time;				/* Time to wait before sending */
					/* SIGKILL signal after SIGTERM */
{
	struct queue *queue;		/* Pointer to queue structure */

	queue = nqs_fndnnq (que_name);
	if (queue == (struct queue *)0) {
		return(TCML_NOSUCHQUE);	/* No such local queue */
	}
	nqs_aboque (queue, wait_time,0);/* Do not requeue aborted reqs */
	return (TCML_COMPLETE);		/* Successful completion */
}


/*** upq_addqueacc
 *
 *
 *	long upq_addqueacc():
 *	Add queue access for a group or user.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_ALREADACC:	if the queue was restricted, and the group
 *				or user already had access.
 *		TCML_INSUFFMEM:	if there was insufficient memory to add
 *				the group or user.
 *		TCML_NOSUCHQUE:	if the named queue did not exist.
 *		TCML_UNRESTR:	if the queue was unrestricted.
 *		TCML_WROQUETYP:	if the queue was a network queue.
 */
long upq_addqueacc (quename, who)
char *quename;				/* Queue name */
unsigned long who;			/* Group or user to be added */
{
	long upq_lowaddacc();		/* Low-level add queue access */

	register struct queue *queue;
	register long upd;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *) 0) {
		return (TCML_NOSUCHQUE);/* No such queue */
	}
	if ((upd = upq_lowaddacc (queue, who)) != TCML_COMPLETE) {
		return (upd);
	}
	udb_qaccess (queue);		/* Update the database image */
	return (TCML_COMPLETE);
}


/*** upq_creque
 *
 *
 *	long upq_creque():
 *	Create an NQS batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_ALREADEXI:	if the device already exists.
 *		TCML_INSUFFMEM:	if insufficient memory to create queue
 *				descriptor.
 */
long upq_creque (quename, quetype, quepriority, querunlimit, pipeonly,
		 servername)
char *quename;			/* Name of queue */
int quetype;			/* Queue type: QUE_ */
int quepriority;		/* Queue priority */
int querunlimit;		/* Queue run-limit */
int pipeonly;			/* BOOLEAN non-zero if the queue has the */
				/* PIPEONLY entry attribute */
char *servername;		/* Server and arguments for pipe queues */
{
	void insquebypri();		/* Insert queue by priority */

	register struct queue *queue;
	register struct queue *prevqueue;
	register int res;		/* Result code */
	register int i;			/* Iteration loop var */

	/*
	 *  Search the lexicographically ordered queue set for the specified
	 *  queue.
	 */
	prevqueue = (struct queue *)0;
	queue = Nonnet_queueset;	/* Creating a non-network queue */
	while (queue != (struct queue *) 0) {
		res = strcmp (queue->q.namev.name, quename);
		if (res < 0) {
			/*
			 *  The queue has not yet been found, but might
			 *  still be present in the queue set.
			 */
			prevqueue = queue;
			queue = queue->next;
		}
		else if (res == 0) {
			/*
			 *  The queue already exists.
			 */
			return (TCML_ALREADEXI);
		}
		else queue = (struct queue *)0;	/* Set to exit */
	}
	/*
	 *  The queue does not already exist, and when created, it should be
	 *  inserted with "prevqueue" as its predecessor (unless "prevqueue"
	 *  is null, in which case the new queue must be inserted at the very
	 *  beginning of the queue set.
	 */
	if ((queue = (struct queue *)
		      malloc (sizeof (struct queue))) == (struct queue *)0) {
		return (TCML_INSUFFMEM);	/* Insufficient memory */
	}
	bytezero ((char *) queue, sizeof (struct queue));
	queue->departset = (struct request *) 0;/* No reqs in departing set */
	queue->runset = (struct request *) 0;	/* No reqs in running set */
	queue->stageset = (struct request *) 0;	/* No reqs in staging set */
	queue->queuedset = (struct request *)0; /* No reqs in eligible set */
	queue->waitset = (struct request *) 0;	/* No reqs in waiting set */
	queue->holdset = (struct request *) 0;	/* No reqs in holding set */
	queue->arriveset = (struct request *)0;	/* No reqs in arriving set */
	queue->sigkill = 0;			/* No pending abort SIGKILL */
	queue->q.orderversion = 0;	/* Current ordering version = 0 */
	queue->q.accessversion = 0;	/* Current access version = 0 */
	queue->q.status = 0;		/* Queue is stopped and disabled, */
					/* with unrestricted access */
	if (pipeonly) queue->q.status |= QUE_PIPEONLY;
					/* PIPEONLY attribute specified */
	queue->q.type = quetype;	/* Store queue type */
	queue->q.priority = quepriority;/* Assign priority */
	strcpy (queue->q.namev.name, quename);	/* Remember queue name */
#if	UNICOS | SGI | SYS52 | UTS | OSF
	queue->q.ru_stime = 0;		/* No clock ticks yet consumed */
	queue->q.ru_utime = 0;
#else
#if	BSD42 | BSD43 | ULTRIX
	queue->q.ru_stime_usec = 0;	/* No clock ticks yet consumed */
	queue->q.ru_stime = 0;
	queue->q.ru_utime_usec = 0;
	queue->q.ru_utime = 0;
#else
BAD SYSTEM TYPE
#endif
#endif
	queue->q.departcount = 0;	/* No reqs are departing/staging-out */
	queue->q.runcount = 0;		/* No reqs are running */
	queue->q.stagecount = 0;	/* No reqs are staging-in */
	queue->q.queuedcount = 0;	/* No reqs are eligible for execution */
	queue->q.waitcount = 0;		/* No reqs are waiting */
	queue->q.holdcount = 0;		/* No reqs are holding */
	queue->q.arrivecount = 0;	/* No reqs are arriving */
	queue->next = (struct queue *)0;/* No next queue descriptor */
	/*
	 *  Initialize queue type dependent variants.
	 */
	switch (quetype) {
	case QUE_BATCH:
		/*
		 *  We are creating a batch queue.
		 */
		for (i = 0; i < BSET_CPUVECSIZE; i++) {
			queue->q.v1.batch.cpuset [i] = 0;	/* RESERVED */
			queue->q.v1.batch.cpuuse [i] = 0;	/* RESERVED */
		}
#ifdef SDSC
		queue->q.v1.batch.node_group = 0;
#else
		queue->q.v1.batch.ncpuset = 0;	/* RESERVED */
#endif
		queue->q.v1.batch.ncpuuse = 0;	/* RESERVED */
		for (i = 0; i < BSET_RS1VECSIZE; i++) {
			queue->q.v1.batch.rs1set [i] = 0;	/* RESERVED */
			queue->q.v1.batch.rs1use [i] = 0;	/* RESERVED */
		}

#ifdef SDSC
                queue->q.v1.batch.sPerQueMaxQueuedULimit = 0;
                /* we are creating new queue where */
                /* value 0 means this limit will NOT be enforced by NQS and qsub */
#else
                queue->q.v1.batch.nrs1set = 0;  /* RESERVED */
#endif

		queue->q.v1.batch.nrs1use = 0;	/* RESERVED */
		for (i = 0; i < BSET_RS2VECSIZE; i++) {
			queue->q.v1.batch.rs2set [i] = 0;	/* RESERVED */
			queue->q.v1.batch.rs2use [i] = 0;	/* RESERVED */
		}
		queue->q.v1.batch.nrs2set = 0;	/* RESERVED */
		queue->q.v1.batch.nrs2use = 0;	/* RESERVED */
		queue->q.v1.batch.runlimit = querunlimit;
		queue->q.v1.batch.explicit = 0L;
		queue->q.v1.batch.infinite = 0L;
		/*
		 *  Explicit and infinite are bit vectors. They control
		 *  several fields below, beginning with ppcoreunits and
		 *  ending with ppnice.  The bits work like this: '1' in
		 *  explicit means that the corresponding limit was set using
		 *  qmgr; '0' means that the limit was set by NQS itself.
		 *  '1' in infinite means that the limit is infinite and that
		 *  coeff(icient) and units should be ignored;
		 *  '0' means that coeff(icient) and units hold the limit.
		 *  The correspondence between the various limits and the bits
		 *  of explicit and infinite is defined in nqs.h (see LIM_???).
		 */
#ifdef SDSC
#if	(VALID_LIMITS & LIM_PPCPUT)
		queue->q.v1.batch.infinite |= LIM_PPCPUT;
#endif
#if	(VALID_LIMITS & LIM_PRCPUT)
		queue->q.v1.batch.infinite |= LIM_PRCPUT;
#endif
#if	(VALID_LIMITS & LIM_PPCORE)
		queue->q.v1.batch.infinite |= LIM_PPCORE;
#endif
#if	(VALID_LIMITS & LIM_PPDATA)
		queue->q.v1.batch.infinite |= LIM_PPDATA;
#endif
#if	(VALID_LIMITS & LIM_PPPFILE)
		queue->q.v1.batch.infinite |= LIM_PPPFILE;
#endif
#if	(VALID_LIMITS & LIM_PPSTACK)
		queue->q.v1.batch.infinite |= LIM_PPSTACK;
#endif
#if	(VALID_LIMITS & LIM_PPWORK)
		queue->q.v1.batch.infinite |= LIM_PPWORK;
#endif
#endif
		queue->q.v1.batch.ppcoreunits = DLIM_PPCOREUNITS;
		queue->q.v1.batch.ppcorecoeff = DLIM_PPCORECOEFF;
		queue->q.v1.batch.ppdataunits = DLIM_PPDATAUNITS;
		queue->q.v1.batch.ppdatacoeff = DLIM_PPDATACOEFF;
		queue->q.v1.batch.pppfileunits = DLIM_PPPFILEUNITS;
		queue->q.v1.batch.pppfilecoeff = DLIM_PPPFILECOEFF;
		queue->q.v1.batch.prpfileunits = DLIM_PRPFILEUNITS;
		queue->q.v1.batch.prpfilecoeff = DLIM_PRPFILECOEFF;
		queue->q.v1.batch.ppqfileunits = DLIM_PPQFILEUNITS;
		queue->q.v1.batch.ppqfilecoeff = DLIM_PPQFILECOEFF;
		queue->q.v1.batch.prqfileunits = DLIM_PRQFILEUNITS;
		queue->q.v1.batch.prqfilecoeff = DLIM_PRQFILECOEFF;
		queue->q.v1.batch.pptfileunits = DLIM_PPTFILEUNITS;
		queue->q.v1.batch.pptfilecoeff = DLIM_PPTFILECOEFF;
		queue->q.v1.batch.prtfileunits = DLIM_PRTFILEUNITS;
		queue->q.v1.batch.prtfilecoeff = DLIM_PRTFILECOEFF;
		queue->q.v1.batch.ppmemunits = DLIM_PPMEMUNITS;
		queue->q.v1.batch.ppmemcoeff = DLIM_PPMEMCOEFF;
		queue->q.v1.batch.prmemunits = DLIM_PRMEMUNITS;
		queue->q.v1.batch.prmemcoeff = DLIM_PRMEMCOEFF;
		queue->q.v1.batch.ppstackunits = DLIM_PPSTACKUNITS;
		queue->q.v1.batch.ppstackcoeff = DLIM_PPSTACKCOEFF;
		queue->q.v1.batch.ppworkunits = DLIM_PPWORKUNITS;
		queue->q.v1.batch.ppworkcoeff = DLIM_PPWORKCOEFF;
		queue->q.v1.batch.ppcpusecs = DLIM_PPCPUSECS;
		queue->q.v1.batch.ppcpums = DLIM_PPCPUMS;
		queue->q.v1.batch.prcpusecs = DLIM_PRCPUSECS;
		queue->q.v1.batch.prcpums = DLIM_PRCPUMS;
		queue->q.v1.batch.ppnice = DLIM_PPNICE;
		queue->q.v1.batch.prdrives = DLIM_PRDRIVES;
		queue->q.v1.batch.prncpus = DLIM_PRNCPUS;
		queue->v1.batch.acclistp = (struct acclist *) 0;
					/* No access list set */
		queue->v1.batch.nextpriority = (struct queue *) 0;
					/* No next queue in the */
					/* priority ordered list */
		for (i=0; i < MAX_COMPLXSPERQ; i++) {
			queue->v1.batch.qcomplex [i] = (struct qcomplex *) 0;
		}			/* Queue complexes are not supported */
					/* (yet) */
		/*
		 *  Insert the new batch queue into the priority-ordered
		 *  batch queue set.
		 */
		insquebypri (queue);
		break;
	case QUE_DEVICE:
		/*
		 *  We are creating a device queue.
		 */
		queue->v1.device.acclistp = (struct acclist *) 0;
					/* No access list set */
		queue->v1.device.qdmaps = (struct qdevmap *) 0;
		/*
		 *  Please note that is no priority-ordered device
		 *  queue set.
		 */
		break;
	case QUE_NET:
		/*
		 *  We are creating a network queue.
		 */
		queue->q.v1.network.runlimit = querunlimit;
		strcpy (queue->q.v1.network.server, servername);
		queue->q.v1.network.retry_at = 0;
		queue->q.v1.network.retrytime = 0;
		queue->q.v1.network.retrydelta = 0;
		queue->v1.network.nextpriority = (struct queue *) 0;
					/* No next queue in the */
					/* priority ordered list */
		/*
		 *  Insert the new network queue into the priority-ordered
		 *  network queue set.
		 */
		insquebypri (queue);
		break;
	case QUE_PIPE:
		/*
		 *  We are creating a pipe queue.
		 */
		queue->q.v1.pipe.runlimit = querunlimit;
		strcpy (queue->q.v1.pipe.server, servername);
		queue->v1.pipe.acclistp = (struct acclist *) 0;
					/* No access list set */
		queue->v1.pipe.destset = (struct qdestmap *) 0;
					/* Pipe queue destination */
					/* set is empty */
		queue->v1.pipe.nextpriority = (struct queue *) 0;
					/* No next queue in the */
					/* priority ordered list */
		/*
		 *  Insert the new pipe queue into the priority-ordered
		 *  pipe queue set.
		 */
		insquebypri (queue);
		break;
	}
	/*
	 *  The new queue structure is now completely initialized; insert
	 *  it into the lexicographically ordered queue set.
	 */
	if (prevqueue == (struct queue *) 0) {
		/*
		 *  Insert at the beginning of the list.
		 */
		queue->next = Nonnet_queueset;
		Nonnet_queueset = queue;
	}
	else {
		queue->next = prevqueue->next;
		prevqueue->next = queue;
	}
	if (Booted) {
		/*
		 *  NQS is completely "booted", and so this new queue needs
		 *  to be added to the NQS database image.
		 */
		udb_creque (queue);	/* Add the queue descriptor to the */
					/* NQS database image. */
	}
	else New_queue = queue;		/* Otherwise, update most recently */
					/* added queue for the benefit of  */
					/* nqs_ldconf.c */
	return (TCML_COMPLETE);
}


/*** upq_delque
 *
 *
 *	long upq_delque():
 *	Delete an NQS batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if no such queue existed to delete.
 *		TCML_QUEENABLE:	Queue is enabled.
 *		TCML_QUEHASREQ:	if the queue contains requests (and
 *				therefore cannot be deleted).
 */
long upq_delque (quename)
char *quename;				/* Name of queue */
{
	long unrestr_access();		/* Set unrestricted access */
	void remquefrompri();		/* Remove queue from priority */
					/* ordered set */

	struct qdestmap *mapdest;
	register struct qdevmap *map;
	struct qdestmap *nextmapdest;
	register struct qdevmap *walkmap;
	register struct qdevmap *prevmap;
	register struct queue *prevqueue;
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) return (TCML_NOSUCHQUE);
	if (queue->q.status & QUE_ENABLED) return (TCML_QUEENABLE);
	if (queue->q.departcount || queue->q.runcount ||
	    queue->q.queuedcount || queue->q.waitcount ||
	    queue->q.holdcount || queue->q.arrivecount) {
		return(TCML_QUEHASREQ);	/* Cannot delete a queue with reqs */
	}
	if (queue->q.type != QUE_NET) {
		unrestr_access (queue);	/* Free in memory access list */
					/* Disk access list will be removed */
					/* by udb_delque() */
	}
	switch (queue->q.type) {
	case QUE_BATCH:
		/*
		 *  Remove the batch queue from the priority-ordered
SUCHCOM);
		 *  batch queue set.
		 */
		remquefrompri (queue);
		remquefromcomp(queue,queue->v1.batch.qcomplex);
		break;
	case QUE_DEVICE:
		/*
		 *  Delete the queue/device mapping set for the queue.
		 */
		map = queue->v1.device.qdmaps;
		while (map != (struct qdevmap *)0) {
			/*
			 *  Remove this mapping from the device-to-
			 *  queue mapping set as threaded from the
			 *  device.
			 */
			prevmap = (struct qdevmap *)0;
			walkmap = map->device->dqmaps;
			while (walkmap != map) {
				prevmap = walkmap;
				walkmap = walkmap->nextdq;
			}
			if (prevmap == (struct qdevmap *)0) {
				map->device->dqmaps = map->nextdq;
			}
			else prevmap->nextdq = map->nextdq;
			/*
			 *  Delete the queue/device mapping.
			 */
			udb_delquedev (map);	/* Update NQS database */
			walkmap = map->nextqd;
			free ((char *) map);
			map = walkmap;
		}
		/*
		 *  Please note that is no priority-ordered device
		 *  queue set.
		 */
		remquefromcomp(queue,queue->v1.device.qcomplex);
		break;
	case QUE_NET:
		/*
		 *  Remove the network queue from the priority-ordered
		 *  network queue set.
		 */
		remquefrompri (queue);
		break;
	case QUE_PIPE:
		/*
		 *  Delete the destination set for the queue.
		 */
		mapdest = queue->v1.pipe.destset;/* First member of dest set */
		while (mapdest != (struct qdestmap *) 0) {
			/*
			 *  Delete the queue/destination mapping.
			 */
			udb_delquedes (mapdest);	/* Update database */
			if (--mapdest->pipeto->refcount == 0) {
				/*
				 *  No more references exist to this pipe
				 *  queue destination.  Delete it.
				 */
				upd_deldes (mapdest->pipeto);
			}
			nextmapdest = mapdest->next;
			free ((char *) mapdest);
			mapdest = nextmapdest;
		}
		/*
		 *  Remove the pipe queue from the priority-ordered
		 *  pipe queue set.
		 */
		remquefrompri (queue);
		break;
	}
	/*
	 *  Remove the queue descriptor from the non-network queue set.
	 */
	if (Nonnet_queueset == queue) Nonnet_queueset = queue->next;
	else {
		prevqueue = Nonnet_queueset;
		while (prevqueue->next != queue) {
			prevqueue = prevqueue->next;
		}
		prevqueue->next = queue->next;
	}
#ifdef SDSC
	/* If the queue name match Defbatque, remove Defbatque */

	if (strcmp (Defbatque, quename) == 0) {	/* A match */
		Defbatque[0] = '\0';
		udb_genparams();
	}
#endif
	udb_delque (queue);		/* Delete the queue descriptor from */
					/* the NQS database image. */
	free ((char *) queue);		/* Delete the queue structure */
	return (TCML_COMPLETE);
}


/*** upq_delqueacc
 *
 *
 *	long upq_delqueacc():
 *	Delete queue access for a group or user.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOACCNOW:	if the queue was restricted, and the group
 *				or user did not have access.
 *		TCML_NOSUCHQUE:	if the named queue did not exist.
 *		TCML_UNRESTR:	if the queue was unrestricted.
 *		TCML_WROQUETYP:	if the queue was a network queue.
 */
long upq_delqueacc (quename, who)
char *quename;				/* Queue name */
unsigned long who;			/* Group or user to be deleted */
{
	long del_access();		/* Delete queue access */

	register struct queue *queue;
	register long upd;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *) 0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if ((upd = del_access (queue, who)) != TCML_COMPLETE) {
		return (upd);
	}
	udb_qaccess (queue);		/* Update the database image */
	return (TCML_COMPLETE);
}


/*** upq_disque
 *
 *
 *	long upq_disque():
 *	Disable an NQS batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if no such queue exists.
 */
long upq_disque (quename)
char *quename;				/* Name of queue */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);	/* Get queue by name */
	if (queue == (struct queue *)0) return (TCML_NOSUCHQUE);
	queue->q.status &= ~QUE_ENABLED;/* Mark queue as disabled */
	udb_queue (queue);		/* Update the NQS database image */
	return (TCML_COMPLETE);
}


/*** upq_enaque
 *
 *
 *	long upq_enaque():
 *	Enable an NQS batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if no such queue exists.
 */
long upq_enaque (quename)
char *quename;				/* Name of queue */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);	/* Get queue by name */
	if (queue == (struct queue *)0) return (TCML_NOSUCHQUE);
	queue->q.status |= QUE_ENABLED;	/* Mark queue as enabled */
	udb_queue (queue);		/* Update NQS database image */
	return (TCML_COMPLETE);
}


/*** upq_lowaddacc
 *
 *
 *	long upq_lowaddacc();
 *
 *	Low-level add a group or user to the list of users for a queue.
 *	This function is called from ../src/nqs_ldconf.c!
 *
 *	Returns: TCML_ALREADACC, TCML_INSUFFMEM, TCML_COMPLETE,
 *		 TCML_UNRESTR, or TCML_WROQUETYP.
 */
long upq_lowaddacc (queue, who)
struct queue *queue;				/* Queue descriptor */
unsigned long who;				/* Gid or uid */
{
	struct acclist *acclistp;
	struct acclist *lastlistp;
	int i;
	
	if (queue->q.type == QUE_NET) return (TCML_WROQUETYP);
	if ((queue->q.status & QUE_BYGIDUID) == 0) return (TCML_UNRESTR);
	/*
	 * We never add root to the access list.
	 * If root wants access, we always allow it.
	 */
	if (who == 0) return (TCML_ALREADACC);
	switch (queue->q.type) {
	case QUE_BATCH:
		acclistp = queue->v1.batch.acclistp;
		break;
	case QUE_DEVICE:
		acclistp = queue->v1.device.acclistp;
		break;
	case QUE_PIPE:
		acclistp = queue->v1.pipe.acclistp;
		break;
	}
	lastlistp = (struct acclist *) 0;
	while (acclistp != (struct acclist *) 0) {
		for (i = 0; i < ACCLIST_SIZE; i++) {
			/* Zeroes come only at the end.  */
			if (acclistp->entries[i] == 0) {
				acclistp->entries[i] = who;
				return (TCML_COMPLETE);
			}
			/* Not the end.  Check for duplication.  */
			if (acclistp->entries [i] == who) {
				return (TCML_ALREADACC);
			}
		}
		lastlistp = acclistp;
		acclistp = acclistp->next;
	}
	/*
	 * The new group or user was not in the list, and we haven't
	 * yet found space in the list to add him, her, or them.
	 */
	if ((acclistp = (struct acclist *)
			malloc (sizeof (struct acclist)))
			== (struct acclist *) 0) {
		return (TCML_INSUFFMEM);
	}
	switch (queue->q.type) {
	case QUE_BATCH:
		if (lastlistp == (struct acclist *) 0) {
			queue->v1.batch.acclistp = acclistp;
		}
		else {
			lastlistp->next = acclistp;
		}
		break;
	case QUE_DEVICE:
		if (lastlistp == (struct acclist *) 0) {
			queue->v1.device.acclistp = acclistp;
		}
		else {
			lastlistp->next = acclistp;
		}
		break;
	case QUE_PIPE:
		if (lastlistp == (struct acclist *) 0) {
			queue->v1.pipe.acclistp = acclistp;
		}
		else {
			lastlistp->next = acclistp;
		}
		break;
	}
	acclistp->next = (struct acclist *) 0;
	acclistp->entries [0] = who;
	for (i = 1; i < ACCLIST_SIZE; i++) {
		acclistp->entries [i] = 0;
	}
	return (TCML_COMPLETE);
}


/*** upq_purque
 *
 *
 *	long upq_purque():
 *
 *	Purge an NQS batch, device, or pipe queue.  Delete all requests
 *	not presently running in the specified queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if no such queue exists.
 */
long upq_purque (quename)
char *quename;				/* Name of queue to be purged */
{
	register struct queue *queue;	/* Pointer to queue structure */
	register struct request *req;	/* Walk request sets in queue */
	register struct request *next;	/* Next request */
	register struct request *lastkept;	/* Last request not discarded */


	queue = nqs_fndnnq (quename);	/* Get queue by name */
	if (queue == (struct queue *)0) return (TCML_NOSUCHQUE);
	/*
	 *  Purge requests from the queued set.
	 */
	req = queue->queuedset;
	queue->queuedset = (struct request *) 0;
	lastkept = (struct request *) 0;
	while (req != (struct request *) 0) {
		next = req->next;
		if (req->status & RQF_PREDEPART) {
			/*
			 *  ****TRIAGE****  You cannot delete a request
			 *  in a pre-depart or depart state until network
			 *  queues are implemented!
			 */
			if (queue->queuedset == (struct request *) 0) {
				queue->queuedset = req;
			}
			else {
				lastkept->next = req;
			}
			req->next = (struct request *) 0;
			lastkept = req;
		}
		else {
			nqs_disreq (req, 1);	/* Dispose of request */
			--queue->q.queuedcount;
		}
		req = next;		/* Get the next file */
	}
	/*
	 *  Purge requests from the waiting set.
	 */
	req = queue->waitset;
	queue->waitset = (struct request *) 0;
	lastkept = (struct request *) 0;
	while (req != (struct request *) 0) {
		next = req->next;
		if (req->status & RQF_PREDEPART) {
			/*
			 *  ****TRIAGE****  You cannot delete a request
			 *  in a pre-depart or depart state until network
			 *  queues are implemented!
			 */
			if (queue->waitset == (struct request *) 0) {
				queue->waitset = req;
			}
			else {
				lastkept->next = req;
			}
			req->next = (struct request *) 0;
			lastkept = req;
		}
		else {
			nqs_disreq (req, 1);	/* Dispose of request */
			--queue->q.waitcount;
		}
		req = next;		/* Get the next file */
	}
	/*
	 *  Purge requests from the holding set.
	 */
	req = queue->holdset;
	queue->holdset = (struct request *) 0;
	lastkept = (struct request *) 0;
	while (req != (struct request *) 0) {
		next = req->next;
		if (req->status & RQF_PREDEPART) {
			/*
			 *  ****TRIAGE****  You cannot delete a request
			 *  in a pre-depart or depart state until network
			 *  queues are implemented!
			 */
			if (queue->holdset == (struct request *) 0) {
				queue->holdset = req;
			}
			else {
				lastkept->next = req;
			}
			req->next = (struct request *) 0;
			lastkept = req;
		}
		else {
			nqs_disreq (req, 1);	/* Dispose of request */
			--queue->q.holdcount;
		}
		req = next;		/* Get the next file */
	}
	/*
	 *  Purge requests from the arriving set.
	 */
	req = queue->arriveset;
	while (req != (struct request *) 0) {
		next = req->next;
		nqs_disreq (req, 1);	/* Dispose of request */
		req = next;		/* Get the next file */
	}
	queue->arriveset = (struct request *) 0;
	queue->q.arrivecount = 0;	/* No arriving requests */
	udb_qorder (queue);		/* Update database image */
	return (TCML_COMPLETE);		/* Successful completion */
}


/*** upq_setcpulim
 *
 *
 *	long upq_setcpulim():
 *	Set one of the two CPU time limits for a batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:   if the specified CPU time limit had to
 *				 be brought within the bounds supported
 *				 by the local host, but no grandfather
 *				 clauses had to be applied;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause;
 *		TCML_INTERNERR:	 if there was control file trouble;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist;
 *		TCML_NOSUCHQUO:	 if the named quota does not exist
 *				 on this machine;
 *		TCML_WROQUETYP:  if the named queue does not support
 *				 the setting of this limit.
 */
long upq_setcpulim (quename, seconds, ms, infinite, limit_type)
char *quename;			/* Queue name */
unsigned long seconds;		/* Integral part of limit (in seconds) */
short ms;			/* Milliseconds */
short infinite;			/* Non-zero if infinite */
long limit_type;		/* Which limit, expressed in LIM_??? */
{
	long cpu_grandfather();	/* Check for grandfather clause on CPU limit */

	register struct queue *queue;
#if	(VALID_LIMITS & (LIM_PPCPUT | LIM_PRCPUT))
	short fix;		/* Boolean; did we bring lim within bounds? */
	unsigned long new_secs;	/* Holds cpulimhigh(), cpulimlow() ret value */
	short new_ms;		/* Holds cpulimhigh(), cpulimlow() ret value */
	register long retcode;	/* Return completion code */
#endif

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
#if	(VALID_LIMITS & (LIM_PPCPUT | LIM_PRCPUT))
	if (queue->q.type == QUE_BATCH) {
		queue->q.v1.batch.explicit |= limit_type;
		fix = 0;
		/*
		 *  If necessary, change the value entered
		 *  by the manager to something that can be
		 *  enforced on this machine.
		 */
		if (cpulimhigh (seconds, ms, infinite, &new_secs, &new_ms)) {
			infinite = 0;	/* Altered value is never infinite */
			fix = 1;
			seconds = new_secs;
			ms = new_ms;
		}
		if (infinite) queue->q.v1.batch.infinite |= limit_type;
		else {
			if (cpulimlow (seconds, ms, &new_secs, &new_ms)) {
				fix = 1;
				seconds = new_secs;
				ms = new_ms;
			}
			queue->q.v1.batch.infinite &= ~limit_type;
			switch (limit_type) {
#if	(VALID_LIMITS & LIM_PPCPUT)
			case LIM_PPCPUT:
				queue->q.v1.batch.ppcpusecs = seconds;
				queue->q.v1.batch.ppcpums = ms;
				break;
#endif
#if	(VALID_LIMITS & LIM_PRCPUT)
			case LIM_PRCPUT:
				queue->q.v1.batch.prcpusecs = seconds;
				queue->q.v1.batch.prcpums = ms;
				break;
#endif
			}
		}
		udb_queue (queue);	/* Update NQS database image */
		if (infinite) {
			/*
			 *  The set operation was successful, and
			 *  we do not have to check for requests
			 *  that will want more than the new limit.
			 */
			return (TCML_COMPLETE);
		}
		/*
		 *  Check for request grandfather clauses.
		 */
		retcode = cpu_grandfather (queue->departset, queue,
					   limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = cpu_grandfather (queue->runset, queue, limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = cpu_grandfather (queue->stageset, queue,
					   limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = cpu_grandfather (queue->queuedset, queue,
					   limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = cpu_grandfather (queue->waitset, queue, limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = cpu_grandfather (queue->holdset, queue, limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = cpu_grandfather (queue->arriveset, queue,
					   limit_type);
		if (retcode != TCML_COMPLETE) return (retcode);
		if (fix) {
			/*
			 *  The set operation was successful, but NQS had
			 *  to alter the limit value slightly because the
			 *  original numerical limit could not have been
			 *  enforced on the local host.
			 */
			return (TCML_FIXBYNQS);
		}
		return (TCML_COMPLETE);
	}
	return (TCML_WROQUETYP);	/* Wrong queue type */
#else
	return (TCML_NOSUCHQUO);	/* No such quota on this machine */
#endif
}


/*** upq_setentatr
 *
 *
 *	long upq_setentatr():
 *	Set queue entry attribute.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 */
long upq_setentatr (quename, pipeonly)
char *quename;			/* Queue name */
int pipeonly;			/* Non-zero if pipeonly attr should be set */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if (pipeonly) queue->q.status |= QUE_PIPEONLY;
	else queue->q.status &= ~QUE_PIPEONLY;
	udb_queue (queue);		/* Update queue header */
	return (TCML_COMPLETE);
}

/*** upq_setniclim
 *
 *
 *	long upq_setniclim():
 *	Set the nice limit value for a batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:   if the specified nice value had to
 *				 be brought within the bounds supported
 *				 by the local host, but no grandfather
 *				 clauses had to be applied;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause;
 *		TCML_INTERNERR:	 if there was control file trouble;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist;
 *		TCML_NOSUCHQUO:	 if the named quota does not exist
 *				 on this machine;
 *		TCML_WROQUETYP:  if the named queue does not support
 *				 the setting of this limit.
 */
long upq_setniclim (quename, nice_value)
char *quename;				/* Queue name */
int nice_value;				/* [MIN_REQNICE..MAX_REQNICE] */
{
	long scalar_grandfather();	/* Check for grandfather clause */

	register struct queue *queue;
	short high_fix;			/* Boolean bounded nice to max */
	short low_fix;			/* Boolean bounded nice to min */
	register long retcode;		/* Return completion code */

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
#if	(VALID_LIMITS & LIM_PPNICE)
	if (queue->q.type == QUE_BATCH) {
		queue->q.v1.batch.explicit |= LIM_PPNICE;
		queue->q.v1.batch.infinite &= ~LIM_PPNICE;
		high_fix = 0;
		low_fix = 0;
		/*
		 *  If necessary, change the value entered by the
		 *  manager to something that can be enforced on
		 *  this machine.
		 */
		if (nice_value > MAX_NICE) {
			high_fix = 1;
			nice_value = MAX_NICE;
		}
		if (nice_value < MIN_NICE) {
			low_fix = 1;
			nice_value = MIN_NICE;
		}
		queue->q.v1.batch.ppnice = nice_value;
		udb_queue (queue);	/* Update NQS database image */
		if (low_fix) {
			/*
			 *  The set operation was successful, but NQS
			 *  had to bind the numerical value of the
			 *  original nice value to the minimum
			 *  numerical value supported on the local
			 *  host.  In this case, it is not necessary
			 *  to check for request grandfather clauses.
			 */
			return (TCML_FIXBYNQS);
		}
		retcode = scalar_grandfather (queue->departset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->runset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->stageset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->queuedset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->waitset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->holdset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->arriveset, queue,
			LIM_PPNICE);
		if (retcode != TCML_COMPLETE) return (retcode);
		if (high_fix) {
			/*
			 *  The set operation was successful, but NQS
			 *  had to alter the numerical value of the
			 *  original nice limit to the maximum
			 *  numerical value supported on the local
			 *  host.
			 */
			return (TCML_FIXBYNQS);
		}
		return (TCML_COMPLETE);
	}
	return (TCML_WROQUETYP);	/* Wrong queue type */
#else
	return (TCML_NOSUCHQUO);	/* No such quota on this machine */
#endif
}

#ifdef SDSC
/*** upq_setncpuslim
 *
 *
 *	long upq_setncpuslim():
 *	Set the ncpu limit value for a batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:   if the specified ncpus value had to
 *				 be brought within the bounds supported
 *				 by the local host, but no grandfather
 *				 clauses had to be applied;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause;
 *		TCML_INTERNERR:	 if there was control file trouble;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist;
 *		TCML_NOSUCHQUO:	 if the named quota does not exist
 *				 on this machine;
 *		TCML_WROQUETYP:  if the named queue does not support
 *				 the setting of this limit.
 */
long upq_setncpuslim (quename, ncpus_value)
char *quename;				/* Queue name */
int ncpus_value;			/* [MIN_REQNICE..MAX_REQNICE] */
{
	long scalar_grandfather();	/* Check for grandfather clause */

	register struct queue *queue;
	short high_fix;			/* Boolean bounded nice to max */
	short low_fix;			/* Boolean bounded nice to min */
	register long retcode;		/* Return completion code */

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
#if	(VALID_LIMITS & LIM_PRNCPUS)
	if (queue->q.type == QUE_BATCH) {
		queue->q.v1.batch.explicit |= LIM_PRNCPUS;
		queue->q.v1.batch.infinite &= ~LIM_PRNCPUS;
		high_fix = 0;
		low_fix = 0;
		/*
		 *  If necessary, change the value entered by the
		 *  manager to something that can be enforced on
		 *  this machine.
		 */
		if (ncpus_value > MESH_H * MESH_W) {
			high_fix = 1;
			ncpus_value = MESH_H * MESH_W;
		}
		if (ncpus_value < MIN_NCPUS) {
			low_fix = 1;
			ncpus_value = MIN_NCPUS;
		}
		queue->q.v1.batch.prncpus = ncpus_value;
		udb_queue (queue);	/* Update NQS database image */
		if (low_fix) {
			/*
			 *  The set operation was successful, but NQS
			 *  had to bind the numerical value of the
			 *  original nice value to the minimum
			 *  numerical value supported on the local
			 *  host.  In this case, it is not necessary
			 *  to check for request grandfather clauses.
			 */
			return (TCML_FIXBYNQS);
		}
		retcode = scalar_grandfather (queue->departset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->runset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->stageset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->queuedset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->waitset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->holdset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		retcode = scalar_grandfather (queue->arriveset, queue,
			LIM_PRNCPUS);
		if (retcode != TCML_COMPLETE) return (retcode);
		if (high_fix) {
			/*
			 *  The set operation was successful, but NQS
			 *  had to alter the numerical value of the
			 *  original ncpus limit to the maximum
			 *  numerical value supported on the local
			 *  host.
			 */
			return (TCML_FIXBYNQS);
		}
		return (TCML_COMPLETE);
	}
	return (TCML_WROQUETYP);	/* Wrong queue type */
#else
	return (TCML_NOSUCHQUO);	/* No such quota on this machine */
#endif
}

/*** upq_setnodegrp
 *
 *
 *	long upq_setnodegrp():
 *	Set the node group value for a batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause;
 *		TCML_INTERNERR:	 if there was control file trouble;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist;
 *		TCML_WROQUETYP:  if the named queue does not support
 *				 the setting of this limit.
 */
long upq_setnodegrp (quename, nodegrp_value)
char *quename;				/* Queue name */
int nodegrp_value;			/* [MIN_NODEGRP	..MAX_NODEGRP] */
{
	long scalar_grandfather();	/* Check for grandfather clause */

	register struct queue *queue;
	register long retcode;		/* Return completion code */

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if (queue->q.type == QUE_BATCH) {
		queue->q.v1.batch.node_group = nodegrp_value;
		udb_queue (queue);	/* Update NQS database image */
		return (TCML_COMPLETE);
	}
	return (TCML_WROQUETYP);	/* Wrong queue type */
}
#endif


/*** upq_setnoqueacc
 *
 *
 *	long upq_setnoqueacc():
 *	Set queue access to no one (except root).
 *
 *	Returns:
#endif


/*** upq_setnoqueacc
 *
 *
 *	long upq_setnoqueacc():
 *	Set queue access to no one (except root).
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if the named queue did not exist.
 *		TCML_WROQUETYP:	if the queue was a network queue.
 */
long upq_setnoqueacc (quename)
char *quename;			/* Queue name */
{
	long no_access();	/* Set no access except root */

	register struct queue *queue;
	register long upd;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if ((upd = no_access (queue)) != TCML_COMPLETE) {
		return (upd);
	}
	udb_qaccess (queue);		/* Update the database image */
	return (TCML_COMPLETE);
}


/*** upq_setpipcli
 *
 *
 *	long upq_setpipcli():
 *	Set pipe client for a pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 *		TCML_WROQUETYP: if the queue is not a pipe queue.
 */
long upq_setpipcli (quename, clientname)
char *quename;			/* Queue name */
char *clientname;		/* New client for queue */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);		/* No such queue */
	}
	if (queue->q.type != QUE_PIPE) return (TCML_WROQUETYP);
	strcpy (queue->q.v1.pipe.server, clientname);	/* Copy new client */
	udb_queue (queue);			/* Update NQS database image */
	return (TCML_COMPLETE);
}


/*** upq_setquepri
 *
 *
 *	long upq_setquepri():
 *	Set queue priority for a batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 */
long upq_setquepri (quename, priority)
char *quename;			/* Queue name */
int priority;			/* Priority to be assigned to queue */
{
	void insquebypri();		/* Insert queue by priority */
	void remquefrompri();		/* Remove queue from priority */
					/* ordered set */
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	queue->q.priority = priority;	/* Assign new priority */
	if (queue->q.type != QUE_DEVICE) {
		/*
		 *  Batch, network, and pipe queues are also threaded
		 *  in a priority ordered queue set of the same type,
		 *  for use by the various scheduling modules.
		 *
		 *  Reinsert the queue into its appropriate location in
		 *  the proper priority ordered queue set.
		 */
		remquefrompri (queue);	/* Remove and re-insert into */
		insquebypri (queue);	/* priority ordered list */
	}
	udb_queue (queue);		/* Update NQS database image */
	return (TCML_COMPLETE);
}


/*** upq_setquerun
 *
 *
 *	long upq_setquerun():
 *	Set queue run-limit for a batch or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 *		TCML_WROQUETYP: if the named queue is a device queue.
 */
long upq_setquerun (quename, run_limit)
char *quename;			/* Queue name */
int run_limit;			/* Run-limit for queue */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if (queue->q.type == QUE_DEVICE) return (TCML_WROQUETYP);
	/*
	 *  Set the run limit on the queue, and spawn requests
	 *  as necessary.
	 */
	if (queue->q.type == QUE_BATCH) {
		queue->q.v1.batch.runlimit = run_limit;
	}
	else queue->q.v1.pipe.runlimit = run_limit;
	queue->q.status |= QUE_UPDATE;	/* Queue database image is now */
					/* out of date; the queue run-limit*/
					/* was changed by upq_setquerun(). */
	if (queue->q.queuedcount && run_limit > queue->q.runcount) {
		/*
		 *  Request(s) eligible to run in queue.
		 */
		switch (queue->q.type) {
		case QUE_BATCH:
			bsc_spawn();	/* Maybe spawn batch req(s) */
			break;
		case QUE_PIPE:
			psc_spawn();	/* Maybe spawn pipe req(s) */
			break;
		}
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  No requests were spawned from the started queue.
			 *  Therefore, the NQS database image of the queue
			 *  header is still out of date.
			 */
			udb_queue (queue);	/* Update queue header */
						/* clearing QUE_UPDATE bit */
		}
	}
	else udb_queue (queue);		/* Update queue header clearing */
					/* QUE_UPDATE bit */
	return (TCML_COMPLETE);
}

#ifdef SDSC
/*** upq_setquequeued
 *
 *
 *      long upq_setquequeued():
 *      Set queue run-limit for a batch or pipe queue.
 *
 *      Returns:
 *              TCML_COMPLETE:  if successful;
 *              TCML_NOSUCHQUE: if the named queue does not exist.
 *              TCML_WROQUETYP: if the named queue is a device queue.
 */
long upq_setquequeued (quename, queued_limit)
char *quename;                  /* Queue name */
int queued_limit;               /* Per-user queued and running request limit */
{
        register struct queue *queue;
 
        queue = nqs_fndnnq (quename);
        if (queue == (struct queue *)0) {
                return (TCML_NOSUCHQUE);        /* No such queue */
        }
        if (queue->q.type == QUE_DEVICE) return (TCML_WROQUETYP);
        /*
         *  Set the per-user request limit on the queue, and spawn requests
         *  as necessary.
         */
        if (queue->q.type == QUE_BATCH) {
                queue->q.v1.batch.sPerQueMaxQueuedULimit = queued_limit;
        }
        else return (TCML_WROQUETYP);   /* it is not a batch queue */
        queue->q.status |= QUE_UPDATE;  /* Queue database image is now */
                                        /* out of date; the per-user request limit */
                                        /* was changed by upq_setquequeued(). */
        udb_queue (queue);              /* Update queue header clearing */
                                        /* QUE_UPDATE bit */
        return (TCML_COMPLETE);
}
#endif
 
/*** upq_setquolim
 *
 *
 *	long upq_setquolim():
 *	Set one of the ten quota limits.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:   if the specified quota limit had to
 *				 be brought within the bounds supported
 *				 by the local host, but no grandfather
 *				 clauses had to be applied;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause;
 *		TCML_INTERNERR:	 if there was control file trouble;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist;
 *		TCML_NOSUCHQUO:	 if the named quota does not exist
 *				 on this machine;
 *		TCML_WROQUETYP:  if the named queue does not support
 *				 the setting of this limit.
 */
long upq_setquolim (quename, coeff, units, infinite, limit_type)
char *quename;			/* Queue name */
unsigned long coeff;		/* The coefficient of the limit */
short units;			/* The units, expressed as QLM_??? */
short infinite;			/* Non-zero if infinite */
long limit_type;		/* The limit, expressed as LIM_??? */
{
	long quota_grandfather();
				/* Check for grandfather clause on quota */

	register struct queue *queue;
	unsigned long new_coeff;/* Adjusted new coefficient */
	short new_units;	/* Adjusted new units */
	short fix;		/* Boolean: altered limit? */
	register long retcode;	/* Return completion code */

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if (VALID_LIMITS & limit_type) {
		if (queue->q.type == QUE_BATCH) {
			queue->q.v1.batch.explicit |= limit_type;
			fix = 0;
			/*
			 *  If necessary, change the limit value
			 *  to a value that can be enforced on this
			 *  machine.
			 */
			if (quolimhigh (coeff, units, infinite, limit_type,
					&new_coeff, &new_units) != 0) {
				infinite = 0;	/* Altered val never infinite */
				fix = 1;
				coeff = new_coeff;
				units = new_units;
			}
			if (infinite) queue->q.v1.batch.infinite |= limit_type;
			else {
				if (quolimlow (coeff, units, limit_type,
					&new_coeff, &new_units) != 0) {
					fix = 1;
					coeff = new_coeff;
					units = new_units;
				}
				queue->q.v1.batch.infinite &= ~limit_type;
				switch (limit_type) {
#if (VALID_LIMITS & LIM_PPCORE)
				case LIM_PPCORE:
					queue->q.v1.batch.ppcorecoeff = coeff;
					queue->q.v1.batch.ppcoreunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPDATA)
				case LIM_PPDATA:
					queue->q.v1.batch.ppdatacoeff = coeff;
					queue->q.v1.batch.ppdataunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPMEM)
				case LIM_PPMEM:
					queue->q.v1.batch.ppmemcoeff = coeff;
					queue->q.v1.batch.ppmemunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPPFILE)
				case LIM_PPPFILE:
					queue->q.v1.batch.pppfilecoeff = coeff;
					queue->q.v1.batch.pppfileunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPQFILE)
				case LIM_PPQFILE:
					queue->q.v1.batch.ppqfilecoeff = coeff;
					queue->q.v1.batch.ppqfileunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPSTACK)
				case LIM_PPSTACK:
					queue->q.v1.batch.ppstackcoeff = coeff;
					queue->q.v1.batch.ppstackunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPTFILE)
				case LIM_PPTFILE:
					queue->q.v1.batch.pptfilecoeff = coeff;
					queue->q.v1.batch.pptfileunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PPWORK)
				case LIM_PPWORK:
					queue->q.v1.batch.ppworkcoeff = coeff;
					queue->q.v1.batch.ppworkunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PRMEM)
				case LIM_PRMEM:
					queue->q.v1.batch.prmemcoeff = coeff;
					queue->q.v1.batch.prmemunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PRPFILE)
				case LIM_PRPFILE:
					queue->q.v1.batch.prpfilecoeff = coeff;
					queue->q.v1.batch.prpfileunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PRQFILE)
				case LIM_PRQFILE:
					queue->q.v1.batch.prqfilecoeff = coeff;
					queue->q.v1.batch.prqfileunits = units;
					break;
#endif
#if (VALID_LIMITS & LIM_PRTFILE)
				case LIM_PRTFILE:
					queue->q.v1.batch.prtfilecoeff = coeff;
					queue->q.v1.batch.prtfileunits = units;
					break;
#endif
				}
			}
			udb_queue (queue);	/* Update NQS database image */
			if (infinite) {
				/*
				 *  The set operation was successful, and
				 *  we do not have to check for requests
				 *  that will want more than the new limit.
				 */
				return (TCML_COMPLETE);
			}
			retcode = quota_grandfather (queue->departset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			retcode = quota_grandfather (queue->runset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			retcode = quota_grandfather (queue->stageset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			retcode = quota_grandfather (queue->queuedset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			retcode = quota_grandfather (queue->waitset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			retcode = quota_grandfather (queue->holdset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			retcode = quota_grandfather (queue->arriveset,
						     queue, limit_type);
			if (retcode != TCML_COMPLETE) return (retcode);
			if (fix) {
				/*
				 *  The set operation was successful, but NQS
				 *  had to alter the limit value slightly
				 *  because the original numerical limit
				 *  could not have been enforced on the
				 *  local host.
				 */
				return (TCML_FIXBYNQS);
			}
			return (TCML_COMPLETE);
		}
		return (TCML_WROQUETYP);	/* Wrong queue type */
	}
	return (TCML_NOSUCHQUO);	/* No such quota on this machine */
}


/*** upq_setunrqueacc
 *
 *
 *	long upq_setunrqueacc():
 *	Set queue access to unrestricted.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if the named queue did not exist.
 *		TCML_WROQUETYP:	if the queue was a network queue.
 */
long upq_setunrqueacc (quename)
char *quename;				/* Queue name */
{
	long unrestr_access();		/* Set unrestricted access */

	register struct queue *queue;
	register long upd;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);/* No such queue */
	}
	if ((upd = unrestr_access (queue)) != TCML_COMPLETE) {
		return (upd);
	}
	udb_qaccess (queue);		/* Update the database image */
	return (TCML_COMPLETE);
}


/*** upq_staque
 *
 *
 *	long upq_staque():
 *	Start an NQS batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 */
long upq_staque (quename)
char *quename;				/* Queue name */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	queue->q.status |= (QUE_RUNNING | QUE_UPDATE);
					/* Set running bit and record that */
					/* the queue database image is now */
					/* out of date. */
	if (queue->q.queuedcount) {
		/*
		 *  Request(s) eligible to run in queue.
		 */
		switch (queue->q.type) {
		case QUE_BATCH:
			bsc_spawn();	/* Maybe spawn batch req(s) */
			break;
		case QUE_DEVICE:
			dsc_spawn();	/* Maybe spawn device req(s) */
			break;
		case QUE_PIPE:
			psc_spawn();	/* Maybe spawn pipe req(s) */
			break;
		}
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  No requests were spawned from the started queue.
			 *  Therefore, the NQS database image of the queue
			 *  header is out of date.
			 */
			udb_queue (queue);	/* Update queue header */
						/* clearing QUE_UPDATE bit */
		}
	}
	else udb_queue (queue);		/* Update queue header clearing */
					/* QUE_UPDATE bit */
	return (TCML_COMPLETE);
}


/*** upq_stoque
 *
 *
 *	long upq_stoque():
 *	Stop an NQS batch, device, or pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue did not exist.
 */
long upq_stoque (quename)
char *quename;				/* Queue name */
{
	register struct queue *queue;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);		/* No such queue */
	}
	queue->q.status &= ~QUE_RUNNING;	/* Clear the running bit */
	udb_queue (queue);			/* Update NQS database image */
	return (TCML_COMPLETE);
}


/*** condense_access
 *
 *
 *	struct acclist *condense_access ():
 *
 *	Remove holes from the list of group and users that have access
 *	to a queue, freeing memory where possible.
 */
static struct acclist *condense_access (checklistp)
struct acclist *checklistp;
{
	void free_access();
	struct acclist *keeplistp;		/* One link in kept list */
	struct acclist *keptlistp;		/* Beginning of kept list */
	struct acclist *lastkeptp;		/* Last struct acclist kept */
	int keepindex;				/* Index of next free slot */
	int i;

	keeplistp = keptlistp = checklistp;
	keepindex = 0;
	lastkeptp = (struct acclist *) 0;
	while (checklistp != (struct acclist *) 0) {
		for (i = 0; i < ACCLIST_SIZE; i++) {
			if (checklistp->entries [i] != 0) {
				keeplistp->entries [keepindex] =
					checklistp->entries [i];
				if (++keepindex >= ACCLIST_SIZE) {
					keepindex = 0;
					lastkeptp = keeplistp;
					keeplistp = keeplistp->next;
				}
			}
		}
		checklistp = checklistp->next;
	}
	/*
	 * Done condensing.  Now zero out the end of the last acclist
	 * still in use, free any unnecessary acclists, and zero
	 * the pointers to any now-freed space.
	 */
	if (keepindex == 0) {		/* The last kept is not *keeplistp */
		free_access (keeplistp);
		if (lastkeptp == (struct acclist *) 0) {
			return ((struct acclist *) 0);
		}
		else {
			lastkeptp->next = (struct acclist *) 0;
			return (keptlistp);
		}
	}
	else {				/* The last kept is *keeplistp */
		for (i = keepindex; i < ACCLIST_SIZE; i++) {
			keeplistp->entries [i] = 0;
		}
		free_access (keeplistp->next);
		keeplistp->next = (struct acclist *) 0;
		return (keptlistp);
	}
}


#if	(VALID_LIMITS & (LIM_PPCPUT | LIM_PRCPUT))
/*** cpu_grandfather
 *
 *
 *	long cpu_grandfather ():
 *
 *	We are setting a CPU time limit for a batch queue.  Examine
 *	a non-running request chain to determine if any request
 *	requires a grandfather clause.
 *
 *	Return the value that upq_setcpulim() should return (with the
 *	exception that cpu_grandfather() returning TCML_COMPLETE means
 *	continue looking).
 */
static long cpu_grandfather (req, queue, limit_type)
struct request *req;			/* The list of requests */
struct queue *queue;			/* The queue they are in */
long limit_type;			/* The limit being set */
{
	register int file_descr;	/* Control file file-descriptor */
	struct rawreq rawreq;
	
	while (req != (struct request *) 0) {
		file_descr = getrreq (req->v1.req.orig_seqno,
				      req->v1.req.orig_mid, &rawreq);
		if (file_descr == -1) {
			/*
			 * Nqs_enfile() reports to the log and returns
			 * 1 if it was EMFILE or ENFILE.
			 */
			if (nqs_enfile ()) if (errno == EMFILE) nqs_abort();
			return (TCML_INTERNERR);
		}
		close (file_descr);	/* Close the request file */
		switch (limit_type) {
#if	(VALID_LIMITS & LIM_PPCPUT)
		case LIM_PPCPUT:
			if (rawreq.v.bat.infinite & LIM_PPCPUT) {
				return (TCML_GRANFATHER);
			}
			if ((queue->q.v1.batch.ppcpusecs <
				rawreq.v.bat.ppcputime.max_seconds) ||
			    ((queue->q.v1.batch.ppcpusecs ==
				rawreq.v.bat.ppcputime.max_seconds) &&
			    (queue->q.v1.batch.ppcpums <
				rawreq.v.bat.ppcputime.max_ms))) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PRCPUT)
		case LIM_PRCPUT:
			if (rawreq.v.bat.infinite & LIM_PRCPUT) {
				return (TCML_GRANFATHER);
			}
			if ((queue->q.v1.batch.prcpusecs <
				rawreq.v.bat.prcputime.max_seconds) ||
			    ((queue->q.v1.batch.prcpusecs ==
				rawreq.v.bat.prcputime.max_seconds) &&
			    (queue->q.v1.batch.prcpums <
				rawreq.v.bat.prcputime.max_ms))) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
		}
		req = req->next;
	}
	return (TCML_COMPLETE);
}
#endif


/*** del_access
 *
 *
 *	long del_access();
 *	Delete a group or user from the list of users for a queue.
 *
 *	Returns: TCML_NOACCNOW, TCML_COMPLETE, TCML_ROOTINDEL,
 *		 TCML_UNRESTR, or TCML_WROQUETYP.
 */
static long del_access (queue, who)
struct queue *queue;				/* Queue descriptor */
unsigned long who;				/* Gid or uid */
{
	struct acclist *acclistp;
	int i;
	
	if (queue->q.type == QUE_NET) return (TCML_WROQUETYP);
	if ((queue->q.status & QUE_BYGIDUID) == 0) return (TCML_UNRESTR);
	/*
	 * Root always has access, even if not contained in
	 * the access list.  Root's access may not be deleted.
	 */
	if (who == 0) return (TCML_ROOTINDEL);
	switch (queue->q.type) {
	case QUE_BATCH:
		acclistp = queue->v1.batch.acclistp;
		break;
	case QUE_DEVICE:
		acclistp = queue->v1.device.acclistp;
		break;
	case QUE_PIPE:
		acclistp = queue->v1.pipe.acclistp;
		break;
	}
	while (acclistp != (struct acclist *) 0) {
		for (i = 0; i < ACCLIST_SIZE; i++) {
			if (acclistp->entries [i] == who) {
				acclistp->entries [i] = 0;
				switch (queue->q.type) {
				case QUE_BATCH:
					queue->v1.batch.acclistp =
						condense_access
						(queue->v1.batch.acclistp);
					return (TCML_COMPLETE);
				case QUE_DEVICE:
					queue->v1.device.acclistp =
						condense_access
						(queue->v1.device.acclistp);
					return (TCML_COMPLETE);
				case QUE_PIPE:
					queue->v1.pipe.acclistp =
						condense_access
						(queue->v1.pipe.acclistp);
					return (TCML_COMPLETE);
				}
			}
		}
		acclistp = acclistp->next;
	}
	return (TCML_NOACCNOW);
}


/*** free_access
 *
 *
 *	void free_access();
 *	Free a queue access list.
 *
 *	Returns: void
 */
static void free_access (acclistp)
struct acclist *acclistp;			/* The list to be freed */
{
	struct acclist *temp;
	
	while (acclistp != (struct acclist *) 0) {
		temp = acclistp->next;
		free ((char *) acclistp);
		acclistp = temp;
	}
}


/*** insquebypri
 *
 *
 *	void insquebypri():
 *
 *	Insert the specified batch, network, or pipe queue (not device
 *	queue!) into the appropriate priority ordered set.
 */
static void insquebypri (queue)
struct queue *queue;
{
	register struct queue *walkqueue;
	register struct queue *prevqueue;
	register short priority;

	/*
	 *  Insert by priority into the proper queue set.
	 */
	priority = queue->q.priority;
	prevqueue = (struct queue *) 0;
	switch (queue->q.type) {
	case QUE_BATCH:
		walkqueue = Pribatqueset;
		while (walkqueue != (struct queue *) 0 &&
		       walkqueue->q.priority >= priority) {
			prevqueue = walkqueue;
			walkqueue = walkqueue->v1.batch.nextpriority;
		}
		if (prevqueue == (struct queue *) 0) {
			/*
			 *  This queue is going to be at the very head of
			 *  its priority ordered queue set.
			 */
			queue->v1.batch.nextpriority = Pribatqueset;
			Pribatqueset = queue;
		}
		else {
			/*
			 *  This queue is not being inserted at the
			 *  head of its priority ordered queue set.
			 */
			queue->v1.batch.nextpriority
				= prevqueue->v1.batch.nextpriority;
			prevqueue->v1.batch.nextpriority = queue;
		}
		break;
	case QUE_NET:
		walkqueue = Prinetqueset;
		while (walkqueue != (struct queue *) 0 &&
		       walkqueue->q.priority >= priority) {
			prevqueue = walkqueue;
			walkqueue = walkqueue->v1.network.nextpriority;
		}
		if (prevqueue == (struct queue *) 0) {
			/*
			 *  This queue is going to be at the very head of
			 *  its priority ordered queue set.
			 */
			queue->v1.network.nextpriority = Prinetqueset;
			Prinetqueset = queue;
		}
		else {
			/*
			 *  This queue is not being inserted at the
			 *  head of its priority ordered queue set.
			 */
			queue->v1.network.nextpriority
				= prevqueue->v1.network.nextpriority;
			prevqueue->v1.network.nextpriority = queue;
		}
		break;
	case QUE_PIPE:
		walkqueue = Pripipqueset;
		while (walkqueue != (struct queue *) 0 &&
		       walkqueue->q.priority >= priority) {
			prevqueue = walkqueue;
			walkqueue = walkqueue->v1.pipe.nextpriority;
		}
		if (prevqueue == (struct queue *) 0) {
			/*
			 *  This queue is going to be at the very head of
			 *  its priority ordered queue set.
			 */
			queue->v1.pipe.nextpriority = Pripipqueset;
			Pripipqueset = queue;
		}
		else {
			/*
			 *  This queue is not being inserted at the
			 *  head of its priority ordered queue set.
			 */
			queue->v1.pipe.nextpriority
				= prevqueue->v1.pipe.nextpriority;
			prevqueue->v1.pipe.nextpriority = queue;
		}
		break;
	}
}


#if	(VALID_LIMITS & LIM_PPNICE)
/*** scalar_grandfather
 *
 *
 *	long scalar_grandfather ():
 *
 *	We are setting a nice limit value for a queue.  Scan a
 *	request chain containing non-running requests to determine
 *	if any request grandfather clauses must be applied.
 *
 *	Return the value that upq_setniclim() should return (with
 *	the exception that scalar_grandfather() returning TCML_COMPLETE
 *	means continue looking).
 */
static long scalar_grandfather (req, queue, limit_type)
struct request *req;			/* The list of requests */
struct queue *queue;			/* The queue they are in */
long limit_type;			/* LIM_??? */
{
	register int file_descr;	/* Control file file-descriptor */
	struct rawreq rawreq;
	
	while (req != (struct request *) 0) {
		file_descr = getrreq (req->v1.req.orig_seqno,
				      req->v1.req.orig_mid, &rawreq);
		if (file_descr == -1) {
			/*
			 * Nqs_enfile() reports to the log and returns
			 * 1 if it was EMFILE or ENFILE.
			 */
			if (nqs_enfile ()) if (errno == EMFILE) nqs_abort();
			return (TCML_INTERNERR);
		}
		close (file_descr);	/* Close the request file */
		switch (limit_type) {
		case LIM_PPNICE:
			/*
			 *  Remember that SMALLER (and -10 is smaller than -1)
			 *  nice values indicate MORE priority....
			 */
			if ((queue->q.v1.batch.ppnice > rawreq.v.bat.ppnice)) {
				return (TCML_GRANFATHER);
			}
		case LIM_PRDRIVES:
		case LIM_PRNCPUS:
			/*
			 * MOREHERE
			 */
			break;
		}
		req = req->next;
	}
	return (TCML_COMPLETE);
}
#endif


/*** no_access
 *
 *
 *	long no_access();
 *	Set queue access to deny access to all but root.
 *
 *	Returns: TCML_COMPLETE or TCML_WROQUETYP.
 */
static long no_access (queue)
struct queue *queue;				/* Queue descriptor */
{
	if (queue->q.type == QUE_NET) return (TCML_WROQUETYP);
	queue->q.status |= QUE_BYGIDUID;
	switch (queue->q.type) {
	case QUE_BATCH:
		free_access (queue->v1.batch.acclistp);
		queue->v1.batch.acclistp = (struct acclist *) 0;
	case QUE_DEVICE:
		free_access (queue->v1.device.acclistp);
		queue->v1.device.acclistp = (struct acclist *) 0;
	case QUE_PIPE:
		free_access (queue->v1.pipe.acclistp);
		queue->v1.pipe.acclistp = (struct acclist *) 0;
	}
	return (TCML_COMPLETE);
}


/*** quota_grandfather
 *
 *
 *	long quota_grandfather():
 *
 *	A memory or file quota limit is being set for a queue.
 *	Examine a chain of non-running requests to see if any
 *	grandfather clauses need to be applied.
 *
 *	Return the value that upq_setquolim() should return
 *	(with the exception that quota_grandfather() returning
 *	TCML_COMPLETE means continue looking).
 */
static long quota_grandfather (req, queue, limit_type)
register struct request *req;		/* The list of requests */
struct queue *queue;			/* The queue they are in */
long limit_type;			/* The limit being set */
{
	register int file_descr;	/* Control file file-descriptor */
	struct rawreq rawreq;
	
	while (req != (struct request *) 0) {
		file_descr = getrreq (req->v1.req.orig_seqno,
				      req->v1.req.orig_mid, &rawreq);
		if (file_descr == -1) {
			/*
			 * Nqs_enfile() reports to the log and returns
			 * 1 if it was EMFILE or ENFILE.
			 */
			if (nqs_enfile ()) if (errno == EMFILE) nqs_abort();
			return (TCML_INTERNERR);
		}
		close (file_descr);	/* Close the request file */
		switch (limit_type) {
#if	(VALID_LIMITS & LIM_PPCORE)
		case LIM_PPCORE:
			if (rawreq.v.bat.infinite & LIM_PPCORE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.ppcorecoeff,
				      queue->q.v1.batch.ppcoreunits,
				      rawreq.v.bat.ppcoresize.max_quota,
				      rawreq.v.bat.ppcoresize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPDATA)
		case LIM_PPDATA:
			if (rawreq.v.bat.infinite & LIM_PPDATA) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.ppdatacoeff,
				      queue->q.v1.batch.ppdataunits,
				      rawreq.v.bat.ppdatasize.max_quota,
				      rawreq.v.bat.ppdatasize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPMEM)
		case LIM_PPMEM:
			if (rawreq.v.bat.infinite & LIM_PPMEM) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.ppmemcoeff,
				      queue->q.v1.batch.ppmemunits,
				      rawreq.v.bat.ppmemsize.max_quota,
				      rawreq.v.bat.ppmemsize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPPFILE)
		case LIM_PPPFILE:
			if (rawreq.v.bat.infinite & LIM_PPPFILE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.pppfilecoeff,
				      queue->q.v1.batch.pppfileunits,
				      rawreq.v.bat.pppfilesize.max_quota,
				      rawreq.v.bat.pppfilesize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPQFILE)
		case LIM_PPQFILE:
			if (rawreq.v.bat.infinite & LIM_PPQFILE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.ppqfilecoeff,
				      queue->q.v1.batch.ppqfileunits,
				      rawreq.v.bat.ppqfilesize.max_quota,
				      rawreq.v.bat.ppqfilesize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPSTACK)
		case LIM_PPSTACK:
			if (rawreq.v.bat.infinite & LIM_PPSTACK) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.ppstackcoeff,
				      queue->q.v1.batch.ppstackunits,
				      rawreq.v.bat.ppstacksize.max_quota,
				      rawreq.v.bat.ppstacksize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPTFILE)
		case LIM_PPTFILE:
			if (rawreq.v.bat.infinite & LIM_PPTFILE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.pptfilecoeff,
				      queue->q.v1.batch.pptfileunits,
				      rawreq.v.bat.pptfilesize.max_quota,
				      rawreq.v.bat.pptfilesize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PPWORK)
		case LIM_PPWORK:
			if (rawreq.v.bat.infinite & LIM_PPWORK) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.ppworkcoeff,
				      queue->q.v1.batch.ppworkunits,
				      rawreq.v.bat.ppworkset.max_quota,
				      rawreq.v.bat.ppworkset.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PRMEM)
		case LIM_PRMEM:
			if (rawreq.v.bat.infinite & LIM_PRMEM) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.prmemcoeff,
				      queue->q.v1.batch.prmemunits,
				      rawreq.v.bat.prmemsize.max_quota,
				      rawreq.v.bat.prmemsize.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PRPFILE)
		case LIM_PRPFILE:
			if (rawreq.v.bat.infinite & LIM_PRPFILE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.prpfilecoeff,
				      queue->q.v1.batch.prpfileunits,
				      rawreq.v.bat.prpfilespace.max_quota,
				      rawreq.v.bat.prpfilespace.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PRQFILE)
		case LIM_PRQFILE:
			if (rawreq.v.bat.infinite & LIM_PRQFILE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.prqfilecoeff,
				      queue->q.v1.batch.prqfileunits,
				      rawreq.v.bat.prqfilespace.max_quota,
				      rawreq.v.bat.prqfilespace.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
#if	(VALID_LIMITS & LIM_PRTFILE)
		case LIM_PRTFILE:
			if (rawreq.v.bat.infinite & LIM_PRTFILE) {
				return (TCML_GRANFATHER);
			}
			if (secgrfir (queue->q.v1.batch.prtfilecoeff,
				      queue->q.v1.batch.prtfileunits,
				      rawreq.v.bat.prtfilespace.max_quota,
				      rawreq.v.bat.prtfilespace.max_units)) {
				return (TCML_GRANFATHER);
			}
			break;
#endif
		}
		req = req->next;
	}
	return (TCML_COMPLETE);
}


/*** remquefrompri
 *
 *
 *	void remquefrompri():
 *
 *	Remove the specified queue from the appropriate priority ordered
 *	set.  All queues are always placed in such sets for the benefit
 *	of their respective scheduling modules.
 */
static void remquefrompri (queue)
register struct queue *queue;
{
	register struct queue *walkqueue;
	register struct queue *prevqueue;

	/*
	 *  Remove the queue from the appropriate priority ordered set.
	 */
	prevqueue = (struct queue *) 0;
	switch (queue->q.type) {
	case QUE_BATCH:
		walkqueue = Pribatqueset;
		while (walkqueue != queue) {
			prevqueue = walkqueue;
			walkqueue = walkqueue->v1.batch.nextpriority;
		}
		if (prevqueue == (struct queue *) 0) {
			/*
			 *  This queue must be removed from the very head
			 *  of its priority ordered queue set.
			 */
			Pribatqueset = queue->v1.batch.nextpriority;
		}
		else {
			/*
			 *  This queue is not being removed from the
			 *  head of its priority ordered queue set.
			 */
			prevqueue->v1.batch.nextpriority
				= queue->v1.batch.nextpriority;
		}
		break;
	case QUE_NET:
		walkqueue = Prinetqueset;
		while (walkqueue != queue) {
			prevqueue = walkqueue;
			walkqueue = walkqueue->v1.network.nextpriority;
		}
		if (prevqueue == (struct queue *) 0) {
			/*
			 *  This queue must be removed from the very head
			 *  of its priority ordered queue set.
			 */
			Prinetqueset = queue->v1.network.nextpriority;
		}
		else {
			/*
			 *  This queue is not being removed from the
			 *  head of its priority ordered queue set.
			 */
			prevqueue->v1.network.nextpriority
				= queue->v1.network.nextpriority;
		}
		break;
	case QUE_PIPE:
		walkqueue = Pripipqueset;
		while (walkqueue != queue) {
			prevqueue = walkqueue;
			walkqueue = walkqueue->v1.pipe.nextpriority;
		}
		if (prevqueue == (struct queue *) 0) {
			/*
			 *  This queue must be removed from the very head
			 *  of its priority ordered queue set.
			 */
			Pripipqueset = queue->v1.pipe.nextpriority;
		}
		else {
			/*
			 *  This queue is not being removed from the
			 *  head of its priority ordered queue set.
			 */
			prevqueue->v1.pipe.nextpriority
				= queue->v1.pipe.nextpriority;
		}
		break;
	}
}


/*** unrestr_access
 *
 *
 *	long unrestr_access ();
 *	Set queue access to allow access to all.
 *
 *	Returns: TCML_COMPLETE or TCML_WROQUETYP.
 */
static long unrestr_access (queue)
struct queue *queue;				/* Queue descriptor */
{
	if (queue->q.type == QUE_NET) return (TCML_WROQUETYP);
	queue->q.status &= ~QUE_BYGIDUID;
	switch (queue->q.type) {
	case QUE_BATCH:
		free_access (queue->v1.batch.acclistp);
		queue->v1.batch.acclistp = (struct acclist *) 0;
	case QUE_DEVICE:
		free_access (queue->v1.device.acclistp);
		queue->v1.device.acclistp = (struct acclist *) 0;
	case QUE_PIPE:
		free_access (queue->v1.pipe.acclistp);
		queue->v1.pipe.acclistp = (struct acclist *) 0;
	}
	return (TCML_COMPLETE);
}



remquefromcomp(queue,cmplx_tbl)
struct queue *queue;
struct qcomplex *cmplx_tbl[MAX_COMPLXSPERQ];
{
	register struct qcomplex *qcomplex;
	int	complexno, queno;

	/* Remove the batch queue from any complexes it may be 
	 * a member of.
	 */
	complexno = 0;

	while (complexno < MAX_COMPLXSPERQ &&
		(qcomplex = cmplx_tbl[complexno]) 
			!= (struct qcomplex *) 0) {
		/*
		 *	Find the queue in the complex
		 */
		queno =0;
		while (queno < MAX_QSPERCOMPLX && qcomplex->queue[queno]
			!= queue)
			queno++;
		if (queno == MAX_QSPERCOMPLX )
                       return(TCML_NOSUCHCOM);
                       /*
                        *  Delete the queue from the complex by moving
                        *  everyone else up in the linked list array.
                        */
                       while (queno < MAX_QSPERCOMPLX -1 &&
                           qcomplex->queue[queno] != (struct queue *)0) {
                           qcomplex->queue[queno] = qcomplex->queue[queno+1];
                           queno++;
                       }
                       if (queno == MAX_QSPERCOMPLX - 1)
                               qcomplex->queue[queno] = (struct queue *)0;
                       udb_quecom(qcomplex);
			complexno++;
		}
}
