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
 
/*++ nqs_rbuild.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_rbuild.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Rebuild the NQS state after a system crash, or an orderly
 *	shutdown.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:09 $ $State: Exp $)
 * $Log: nqs_rbuild.c,v $
 * Revision 1.3  1994/11/19  02:53:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:04  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:25  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:38  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_rbuild.c	1.2 (nqs_rbuild.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"			/* Include NQS constants/types	*/
#include "nqsmail.h"			/* NQS mail definitions */
#include "informcc.h"			/* NQS information completion codes */
					/* and masks */
#include "requestcc.h"			/* NQS request completion codes */
#include "transactcc.h"			/* NQS transaction completion codes */
#include "nqsxvars.h"			/* NQS global variables and */
					/* directories */
#if	SGI | SYS52 | UTS | UNICOS | OSF
#include "dir.4.2.h"			/* Include directory walking defs */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/dir.h>			/* Include directory walking defs */
#else
BAD SYSTEM TYPE
#endif
#endif
#define	NOT_A_NEW_REQ	0		/* Requests being requeued are not */
					/* new reqs, they were there before */


/*
 *	External functions:
 */
extern unsigned long a6btoul();		/* Convert 6-bit alphabet name to */
					/* numeric equivalent */
extern char *asciierrno();		/* Return ASCII errno */
extern void bsc_spawn();		/* Maybe spawn a batch req */
extern void dsc_spawn();		/* Maybe spawn a device req */
extern int errno;			/* System call error# */
extern char *fmtmidname();		/* Format name for machine-id */
extern int getreq();			/* Get/read a raw request */
extern int is6bitstr();			/* Is 6-bit alphabet string bool fn */
extern int mai_send();			/* Send request mail */
extern char *namstderr();		/* Name batch request spooled stderr */
extern char *namstdlog();		/* Name batch request spooled log */
extern char *namstdout();		/* Name batch request spooled stdout */
extern void nqs_abort();		/* Abort NQS execution */
extern void nqs_delrfs();		/* Delete the specified control */
					/* and data request files */
extern int nqs_enfile();		/* Diagnose open error returning */
					/* non-zero if too many file */
					/* descriptors were open causing */
					/* the last open to fail; otherwise */
					/* 0 is returned. */
extern void nqs_failed();		/* Unlink request file from control */
					/* directory, and place it in the */
					/* failed directory for bad */
					/* requests */
extern struct request *nqs_fndreq();	/* Find request by original seq# */
					/* and originating machine-id */
extern long nsq_enque();		/* Insert request into queue */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern void psc_spawn();		/* Maybe spawn a pipe req */
extern int strcmp();			/* String comparison */
extern int strlen();			/* Return string length */
extern int tid_allocate();		/* Allocate a transaction-id */
extern int tra_read();			/* Read transaction descriptor */
extern int tra_release();		/* Release transaction descriptor */
extern void udb_device();		/* Update device state */
extern void udb_qorder();		/* Update a queue and ordering */
extern int writehdr();			/* Update common request header */

/*
 *	Static vars local to this module.
 */
static char rbuildsuffix[] = " error in nqs_rbuild().\n";
static char inforeason[] = "I$%s.\n";
static char infotid[] = "I$Transaction-id: %1d.\n";


/*** nqs_rbuild
 *
 *
 *	void nqs_rbuild():
 *
 *	This function is invoked to rebuild the NQS state after
 *	a system crash, or an orderly shutdown.
 *
 *	This function unlinks ALL of the new request files in the
 *	temporary locally originated new request directory and all
 *	of the queue ordering files.
 *
 *	The locally originated new request files in the temporary
 *	locally originated new request directory are unlinked
 *	because we do not know if the system crashed in the middle
 *	of creating a control or data file (hence an incomplete
 *	control/data file).
 *
 *	The queue ordering files are scrapped because we are going
 *	to rebuild all queues from the valid requests in the NQS
 *	control directory.
 *
 *	The caller must be running with the current directory
 *	of:  Nqs_root.
 */
