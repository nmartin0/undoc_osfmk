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

extern int ltqflag, ltvflag;

ltexit(status)
	int status;
{
	char *clock_string, *ctime();
	long clock, time();

	clock = time((long *)0);
	clock_string = ctime(&clock);

	if (status) {
		lteprintf("FAILED with status %d on %s\n", status,
			clock_string);
		exit(status);
	}

	if (ltqflag == 0)
		ltvflag == 1;	/* hack so that we can get a status msg */
	ltprintf("PASSED on %s\n", clock_string);
	exit(status);
}
