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
 
/*++ nqs_pip.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_pip.c,v $
 *
 * DESCRIPTION:
 *
 *	Pipe-queue server, network-queue server, and network server
 *	interaction update module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 31, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.9 $ $Date: 1994/11/19 02:53:06 $ $State: Exp $)
 * $Log: nqs_pip.c,v $
 * Revision 1.9  1994/11/19  02:53:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/05/06  20:48:55  mwan
 * Fixed 8230, 8802, 6877 and 9010.
 *
 * Reviewer: kremenek
 *  Risk: Low
 *  Benefit or PTS #: 8230, 8802, 6877 and 9010.
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c netdaemon.c nqs_pip.c nqs_reqser.c
 * 	    ccs/lib/libnqs/listq.c
 *
 * Revision 1.7  1993/11/02  00:57:31  mwan
 * R1.2 mods
 *
 * Revision 1.6  1993/09/07  20:48:53  shala
 * Fixed bug #6360 by jkearns.
 *
 * Revision 1.5.16.1  1993/09/07  18:09:10  shala
 * Fixed bug #6360. Fixed by jkearns.
 *
 * $Log: nqs_pip.c,v $
 * Revision 1.9  1994/11/19  02:53:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/05/06  20:48:55  mwan
 * Fixed 8230, 8802, 6877 and 9010.
 *
 * Reviewer: kremenek
 *  Risk: Low
 *  Benefit or PTS #: 8230, 8802, 6877 and 9010.
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c netdaemon.c nqs_pip.c nqs_reqser.c
 * 	    ccs/lib/libnqs/listq.c
 *
 * Revision 1.7  1993/11/02  00:57:31  mwan
 * R1.2 mods
 *
 * Revision 1.5  1992/12/17  17:41:49  mwan
 * T6 update 2.2
 *
 * Revision 1.4  1992/12/17  17:26:14  mwan
 * T6 update 2.1
 *
 * Revision 1.3  1992/12/16  23:26:26  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:25:55  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:21  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:19  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"			/* Transaction completion codes */

#ifdef SDSC
#include "informcc.h"
#include "buddyxvar.h"
#include <grp.h>
#include <ctype.h>
#include "res.h"
#ifdef NXACCT
#include <nx/nxacct.h>
#endif
#endif

#ifdef SDSC
extern int errno;
#ifdef NXACCT
extern gid_t nx_getacctid ();
extern struct nxacct *nx_getanam ();
#else  
extern struct passwd *getpwuid ();
extern struct group *getgrnam ();
extern struct group *getgrgid ();
#endif 
extern int my_open ();
extern int my_runok ();
#endif


extern void a2s_a2hset();		/* Add to holding set */
extern void a2s_a2qset();		/* Add to queued set */
extern void a2s_a2wset();		/* Add to waiting set */
extern void bsc_spawn();		/* Possibly spawn some batch reqs */
extern void dsc_spawn();		/* Possibly spawn some device reqs */
extern char *fmtmidname();		/* Format name for machine-id */
extern void nqs_disreq();		/* Dispose of request */
extern struct request *nqs_fndreq();	/* Find request by sequence# & mid */
extern void nqs_vtimer();		/* Set virtual timer */
extern void psc_spawn();		/* Possibly spawn some pipe reqs */
extern time_t time();			/* GMT time since 0:00:00 Jan 1, 1970 */
extern void udb_qorder();		/* Update queue-ordering */


/*** pip_reqreceived
 *
 *
 *	long pip_reqreceived():
 *
 *	Perform the necessary updates for the specified request
 *	that has now been completely received from the local machine,
 *	or from a remote machine.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHREQ:	if the specified request does not
 *				exist on this machine.
 *
 */
long pip_reqreceived (orig_seqno, orig_mid)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	int state;			/* Request state */
	register time_t timenow;	/* Current time */
	register struct queue *queue;	/* Queue containing request */

#ifdef SDSC
	struct rawreq rawreq;          /* To hold most of control file */
        char *tbp;           /* temp buffer */
        int c;
        int buflen;
	int prncpus;
        char buffer[MAX_REQPATH+2];
        char path[MAX_PATHNAME+1];      /* pathname of control file */
        int fd;                        /* file descriptor for control file */
	FILE *ffd;
	int headersize;
#ifdef NXACCT
        struct nxacct *nxacct;
#else
        struct group *grp;
#endif
#endif

	/*
	 *  Search for the request.
	 */
	state = RQS_ARRIVING;		/* Search only arriving state */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	/*
	 *  The request has been located.
	 */
	timenow = time ((time_t *) 0);
	queue = req->queue;		/* Containing queue */

