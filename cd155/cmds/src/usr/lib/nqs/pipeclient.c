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
 
/*++ pipeclient.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/pipeclient.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS pipe client.  Enqueues requests at pipe queue destinations.
 *
 *
 *	Authors:
 *	--------
 *	Brent A. Kingsbury, Robert W. Sandstrom.
 *	Sterling Software Incorporated.
 *	May 24, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:53:34 $ $State: Exp $)
 * $Log: pipeclient.c,v $
 * Revision 1.4  1994/11/19  02:53:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/12/16  23:26:49  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:27:03  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:58  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:54  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)pipeclient.c	1.2 (pipeclient.c OSF/1 NQS2.0 GJK) 6/30/92";
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
#include "nqs.h"			/* Types and definitions */
#include <sys/stat.h>			/* For stat() */
#include "nqspacket.h"			/* Packets to the local daemon */
#include "netpacket.h"			/* Packets to remote machines */
#include "informcc.h"			/* Information completion codes */
#include "requestcc.h"			/* Request completion codes */
#include "transactcc.h"			/* Transaction completon codes */
#include "nqsdirs.h"			/* For nqslib library functions */

extern int atoi();			/* ASCII to integer */
extern long atol();			/* ASCII to long */
extern long errnototcm();		/* Converts errno to tcm */
#if	NETWORKED
extern int establish();			/* Establish a connection */
#endif
extern void exiting();			/* Relinquish the inter-process */
					/* communication file from the */
					/* local NQS daemon */
#if	NETWORKED
extern int getsockch();			/* Get a char from a socket */
#endif
extern char *getenv();			/* Get value of environment variable */
extern long inter();			/* Send a packet to the local daemon */
extern void interclear();		/* Get a clean slate */
extern int interfmt();			/* Format a packet for writing */
extern int intern32i();			/* Get number of 32 bit ints */
extern int internstr();			/* Get number of strings */
extern long interr32i();		/* Get a 32 bit int */
extern int interread();			/* Read a packet */
extern void interset();			/* Set the file-descriptor used */
					/* to communicate messages to the */
					/* local NQS daemon */
extern void interw32i();		/* Write a 32 bit int */
extern long mergertcm();		/* Merge rcm and tcm */
extern void nqssleep();			/* NQS sleep call */
extern char *pipeqdest();		/* Return info about the n-th pipe */
					/* queue destination */
extern int pipeqenable();		/* Boolean function */
extern void pipeqexitifbad();		/* Exit if the situation is bad */
extern long pipeqfailinfo();		/* Return bits describing failure */
extern long pipeqreq();			/* Attempt to queue a request at a */
					/* particular pipe queue destination */
extern int pipeqretry();		/* Transaction destination and/or */
					/* request retry analysis */
extern int readreq();			/* Read the request header */
extern void serexit();			/* Tell the shepherd what happened */
extern char *strncpy();			/* Limited string copy */
extern int tra_read();			/* Read a transaction descriptor */
extern int tra_setinfo();		/* Set half of transaction descriptor */
extern int tra_setstate();		/* Set half of transaction descriptor */
extern int writehdr();			/* Write control file header */

/*
 *	Variables global to this module.
 */
static int Debug;

