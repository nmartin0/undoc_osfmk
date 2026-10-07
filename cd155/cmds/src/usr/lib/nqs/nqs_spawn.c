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
 
/*++ nqs_spawn.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_spawn.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains part of the logic to spawn
 *	an NQS request, and is the ONLY entry point to
 *	perform such an action.
 *
 *	The other modules containing logic dealing with
 *	the spawning of an NQS request are:
 *
 *		../src/nqs_reqser.c	and
 *		../src/nqs_reqexi.c
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
 * REVISION HISTORY: ($Revision: 1.14 $ $Date: 1995/03/29 23:27:59 $ $State: Exp $)
 * REVISION HISTORY: ($Revision: 1.14 $ $Date: 1995/03/29 23:27:59 $ $State: Exp $)
 * $Log: nqs_spawn.c,v $
 * Revision 1.14  1995/03/29  23:27:59  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 12847
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/lib/nqs/nqs_spawn.c
 *
 * Revision 1.13  1995/03/21  22:10:55  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 10507
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_spawn.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.12  1994/11/19  02:53:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/04/06  21:38:29  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s):  macs_blk.c macs_job.c macs_lib.c macs_rootp.c macs_sched.c
 * 	     nqs_spawn.c res_msg.c smd_msg.c
 *
 * Revision 1.9.2.4  1994/03/30  02:01:19  mwan
 * Fixed pts 8088, 8597
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8088, 8597
 *  Testing:
 *  Module(s): usr/lib/nqs/nqs_spawn.c, usr/lib/nqs/macs_lib.c,
 *          usr/lib/nqs/res_msg.c.
 *
 * Revision 1.9.2.3  1994/03/23  17:51:20  mwan
 * Fixed pts 8599,8576,8600,8601.
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8599,8576,8600,8601
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c, usr/include/nqs/buddy.h,
 * 	    usr/lib/nqs/macs_lib.c, usr/lib/nqs/nqs_spawn.c,
 * 	    usr/lib/nqs/macs_sched.c, usr/lib/nqs/macs_job.
 *
 * Revision 1.7  1993/07/27  19:00:49  mwan
 * T11 - This is related to PTS 5893. Set alarm back to zero after catching
 * the second SIGALRM signal.
 *
 * Revision 1.6  1993/07/27  18:40:15  mwan
 * T11 - a quick fix for PTS 5893. Add a alarm call so that the grace time will be based on the wall clock time.
 * This is a quick fix for the problem associated with running interactive
 * applications in the batch partitions.
 *
 * Revision 1.5  1993/07/13  17:51:53  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.7  1993/07/27  19:00:49  mwan
 * T11 - This is related to PTS 5893. Set alarm back to zero after catching
 * the second SIGALRM signal.
 *
 * Revision 1.6  1993/07/27  18:40:15  mwan
 * T11 - a quick fix for PTS 5893. Add a alarm call so that the grace time will be based on the wall clock time.
 * This is a quick fix for the problem associated with running interactive
 * applications in the batch partitions.
 *
 * Revision 1.5  1993/07/13  17:51:53  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.4  1993/05/28  00:20:23  mwan
 * Change to make sure it is a batch queue (and not a pipe queue) before
 * doing rmpart, etc.
 *
 * Revision 1.3  1992/10/17  00:54:08  rkl
 * Change   if (alarm_cnt = 0)   to   if (alarm_cnt == 0)
 *
 * Revision 1.2  1992/10/09  22:26:23  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:36  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:08:14  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_spawn.c	1.2 (nqs_spawn.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>			/* Signal definitions */
#if	UNICOS
#include <sys/category.h>
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "nqsmail.h"			/* NQS mail definitions */
#include "nqsxvars.h"			/* NQS global variables and dirs */
#include "informcc.h"			/* NQS information completion */
					/* codes and masks */
#include "requestcc.h"			/* NQS request completion codes */
#include "transactcc.h"			/* NQS transaction codes */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

#ifdef SDSC
#include "buddyxvar.h"
#include "res.h"
#endif

/*
 *
 *	External functions and variables.
 */
extern int errno;			/* System call error record */
extern void a2s_a2rset();		/* Add to running set */
extern char *asciierrno();		/* Return ASCII version of errno */
extern void closepwdb();		/* Close account/password database */
extern long errnototcm();		/* Convert errno to TCMx_ code */
extern struct passwd *fetchpwuid();	/* Get password entry from database */
extern int getreq();			/* Open/read request header */
extern long mergertcm();		/* Merge TCMx_ with RCM_ code */
extern void nqs_abort();		/* Abort NQS execution */
extern int nqs_enfile();		/* Diagnose file descr shortage */
extern void nqs_reqexi();		/* Request exit */
extern void nqs_reqser();		/* Request server process */
					/* for nqs_spawn */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern void serexit();			/* Server exit procedure */
/*
 * Sprintf() returns an int on System V, a char * on BSD.
 */
extern char *strcpy();			/* String copy */
extern int strlen();			/* String length */
extern int tra_setstate();		/* Set transaction state */
extern void udb_device();		/* Update device file */
extern void udb_qorder();		/* Update queue/ordering file */
extern void udb_queue();		/* Update queue header */

#ifdef SDSC
extern struct job_req *match_req ();
extern int my_sendmsg ();
extern int my_open ();
extern int smd_cancel ();
extern int smd_set_alarm ();
int pid;
struct rawreq *g_rawreq;                /* Raw request structure */
int part_id;
struct job_req *j_req = NULL;
int alarm_cnt = 0;
#endif

/*
 *
 *	Variables local to this module.
 */
static short Shell_execve_failed;	/* Shell execve() failed flag */
static long jid;			/* Server process identifier */

/*** nqs_spawn
 *
 *
 *	void nqs_spawn():
 *
 *	Spawn an NQS request by fork-ing and execve-ing the proper server.
 *	If the particular request is a device request, then a pointer
 *	to the device structure of the device that is to be used to
 *	handle the request, must be supplied as the second argument.
 *	Otherwise, the second argument should be NIL.
 *
 *	The request structure specified by the request argument, MUST
 *	be contained somewhere within the queuedset for its containing
 *	queue.
 *
 *	The NQS database image of any device associated with the request
 *	is properly updated by this function.
 *
 *	The NQS database image of the queue associated with the request
 *	is NOT updated by this function (the QUE_UPDATE bit is set instead).
 */
void nqs_spawn (request, device)
struct request *request;		/* Request to be spawned */
struct device *device;			/* Device (if relevant) */
{
	void shepherdprocess();		/* Server shepherd process */

	/*
	 *  Static declarations.
	 *  --------------------
	 */
	static char *improper = "F$Improper invocation of nqs_spawn().\n";
	static char *internalerror = "I$Internal consistency check error.\n";

	/*
	 *  Local variable declarations.
	 *  ----------------------------
	 */
	struct passwd *passwd;		/* Pointer to password entry */
	struct queue *queue;		/* Queued holding request */
	struct queue *walkqueue;	/* Walk queue structures */
	register struct request *prevreq;
					/* Queue request set walking */
	register struct request *walkreq;
					/* Queue request set walking */
	register int i;			/* Iteration variable */