void nqs_rbuild()
{
	void requeuerequest();		/* Requeue a request */

	struct transact transact [MAX_TRANSACTS];
					/* All transaction descriptors */
	char path [MAX_PATHNAME+1];	/* Pathname */
	char filepath [MAX_PATHNAME+1];	/* Another pathname */
	DIR *dir;			/* Open queue order dir structure */
	struct direct *dirent;		/* Ptr to directory entry */
	struct rawreq rawreq;		/* Ptr to raw request */
	struct queue *queue;		/* Ptr to a queue structure */
	struct device *device;		/* Ptr to a device structure */
	struct request *predecessor;	/* Ptr to request predecessor */
	struct request *request;	/* Ptr to a request structure */
	long orig_seqno;		/* Sequence# */
	mid_t orig_mid;			/* Machine-id */
	register int tidminus1;		/* Transaction-id - 1 */
	char suffixchar;		/* Output file suffix character */
	int datano;			/* Data file# */
	int fd;				/* Control file file-descriptor */
	int state;			/* Request state:  RQS_ */
	int i;				/* Loop iteration var */

	/*
	 *  Discard all requests in the new request staging directory.
	 *  The system may have crashed leaving garbage files in the
	 *  new requests directory.
	 */
	if ((dir = opendir (Nqs_requests)) == (DIR *) 0) {
		printf ("F$Unable to open new request directory.\n");
		nqs_abort();
	}
	while ((dirent = readdir (dir)) != (struct direct *) 0) {
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
		if (!reserved (dirent->d_name) &&
		    strcmp (dirent->d_name, Nqs_fifo)) {
#else
#if	BSD42 | BSD43
		if (!reserved (dirent->d_name)) {
#else
BAD SYSTEM TYPE
#endif
#endif
			/*
			 *  We have another new request NQS control/data
			 *  file to delete.
			 */
			sprintf (path, "%s/%s", Nqs_requests, dirent->d_name);
			unlink (path);
		}
	}
	closedir (dir);			/* Close the locally originated */
					/* new request staging directory */
	/*
	 *  Discard any and all left-over inter-process communication
	 *  files.
	 */
	if ((dir = opendir (Nqs_inter)) == (DIR *) 0) {
		printf ("F$Unable to open inter-");
		printf ("process communication directory.\n");
		nqs_abort();
	}
	while ((dirent = readdir (dir)) != (struct direct *) 0) {
		if (!reserved (dirent->d_name)) {
			/*
			 *  We have another inter-process communication
			 *  file to delete.
			 */
			sprintf (path, "%s/%s", Nqs_inter, dirent->d_name);
			unlink (path);
		}
	}
	closedir (dir);			/* Close the inter-process */
					/* communication directory */
	/*
	 *  Read the state of ALL NQS transaction descriptors on the
	 *  local machine.
	 *
	 *  The reader will note that we read transaction descriptors
	 *  in a rather odd way.  There is a method to our madness.
	 *  We're attempting to read all transaction descriptors in
	 *  the SAME directory before moving on to the next transaction
	 *  directory in an effort to squeeze a bit more alacrity out
	 *  of a process that is very disk I/O intensive.
	 */
	for (i = 0; i < MAX_TRANSACTS && i < MAX_TDSCSUBDIRS; i++) {
		for (tidminus1 = i; tidminus1 < MAX_TRANSACTS;
		     tidminus1 += MAX_TDSCSUBDIRS) {
			if (tra_read (tidminus1+1,
				      &transact [tidminus1]) == -1) {
				/*
				 *  Error reading transaction tidminus1.
				 */
				printf ("F$Tra_read()");
				printf (rbuildsuffix);
				printf (inforeason, asciierrno());
				printf (infotid, tidminus1+1);
				nqs_abort();	/* Abort execution */
			}
		}
	}
	/*
	 *  All transaction descriptors have now been read.
	 *
	 *  Discard all queue ordering files to prepare for rebuild.
	 *  The queue files will be rebuilt from scratch from the
	 *  control files in the control directory.
	 */
	if ((dir = opendir (Nqs_qorder)) == (DIR *) 0) {
		printf ("F$Unable to open queue ordering directory.\n");
		nqs_abort();
	}
	while ((dirent = readdir (dir)) != (struct direct *) 0) {
		if (!reserved (dirent->d_name)) {
			/*
			 *  We have another queue ordering file to delete.
			 */
			sprintf (path, "%s/%s", Nqs_qorder, dirent->d_name);
			unlink (path);
		}
	}
	closedir (dir);			/* Close the queue order directory */
	/*
	 *  Discard any and all left-over links to shell script
	 *  files created for previously spawned batch requests.
	 */
	if ((dir = opendir (Nqs_scripts)) == (DIR *) 0) {
		printf ("F$Unable to open shell-script link directory.\n");
		nqs_abort();
	}
	while ((dirent = readdir (dir)) != (struct direct *) 0) {
		if (!reserved (dirent->d_name)) {
			/*
			 *  We have another shell-script link to delete.
			 */
			sprintf (path, "%s/%s", Nqs_scripts, dirent->d_name);
			unlink (path);
		}
	}
	closedir (dir);			/* Close the shell-script link */
					/* directory */
	/*
	 *  Rebuild request queues from control files.
	 */
	for (i = 0; i < MAX_CTRLSUBDIRS; i++) {
		pack6name (path, Nqs_control, (int) i, (char *) 0,
			   0L, 0, 0L, 0, 0, 0);
		if ((dir = opendir (path)) == (DIR *) 0) {
			printf ("F$Unable to open the directory: %s.\n", path);
			nqs_abort();
		}
		while ((dirent = readdir (dir)) != (struct direct *) 0) {
			if (!reserved (dirent->d_name)) {
				/*
				 *  The control file name is not "." or "..".
				 *  Process yet another control file.  Note
				 *  that errno is set to 0 by openreq() in
				 *  the absence of an error....
				 */
				fd = openreq (dirent->d_name, &rawreq);
				if (fd >= 0) {
					/*
					 *  This control file appears to be
					 *  complete, and has a sequence number
					 *  assigned to it.
					 */
					requeuerequest (fd, &rawreq, transact);
				}
				else if (nqs_enfile()) {
					/*
					 *  We are suffering from a lack of
					 *  file descriptors.
					 */
					printf ("F$Cannot recover from ");
					printf ("previous error.\n");
					nqs_abort();	/* Die, since we */
							/* could not open a */
							/* control file */
				}
				else {
					/*
					 *  This control file is damaged or
					 *  bogus.  Discard it.
					 */
					printf ("W$Unable to requeue bad ");
					printf ("request: %s.\n",
						dirent->d_name);
					printf ("W$%s being discarded.\n",
						dirent->d_name);
					sprintf (filepath, "%s/%s", path,
						 dirent->d_name);
					unlink (filepath);
					printf ("W$No mail sent to owner.\n");
					fflush (stdout);
				}
				close (fd);	/* Close the control file */
			}
		}	
		closedir (dir);		/* Close the control file directory */
	}
	/*
	 *  Traverse the request data file directories, making sure
	 *  that each data file is the "child" of a control file.
	 *  If an orphaned data file is found, then the data file
	 *  must be deleted.
	 */
	for (i = 0; i < MAX_DATASUBDIRS; i++) {
		pack6name (path, Nqs_data, (int) i, (char *) 0,
			   0L, 0, 0L, 0, 0, 0);
		if ((dir = opendir (path)) == (DIR *) 0) {
			printf ("F$Unable to open the directory: %s.\n", path);
			nqs_abort();
		}
		while ((dirent = readdir (dir)) != (struct direct *) 0) {
			/*
			 *  Make sure this data file is "owned" by a parent
			 *  control file.
			 */
			if (!reserved (dirent->d_name)) {
				/*
				 *  The data file name is not "." or "..".
				 */
				request = (struct request *) 0;
				if (strlen (dirent->d_name) == 14 &&
				    is6bitstr (dirent->d_name, 14)) {
					orig_seqno = a6btoul (dirent->d_name,
							      5);
					orig_mid = (mid_t)
						    a6btoul (dirent->d_name+5,
							     6);
					datano = a6btoul (dirent->d_name + 11,
							  3);
					/*
					 *  The request can be queued in any
					 *  state.
					 */
					state = RQS_DEPARTING | RQS_RUNNING
					      | RQS_STAGING | RQS_QUEUED
					      | RQS_WAITING | RQS_HOLDING
					      | RQS_ARRIVING;
					request = nqs_fndreq (orig_seqno,
							      orig_mid,
							      &predecessor,
							      &state);
				}
				if (request == (struct request *) 0 ||
				    request->v1.req.ndatafiles < datano) {
					/*
					 *  This data file has no parent.
					 */
					printf ("E$Data file: %s ",
						dirent->d_name);
					printf ("has no parent ctrl file.\n");
					printf ("W$Data file: %s ",
						dirent->d_name);
					printf ("being discarded.\n");
					fflush (stdout);
					sprintf (filepath, "%s/%s", path,
						 dirent->d_name);
					unlink (filepath);
				}
			}
		}
		closedir (dir);		/* Close the data file directory */
	}
	/*
	 *  Traverse the request output file directories, making sure
	 *  that each output file is the "child" of a control file.
	 *  If an orphaned output file is found, then the output file
	 *  must be deleted.
	 */
	for (i = 0; i < MAX_OUTPSUBDIRS; i++) {
		pack6name (path, Nqs_output, (int) i, (char *) 0,
			   0L, 0, 0L, 0, 0, 0);
		if ((dir = opendir (path)) == (DIR *) 0) {
			printf ("F$Unable to open the directory: %s.\n", path);
			nqs_abort();
		}
		while ((dirent = readdir (dir)) != (struct direct *) 0) {
			/*
			 *  Make sure this output file is "owned" by a parent
			 *  control file.
			 */
			if (!reserved (dirent->d_name)) {
				/*
				 *  The output file name is not "." or "..".
				 */
				request = (struct request *) 0;
				suffixchar = dirent->d_name [11];
				if (strlen (dirent->d_name) == 12 &&
				    is6bitstr (dirent->d_name, 11) &&
				    (suffixchar == 'e' || suffixchar == 'l' ||
				     suffixchar == 'o')) {
					orig_seqno = a6btoul (dirent->d_name,
							      5);
					orig_mid = (mid_t)
						    a6btoul (dirent->d_name+5,
							     6);
					/*
					 *  The request can be queued in any
					 *  state, except arriving.
					 */
					state = RQS_DEPARTING | RQS_RUNNING
					      | RQS_STAGING | RQS_QUEUED
					      | RQS_WAITING | RQS_HOLDING;
					request = nqs_fndreq (orig_seqno,
							      orig_mid,
							      &predecessor,
							      &state);
				}
				if (request == (struct request *) 0) {
					/*
					 *  This output file has no parent.
					 */
					printf ("E$Output file: %s ",
						dirent->d_name);
					printf ("has no parent ctrl file.\n");
					printf ("W$Output file: %s ",
						dirent->d_name);
					printf ("being discarded.\n");
					fflush (stdout);
					sprintf (filepath, "%s/%s", path,
						 dirent->d_name);
					unlink (filepath);
				}
			}
		}
		closedir (dir);		/* Close the output file directory */
	}
	/*
	 *  Traverse the transaction descriptor array, discarding
	 *  transaction descriptors allocated to non-existent requests,
	 *  a condition which can occur when system administrator and
	 *  heaven forbid--developers--manually alter the NQS database.
	 */
	for (tidminus1 = 0; tidminus1 < MAX_TRANSACTS; tidminus1++) {
		if (transact [tidminus1].tra_type != TRA_UNALLOCATED) {
			/*
			 *  We have paired all existing NQS requests with their
			 *  associated transaction descriptor, and find that
			 *  this transaction descriptor is allocated for a non-
			 *  existent request.
			 *
			 *  Release the transaction descriptor.
			 */
			printf ("W$Releasing orphaned transaction ");
			printf ("descriptor: %1d.\n", tidminus1+1);
			fflush (stdout);
			if (tra_release (tidminus1+1) == -1) {
				/*
				 *  Error releasing the transaction descriptor.
				 */
				printf ("F$Tra_release()");
				printf (rbuildsuffix);
				printf (inforeason, asciierrno());
				printf (infotid, tidminus1+1);
				nqs_abort();		/* Abort execution */
			}
		}
	}
	/*
	 *  Record that NQS is booted.  Disabled queues are now truly
	 *  disabled.
	 */
	Booted = 1;			/* Make it impossible to place a */
					/* request in a disabled queue */
	/*
	 *  Update all device states to show inactive in case system
	 *  crashed leaving erroneous device states.
	 */
	device = Devset;
	while (device != (struct device *) 0) {
		device->status &= ~DEV_ACTIVE;	/* Clear device active */
		udb_device (device);		/* Write-out device state */
		device = device->next;
	}
	/*
	 *  Show all queues as requiring updates of their corresponding
	 *  database image.
	 */
	queue = Nonnet_queueset;		/* Non-network queues */
	while (queue != (struct queue *) 0) {
		queue->q.status |= QUE_UPDATE;	/* Set update bit */
		queue = queue->next;
	}
	queue = Net_queueset;			/* Network queues */
	while (queue != (struct queue *) 0) {
		queue->q.status |= QUE_UPDATE;	/* Set update bit */
		queue = queue->next;
	}
	/*
	 *  Spawn requests that can run (updating altered queue files
	 *  accordingly).
	 */
	bsc_spawn();		/* Spawn batch reqs as appropriate */
	dsc_spawn();		/* Spawn device reqs as appropriate */
	psc_spawn();		/* Spawn pipe queue reqs as appropriate */
	/*
	 *  Update all queue images in the NQS database that are out of
	 *  date (those not updated by the spawning activity).  Please
	 *  remember that udb_qorder() clears the QUE_UPDATE bit.
	 */
	queue = Nonnet_queueset;		/* Non-network queues */
	while (queue != (struct queue *) 0) {
		if (queue->q.status & QUE_UPDATE) udb_qorder (queue);
		queue = queue->next;
	}
	queue = Net_queueset;			/* Network queues */
	while (queue != (struct queue *) 0) {
		if (queue->q.status & QUE_UPDATE) udb_qorder (queue);
		queue = queue->next;
	}
}


/*** deleteoutput
 *
 *
 *	void deleteoutput():
 *	Delete all request output files.
 */
static void deleteoutput (rawreq)
register struct rawreq *rawreq;
{
	unlink (namstderr (rawreq->orig_seqno, rawreq->orig_mid));
	unlink (namstdlog (rawreq->orig_seqno, rawreq->orig_mid));
	unlink (namstdout (rawreq->orig_seqno, rawreq->orig_mid));
}


/*** deleterequest
 *
 *
 *	void deleterequest():
 *	Delete a request that cannot be requeued.
 */
static void deleterequest (rawreq, transact)
register struct rawreq *rawreq;
register struct transact transact [MAX_TRANSACTS];
{
	register short trans_id;	/* Transaction-id of request */

	trans_id = rawreq->trans_id;
	if (trans_id) {
		/*
		 *  Release the transaction descriptor/tid associated
		 *  with the request.
		 */
		if (tra_release (trans_id) == -1) {
			/*
			 *  Error releasing the transaction descriptor/tid
			 *  associated with the request.
			 */
			printf ("F$Tra_release()");
			printf (rbuildsuffix);
			printf (inforeason, asciierrno());
			printf (infotid, trans_id);
			nqs_abort();		/* Abort execution */
		}
		transact [trans_id-1].tra_type = TRA_UNALLOCATED;
	}
	/*
	 *  Delete any request stderr, stdlog, or stdout output files.
	 */
	deleteoutput (rawreq);
	/*
	 *  Delete all remaining files associated with the request.
	 */
	nqs_delrfs (rawreq->orig_seqno, rawreq->orig_mid, rawreq->ndatafiles);
}


/*** openreq
 *
 *
 *	int openreq():
 *
 *	Check to see if the specified control file name is valid.
 *	If it is, then try to open it.
 *
 *	Returns:
 *		>= 0 as the file descriptor of the open control file
 *		     if successful (with rawreq loaded from the control
 *		     file header;
 *		-1   otherwise.
 */
static int openreq (name, rawreq)
char *name;				/* Unqualified name of ctrl name */
struct rawreq *rawreq;			/* Raw request structure */
{
	if (strlen (name) == 11 && is6bitstr (name, 11)) {
	    /*
	     *  The name is valid.
	     */
	    return (getreq ((long) (a6btoul (name, 5)),
			    (mid_t) (a6btoul (name+5, 6)),
			    rawreq)
		   );
	}
	return (-1);
}


/*** requeuerequest
 *
 *
 *	void requeuerequest():
 *	Requeue a request.
 */
static void requeuerequest (fd, rawreq, transact)
int fd;					/* Control file file descriptor */
register struct rawreq *rawreq;		/* Rawreq structure for request */
register struct transact transact [MAX_TRANSACTS];
{
	struct requestlog requestlog;	/* Mail request log record */
	register short tidminus1;	/* Transaction-id - 1 */
	register short mode;		/* Queueing mode */
	register short wasexe;		/* Was executing flag */
	register time_t prearrive;	/* Pre-arrive transaction time */

	if (rawreq->trans_id == 0) {
		/*
		 *  The system crashed before the transaction descriptor/tid
		 *  allocated to the request could be recorded in the control
		 *  file of the request.
		 *
		 *  Note that if the request was in the process of being
		 *  deleted at the time of a system crash, then no transaction
		 *  descriptor/tid will be allocated to the request.
		 *
		 *  Paw through the set of ALL transaction descriptors looking
		 *  for the one allocated to this request (if one is indeed
		 *  allocated).
		 */
		tidminus1 = 0;
		while (tidminus1 < MAX_TRANSACTS &&
		      (transact [tidminus1].tra_type == TRA_UNALLOCATED ||
		       transact [tidminus1].orig_seqno != rawreq->orig_seqno ||
		       transact [tidminus1].orig_mid != rawreq->orig_mid)) {
			/*
			 *  No transaction descriptor/tid has yet been found
			 *  for the request.  Keep looking.
			 */
			tidminus1++;
		}
		if (tidminus1 >= MAX_TRANSACTS) {
			/*
			 *  No transaction descriptor/tid is presently
			 *  allocated to the request.  Therefore, the
			 *  request must have been in the process of
			 *  being deleted at the time of a system crash.
			 */
			deleterequest (rawreq, transact);
						/* Delete the request */
			return;			/* Return to caller */
		}
		/*
		 *  The transaction descriptor/tid associated with the
		 *  request has been found.  Update the request control
		 *  file accordingly.
		 */
		rawreq->trans_id = tidminus1 + 1;
		writehdr (fd, rawreq);
	}
	else {
		/*
		 *  A transaction descriptor/tid is recorded as being
		 *  allocated to this request.
		 *
		 *  Note that if the request was in the process of being
		 *  deleted at the time of a system crash, then no transaction
		 *  descriptor/tid will be allocated to the request.
		 */
		tidminus1 = rawreq->trans_id - 1;
		if (transact [tidminus1].tra_type == TRA_UNALLOCATED ||
		    transact [tidminus1].orig_seqno != rawreq->orig_seqno ||
		    transact [tidminus1].orig_mid != rawreq->orig_mid) {
			/*
			 *  The request was in the process of being
			 *  deleted.  Set the delete flag.
			 */
			rawreq->trans_id = 0;	/* No tid */
			deleterequest (rawreq, transact);
						/* Delete the request */
			return;			/* Return to caller */
		}
	}
	/*
	 *  The transaction-id/descriptor for the request has been
	 *  established.
	 *
	 *  The variable:  tidminus1  holds the transact[] offset of
	 *  the transaction descriptor allocated to the request.
	 */
	if (!(rawreq->flags & RQF_RESTARTABLE) &&
	    transact [tidminus1].state == RTS_EXECUTING) {
		/*
		 *  A non-restartable request was running at the time
		 *  of a crash or system shutdown.
		 */
		requestlog.reqrcmknown = 1;
		requestlog.svm.rcm = RCM_NORESTART;
		requestlog.svm.mssg [0] = '\0';
		requestlog.outrcmknown = 0;
		printf ("I$Unrestartable request: %1ld.%s being deleted.\n",
		       (long) rawreq->orig_seqno,
			fmtmidname (rawreq->orig_mid));
		fflush (stdout);
		/*
		 *  Send mail to the user.
		 */
		if (mai_send (rawreq, &requestlog, MSX_DELETED) == -1) {
			printf ("I$No mail ");
		}
		else printf ("I$Mail ");
		printf ("sent to owner.\n");
		fflush (stdout);
		deleterequest (rawreq, transact);
						/* Delete the request */
		return;				/* Return to caller */
	}
	/*
	 *  Requeue the request.
	 *
	 *  A more robust implementation should search here to make sure
	 *  that all of the data files for the request are present (except
	 *  for requests that are in the arriving state).  MOREHERE possibly
	 *  in the future.
	 */
	mode = 0;				/* Queueing mode */
	wasexe = 0;				/* Was executing flag */
	prearrive = 0;				/* No pre-arrive time */
	switch (transact [tidminus1].state) {
	case RTS_EXECUTING:
		wasexe = 1;			/* Restart flag */
		break;
	case RTS_PREARRIVE:
		mode = 2;			/* Queue remotely */
		prearrive = transact [tidminus1].update;
		break;				/* Pre-arrive state time */
	case RTS_ARRIVE:
		mode = 2;			/* Queue remotely */
		break;
	}
	if ((nsq_enque (mode, fd, rawreq, wasexe, (mid_t) 0,
			prearrive) & XCI_FULREA_MASK) != TCML_SUBMITTED) {
		/*
		 *  Unable to insert request into queue.
		 *  Try to send mail and place the
		 *  request in the failed directory.
		 */
		printf ("E$Unable to requeue request: %1ld.%s",
		       (long) rawreq->orig_seqno,
			fmtmidname (rawreq->orig_mid));
		printf (" during rebuild.\n");
		fflush (stdout);
		requestlog.reqrcmknown = 1;
		requestlog.svm.rcm = RCM_REBUILDFAI;
		requestlog.svm.mssg [0] = '\0';
		requestlog.outrcmknown = 0;
		if (mai_send (rawreq, &requestlog, MSX_FAILED) == -1) {
			printf ("I$No mail ");
		}
		else printf ("I$Mail ");
		printf ("sent to owner.\n");
		fflush (stdout);
		/*
		 *  Discard any request log and output files.
		 */
		deleteoutput (rawreq);
		/*
		 *  Save request files in failed directory.
		 */
		nqs_failed (rawreq->orig_seqno, rawreq->orig_mid);
		/*
		 *  Deallocate the transaction-id/descriptor
		 *  allocated to the request.
		 */
		if (tra_release (rawreq->trans_id) == -1) {
			/*
			 *  An error occurred releasing the
			 *  transaction descriptor/tid associated
			 *  to the request.
			 */
			printf ("F$Tra_release()");
			printf (rbuildsuffix);
			printf (inforeason, asciierrno());
			printf (infotid, rawreq->trans_id);
			nqs_abort();		/* Abort execution */
		}
		transact [rawreq->trans_id-1].tra_type = TRA_UNALLOCATED;
	}
	else {
		/*
		 *  Otherwise, the request was successfully
		 *  requeued.  Reserve the allocated
		 *  request transaction-id/descriptor.
		 */
		if (!tid_allocate (rawreq->trans_id,
				   rawreq->flags & RQF_EXTERNAL)){
			/*
			 *  The transaction descriptor was
			 *  allocated by a previous request.
			 */
			printf ("F$Multiple reservations ");
			printf ("on tid in nqs_rbuild().\n");
			printf (infotid, rawreq->trans_id);
			errno = 0;	/* Not a system call error */
			nqs_abort();	/* Abort execution */
		}
		/*
		 *  I'm sure this seems a little odd.  What we're doing
		 *  here is to mark every transaction descriptor that has
		 *  been paired with a request as unallocated in our own
		 *  internal memory array, so that when all pairing is
		 *  complete, we can ascertain if there are any transaction
		 *  descriptors that are allocated to non-existent requests--
		 *  a condition that can occur when system administrator
		 *  types (or developers!) manually alter things in the NQS
		 *  database....
		 */
		transact [rawreq->trans_id-1].tra_type = TRA_UNALLOCATED;
	}
}


/*** reserved
 *
 *
 *	int reserved():
 *	Determine if the specified file name is either: "." or "..".
 *
 *	Returns:
 *		1: if the file name is either "." or "..";
 *		0: if neither.
 */
static int reserved (name)
register char *name;			/* Local filaname entry */
{
	if (strcmp (name, ".") == 0 || strcmp (name, "..") == 0) return (1);
	return (0);
}
