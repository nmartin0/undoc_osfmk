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
 
/*++ nqs_nsq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_nsq.c,v $
 *
 * DESCRIPTION:
 *
 *	Insert queue operations module.
 *
 *	This module contains three functions:
 *
 *		nsq_enque(),
 *		nsq_spawn(), and
 *		nsq_unque().
 *
 *	Nqs_insque() enters the specified request into the specified
 *	queue.  If successful, then a pointer to the new request is
 *	recorded in a variable local to this module.
 *
 *	The function:  nsq_spawn()  invokes the proper spawner:
 *
 *		bsc_spawn(),
 *		dsc_spawn(), or
 *		psc_spawn()
 *
 *	for the last most recently queued request.
 *
 *	Nqs_unque() undoes the most recent queue operation.  This
 *	must be used ONLY by nqs_quereq().  Note that nsq_unque()
 *	can only "unqueue" the last most recently queued request.
 *	It CANNOT be called n times (n > 1) to successively undo
 *	the last n queued requests.
 *
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
 * REVISION HISTORY: ($Revision: 1.8 $ $Date: 1995/04/01 00:38:08 $ $State: Exp $)
 * $Log: nqs_nsq.c,v $
 * Revision 1.8  1995/04/01  00:38:08  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 12062
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 *
 * Revision 1.7  1995/02/24  23:43:03  kremenek
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
 * Revision 1.6  1994/11/19  02:53:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/11/02  00:57:26  mwan
 * R1.2 mods
 *
 * Revision 1.4  1993/07/13  17:51:31  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.3  1992/12/16  23:26:20  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:25:52  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:19  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:08  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <pwd.h>			/* Struct passwd */
#include "nqs.h"			/* NQS constants and data types */
#include "informcc.h"			/* NQS information completion */
					/* codes and masks */
#include "transactcc.h"			/* NQS transaction completion codes */
#include "nqsxvars.h"			/* NQS external variables */
#include <sys/stat.h>
#ifdef SDSC
#include <grp.h>
#include <pwd.h>
#include "buddyxvar.h"
#endif

extern void a2s_a2aset();		/* Add to arriving set */
extern void a2s_a2hset();		/* Add to hold set in queue */
extern void a2s_a2qset();		/* Add to queued set in queue */
extern void a2s_a2wset();		/* Add to wait set in queue */
extern char *asciierrno();		/* Return ASCII errno */
extern int bsc_sched();			/* Schedule a batch request */
extern void bsc_spawn();		/* MAYBE spawn a batch request */
extern int cpulimhigh();		/* Is time limit too high? */
extern int cpulimlow();			/* Is time limit too low? */
extern int dsc_sched();			/* Schedule a device request */
extern void dsc_spawn();		/* MAYBE spawn a device request */
extern struct passwd *fetchpwuid();	/* Fetch struct passwd given uid */
extern void free();			/* Free heap space */
extern char *malloc();			/* Allocate dynamic memory */
extern struct gendescr *nextdb();	/* Next allocated descriptor */
extern void nqs_deque();		/* Dequeue a request */
extern void nqs_disreq();		/* Dispose of request */
extern struct queue *nqs_fndnnq();	/* Find non-network queue */
extern struct request *nqs_fndreq();	/* Find request */
extern void nqs_vtimer();		/* Virtual timer */
extern void pip_timeout();		/* Abort a remote pipe-queue */
					/* transaction on timeout */
extern int psc_sched();			/* Schedule a pipe request */
extern void psc_spawn();		/* MAYBE spawn a pipe queue request */
extern int quolimhigh();		/* Is quota limit too high? */
extern int quolimlow();			/* Is quota limit too low? */
extern void seekdbb();			/* Buffered seek on database file */
extern int strcmp();			/* String comparison */
extern int tid_allocate();		/* Allocate a transaction-id */
extern void tid_deallocate();		/* Deallocate a transaction-id */
extern time_t time();			/* Get GMT time since 0:00:00 1/1/70 */
extern int tra_allocate();		/* Allocate a transaction descr */
extern int tra_release();		/* Release a transaction descriptor */
extern void udb_qorder();		/* Update NQS database image of */
					/* a queue */

static struct request *lastrequest;	/* Ptr to most recently queued req */


/*** nsq_enque
 *
 *
 *	long nsq_enque():
 *	Insert a request into an NQS queue.
 *
 *	When invoked, the argument:  cfd  is the open control/request file
 *	file-descriptor which has been opened for reading/writing.  The
 *	current file position of the control/request file is at the byte
 *	immediately following the request header which has been read into
 *	the argument:  rawreq.
 *
 *	This function must NOT close the control/request file.  Any
 *	changes to rawreq MUST be written to disk after nsq_enque()
 *	returns, by the caller.
 *
 *	Returns: A transaction completion code in the bits indicated by
 *	XCI_FULREA_MASK AND an optional information completion code
 *	in the bits indicated by XCI_INFORM_MASK.
 */
long nsq_enque (mode, cfd, rawreq, wasexe, frommid, prearrive)
int mode;				/* Queueing mode: 0=new; 1=local */
					/* pipe queue; 2=remote pipe queue */
int cfd;				/* Control file file-descr */
struct rawreq *rawreq;			/* Rawreq structure for request */
int wasexe;				/* Boolean TRUE if the request */
					/* is being requeued via nqs_rbuild()*/
					/* and is going to be restarted */
