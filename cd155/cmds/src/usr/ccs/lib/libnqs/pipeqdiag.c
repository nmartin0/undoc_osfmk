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
 
/*++ pipeqdiag.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/pipeqdiag.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains four (4) functions that diagnose
 *	pipe queue transaction codes.  These four functions are
 * 	used by pipe queue servers to analyze the severity of
 *	transaction codes.
 *
 *	The four functions are:
 *
 *		int	pipeqretry(),
 *		int	pipeqenable(),
 *		void	pipeqexitifbad(),	and
 *		long	pipeqfailinfo().
 *
 *	Pipeqretry() accepts a transaction code, and returns
 *	a code indicating the need to reschedule the destination,
 *	and also a judgement on whether the destination might
 *	ever accept the request in the future.
 *
 *	Pipeqenable() accepts a transaction code, and returns
 *	boolean TRUE (non-zero), if a message packet should be
 *	sent to inform the local NQS daemon that the queue dest-
 *	ination associated with the transaction code, should be
 *	completely enabled (retry mode exited).
 *
 *	Pipeqexitifbad() accepts a transaction code, and exits
 *	if that transaction code is fatal (i.e, there is no point in
 *	retrying ANY destination for the request in the foreseeable
 *	future.)
 *
 *	Pipeqfailinfo() accepts a FAILED transaction code, and returns
 *	a value that later can be or'd into a request completion
 *	code, should all destinations fail in non-retriable ways.
 *	Pipeqfailinfo() is not useful unless called with a
 *	non-retriable, non-fatal transaction code.
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 25, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:27:32 $ $State: Exp $)
 * $Log: pipeqdiag.c,v $
 * Revision 1.5  1994/11/19  02:27:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/05/28  20:08:56  mwan
 * Change to mail the correct bad dedicated time slot message
 *
 * Revision 1.2  1992/10/09  20:18:10  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:33  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:19  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "informcc.h"		/* NQS information completion codes */
#include "requestcc.h"		/* NQS request completion codes */
#include "transactcc.h"		/* NQS transaction completion codes */

extern void serexit ();		/* Server exit */

/*** pipeqretry
 *
 *
 *	int pipeqretry():
 *
 *	Identify a possibly retriable transaction event for a destination
 *	from the transaction completion code.
 *
 *	Returns:
 *		+1: if the transaction completion code indicates
 *		    a condition at the destination such that it
 *		    is possible that the request might someday be
 *		    queued and/or delivered to that destination
 *		    after waiting a suitable amount of time (i.e.
 *		    TCMP_NOLOCALDAE).
 *
 *		-1: if the transaction completion code indicates
 *		    a serious condition or failure at the destination
 *		    such that the destination should be disabled for
 *		    an appropriate length of time in the hope that
 *		    the situation at the destination machine will
 *		    be fixed.  In all likelihood though, it will
 *		    never be possible to queue and/or deliver the
 *		    request to the destination in question (i.e.
 *		    TCMP_NETDBERR).
 *
 *		 0: if the transaction completion code indicates
 *		    a condition whereby the destination was fine,
 *		    but that the request will never be accepted
 *		    there (i.e. TCMP_ACCESSDEN).
 */
int pipeqretry (code)
register long code;		/* Completion code */
{
	code &= XCI_FULREA_MASK;/* Clear information bits */
	switch (code) {
	case TCMP_CONNBROKEN:
	case TCMP_CONNTIMOUT:
	case TCML_ENFILE:
	case TCMP_ENFILE:
	case TCML_ENOBUFS:
	case TCMP_ENOBUFS:
	case TCML_ENOMEM:
	case TCMP_ENOMEM:
	case TCML_ENOSPC:
	case TCMP_ENOSPC:
	case TCML_ERRORRETRY:
	case TCMP_ERRORRETRY:
	case TCML_ETIMEDOUT:
#ifndef SDSC
	case TCML_INSQUESPA:
	case TCMP_INSQUESPA:
#endif
	case TCMP_MAXNETCONN:
	case TCML_NOESTABLSH:
	case TCMP_NOESTABLSH:
	case TCML_NOLOCALDAE:
	case TCMP_NOLOCALDAE:
	case TCML_NOMOREPROC:
	case TCMP_NOMOREPROC:
	case TCMP_NONETDAE:
	case TCML_NOPORTAVAI:
	case TCML_QUEDISABL:
	case TCMP_QUEDISABL:
		return (1);	/* Reschedule destination, and the request */
				/* might someday be accepted here */
	case TCML_ACCESSDEN:
	case TCMP_ACCESSDEN:
	case TCML_FATALABORT:
	case TCMP_FATALABORT:
#ifdef SDSC
	case TCML_INSQUESPA:
	case TCMP_INSQUESPA:
#endif
	case TCMP_NOACCAUTH:
	case TCML_NOSUCHFORM:
	case TCMP_NOSUCHFORM:
	case TCML_QUOTALIMIT:
	case TCMP_QUOTALIMIT:
	case TCMP_RRFUNKNMID:
	case TCML_WROQUETYP:
	case TCMP_WROQUETYP:
		return (0);	/* The request will never be accepted at */
				/* this destination */
	default:
		return (-1);	/* Reschedule destination, although the */
	}			/* request will probably never be accepted */
}				/* here */


/*** pipeqenable
 *
 *
 *	int pipeqenable():
 *
 *	Analyze a transaction code, and return boolean TRUE (non-zero),
 *	if a message packet should be sent informing the local NQS
 *	daemon that the queue destination associated with the transaction
 *	code, should be completely enabled (retry mode exited).
 */
