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
 **********************************************************************
 * HISTORY
 * 09-Apr-87  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Changed to save length and use bcopy instead of strcpy.
 *
 * 02-Nov-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Created from routine by same name in Steve Shafer's sup program.
 *
 **********************************************************************
 */
char *malloc();

char *salloc(p)
char *p;
{
	register char *q;
	register int l;

	q = malloc(l = strlen(p) + 1);
	if (q != 0)
		bcopy(p, q, l);
	return(q);
}
