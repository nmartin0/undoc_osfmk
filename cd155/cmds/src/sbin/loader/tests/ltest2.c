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
/* ltest2.c
 * Test file for tload2
 * Must be linked -A against tload2
 */

int a = 5;

int MATRIX[8000];

foo_entry()
{
	MATRIX[2000] = 29;
	printf("foo_entry: MATRIX[2000] = %d\n", MATRIX[2000]);

	printf("running in foo_entry\n");
	return(a * a);
}
