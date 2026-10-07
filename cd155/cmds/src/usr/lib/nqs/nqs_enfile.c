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
 
/*++ nqs_enfile.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_enfile.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine if the last file open error operation was caused
 *	by a lack of file descriptors, and if so--display the proper
 *	error output.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:55 $ $State: Exp $)
 * $Log: nqs_enfile.c,v $
 * Revision 1.3  1994/11/19  02:52:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:25  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:03  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:54  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <errno.h>
#include "nqs.h"
#include "nqsxvars.h"			/* Global vars */

extern int errno;			/* Error number */

/*** nqs_enfile
 *
 *
 *	int nqs_enfile():
 *	Diagnose a shortage of file descriptors.
 *
 *	Returns:
 *		0: if errno is NOT either EMFILE or ENFILE.
 *		1: if errno is either EMFILE or ENFILE in which
 *		   case error output is generated.
 */
int nqs_enfile ()
{
	if (errno == ENFILE || errno == EMFILE) {
		if (errno == ENFILE) {
			printf ("E$Unable to open file.\n");
			printf ("I$System file table full.\n");
			fflush (stdout);
			return (1);
		}
		printf ("F$Unable to open file.\n");
		printf ("I$Max open files per-process limit reached.\n");
		fflush (stdout);
		return (1);
	}
	return (0);		/* Nope, not ENFILE or EMFILE. */
}
