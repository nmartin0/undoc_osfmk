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
 
/*++ shoalldev.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shoalldev.c,v $
 *
 * DESCRIPTION:
 *
 *	Show information about ALL devices on the local machine.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:52 $ $State: Exp $)
 * $Log: shoalldev.c,v $
 * Revision 1.3  1994/11/19  02:27:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:59  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:00  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:44  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern struct gendescr *nextdb();/* Return next allocated entry */
					/* from a database file */
extern int shodbydesc();		/* Show a device given a descr */

/*** shoalldev
 *
 *
 *	int shoalldev():
 *	Show status information on ALL of the local NQS devices.
 *
 *	WARNING:
 *		It is assumed that the current position
 *		of the device definitions file is 0.
 *
 *	Returns:
 *		0: If information about one or more devices
 *		   was displayed.
 *	       -1: If no output was produced.
 *
 */
int shoalldev (file)
register struct confd *file;		/* MUST be the NQS device file */
{
	int result;
	register struct gendescr *descr;

	result = -1;			/* No output produced */
	descr = nextdb (file);
	while (descr != (struct gendescr *)0) {
		if (shodbydesc (descr) == 0) {
			/*
			 *  Output was produced.
			 */
			result = 0;
			putchar ('\n');	/* Intervening space */
		}
		descr = nextdb (file);	/* Get the next device */
	}
	return (result);
}