	/*
	 *  Begin code.
	 *  -----------
	 */
	queue = request->queue;
	if (queue->q.type == QUE_DEVICE) {
		/*
		 *  Spawning a device request.
		 */
		if (device == (struct device *) 0) {
			/*
			 *  The scheduler that called us did not give us
			 *  the device to run the request on.
			 */
			fputs (improper, stdout);
			fputs (internalerror, stdout);
			fputs ("I$No device specified for device request.\n",
				stdout);
			printf ("I$Queue=%s.\n", queue->q.namev.name);
			udb_queue (queue);	/* Update queue */
			nqs_abort();		/* Die */
		}
	}
	else {
		/*
		 *  Spawning a batch, network, or pipe request.
		 */
		if (device != (struct device *) 0) {
			/*
			 *  The scheduler that called us is under the wrong
			 *  impression that batch, network, and pipe requests
			 *  run on a specific device that NQS knows about.
			 *  Wrong!
			 */
			fputs (improper, stdout);
			fputs (internalerror, stdout);
			fputs ("I$Device specified for non-device ", stdout);
			fputs ("batch, network, or pipe request.\n", stdout);
			printf ("I$Queue=%s.\n", queue->q.namev.name);
			errno = 0;		/* Not a system call error */
			nqs_abort();		/* Die */
		}
	}
	/*
	 *  Locate request which must be in the queued set for the specified
	 *  queue.
	 */
	prevreq = (struct request *) 0;
	walkreq = queue->queuedset;
	while (walkreq != (struct request *) 0 && walkreq != request) {
		/*
		 *  Keep looping until we find the specified request
		 *  to spawn.
		 */
		prevreq = walkreq;
		walkreq = walkreq->next;
	}
	if (walkreq == (struct request *) 0) {
		/*
		 *  The scheduler that called us has done something
		 *  quite wrong.
		 */
		fputs (improper, stdout);
		fputs (internalerror, stdout);
		fputs ("I$Request to spawn not found in queued set.\n",
			stdout);
		printf ("I$Queue=%s.\n", queue->q.namev.name);
		errno = 0;			/* Not a system call error */
		nqs_abort();			/* Die */
	}
	/*
	 *  Locate a slot in the Runvars table for the request.
	 */
	i = 0;
	while (i < MAX_GBLRUNLIMIT && Runvars [i].allocated) i++;
	if (i >= MAX_GBLRUNLIMIT) {
		/*
		 *  Egads!  Too many requests are running (or someone
		 *  has updated the Runvars table incorrectly)....
		 */
		fputs ("F$No run slot available to spawn request.\n", stdout);
		fputs (internalerror, stdout);
		errno = 0;			/* Not a system call error */
		nqs_abort();			/* Abort */
	}
	request->reqindex = i;			/* Store req Runvars index */
	/*
	 *  No signals queued for request;  do not requeue if request
	 *  aborts because of signal.
	 */
	request->status &= (~RQF_SIGQUEUED & ~RQF_SIGREQUEUE);
	Runvars [i].queued_signal = 0;		/* No queued signal */
	Runvars [i].process_family = 0;		/* No process family known */
	/*
	 *  Get a pointer to the user's password entry on this machine for
	 *  the home directory, and group-id values.
	 *
	 *  The serious student will note that we do this here (and NOT in
	 *  the child) since we would then have 2 or more processes moving
	 *  the same inherited file pointer around, which in a previous
	 *  incarnation of NQS caused a bug of the most mysterious and
	 *  deadly kind.
	 *
	 *  Remember, Nqs_ldconf() (called in nqs_boot.c) opened the
	 *  password file, and left it open, because we wanted to go fast.
	 */
	passwd = fetchpwuid ((int) request->v1.req.uid);
	fflush (stdout);		/* Flush any diagnostic messages */
	fflush (stderr);		/* BEFORE the fork() */
	if ((Runvars [i].shepherd_pid = fork()) == 0) {
		/*
		 *  We are the NQS request server shepherd process.
		 */
		shepherdprocess (request, passwd, device);
	}
	else if (Runvars [i].shepherd_pid == -1) {
		/*
		 *  The fork failed.  There were not enough
		 *  processes to fork the server process.  DO NOT
		 *  therefore move the request to the run set.
		 *  Let it stay in the queue where it is.  Stop
		 *  ALL NQS queues displaying a panic message and
		 *  let the operator/system-administrator folks take
		 *  appropriate action.
		 */
		printf ("E$Insufficient processes to fork request server.\n");
		printf ("I$Request not spawned.\n");
		printf ("I$Request remaining in queue.\n");
		printf ("I$Stopping all queues to conserve processes.\n");
		fflush (stdout);
		walkqueue = Nonnet_queueset;		/* Non-network queues*/
							/* are stopped first */
		while (walkqueue != (struct queue *) 0) {
			walkqueue->q.status &= ~QUE_RUNNING;
							/* Stop the queue */
			if (walkqueue != queue) {	/* Update queue head */
				udb_queue (walkqueue);	/* if not the request*/
			}				/* queue; */
			walkqueue = walkqueue->next;	/* Advance to next */
		}					/* queue */
		walkqueue = Net_queueset;		/* Network queues */
							/* are stopped last */
		while (walkqueue != (struct queue *) 0) {
			walkqueue->q.status &= ~QUE_RUNNING;
							/* Stop the queue */
			if (walkqueue != queue) {	/* Update queue head */
				udb_queue (walkqueue);	/* if not the request*/
			}				/* queue; */
			walkqueue = walkqueue->next;	/* Advance to next */
		}					/* queue */
		/*
		 *  Update the queue head AND ordering of the specified
		 *  request queue, since one of the spawners: (nqs_bspawn,
		 *  nqs_dspawn, nqs_nspawn, or nqs_pspawn) may have invoked
		 *  us after being invoked from one of request completion
		 *  functions: (nqs_breqcom, nqs_dreqcom, nqs_nreqcom,
		 *  nqs_preqcom).
		 *
		 *  Under these conditions, it is possible that the queue
		 *  image for the specified queue is out of date, since the
		 *  specified queue MIGHT be the one which just had a request
		 *  complete (see nqs_reqcom.c).
		 */
		udb_qorder (queue);		/* Update request queue */
						/* and ordering */
		printf ("I$All queues stopped.\n");
		fflush (stdout);
	}
	else {
		/*
		 *  The server process was successfully spawned.
	 	 *  Allocate the chosen Runvars[] table record.
		 */
		Runvars [request->reqindex].allocated = 1;
		/*
		 *  Record one more running request of the appropriate
		 *  type (if the request is NOT a device request).
		 */
		switch (queue->q.type) {
		case QUE_BATCH:
			Gblbatcount++;		/* One more batch request */
			break;
		case QUE_NET:
			Gblnetcount++;		/* One more network request */
			break;
		case QUE_PIPE:
			Gblpipcount++;		/* One more pipe request */
			break;
		}
		/*
		 *  Remove the request from the queued set.
		 */
		if (prevreq == (struct request *) 0) {
			queue->queuedset = request->next;
		}
		else prevreq->next = request->next;
		queue->q.queuedcount--;		/* One less request waiting */
						/* to execute */
		/*
		 *  Add the running request to the running set.
		 */
		a2s_a2rset (request, queue);
		/*
		 *  Update device state if needed.
		 */
		if (queue->q.type == QUE_DEVICE) {
			device->status |= DEV_ACTIVE;
			device->curreq = request;/* Current request */
			device->curque = queue;	/* Current queue */
			udb_device (device);	/* Update database image */
		}
		/*
		 *  The NQS database queue image is NOT updated, and
		 *  is now out-of-date.
		 */
		queue->q.status |= QUE_UPDATE;	/* Mark for update */
	}
}


/*** shepherdprocess
 *
 *
 *	void shepherdprocess():
 *
 *	We are the request server shepherd process.  We are the direct
 *	child of the NQS daemon.
 */
