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
 
/*++ nqs_reqcom.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_reqcom.c,v $
 *
 * DESCRIPTION:
 *
 *	Process an NQS request completion event.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:53:12 $ $State: Exp $)
 * $Log: nqs_reqcom.c,v $
 * Revision 1.4  1994/11/19  02:53:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/02  23:01:34  mwan
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
 * Revision 1.2  1992/10/09  22:26:07  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:26  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:43  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_reqcom.c	50.3 (nqs_reqcom.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <errno.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS global variables */
#include "nqsacct.h"
#if	UNICOS | SGI | SYS52 | UTSQ | OSF
#include <sys/times.h>			/* Get struct tms definition */
#include <fcntl.h>			/* O_CREAT etc. for accounting */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#include <sys/resource.h>		/* Get struct rusage definition */
#include <sys/file.h>			/* O_CREAT etc. for accounting */
#else
BAD SYSTEM TYPE
#endif
#endif

#ifdef SDSC
#include "buddyxvar.h"
extern int end_job ();
#endif

extern int errno;			/* System call error number */
extern void a2s_a2qset();		/* Add request to queued set */
extern void a2s_a2wset();		/* Add request to waiting set */
extern char *asciierrno();		/* Return ASCII errno */
extern void bsc_reqcom();		/* Batch request completion */
extern void dsc_reqcom();		/* Device request completion */
extern void dsc_spawn();		/* Possibly spawn a device request */
extern char *getusenam();		/* Get username for uid */
extern void nqs_abort();		/* Abort NQS execution */
extern void nqs_disreq();		/* Dispose of request */
extern void nqs_failed();		/* Place request in failed directory */
					/* for the mystification of the */
					/* system staff folks. */
extern struct queue *nqs_fndnnq();	/* Find non-network queue */
extern long pip_reqreceived();		/* Request completely received */
extern void psc_reqcom();		/* Pipe request completion */
extern time_t time();			/* Get GMT time */
extern void udb_device();		/* Update NQS device status */
extern void udb_qorder();		/* Update NQS queue ordering */

/*** nqs_reqcom
 *
 *
 *	void nqs_reqcom():
 *	Process an NQS request completion event.
 */
#if	UNICOS | SGI | SYS52 | UTS | OSF
void nqs_reqcom (orig_seqno, orig_mid, exitcode, tms, queuename)
long orig_seqno;			/* Request original sequence# */
mid_t orig_mid;				/* Request original machine-id */
int exitcode;				/* Exit code */
struct tms *tms;			/* CPU time usage by request */
char *queuename;			/* Queuename for which request */
					/* is reporting an exit code */
#else
#if	BSD42 | BSD43 | ULTRIX
void nqs_reqcom (orig_seqno, orig_mid, exitcode, rusage, queuename)
long orig_seqno;			/* Request original sequence# */
mid_t orig_mid;				/* Request original machine-id */
int exitcode;				/* Exit code */
struct rusage *rusage;			/* Resource utilization by request */
char *queuename;			/* Queuename for which request */
					/* is reporting an exit code */
