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

char testpurpose[] = "test bad pointer to file name (EFAULT)";
char usagestring[] = "[-v] [-q]";

extern char **environ;
extern int optind;

#define	LOADER		"failure_loader"
#define	BAD_POINTER	-1

extern int errno;

main(argc, argv)
	char *argv[];
{
	int flags = 0;

	while (ltgetopt(argc, argv, "") != EOF)
		;

	ltprintf("calling exec_with_loader(0, \"%s\", (char *)%d, 0, 0)\n",
		LOADER, BAD_POINTER);
	if (exec_with_loader(0, LOADER, (char *)BAD_POINTER, (char **)0,
	    (char **)0) != -1) {
		lteprintf("exec_with_loader() succeeded and should have failed with EFAULT\n");
		ltexit(1);
	}
	if (errno != EFAULT) {
		lteprintf("exec_with_loader() failed with %d, instead of %d (EFAULT)\n",
			errno, EFAULT);
		ltexit(2);
	}
	ltexit(0);
}