static void shepherdprocess (req, passwd, device)
struct request *req;			/* Request being spawned */
struct passwd *passwd;			/* Password entry for request */
struct device *device;			/* Device to use (can be NIL) */
{
	void catchshutdown();		/* Catch any SIGTERM from the */
					/* main NQS daemon process */
					/* indicating that NQS is */
					/* shutting down */
	void shellfailed();		/* Catch SIGPIPE as signal used */
					/* to indicate that a shell */
					/* execve() operation failed */
	void stageout();		/* Kludge output return to meet */
					/* deadline pressures */
					/* ***TRIAGE*** */
#ifdef SDSC
        void catchalarm1();             /* send warning */
        void catchalarm2();             /* send SIGUSR1 */
        struct nqs_job_info res_info;
#ifdef RES
#ifndef R1_1
        struct nqs_job_info stResInfoTmp;
        struct job_charge jc;
        time_t timeval;                 /* Current time */
        char sTmp[512];
        int iTmp, filemask;
        int standout, standerr; /* from nqs_reqser.c */
        int mkspool();                  /* Make a spooled output file */
        extern char *namstderr();       /* Return name of temp stderr file */
        extern char *namstdout();       /* Return name of temp stdout file */
#endif
#endif
	int smd_sock;
#endif 

	/*
	 *  Static variables:
	 *  -----------------
	 */
	static char
	emfile_message[] = "I$Request shepherd exiting with RCM_ENFILERUN.\n";

	/*
	 *  Local variables:
	 *  ----------------
	 */
	char script_pathname [MAX_PATHNAME+1];
					/* Shell script pathname for */
					/* request */
	char script_linkname [MAX_PATHNAME+1];
					/* Name used to form */
					/* link to batch request script */
					/* file if necessary */
	struct requestlog requestlog;	/* Mail request log record */
	struct rawreq rawreq;		/* Raw request structure */
	struct transact transaction;	/* Transaction descr for request */
	struct queue *queue;		/* Queue being served */
	int serverstatus;		/* Server completion status */
	int pipefd1 [2];		/* Request completion message pipe */
	int pipefd2 [2];		/* Pipe containing the name of the */
					/* user-process visible shell */
					/* script link name */
	register int i;			/* Index var */
	register int dtablesize;	/* Size of file descriptor table */
	short oldstate;			/* Old request transaction state */
	short needcompletion;		/* Boolean need to read completion */
					/* information from request pipe */
#ifndef SDSC
	int pid;			/* Pid of server process */
#endif
#ifdef SDSC
	int mkpart_retry;
	int alarm_time;
#endif
	int fd;				/* File-descriptor */

	/*
	 *  Begin code:
	 *  -----------
	 *
	 *  Change our presentation in a "ps -f" command invocation
	 *  so that we show up as a shepherd, and not the main NQS
	 *  daemon.
	 *
	 *  Note that we use sprintf() instead of strncpy() to pad
	 *  with spaces, instead of nulls so that the rotten algorithm
	 *  employed by ps(1) will not get lost.
	 */
	sprintf (Argv0, "%-*s", Argv0size, "NQS shepherd");
	/*
	 *  Catch any SIGTERM shutdown signal from the main NQS daemon.
	 */
	signal (SIGTERM, catchshutdown);
	/*
	 *  Close ALL file descriptors, except for the write pipe to the
	 *  local NQS daemon.
	 */
	close (Fromlog);		/* Close connection FROM the NQS */
					/* log process leaving the TO */
					/* connection intact */
	close (Read_fifo);		/* Close the pipe that the local */
					/* NQS daemon reads; this is */
					/* ALWAYS file-descr #0 (see */
					/* nqs_boot.c) */
	/*
	 *  Close all database files.
	 */
	closepwdb();			/* Close password database */
	closedb (Devicefile);		/* Close device descriptor file */
	closedb (Netqueuefile);		/* Close network queue descr file */
	closedb (Queuefile);		/* Close non-network queue descr file*/
	closedb (Qmapfile);		/* Close queue/device/destination */
					/* mapping file */
	closedb (Pipeqfile);		/* Close pipe queue destination file */
	closedb (Paramfile);		/* Close the general parameters file */
	closedb (Mgrfile);		/* Close NQS managers file */
	closedb (Formsfile);		/* Close the forms file */
	/*
	 *  Get pointer to the queue containing this request.
	 */
	queue = req->queue;

