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

char testpurpose[] = "test no execute access to file (EACCES)";
char usagestring[] = "[-v] [-q]";

extern char **environ;
extern int optind;

/* let's hope /etc/passwd doesn't have execute access!!! */

#define	LOADER	"failure_loader"
#define	FILE	"/etc/passwd"

extern int errno;

main(argc, argv)
	char *argv[];
{
	while (ltgetopt(argc, argv, "") != EOF)
		;

	ltprintf("calling exec_with_loader(0, \"%s\", \"%s\", (char **)0, (char **)0)\n",
		LOADER, FILE);
	if (exec_with_loader(0, LOADER, FILE, (char **)0,
	    (char **)0) != -1) {
		lteprintf("exec_with_loader() succeeded and should have failed with EACCES\n");
		ltexit(1);
	}
	if (errno != EACCES) {
		lteprintf("exec_with_loader() failed with %d, instead of %d (EACCES)\n",
			errno, EACCES);
		ltexit(2);
	}
	ltexit(0);
}