mid_t frommid;				/* If mode=2, then this parameter */
					/* contains the machine-id of the */
					/* peer machine */
time_t prearrive;			/* Contains transaction time if */
					/* the request is to be queued in */
					/* the pre-arrive state AND */
{					/* mode = 2 */
	long verify_resources();	/* Verify request resources */

	struct transact transact;	/* Transaction descriptor */
	struct stat statbuf;		/* fstat() buffer */
	register struct queue *queue;	/* Queue in which req is placed */
	register struct request *req;	/* Request struct allocated for req */
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	int state;			/* Request state */
	int tid;			/* Transaction-id */
	long res;			/* Result of verify_resources */
	struct passwd *passwd;		/* For gid, uid */
#ifdef SDSC
	int period_inx;
#endif

	/*
	 *  Find out the owner of the file....which is the mapped
	 *  request owner user-id.
	 */
	fstat (cfd, &statbuf);
	passwd = fetchpwuid ((int) statbuf.st_uid);
	if (passwd == (struct passwd *) 0) {
		return (TCML_NOACCAUTH);	/* No such account */
	}
	/*
	 *  We have located the queue in which to place the request.
	 *  Verify access.
	 */
	queue = nqs_fndnnq (rawreq->quename);	/* Find the target queue */
	if (queue == (struct queue *) 0) {
		return (TCML_NOSUCHQUE);	/* No such queue */
	}
	if (Booted) {
		/*
		 *  The request being queued is not being requeued as
		 *  part of an NQS rebuild operation.
		 */
		if ((queue->q.status & QUE_PIPEONLY) && mode == 0) {
			/*
			 *  This queue can only receive requests submitted
			 *  from other pipe queues.
			 */
			return (TCML_ACCESSDEN);	/* Access denied */
		}
		/*
		 *  If the queue is disabled, then we cannot queue the
		 *  request.
		 */
		if (!(queue->q.status & QUE_ENABLED)) {
			/*
			 *  This queue is disabled, and we are not rebuilding
			 *  the queue states.
			 */
			return (TCML_QUEDISABL);	/* Queue is disabled */
		}
		if (queue->q.type != QUE_NET) {
			if (verify_access (passwd->pw_gid, passwd->pw_uid,
					   queue) == 0) {
				return (TCML_ACCESSDEN);
			}
		}
	}
	/*
	 *  Verify request quota limits and queue/request type agreement.
	 *  Among other things, the request type must agree with the
	 *  queue type if the queue is a batch or device queue.
	 */
	res = TCML_SUBMITTED;			/* Default result code */
	if (queue->q.type != QUE_PIPE) {
		/*
		 *  Verify request resources and type.
		 */
		if (rawreq->type == RTYPE_BATCH) {
			if (queue->q.type != QUE_BATCH) {
				/*
				 *  Wrong queue type.
				 */
				return (TCML_WROQUETYP);
			}
			/*
			 *  A batch request must have EXACTLY one data
			 *  file (the script file).  Notice that nqs_spawn
			 *  uses this fact if Shell_strategy = SHSTRAT_FREE.
			 */
			if (rawreq->ndatafiles != 1) {
				return (TCML_BADCDTFIL);
			}
		}
		else if (rawreq->type == RTYPE_DEVICE) {
			if (queue->q.type != QUE_DEVICE) {
				/*
				 *  Wrong queue type.
				 */
				return (TCML_WROQUETYP);
			}
		}
		if (Booted) {
			/*
			 *  The request is not being merely requeued
			 *  as part of an NQS rebuild operation.  We
			 *  must verify the resource requirements of
			 *  all of the queued requests.
			 */
			res = verify_resources (queue, rawreq);
			if ((res & XCI_FULREA_MASK) != TCML_SUBMITTED) {
				return (res);	/* Return failure result */
			}
#ifdef SDSC

			/* Check for tennis court scheduling. The return code
		 	 * TCML_INSQUESPA is not defined correctly due to
                	 * difficulty in adding new return codes.
			 */

			if (queue->q.type == QUE_BATCH) {
		    	    if (rawreq->start_time > 0 && 
		    	    queue->q.priority >= TSCHED_PRI) {
		    		period_inx = chk_tsched (rawreq->start_time, 
        	    		rawreq->v.bat.prcputime.max_seconds,
        	    		rawreq->v.bat.prncpus, &rawreq->major_nodes,
				queue->q.v1.batch.node_group);
		    		if (period_inx >= 0 && period_inx < 
				MAX_PERIOD + 1) {
			    	    rawreq->period_inx = period_inx;
		    		} else {
			    	    return (TCML_INSQUESPA);
		    		}
		    	    }
			}
#endif
		}
	}
	/*
	 *  At this point, res holds the tentative return code.
	 */
	if (mode != 1) {
		/*
		 *  We are either queueing a brand new request that
		 *  is originating on the local machine, or we are
		 *  performing a remote enqueueing operation for a
		 *  remote client.
		 *
		 *  Allocate the request structure for the request.
		 */
		req = (struct request *) malloc (sizeof (struct request));
		if (req == (struct request *) 0) {
			/*
			 *  Insufficient memory to queue request.
			 */
			return (TCML_INSQUESPA);
		}
		/*
		 *  If no transaction descriptor is allocated to this
		 *  request, then allocate and initialize a transaction
		 *  descriptor for the new request.
		 */
		if (rawreq->trans_id == 0) {
			/*
			 *  This request is being queued for the very first
			 *  time at this machine.  A transaction-id and
			 *  descriptor must be allocated to it.
			 */
			tid = tid_allocate (0, rawreq->flags & RQF_EXTERNAL);
			if (tid == 0) {
				/*
				 *  No transaction-id/descriptor exists that
				 *  can be allocated to this request.
				 */
				free ((char *) req);	/* Release request */
				return (TCML_INSQUESPA);
			}
			/*
			 *  A transaction-id has been allocated for the
			 *  request.  Now go initialize the corresponding
			 *  transaction descriptor.
			 *
			 *  (Note that it is not necessary to set the update
			 *   time.  That field will be set by tra_allocate().)
			 */
			transact.tra_type = TRA_REQSTATE;
			transact.orig_seqno = rawreq->orig_seqno;
			transact.orig_mid = rawreq->orig_mid;
			if (mode == 0) {		/* New request */
				transact.state = RTS_STASIS;
				transact.v.events31 = 0;
			}
			else {			/* Mode = 2; remote request */
				transact.state = RTS_PREARRIVE;
				transact.v.peer_mid = frommid;
			}
			if (tra_allocate (tid, &transact,
					 (uid_t) passwd->pw_uid,
					 (gid_t) passwd->pw_gid) == -1) {
				/*
			 	 *  Something went wrong.  We were unsuccessful
				 *  in our attempt to initialize the allocated
				 *  transaction descriptor.
				 */
				printf ("E$Tra_allocate() error in ");
				printf ("nsq_enque().\n");
				printf ("I$%s.\n", asciierrno());
				printf ("I$Transaction-id was: %1d.\n", tid);
				tid_deallocate (tid);
				if (tra_release (tid) == -1) {
					/*
					 *  Things are really falling apart!
					 */
					printf ("E$Tra_release() error in ");
					printf ("nsq_enque().\n");
					printf ("I$%s.\n", asciierrno());
					printf ("I$Transaction-id was: %1d.\n",
						tid);
				}
				fflush (stdout);
				free ((char *) req);	/* Release request */
				return (TCML_UNAFAILURE);
			}			/* Submit operation failed */
			rawreq->trans_id = tid;	/* Update rawreq */
			prearrive = transact.update;
		}				/* Pre-arrive event time */
		if (rawreq->flags & RQF_EXTERNAL) {
			/*
			 *  This request is being queued from an external
			 *  source.  Note that the tid_allocate() call above
			 *  would NOT succeed if:
			 *
			 *	Extreqcount
			 *
			 *   were >= Maxextrequests.
			 */
			Extreqcount++;	/* One more externally queued */
		}			/* request */
	}
	else {
		/*
		 *  This request is being queued via a local pipe queue,
		 *  and already has a request structure and transaction-id
		 *  assigned to it.
		 *
		 *  Locate the local request, and remove it from its present
		 *  position (in the routing state), from its parent pipe
		 *  queue, and place it in the arriving set into the target
		 *  local destination queue.
		 */
		state = RQS_RUNNING;	/* Search only the routing state */
		if ((req = nqs_fndreq (rawreq->orig_seqno, rawreq->orig_mid,
				       &predecessor,
				       &state)) == (struct request *) 0) {
			/*
			 *  The request was not found to be routing in any
			 *  of the local pipe queues.
			 */
			return (TCML_NOSUCHREQ);
		}
		/*
		 *  The request has been located.
		 *  Remove the request from the running (routing set) of
		 *  the containing pipe queue.
		 */
		if (predecessor == (struct request *) 0) {
			req->queue->runset = req->next;
		}
		else predecessor->next = req->next;
		req->queue->q.runcount--;	/* One less routing request */
		/*
		 *  Set the QUE_UPDATE bit for the parent queue, so that
		 *  the database image of the queue will be updated later.
		 */
		req->queue->q.status |= QUE_UPDATE;
		/*
		 *  Free up the allocated entry in the Runvars[] array for
		 *  the recently routed request.
		 */
		Runvars [req->reqindex].allocated = 0;
		/*
		 *  Record one less running pipe queue request, and notify
		 *  the pipe queue scheduler of this momentous occasion.
		 */
		Gblpipcount--;		/* One less pipe request */
		psc_reqcom (req);	/* Notify pipe scheduler */
		if (req->queue->q.status & QUE_UPDATE) {
			/*
			 *  No requests from the specified queue were
			 *  activated (and therefore the NQS database
			 *  image for the queue has not been updated).
			 *  Do so now.
			 */
			udb_qorder (req->queue);
		}
	}
	if (rawreq->type == RTYPE_DEVICE) {
		/*
		 *  Store away device request parameters.
		 */
		strcpy (req->v1.req.v2.dev.forms, rawreq->v.dev.forms);
		req->v1.req.v2.dev.copies = rawreq->v.dev.copies;
		req->v1.req.v2.dev.size = rawreq->v.dev.size;
	}
	/*
	 *  Now, schedule the request for execution.
	 */
	switch (queue->q.type) {
	case QUE_BATCH:
		req->v1.req.priority = bsc_sched (rawreq);
		break;
	case QUE_DEVICE:
		req->v1.req.priority = dsc_sched (rawreq);
		break;
	case QUE_PIPE:
		req->v1.req.priority = psc_sched (rawreq);
		break;
	}
	/*
	 *  Set the remaining fields of the request as appropriate.
	 */
	req->status = rawreq->flags;	/* Load status bits as needed */
	if (wasexe) {
		/*
		 *  This request is being requeued via nqs_rbuild(),
		 *  and will be restarted.
		 */
		req->status |= RQF_WASEXE;
	}
	if (mode == 2 && prearrive > 0) {
		/*
		 *  The request is in the pre-arrive state, being
		 *  queued from another remote machine.
		 *
		 *  Set pre-arrive flag in the request status bits,
		 *  and set the pre-arrive abort remote transaction
		 *  timer.
		 */
		req->status |= RQF_PREARRIVE;
		req->v1.req.state_time = prearrive;
		prearrive += TRANS_TIMEOUT;
		nqs_vtimer (&prearrive, pip_timeout);
	}
	else req->v1.req.state_time = rawreq->enter_time;
	req->reqindex = -1;		/* Request is not running */
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
	}
	else {
		/*
		 *  The request is being placed in a batch or device
		 *  queue, and so the -a time parameter of the request
		 *  is now meaningful.
		 */
		req->start_time = rawreq->start_time;
					/* Remember start after time */
	}
	req->next = (struct request *)0;/* No more requests */
	strcpy (req->v1.req.reqname, rawreq->reqname);
	req->v1.req.uid = statbuf.st_uid;
	req->v1.req.orig_mid = rawreq->orig_mid;
	req->v1.req.orig_seqno = rawreq->orig_seqno;
	req->v1.req.trans_id = rawreq->trans_id;
	req->v1.req.ndatafiles = rawreq->ndatafiles;
