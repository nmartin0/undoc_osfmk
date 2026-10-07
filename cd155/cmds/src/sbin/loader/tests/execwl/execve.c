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

/*
 * execve - progam to call execve()
 */

#include <stdio.h>

char usagestring[] =
"usage: execve <file> argv[0] argv[1] ... argv[n]\n\
\n\
       file   = file name of the file execute \n\
       argv   = arguments, including argv[0], to be passed to the command\n\
\n\
                environment passed as is to the loader and command; use\n\
                env(1) to override environment\n";

extern char *environ;

int
main(argc, argv)
	char *argv[];
{
	char *file, **nargv;

	if (argc < 2) {
		(void)fprintf(stderr, "%s", usagestring);
		exit(1);
	}

	file = argv[1];
	nargv = &argv[2];

	if (execve(file, nargv, environ) == -1) {
		perror("execve: execve() failed");
		exit(3);
	}
	exit(0);
	return(0);	
}
