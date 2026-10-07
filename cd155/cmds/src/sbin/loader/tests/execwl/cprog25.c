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
#include <errno.h>

char testpurpose[] = "test #! argument processing, looking for ENOEXEC";
char usagestring[] = "[-v] [-q] <script-file-name>";

extern char **environ;
extern int optind;

#define	LOADER		"failure_loader"

extern int errno;

main(argc, argv)
	char *argv[];
{
	char *script;

	while (ltgetopt(argc, argv, "") != EOF)
		;

	if (optind >= argc)
		ltprintusage();
	script = argv[optind];

	ltprintf("calling exec_with_loader(0, \"%s\", \"%s\", (char **)0, (char **)0)\n",
		LOADER, script);
	if (exec_with_loader(0, LOADER, script, (char **)0,
	    (char **)0) != -1) {
		lteprintf("exec_with_loader() succeeded and should have failed with ENOEXEC\n");
		ltexit(1);
	}
	if (errno != ENOEXEC) {
		lteprintf("exec_with_loader() failed with %d, instead of %d (ENOEXEC)\n",
			errno, ENOEXEC);
		ltexit(2);
	}
	ltexit(0);
}