/*** main
 *
 *
 *	main():
 *
 *	This process is exec'd by the child of a shepherd process (child of
 *	the NQS daemon) with the following file descriptors open as described:
 *
 *		File descriptor 0: Control file (O_RDWR).
 *		File descriptor 1: Stdout writes to the NQS log process.
 *		File descriptor 2: Stderr writes to the NQS log process.
 *		File descriptor 3: Write file descriptor for serexit()
 *				   back to the shepherd process.
 *		File descriptor 4: Connected to the local NQS daemon
 *				   request FIFO pipe.
 *		File descriptors [5.._NFILE] are closed.
 *
 *
 *	At this time, this process is running with a real AND effective
 *	user-id of root!
 *
 *
 *	The current working directory of this process is the NQS
 *	root directory.
 *
 *
 *	All signal actions are set to SIG_DFL.
 *
 *
 *	The environment of this process contains the environment string:
 *
 *		DEBUG=n
 *
 *	where n specifies (in integer ASCII decimal format), the debug
 *	level in effect when this client was exec'd over the child of
 *	an NQS shepherd.
 *
 *
 *	The user upon whose behalf this work is being performed (the
 *	owner of the request), is identified by the environment variables:
 *
 *		UID=n
 *		USERNAME=username-of-request-owner
 *
 *	where n specifies (in integer ASCII decimal format), the integer
 *	user-id of the request owner.
 * 
 *
 *	For System V based implementations of NQS, the environment
 *	variable:
 *
 *		TZ=timezonename
 *
 *	will also be present.  Berkeley based implementations of NQS will
 *	not contain this environment variable.
 *
 *
 *	The environment of this process also contains the default retry
 *	state tolerance time, and the default retry wait time recommenda-
 *	tions (in seconds):
 *
 *		DEFAULT_RETRYWAIT=n
 *		DEFAULT_RETRYTIME=n
 *
 *	where n is an ASCII decimal format long integer number.
 *
 *
 *	Additional environment vars define the possible destination set
 *	for the request:
 *
 *		Dnnn=mmm rrr queuename
 *
 *	where:
 *
 *		nnn	  specifies the destination number (in ASCII
 *			  decimal) in the range [0..#of destinations-1];
 *		mmm	  specifies the machine-id of the destination
 *			  (in ASCII decimal).
 *		rrr	  specifies the amount of time that this
 *			  destination has spent in a retry mode.
 *		queuename specifies the name of the destination queue
 *			  at the destination machine.
 *
 *
 *	Lastly, the presence of the environment variable:
 *
 *		PERSIST=Y
 *
 *	indicates that the request should be requeued, even if all of the
 *	destinations indicate that they will never accept the request.  The
 *	presence of the PERSIST environment variable is a signal to the
 *	pipe queue server (pipeclient), that there are presently disabled
 *	destination(s) for the associated pipe queue, that MIGHT accept the
 *	request at a later time.
 *
 */
