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
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: getopt.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:25:43 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * static char sccsid[] = "com/cmd/sh,3.1,9013 9/13/89 21:01:12";
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<locale.h>

#include <NLchar.h>
#include "getopt_msg.h"

extern int getopt(), optind;
extern char *optarg;

#define	STR_LENGTH	5120		/* output string length	*/

/*
 * NAME:	getopt
 *
 * SYNTAX:	getopt legal-args $*
 *
 * FUNCTION:	getopt - parse command line flags and parameters
 *
 * NOTES:	Getopt can be used to break up flags and parameters in
 *		command lines for easy parsing by shell procedures and
 *		to check for valid flags.  "Legal-args" is a string of
 *		recognized flags (see getopt(3)).
 *
 * RETURN VALUE DESCRIPTION:	0 if no errors were found, 2 if command line
 *		errors were found, 1 if non-ascii characters were found (NLS)
 */

int
main(argc, argv)
int argc;
char **argv;
{
	register int	c;
	int	errflg = 0;
	char	tmpstr[4];	/* big enuf for "-c \0"	*/
	char	*goarg;
	char    outstr[STR_LENGTH];
	char    *s;

	(void) setlocale (LC_ALL, "");

	/*
	 * check argument count
	 */
	if (argc < 2) {
		(void) fputs(NLgetamsg(MF_GETOPT, MS_GETOPT, USAGE,
			"usage: getopt legal-args\n"), stderr);
		return(2);
	}

        /* Check for any NLS characters in the option string. */
        for (s = argv[1]; *s; s++) {
	    if (NLisNLcp(s)) {
	        (void) fputs(NLgetamsg(MF_GETOPT, MS_GETOPT, NONLSCH,
	"getopt: Only ASCII characters are permitted in option string.\n"),
			stderr);
	        return(1);
	    }; /* end if */
        }; /* end for */

	*outstr = '\0';         /* initialize the output string */
	goarg = argv[1];
	argv[1] = argv[0];
	argv++;
	argc--;

	/*
	 * process flags
	 */
	while((c=getopt(argc, argv, goarg)) != EOF) {
		if(c=='?') {
			errflg++;
			continue;
		}

		/*
		 * build string based on flag found
		 */
		tmpstr[0] = '-';
		tmpstr[1] = c;
		tmpstr[2] = ' ';
		tmpstr[3] = '\0';

		/*
		 * concatenate to output string
		 */
		(void) strcat(outstr, tmpstr);

		if(*(strchr(goarg, c)+1) == ':') {
			/*
			 * get optarg for flag
			 */
			(void) strcat(outstr, optarg);
			(void) strcat(outstr, " ");
		}
	}

	/*
	 * exit with status 2 if we found any errors
	 */
	if(errflg) {
		return(2);
	}

	/*
	 * add "--" to end options
	 */
	(void) strcat(outstr, "-- ");

	/*
	 * add rest of arguments
	 */
	while(optind < argc) {
		(void) strcat(outstr, argv[optind++]);
		(void) strcat(outstr, " ");
	}

	/*
	 * print it
	 */
	(void) puts(outstr);

	return(0);
}
