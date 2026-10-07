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
 
/*++ serexit.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/serexit.c,v $
 *
 * DESCRIPTION:
 *
 *	Server exit routine.  ALL NQS servers should call this routine
 *	WHENEVER they wish to exit.
 *
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:50 $ $State: Exp $)
 * $Log: serexit.c,v $
 * Revision 1.3  1994/11/19  02:27:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:54  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:56  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:20  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"		/* NQS types and definitions */

extern char *strcpy();		/* String copy procedure */

/*** serexit
 *
 *
 *	void serexit():
 *
 *	Send a server-complete message to the shepherd process
 *	(parent of the server) on file descriptor 3, and exit(0).
 */
void serexit (rcm, servermssg)
long rcm;			/* Req/server completion code */
char servermssg [256];		/* Server message (can be NULL) */
{
	struct servermssg ret_packet;

	ret_packet.rcm = rcm;	/* Store req/server completion message */
	if (servermssg == (char *)0) ret_packet.mssg [0] = '\0';
	else strcpy (ret_packet.mssg, servermssg);
	/*
	 *  If you change the file descriptor that the req/server
	 *  completion message is reported on, then you must also
	 *  change nqs_spawn.c.
	 */
	write (3, (char *) &ret_packet, sizeof (ret_packet));
	exit (0);		/* Exit */
}