int pipeqenable (code)
register long code;			/* Completion code */
{
	code &= XCI_FULREA_MASK;	/* Clear information bits */
	switch (code) {
	case TCML_ACCESSDEN:
	case TCMP_ACCESSDEN:
	case TCML_BADCDTFIL:
	case TCMP_BADCDTFIL:
	case TCML_COMPLETE:
	case TCMP_COMPLETE:
	case TCMP_CONTINUE:
	case TCML_NOACCAUTH:
	case TCMP_NOACCAUTH:
	case TCML_NOSUCHFORM:
	case TCMP_NOSUCHFORM:
	case TCML_NOSUCHREQ:
	case TCMP_NOSUCHREQ:
	case TCML_NOSUCHSIG:
	case TCMP_NOSUCHSIG:
	case TCML_NOTREQOWN:
	case TCMP_NOTREQOWN:
	case TCML_PATHLEN:
	case TCMP_PATHLEN:
	case TCML_QUOTALIMIT:
	case TCMP_QUOTALIMIT:
	case TCML_REQCOLLIDE:
	case TCMP_REQCOLLIDE:
	case TCML_REQDELETE:
	case TCMP_REQDELETE:
	case TCML_REQRUNNING:
	case TCMP_REQRUNNING:
	case TCML_REQSIGNAL:
	case TCMP_REQSIGNAL:
	case TCMP_RRFUNKNMID:
	case TCML_SUBMITTED:
	case TCMP_SUBMITTED:
	case TCML_WROQUETYP:
	case TCMP_WROQUETYP:
		return (1);		/* Return TRUE */
	}
	return (0);
}


/*** pipeqexitifbad
 *
 *
 *	void pipeqexitifbad ():
 *
 *	Analyze the transaction code.  If the code indicates
 *	that there is no point in trying ANY more destinations,
 *	exit.  Otherwise return.
 *
 */
void pipeqexitifbad (code)
register long code;			/* Completion code */
{
	code &= XCI_FULREA_MASK;	/* Clear information bits */
	switch (code) {
	case TCML_BADCDTFIL:
		serexit (RCM_BADCDTFIL, (char *) 0);
	case TCMP_BADCDTFIL:
		/*
		 *  The remote peer machine dropped the ball.
		 *  The only thing we can do is silently clean up.
		 */
		serexit (RCM_PIPREQDEL, (char *) 0);
#ifdef SDSC
	case TCML_INSQUESPA:
	case TCMP_INSQUESPA:
		serexit (RCM_BADTSCHED, (char *) 0);
#endif
	case TCML_NOACCAUTH:
		serexit (RCM_NOACCAUTH, (char *) 0);
	case TCMP_NONSECPORT:
		serexit (RCM_NONSECPORT, (char *) 0);
	case TCML_NOSUCHREQ:
	case TCMP_NOSUCHREQ:
		serexit (RCM_PIPREQDEL, (char *) 0);
	case TCML_PATHLEN:
	case TCMP_PATHLEN:
		serexit (RCM_PATHLEN, (char *) 0);
	case TCML_REQCOLLIDE:
	case TCMP_REQCOLLIDE:
		serexit (RCM_REQCOLLIDE, (char *) 0);
	}
}


/*** pipeqfailinfo
 *
 *
 *	long pipeqfailinfo():
 *
 *	Return the proper RCI_ information bit code for the specified
 *	FAILED transaction completion code.
 */
long pipeqfailinfo (code)
register long code;			/* Completion code */
{
	code &= XCI_FULREA_MASK;	/* Clear information bits */
	switch (code) {
	case TCML_ACCESSDEN:
	case TCMP_ACCESSDEN:
		return (RCI_ACCESSDEN);
	case TCMP_CLIMIDUNKN:
		return (RCI_CLIMIDUNKN);
	case TCML_EFBIG:
	case TCMP_EFBIG:
		return (RCI_EFBIG);
	case TCML_FATALABORT:
	case TCMP_FATALABORT:
		return (RCI_FATALABORT);
	case TCMP_INTERNERR:
		return (RCI_PEERINTERR);
	case TCMP_MIDCONFLCT:
		return (RCI_MIDCONFLICT);
	case TCMP_NETDBERR:
		return (RCI_PEERNETDB);
	case TCML_NETNOTSUPP:
		return (RCI_NETNOTSUPP);
	case TCMP_NETPASSWD:
		return (RCI_NETPASSWD);
	case TCMP_NOACCAUTH:
		return (RCI_PEERNOACATH);
	case TCML_NOSUCHFORM:
	case TCMP_NOSUCHFORM:
		return (RCI_NOSUCHFORM);
	case TCML_NOSUCHQUE:
	case TCMP_NOSUCHQUE:
		return (RCI_NOSUCHQUE);
	case TCML_PROTOFAIL:
	case TCMP_PROTOFAIL:
		return (RCI_PROTOFAIL);
	case TCML_QUOTALIMIT:
	case TCMP_QUOTALIMIT:
		return (RCI_QUOTALIMIT);
	case TCMP_RRFUNKNMID:
		return (RCI_RRFUNKNMID);
	case TCMP_SELMIDUNKN:
		return (RCI_PEERMIDUNKN);
	case TCML_WROQUETYP:
	case TCMP_WROQUETYP:
		return (RCI_WROQUETYP);
	default:
		return (RCI_UNAFAILURE);
	}
}
