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
 
/*++ sholbymach.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/sholbymach.c,v $
 *
 * DESCRIPTION:
 *
 *	Print on stdout the limits that are meaningful for a machine.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	January 13, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:55 $ $State: Exp $)
 * $Log: sholbymach.c,v $
 * Revision 1.3  1994/11/19  02:27:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:40  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:05  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:11  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>			/* for stderr */
#include "nqs.h"			/* NQS types and definitions */
#include NETDB				/* Network database header file; */
					/* Proto/Makefile will tell us */
					/* where to find netdb.h */

extern struct hostent *gethostbyname();	/* Get hostent structure for name */
extern int localmid();			/* Get local machine-id */
extern int nmap_get_mid();		/* Get machine-id for name */
extern int shoalllim();			/* Show all (local) limits */


/*** sholbymach
 *
 *
 *	int sholbymach():
 *
 *	Print on stdout the limits that are meaningful
 *	for the named machine.
 *
 *	Returns: 0 if output was produced.
 *		-1 if no output was produced.
 */
int sholbymach (paramfile, itsname, flags)
struct confd *paramfile;		/* This pointer useful for */
					/* local case only */
char *itsname;				/* The machine in question */
long flags;				/* SHO_? */
{
	mid_t itsmid;
	mid_t mymid;
	struct hostent *ent;		/* Host table entry structure */

	if ((ent = gethostbyname (itsname)) == (struct hostent *) 0) {
		fprintf (stderr, "Invalid hostname specification: ");
		fprintf (stderr, "%s.\n", itsname);
		return (0);
	}
	switch (nmap_get_mid (ent, &itsmid)) {
	case NMAP_SUCCESS:		/* Successfully got machine-id */
		break;
	case NMAP_ENOMAP:		/* What?  No mid defined! */
		fprintf (stderr, "Invalid hostname specification: ");
		fprintf (stderr, "%s.\n", itsname);
		return (0);
	case NMAP_ENOPRIV:		/* No privilege */
		fprintf (stderr, "Unexpected error in nmap_get_mid(), ");
		fprintf (stderr, "machine = %s.\n", itsname);
		return (0);
	default:
		fprintf (stderr, "Unexpected error in nmap_get_mid(), ");
		fprintf (stderr, "machine = %s.\n", itsname);
		return (0);
	}
	if (localmid (&mymid) != 0) {
		fprintf (stderr, "Unexpected error in localmid().\n");
		return (0);
	}
	if (itsmid == mymid) {
		if (telldb (paramfile) != -1) {	/* if we have read before, */
			seekdb (paramfile, 0L);	/* seek to beginning */
		}
		return (shoalllim (paramfile, flags));
	}
	else {
		fprintf (stderr, "You are asking about a remote machine.\n");
		fprintf (stderr, "Networking is not supported in this ");
		fprintf (stderr, "implementation.\n");
		return (0);
	}
}