#ifdef SDSC
        req->v1.req.v2.bat.rcpu = rawreq->v.bat.prcputime.max_seconds;
        req->v1.req.v2.bat.ncpus = rawreq->v.bat.prncpus;
	req->v1.req.v2.bat.period_inx = rawreq->period_inx;
	req->v1.req.v2.bat.major_nodes = rawreq->major_nodes;
#endif
	/*
	 *  Place the prioritized request into the queue.
	 */
	if (mode > 0) {
		/*
		 *  We are queueing a request that is being queued
		 *  from a local or remote pipe queue.  Please note
		 *  that the QUE_UPDATE bit is set by a2s_a2aset().
		 */
		a2s_a2aset (req, queue);/* Add to arriving set */
		if (Booted && mode == 2) {
			/*
			 *  It is guaranteed that the request is not
			 *  necessarily going to be changing state
			 *  anytime soon, and so we must therefore
			 *  update the queue-ordering file at this
			 *  time.
			 *
			 *  All this work is being done at the
			 *  behest of a PKT_RMTQUEREQ packet.
			 */
			udb_qorder (queue);
		}
	}
	else if ((rawreq->flags & RQF_OPERHOLD) ||
		 (rawreq->flags & RQF_USERHOLD)) {
		/*
		 *  Add the request to the hold set for this queue.
		 *  The QUE_UPDATE bit is set by a2s_a2hset().
		 */
		a2s_a2hset (req, queue);/* Add to hold set */
	}
	else if (rawreq->start_time > time ((time_t *) 0)) {
		/*
		 *  The request has a start time in the future.
		 *  The QUE_UPDATE bit is set by a2s_a2wset().
		 */
		a2s_a2wset (req, queue);/* Add to wait set */
	}
	else {
		/*
		 *  Place the request in the eligible to run set.
		 *  The QUE_UPDATE bit is set by a2s_a2qset().
		 */
		a2s_a2qset (req, queue);/* Add to queued set */
	}
	lastrequest = req;		/* Remember new request */