main (argc, argv, envp)
int argc;
char *argv [];
char *envp [];
{
#if	NETWORKED
	long deleteold();		/* Delete a previous transmission */
	long deliver();			/* Act like a network queue client */
#endif
	long commit();			/* 2nd phase of 2 phase commit */
	void evalandexit();		/* Evaluate tcm; don't return */
	void evalmaybeexit();		/* Evaluate tcm; maybe return */
	void ourserexit ();		/* Close IPC file, exit */
	void reportdep();		/* Report departed */
	void reportenable();		/* Report enable */
	void reportpredep();		/* Report predeparted */
	void reportschreq();		/* Report when to reschedule a req. */
	void reportstasis();		/* Request should go to stasis state */

	char *cp;			/* Ascii value of env var */
	int cuid;			/* Client user id */
	char *cusername;		/* Client username */
	long dretrytime;		/* Default seconds before we give up */
	long dretrywait;		/* Default seconds per wait */
	struct rawreq rawreq;		/* What we are about to send */
	struct transact transact;	/* Holds output of tra_read() */
	short thereshope;		/* Boolean */
	long finalrcm;			/* Or several rcms into here */
	register int destno;		/* Destination number */
	long transactcc;		/* Transaction completion code */
	long quereqinfo;		/* Info from NPK_QUEREQ transaction */
	register char *destqueue;	/* Destination queue */
	mid_t destmid;			/* Destination machine-id */
	long retrytime;			/* The amount of time that the */
					/* current destination has spent */
					/* in a retry mode */
	int sd;				/* Socket descriptor */

	/*
	 *  On some UNIX implementations, stdio functions alter errno to
	 *  ENOTTY when they first do something where the file descriptor
	 *  corresponding to the stream happens to be a pipe (which is
	 *  the case here).
	 *
	 *  The fflush() calls are added to get this behavior out of the
	 *  way, since bugs have occurred in the past when a server ran
	 *  into difficultly, and printed out an errno value in situations
	 *  where the diagnostic printf() displaying errno occurred AFTER
	 *  the first stdio function call invoked.
	 */
	fflush (stdout);		/* Stdout is a pipe */
	fflush (stderr);		/* Stderr is a pipe */
	/*
	 * We must be root to bind to a reserved port.
	 */
	if (getuid() != 0 || geteuid() != 0) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
	if ((cp = getenv ("DEBUG")) == (char *) 0) {
		Debug = 0;
	}
	else Debug = atoi (cp);
	interset (4);		/* Use this file descriptor to communicate */
				/* with the local NQS daemon */
	if ((cp = getenv ("UID")) == (char *) 0) {
		ourserexit (RCM_BADSRVARG, (char *) 0);
	}
	cuid = atoi (cp);
	if ((cusername = getenv ("USERNAME")) == (char *) 0) {
		ourserexit (RCM_BADSRVARG, (char *) 0);
	}
	if ((cp = getenv ("DEFAULT_RETRYTIME")) == (char *) 0) {
		ourserexit (RCM_BADSRVARG, (char *) 0);
	}
	dretrytime = atoi (cp);
	if ((cp = getenv ("DEFAULT_RETRYWAIT")) == (char *) 0) {
		ourserexit (RCM_BADSRVARG, (char *) 0);
	}
	dretrywait = atoi (cp);
	if (Debug > 2) {
		while (*envp != (char *) 0) {
			printf ("D$Pipeclient envp: %s\n", *envp);
			envp++;
		}
		fflush (stdout);
	}
	/*
	 * Readreq leaves the file pointer pointing to the first
	 * byte beyond the header.
	 */
	if (readreq (STDIN, &rawreq) == -1) {
		ourserexit (RCM_BADCDTFIL, (char *) 0);
	}
	/*
	 * Find out whether we are starting from scratch.
	 */
	if (tra_read (rawreq.trans_id, &transact) == -1) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
	switch (transact.state) {
	/*
	 * After network queues are implemented,
	 * it will no longer be possible for pipeclient
	 * to called on a request in RTS_DEPART state.
	 */
#if	NETWORKED
	case RTS_DEPART:
		transactcc = deliver (transact.v.peer_mid, &rawreq,
				      cuid, cusername);
		if (Debug > 2) {
			printf ("D$Pipeclient: redeliver tcm oct = %o\n",
				transactcc);
			fflush (stdout);
		}
		if (pipeqenable (transactcc)) {
			reportenable (transact.v.peer_mid);
		}
		switch (transactcc) {
		case TCMP_COMPLETE:
			ourserexit (RCM_DELIVERED, (char *) 0);
		case TCMP_NOSUCHREQ:
			ourserexit (RCM_PIPREQDEL, (char *) 0);
		default:
			evalandexit (transactcc, rawreq.trans_quename,
				transact.v.peer_mid, dretrywait,
				RCM_DELIVERFAI, &rawreq);
		}
	case RTS_PREDEPART:
		transactcc = deleteold (transact.v.peer_mid, &rawreq,
					cuid, cusername);
		if (Debug > 2) {
			printf ("D$Pipeclient: deleteold tcm oct = %o\n",
				transactcc);
			fflush (stdout);
		}
		if (transactcc != TCMP_CONTINUE &&
		    transactcc != TCMP_NOSUCHREQ) {
			evalandexit (transactcc, rawreq.trans_quename,
				     transact.v.peer_mid, dretrywait,
				     RCM_ROUTEFAI, &rawreq);
		}
		reportenable (transact.v.peer_mid);
		reportstasis (&rawreq);
		/*
		 * Now we have a clean slate.
		 * FALL THROUGH.
		 */
#endif
	case RTS_STASIS:
		/*
		 * Our assumptions, until better information can be obtained:
		 * (finalrcm): We don't know WHY any unretryable destinations
		 * are unretryable.
		 * (thereshope): There are NO known retryable destinations.
		 */
		finalrcm = RCM_ROUTEFAI;
		thereshope = 0;
		destno = 1;		/* Try destination #1 first */
		if ((destqueue = pipeqdest (destno, &destmid,
					    &retrytime)) == (char *) 0) {
			if (getenv ("PERSIST") != (char *) 0) {
				/*
				 *  The request should be allowed to persist.
				 */
				reportschreq (&rawreq, dretrywait);
				ourserexit (RCM_RETRYLATER, (char *) 0);
			}
			ourserexit (RCM_ROUTEFAI, (char *) 0);
		}
		do {
			/*
			 *  Another destination exists.  Try it.
			 */
			if (Debug > 2) {
				printf ("D$Pipeclient: dest=%1d: ", destno);
				printf ("mid=%1d;  queue=%s\n", destmid,
					destqueue);
				fflush (stdout);
			}
			/*
			 * Pipeqreq() returns when 
			 * 1) the request is in pre-arrive state
			 *	on the remote machine, or
			 * 2) the request has been submitted
			 *	on the local machine.
			 */
			transactcc = pipeqreq (destmid, destqueue, &rawreq,
					       cuid, cusername, &sd);
			if (Debug > 2) {
				printf ("D$Pipeclient: pipeqreq tcm oct = %o\n",
					transactcc);
				fflush (stdout);
			}
			if (pipeqenable (transactcc)) reportenable (destmid);
			if (transactcc != TCMP_CONTINUE &&
			   (transactcc & XCI_FULREA_MASK) != TCML_SUBMITTED) {
				evalmaybeexit (transactcc, destqueue,
					destmid, dretrywait,
					&thereshope, &finalrcm);
			}
		} while (
			transactcc != TCMP_CONTINUE &&
			(transactcc & XCI_FULREA_MASK) != TCML_SUBMITTED &&
			(destqueue = pipeqdest (++destno, &destmid,
				&retrytime)) != (char *) 0
			);
		if ((transactcc & XCI_FULREA_MASK) == TCML_SUBMITTED) {
			/*
			 *  The request was submitted to a queue on the local host.
			 */
			ourserexit (RCM_ROUTEDLOC, (char *) 0);
		}
		if (destqueue == (char *) 0) {
			if (thereshope == 1 ||
			    getenv ("PERSIST") != (char *) 0) {
				/*
				 *  The request should be allowed to persist.
				 */
				reportschreq (&rawreq, dretrywait);
				ourserexit (RCM_RETRYLATER, (char *) 0);
			}
			else {
				ourserexit (finalrcm, (char *) 0);
			}
		}
		/*
		 * Only TCMP_CONTINUE can reach this point.
		 * (The conversation is between different machines.)
		 * Tell the local daemon that the remote machine liked packet 2.
		 */
		reportpredep (&rawreq, destmid, destqueue);
		break;
	default:
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
#if	NETWORKED
	/*
	 * At this point, the request is in the RTS_PREDEPART state,
	 * we know the destination, and we have a socket open
	 * to the destination machine.  Send packet 3.
	 */
	transactcc = commit (sd);
	if (Debug > 2) {
		printf ("D$Pipeclient: commit tcm oct = %o\n", transactcc);
		fflush (stdout);
	}
	if ((transactcc & XCI_FULREA_MASK) != TCMP_SUBMITTED) {
		evalandexit (transactcc, rawreq.trans_quename,
			transact.v.peer_mid, dretrywait,
			RCM_ROUTEFAI, &rawreq);
	}
	/*
	 * Remember the information bits, so that we can tell
	 * our parent when we exit.
	 * Tell the local daemon that the remote machine liked packet 3.
	 */
	quereqinfo = transactcc & XCI_INFORM_MASK;
	reportdep (&rawreq);
	/*
	 * After network queues are implemented, we will exit here
	 * with RCM_ROUTED, passing any information bits (quereqinfo)
	 * back to our caller.
	 */
	/*
	 * Start a new conversation.  Send the tail of the
	 * control file. Send all of the data files.
	 * Eventually, CODE BELOW THIS POINT WILL BECOME PART OF A
	 * DIFFERENT PROCESS, hence the duplication of effort.
	 * That different process will have cuid and cusername as
	 * environmental variables.
	 * Readreq leaves the file pointer pointing to the first
	 * byte beyond the header.
	 */
	if (readreq (STDIN, &rawreq) == -1) {
		ourserexit (RCM_BADCDTFIL, (char *) 0);
	}
	/*
	 * Find out whether we are starting from scratch.
	 */
	if (tra_read (rawreq.trans_id, &transact) == -1) {
		ourserexit (RCM_BADCDTFIL, (char *) 0);
	}
	if (transact.state != RTS_DEPART) {
		ourserexit (RCM_BADCDTFIL, (char *) 0);
	}
	transactcc = deliver (transact.v.peer_mid, &rawreq, cuid, cusername);
	if (Debug > 2) {
		printf ("D$Pipeclient: deliver tcm oct = %o\n", transactcc);
		fflush (stdout);
	}
	switch (transactcc) {
	case TCMP_COMPLETE:
		ourserexit (RCM_DELIVERED | quereqinfo, (char *) 0);
	case TCMP_NOSUCHREQ:
		ourserexit (RCM_PIPREQDEL, (char *) 0);
	default:
		evalandexit (transactcc, rawreq.trans_quename,
			transact.v.peer_mid, dretrywait,
			RCM_DELIVERFAI, &rawreq);
	}
#endif
}


/*** badfailure
 *
 *	int badfailure:
 *
 *	Return 1 if this is a bad failure.
 *	Return 0 if this is a success or a mild failure.
 */
static int badfailure (transactcc)
long transactcc;
{
	return (transactcc == TCML_NOLOCALDAE || transactcc == TCML_PROTOFAIL);
}


#if	NETWORKED
/*** commit
 *
 *	long commit:
 *
 *	Send a packet to the remote machine committing enqueueing
 *	at the previously attempted destination.
 *	This is the 2nd phase of the 2 phase commit.
 *	Returns: a TCM.
 */
static long commit (sd)
int sd;						/* Socket descriptor */
{
	char packet [MAX_PACKET];
	int packetsize;
	int integers;
	int strings;
	
	interclear ();
	interw32i (NPK_COMMIT);
	if ((packetsize = interfmt (packet)) == -1) {
		return (TCML_INTERNERR);
		
	}
	if (write (sd, packet, packetsize) != packetsize) {
		return (TCML_INTERNERR);
			
	}
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		return (TCMP_CONNBROKEN);
	case -2:
		return (TCMP_PROTOFAIL);
	}
	integers = intern32i ();
	strings = internstr ();
	if (integers == 1 && strings == 0) {
		return (interr32i (1));
	}
	else return (TCMP_PROTOFAIL);
}