	/*
	 * Intergraph addition Bill Mar 	12/08/89
	 * Update the device state.  Because of the fork to
	 * get here, the internal queue structures of the 
	 * shepherd process (the forked child) are no longer
	 * updated and are a static copy.  The device state
	 * must be updated so that it knows what request
	 * it is executing.  This caused a problem in nqs_reqexi
	 * if the exec of the lpserver ever failed because it 
	 * could not find the current request on the device.
	 */
	if (queue->q.type == QUE_DEVICE) {
		device->status |= DEV_ACTIVE;
		device->curreq = req;	/* Current request */
		device->curque = queue; /* Current queue */
	}	/* end of Intergraph addition */
	/*
	 *  Initialize the transaction descriptor for the
	 *  request.
	 */
	transaction.tra_type = TRA_REQSTATE;
	transaction.state = RTS_STASIS;
	transaction.orig_seqno = rawreq.orig_seqno;
	transaction.orig_mid = rawreq.orig_mid;
	transaction.v.events31 = 0;
	/*
	 *  Fill in default values for mail request log record.
	 */
	requestlog.reqrcmknown = 1;	/* Request completion code is known */
	requestlog.svm.mssg [0] = '\0';	/* No server message */
	requestlog.outrcmknown = 0;	/* Output file diposition is */
					/* unknown */
	/*
	 *  Make sure that this request has a password entry on this machine,
	 *  for determining the home directory, and group-id values.
	 */
	if (passwd == (struct passwd *) 0) {
		/*
		 *  When the NQS daemon spawned us, there was no password
		 *  entry for the request.  Use the nqs_reqexi() procedure
		 *  (sending a request completion/deletion message to the
		 *  local NQS daemon) to cause the request to be deleted.
		 */
		requestlog.svm.rcm = RCM_NOACCAUTH;
		nqs_reqexi (req, &transaction, &rawreq, &requestlog, device);
	}
	/*
	 *  Open and read the request header on file descriptor 0.
	 */
	if ((fd = getreq ((long) req->v1.req.orig_seqno, req->v1.req.orig_mid,
			  &rawreq)) == -1) {
		/*
		 *  Couldn't read the request control file.  Fill in the
		 *  fields of the raw request structure used by nqs_reqexi()
		 *  ourselves.
		 */
		rawreq.flags = 0;	/* No special characteristics */
		rawreq.orig_seqno = req->v1.req.orig_seqno;
		rawreq.orig_mid = req->v1.req.orig_mid;
		if (errno == ENFILE) {
			/*
			 *  The system file table is full.
			 *  It is impossible for us to reach
			 *  a per-process file-descr limit
			 *  here, since we just closed a
			 *  whole bunch of files above.
			 */
			requestlog.svm.rcm = RCM_ENFILERUN;
			nqs_reqexi (req, &transaction, &rawreq,
				    &requestlog, device);
		}
		requestlog.svm.rcm = RCM_BADCDTFIL;
		nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
	}
	/*
	 *  The request control file MUST be on file descriptor #0.  This
	 *  fact is used everywhere.
	 */
	if (fd != 0) {			/* Force to stdin */
		fcntl (fd, F_DUPFD, 0);	/* We said close(0) above */
		close (fd);
	}
	/*
	 *  Create the pipe from the server or shell process to the
	 *  shepherd that will be used to send back the request
	 *  completion message.
	 */
	if (pipe (pipefd1) == -1) {	/* Pipe failed */
		if (errno == EMFILE) {
			/*
			 *  We have encountered a per-process open
			 *  file-descriptor limit!
			 */
			nqs_enfile();	/* Display */
			printf (emfile_message);
		}
		requestlog.svm.rcm = RCM_ENFILERUN;
		nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
	}
	/*
	 *  Batch requests require special handling....
	 */
	if (queue->q.type == QUE_BATCH) {
#ifdef SDSC
	    /* Make the partition for the request */
	
	    if (req->v1.req.v2.bat.ncpus > 0) {

		if ((j_req = match_req (req)) == NULL) {
		    printf ("I$shepherdprocess:No matching job_req for req %d\n"
		    , req->v1.req.orig_seqno);
		    fflush (stdout);
		    requestlog.svm.rcm = RCM_UNAFAILURE;
		    nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
		}

		mkpart_retry = 0;
		while (mkpart_retry <= MKPART_RETRY) {
		    if ((part_id = mkpart (j_req)) < 0) {
		        printf ("I$shepherdprocess:Cannot mkpart for req %d\n",
		    	req->v1.req.orig_seqno);
		        fflush (stdout);
			mkpart_retry ++;
			sleep (MKPART_SLEEPT);
			continue;
		    } else {
			printf ("I$part_id=%d, req_id=%d, pid=%d\n", 
			part_id, req->v1.req.orig_seqno, getpid ());
			break;
		    }
		}
		if (part_id < 0) {
		    printf ("I$shepherdprocess:Cannot mkpart for req %d \n"
		    , req->v1.req.orig_seqno);
		    fflush (stdout);
		    requestlog.svm.rcm = RCM_NQSABORT;
		    nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
		}
	    } else {
		j_req = NULL;
	    }
#endif
		/*
		 *  We are spawning a batch request.  Establish a signal
		 *  handler to catch failed execve() operations of the
		 *  batch request shell.
		 */
		Shell_execve_failed = 0;	/* Clear failed execve flag */
		signal (SIGPIPE, shellfailed);	/* Establish handler */
		if (Shell_strategy == SHSTRAT_FREE &&
		    rawreq.v.bat.shell_name [0] == '\0') {
			/*
			 *  Create the second pipe that will be used to
			 *  transmit the name of the user-process visible
			 *  shell script link name to the shell process.
			 */
			if (pipe (pipefd2) == -1) {	/* Pipe failed */
				if (errno == EMFILE) {
					/*
					 *  We have encountered a per-process
					 *  open file-descriptor limit!
					 */
					nqs_enfile();	/* Display */
					printf (emfile_message);
				}
				requestlog.svm.rcm = RCM_ENFILERUN;
				nqs_reqexi (req, &transaction, &rawreq,
					    &requestlog, device);
			}
			/*
			 *  Shell strategy is free.  Let the user's
			 *  login shell choose what shell to use,
			 *  when executing the batch request script.
			 *
			 *  Generate the name for the link that must be
			 *  made to the shell script file for the batch
			 *  request that is to run with a free-shell
			 *  policy.
			 */
			sprintf (script_linkname, "%s/%1d", Nqs_scripts,
				 getpid());
			/*
			 *  Form user-process visible link to shell
			 *  script file.  Note the requirement that
			 *  only one script file exist per batch
			 *  request....
			 */
			pack6name (script_pathname, Nqs_data,
				  (int) (rawreq.orig_seqno % MAX_DATASUBDIRS),
				  (char *) 0, (long) rawreq.orig_seqno, 5,
				  (long) rawreq.orig_mid, 6, 0, 3);
			if (link (script_pathname, script_linkname) == -1) {
				/*
				 *  Link failed!
				 */
				if (errno == ENOSPC) {
					requestlog.svm.rcm = RCM_ENOSPCRUN;
					nqs_reqexi (req, &transaction,
						    &rawreq, &requestlog,
						    device);
				}
				requestlog.svm.rcm = RCM_UNAFAILURE;
				strcpy (requestlog.svm.mssg, asciierrno());
				nqs_reqexi (req, &transaction, &rawreq,
					    &requestlog, device);
			}
			/*
			 *  Write the name of the user-process visible
			 *  shell script link name on the pipe that will
			 *  eventually be read by the shell as the shell
			 *  process' stdin.
			 */
			write (pipefd2 [1], script_linkname,
			       strlen (script_linkname));
			write (pipefd2 [1], "\n", 1);
			close (pipefd2 [1]);
		}
	}
	/*
	 *  Fork() to create the actual server or shell process.
	 */
	fflush (stdout);		/* Flush any diagnostic messages */
	fflush (stderr);		/* BEFORE the fork() */
	if ((pid = fork()) == 0) {
		/*
		 *  We are the server or shell process.  We are the child
		 *  of the shepherd process which is in turn, a child of
		 *  the NQS daemon.
		 */
		if (pipefd1 [1] != 3) {
			/*
			 *  Force the write pipe from the server to the
			 *  shepherd to be on descriptor #3.
			 */
			if (queue->q.type == QUE_BATCH &&
			    Shell_strategy == SHSTRAT_FREE &&
			    rawreq.v.bat.shell_name [0] == '\0' &&
			    pipefd2 [0] == 3) {
				/*
				 *  It is necessary to move the read file
				 *  descriptor for the user-process visible
				 *  shell script link name to a different
				 *  file descriptor.
				 */
				pipefd2 [0] = fcntl (pipefd2 [0], F_DUPFD, 0);
				if (pipefd2 [0] == -1) {
					/*
					 *  We have encountered a
					 *  per-process open
					 *  file-descriptor limit!
					 */
					nqs_enfile();	/* Display */
					printf (emfile_message);
					serexit (RCM_ENFILERUN, (char *) 0);
				}
			}
			close (3);
			fcntl (pipefd1 [1], F_DUPFD, 3);
			close (pipefd1 [1]);/* Close the old write pipe descr*/
		}
		/*
		 *  File descriptors [0..4] must be left open.
		 */
		i = 5;
		/*
		 *  Do different things when spawning a batch request
		 *  versus device, network, and pipe requests.
		 */
		if (queue->q.type == QUE_BATCH) {
			/*
			 *  We are the shell process before the exec.
			 * 
			 *  Change our presentation in a "ps -f" command
			 *  invocation so that we show up as a shell,
			 *  and not a shepherd.
			 *
			 *  Note that we use sprintf() instead of strncpy()
			 *  to pad with spaces, instead of nulls so that
			 *  the rotten algorithm employed by ps(1) will not
			 *  get lost.
			 */
			sprintf (Argv0, "%-*s", Argv0size, "NQS shell");
			if (Shell_strategy == SHSTRAT_FREE &&
			    rawreq.v.bat.shell_name [0] == '\0') {
				/*
				 *  A free shell strategy is in effect.
				 */
				if (pipefd2 [0] != 5) {
					/*
					 *  Force the read file descriptor
					 *  for the user-process visible
					 *  shell script link name to be
					 *  on file descriptor #5.
					 */
					close (5);
					fcntl (pipefd2 [0], F_DUPFD, 5);
					close (pipefd2 [0]);	/* Close old */
								/* descriptor*/
				}
				/*
				 *  File descriptors [0..5] must be left
				 *  open.
				 */
				i = 6;
			}
		}
		else {
			/*
			 *  We are the server for a device, network, or
			 *  pipe request.
			 *
			 *  Change our presentation in a "ps -f" command
			 *  invocation so that we show up as a server,
			 *  and not a shepherd.
			 *
			 *  Note that we use sprintf() instead of strncpy()
			 *  to pad with spaces, instead of nulls so that
			 *  the rotten algorithm employed by ps(1) will not
			 *  get lost.
			 */
			sprintf (Argv0, "%-*s", Argv0size, "NQS server");
		}
		/*
		 *  Close file descriptors [i.._NFILE-1].
		 *  (file descriptors [i..getdtablesize()-1] on Berkeley
		 *   UNIX implementations of NQS).
		 */
#if	BSD42 | BSD43 | ULTRIX
		dtablesize = getdtablesize();	/* #of file descrs per proc */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		dtablesize = _NFILE;		/* #of file descrs per proc */
#else
BAD SYSTEM TYPE
#endif
#endif
		while (i < dtablesize) {
			close (i++);
		}
		/*
		 *  At this point:
		 *	File descriptor 0: Control file (O_RDWR).
		 *	File descriptor 1: Stdout writing to log process.
		 *	File descriptor 2: Stderr writing to log process.
		 *	File descriptor 3: Write file descriptor for sending
		 *			   back request completion messages
		 *			   from the server to the shepherd.
		 *	File descriptor 4: Write file descriptor to NQS daemon
		 *			   request pipe, this is enforced
		 *			   by startup code in nqs_boot.c!
		 *
		 *  If we are spawning a batch request and the shell
		 *  strategy is FREE, then
		 *
		 *	File descriptor 5: Read file descriptor from the
		 *			   shepherd process to the server
		 *			   shell process containing the
		 *			   user-process visible shell
		 *			   script link name.
		 *
		 *  otherwise:
		 *
		 *	File descriptor 5: is closed.
		 *
		 *  In all cases:
		 *
		 *	File descriptors [6.._NFILE-1] are closed.
		 *	(file descriptors [6..getdtablesize()-1] on Berkeley
		 *	 UNIX implementations of NQS).
		 */
		nqs_reqser (req, &rawreq, (req->status & RQF_WASEXE), queue,
#ifndef SDSC
			    device, passwd);
#else
			    device, passwd, j_req);
#endif
	}
	if (pid == -1) {
		/*
		 *  We were unable to fork the server process.
		 */
		if (errno == ENOMEM) requestlog.svm.rcm = RCM_INSUFFMEM;
		else requestlog.svm.rcm = RCM_NOMOREPROC;
		nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
	}
	/*
	 *  We are still the shepherd process.  The fork of the
	 *  server process succeeded.  We must now wait to set
	 *  the transaction state to executing (if necessary),
	 *  and then wait for the server to complete.
	 */
	close (pipefd1 [1]);		/* Close write pipe */
	needcompletion = 1;		/* Set request completion */
					/* information needed flag */
	requestlog.svm.rcm = RCM_NOSVRETCODE;	/* Assume the worst */
	/*
	 *  Read the server message.
	 */
	while (read (pipefd1 [0], (char *) &requestlog.svm,
		     sizeof (requestlog.svm)) == -1 && errno == EINTR);
	jid = requestlog.svm.id;
	oldstate = transaction.state;
	transaction.state = RTS_EXECUTING;
	if (req->status & RQF_WASEXE) {
		/*
		 *  The request is being restarted, its transaction
		 *  state is already set to executing.
		 */
		kill (pid, SIGPIPE);		/* signal server to continue */
	} else {
		/*
		 *  The request has never been executed before.
		 *  Read on the request status pipe for a completion code,
		 *  or a begin execution command.
		 */
		
		if (requestlog.svm.rcm == RCM_EXECUTING) {
			/*
			 *  The request is completely ready to execute.
			 *  Set the request state to executing.
			 */
			if (tra_setstate (req->v1.req.trans_id,
					  &transaction) == -1) {
				/*
				 *  We were unable to successfully set the
				 *  request transaction state to executing.
				 */
				transaction.state = oldstate;
				printf ("E$Unable to set request transaction ");
				printf ("state in shepherdprocess().\n");
				printf ("W$Killing request.\n");
				kill (pid, SIGKILL);
			} else {
				/*
				 *  The request transaction state was
				 *  successfully set to executing.  Signal
				 *  the server process to continue.
				 */
				if (Debug > 2) {
					printf("D$Transaction state set to RTS_EXECUTING\n");
					fflush(stdout);
				}
				kill (pid, SIGPIPE);
			}
		} else {
			/*
			 *  The request never reached the point of being
			 *  ready for executing.  We already have the
			 *  request completion information.
			 */
			needcompletion = 0;
		}
	}
