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
/*  sindex  --  find index of one string within another
 *
 *  Usage:  p = sindex (big,small)
 *	char *p,*big,*small;
 *
 *  Sindex searches for a substring of big which matches small,
 *  and returns a pointer to this substring.  If no matching
 *  substring is found, 0 is returned.
 *
 *  Originally from rjg (Ralph Guggenheim) on IUS/SUS UNIX.
 */


char *sindex (big,small) char *big,*small;
    {
    register char *bp, *bp1, *sp;
    register char c = *small++;

    if (c==0) return(0);
    for (bp=big;  *bp;  bp++)
	if (*bp == c)
	    {
	    for (sp=small,bp1=bp+1;   *sp && *sp == *bp1++;  sp++)
		;
	    if (*sp==0) return(bp);
	    }
    return 0;
    }