/*** deleteold
 *
 *	long deleteold:
 *
 *	This function establishes a fresh connection with
 *	destmid.  We tell destmid that it should delete
 *	the request in question.
 *
 */
static long deleteold (destmid, rawreq, cuid, cusername)
mid_t destmid;				/* Destination machine id */
struct rawreq *rawreq;			/* Request info */
int cuid;				/* Client user id */
char *cusername;			/* Client username */
{
	int sd;				/* Socket descriptor */
	short timeout;			/* Seconds between tries */
	long transactcc;		/* Holds establish() return value */

	interclear ();
	interw32i ((long) cuid);		/* Client uid */
	interwstr (cusername);			/* Client username */
	interw32i (rawreq->orig_seqno);		/* Original sequence number */
	interw32i ((long) rawreq->orig_mid);	/* Original machine id */
	interw32i (0L);				/* Signal (none) */
	interw32i ((long) RQS_ARRIVING);	/* Request queueing state */
	/*
	 * Send the first and only packet of the deleteold connection.
	 */
	sd = establish (NPK_DELREQ, destmid, cuid, cusername, &transactcc);
	if (sd == -2) {
		/*
		 * Retry is in order.
		 */
		timeout = 1;
		do {
			nqssleep (timeout);
			interclear ();
			interw32i ((long) cuid);
			interwstr (cusername);
			interw32i (rawreq->orig_seqno);
			interw32i ((long) rawreq->orig_mid);
			interw32i (0L);
			interw32i ((long) RQS_ARRIVING);
			sd = establish (NPK_DELREQ, destmid,
				cuid, cusername, &transactcc);
			timeout *= 2;
		} while (sd == -2 && timeout <= 16);
	}
	return (transactcc);
}


