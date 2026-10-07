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
 
/*++ nqs_quereq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_quereq.c,v $
 *
 * DESCRIPTION:
 *
 *	Queue an NQS request for execution.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:08 $ $State: Exp $)
 * $Log: nqs_quereq.c,v $
 * Revision 1.3  1994/11/19  02:53:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:01  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:23  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:28  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_quereq.c	50.2 (nqs_quereq.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "informcc.h"			/* NQS information completion */
					/* codes and masks */
#include "transactcc.h"			/* NQS transaction completion codes */
#include "nqsxvars.h"			/* NQS global vars and directories */

extern char *asciierrno();		/* Return ASCII errno */
extern long lseek();			/* Seek */
extern void mkdefault();		/* Make default output file name */
extern void nqs_delrfs();		/* Delete the specified control and */
					/* data request files */
extern int nqs_enfile();		/* Diagnose open error returning */
					/* non-zero if too many file */
					/* descriptors were open causing */
					/* the last open to fail; otherwise */
					/* 0 is returned. */
extern long nsq_enque();		/* Insert a request into a queue */
extern void nsq_unque();		/* Unqueue the most recently queued */
					/* request */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern int readreq();			/* Read the request header portion */
					/* of a control file for an NQS req */
extern char *strcpy();			/* String copy */
extern int strlen();			/* String length */
extern time_t time();			/* Time */
extern void udb_useqno();		/* Update next available user req */
					/* sequence number stored in the */
					/* NQS database. */
extern int writereq();			/* Write/rewrite request header */


/*** nqs_quereq
 *
 *
 *	long nqs_quereq():
 *	Queue an NQS request for execution.
 *
 *	Returns:
 *		TCML_SUBMITTED: if successful in queueing the request;
 *		otherwise, a failure transaction code is returned (TCMx_).
 *		The information completion code portion of the return
 *		value is 0.
 */
long nqs_quereq (mode, ctrlname, orig_seqno, orig_mid, destqueue, frommid)
int mode;				/* Queueing mode:  0=new; 1=local */
					/* pipe queue; 2=remote pipe queue */
char *ctrlname;				/* The name of the req control file */
					/* (Only meaningful if mode = 0) */
long orig_seqno;			/* Original request sequence# */
					/* (Only meaningful if mode > 0) */
mid_t orig_mid;				/* Original request machine-id */
					/* (Only meaningful if mode > 0) */
char *destqueue;			/* Destination queue */
					/* (Only meaningful if mode > 0) */