#ifdef SDSC
        if ((fd = getreq ((long) orig_seqno, orig_mid, &rawreq)) == -1) {
                printf("E$nqs_pip(): Unable to getreq.");
                printf ("  Errno = %d\n", errno);
                fflush (stdout);
                return (TCML_UNAFAILURE);
        }          

        pack6name (path, Nqs_control,
                  (int) (orig_seqno % MAX_CTRLSUBDIRS), (char *) 0, 
		  (long) orig_seqno, 5, 
		  (long) orig_mid, 6, 0, 0);
        if ((ffd = fopen (path, "r" )) < 0) {
                printf("E$nqs_pip(): Unable to fopen control file (%s).", path);
                printf ("  Errno = %d\n", errno);
                fflush (stdout);
                return (TCML_UNAFAILURE);
        }          

	/* Read the varying portion of the control file as a
	 *  sequence of lines terminated with newline characters,
	 *  adding environment variables as appropriate.
	 */

        fseek (ffd, lseek (fd, 0L, 1), 0);
        while (!feof (ffd) &&
        fgets (buffer, MAX_REQPATH+2, ffd) != (char *) 0) {
            buflen = strlen (buffer) - 1;    /* Get length of buffer -1 */
            if (buffer [buflen] != '\n') {

                /*    
                 *  We did not see a new line character.
                 *  The control file line is too long.
                 */   
		close (fd);
		fclose (ffd);
		printf ("I$nqs_pip : Problem with env variable len\n");
		fflush (stdout);
                return (TCML_UNAFAILURE);
            }
            buffer [buflen] = '\0';          /* Delete the newline char */
	    tbp = buffer;
            if (strncmp (buffer, "EACCOUNT=", strlen("EACCOUNT=")) == 0) {
 
		/* is cube_account an integer ? */
 
		tbp = buffer + strlen("EACCOUNT=");
                while ((c = (int) *tbp) != '\0') {
		    if (isdigit (c) == 0)
			break;
                    tbp++;
                }
                tbp = buffer + strlen("EACCOUNT=");
                if (c != '\0') {    /* not a digit */
#ifdef NXACCT
		    if ((nxacct = nx_getanam (tbp)) == NULL) {
			close (fd);
			fclose (ffd);
			printf ("I$nqs_pip : bad account-%s\n", tbp);
			fflush (stdout);
			return (TCML_NOACCAUTH);
		    }
                    rawreq.cube_account = nxacct->acct_id;
#else
                    if ((grp = getgrnam (tbp)) == NULL) {
			close (fd);
			fclose (ffd);
			printf ("I$nqs_pip : bad group-%s\n", tbp);
			fflush (stdout);
			return (TCML_NOACCAUTH);
                    }
                    rawreq.cube_account = grp->gr_gid;
#endif

		} else {        /* it is a digit */
		    if (*tbp != '\0') {
			rawreq.cube_account = atoi (tbp);
		    }
		}
 	    } else if (strncmp (buffer, "ENCPUS=", strlen("ENCPUS=")) == 0) {
		tbp = buffer + strlen("ENCPUS=");
		if (*tbp != '\0') {
		    prncpus = atoi (tbp);
                    if (prncpus > queue->q.v1.batch.prncpus || 
		    prncpus < MIN_NCPUS) {
			printf ("I$nqs_pip : bad ncpus-%d, %d\n", 
			prncpus, queue->q.v1.batch.prncpus);
			fflush (stdout);
			close (fd);
			fclose (ffd);
                        return (TCML_QUOTALIMIT | TCI_PR_NCPEXC);
		    } else {
                        rawreq.v.bat.prncpus = prncpus;
        		req->v1.req.v2.bat.ncpus = prncpus;
		        rawreq.v.bat.explicit |= LIM_PRNCPUS;
                        rawreq.v.bat.infinite &= ~LIM_PRNCPUS;
		    }
		}
	    }
	}
	fclose (ffd);
	/* check with MACS */
	if (MACS_FLAG != MACS_OFF) {
	  if (my_open () >= 0) {
	    if ((c = my_runok (rawreq.cube_account,
	    	rawreq.orig_uid, rawreq.v.bat.prncpus)) < 0) {
	    	printf ( "I$nqs_pip:Bad acct,acc=%d uid=%d nod=%d\n"
            	,rawreq.cube_account
            	,rawreq.orig_uid, rawreq.v.bat.prncpus);
	    	close (fd);
            	return (TCML_NOACCAUTH);
            }
	  } else {
	    printf ("I$nqs_pip: Unable to connect to MACS.\n");
            if (MACS_FLAG == MACS_ON) {
		printf ("I$nqs_pip: Request %d not queued\n", 
		rawreq.orig_seqno);
                return (TCML_NOACCAUTH);
            }
	  }
	}
        if (writereq (fd, &rawreq) != 0) {
            printf("E$nqs_pip(): Unable to writereq.");
            printf ("  Errno = %d\n", errno);
            fflush (stdout);
	    close (fd);
            return (TCML_UNAFAILURE);
        }
	close (fd);
