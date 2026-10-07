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
 
/*++ analyzercm.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/analyzercm.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Completely analyze and diagnose a request completion
 *	code.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:17 $ $State: Exp $)
 * $Log: analyzercm.c,v $
 * Revision 1.3  1994/11/19  02:26:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:18  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:10  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:28  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>		/* FILE definition */
#include "informcc.h"		/* NQS information completion codes */
#include "requestcc.h"		/* NQS request completion codes */

extern void rcimsgs();		/* Diagnose request completion info bits */
extern void rcmmsgs();		/* Diagnose request completion reason bits */
extern void tcimsgs();		/* Diagnose transaction information bits */
extern void tcmmsgs();		/* Diagnose transaction reason bits */


/*** analyzercm
 *
 *
 *	void analyzercm():
 *
 *	Completely analyze and diagnose a request completion
 *	code.
 */
void analyzercm (code, stream, prefix)
register long code;			/* Completion code */
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	void applypreamble();		/* Display limit message */
	void boundpreamble();		/* Display bounds preamble */
	void routefaipreamble();	/* Display route preamble */

	register short reason;		/* Reason bits of completion code */

	reason = (code & XCI_REASON_MASK);
	if (reason < 0 || reason > (RCM_MAXRCM & XCI_REASON_MASK) ||
	   (code & XCI_TRANSA_MASK)) {
		/*
		 *  The request completion code is invalid.
		 */
		reason = (RCM_UNDEFINED & XCI_REASON_MASK);
		code = RCM_UNDEFINED;
	}
	else if (reason == (RCM_UNDEFINED & XCI_REASON_MASK)) {
		/*
		 *  Clear information bits if undefined request completion
		 *  code.
		 */
		code &= ~XCI_INFORM_MASK;
	}
	/*
	 *  Analyze and describe a specific request completion code.
	 */
	rcmmsgs (code, stream, prefix);
	/*
	 *  Analyze and describe any additional information bits
	 *  set in the request completion code.
	 */
	if (reason == RCM_DELIVERFAI || reason == RCM_DELIVERRETX) {
		fputc ('\n', stream);
		fputs (prefix, stream);
		fputs ("Transaction failure reason:\n", stream);
		fflush (stream);	/* For output to logfile */
		tcmmsgs (code >> 10, stream, prefix);
	}
	else if (reason == RCM_STAGEOUTBAK || reason == RCM_STAGEOUTFAI) {
		fputc ('\n', stream);
		fputs (prefix, stream);
		fputs ("Transaction failure reason at primary destination:\n",
			stream);
		fflush (stream);	/* For output to logfile */
		tcmmsgs (code >> 10, stream, prefix);
	}
	else if (reason == RCM_ROUTED && reason == RCM_ROUTEDLOC &&
		 reason == RCM_DELIVERED) {
		/*
		 *  For the moment, we allow RCM_DELIVERED
		 *  to have quota information bits in this
		 *  ****TRIAGE**** implementation.
		 */
		tcimsgs (code, stream, prefix, boundpreamble);
	}
	else if (reason == RCM_ROUTEFAI || reason == RCM_ROUTERETX) {
		rcimsgs (code, stream, prefix, routefaipreamble);
	}
	else if (reason == RCM_UNABLETOEXE) {
		tcimsgs (code, stream, prefix, applypreamble);
	}
}


/*** applypreamble
 *
 *
 *	void applypreamble():
 *	Display limit application preamble.
 */
static void applypreamble (stream, prefix)
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	fputc ('\n', stream);
	fputs (prefix, stream);
	fputs ("Problem enforcing the following limit(s):\n\n", stream);
	fflush (stream);
}


/*** boundpreamble
 *
 *
 *	void boundpreamble():
 *	Display bounded limit preamble.
 */
static void boundpreamble (stream, prefix)
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	fputc ('\n', stream);
	fputs (prefix, stream);
	fputs ("In order to successfully queue or deliver the request,\n",
		stream);
	fputs (prefix, stream);
	fputs ("it was necessary to alter the following explicit request\n",
		stream);
	fputs (prefix, stream);
	fputs ("quota limit(s), binding them to values within the ranges\n",
		stream);
	fputs (prefix, stream);
	fputs ("enforceable at the execution machine:\n\n", stream);
	fflush (stream);		/* For output to logfile */
}


/*** routefaipreamble
 *
 *
 *	void routefaipreamble():
 *	Display pipe queue request routing failure preamble.
 */
static void routefaipreamble (stream, prefix)
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	fputc ('\n', stream);
	fputs (prefix, stream);
	fputs ("The request could not be routed to any of the possible\n",
		stream);
	fputs (prefix, stream);
	fputs (
	  "pipe queue destinations because of the following reason(s):\n\n",
	   stream);
	fflush (stream);		/* For output to logfile */
}