/*** deliver
 *
 *	long deliver:
 *
 *	This function establishes a fresh connection with
 *	destmid. We send over the tail of the control file.
 *	We send over all data files.
 *	In the near future, the network queue client
 *	take the place of deliver().
 *
 */
static long deliver (destmid, rawreq, cuid, cusername)
mid_t destmid;				/* Destination machine id */
struct rawreq *rawreq;			/* Request info */
int cuid;				/* Client user id */
char *cusername;			/* Client username */
{
	int sd;				/* Socket descriptor */
	short timeout;			/* Seconds between tries */
	long transactcc;		/* Holds establish() return value */
	long headersize;		/* Bytes in control file header */
	long totalsize;			/* Bytes in control file */
	char packet [MAX_PACKET];	/* Buffer area */
	int packetsize;			/* Bytes in this packet */
	int strings;			/* Number of strings */
	int integers;			/* Number of integers */
	int i;				/* Loop index */
	char packedname [29];		/* Dir:14, file:14, slash:1 */
	struct stat statbuf;		/* Holds stat() output */
	int datafd;			/* File descriptor of data file */

	interclear ();
	/*
	 * Send the first packet of the delivery connection.
	 */
	sd = establish (NPK_REQFILE, destmid, cuid, cusername, &transactcc);
	if (sd < 0) {
		if (sd == -2) {
			/*
			 * Retry is in order.
			 */
			timeout = 1;
			do {
				nqssleep (timeout);
				interclear ();
				sd = establish (NPK_REQFILE, destmid,
					cuid, cusername, &transactcc);
				timeout *= 2;
			} while (sd == -2 && timeout <= 16);
			/*
			 * Beyond this point, give up on retry.
			 */
			if (sd < 0) {
				return (transactcc);
			}
		}
		else {
			/*
			 * Retry would surely fail.
			 */
			return (transactcc);
		}
	}
	/*
	 * The server liked us.  The next step is to determine
	 * the length of the tail of the control file.
	 * We count on the file pointer being undisturbed from above.
	 */
	if ((headersize = lseek (STDIN, 0L, 1)) == -1) {
		return (errnototcm());
	}
	if ((totalsize = lseek (STDIN, 0L, 2)) == -1) {
		return (errnototcm());
	}
	interclear();
	interw32i ((long) NPK_REQFILE);
	interw32i ((long) -1);		/* "-1" means "the control file" */
	interw32i ((long) totalsize - headersize);
	interw32i ((long) rawreq->orig_seqno);
	interw32i ((long) rawreq->orig_mid);
	if ((packetsize = interfmt (packet)) == -1) {
		return (TCML_INTERNERR);
	}
	if (write (sd, packet, packetsize) != packetsize) {
		return (TCMP_CONNBROKEN);
	}
	/*
	 * See if the server got packet 2 of the delivery connection.
	 */
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		return (TCMP_CONNBROKEN);
	case -2:
		return (TCML_PROTOFAIL);
	}
	integers = intern32i();
	strings = internstr();
	if (integers != 1 || strings != 0) {
		return (TCML_PROTOFAIL);
	}
	switch (transactcc = interr32i (1)) {
	case TCMP_CONTINUE:
		break;
	case TCMP_NOSUCHREQ:			/* Deleted or already ran */
	case TCMP_COMPLETE:			/* Already delivered */
	default:
		return (transactcc);
	}
	/*
	 * Send the tail of the control file as the beginning of
	 * packet 3 of the delivery connection.
	 */
	lseek (STDIN, (long) headersize, 0);
	if (Debug > 2) {
		printf ("D$Pipeclient: about to send tail\n");
		fflush (stdout);
	}
	if (filecopyentire (STDIN, sd) != totalsize - headersize) {
		return (errnototcm ());
	}
	for (i = 0; i < rawreq->ndatafiles; i++) {
		pack6name (packedname, Nqs_data,
			(int) rawreq->orig_seqno % MAX_DATASUBDIRS,
			(char *) 0, rawreq->orig_seqno, 5,
			(long) rawreq->orig_mid, 6, i, 3);
		if (stat (packedname, &statbuf) == -1) {
			return (TCML_INTERNERR);
		}
		if ((datafd = open (packedname, O_RDONLY)) == -1) {
			return (TCML_INTERNERR);
		}
		interclear ();
		interw32i ((long) NPK_REQFILE);
		interw32i ((long) i);
		interw32i (statbuf.st_size);
		if ((packetsize = interfmt (packet)) == -1) {
			return (TCML_INTERNERR);
		}
		if (write (sd, packet, packetsize) != packetsize) {
			return (TCMP_CONNBROKEN);
		}
		switch (interread (getsockch)) {
		case 0:
			break;
		case -1:
			return (TCMP_CONNBROKEN);
		case -2:
			return (TCML_PROTOFAIL);
		}
		integers = intern32i();
		strings = internstr();
		if (integers == 1 && strings == 0) {
			if ((transactcc = interr32i (1)) != TCMP_CONTINUE) {
				return (transactcc);
			}
		}
		/*
		 * Send the data file here
		 */
		if (Debug > 2) {
			printf ("D$Pipeclient: about to send data #%d\n", i);
			fflush (stdout);
		}
		if (filecopyentire (datafd, sd) != statbuf.st_size) {
			return (errnototcm ());
		}
	}			/* End of for loop */
	/*
	 * We have already put a file into this packet.
	 * Tack a note onto the end saying that there are no more files.
	 */
	interclear ();
	interw32i ((long) NPK_DONE);
	interw32i ((long) 0);
	interw32i ((long) 0);
	if ((packetsize = interfmt (packet)) == -1) {
		return (TCML_INTERNERR);
	}
	if (write (sd, packet, packetsize) != packetsize) {
		return (TCMP_CONNBROKEN);
	}
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		return (TCMP_CONNBROKEN);
	case -2:
		return (TCML_PROTOFAIL);
	}
	integers = intern32i();
	strings = internstr();
	if (integers == 1 && strings == 0) {
		return (interr32i (1));
	}
	return (TCML_PROTOFAIL);
}
#endif