#endif
	/*
	 *  It is necessary to dequeue the request from its current
	 *  position in the arriving set of the queue.
	 */
	if (predecessor == (struct request *) 0) {
		queue->arriveset = req->next;
	}
	else predecessor->next = req->next;
	queue->q.arrivecount--;		/* One less request in the arrive */


	/*  The request is now significantly changing state.
	 *  Place the request in the appropriate state set
	 *  within the same queue.
	 */
	req->v1.req.state_time=timenow;	/* Adjust request state time */
	if ((req->status & RQF_OPERHOLD) || (req->status & RQF_USERHOLD)) {
		/*
		 *  Add the request to the hold set for this queue,
		 *  and update the NQS database image.
		 */
		a2s_a2hset (req, queue);
		udb_qorder (queue);	/* Update NQS database image */
	}
	else if (req->start_time > timenow) {
		/*
		 *  The request has a start time in the future,
		 *  and update the NQS database image.
		 */
		a2s_a2wset (req, queue);
		udb_qorder (queue);	/* Update NQS database image */
	}
	else {
		/*
		 *  Place the request in the eligible to run set.
		 *  The QUE_UPDATE bit is set by a2s_a2qset().
		 */
		a2s_a2qset (req, queue);
		switch (queue->q.type) {
		case QUE_BATCH:
			bsc_spawn();	/* Maybe spawn the request */
			break;
		case QUE_DEVICE:
			dsc_spawn();	/* Maybe spawn the request */
			break;
		case QUE_PIPE:
			psc_spawn();	/* Maybe spawn the request */
			break;
		}
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  No requests were spawned from the queue in
			 *  which the request was just completely received.
			 *  Update the NQS database image of the queue.
			 */
			udb_qorder (queue);
		}
	}
	return (TCML_COMPLETE);		/* Return complete */
}


/*** pip_rmtaccept
 *
 *
 *	long pip_rmtaccept():
 *
 *	Record that the remote queue destination for a pipe queue
 *	request has tentatively accepted the request (transaction
 *	state = RTS_PREDEPART).
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHREQ:	if the specified request does not
 *				exist on this machine.
 *
 */
long pip_rmtaccept (orig_seqno, orig_mid)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	int state;			/* Request state */

	/*
	 *  Search for the request.
	 */
	state = RQS_RUNNING;		/* Search only running state */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	/*
	 *  The request has been located.
	 */
	req->status |= RQF_PREDEPART;	/* Set the pre-depart flag */
	return (TCML_COMPLETE);		/* Return complete */
}


/*** pip_rmtarrive
 *
 *
 *	long pip_rmtarrive():
 *
 *	The specified tentatively queued request has been committed
 *	(transaction state = RTS_ARRIVE).
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHREQ:	if the specified request does not
 *				exist on this machine.
 *
 */
long pip_rmtarrive (orig_seqno, orig_mid)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	int state;			/* Request state */

	/*
	 *  Search for the request.
	 */
	state = RQS_ARRIVING;		/* Search only arriving state */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	/*
	 *  The request has been located.
	 */
	req->status &= ~RQF_PREARRIVE;	/* Clear the pre-arrive flag */
	return (TCML_COMPLETE);		/* Return complete */
}


/*** pip_rmtdepart
 *
 *
 *	long pip_rmtdepart():
 *
 *	Record that the remote queue destination for the specified
 *	pipe queue request has committed the request for arrival.
 *
 *	****TRIAGE****
 *	Until network queues are implemented, this procedure is
 *	a no-op.  When network queues are implemented, this
 *	procedure will clear the pre-depart flag for the request,
 *	and move the request into the depart set of the containing
 *	pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHREQ:	if the specified request does not
 *				exist on this machine.
 *
 */
