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
 
/*++ analyzetcm.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/analyzetcm.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Completely analyze and diagnose a transaction completion
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
 * $Log: analyzetcm.c,v $
 * Revision 1.3  1994/11/19  02:26:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:20  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:12  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:32  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>		/* FILE definition */
#include "informcc.h"		/* NQS information completion codes */
#include "transactcc.h"		/* NQS transaction completion codes */

extern void tcimsgs();		/* Diagnose transaction information bits */
extern void tcmmsgs();		/* Diagnose transaction reason bits */


/*** analyzetcm
 *
 *
 *	void analyzetcm():
 *
 *	Completely analyze and diagnose a transaction completion
 *	code.
 */
void analyzetcm (code, stream, prefix)
register long code;			/* Completion code */
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	void boundpreamble();		/* Display bounds message */
	void exceedpreamble();		/* Display exceed message */

	register short reason;		/* Reason bits of completion code */

	reason = (code & XCI_REASON_MASK);
	if (reason < 0 || reason > (TCM_MAXTCM & XCI_REASON_MASK) ||
	   (code & XCI_TRANSA_MASK) == 0) {
		/*
		 *  The transaction code is invalid.
		 */
		reason = (TCML_UNDEFINED & XCI_REASON_MASK);
		code = TCML_UNDEFINED;
	}
	else if (reason == (TCML_UNDEFINED & XCI_REASON_MASK)) {
		/*
		 *  Clear information bits if undefined transaction code.
		 */
		code &= ~XCI_INFORM_MASK;
	}
	/*
	 *  Analyze and describe a specific transaction code.
	 */
	tcmmsgs (code, stream, prefix);
	/*
	 *  Analyze and describe any additional information bits
	 *  set in the transaction completion code.
	 */
	if (reason == (TCML_QUOTALIMIT & XCI_REASON_MASK)) {
		tcimsgs (code, stream, prefix, exceedpreamble);
	}
	else if (reason == (TCML_SUBMITTED & XCI_REASON_MASK)) {
		tcimsgs (code, stream, prefix, boundpreamble);
	}
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


/*** exceedpreamble
 *
 *
 *	void exceedpreamble():
 *	Display limit too large to queue preamble.
 */
static void exceedpreamble (stream, prefix)
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	fputc ('\n', stream);
	fputs (prefix, stream);
	fputs ("The request could not be queued or delivered because the\n",
		stream);
	fputs (prefix, stream);
	fputs ("following explicit request quota limit(s) exceeded the\n",
		stream);
	fputs (prefix, stream);
	fputs ("corresponding limits of the target queue:\n\n", stream);
	fflush (stream);		/* For output to logfile */
}