#ifdef SDSC
	/* Do an update now to eliminate the window that the request
	 * doesn't show up in the qstat command.
	 */

	if (req->queue->q.status & QUE_UPDATE) {
		udb_qorder (req->queue);
	}
#endif
	/*
	 *  Return success, but also report the limits that had to be
	 *  altered to reasonable values for the underlying UNIX
	 *  implementation at the local host.
	 */
	return (res);
}

 
/*** nsq_spawn
 *
 *
 *	void nsq_spawn():
 *	Invoke the proper scheduler for the request last queued.
 */
void nsq_spawn()
{
	register struct queue *queue;

	queue = lastrequest->queue;
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
}


/*** nsq_unque
 *
 *
 *	void nsq_unque():
 *
 *	Unqueue the last most recently queued request.  This procedure
 *	should ONLY be invoked by nqs_quereq().
 */
void nsq_unque()
{
	nqs_deque (lastrequest);	/* Dequeue the request */
	lastrequest->queue->q.status &= ~QUE_UPDATE;
					/* The containing queue is now */
					/* back to its original state */
	nqs_disreq (lastrequest, 0);	/* Dispose of the request */
}


#ifdef SDSC
/*** iOtherGroups()
 *
 *
 *      int iOtherGroups()
 *
 *      will search file /etc/groups 
 *
 *	Returns: 1 if user "uid" is a member of group "gid" otherwise return 0
 *
 */
