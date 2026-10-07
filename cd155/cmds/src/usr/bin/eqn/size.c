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
static char sccsid[] = "@(#)size.c	4.2 8/11/83";
#endif

# include "e.h"

setsize(p)	/* set size as found in p */
char *p;
{
	if (*p == '+')
		ps += atoi(p+1);
	else if (*p == '-')
		ps -= atoi(p+1);
	else
		ps = atoi(p);
	if(dbg)printf(".\tsetsize %s; ps = %d\n", p, ps);
}

size(p1, p2) int p1, p2; {
		/* old size in p1, new in ps */
	int effps, effp1;

	yyval = p2;
	if(dbg)printf(".\tb:sb: S%d <- \\s%d S%d \\s%d; b=%d, h=%d\n", 
		yyval, ps, p2, p1, ebase[yyval], eht[yyval]);
	effps = EFFPS(ps);
	effp1 = EFFPS(p1);
	printf(".ds %d \\s%d\\*(%d\\s%d\n", 
		yyval, effps, p2, effp1);
	ps = p1;
}

globsize() {
	char temp[20];

	getstr(temp, 20);
	if (temp[0] == '+')
		gsize += atoi(temp+1);
	else if (temp[0] == '-')
		gsize -= atoi(temp+1);
	else
		gsize = atoi(temp);
	yyval = eqnreg = 0;
	setps(gsize);
	ps = gsize;
	if (gsize >= 12)	/* sub and sup size change */
		deltaps = gsize / 4;
	else
		deltaps = gsize / 3;
}
