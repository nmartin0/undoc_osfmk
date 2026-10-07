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
 
/*++ pipeqreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/pipeqreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Attempt to queue a request at the designated pipe queue
 *	destination.
 *
 *
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *
 *
 *		This module cannot be readily changed,
 *		since it is responsible for building
 *		part of the contents of an NPK_QUEREQ
 *		network packet, a packet which must
 *		be recognizable by other NQS machines.
 *
 *
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 20, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:33 $ $State: Exp $)
 * $Log: pipeqreq.c,v $
 * Revision 1.3  1994/11/19  02:27:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:15  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:35  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:29  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>			/* For logfile message writing */
#include "nqs.h"			/* NQS types and definitions */
#include "netpacket.h"			/* NQS Network packet types */
#include "nqspacket.h"			/* NQS local packet types */
#include "transactcc.h"			/* NQS transaction completion codes */

extern int errno;			/* System call error# */
#if	NETWORKED
extern int establish();			/* Establish a connection */
extern int getsockch();			/* Get a char from a socket */
#endif
extern long inter();			/* Talk to local NQS daemon */
extern void interclear();		/* Get a clean slate */
extern int interfmt();			/* Format a packet */
extern int intern32i();			/* Get number of 32 bit ints */
extern int internsr();			/* Get number of strings */
extern long interr32i();		/* Read a 32 bit int */
extern int interread();			/* Read a packet */
extern void interw32i();		/* Write packet integer datum */
extern void interw32u();		/* Write packet unsigned datum */
extern void interwstr();		/* Write packet string datum */
extern int localmid();			/* Get local machine id */
extern void nqssleep();			/* Avoid sleep() bug */


/*** pipeqreq
 *
 *
 *	long pipeqreq():
 *
 *	Attempt to queue a request at the designated pipe queue
 *	destination.  If successful, *sd will contain the socket
 *	descriptor of the connection.
 *
 *	Expected returns values:
 *		TCML_ACCESSDEN:	 Queue access denied at local host destination.
 *		TCMP_ACCESSDEN:  Queue access denied at remote peer
 *				 destination.
 *		TCML_BADCDTFIL:	 Local machine destination garbled the request.
 *		TCMP_BADCDTFIL:  Remote peer destination machine garbled the
 *				 request.
 *		TCMP_CONNBROKEN: Connection broken.
 *		TCMP_CONNTIMOUT: Connection timed-out.
 *		TCMP_CONTINUE:	 Request queued successfully, continue
 *				 queueing operation via 2-phase commit.
 *		TCML_ENFILE:	 File descriptor shortage at local host
 *				 destination.
 *		TCMP_ENFILE:	 File descriptor shortage at remote peer
 *				 destination.
 *		TCML_ENOSPC:	 File system resource shortage at local host
 *				 destination.
 *		TCMP_ENOSPC:	 File system resource shortage at remote peer
 *				 destination.
 *		TCML_ERRORRETRY: An error condition occurred at the local host
 *				 destination which may go away in the near
 *				 future.
 *		TCMP_ERRORRETRY: An error condition occurred at the remote peer
 *				 destination which may go away in the near
 *				 future.
 *		TCML_ETIMEDOUT:	 Connect(2) timed out at local host.
 *		TCML_FATALABORT: Non-retriable error condition at local host
 *				 destination.
 *		TCMP_FATALABORT: Non-retriable error condition at remote peer
 *				 destination.
 *		TCML_INSQUESPA:	 Insufficient queue space at local host
 *				 destination.
 *		TCMP_INSQUESPA:  Insufficient queue space at remote peer
 *				 destination.
 *		TCML_INTERNERR:	 Internal error on local host destination.
 *		TCMP_INTERNERR:	 Internal error at remote peer destination.
 *		TCML_NOLOCALDAE: No NQS local daemon at local host destination!
 *		TCMP_NOLOCALDAE: No NQS local daemon at remote peer destination.
 *		TCMP_NONETDAE:   No NQS net daemon at remote peer destination.
 *		TCML_NOSUCHFORM: No such form at local host destination.
 *		TCMP_NOSUCHFORM: No such form at remote peer destination.
 *		TCML_NOSUCHQUE:	 No such queue at local host destination.
 *		TCMP_NOSUCHQUE:  No such queue at remote peer destination.
 *		TCML_PATHLEN:	 Resolved path of batch request exceeds maximum
 *				 supported length at local host destination.
 *		TCMP_PATHLEN:	 Resolved path of batch request exceeds maximum
 *				 supported length at remote peer destination.
 *		TCML_PROTOFAIL:	 NQS protocol failure error at local host.
 *		TCMP_PROTOFAIL:  NQS protocol failure error at remote peer
 *				 destination, or protocol failure between the
 *				 local host and the remote peer.
 *		TCML_QUEDISABL:	 Destination queue at local host is disabled.
 *		TCMP_QUEDISABL:	 Destination queue at remote peer is disabled.
 *		TCML_QUOTALIMIT: Destination queue quota limits exceeded at
 *				 local host.
 *		TCMP_QUOTALIMIT: Destination queue quota limits exceeded at
 *				 remote peer host.
 *		TCML_REQCOLLIDE: Request collided with existing request at
 *				 local host destination.
 *		TCMP_REQCOLLIDE: Request collided with existing request at
 *				 remote peer destination.
 *		TCML_SUBMITTED:	 Request successfully submitted to local
 *				 host destination.
 *		TCMP_SUBMITTED:	 Request successfully submitted to remote
 *				 peer destination.
 *		TCMP_RRFUNKNMID: Machine-id value in request is unknown at
 *				 remote peer destination.
 *		TCML_UNAFAILURE: Unanticipated failure at local host.
 *		TCMP_UNAFAILURE: Unanticipated failure at remote peer.
 *		TCML_WROQUETYP:	 Destination queue at local host is wrong
 *				 type for request.
 *		TCMP_WROQUETYP:	 Destination queue at remote peer is wrong
 *				 type for request.
 */
