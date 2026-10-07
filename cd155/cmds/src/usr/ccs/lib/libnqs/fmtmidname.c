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
 
/*++ fmtmidname.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fmtmidname.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the principal name of the machine
 *	identified by the specified machine-id.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 29, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:44 $ $State: Exp $)
 * $Log: fmtmidname.c,v $
 * Revision 1.3  1994/11/19  02:26:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:06  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:35  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:44  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)fmtmidname.c	1.2 (fmtmidname.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#include NMAP			/* Include network mapping definitions */

/*
 *	External functions.
 */
extern char *nmap_get_nam();	/* Get the principal name of a machine */
				/* based upon the machine-id */


/*** fmtmidname
 *
 *
 *	char *fmtmidname():
 *
 *	Return a pointer to the principal name of the machine
 *	identified by the specified machine-id.
 */
char *fmtmidname (mid)
mid_t mid;				/* Machine-id */
{
	static char standinbuf [14];	/* Stand-in buffer */

	register char *name;		/* Pointer to name */

	name = nmap_get_nam (mid);
	if (name == (char *) 0) {
		/*
		 *  No such machine!  Format a "stand-in" name.
		 */
		sprintf (standinbuf, "[%1ld]", (long) mid);
		name = standinbuf;
	}
	return (name);
}
