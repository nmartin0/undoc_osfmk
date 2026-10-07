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
 
/*++ shodbyname.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shodbyname.c,v $
 *
 * DESCRIPTION:
 *
 *	Print on stdout information about a device.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	December 18, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:54 $ $State: Exp $)
 * $Log: shodbyname.c,v $
 * Revision 1.3  1994/11/19  02:27:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:36  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:04  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:07  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>			/* for stderr */
#include "nqs.h"			/* NQS types and definitions */

extern char *destdev();
extern int localmid();
extern int machspec();
extern struct gendescr *nextdb();
extern int seekdb();
extern int shodbydesc();
extern long telldb();		/* Return position of last read on an NQS */
				/* database file */

/*** shodbyname
 *
 *
 *	int shodbyname():
 *
 *	Print on stdout information about the named device.
 *	Returns: 0 if output was produced.
 *		-1 if no output was produced.
 */
int shodbyname (devicefile, fullname)
struct confd *devicefile;
char *fullname;
{
	struct gendescr *descr;
	int found;
	mid_t itsmid;
	mid_t mymid;
	char *localname;

	switch (machspec (fullname, &itsmid)) {
		case 0:
			break;
		case -1:
		case -2:
			fprintf (stderr, "Invalid hostname specification ");
			fprintf (stderr, "for device %s.\n", fullname);
			return (0);
		case -3:
		case -4:
			fprintf (stderr, "Unexpected error in machspec(), ");
			fprintf (stderr, "device = %s.\n", fullname);
			return (0);
	}
	if (localmid (&mymid) != 0) {
		fprintf (stderr, "Unexpected error in localmid().\n");
		return (0);
	}
	if (itsmid == mymid) {
		found = 0;
		if (telldb (devicefile) != -1) {
			/*
			 *  We are not necessarily at the start of the
			 *  database file....
			 */
			seekdb (devicefile, 0L);/* Seek to the beginning */
		}
		descr = nextdb (devicefile);
		localname = destdev (fullname);
		while (descr != NULL && !found) {
			if (strcmp (descr->v.dev.dname, localname) == 0) {
				found = 1;
			}
			else descr = nextdb (devicefile);
		}
		if (found) return (shodbydesc (descr));
		else {
			fprintf (stderr, "Device: %s does not exist.\n",
				 fullname);
			return (0);
		}
	}
	else {
		fprintf (stderr, "You are asking about a remote device.\n");
		fprintf (stderr, "Networking is not supported in this ");
		fprintf (stderr, "implementation.\n");
		return (0);
	}
}
