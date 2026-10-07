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
 
/*++ qlimit.c - Network Deviceing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qlimit/qlimit.c,v $
 *
 * DESCRIPTION:
 *
 *	Display  
 *	1) a list of the meaningful limits and
 *	2) the current shell strategy
 *	for the named machines.
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
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	January 13, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 01:35:28 $ $State: Exp $)
 * $Log: qlimit.c,v $
 * Revision 1.3  1994/11/19  01:35:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:29:49  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:27:59  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:59:04  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:14  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)qlimit.c	1.2 (qlimit.c OSF/1 NQS2.0 GJK) 6/30/92";
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
extern int shoalllim();			/* Show all limits on this machine */
extern int sholbymach();		/* Show limits given a machine name */

/*
 *	Global variables:
 */
char *Qlimit_prefix = "Qlimit";

/*** main
 *
 *
 *	qlimit [ <machine-name(s)> ]
 */
main (argc, argv)
int argc;
char *argv[];				/* Possible list of machines */
{
	struct confd *paramfile;
	int exitcode = 0;			/* Function exit code */

	if (chdir (Nqs_root) == -1) {
		fprintf (stderr, "%s(FATAL): Unable to chdir() to the NQS ",
			 Qlimit_prefix);
		fprintf (stderr, "root directory.\n");
		exit (1);
	}
	if ((paramfile = opendb (Nqs_params, O_RDONLY)) == NULL) {
		fprintf (stderr, "%s(FATAL): Unable to open the NQS ",
			 Qlimit_prefix);
		fprintf (stderr, "parameters database file.\n");
		exit (1);
	}
	/*
	 *  Block buffer stdout for efficiency.
	 */
	bufstdout();
	if (*++argv == NULL)  exitcode |= shoalllim (paramfile, SHO_SHS);
	else {
		while (*argv != NULL) {		/* Show info on a machine */
			exitcode |= sholbymach (paramfile, *argv, SHO_SHS);
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