#ifdef SDSC
        g_rawreq = &rawreq;
	sigsetmask (0);
        signal (SIGALRM, catchalarm1);
	if (j_req != NULL) {
	    if ((j_req->start_time > 0) &&
            (j_req->nqs_req->queue->q.priority >= TSCHED_PRI)) {
		alarm_time = j_req->start_time + j_req->req_time - time (0);
		if (alarm_time < 0) 
		    alarm_time = 1;
                alarm (alarm_time);     /* for tenis court schedulling only */
	    } else {
#ifdef PREM_ALARM
                j_req->run_flag = time (0);
#endif

		if (WALL_CLOCK != 0 ) { /* NO_SMD_ALARM */
		  if ((rawreq.v.bat.infinite & LIM_PRCPUT) == 0) {
		    /* the queue time limit is not set to unlimited */
		    if (j_req->req_time > 0) {
		      alarm (j_req->req_time);  
		    }
		  }
		}
 	    }
        } else {
            if (rawreq.v.bat.prcputime.max_seconds > 0) 
                alarm (rawreq.v.bat.prcputime.max_seconds);
        }
#endif 
	while (wait (&serverstatus) == -1 && errno == EINTR)
		;
#ifdef SDSC
#ifdef DEBUG
	printf ("I$shepherd: request %d got through the wait call\n", 
	req->v1.req.orig_seqno);
	fflush (stdout);