/*** evalandexit
 *
 *	void evalandexit:
 *
 *	Evaluate a transaction completion code, and act on any retry
 *	information.  Then exit, passing a request completion code to
 *	the shepherd process.
 */
static void evalandexit (transactcc, quename, destmid, wait, basercm, rawreq)
long transactcc;			/* Transaction completion code */
char *quename;				/* Name without @machine */
mid_t destmid;				/* Destination machine id */
long wait;				/* Seconds before next try */
long basercm;				/* Generalized reason for failure */
struct rawreq *rawreq;			/* Pointer to request header info */
{
	void reportschdes();
	void reportschreq();
	void ourserexit();
#ifdef SDSC
        register short reason;          /* Reason bits of completion code */
#endif
	
	switch (pipeqretry (transactcc)) {
	case 1:
		reportschdes (quename, destmid, wait);
		reportschreq (rawreq, wait);
		ourserexit (RCM_RETRYLATER, (char *) 0);
	case 0:
		pipeqexitifbad (transactcc);
#ifdef SDSC
                /* this is needed - see analyzercm () - tcmmsgs will be
                 * called for these 2 reasons.
                 */

                reason = (basercm & XCI_REASON_MASK);
                if (reason == RCM_DELIVERFAI || reason == RCM_DELIVERRETX) {
                    ourserexit (mergertcm (basercm, transactcc), (char *) 0);
                } else {
                    ourserexit (basercm | pipeqfailinfo (transactcc),
                    (char *) 0);
                }
#else
		ourserexit (basercm | pipeqfailinfo (transactcc), (char *) 0);
#endif
	case -1:
		reportschdes (quename, destmid, wait);
		ourserexit (basercm | pipeqfailinfo (transactcc), (char *) 0);
	}
}