int iOtherGroups(gid, uid)
gid_t gid;			       /* Group id (internal NQS form)*/
uid_t uid;			       /* User id */
{
  struct passwd *pw;
  struct group *gr;
  char **cp;

  pw = fetchpwuid((int)uid);
  if (pw == NULL ) return(0);
  /* if (0 == setgrent ()) return(0);      0 == failure */
  /* should be int setgrent() but it is void setgrent() in OSF/1 AD */
  (void) setgrent(); 
  while (gr = getgrent()) {         
    /* search group file for user */
    if (pw->pw_gid == gr->gr_gid) {
      if (gid == (gr->gr_gid | MAKEGID)) {
	return(1);
      }
      continue;
    }
    for (cp = gr->gr_mem; cp && *cp; cp++)
      if (strcmp(*cp, pw->pw_name) == 0) {
	if (gid == (gr->gr_gid | MAKEGID)) {
	  return(1);
	}
	break;
      }
  }
  return(0);
}
#endif


/*** verify_access
 *
 *
 *	int verify_access ():
 *
 *	Verify that the request can be placed in the specified queue.
 *	Returns: 1 if access is permitted, 0 if access is not permitted.
 *
 */
static int verify_access (gid, uid, queue)
gid_t gid;					/* Group id */
uid_t uid;					/* User id */
struct queue *queue;				/* Queue in question */
{
	struct acclist *acclistp;		/* Marches thru acclist */
	int i;					/* Loop variable */
	
	if (uid == 0) return (1);		/* Root always has access */
	if (queue->q.status & QUE_BYGIDUID) {	/* Restricted access */
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
				if ((acclistp->entries [i] == (gid | MAKEGID))
				 || (acclistp->entries [i] == uid)) {
					return (1);
				}
#ifdef SDSC
				if (iOtherGroups(acclistp->entries [i], uid) == 1)
				  return (1);
#endif
				/* 
				 * Zeroes come only at the end.
				 */
				if (acclistp->entries [i] == 0) {
				  return (0);	
				}
			}
			acclistp = acclistp->next;
		}
		return (0);	
	}
	else return (1);		/* Unrestricted access */
}


/*** verify_resources
 *
 *
 *	long verify_resources():
 *	Verify resources for a request.
 *
 *	Returns:
 *		TCML_SUBMITTED if successful (information bits
 *		may be present).  Otherwise the TCML_reason code
 *		is returned describing the condition (information
 *		bits may be present).
 */
