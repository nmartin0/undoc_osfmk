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
 
/*++ pipeqdest.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/pipeqdest.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Return the machine-id, queue name, and retry count values for
 *	the specified pipe queue destination (as defined in the environment
 *	of a pipe queue server).
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 8, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:31 $ $State: Exp $)
 * $Log: pipeqdest.c,v $
 * Revision 1.3  1994/11/19  02:27:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:08  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:32  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:15  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include NMAP				/* Network mapping stuff (mid_t) */

extern char *getenv();			/* Get environment variable */


/*** pipeqdest
 *
 *
 *	char *pipeqdest():
 *
 *	Return the machine-id, time spent in retry state, and queue name
 *	values for the specified pipe queue destination (as defined in the
 *	environment of a pipe queue server).
 *
 *	Returns:
 *		A pointer to the name of the destination queue (if
 *		the specified queue destination exists).  Otherwise,
 *		(char *) 0 is returned.
 */
char *pipeqdest (destno, destmid, retrytime)
int destno;				/* Destination number [1..n] */
mid_t *destmid;				/* Machine-id of destination */
long *retrytime;			/* Time spent in retry state */
{
	char envvarname [5];		/* Environment var name */
	register char *dest;		/* Ptr to dest environment var */
	register unsigned long ulong;	/* For scanning */
	register char ch;		/* Scan character */
	register short neg;		

	if (destno < 1 || destno > 1000) {
		return ((char *) 0);	/* No such destination */
	}
	sprintf (envvarname, "D%03d", destno-1);
	dest = getenv (envvarname);	/* Get the environment var for dest */
	if (dest == (char *) 0) {
		return ((char *) 0);	/* No such destination */
	}
	ch = *dest;			/* Get scan character */
	while (ch == ' ') {		/* Scan any whitespace */
		ch = *++dest;
	}

	if (ch == '-') {		/* Scan any for negative */
		ch = *++dest;
		neg = -1;
	}
	else
		neg =1;

	ulong = 0;
	while (ch >= '0' && ch <= '9') {/* Scan destination machine-id */
		ulong *= 10;
		ulong += ch - '0';
		ch = *++dest;
	}
	*destmid = ulong * neg;
	while (ch == ' ') {		/* Scan any whitespace */
		ch = *++dest;
	}
	ulong = 0;
	while (ch >= '0' && ch <= '9') {/* Scan destination machine-id */
		ulong *= 10;
		ulong += ch - '0';
		ch = *++dest;
	}
	*retrytime = ulong;
	while (ch == ' ') {		/* Scan any whitespace */
		ch = *++dest;
	}
	return (dest);			/* Return pointer to queue name */
}