#endif
	if (req->v1.req.v2.bat.ncpus > 0 && j_req != NULL &&
	queue->q.type == QUE_BATCH) {
	    if (rmpart_r (j_req->part_name) < 0) {
		printf ("I$shepherd: Unable to rmpart_r %s\n", 
		j_req->part_name);
	    }
	    /* send a message to resd and SMD */

	    if (MACS_FLAG != MACS_OFF) {
	      if (my_open () >= 0) {
	    	res_info.uid = j_req->uid;
	    	res_info.acct_id = rawreq.cube_account;
	    	res_info.part_id = j_req->part_id;
	    	res_info.part_size = j_req->req_nodes;
                res_info.cpu_time = 0;
                res_info.part_active = 0;
                res_info.part_idle = 0;
	    	res_info.event_time = time (0);
	    	strncpy (res_info.queue_name, j_req->que_name, NAME_LEN);
#ifndef R1_1
            res_info.submit_time = rawreq.create_time;
            /* j_req->start_time ? */
            res_info.requested_time = j_req->req_time;
#endif


#ifdef RES
#ifndef R1_1
            bcopy((char *) &res_info, (char *) &stResInfoTmp, sizeof(res_info));
            if (my_sendmsg(NQS_JOB_END, &stResInfoTmp) < 0) {
#else
            if (my_sendmsg(NQS_JOB_END, &res_info) < 0) {
#endif
               printf("I$nqs_spawn: Cannot send msg to MACS\n");
            }			/* end of if (my_sendmsg(NQS_JOB_END,
                                 * &stResInfoTmp) < 0) then */
	    else {
               /* now I can talk to resd */
               bcopy((char *) &stResInfoTmp, (char*) &jc,
                  sizeof(jc));
               /*
                * destroing res_info and res_info HAS to be BIGGER than
                * job_charge
                */

               /* write the accounting  message to  output files */
               time(&timeval);  /* Get current time */
	       /*		
               printf
		("I$nqs_spawn: NQS request finished at %s. cpu_time=%ld, 
		cpu_rate=%f, underused_time=%ld, underused_rate=%f, 
		idle_time=%ld, idle_rate=%f\n", fmttime(&timeval),
                jc.cpu_time,jc.cpu_rate,
                jc.under_used,jc.under_rate,
                jc.idle_time,jc.idle_rate);
                */
            }                   /* end of if (my_sendmsg(NQS_JOB_END,
                                 * &stResInfoTmp) < 0) else */
#endif

	      } else {
		printf ("I$nqs_spawn: Unable to connect to MACS\n");
	      }
	    }
	    /* have to cancel two times */
	    if ((smd_sock = smd_open ()) >= 0) {
	        if (smd_cancel (smd_sock, j_req) < 0)
	    	    printf ("I$nqs_spawn: Cannot send msg to SMD\n");
	        if (smd_cancel (smd_sock, j_req) < 0)
	    	    printf ("I$nqs_spawn: Cannot send msg to SMD\n");
		close (smd_sock);
	    }
	}
#endif

#if	UNICOS
	/*
	 *  Signal any request processes that survived the exit of their
	 *  ancestor server or shell process.
	 */
	if (queue->q.type == QUE_BATCH ||
	    queue->q.type == QUE_DEVICE) {
		killm (C_JOB, jid, Termsignal);
	}
#endif
	if (queue->q.type == QUE_BATCH &&
	    Shell_strategy == SHSTRAT_FREE &&
	    rawreq.v.bat.shell_name [0] == '\0') {
		/*
		 *  Remove visible link to the script-file of the
		 *  batch request, and close the read end of the
		 *  pipe used to transmit the user-process visible
		 *  script file link name to the shell process.
		 */
		unlink (script_linkname);
		close (pipefd2 [0]);	/* Close read end of link name pipe */
	}
	if ((serverstatus & 0xFF) == 0) {
		/*
		 *  Server exited normally via _exit().
		 *  Forward request completion status from
		 *  pipefd1 [0] as reported by the server.
		 *  Shift and mask to get _exit() status.
		 */
		serverstatus = ((serverstatus >> 8) & 0xFF);
		if (needcompletion &&
		    read (pipefd1 [0], (char *) &requestlog.svm,
			  sizeof(requestlog.svm)) != sizeof(requestlog.svm)) {
		    /*
		     *  No completion message was returned.
		     */
		    if (queue->q.type == QUE_BATCH) {
			/*
			 *  A batch request was spawned.
			 */
			if (Shell_execve_failed) {
			    /*
			     *  Aha!  The shell execve() DID
			     *  fail.  Serverstatus has the
			     *  errno code for the execve()
			     *  failure (see ../src/shlexefai.c).
			     */
			    if (serverstatus == E2BIG) {
				/*
				 *  The argument list was
				 *  too big, and will always
				 *  be too big.
				 */
				requestlog.svm.rcm = RCM_SHEXEF2BIG;
			    }
			    else if (serverstatus == ENOMEM) {
				/*
				 *  Memory limit exceeded.
				 */
				requestlog.svm.rcm = RCM_INSUFFMEM;
			    }
			    else if (rawreq.v.bat.shell_name [0] != '\0') {
				/*
				 *  The execve() of the
				 *  user-specified shell
				 *  failed.
				 */
				requestlog.svm.rcm = RCM_USHEXEFAI;
			    }
			    else {
				/*
				 *  The execve() of the
				 *  system chosen shell
				 *  failed.
				 */
				requestlog.svm.rcm = RCM_SSHEXEFAI;
			    }
			    strcpy (requestlog.svm.mssg, asciierrno());
			}
			else {
			    /*
			     *  The shell process created for
			     *  the batch request exited
			     *  normally.
			     */
			    requestlog.svm.rcm = RCM_EXITED;
			    sprintf (requestlog.svm.mssg,
				     "_Exit() value was: %1d.",
				     serverstatus);
			}
		    }
		    else {
			/*
			 *  The server process failed to return a
			 *  completion code message.
			 */
			requestlog.svm.rcm = RCM_NOSVRETCODE;
			requestlog.svm.mssg [0] = '\0';
		    }
		}
		close (pipefd1 [0]);	/* Close completion message pipe */
		if (req->status & RQF_EXPIRED &&
		   (requestlog.svm.rcm & XCI_FULREA_MASK) == RCM_RETRYLATER) {
		    /*
		     *  The request route or delivery time has expired,
		     *  and the server indicates retry.  Convert the
		     *  retry code to the proper expiration code.
		     */
		    if (queue->q.type == QUE_NET) {
			requestlog.svm.rcm = RCM_DELIVEREXP;
		    }
		    else if (queue->q.type == QUE_PIPE) {
			requestlog.svm.rcm = RCM_ROUTEEXP;
		    }
		}
		/*
		 *  Return output files as necessary.
		 */
		stageout (req, passwd, &rawreq, &requestlog);  /****TRIAGE****/
		nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
	}
	close (pipefd1 [0]);
	if ((serverstatus & 0xFF00) == 0) {
		serverstatus &= 0x7F;
		/*
		 *  The server or shell process was terminated via
		 *  the receipt of a signal.
		 *
		 *  No completion message is assumed to have been
		 *  placed in the request completion message pipe.
		 */
		sprintf (requestlog.svm.mssg, "Aborting signal was: %1d.",
			 serverstatus);
		if (queue->q.type == QUE_BATCH ||
		    queue->q.type == QUE_DEVICE) {
			/*
			 *  The server or shell process was for a batch
			 *  or device request.
			 */
			if (Shutdown &&
			   (serverstatus == SIGKILL ||
			    serverstatus == SIGTERM)) {
				if (rawreq.flags & RQF_RESTARTABLE) {
					requestlog.svm.rcm = RCM_SHUTDNREQUE;
				}
				else requestlog.svm.rcm = RCM_SHUTDNABORT;
			}
			else {
				requestlog.svm.rcm = RCM_ABORTED;
				if (serverstatus != SIGKILL &&
				    serverstatus != SIGTERM) {
					/*
					 *  Definitely send mail, because the
					 *  request was killed by a more
					 *  exotic signal that was not a
					 *  SIGKILL or SIGTERM signal.
					 */
					rawreq.flags |= RQF_ENDMAIL;
				}
			}
			if (queue->q.type == QUE_BATCH) {
				/*
				 *  Return output files as necessary.
				 */
				stageout (req, passwd, &rawreq, &requestlog);
							/****TRIAGE****/
			}
		}
		else {
			/*
			 *  The server was for a pipe or network queue.
			 */
			if (serverstatus == SIGKILL) {
				requestlog.svm.rcm = RCM_INTERRUPTED;
			}
			else requestlog.svm.rcm = RCM_SERVESIGERR;
		}
		nqs_reqexi (req, &transaction, &rawreq,
			    &requestlog, device);
	}
	/*
	 *  The server or shell process stopped because of a breakpoint!
	 *  Sorry, but we do not support breakpoints in NQS.  Kill the
	 *  server or shell process group (as best we can).
	 */
#if	UNICOS
	if (queue->q.type == QUE_BATCH ||
	    queue->q.type == QUE_DEVICE) {
		killm (C_JOB, jid, SIGKILL);
	}
#endif
	if (queue->q.type == QUE_BATCH) {
		/*
		 *  The shell process reached a breakpoint!
		 *  Aaaack!
		 */
		if (rawreq.v.bat.shell_name [0] == '\0') {
			requestlog.svm.rcm = RCM_SSHBRKPNT;
		}
		else requestlog.svm.rcm = RCM_USHBRKPNT;
		strcpy (requestlog.svm.mssg,
			"Illegal shell breakpoint reached.  Request killed.");
	}
	else {
		/*
		 *  The server process reached a breakpoint.
		 */
		requestlog.svm.rcm = RCM_SERBRKPNT;
		strcpy (requestlog.svm.mssg,
			"Illegal server breakpoint reached.  Request killed.");
	}
	/*
	 *  Return output files as necessary.
	 */
	stageout (req, passwd, &rawreq, &requestlog);	/****TRIAGE****/
	nqs_reqexi (req, &transaction, &rawreq, &requestlog, device);
}

