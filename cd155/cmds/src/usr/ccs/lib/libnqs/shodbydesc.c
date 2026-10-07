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
 
/*++ shodbydesc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shodbydesc.c,v $
 *
 * DESCRIPTION:
 *
 *	Show status information for the specified local NQS device.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:54 $ $State: Exp $)
 * $Log: shodbydesc.c,v $
 * Revision 1.3  1994/11/19  02:27:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:19:06  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:03  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:59  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"

extern int gethostname();	/* Get name of local host */
extern char *getmacnam();	/* Get machine-name from machine-id */
extern char *getusenam();	/* Get user-name from user-id */

/*** shodbydesc
 *
 *
 *	int shodbydesc():
 *	Show status information for the specified local NQS device.
 *
 *	Returns:
 *		This function always returns 0.
 *
 */
int shodbydesc (descr)
register struct gendescr *descr;	/* Device database descriptor */
{
	char hostname [256];		/* 255-char host names */

	gethostname (hostname, 255);	/* Get the name of the local host */
	hostname [255] = '\0';		/* Make absolutely sure that it is */
					/* null terminated */
	printf ("  %s@%s\n", descr->v.dev.dname, hostname);
	printf ("  Fullname: %s\n", descr->v.dev.fullname);
	printf ("  Server: %s\n", descr->v.dev.server);
	printf ("  Forms: %s\n", descr->v.dev.forms);
	printf ("  Status = [");
	if (descr->v.dev.status & DEV_ENABLED) printf ("ENABLED, ");
	else if (descr->v.dev.status & DEV_ACTIVE) {
		printf ("ENABLED/CLOSED, ");
	}
	else printf ("DISABLED, ");
	if (descr->v.dev.status & DEV_FAILED) printf ("FAILED");
	else if (descr->v.dev.status & DEV_ACTIVE) printf ("ACTIVE");
	else printf ("INACTIVE");
	if (descr->v.dev.statmsg [0] != '\0') {
		/*
		 *  Device status message text is defined for this device.
		 *  Display it.
		 */
		printf (", %s", descr->v.dev.statmsg);
	}
	printf ("];\n");
	if (descr->v.dev.status & DEV_ACTIVE) {
		/*
		 *  The device is active and handling a request.
		 *  Display a certain amount of information about
		 *  the current request being handled.
		 */
		printf ("  Request name=%s;  id=%1ld.%s\n",
			descr->v.dev.reqname, descr->v.dev.orig_seqno,
			getmacnam (descr->v.dev.orig_mid));
		printf ("  Owner=%8s;\n", getusenam (descr->v.dev.uid));
	}
	return (0);			/* Output produced */
}
