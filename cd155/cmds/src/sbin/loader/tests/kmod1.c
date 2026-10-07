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

#define	RV	42

int rv = RV;

int
kmod1()
{
	printf("kmod1: Hi there!  I'm running in the kernel!\n");
	printf("kmod1: I'll return the answer to all questions!\n");
	printf("kmod1: kmod1() = %d\n", rv);
	return(rv);
}
