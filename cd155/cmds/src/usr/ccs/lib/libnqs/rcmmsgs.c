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
 
/*++ rcmmsgs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/rcmmsgs.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Diagnose a request completion code WITHOUT
 *	diagnosing any RCI_ information bits that might
 *	be present in the request completion code.
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 28, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:27:38 $ $State: Exp $)
 * $Log: rcmmsgs.c,v $
 * Revision 1.5  1994/11/19  02:27:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/02  23:04:59  mwan
 * Fixed PTS #7838 and #7858
 *
 *  Reviewer: jkearns
 *  Risk: L
 *  Benefit or PTS #: 7838, 7858
 *  Testing:
 *  Module(s): usr/include/nqs/requestcc.h, usr/include/nqs/buddy.h,
 *                    usr/ccs/lib/libnqs/rcmmsgs.c,
 *                    usr/lib/nqs/nqs_reqser.c, usr/lib/nqs/nqs_reqexi.c,
 *                    usr/lib/nqs/nqs_reqcom.c, usr/lib/nqs/nqs_spawn.c
 * 		   usr/lib/macs_rootp.c.
 *
 * Revision 1.3  1993/05/28  20:11:03  mwan
 * Change to cause NQS to mail the correct bad dedicated time slot message.
 *
 * Revision 1.2  1992/10/09  20:18:24  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:40  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:56  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>		/* FILE definition */
#include "informcc.h"		/* NQS information completion codes */
#include "requestcc.h"		/* NQS request completion codes */


/*** rcmmsgs
 *
 *
 *	void rcmmsgs():
 *
 *	Diagnose a request completion code WITHOUT
 *	diagnosing any RCI_ information bits that might
 *	be present in the request completion code.
 */