static long verify_resources (queue, rawreq)
register struct queue *queue;		/* Target queue */
register struct rawreq *rawreq;		/* Rawreq for request */
{
	void verify_cpu_limit();	/* Verify CPU limit */
	void verify_quota_limit();	/* Verify quota limit */

	register struct gendescr *forms;/* Forms file descriptor */
	long modifyres;			/* Quota limit modified set */
	long exceedres;			/* Quota limit exceeded set */
#ifdef SDSC
        int iTotalQueued = 0;
#endif

	modifyres = 0;			/* No quotas modified or */
	exceedres = 0;			/* exceeded (yet anyway) */
	if (rawreq->type == RTYPE_BATCH) {
		/*
		 *  Check that the request does not exceed any
		 *  of the resource limits associated with the
		 *  batch queue.
		 */
#if	(VALID_LIMITS & LIM_PPCORE)
		verify_quota_limit (rawreq, &rawreq->v.bat.ppcoresize,
				    LIM_PPCORE, TCI_PP_CFLEXC, queue,
				    queue->q.v1.batch.ppcorecoeff,
				    queue->q.v1.batch.ppcoreunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPDATA)
		verify_quota_limit (rawreq, &rawreq->v.bat.ppdatasize,
				    LIM_PPDATA, TCI_PP_DSLEXC, queue, 
				    queue->q.v1.batch.ppdatacoeff,
				    queue->q.v1.batch.ppdataunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPPFILE)
		verify_quota_limit (rawreq, &rawreq->v.bat.pppfilesize,
				    LIM_PPPFILE, TCI_PP_PFLEXC, queue, 
				    queue->q.v1.batch.pppfilecoeff,
				    queue->q.v1.batch.pppfileunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PRPFILE)
		verify_quota_limit (rawreq, &rawreq->v.bat.prpfilespace,
				    LIM_PRPFILE, TCI_PR_PFLEXC, queue, 
				    queue->q.v1.batch.prpfilecoeff,
				    queue->q.v1.batch.prpfileunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPQFILE)
		verify_quota_limit (rawreq, &rawreq->v.bat.ppqfilesize,
				    LIM_PPQFILE, TCI_PP_QFLEXC, queue, 
				    queue->q.v1.batch.ppqfilecoeff,
				    queue->q.v1.batch.ppqfileunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PRQFILE)
		verify_quota_limit (rawreq, &rawreq->v.bat.prqfilespace,
				    LIM_PRQFILE, TCI_PR_QFLEXC, queue, 
				    queue->q.v1.batch.prqfilecoeff,
				    queue->q.v1.batch.prqfileunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPTFILE)
		verify_quota_limit (rawreq, &rawreq->v.bat.pptfilesize,
				    LIM_PPTFILE, TCI_PP_TFLEXC, queue, 
				    queue->q.v1.batch.pptfilecoeff,
				    queue->q.v1.batch.pptfileunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PRTFILE)
		verify_quota_limit (rawreq, &rawreq->v.bat.prtfilespace,
				    LIM_PRTFILE, TCI_PR_TFLEXC, queue, 
				    queue->q.v1.batch.prtfilecoeff,
				    queue->q.v1.batch.prtfileunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPMEM)
		verify_quota_limit (rawreq, &rawreq->v.bat.ppmemsize,
				    LIM_PPMEM, TCI_PP_MSLEXC, queue, 
				    queue->q.v1.batch.ppmemcoeff,
				    queue->q.v1.batch.ppmemunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PRMEM)
		verify_quota_limit (rawreq, &rawreq->v.bat.prmemsize,
				    LIM_PRMEM, TCI_PR_MSLEXC, queue, 
				    queue->q.v1.batch.prmemcoeff,
				    queue->q.v1.batch.prmemunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPSTACK)
		verify_quota_limit (rawreq, &rawreq->v.bat.ppstacksize,
				    LIM_PPSTACK, TCI_PP_SSLEXC, queue, 
				    queue->q.v1.batch.ppstackcoeff,
				    queue->q.v1.batch.ppstackunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPWORK)
		verify_quota_limit (rawreq, &rawreq->v.bat.ppworkset,
				    LIM_PPWORK, TCI_PP_WSLEXC, queue, 
				    queue->q.v1.batch.ppworkcoeff,
				    queue->q.v1.batch.ppworkunits,
				    &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPCPUT)
		verify_cpu_limit (rawreq, &rawreq->v.bat.ppcputime,
				  LIM_PPCPUT, TCI_PP_CTLEXC, queue, 
				  queue->q.v1.batch.ppcpusecs,
				  queue->q.v1.batch.ppcpums,
				  &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PRCPUT)
		verify_cpu_limit (rawreq, &rawreq->v.bat.prcputime,
				  LIM_PRCPUT, TCI_PR_CTLEXC, queue, 
				  queue->q.v1.batch.prcpusecs,
				  queue->q.v1.batch.prcpums,
				  &modifyres, &exceedres);
#endif
#if	(VALID_LIMITS & LIM_PPNICE)
		if (rawreq->v.bat.explicit & LIM_PPNICE) {
			if (rawreq->v.bat.infinite & LIM_PPNICE) {
				/*
				 * Infinite nice is meaningless in NQS.
				 */
				exceedres |= TCI_PP_NELEXC;
			}
			else {
				/*
				 *  The batch request specifies a
				 *  finite nice value.  Bind the
				 *  specified nice value to the
				 *  range supported by the UNIX
				 *  implementation of the local
				 *  host.
				 */
				if (rawreq->v.bat.ppnice > MAX_NICE) {
					/*
					 *  The request specifies a
					 *  nice limit that is too
					 *  high for the supporting
					 *  UNIX implementation.
					 */
					modifyres |= TCI_PP_NELEXC;
					rawreq->v.bat.ppnice = MAX_NICE;
				}
				else if (rawreq->v.bat.ppnice < MIN_NICE){
					/*
					 *  The request specifies a
					 *  nice limit that is too
					 *  low for the supporting
					 *  UNIX implementation.
					 */
					modifyres |= TCI_PP_NELEXC;
					rawreq->v.bat.ppnice = MIN_NICE;
				}
				if (rawreq->v.bat.ppnice <
				    queue->q.v1.batch.ppnice) {
					exceedres |= TCI_PP_NELEXC;
				}
			}
		}
		else {
			/*
			 *  The batch request did not specify a
			 *  nice limit.  The batch request inherits
			 *  the nice limit of the queue by default.
			 */
			rawreq->v.bat.infinite &= ~LIM_PPNICE;
			rawreq->v.bat.ppnice = queue->q.v1.batch.ppnice;
		}
#endif
#if	(VALID_LIMITS & LIM_PRDRIVES)
	/* MOREHERE */
#endif
#if	(VALID_LIMITS & LIM_PRNCPUS)
	/* MOREHERE */
#ifdef SDSC
		if (rawreq->v.bat.explicit & LIM_PRNCPUS) {
			if (rawreq->v.bat.infinite & LIM_PRNCPUS) {
				/*
				 * Infinite ncpus is not allowed in NQS.
				 */
				exceedres |= TCI_PR_NCPEXC;
			}
			else {
				/*
				 *  The batch request specifies a
				 *  finite ncpus value.  Bind the
				 *  specified ncpus value to the
				 *  range supported by the UNIX
				 *  implementation of the local
				 *  host.
				 */
				if (rawreq->v.bat.prncpus > MAX_NCPUS ||
				rawreq->v.bat.prncpus < MIN_NCPUS ||
				rawreq->v.bat.prncpus >
				queue->q.v1.batch.prncpus) {
					exceedres |= TCI_PR_NCPEXC;
				}
			}
		}
		else {
			/*
			 *  The batch request did not specify a
			 *  ncpus limit.  The batch request inherits
			 *  the ncpus limit of the queue by default.
			 */
			rawreq->v.bat.infinite &= ~LIM_PRNCPUS;
			rawreq->v.bat.prncpus = queue->q.v1.batch.prncpus;
		}
#endif
#endif
 
#if     (VALID_LIMITS & LIM_PRNCPUS)
        /* MOREHERE */
#ifdef SDSC
                /*
                 * if queue->q.v1.batch.sPerQueMaxQueuedULimit == 0
                 * than do NOT enforce max queued limit
                 *
                 */
                if ((queue->q.v1.batch.sPerQueMaxQueuedULimit != 0 )) {
                  iTotalQueued = iQueuedByUid(queue->q.namev.name, rawreq->orig_uid);
                  if (queue->q.v1.batch.sPerQueMaxQueuedULimit <= iTotalQueued) {
                    printf("I$qsub request for user uid %d and queue %s was rejected because\n",
                           rawreq->orig_uid, queue->q.namev.name);
                    printf("I$user uid %d has already queued %d requests and max. queued limit is set to %d\n",
                           rawreq->orig_uid, iTotalQueued, queue->q.v1.batch.sPerQueMaxQueuedULimit);
                    fflush(stdout);
		    /*
		      modifyres = 0;                  / * No quotas modified or * /
		      exceedres = 0;                  / * exceeded (yet anyway)  * /
		      exceedres |= TCMP_EPERM;
		    */
		    return (TCML_EPERM);
		  }
                }
#endif GJK
#endif
	}				/* End of batch request checks */
	else {
		if (rawreq->v.dev.forms [0] != '\0') {
			/*
			 *  Forms were specified for the device
			 *  request.  Make sure that the required
			 *  forms exist.
			 */
			seekdbb (Formsfile, 0L);
			forms = nextdb (Formsfile);
			while (forms != (struct gendescr *) 0 &&
			       strcmp (forms->v.form.forms,
				       rawreq->v.dev.forms)) {
				forms = nextdb (Formsfile);
			}
			if (forms == (struct gendescr *) 0) {
				/*
				 *  The specified forms do not exist.
				 */
				return (TCML_NOSUCHFORM);
			}
		}
		if (Maxcopies < rawreq->v.dev.copies) {
			exceedres |= TCI_COPLIMEXC;
		}
		if (Maxprint < rawreq->v.dev.size) {
			exceedres |= TCI_PRILIMEXC;
		}
	}
	if (exceedres) return (TCML_QUOTALIMIT | exceedres);
	return (TCML_SUBMITTED | modifyres);
}