#else
BAD SYSTEM TYPE
#endif
#endif
{
	void requeue();			/* Requeue request function */
	void stopallqueues();		/* Stop all NQS queues */
	void stopdev();			/* Stop and mark device as failed */

	int childpid;			/* Process-id of exited child */
	short free_request_files;	/* BOOLEAN delete files associated */
					/* with the request flag.  This */
					/* flag is NEVER true without */
					/* free_request_struct ALSO being */
					/* true */
	short free_request_struct;	/* BOOLEAN delete request struct */
					/* for request flag */
	register struct queue *serverq;	/* Server was handling this queue */
	register struct request *req;	/* Request structure */
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	register struct device *device;	/* Device handling device request */
	register struct qcomplex *qcomplex; /* Queue complex pointer */
	register int i;
	int fd_acct;			/* Accounting file descriptor */
	struct nqsacct_fin acct_fin;	/* Accounting structure to report */
					/* Cpu usage 			  */


	/*
	 *  Wait for a shepherd process to exit.
	 */
	while ((childpid = wait ((int *) 0)) == -1 && errno == EINTR)
		;
	/*
	 *  Childpid has the process-id of the exited child.
	 *  See if it was (heaven forbid), the NQS network
	 *  daemon or log daemon process!
	 */
	if (Logdaepid == childpid) {
		/*
		 *  The NQS log daemon exited!
		 *  It is better to leave the cave if your light burns out.
		 */
		nqs_abort ();
	}
	if (Netdaepid == childpid) {
		/*
		 *  The NQS network daemon exited!
		 */
		if (Shutdown) {
			printf ("I$Network daemon has exited.\n");
		}
		else {
			printf ("W$Network daemon has crashed.\n");
			printf ("W$No more network requests will be ");
			printf ("received.\n");
		}
		fflush (stdout);
		Netdaepid = 0;		/* Network daemon is gone */
		/*
		 *  Wait again to collect an NQS shepherd process.
		 */
		while ((childpid = wait ((int *) 0)) == -1 && errno == EINTR)
			;
	}
	if (childpid == -1) {		/* Wait call failed */
		if (errno == ECHILD) {
			/*
			 *  We were sent a message that an NQS request
			 *  server completed, but we have no servers!
			 *  No children!  None!
			 */
			printf ("E$Spurious request completion ");
			printf ("message received.\n");
			fflush (stdout);
			return;
		}
		else {
			printf ("F$Wait() call in nqs_reqcom.c ");
			printf ("failed.\n");
			nqs_abort();
		}
	}
	if (Debug > 2) {
		printf ("D$Nqs_reqcom() picked up child %1d.\n", childpid);
		fflush (stdout);
	}
	/*
	 *  Locate the queue containing the completed request.
	 */
	serverq = nqs_fndnnq (queuename);
	if (serverq == (struct queue *) 0) {
		/*
		 *  Bad queuename reported by shepherd!
		 */
		printf ("F$Bad queue name reported by request shepherd.\n");
		errno = 0;		/* Not a system call error */
		nqs_abort();
	}
	/*
	 *  Update resource usage statistics.
	 */
#if	UNICOS | SGI | SYS52 | UTS | OSF
	/*
	 *  Update system call time.
	 */
	serverq->q.ru_stime += tms->tms_stime;
	serverq->q.ru_stime += tms->tms_cstime;
	/*
	 *  Update user-space time.
	 */
	serverq->q.ru_utime += tms->tms_utime;
	serverq->q.ru_utime += tms->tms_cutime;
#else
#if	BSD42 | BSD43 | ULTRIX
	/*
	 *  Update system call time.
	 */
	serverq->q.ru_stime_usec += rusage->ru_stime.tv_usec;
	if (serverq->q.ru_stime_usec > 1000000) {
		serverq->q.ru_stime_usec -= 1000000;
		serverq->q.ru_stime += 1;
	}
	serverq->q.ru_stime += rusage->ru_stime.tv_sec;
	/*
	 *  Update user-space time.
	 */
	serverq->q.ru_utime_usec += rusage->ru_utime.tv_usec;
	if (serverq->q.ru_utime_usec > 1000000) {
		serverq->q.ru_utime_usec -= 1000000;
		serverq->q.ru_utime += 1;
	}
	serverq->q.ru_utime += rusage->ru_utime.tv_sec;
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  If the request was routed by a pipe queue, then we must
	 *  do quite a bit of extra work....
	 */
	if ((exitcode & 0003) == 3) {
		/*
		 *  The request is presently in the arriving state and
		 *  was successfully routed by a local pipe queue.
		 *  Locate the request, and modify the state of the
		 *  request as appropriate (provided that the request
		 *  has not been deleted).
		 */
		pip_reqreceived (orig_seqno, orig_mid);
	}
	else {
		/*
		 *  The request is presently still in the running state.
		 */
		predecessor = (struct request *) 0;
		req = serverq->runset;
		while (req != (struct request *) 0 &&
		      (req->v1.req.orig_seqno != orig_seqno ||
		       req->v1.req.orig_mid != orig_mid)) {
			/*
			 *  Keep searching.
			 */
			predecessor = req;	/* Remember predecessor */
			req = req->next;	/* Examine the next request */
		}
		if (req == (struct request *) 0) {
			/*
			 *  Request not found!
			 */
			printf ("F$Request not found in nqs_reqcom().\n");
			errno = 0;		/* Not a system call error */
			nqs_abort();		/* Abort execution */
		}
		/*
		 *  Remove the request from its current position in the
		 *  queue.
		 */
		if (predecessor == (struct request *) 0) {
			serverq->runset = req->next;
		}
		else predecessor->next = req->next;
		serverq->q.runcount--;		/* One less running request */
		if (serverq->q.type == QUE_BATCH) {
#ifdef SDSC
			end_job (req);
#endif
			for (i = MAX_COMPLXSPERQ; --i >= 0;) {
				qcomplex = serverq->v1.batch.qcomplex[i];
				if (qcomplex == (struct qcomplex *)0) continue;
				qcomplex->runcount--;
			}
		}
		/* addition by Intergraph Bill Mar - 122/08/89 TAC */
		if (serverq->q.type == QUE_DEVICE) {
			for (i = MAX_COMPLXSPERQ; --i >= 0;) {
				qcomplex = serverq->v1.device.qcomplex[i];
				if (qcomplex == (struct qcomplex *)0) continue;
				qcomplex->runcount--;
			}
		}
		serverq->q.status |= QUE_UPDATE;/* Database update required */
		/*
		 *  Free up the allocated entry in the Runvars[] array for the
		 *  recently completed request.
		 */
		Runvars [req->reqindex].allocated = 0;
	}
	/*
	 *  Req points to the request structure for the request that
	 *  just sent its request completion packet to us.
	 *
	 *  Serverq points to the queue structure within which a
	 *  completed request is queued (or was queued).
	 */
	
	/* --------------------------------------
	 * Report cpu, usage etc. for a batch queue;
	 */
	if (serverq -> q.type == QUE_BATCH ) {
		fd_acct = open(NQSACCT_FILE,
			O_WRONLY|O_APPEND|O_CREAT, 0644);
		if (fd_acct < 0) {
			fprintf (stderr, "E$Error opening NQS account");
			fprintf (stderr, " file;  Errno = %d\n", errno);
			fflush (stderr);
		} else {
			bytezero((char *)&acct_fin, sizeof(acct_fin));
			acct_fin.h.type = NQSACCT_FIN;
			acct_fin.h.length = sizeof(acct_fin);
			acct_fin.h.jobid= Runvars[req->reqindex].process_family;
			strncpy(acct_fin.user,getusenam(req->v1.req.uid), 
				sizeof(acct_fin.user));
			strncpy(acct_fin.queue, serverq->q.namev.name, 
				sizeof(acct_fin.queue));
			acct_fin.priority = serverq->q.priority;
#if 	UNICOS | SGI | SYS52 | UTS | OSF
			acct_fin.tms_stime = tms->tms_stime + tms->tms_cstime;
			acct_fin.tms_utime = tms->tms_utime + tms->tms_cutime;
#else
#if	BSD42 | BSD43 | ULTRIX
			acct_fin.s_sec = rusage->ru_stime.tv_sec;
			acct_fin.s_usec = rusage->ru_stime.tv_usec;
			acct_fin.u_sec = rusage->ru_utime.tv_sec;
			acct_fin.u_usec = rusage->ru_utime.tv_usec;
#else
BAD SYSTEM TYPE
#endif
#endif
			acct_fin.orig_mid = req->v1.req.orig_mid;
		}
		write(fd_acct, &acct_fin, sizeof(acct_fin));
		close(fd_acct);
	}
	/*-----------------------*/

	if (serverq->q.type == QUE_DEVICE) {
		/*
		 *  Identify the device that was servicing the recently
		 *  completed request.  The internal representation of
		 *  the device structure is updated to indicate that
		 *  the device is inactive, and that no current request
		 *  is being serviced by the device.  However, the NQS
		 *  database image for the device is NOT updated.
		 */
		device = Devset;
		while (device != (struct device *) 0 &&
		       device->curreq != req) {
			device = device->next;
		}
		if (device == (struct device *) 0) {
			/*
			 *  We did not find the device serving what
			 *  supposedly was a completed device req!
			 */
			printf ("F$NQS internal error.\n");
			printf ("I$Unable to find device that handled ");
			printf ("completed\n");
			printf ("I$device request.\n");
			fflush (stdout);
			nqs_abort();
		}
		device->status &= ~DEV_ACTIVE;	/* Device no longer active */
		device->curreq = (struct request *) 0;
						/* No current request */
		device->curque = (struct queue *) 0;
						/* No current queue */
	}
	/*
	 *  Based upon the request exitcode status, we must do
	 *  several things.  The exitcode bits have the following
	 *  definitions:
	 *
	 *  .---------------.
 	 *  |   |     | |   |	Request server shepherd process
	 *  |7|6|5|4|3|2|1|0|	exit code (of 8 bits)
	 *  `---------------'
	 *
	 *  Bits 1..0: contain the request disposition code:
	 *		0: Delete the request, provided that bit 2
	 *		   is clear, or the request is not to be
	 *		   requeued upon signal termination, or the
	 *		   request is not restartable.  Otherwise,
	 *		   the request must be requeued for restart.
	 *		1: Place the request in the failed directory;
	 *		2: Requeue the request for retry.
	 *		3: Request queued via local pipe queue.
	 *
	 *  Bit  2:    exit status:
	 *		0: Request exited via exit();
	 *		1: Request aborted by receipt of signal.
	 *
	 *  Bits 5..3: contain additional action bits which can
	 *	       be or'ed together as necessary.
	 *		1: Stop the device, that the request was
	 *		   running on, marking the device as
	 *		   failed;
	 *		2: Stop the queue that the request was
	 *		   (or is) residing in;
	 *		4: Stop all queues.
	 *
	 * Bit 6 :      1: abort NQS
	 *  Bits 7   : RESERVED for future use.
	 *
	 */
	switch (exitcode & 0003) {	/* Bits 1..0 */
	case 0: /*
		 *  Request completed normally.
		 */
		if ((req->status & RQF_SIGREQUEUE) &&
		    (req->status & RQF_RESTARTABLE) &&
		    (exitcode & 0004)) {
			/*
			 *  Requeue the request as ready to run.
			 *  The request must not be deleted.
			 */
			free_request_files = 0;
			free_request_struct = 0;
			requeue (req);
		}
		else {
			/*
			 *  The request should be deleted--later
			 *  (see below).
			 */
			free_request_files = 1;
			free_request_struct = 1;
		}
		break;
	case 1:	/*
		 *  The request is to be placed in the failed
		 *  directory.  The files associated with the
		 *  the request should be saved, but the memory-
		 *  resident information on the request should
		 *  be discarded.
		 */
		free_request_files = 0;		/* Save request files but */
		free_request_struct = 1;	/* free request structure */
		break;				/* Place req. in failed dir */
	case 2:	/*
		 *  The request is to be requeued for retry.
		 */
		free_request_files = 0;		/* The request must not */
		free_request_struct = 0;	/* be deleted. */
		requeue (req);			/* Requeue */
		break;
	case 3:	/*
		 *  The request is to be simply left-alone.
		 */
		free_request_files = 0;		/* The request must not */
		free_request_struct = 0;	/* be deleted. */
		break;
	}
	/*
	 *  Check for extra operations to be done depending
	 *  on the values of bits [5..3].
	 */
	if (exitcode & 0010) {
		/*
		 *  Stop the device that was servicing the request
		 *  and mark it as failed.
		 */
		if (serverq->q.type != QUE_DEVICE) {
			printf ("E$Shepherd process specified ");
			printf ("device shutdown for non-device ");
			printf ("request.\n");
			fflush (stdout);
		}
		else stopdev (device, serverq);
	}
	if (exitcode & 0020) {
		/*
		 *  Stop the queue that the request was in (or is
		 *  residing in).
		 */
		printf ("I$Stopping queue: %s.\n",
			serverq->q.namev.name);
		fflush (stdout);
		serverq->q.status &= ~QUE_RUNNING;
	}
	if (exitcode & 0040) {			/* Stop all queues */
		stopallqueues (serverq);	/* but don't update */
						/* database image of */
	}					/* the server queue */
#ifdef SDSC
	if (exitcode & 0100) {			/* Stop all queues */
		ups_shutdown (getpid (), getuid (), 0);
	}
#endif
	/*
	 *  Inform proper scheduler of the request completion.
	 */
	switch (serverq->q.type) {
	case QUE_BATCH:			/* The req was a batch req */
		Gblbatcount--;		/* One less batch request */
		bsc_reqcom (req);	/* Notify batch scheduler */
		break;
	case QUE_DEVICE:		/* The req was a device req */
		/*
		 *  Delete the request and remove it from the queue.
		 */
		udb_device (device);	/* Update database image */
		if (device->status & DEV_FAILED) {
			/*
			 *  The device failed to successfully handle
			 *  the request.  However, it may be possible
			 *  for another device serving the same queue
			 *  to handle the request.
			 */
			dsc_spawn();	/* Maybe spawn it again */
		}			/* on a different device */
		else {
			/*
			 *  Notify device schedule that the device
			 *  has become available.
			 */
			dsc_reqcom (device, req);
		}
		break;
	case QUE_NET:			/* MOREHERE */
		Gblnetcount--;		/* One less network request */
		break;
	case QUE_PIPE:			/* The req was a pipe req */
		if ((exitcode & 0003) != 3) {
			/*
			 *  This will have already been done if the request
			 *  was queued from a local pipe queue into another
			 *  local queue.
			 */
			Gblpipcount--;	/* One less pipe request */
			psc_reqcom(req);/* Notify pipe scheduler */
		}
		break;
	}
	if (serverq->q.status & QUE_UPDATE) {
		/*
		 *  No requests from the specified queue were
		 *  activated (and therefore the NQS database
		 *  image for the queue has not been updated).
		 */
		udb_qorder (serverq);	/* Update queue ordering */
	}
	/*
	 *  It is now safe to free up resources associated with the
	 *  request, as indicated by the free_ flags.
	 */
	if (free_request_struct) {
		/*
		 *  Dispose of the request, as appropriate.
		 */
		nqs_disreq (req, free_request_files);
		if (!free_request_files) {
			/*
			 *  The request files are to be placed in the
			 *  failed directory, for the mystification of
			 *  the NQS maintainers.
			 */
			nqs_failed (req->v1.req.orig_seqno,
				    req->v1.req.orig_mid);
		}
	}
}


