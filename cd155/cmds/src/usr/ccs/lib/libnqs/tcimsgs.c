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
 
/*++ tcimsgs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/tcimsgs.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Diagnose transaction code information bits.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:59 $ $State: Exp $)
 * $Log: tcimsgs.c,v $
 * Revision 1.3  1994/11/19  02:27:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:52  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:11  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:29  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>		/* FILE definition */
#include "informcc.h"		/* NQS information completion codes */

/*** tcimsgs
 *
 *
 *	void tcimsgs():
 *	Diagnose transaction code information bits.
 */
void tcimsgs (code, stream, prefix, headerfn)
register long code;			/* Completion code */
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
register void (*headerfn)();		/* Header display function */
{
	register short preamble;	/* Preamble seen flag */

	preamble = -1;			/* No preamble yet */
	if (code & TCI_COPLIMEXC) {
		preamble = 0;
		(*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Copy limit;\n", stream);
	}
	if (code & TCI_PP_CFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process core-file size limit;\n", stream);
	}
	if (code & TCI_PP_CTLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process CPU time limit;\n", stream);
	}
	if (code & TCI_PP_DSLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process data-segment limit;\n", stream);
	}
	if (code & TCI_PP_MSLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process memory size limit;\n", stream);
	}
	if (code & TCI_PP_NELEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process nice execution priority limit;\n",
		       stream);
	}
	fflush (stream);		/* For logfile */
	if (code & TCI_PP_PFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process permanent file size limit;\n", stream);
	}
	if (code & TCI_PP_QFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process quick file size quota limit;\n", stream);
	}
	if (code & TCI_PP_SSLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process stack-segment limit;\n", stream);
	}
	if (code & TCI_PP_TFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process temporary file size limit;\n", stream);
	}
	if (code & TCI_PP_WSLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-process working set quota limit;\n", stream);
	}
	if (code & TCI_PRILIMEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Print/output size limit;\n", stream);
	}
	fflush (stream);		/* For logfile */
	if (code & TCI_PR_CTLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request CPU time limit;\n", stream);
	}
	if (code & TCI_PR_DRIEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request tape drive quota limit;\n", stream);
	}
	if (code & TCI_PR_MSLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request memory space limit;\n", stream);
	}
	if (code & TCI_PR_NCPEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request CPU quota limit;\n", stream);
	}
	if (code & TCI_PR_PFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request permanent file space limit;\n", stream);
	}
	if (code & TCI_PR_QFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request quick file space quota limit;\n", stream);
	}
	if (code & TCI_PR_TFLEXC) {
		if (!(++preamble)) (*headerfn)(stream, prefix);
		fputs (prefix, stream);
		fputs ("  Per-request temporary file space limit;\n", stream);
	}
	fflush (stream);		/* For logfile */
}