/*** evalmaybeexit
 *
 *
 *	void evalmaybeexit:
 *
 *	Evaluate a transaction completion code.
 *	If it indicates that a destination retry is in order, then tell the
 *	local daemon when to schedule the destination retry event.
 *	If it indicates that retry is not in order, or in a reason into
 *	finalrcm.  If you want to customize the pipeclient, this is a good
 *	place to start.
 */
static void evalmaybeexit (transactcc, quename, destmid, wait, thereshope,
			   finalrcm)
long transactcc;			/* Transaction completion code */
char *quename;				/* Name without @machine */
mid_t destmid;				/* Machine id */
long wait;				/* Seconds before next try */
short *thereshope;			/* Pointer to boolean */
long *finalrcm;				/* Pointer to composite of reasons */
					/* for failure */
{
	void reportschdes();
	
	switch (pipeqretry (transactcc)) {
	case 1:
		*thereshope = 1;
		reportschdes (quename, destmid, wait);
		break;
	case 0:
		pipeqexitifbad (transactcc);
		*finalrcm |= pipeqfailinfo (transactcc);
		break;
	case -1:
		reportschdes (quename, destmid, wait);
		break;
	}
}


/*** ourserexit
 *
 *	void ourserexit:
 *
 *	Give up any IPC file, close the FIFO pipe, and
 *	then tell the shepherd process what happened.
 */
static void ourserexit (rcm, servermssg)
long rcm;				/* Request completion code */
char *servermssg;			/* Additional text */
{
	fflush (stdout);
	exiting ();
	serexit (rcm, servermssg);
}


/*** reportdep
 *
 *	void reportdep:
 *
 *	Report that the request is ready to go to the departed state.
 *	Exit if inter() fails badly.
 */
static void reportdep (rawreq)
struct rawreq *rawreq;			/* Request info */
{
	long transactcc;
	struct transact transact;
	
	interclear ();
	interw32i (rawreq->orig_seqno);
	interw32i ((long) rawreq->orig_mid);
	transactcc = inter (PKT_RMTDEPART);
	if (badfailure (transactcc)) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
	if (transactcc == TCML_NOSUCHREQ)  {
		/*
		 * Should never happen.
		 */
		ourserexit (RCM_PIPREQDEL, (char *) 0);
	}
	/*
	 * Synchronously leave a record in non-volatile memory.
	 */
	tra_read (rawreq->trans_id, &transact);
	transact.state = RTS_DEPART;
	tra_setstate (rawreq->trans_id, &transact);
}


