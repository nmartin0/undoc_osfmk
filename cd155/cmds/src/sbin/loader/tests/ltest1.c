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
/* ltest1.c
 * Test file for tload2
 * Must be linked -A against tload2
 */

#include <stdio.h>

foo_entry()
{
	char	buf[512];
	int	val = 256;
	int	rc;

	printf("running in foo_entry\n");
	if (gets(buf) == NULL) {
		perror("gets error");
		return(0);
	}

	printf("current value is %d\n", val);

	if ((rc = sscanf(buf, "%d", &val)) != 1)
		printf("scanf didn't like %s, returned %d\n", buf, rc);

	printf("current value is %d\n", val);
	return(val);
}
