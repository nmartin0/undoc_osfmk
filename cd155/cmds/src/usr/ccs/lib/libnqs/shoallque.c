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
 
/*++ shoallque.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shoallque.c,v $
 *
 * DESCRIPTION:
 *
 *	Show information about ALL queues on the local machine as
 *	selected by the request selection and display criteria.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:53 $ $State: Exp $)
 * $Log: shoallque.c,v $
 * Revision 1.4  1994/11/19  02:27:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/11/02  18:29:58  kremenek
 *  Reviewer: davidl doyle
 *  Risk: low
 *  Benefit or PTS #: 11271
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/shoallque.c
 *
 * Revision 1.2  1992/10/09  20:19:04  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:02  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:51  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"

extern struct gendescr *nextdb();	/* Return next allocated entry */
					/* from a database file */
extern int shoqbydesc();			/* Show a queue */

/*** shoallque
 *
 *
 *	int shoallque():
 *	Show status information on ALL of the local NQS queues.
 *
 *	WARNING:
 *		It is assumed that the current position of
 *		the queue definitions file is 0.
 *
 *	Returns:
 *		0: If output was produced.
 *	       -1: If no output was produced.
 *
 */
int shoallque_NOT_USED_NOW (file, flags, whomuid, reqs, daepresent, qmapfile, pipeqfile,
	qcomplexfile)
register struct confd *file;		/* MUST be the NQS queue file */
register long flags;			/* Display flags */
uid_t whomuid;				/* Whom we are interested in */
register struct reqset *reqs;		/* Req select set */
short daepresent;			/* Boolean non-zero if the local */
					/* NQS daemon is present and running */
struct confd *qmapfile;			/* Queue/device/destination mapping */
					/* file */
struct confd *pipeqfile;		/* Pipe-queue destination file */
struct confd *qcomplexfile;		/* Queue complex definition file */
{
	int result;
	register struct gendescr *descr;

	result = -1;			/* No output produced */
	descr = nextdb (file);
	while (descr != (struct gendescr *)0) {
		if (shoqbydesc (file, descr, flags, whomuid, reqs,
			daepresent, qmapfile, pipeqfile, qcomplexfile) == 0) {
			/*
			 *  Output was produced.
			 */
			result = 0;
			putchar ('\n');	/* Separate queue info by blank line */
		}
		descr = nextdb (file);	/* Get the next queue */
	}
	return (result);
}