/*** requeue
 *
 *
 *	void requeue():
 *
 *	Requeue the request that just completed, because of some sort
 *	of failure that warrants a retry effort.
 *
 *	The NQS database image of the server queue is NOT updated
 *	by this function.
 */
static void requeue (req)
register struct request *req;			/* Request to requeue */
{

	register struct queue *serverq;		/* Request queue */

	serverq = req->queue;			/* Containing queue */
	req->status &= (~RQF_SIGQUEUED & ~RQF_SIGREQUEUE);
						/* No queued signal; no */
						/* requeue on abort */
	if (req->start_time > time ((time_t *) 0)) {
		/*
		 *  Place the request in the waiting set for the queue
		 *  (a2s_a2wset() calls nqs_vtimer()).
		 */
		a2s_a2wset (req, serverq);
	}
	else {
		/*
		 *  The request can start again as soon as possible.
		 */
		a2s_a2qset (req, serverq);	/* Add to queued set */
	}
	fflush (stdout);
}


/*** stopallqueues
 *
 *
 *	void stopallqueues():
 *	Stop all NQS queues.
 */
static void stopallqueues (serverq)
register struct queue *serverq;	/* Queue containing exited req */
{
	register struct queue *queue;

	queue = Nonnet_queueset;		/* Non-network queues */
	while (queue != (struct queue *) 0) {
		queue->q.status &= ~QUE_RUNNING;	/* Stop the queue */
		if (serverq != queue) {
			/*
			 *  Only update the queue database image if the
			 *  the queue is NOT the server queue (which we
			 *  will update later).
			 */
			udb_queue (queue);	/* Update queue header */
		}
		queue = queue->next;
	}
	queue = Net_queueset;			/* Network queues */
	while (queue != (struct queue *) 0) {
		queue->q.status &= ~QUE_RUNNING;	/* Stop the queue */
		if (serverq != queue) {
			/*
			 *  Only update the queue database image if the
			 *  the queue is NOT the server queue (which we
			 *  will update later).
			 */
			udb_queue (queue);	/* Update queue header */
		}
		queue = queue->next;
	}
	printf ("I$All queues stopped.\n");
	fflush (stdout);
}


