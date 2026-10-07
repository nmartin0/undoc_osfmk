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

#include <stdio.h>

#define OPTARGSIZE 100

int ltpid;
char *ltprogramname;
int ltvflag, ltqflag;

ltgetopt(argc, argv, opts)
	int argc;
	char **argv;
	char *opts;
{
	char optargs[OPTARGSIZE];
	extern int optind, opterr;	/* should be in the test program */
	extern char *optarg;
	extern char testpurpose[];
	char c;

	setbuf(stdout,NULL);		/* force buffering to be turned OFF */

	strcpy(optargs, opts);
	strcat(optargs,"vq");
	if (ltpid == 0) {
		ltpid = getpid();
		ltprogramname = argv[0];
		opterr = 0;
	}
	while (1) {
		c = getopt(argc, argv, optargs);
		if (c == EOF) {
			ltprintf("%s\n",testpurpose);
			return(EOF);
		}
		if (c == 'v' && ltqflag || c == 'q' && ltvflag) {
			lteprintf("-q and -v conflict\n");
			ltprintusage();
		}

		if (c == 'v')
			ltvflag++;
		else if (c == 'q')
			ltqflag++;
		else if (c == '?')
			ltprintusage();
		else return (c);
	}
}
