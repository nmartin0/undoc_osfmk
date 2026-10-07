/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#include <stdio.h>
#include <errno.h>
#include <string.h>

char *myname;

extern int errno;


usage_exit(		/* the highly-structured cure for gotos */
	int exitstat)
{
	fprintf(stderr,
	 "usage: %s [ -signal_name | -signal_number ] arg process_ID ...\n",
	 myname);
	exit(exitstat);
}

main(argc, argv)
int argc;
char *argv[];
{
	int errflag = 0;
	int signo = 32;			/* the only one we send */
	char *sigstring;
	long nodearg;
	int pid;
	int idx;			/* indexes args on cmd line */
	char *p;

	myname = strchr(argv[0], '/');
	if( myname )		/* point 1 past the slash we found */
		{ ++myname; }
	else
		{ myname = argv[0]; }

	if( argc < 3 )
		{ usage_exit(1); }

	idx = 1;			/* we start with argv[1] */

	sigstring = argv[idx];
	if( *sigstring == '-' ) {	/* 1st char in string */
		if( argc < 4 )
			{ usage_exit(1); }
		if( strcmp(argv[1],"-32") != 0 &&
				strcmp(argv[1],"-MIGRATE") != 0 &&
				strcmp(argv[1],"-SIGMIGRATE") != 0 ) {
			fprintf(stderr, "%s: only signal 32 (MIGRATE or SIGMIGRATE) supported.\n", argv[0]);
			usage_exit(1);
		}
		++sigstring;		/* advance past initial '-' */
		++idx;			/* advance past this arg */
	}

	/* now deal with the nodearg */
	nodearg = strtol(argv[idx], &p, 0);
	if( p == argv[idx]  ||  *p != '\0' ) {
		fprintf(stderr, "%s: %s is not a numeric arg.\n",
				myname, argv[idx]);
		exit(1);
	}
	++idx;

	for( ; idx<argc; ++idx ) {
		pid = strtol(argv[idx], &p, 0);
		if( p == argv[idx]  ||  *p != '\0' ) {
			fprintf(stderr,
			 "%s: non-numeric process_ID %s skipped.\n",
			 myname, argv[idx]);
			continue;
		}
		if( kill3(pid, signo, nodearg) <= 0 ) {
			if( errno == EPERM ) {
				fprintf(stderr, "%s: Not owner\n", argv[idx]);
			} else if( errno == ESRCH ) {
				fprintf(stderr, "%s: No such process\n",
						argv[idx]);
			} else if( errno == EINVAL ) {
				fprintf(stderr, "%s: %s is not valid signal\n",
						myname, sigstring);
				exit(1);	/* exit right away for this */
			}
			++errflag;
		}
	}
	if( errflag )
		{ exit(1); }
	exit(0);
}