#ifdef SDSC
 
/*** catchalarm1
 *
 *      void catchalarm1 ();
 *
 *      Catch SIGALRM signal to allow the daemon to kill the request when
 *      the prcpulim expire. Send SIGUSR1 first.
 */
void catchalarm1 ()
{
        void catchalarm2 ();
	int smd_sock;
#ifdef PREM_ALARM
        int alarm_time;
#endif
 
	if (Debug > 2) {
	  if (WALL_CLOCK != 0 ) /* NO_SMD_ALARM */
	    printf ("I$catchalarm1() sending SIGTERM to pid %d (wall clock time used)\n",
		    pid);
	  else
	    printf ("I$catchalarm1() sending SIGTERM to pid %d (SMD time used)\n", 
		    pid);
	  fflush (stdout);
	}

	if (j_req != NULL) {
            if ((j_req->start_time > 0) &&
            (j_req->nqs_req->queue->q.priority >= TSCHED_PRI))
		alarm_cnt = 1;		/* tsched job, skip the alarm */
	}

	if (alarm_cnt == 0) {
#ifdef PREM_ALARM
          if (j_req != NULL && (alarm_time = j_req->run_flag +
          j_req->req_time - time (0)) > 60) {
            signal (SIGALRM, catchalarm1);      /* still the first one */
            printf ("I$PREMATURE ALARM for req %d pid %d. ", 
	    j_req->orig_seqno, getpid ());
            printf ("Job started at %s, ", timstr (j_req->run_flag));
            printf ("got alarm at %s,time requested=%d sec\n",
            timstr (time (0)), j_req->req_time);
            fflush (stdout);
            alarm (alarm_time);
            return;
          } else {
            alarm_cnt ++;
          }
#else
	    alarm_cnt ++;
#endif

	    /* kill parallel apps first */

	    if (j_req != NULL) {
		if (kill_parallel (j_req->part_id, SIGTERM) < 0)
		    printf ("I$none killed in part %d\n", j_req->part_id);
	    }

            kill (-pid, SIGTERM);           /* warn the user first */
            mai_send (g_rawreq, (struct requestlog *) 0, MSX_TIMEOUT);
            signal (SIGALRM, catchalarm2);

/******** This is a quick fix to make sure the second alarm will come.
 ******** Will have to be changed when timesharing is switched on.
 */
            alarm (GRACE_TIME + 10);           
/******** end of quick fix */

	  if (WALL_CLOCK != 0 ) /* NO_SMD_ALARM */
	    alarm (GRACE_TIME);                     /* wait 30 seconds */
	} else {

	    /* kill parallel apps first */

	    if (j_req != NULL) {
		if (kill_parallel (j_req->part_id, Termsignal) < 0)
		    printf ("I$none killed in part %d\n", j_req->part_id);
	    }
            kill (-pid, Termsignal);
	}
}
 
/*** catchalarm2
 *
 *      void catchalarm2 ();
 *
 *      Catch SIGALRM signal to allow the daemon to kill the request when
 *      the prcpulim expire. Now send the SIGKILL signal.
 */
void catchalarm2 ()
{
	if (Debug > 2) {
	  if (WALL_CLOCK != 0 ) /* NO_SMD_ALARM */
	    printf ("I$catchalarm2() sending SIGKILL to pid %d (wall clock time used)\n",
		    pid);
	  else
	    printf ("I$catchalarm2() sending SIGKILL to pid %d (SMD time used)\n", 
		    pid);
	  fflush (stdout);
	}
	kill (-pid, Termsignal);
/******** This is a quick fix to make sure the second alarm will come.
 ******** Will have to be changed when timesharing is switched on.
 */
        alarm (0);            
/******** end of quick fix */
}
 
#endif

/*** catchshutdown
 *
 *
 *	void catchshutdown():
 *
 *	Catch SIGTERM signal delivered from the NQS daemon
 *	process indicating that the NQS daemon is shutting
 *	down.
 */
static void catchshutdown()
{
	signal (SIGTERM, SIG_IGN);	/* Ignore all future SIGTERM */
					/* signals */
	Shutdown = 1;			/* NQS is shutting down */
}


/*** shellfailed
 *
 *
 *	void shellfailed():
 *
 *	Catch SIGPIPE signal indicating that an execve() of
 *	the shell for a batch request has failed.
 */
static void shellfailed ()
{
	signal (SIGPIPE, SIG_IGN);	/* Ignore all future SIGPIPE */
					/* signals */
	Shell_execve_failed = 1;	/* Set flag */
}


/*** stageout
 *
 *
 *	void stageout():			***TRIAGE***
 *
 *	Return any output files for the request.
 *
 *	The reader should know that time pressures made it impossible to
 *	go the full intended distance (using network queues).  Thus, this
 *	module represents an implementation by triage.
 *
 *	We are the request server shepherd process.  We are the direct
 *	child of the NQS daemon.
 */
static void stageout (req, passwd, rawreq, requestlog)
struct request *req;			/* Request being spawned */
struct passwd *passwd;			/* Password entry for request */
register struct rawreq *rawreq;		/* Raw request structure for request */
register struct requestlog *requestlog;	/* Request logfile */
{
	long stageevent();		/* Perform a file stageout event */

	if (req->queue->q.type == QUE_BATCH) {
		/*
		 *  The request is a batch request, and may therefore have
		 *  output files to be returned.
		 */
		if ((rawreq->v.bat.stderr_acc & OMD_SPOOL) &&
		   !(rawreq->v.bat.stderr_acc & OMD_EO)) {
			/*
			 *  A batch request stderr file needs to be
			 *  returned.
			 */
			requestlog->outrcmknown |= (1L << 31);
			requestlog->outrcm [31] = stageevent (req, passwd,
							      rawreq, 31);
		}
		if (rawreq->v.bat.stdout_acc & OMD_SPOOL) {
			/*
			 *  A batch request stdout file needs to be
			 *  returned.
			 */
			requestlog->outrcmknown |= (1L << 30);
			requestlog->outrcm [30] = stageevent (req, passwd,
							      rawreq, 30);
		}
	}
}


