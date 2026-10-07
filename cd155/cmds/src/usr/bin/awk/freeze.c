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
#ifndef lint
static char sccsid[] = "@(#)freeze.c	4.2 8/11/83";
#endif

#include "stdio.h"
freeze(s) char *s;
{	int fd;
	unsigned int *len;
	len = (unsigned int *)sbrk(0);
	if((fd = creat(s, 0666)) < 0) {
		perror(s);
		return(1);
	}
	write(fd, &len, sizeof(len));
	write(fd, (char *)0, len);
	close(fd);
	return(0);
}

thaw(s) char *s;
{	int fd;
	unsigned int *len;
	if(*s == 0) {
		fprintf(stderr, "empty restore file\n");
		return(1);
	}
	if((fd = open(s, 0)) < 0) {
		perror(s);
		return(1);
	}
	read(fd, &len, sizeof(len));
	(void) brk(len);
	read(fd, (char *)0, len);
	close(fd);
	return(0);
}