long pip_rmtdepart (orig_seqno, orig_mid)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	int state;			/* Request state */

	/*
	 *  Search for the request.
	 */
	state = RQS_RUNNING;		/* Search only routing state */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	/*
	 *  The request has been located.
	 *
 	 *  ****TRIAGE****
	 *  Until network queues are implemented, this procedure is
	 *  a no-op.  When network queues are implemented, this
	 *  procedure will clear the pre-depart flag for the request,
	 *  and move the request into the depart set of the containing
	 *  pipe queue.
	 */
	return (TCML_COMPLETE);		/* Return complete */
}


/*** pip_rmtstasis
 *
 *
 *	long pip_rmtstasis():
 *
 *	Record that the remote queue destination for a pipe queue
 *	request has been aborted.  The request is now free to be
 *	targeted to any destination.  The request state WAS
 *	RTS_PREDEPART, and has now been restored to RTS_STASIS.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHREQ:	if the specified request does not
 *				exist on this machine.
 *
 */
long pip_rmtstasis (orig_seqno, orig_mid)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	int state;			/* Request state */

	/*
	 *  Search for the request.
	 */
	state = RQS_RUNNING;		/* Search only running state */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	/*
	 *  The request has been located.
	 */
	req->status &= (~RQF_PREDEPART); /* Clear the pre-depart flag */
	return (TCML_COMPLETE);
}


/*** pip_schedulereq
 *
 *
 *	long pip_schedulereq():
 *
 *	Adjust the in-memory version of the start-after time for the
 *	specified request WITHOUT modifying the start-after time
 *	defined in the request control file.  This allows pipe and
 *	network queues to delay delivery of a request until a more
 *	appropriate time.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHREQ:	if the specified request does not
 *				exist on this machine.
 *
 */
long pip_schedulereq (orig_seqno, orig_mid, waittime)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
long waittime;				/* Number of seconds to wait */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	int state;			/* Request state */

	/*
	 *  Search for the request.
	 */
	state = RQS_RUNNING;		/* Search only routing state */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	/*
	 *  The request has been located.
 	 *  Set the start-after time.
	 */
	req->start_time = time ((time_t *) 0) + waittime;
	return (TCML_COMPLETE);		/* Return complete */
}


/*** pip_timeout
 *
 *
 *	void pip_timeout():
 *	Abort uncommitted timed-out remote transactions.
 */
void pip_timeout ()
{
	void abortrequest();		/* Abort request transaction */

	register struct queue *queue;	/* Queue set walking */
	register struct request *preq;	/* Previous request */
	register struct request *req;	/* Request set walking */
	register struct request *next;	/* next request */
	register time_t timenow;	/* Current GMT time */
	time_t abortat;			/* Time to abort transaction at */

	queue = Nonnet_queueset;	/* Non-network queue set */
	timenow = time ((time_t *) 0);	/* Get current time */
	while (queue != (struct queue *) 0) {
		preq = (struct request *) 0;
		req = queue->arriveset;	/* Requests in the arriving set */
		while (req != (struct request *) 0) {
			next = req->next;
			if (req->status & RQF_PREARRIVE) {
				/*
				 *  This request is in the pre-arrive state.
				 */
				abortat = req->v1.req.state_time+TRANS_TIMEOUT;
				if (abortat <= timenow) {
					/*
					 *  This request was not committed for
					 *  arrival within the specified time
					 *  limits.  Abort the transaction.
					 */
					if (Debug) abortrequest (req);
					if (preq == (struct request *) 0) {
						queue->arriveset = req->next;
					}
					else preq->next = req->next;
					queue->q.status |= QUE_UPDATE;
							/* Database image is */
							/* now out of date */
					queue->q.arrivecount--;
							/* Dequeue request */
					nqs_disreq (req, 1);
							/* Delete data files */
				}			/* as well */
				else {
					/*
					 *  Unless this request is committed
					 *  before the transaction timeout
					 *  time sometime in the future, this
					 *  request transaction will have to
					 *  be aborted.
					 */
					nqs_vtimer (&abortat, pip_timeout);
					preq = req;	/* Remember prev req */
				}
			}
			else preq = req;/* Remember previous request */
			req = next;	/* Check the next request */
		}
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  The NQS database image of this queue is now
			 *  out of date, and needs to be updated.
			 */
			udb_qorder (queue);
		}
		queue = queue->next;	/* Check the next queue */
	}
}


/*** abortrequest
 *
 *
 *	void abortrequest():
 *
 *	Display debug diagnostic to report aborted remote request queueing
 *	transaction.
 */
static void abortrequest (request)
register struct request *request;	/* Request being aborted */
{
	printf ("D$Remote queueing transaction of %1ld.%s aborted.\n",
		request->v1.req.orig_seqno,
		fmtmidname (request->v1.req.orig_mid));
	fflush (stdout);
}