/*** reportenable
 *
 *	void reportenable:
 *
 *	Report that all destinations at a machine should be reenabled.
 *	Exit if inter() fails badly.
 */
static void reportenable (mid)
mid_t mid;				/* Machine id */
{
	interclear ();
	interw32i ((long) mid);
	if (badfailure (inter (PKT_RMTENAMAC))) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
}


/*** reportpredep
 *
 *	void reportpredep:
 *
 *	Report that the request is ready to go to the predeparted state.
 */
static void reportpredep (rawreq, destmid, destqueue)
struct rawreq *rawreq;			/* Pointer to request header info */
mid_t destmid;				/* Destination machine id */
char *destqueue;			/* Destination queue name */
{
	long transactcc;
	struct transact transact;
	
	interclear ();
	interw32i (rawreq->orig_seqno);
	interw32i ((long) rawreq->orig_mid);
	transactcc = inter (PKT_RMTACCEPT);
	if (badfailure (transactcc)) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
	if (transactcc == TCML_NOSUCHREQ)  {
		ourserexit (RCM_PIPREQDEL, (char *) 0);
	}
	strncpy (rawreq->trans_quename, destqueue, MAX_QUEUENAME + 1);
	writehdr (STDIN, rawreq);
	/*
	 * BEWARE!
	 * The machine might crash after the tra_setstate() call has
	 * changed the state to RTS_PREDEPART, but before the write
	 * buffer containing the new trans_quename has been written
	 * to disk.
	 */
	tra_read (rawreq->trans_id, &transact);
	transact.v.peer_mid = destmid;
	transact.state = RTS_PREDEPART;
	/*
	 * The order of update must guarantee that: IF the state has
	 * already been set, then the info fields have also been set.
	 */
	tra_setinfo (rawreq->trans_id, &transact);
	tra_setstate (rawreq->trans_id, &transact);
}


/*** reportschdes
 *
 *	void reportschdes:
 *
 *	Report that the destination should be retried after
 *	retrywait seconds.
 */
static void reportschdes (quename, destmid, wait)
char *quename;				/* Queue name missing @machine */
mid_t destmid;				/* Destination machine id */
long wait;				/* Seconds before next retry */
{
	interclear ();
	interwstr (quename);
	interw32i ((long) destmid);
	interw32i (wait);
	if (badfailure (inter (PKT_RMTSCHDES))) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
}


/*** reportschreq
 *
 *	void reportschreq:
 *
 *	Report that the request should be retried after
 *	retrywait seconds.
 */
static void reportschreq (rawreq, wait)
struct rawreq *rawreq;			/* Pointer to request header info */
long wait;				/* Seconds before next retry */
{
	interclear ();
	interw32i (rawreq->orig_seqno);
	interw32i ((long) rawreq->orig_mid);
	interw32i (wait);
	if (badfailure (inter (PKT_SCHEDULEREQ))) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
}


/*** reportstasis
 *
 *	void reportstasis:
 *
 *	Report that the request should go back to the stasis state.
 */
static void reportstasis (rawreq)
struct rawreq *rawreq;			/* Pointer to request header info */
{
	struct transact transact;
	register long transactcc;
	
	/*
	 * Synchronously leave a record in non-volatile memory.
	 */
	tra_read (rawreq->trans_id, &transact);
	transact.v.peer_mid = 0;
	transact.state = RTS_STASIS;
	/*
	 * The order of update must guarantee that: IF the state has
	 * already been set, then the info fields have also been set.
	 */
	tra_setinfo (rawreq->trans_id, &transact);
	tra_setstate (rawreq->trans_id, &transact);
	/*
	 *  Tell the local NQS daemon, that the request is now back
	 *  in the stasis state.
	 */
	interclear ();
	interw32i (rawreq->orig_seqno);
	interw32i ((long) rawreq->orig_mid);
	transactcc = inter (PKT_RMTSTASIS);
	if (badfailure (transactcc)) {
		ourserexit (RCM_UNAFAILURE, (char *) 0);
	}
	if (transactcc == TCML_NOSUCHREQ)  {
		ourserexit (RCM_PIPREQDEL, (char *) 0);
	}
}
