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
static char *sccsid = "@(#)refer8.c	4.1 (Berkeley) 5/6/83";
#endif

#include "refer..c"

static char ahead[1024];
static int peeked = 0;
static char *noteof = (char *) 1;

char *
input(s)
char *s;
{
	if (peeked) {
		peeked = 0;
		if (noteof == 0)
			return(0);
		strcpy(s, ahead);
		return(s);
	}
	return(fgets(s, 1000, in));
}

char *
lookat()
{
	if (peeked)
		return(ahead);
	noteof = input(ahead);
	peeked = 1;
	return(noteof);
}

addch(s, c)
char *s;
{
	while (*s)
		s++;
	*s++ = c;
	*s = 0;
}