void rcmmsgs (code, stream, prefix)
register long code;			/* Completion code */
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	static char
	shutdown[] ="Request aborted for NQS shutdown.\n";
	static char
	deleted[]  ="Request deleted.\n";
	static char
	failed[]   ="Request files placed in NQS failed directory.\n";
	static char
	nodeliver[]="Request could not be delivered to a destination queue.\n";
	static char
	norun[]    ="Request not executed.\n";
	static char
	reque[]    ="Request requeued.\n";
	static char
	retryexc[] ="Retry limit exceeded.\n";
	static char
	shlbrkpnt[]="Unsupported shell breakpoint encountered.\n";
	static char
	shlexefai[]="Request shell execve() failed.\n";

	static char *messages[] = {
		"Too many environment variables to run batch request.\n",
		norun,
		deleted,
		(char *) 0,		/* RCM_2MANYENVARS */
		"Too many server arguments on server execve().\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_2MANYSVARGS */
		"Request aborted via a signal.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_ABORTED */
		"Corrupted request control and/or data files.\n",
		norun,
		failed,
		(char *) 0,		/* RCM_BADCDTFIL */
		"Bad argument passed to request server.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_BADSRVARG */
		"Request has been successfully to delivered to destination.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_DELIVERED */
		"Request delivery time expired.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_DELIVEREXP */
		"Request could not be delivered.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_DELIVERFAI */
		"Request was not delivered.\n",
		retryexc,
		deleted,
		(char *) 0,		/* RCM_DELIVERRETX */
		"Device open failed.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_DEVOPEFAI */
		"Unable to successfully start request because of a\n",
		"file descriptor shortage.\n",
		reque,
		(char *) 0,		/* RCM_ENFILERUN */
		"Unable to successfully start request because of a\n",
		"file system resource shortage.\n",
		reque,
		(char *) 0,		/* RCM_ENOSPCRUN */
		"Request executing.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_EXECUTING */
		"Request exited normally.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_EXITED */
		"Insufficient memory to start request.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_INSUFFMEM */
		"Server transaction processing aborted for NQS shutdown.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_INTERRUPTED */
		"Machine-id of request owner is no longer recognized\n",
		"by the execution machine.\n",
		norun,
		deleted,		/* RCM_MIDUNKNOWN */
		"Request could not be successfully delivered.  Previously\n",
		"routed request expired or was deleted at destination.\n",
		deleted,
		(char *) 0,		/* RCM_NETREQDEL */
		"No account authorization on execution machine for mapped\n",
		"request owner user-id.\n",
		norun,
		deleted,		/* RCM_NOACCAUTH */
		"Insufficient processes available to spawn request.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_NOMOREPROC */
		"Server bind() error.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_NONSECPORT */
		"Interrupted request prohibits restart on NQS rebuild.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_NORESTART */
		"Server for request did not return a completion code.\n",
		"Request failed.\n",
		failed,
		(char *) 0,		/* RCM_NOSVRETCODE */
		"Resolved stdout or stderr pathname of batch request\n",
		"at destination exceeds the maximum supported length.\n",
		deleted,
		(char *) 0,		/* RCM_PATHLEN */
		deleted,
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_PIPREQDEL */
		"NQS rebuild failure.  Request could not be requeued.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_REBUILDFAI */
		"Request collided with another previously existing\n",
		"request with the same request-id.  The newer request\n",
		"has been deleted.  Seek staff support.\n",
		(char *) 0,		/* RCM_REQCOLLIDE */
		"Request transaction failed.  Retry scheduled.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_RETRYLATER */
		"Request successfully routed for delivery to destination.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_ROUTED */
		"Request sent to local queue destination.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_ROUTEDLOC */
		"Request routing time expired.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_ROUTEEXP */
		"Request could not be routed.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_ROUTEFAI */
		"Request was not routed.\n",
		retryexc,
		deleted,
		(char *) 0,		/* RCM_ROUTERETX */
		"Unsupported server breakpoint encountered.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_SERBRKPNT */
		"Request server execve() failed.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_SEREXEFAI */
		"Server killed by unanticipated signal.\n",
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_SERVESIGERR */
		"Too many environment variables to run request.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_SHEXEF2BIG */
		shutdown,
		"The request was defined as unrestartable, and so the\n",
		"request has been deleted.\n",
		(char *) 0,		/* RCM_SHUTDNABORT */
		shutdown,
		"The request has been requeued for later restart.\n",
		(char *) 0,
		(char *) 0,		/* RCM_SHUTDNREQUE */
		shlbrkpnt,
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_SSHBRKPNT */
		shlexefai,
		reque,
		(char *) 0,
		(char *) 0,		/* RCM_SSHEXEFAI */
		"Output file successfully returned to destination.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_STAGEOUT */
		"Output file could not be returned to primary destination.\n",
		"Output file successfully returned to backup destination\n",
		"in user home directory on the execution machine.\n",
		(char *) 0,		/* RCM_STAGEOUTBAK */
		"Output file could not be returned to primary or backup\n",
		"destination.\n",
		(char *) 0,
		(char *) 0,		/* RCM_STAGEOUTFAI */
		"Unable to execute request.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_UNABLETOEXE */
		"Request failed.\n",
		failed,
		(char *) 0,
		(char *) 0,		/* RCM_UNAFAILURE */
		"Unable to create directly accessed (-re) stderr file.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_UNCRESTDERR */
		"Unable to create directly accessed (-ro) stdout file.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_UNCRESTDOUT */
		"Undefined RCM_ completion code.\n",
		(char *) 0,
		(char *) 0,
		(char *) 0,		/* RCM_UNDEFINED */
		shlbrkpnt,
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_USHBRKPNT */
		shlexefai,
		deleted,
		(char *) 0,
#ifndef SDSC
		(char *) 0		/* RCM_USHEXEFAI */
#else
		(char *) 0,		/* RCM_USHEXEFAI */
		"Problem getting the requested dedicated time slot.\n",
		deleted,
		(char *) 0,
		(char *) 0,		/* RCM_BADTSCHED */
		"Aborting NQS.\n",
		deleted,
		(char *) 0,
		(char *) 0		/* RCM_NQSABORT */
#endif
	};

	register short reason;		/* Reason bits of completion code */

	reason = (code & XCI_REASON_MASK);
	if (reason < 0 || reason > (RCM_MAXRCM & XCI_REASON_MASK) ||
	   (code & XCI_TRANSA_MASK)) {
		/*
		 *  The request completion code is invalid.
		 */
		reason = (RCM_UNDEFINED & XCI_REASON_MASK);
	}
	reason *= 4;
	fputs (prefix, stream);
	fputs (messages [reason++], stream);
	if (messages [reason] != (char *) 0) {
		fputs (prefix, stream);
		fputs (messages [reason++], stream);
	}
	if (messages [reason] != (char *) 0) {
		fputs (prefix, stream);
		fputs (messages [reason++], stream);
	}
	if (messages [reason] != (char *) 0) {
		fputs (prefix, stream);
		fputs (messages [reason++], stream);
	}
	fflush (stream);		/* For logfile */
}
