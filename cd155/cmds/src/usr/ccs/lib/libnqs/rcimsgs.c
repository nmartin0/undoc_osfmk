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
 
/*++ rcimsgs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/rcimsgs.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Diagnose request code information bits.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:38 $ $State: Exp $)
 * $Log: rcimsgs.c,v $
 * Revision 1.3  1994/11/19  02:27:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:22  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:38  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:42  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>		/* FILE definition */
#include "informcc.h"		/* NQS information completion codes */

/*** rcimsgs
 *
 *
 *	void rcimsgs():
 *	Diagnose request code information bits.
 */
void rcimsgs (code, stream, prefix, headerfn)
register long code;			/* Completion code */
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
register void (*headerfn)();		/* Header display function */
{
	register short preamble;	/* Preamble seen flag */

	preamble = -1;			/* No preamble yet */
	if (code & RCI_ACCESSDEN) {
		preamble = 0;
		(*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Access denied;\n", stream);
	}
	if (code & RCI_CLIMIDUNKN) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Client machine-id is unknown at transaction peer;\n",
		       stream);
	}
	if (code & RCI_EFBIG) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  File size limit exceeded;\n", stream);
	}
	if (code & RCI_FATALABORT) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Non-recoverable transaction failure;\n", stream);
	}
	if (code & RCI_MIDCONFLICT) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs (
		  "  Machine-id conflict between client and destination;\n",
		     stream);
	}
	if (code & RCI_NETNOTSUPP) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Networking not supported at NQS site.\n", stream);
	}
	if (code & RCI_NETPASSWD) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Network password verification error;\n", stream);
	}
	fflush (stream);		/* For logfile */
	if (code & RCI_NOSUCHFORM) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  No such device forms;\n", stream);
	}
	if (code & RCI_NOSUCHQUE) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  No such queue;\n", stream);
	}
	if (code & RCI_PEERINTERR) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  NQS internal error at transaction peer;\n", stream);
	}
	if (code & RCI_PEERMIDUNKN) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Local machine-id is unknown at transaction peer;\n",
		       stream);
	}
	if (code & RCI_PEERNETDB) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Network database error at transaction peer;\n",
		       stream);
	}
	if (code & RCI_PEERNOACATH) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  No account authorization at transaction peer;\n",
		       stream);
	}
	fflush (stream);		/* For logfile */
	if (code & RCI_PROTOFAIL) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  NQS protocol failure;\n", stream);
	}
	if (code & RCI_QUOTALIMIT) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Explicit request quota limits exceed maximums;\n",
		       stream);
	}
	if (code & RCI_RRFUNKNMID) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs (
		  "  Request refers to mids unknown at transaction peer;\n",
		   stream);
	}
	if (code & RCI_WROQUETYP) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Wrong queue type for request;\n", stream);
	}
	if (code & RCI_UNAFAILURE) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Unanticipated transaction failure;\n", stream);
	}
	fflush (stream);		/* For logfile */
}
