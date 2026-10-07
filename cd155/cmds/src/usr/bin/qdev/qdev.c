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
 
/*++ qdev.c - Network Deviceing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qdev/qdev.c,v $
 *
 * DESCRIPTION:
 *
 *	Display device status information.
 *	This program must run as a setuid root program.
 *
 *
 * RETURNS:
 *	
 * 	0	-  if output produced
 *	1	-  if an error occurred. A message is sent to the standard
 *			output file for every error.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 24, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 01:35:26 $ $State: Exp $)
 * $Log: qdev.c,v $
 * Revision 1.3  1994/11/19  01:35:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:29:47  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:26:52  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:59:03  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:05  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)qdev.c	1.2 (qdev.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqs.h"
#include "nqsdirs.h"			/* NQS files and directories */


/*
 *	External functions:
 */
extern void bufstdout();		/* Block buffer stdout */
extern struct confd *opendb();		/* Open an NQS database file */
extern int shoalldev();			/* Show all devices */
extern int shodbyname();		/* Show a device given by name */

/*
 *	Global variables:
 */
char *Qdev_prefix = "Qdev";

/*** main
 *
 *
 *	qdev [ <device-name(s)> ]
 */
main (argc, argv)
int argc;
char *argv[];				/* Possible list of devices */
{
	struct confd *devicefile;
	int exitcode = 0;			/* Function exit code */

	if (chdir (Nqs_root) == -1) {
		fprintf (stderr, "%s(FATAL): Unable to chdir() to the NQS ",
			 Qdev_prefix);
		fprintf (stderr, "root directory.\n");
		exit (1);
	}
	if ((devicefile = opendb (Nqs_devices, O_RDONLY)) == NULL) {
		fprintf (stderr, "%s(FATAL): Unable to open the NQS device ",
			 Qdev_prefix);
		fprintf (stderr, "definition database file.\n");
		exit (1);
	}
	/*
	 *  Block buffer stdout for efficiency.
	 */
	bufstdout();
	if (*++argv == NULL)  exitcode |= shoalldev (devicefile);
	else {
		while (*argv != NULL) {		/* Show info on a device */
			exitcode |= shodbyname (devicefile, *argv);
			argv++;
		}
	}
	/*
	 *  Flush output buffers and exit.
	 */
	fflush (stdout);
	fflush (stderr);
	exit (exitcode);
}