/*** verify_cpu_limit
 *
 *
 *	void verify_cpu_limit():
 *	Verify the specified request CPU time limit.
 */
static void verify_cpu_limit (rawreq, quotaval, quotatype, icm_code, queue,
			      queue_seconds, queue_ms, modifyres, exceedres)
struct rawreq *rawreq;			/* Raw request structure */
struct cpulimit *quotaval;		/* CPU quota limit for quota type */
int quotatype;				/* Quota type LIM_ */
long icm_code;				/* XCI_ message bit pattern */
struct queue *queue;			/* Target queue */
unsigned long queue_seconds;		/* Queue CPU limit in seconds */
short queue_ms;				/* Queue CPU milliseconds limit */
long *modifyres;			/* Modified quota limits */
long *exceedres;			/* Exceeded quota limits */
{
	unsigned long new_secs;		/* cpulimhigh(), cpulimlow() return */
					/* value */
	short new_ms;			/* cpulimhigh(), cpulimlow() return */
					/* value */

	if (rawreq->v.bat.explicit & quotatype) {
		/*
		 *  The request specifies an explicit CPU time limit.
		 */
		if (cpulimhigh (quotaval->max_seconds, quotaval->max_ms,
				rawreq->v.bat.infinite & quotatype,
				&new_secs, &new_ms)) {
			*modifyres |= icm_code;
			quotaval->max_seconds = new_secs;
			quotaval->max_ms = new_ms;
			/*
			 * If the former limit was infinite, establish
			 * a sane warning.  If the former limit was
			 * was finite, don't mess with the warning
			 * unless we have to.
			 */
			if (rawreq->v.bat.infinite & quotatype) {
				quotaval->warn_seconds = new_secs;
				quotaval->warn_ms = new_ms;
			}
			else {
				if (quotaval->warn_seconds > new_secs ||
			   	(quotaval->warn_seconds == new_secs &&
			    	quotaval->warn_ms > new_ms)) {
					quotaval->warn_seconds = new_secs;
					quotaval->warn_ms = new_ms;
				}
			}
			rawreq->v.bat.infinite &= ~quotatype;
		}
		if (rawreq->v.bat.infinite & quotatype) {
			/*
			 *  The request specifies an infinite CPU
			 *  limit, and such a limit can be enforced.
			 */
			if (!(queue->q.v1.batch.infinite & quotatype)) {
				/*
				 *  The request specifies an infinite CPU
				 *  time limit, but the corresponding batch
				 *  queue CPU time limit is finite.
				 */
				*exceedres |= icm_code;
			}
		}
		else {
			/*
			 *  The request specifies a finite CPU time limit,
			 *  or the request specifies an infinite CPU time
			 *  limit that we will interpret as a finite CPU
			 *  time limit because an infinite limit cannot be
			 *  enforced.  Check that the finite limit
			 *  is enforceable.  Change if necessary.
			 */
			if (cpulimlow (quotaval->max_seconds, quotaval->max_ms,
				       &new_secs, &new_ms)) {
				*modifyres |= icm_code;
				quotaval->max_seconds = new_secs;
				quotaval->max_ms = new_ms;
				if (quotaval->warn_seconds > new_secs ||
				   (quotaval->warn_seconds == new_secs &&
				    quotaval->warn_ms > new_ms)) {
					/*
					 *  The warning limit must be changed
					 *  as well.
					 */
					quotaval->warn_seconds = new_secs;
					quotaval->warn_ms = new_ms;
				}
			}
			if (!(queue->q.v1.batch.infinite & quotatype) &&
			   ((quotaval->max_seconds > queue_seconds) ||
			    (quotaval->max_seconds == queue_seconds &&
			     quotaval->max_ms > queue_ms))) {
				/*
				 *  The resolved batch request quota limit
				 *  exceeds the corresponding queue quota
				 *  limit.
				 */
				*exceedres |= icm_code;
			}
		}
	}
	else {
		/*
		 *  The batch request did not specify a CPU quota limit
		 *  for the CPU resource limit type.  By default, the batch
		 *  request inherits the corresponding queue CPU limit value.
		 */
		if (queue->q.v1.batch.infinite & quotatype) {
			rawreq->v.bat.infinite |= quotatype;
		}
		else rawreq->v.bat.infinite &= ~quotatype;
		quotaval->max_seconds = queue_seconds;
		quotaval->max_ms = queue_ms;
		quotaval->warn_seconds = queue_seconds;
		quotaval->warn_ms = queue_ms;
	}
}