/*** stageevent
 *
 *
 *	long stageevent():			***TRIAGE***
 *
 *	Handle a specific file staging event for the parent request.
 *
 *	The reader should know that time pressures made it impossible to
 *	go the full intended distance (using network queues).  Thus, this
 *	module represents an implementation by triage.  We offer our
 *	humble apologies.
 */
static long stageevent (req, passwd, rawreq, eventno)
struct request *req;			/* Request being spawned */
struct passwd *passwd;			/* Password entry for request */
register struct rawreq *rawreq;		/* Raw request structure for request */
int eventno;				/* File staging event# */
{
	struct queue fakenetqueue;	/* Fake network queue being served */
	struct request fakesubrequest;	/* Fake output event subrequest */
	struct servermssg servermessage;/* Simulated network queue server */
					/* completion message */
	int serverstatus;		/* Server completion status */
	int pipefd [2];			/* Request completion message pipe */
	register int i;			/* Index var */
	int pid;			/* Pid of server process */

	/*
	 *  Build fake output file staging event subrequest.
	 */
	fakesubrequest.status = RQF_SUBREQUEST;
	fakesubrequest.reqindex = req->reqindex;
					/* Same slot as parent */
	fakesubrequest.start_time = 0;	/* No delay */
	fakesubrequest.next = (struct request *) 0;
	fakesubrequest.queue = &fakenetqueue;
	fakesubrequest.v1.subreq.event = eventno;
	fakesubrequest.v1.subreq.sibling = (struct request *) 0;
	fakesubrequest.v1.subreq.parent = req;
	/*
	 *  Initialize the fields of the fake network queue that will
	 *  be examined by nqs_reqser().
	 */
	fakenetqueue.q.type = QUE_NET;
	if (eventno == 30) {		/* Stdout event #30 */
		fakenetqueue.q.namev.to_destination = rawreq->v.bat.stdout_mid;
	}
	else {				/* Stderr event #31 */
		fakenetqueue.q.namev.to_destination = rawreq->v.bat.stderr_mid;
	}
	strcpy (fakenetqueue.q.v1.network.server, Netclient);
	/*
	 *  Create the pipe from the network server to the shepherd
	 *  that will be used to send back the request completion message.
	 */
	if (pipe (pipefd) == -1) {	/* Egads! */
		return (mergertcm (RCM_STAGEOUTFAI, errnototcm()));
	}
	/*
	 *  Fork() to create the actual server or shell process.
	 */
	fflush (stdout);		/* Flush any diagnostic messages */
	fflush (stderr);		/* BEFORE the fork() */
	if ((pid = fork()) == 0) {
		/*
		 *  We are the simulated network queue server process.
		 *  We are the child of the shepherd process which is
		 *  in turn, a child of the NQS daemon.
		 *
		 *  Change our presentation in a "ps -f" command
		 *  invocation so that we show up as an NQS netclient,
		 *  and not as an NQS shepherd process.
		 *
		 *  Note that we use sprintf() instead of strncpy()
		 *  to pad with spaces, instead of nulls so that
		 *  the rotten algorithm employed by ps(1) will not
		 *  get lost.
		 */
		sprintf (Argv0, "%-*s", Argv0size, "NQS netclient");
		if (pipefd [1] != 3) {
			/*
			 *  Force the write pipe from the server to the
			 *  shepherd to be on descriptor #3.
			 */
			close (3);
			fcntl (pipefd [1], F_DUPFD, 3);
			close (pipefd [1]);/* Close the old write pipe descr*/
		}
		/*
		 *  Close file descriptors [5.._NFILE-1].
		 *  (file descriptors [5..getdtablesize()-1] on Berkeley
		 *   UNIX implementations).
		 *
		 *  File descriptors [0..4] must be left open.
		 */
#if	BSD42 | BSD43 | ULTRIX
		i = getdtablesize();	/* #of file descrs per proc */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		i = _NFILE;		/* #of file descrs per proc */
#else
BAD SYSTEM TYPE
#endif
#endif
		while (--i >= 5) close (i);
		/*
		 *  At this point:
		 *	File descriptor 0: Control file (O_RDWR).
		 *	File descriptor 1: Stdout writing to log process.
		 *	File descriptor 2: Stderr writing to log process.
		 *	File descriptor 3: Write file descriptor for sending
		 *			   back request completion messages
		 *			   from the server to the shepherd.
		 *	File descriptor 4: Write file descriptor to NQS daemon
		 *			   request pipe, this is enforced
		 *			   by startup code in nqs_boot.c!
		 *	File descriptors [5.._NFILE-1] are closed.
		 *	(file descriptors [5..getdtablesize()-1] on Berkeley
		 *	 UNIX implementations).
		 */
		nqs_reqser (&fakesubrequest, rawreq, 0, &fakenetqueue,
#ifndef SDSC
			   (struct device *) 0, passwd);
#else
			   (struct device *) 0, passwd, (struct job_req *) 0);
#endif
	}
	if (pid == -1) {
		/*
		 *  We were unable to fork the server process.
		 */
		return (mergertcm (RCM_STAGEOUTFAI, errnototcm()));
	}
	/*
	 *  We are still the shepherd process.  The fork of the
	 *  server process succeeded.  We must now wait for the
	 *  simulated network queue server to complete.
	 */
	close (pipefd [1]);		/* Close write pipe */
	while (wait (&serverstatus) == -1 && errno == EINTR)
		;
	if ((serverstatus & 0xFF) == 0) {
		/*
		 *  Server exited normally via _exit().
		 *  Forward request completion status from
		 *  pipefd [0] as reported by the server.
		 *  Shift and mask to get _exit() status.
		 */
		if (read (pipefd [0], (char *) &servermessage,
			  sizeof (servermessage)) != sizeof (servermessage)) {
			/*
			 *  No completion message was returned.
			 *  The server process failed to return a
			 *  completion code message.
			 */
			servermessage.rcm = mergertcm (RCM_STAGEOUTFAI,
						       TCML_INTERNERR);
		}
		close (pipefd [0]);	/* Close completion message pipe */
		return (servermessage.rcm);
	}
	close (pipefd [0]);
	if ((serverstatus & 0xFF00) == 0) {
		/*
		 *  The server or shell process was terminated via
		 *  the receipt of a signal.
		 *
		 *  No completion message is assumed to have been
		 *  placed in the request completion message pipe.
		 *
		 *	WHEN NETWORK QUEUES ARE IMPLEMENTED, THIS
		 *	ENTIRE TRIAGE IMPLEMENTATION WILL BE JUNKED,
		 *	AND THE LOGIC FOR PIPE AND NETWORK QUEUE
		 *	SERVERS UPON SIGNAL DEATH WILL LOOK LIKE
		 *	THE FOLLOWING:
		 *
		 *
		 *		if (serverstatus == SIGKILL) {
		 *			return (RCM_INTERRUPTED);
		 *		}
		 *		else return (RCM_SERVESIGERR);
		 *
		 *
		 *	UNTIL THAT DAY HOWEVER, WE ARE FORCED TO
		 *	RETURN RCM_STAGEOUTFAI, SINCE THERE IS NO
		 *	QUEUE IN WHICH TO REQUEUE STAGEOUT EVENTS
		 *	IN THIS TIME FORCED TRIAGE IMPLEMENTATION.
		 */
		return (mergertcm (RCM_STAGEOUTFAI, TCML_FATALABORT));
	}
	/*
	 *  The simulated network queue server process reached a breakpoint.
	 */
	return (mergertcm (RCM_STAGEOUTFAI, TCML_INTERNERR));
}