mid_t frommid;				/* Machine delivering request */
{					/* (Only meaningful if mode = 2) */

	char newpath [MAX_PATHNAME+1];	/* New pathname */
	char oldpath [MAX_PATHNAME+1];	/* Old pathname */
	struct rawreq rawreq;		/* Raw request structure */
	int cfd;			/* Control file file-descriptor */
	long insqueres;			/* nsq_enque() result code */
	register int i;			/* Counter */

	if (Debug) {
		printf ("D$entering nqs_quereq().\n");
		fflush (stdout);
	}
	if (mode == 0) {
		/*
		 *  This request is a brand new request, with its
		 *  associated files in the new local request
		 *  directory.
		 */
		sprintf (newpath, "%s/%s", Nqs_requests, ctrlname);
	}
	else {
		/*
		 *  The request is being routed by a pipe
		 *  queue.
		 */
		pack6name (newpath, Nqs_control,
			  (int) (orig_seqno % MAX_CTRLSUBDIRS), (char *) 0,
			  (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	}
	if ((cfd = open (newpath, O_RDWR)) == -1) {
		/*
		 *  We are unable to open the control file!
		 */
		if (nqs_enfile ()) {
			printf ("I$Unable to accept new request because of ");
			printf ("file descriptor shortage.\n");
			fflush (stdout);
			return (TCML_ENFILE);
		}
		return (TCML_UNAFAILURE);
	}
	if (Debug) {
		printf ("D$Attempting to read request.\n");
		fflush (stdout);
	}
	if (readreq (cfd, &rawreq) == -1) {
		/*
		 *  Request is corrupt.
		 */
		if (mode == 0) {
			rawreq.tcm = TCML_UNAFAILURE;
			writereq (cfd, &rawreq);
		}
		close (cfd);			/* Close request file */
		return (TCML_UNAFAILURE);	/* Failure */
	}
	if (Debug) {
		printf ("D$Request was read.\n");
		fflush (stdout);
	}
	/*
	 *  Assign sequence number to request.
	 */
	if (mode == 0) {
		/*
		 *  The request is being submitted for the first time and
		 *  does not have a sequence number or transaction-id
		 *  assigned to it.
		 */
		rawreq.trans_id = 0;		/* No arguments about this */
		time (&rawreq.create_time);
		rawreq.tcm = TCML_SUBMITTED;	/* Might overwrite this below */
		rawreq.orig_seqno = Seqno_user;	/* Seq# the request will have */
	}					/* if it queues successfully */
	else {
		/*
		 *  The request is being routed by a pipe queue.
		 *  Copy the destination queue name into the queue
		 *  field of the rawreq structure for the request.
		 */
		strcpy (rawreq.quename, destqueue);
		if (mode == 2) {
			/*
			 *  Set values out of paranoia (that should
			 *  already be set) for requests that are
			 *  being queued from a remote machine.
			 */
			rawreq.trans_id = 0;	/* No transaction-id */
			rawreq.flags |= RQF_EXTERNAL;
		}
	}
	/*
	 *  Assign queue entry time to request.
	 */
	time (&rawreq.enter_time);
	/*
	 *  Manufacture default output file pathnames for batch request
	 *  stderr, stdlog, and stdout files as necessary.
	 */
	if (rawreq.type == RTYPE_BATCH) {
		i = strlen (rawreq.v.bat.stderr_name);
		if ((rawreq.v.bat.stderr_acc & OMD_EO) == 0 &&
		    (i == 0 || rawreq.v.bat.stderr_name [i-1] == '/')) {
			/*
			 *  The stderr file name needs to be resolved.
			 *  The user did NOT specify the pathname for
			 *  the stderr file, and the stderr file is to
			 *  be placed on the execution machine, or
			 *  returned to the submitting machine as
			 *  determined by the OMD_M_KEEP modifier bit.
			 */
			if (Debug > 1) {
				printf ("D$Stderr name needs resolution.\n");
				fflush (stdout);
			}
			if (resolvepath (&rawreq, &rawreq.v.bat.stderr_mid,
					 rawreq.v.bat.stderr_name,
					 rawreq.v.bat.stderr_acc, i,
					 'e') == -1) {
				/*
				 *  The resolved pathname will be too long.
				 */
				if (mode == 0) {
					rawreq.tcm = TCML_PATHLEN;
					writereq (cfd, &rawreq);
				}
				close (cfd);		/* Close request file */
				return (TCML_PATHLEN);
			}
			if (Debug > 1) {
				printf ("D$Resolved stderr name = %s.\n",
					 rawreq.v.bat.stderr_name);
				fflush (stdout);
			}
		}
		i = strlen (rawreq.v.bat.stdlog_name);
		if (i == 0 || rawreq.v.bat.stdlog_name [i-1] == '/') {
			/*
			 *  The stdlog file name needs to be resolved.
			 *  The user did NOT specify the pathname for
			 *  the stdlog file, and the stdlog file is to
			 *  be placed on the execution machine, or
			 *  returned to the submitting machine as
			 *  determined by the OMD_M_KEEP modifier bit.
			 */
			if (Debug > 1) {
				printf ("D$Stdlog name needs resolution.\n");
				fflush (stdout);
			}
			if (resolvepath (&rawreq, &rawreq.v.bat.stdlog_mid,
					 rawreq.v.bat.stdlog_name,
					 rawreq.v.bat.stdlog_acc, i,
					 'l') == -1) {
				/*
				 *  The resolved pathname will be too long.
				 */
				if (mode == 0) {
					rawreq.tcm = TCML_PATHLEN;
					writereq (cfd, &rawreq);
				}
				close (cfd);		/* Close request file */
				return (TCML_PATHLEN);
			}
			if (Debug > 1) {
				printf ("D$Resolved stdlog name = %s.\n",
					 rawreq.v.bat.stdlog_name);
				fflush (stdout);
			}
		}
		i = strlen (rawreq.v.bat.stdout_name);
		if (i == 0 || rawreq.v.bat.stdout_name [i-1] == '/') {
			/*
			 *  The stdout file name needs to be resolved.
			 *  The user did NOT specify the pathname for
			 *  the stdout file, and the stdout file is to
			 *  be placed on the execution machine, or
			 *  returned to the submitting machine as
			 *  determined by the OMD_M_KEEP modifier bit.
			 */
			if (Debug > 1) {
				printf ("D$Stdout name needs resolution.\n");
				fflush (stdout);
			}
			if (resolvepath (&rawreq, &rawreq.v.bat.stdout_mid,
					 rawreq.v.bat.stdout_name,
					 rawreq.v.bat.stdout_acc, i,
					 'o') == -1) {
				/*
				 *  The resolved pathname will be too long.
				 */
				if (mode == 0) {
					rawreq.tcm = TCML_PATHLEN;
					writereq (cfd, &rawreq);
				}
				close (cfd);		/* Close request file */
				return (TCML_PATHLEN);
			}
			if (Debug > 1) {
				printf ("D$Resolved stdout name = %s.\n",
					 rawreq.v.bat.stdout_name);
				fflush (stdout);
			}
		}
	}
	/*
	 *  The request has been assigned a sequence number, and any
	 *  unresolved output return pathnames have also been resolved.
	 *
	 *  Try to insert the request into the internal queues of NQS.
	 */
	insqueres = nsq_enque (mode, cfd, &rawreq, 0, frommid,
			       rawreq.enter_time);
	if ((insqueres & XCI_FULREA_MASK) != TCML_SUBMITTED) {
		/*
		 *  The request was NOT submitted.
		 */
		if (mode == 0) {
			rawreq.tcm = insqueres;	/* Store transaction and */
						/* information completion */
						/* codes */
			writereq (cfd, &rawreq);/* Update request header */
		}
		close (cfd);			/* Close request file */
		return (insqueres);		/* Return transaction code */
	}
	/*
	 *  The request was successfully enqueued.  Make sure to increment
	 *  the next available request sequence number, if necessary.
	 */
	if (mode == 0) {
		rawreq.tcm = insqueres;	/* Store transaction and information */
					/* completion codes */
		/*
		 *  Update the next available sequence number.
		 */
		if (++Seqno_user > MAX_SEQNO_USER) Seqno_user = 0;
		udb_useqno (Seqno_user);	/* Update */
		/*
		 *  A Sequence number and transaction-id has been assigned
		 *  to the request, and the request has been successfully
		 *  enqueued.  Form links in the NQS data directory to all
		 *  data files that are a part of the request.
		 *
		 *  Note however that the control file has NOT been updated
		 *  with the assigned sequence number or transaction-id.  If
		 *  the system crashes after the links have been made, but
		 *  with the control file un-updated, then nqs_rbuild.c will
		 *  have to catch it later.
		 */
		if (Debug) {
			printf ("D$Trying to link in files.\n");
			fflush (stdout);
		}
		for (i=0; i < rawreq.ndatafiles; i++) {
			pack6name (newpath, Nqs_data,
				  (int) (rawreq.orig_seqno % MAX_DATASUBDIRS),
				  (char *) 0, (long) rawreq.orig_seqno, 5,
				  (long) rawreq.orig_mid, 6, i, 3);
			pack6name (oldpath, Nqs_requests, -1, ctrlname, 0L, 0,
				   0L, 0, i, 3);
			if (link (oldpath, newpath) == -1) {
				/*
				 *  The link failed!
				 */
				printf ("E$Unable to form link to ");
				printf ("new request data file.\n");
				printf ("I$link (%s, %s) failed;\n",
					 oldpath, newpath);
				printf ("I$%s.\n", asciierrno());
				fflush (stdout);
				/*
				 *  Unlink any previous links to data files
				 *  for the new request.
				 */
				nqs_delrfs (rawreq.orig_seqno,
					    rawreq.orig_mid, i);
				rawreq.tcm = TCML_UNAFAILURE;
				writereq (cfd, &rawreq);
				close (cfd);	/* Close request file */
				nsq_unque();	/* Unqueue the request */
				return (TCML_UNAFAILURE);	/* Failure */
			}
			unlink (oldpath);	/* Unlink original */
		}
		/*
		 *  Form the link to the control file for the new request.
		 */
		pack6name (newpath, Nqs_control,
			  (int) (rawreq.orig_seqno % MAX_CTRLSUBDIRS),
			  (char *) 0, (long) rawreq.orig_seqno, 5,
			  (long) rawreq.orig_mid, 6, 0, 0);
		sprintf (oldpath, "%s/%s", Nqs_requests, ctrlname);
		if (link (oldpath, newpath) == -1) {
			/*
			 *  We failed.
			 */
			printf ("E$Unable to form link to new request ");
			printf ("control file.\n");
			printf ("I$link (%s, %s) failed;\n", oldpath, newpath);
			printf ("I$%s.\n", asciierrno());
			fflush (stdout);
			/*
			 *  Unlink all of the data files and control files
			 *  for the new request.
			 */
			nqs_delrfs (rawreq.orig_seqno, rawreq.orig_mid,
				    rawreq.ndatafiles);
			rawreq.tcm = TCML_UNAFAILURE;
			writereq (cfd, &rawreq);
			close (cfd);		/* Close request file */
			nsq_unque();		/* Unqueue the request */
			return (TCML_UNAFAILURE);
		}				/* Failure */
		unlink (oldpath);		/* Unlink original */
	}
	/*
	 *  The request has been assigned a sequence number and a transaction
	 *  descriptor.  The request has also been queued, and all of the
	 *  control and data files have been linked in.  Rawreq.tcm holds
	 *  the transaction completion code.
	 */
	writereq (cfd, &rawreq);		/* Update request header */
	close (cfd);				/* Close request file */
	return (insqueres);			/* Return transaction code */
}


/*** resolvepath
 *
 *
 *	int resolvepath():
 *	Resolve batch request output file path (when necessary).
 *
 *	Returns:
 *		 0: Path resolved successfully.
 *		-1: Resolved path is too long.
 */
static int resolvepath (rawreq, mid_resolve, path_resolve, access_mode, length,
			suffixchar)
register struct rawreq *rawreq;		/* Raw request structure for request */
mid_t *mid_resolve;			/* Machine-id requiring resolution */
register char *path_resolve;		/* Path requiring resolution */
short access_mode;			/* Access mode */
register int length;			/* Len of path requiring resolution */
char suffixchar;			/* Suffix char for default */
{
	char dirpath [MAX_REQPATH+1];	/* Output file pathname */

	if (length + 14 > MAX_REQPATH) {
		/*
		 *  The resolved pathname will be
		 *  too long.
		 */
		rawreq->tcm = TCML_PATHLEN;
		return (-1);		/* Too long */
	}
	strcpy (dirpath, path_resolve);
	if (length) dirpath [length-1] = '\0';
	/*
	 *  Manufacture proper default pathname.
	 */
	mkdefault (path_resolve, dirpath, rawreq->reqname,
		  (long) rawreq->orig_seqno, suffixchar);
	if (access_mode & OMD_M_KEEP) {
		/*
		 *  The file must be left on the execution machine.
		 */
		*mid_resolve = Locmid;
	}
	return (0);			/* Success */
}
