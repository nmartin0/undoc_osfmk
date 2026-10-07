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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/getmagic/main.c,v 1.5 1994/11/21 16:41:13 mtm Exp $
 *
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "bootmagic.h"

extern 	int optind;
extern 	char *optarg;

/* option flags */
char 	*progname;
char    *cp;
int	debug;
int     whitespace;                     /* for whitespace in .._LIST values */

main(argc, argv)
int argc;
char *argv[];
{
	/* get arguments, if any */
	progname = argv[0];
	if (get_params(argc,argv) < 0) {
		usage();
		exit(1);
	}

	/* read in bootmagic */
	if (get_magic(bootmagicfile) < 0) {
		exit(1);
	}

	if (optind < argc) {
		for (; optind < argc; optind++) {
			cp = (char *) extract_bootenv(argv[optind]);
			if (cp == (char *) NULL) {
				printf("\n");
			} else {
				printf("%s\n", cp);
			}
		}
	}
	else if (whitespace) {
		print_bootenv(whitespace_buf);
	}
	else {
		print_bootenv(in_bootmagic);
	}
	exit(0);
}

usage()
{
        printf("Usage %s:\n",progname);
        printf("\t[-m masterfile]:    Specify Paragon bootmagic file\n");
#ifdef DEBUG
        printf("\t[-D]:               Debug\n");
#endif DEBUG
	printf("\t[-w]:               Expand lists and add whitespace\n");
}

/*
 * Process command line options
 */
get_params(argc, argv)
	int argc;
	char **argv;
{
	char c;
        optind = 1;
	whitespace = 0;

#ifdef DEBUG
if(debug) printf("get_params\n");
#define ARGSTRING "Dwm:"
#else DEBUG
#define ARGSTRING "wm:"	
#endif DEBUG

	while ((c = getopt(argc, argv, ARGSTRING)) != EOF) {

                switch (c) {

#ifdef DEBUG
		case 'D':	/* debug */
			debug++;
			break;
#endif DEBUG
		case 'w':	/* whitespace */
			whitespace++;
			break;
                case 'm':       /* override system bootmagic with file */
			bootmagicfile = optarg;
                        break;

		case '?':
			return(-1);
		}

	}
	return(0);
}