long pipeqreq (destmid, destqueue, rawreq, cuid, cusername, sd)
long destmid;				/* Destination machine id */
char *destqueue;			/* Destination queue */
register struct rawreq *rawreq;		/* Raw request to be packaged */
int cuid;				/* Client user id */
char *cusername;			/* Client username */
int *sd;				/* Where to put socket descriptor */
{
	typedef unsigned long longcard;	/* Less typing */

	long locmid;			/* Local machine id */
	long transactcc;		/* Transaction completion code */
	short timeout;			/* Seconds */
	register short i;		/* Loop var */
	char packet [MAX_PACKET];	/* Message packet buffer */
	int packetsize;			/* Bytes in this packet */
	int integers;			/* Number of integers received */
	int strings;			/* Number of strings received */

	/*
	 * NOTE, We will be relying on the signal (SIGPIPE, fn) handler
	 * created by the call to establish() to convert SIGPIPE signals
	 * to errno = EPIPE.
	 */
	
	interclear ();
	if (localmid (&locmid) != 0) {
		return (TCML_SELMIDUNKN);
	}
	if (destmid == locmid) {
		interw32i (rawreq->orig_seqno);
		interw32i ((long) rawreq->orig_mid);
		interwstr (destqueue);
		return (inter (PKT_QUEREQVLPQ));
	}
#if	NETWORKED
	/*
	 * Only remote destinations get to this point.
	 * Establish connection to destination machine.
	 */
	*sd = establish (NPK_QUEREQ, destmid, cuid, cusername, &transactcc);
	if (*sd < 0) {
		if (*sd == -2) {
			/*
			 *  Retry is in order.
			 */
			timeout = 1;
			do {
				nqssleep (timeout);
				interclear ();
				*sd = establish (NPK_QUEREQ, destmid,
						 cuid, cusername,
						 &transactcc);
				timeout *= 2;
			} while (*sd == -2 && timeout <= 2);
			/*
			 * Beyond this point, give up on retry.
			 */
			if (*sd < 0) return (transactcc);
		}
		else return (transactcc);
	}
	interclear ();
	/*
	 *  Define packet contents.
	 */
	interwstr (destqueue);		/* Destination queue */
	interw32i ((long) rawreq->magic1);
	interw32u ((longcard) rawreq->create_time);
	interw32u ((longcard) rawreq->enter_time);
	interw32i ((long) rawreq->type);
	interw32i ((long) rawreq->orig_uid);
	interw32i ((long) rawreq->orig_mid);
	interw32i ((long) rawreq->orig_seqno);
	interw32i ((long) rawreq->rpriority);
	interw32i ((long) rawreq->flags);
	interw32u ((longcard) rawreq->start_time);
	interw32i ((long) rawreq->ndatafiles);
	interwstr (rawreq->reqname);
	interwstr (rawreq->username);
	interwstr (rawreq->mail_name);
	interw32i ((long) rawreq->mail_mid);
	if (rawreq->type == RTYPE_DEVICE) {
		/*
		 *  We are packaging a device request.
		 */
		interwstr (rawreq->v.dev.forms);
		interw32i ((long) rawreq->v.dev.copies);
		interw32i ((long) rawreq->v.dev.reserved1);
		interw32i ((long) rawreq->v.dev.reserved2);
		interw32u ((longcard) rawreq->v.dev.size);
		for (i = 0; i < MAX_DEVPREF; i++) {
			interwstr (rawreq->v.dev.devprefname [i]);
			interw32i ((long) rawreq->v.dev.devprefmid [i]);
		}
	}
	else {
		/*
		 *  We are packaging a batch request.
		 */
		interw32i ((long) rawreq->v.bat.umask);
		interwstr (rawreq->v.bat.shell_name);
		interw32i ((long) rawreq->v.bat.explicit);
		interw32i ((long) rawreq->v.bat.infinite);
		/*
		 *  Package per-process corefile size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppcoresize.max_quota);
		interw32i ((long) rawreq->v.bat.ppcoresize.max_units);
		interw32u ((longcard) rawreq->v.bat.ppcoresize.warn_quota);
		interw32i ((long) rawreq->v.bat.ppcoresize.warn_units);
		/*
		 *  Package per-process data-segment size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppdatasize.max_quota);
		interw32i ((long) rawreq->v.bat.ppdatasize.max_units);
		interw32u ((longcard) rawreq->v.bat.ppdatasize.warn_quota);
		interw32i ((long) rawreq->v.bat.ppdatasize.warn_units);
		/*
		 *  Package per-process permanent file size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.pppfilesize.max_quota);
		interw32i ((long) rawreq->v.bat.pppfilesize.max_units);
		interw32u ((longcard) rawreq->v.bat.pppfilesize.warn_quota);
		interw32i ((long) rawreq->v.bat.pppfilesize.warn_units);
		/*
		 *  Package per-request permanent file space limits.
		 */
		interw32u ((longcard) rawreq->v.bat.prpfilespace.max_quota);
		interw32i ((long) rawreq->v.bat.prpfilespace.max_units);
		interw32u ((longcard) rawreq->v.bat.prpfilespace.warn_quota);
		interw32i ((long) rawreq->v.bat.prpfilespace.warn_units);
		/*
		 *  Package per-process quick file size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppqfilesize.max_quota);
		interw32i ((long) rawreq->v.bat.ppqfilesize.max_units);
		interw32u ((longcard) rawreq->v.bat.ppqfilesize.warn_quota);
		interw32i ((long) rawreq->v.bat.ppqfilesize.warn_units);
		/*
		 *  Package per-request quick file space limits.
		 */
		interw32u ((longcard) rawreq->v.bat.prqfilespace.max_quota);
		interw32i ((long) rawreq->v.bat.prqfilespace.max_units);
		interw32u ((longcard) rawreq->v.bat.prqfilespace.warn_quota);
		interw32i ((long) rawreq->v.bat.prqfilespace.warn_units);
		/*
		 *  Package per-process temporary file size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.pptfilesize.max_quota);
		interw32i ((long) rawreq->v.bat.pptfilesize.max_units);
		interw32u ((longcard) rawreq->v.bat.pptfilesize.warn_quota);
		interw32i ((long) rawreq->v.bat.pptfilesize.warn_units);
		/*
		 *  Package per-request temporary file space limits.
		 */
		interw32u ((longcard) rawreq->v.bat.prtfilespace.max_quota);
		interw32i ((long) rawreq->v.bat.prtfilespace.max_units);
		interw32u ((longcard) rawreq->v.bat.prtfilespace.warn_quota);
		interw32i ((long) rawreq->v.bat.prtfilespace.warn_units);
		/*
		 *  Package per-process memory size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppmemsize.max_quota);
		interw32i ((long) rawreq->v.bat.ppmemsize.max_units);
		interw32u ((longcard) rawreq->v.bat.ppmemsize.warn_quota);
		interw32i ((long) rawreq->v.bat.ppmemsize.warn_units);
		/*
		 *  Package per-request memory size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.prmemsize.max_quota);
		interw32i ((long) rawreq->v.bat.prmemsize.max_units);
		interw32u ((longcard) rawreq->v.bat.prmemsize.warn_quota);
		interw32i ((long) rawreq->v.bat.prmemsize.warn_units);
		/*
		 *  Package per-process stack-segment size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppstacksize.max_quota);
		interw32i ((long) rawreq->v.bat.ppstacksize.max_units);
		interw32u ((longcard) rawreq->v.bat.ppstacksize.warn_quota);
		interw32i ((long) rawreq->v.bat.ppstacksize.warn_units);
		/*
		 *  Package per-process working set size limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppworkset.max_quota);
		interw32i ((long) rawreq->v.bat.ppworkset.max_units);
		interw32u ((longcard) rawreq->v.bat.ppworkset.warn_quota);
		interw32i ((long) rawreq->v.bat.ppworkset.warn_units);
		/*
		 *  Package per-process CPU time limits.
		 */
		interw32u ((longcard) rawreq->v.bat.ppcputime.max_seconds);
		interw32i ((long) rawreq->v.bat.ppcputime.max_ms);
		interw32u ((longcard) rawreq->v.bat.ppcputime.warn_seconds);
		interw32i ((long) rawreq->v.bat.ppcputime.warn_ms);
		/*
		 *  Package per-request CPU time limits.
		 */
		interw32u ((longcard) rawreq->v.bat.prcputime.max_seconds);
		interw32i ((long) rawreq->v.bat.prcputime.max_ms);
		interw32u ((longcard) rawreq->v.bat.prcputime.warn_seconds);
		interw32i ((long) rawreq->v.bat.prcputime.warn_ms);
		/*
		 *  Package remaining batch request fields.
		 */
		interw32i ((long) rawreq->v.bat.ppnice);
		interw32i ((long) rawreq->v.bat.prdrives);
		interw32i ((long) rawreq->v.bat.prncpus);
		for (i=0; i < MAX_PREDECESSOR; i++) {
			interwstr (rawreq->v.bat.predecessors [i]);
		}
		interw32i ((long) rawreq->v.bat.stderr_acc);
		interw32i ((long) rawreq->v.bat.stdlog_acc);
		interw32i ((long) rawreq->v.bat.stdout_acc);
		interw32i ((long) rawreq->v.bat.stderr_mid);
		interw32i ((long) rawreq->v.bat.stdlog_mid);
		interw32i ((long) rawreq->v.bat.stdout_mid);
		interwstr (rawreq->v.bat.stderr_name);
		interwstr (rawreq->v.bat.stdlog_name);
		interwstr (rawreq->v.bat.stdout_name);
		interw32i ((long) rawreq->v.bat.instacount);
		interw32i ((long) rawreq->v.bat.oustacount);
		interw32i ((long) rawreq->v.bat.instahiermask);
		interw32i ((long) rawreq->v.bat.oustahiermask);
		for (i=0; i < MAX_INSTAPERREQ; i++) {
			interw32i ((long) rawreq->v.bat.instamid [i]);
		}
		for (i=0; i < MAX_OUSTAPERREQ; i++) {
			interw32i ((long) rawreq->v.bat.oustamid [i]);
		}
	}
	/*
	 *  Transmit packet 2 to the destination machine on
	 *  the established socket connection.
	 */
	if ((packetsize = interfmt (packet)) == -1) {
		/*
		 *  The packet contents are too large.
		 */
		close (*sd);	/* Close connection */
		return (TCML_INTERNERR);	/* Local internal error */
	}
	if (write (*sd, packet, packetsize) != packetsize) {
		/*
		 *  Error sending NPK_QUEREQ to destination.
		 */
		close (*sd);	/* Close connection */
		return (TCML_ERRORRETRY);/* Remote server "bit the dust" */
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
#endif
}