/*** verify_quota_limit
 *
 *
 *	void verify_quota_limit():
 *	Verify the specified request quota limit.
 */
static void verify_quota_limit (rawreq, quotaval, quotatype, icm_code, queue,
				queue_coefficient, queue_units, modifyres,
				exceedres)
struct rawreq *rawreq;			/* Raw request structure */
struct quotalimit *quotaval;		/* Quota limit for quota type */
int quotatype;				/* Quota type LIM_ */
long icm_code;				/* XCI_ message bit pattern */
struct queue *queue;			/* Target queue */
unsigned long queue_coefficient;	/* Queue quota coefficient */
short queue_units;			/* Queue quota units */
long *modifyres;			/* Modified quota limits */
long *exceedres;			/* Exceeded quota limits */
{
	unsigned long new_quota;	/* quolimhigh(), quolimlow() return */
					/* value */
	short new_units;		/* quolimhigh(), quolimlow() return */
					/* value */

	if (rawreq->v.bat.explicit & quotatype) {
		/*
		 *  The batch request specifies an explicit quota limit
		 *  value.
		 */
		if (quolimhigh (quotaval->max_quota, quotaval->max_units,
				rawreq->v.bat.infinite & quotatype,
				quotatype, &new_quota, &new_units)) {
			*modifyres |= icm_code;
			quotaval->max_quota = new_quota;
			quotaval->max_units = new_units;
			/*
			 *  If the former limit was infinite, establish
			 *  a sane warning.  If the former limit was
			 *  was finite, don't mess with the warning
			 *  unless we have to.
			 */
			if (rawreq->v.bat.infinite & quotatype) {
				quotaval->warn_quota = new_quota;
				quotaval->warn_units = new_units;
			}
			else {
				if (secgrfir (new_quota, new_units,
				      		quotaval->warn_quota,
				      		quotaval->warn_units)) {
					quotaval->warn_quota = new_quota;
					quotaval->warn_units = new_units;
				}
			}
			rawreq->v.bat.infinite &= ~quotatype;
		}
		if (rawreq->v.bat.infinite & quotatype) {
			/*
			 *  The request specifies an infinite quota
			 *  limit, and such a limit can be enforced.
			 */
			if (!(queue->q.v1.batch.infinite & quotatype)) {
				/*
				 *  The batch request specifies an infinite
				 *  limit, while the corresponding queue
				 *  limit is finite.
				 */
				*exceedres |= icm_code;
			}
		}
		else {
			/*
			 *  The request specifies a finite quota limit,
			 *  or the request specifies an infinite quota
			 *  limit that we will interpret as a finite quota
			 *  limit because an infinite limit cannot be
			 *  enforced.  Check that the finite limit
			 *  is enforceable.  Change if necessary.
			 */
			if (quolimlow (quotaval->max_quota,
				       quotaval->max_units, quotatype,
				       &new_quota, &new_units)) {
				*modifyres |= icm_code;
				quotaval->max_quota = new_quota;
				quotaval->max_units = new_units;
				if (secgrfir (new_quota, new_units,
					      quotaval->warn_quota,
					      quotaval->warn_units)) {
					/*
					 *  The warning limit must be changed
					 *  as well.
					 */
					quotaval->warn_quota = new_quota;
					quotaval->warn_units = new_units;
				}
			}
			if (!(queue->q.v1.batch.infinite & quotatype) &&
			    secgrfir (queue_coefficient,
				      queue_units,
				      quotaval->max_quota,
				      quotaval->max_units)) {
				/*
				 *  The resolved batch request quota limit
				 *  exceeds the corresponding queue quota
				 *  limit.
				 */
				*exceedres |= icm_code;
			}
		}
	}
	else {
		/*
		 *  The batch request did not specify a quota limit
		 *  for the resource limit type.  By default, the batch
		 *  request inherits the corresponding queue limit value.
		 */
		if (queue->q.v1.batch.infinite & quotatype) {
			rawreq->v.bat.infinite |= quotatype;
		}
		else rawreq->v.bat.infinite &= ~quotatype;
		quotaval->max_quota = queue_coefficient;
		quotaval->max_units = queue_units;
		quotaval->warn_quota = queue_coefficient;
		quotaval->warn_units = queue_units;
	}
}