/*** stopdev
 *
 *
 *	void stopdev():
 *
 *	Stop and mark the specified device as failed.
 *	Note, that the server queue will be effectively stopped if the
 *	device was the last device enabled in the queue/device set
 *	for the server queue.
 */
static void stopdev (device, serverq)
register struct device *device;		/* Device to be marked as failed */
register struct queue *serverq;	/* Queue that was being handled by */
					/* the device */
{
	register struct qdevmap *map;	/* Walk queue/device mappings */

	/*
	 *  Inform.
	 */
	printf ("I$Stopping and marking device: %s as failed.\n",
		device->name);
	/*
	 *  Mark device as failed.
	 */
	device->status = DEV_FAILED;
	udb_device (device);		/* Update NQS image */
	/*
	 *  Determine if the device queue has any more devices
	 *  to which reqs can be sent which are enabled.  If
	 *  not, then the queue is effectively stopped and we
	 *  want to print a warning message.
	 */
	map = serverq->v1.device.qdmaps;
	while (map != (struct qdevmap *) 0 &&
	      (map->device->status & DEV_ENABLED) == 0) {
		map = map->nextqd;
	}
	if (map == (struct qdevmap *) 0) {
		/*
		 *  The server queue has no enabled devices for
		 *  it to use.  Warn that the queue is effectively
		 *  stopped.
		 */
		printf ("W$Queue: %s is effectively stopped ",
			serverq->q.namev.name);
		printf ("since no enabled devices\n");
		printf ("W$remain in its queue/device mapping set.\n");
		fflush (stdout);
	}
}
