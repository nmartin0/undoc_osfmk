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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: hashlook.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:10 $";
#endif
/*
 * COMPONENT_NAME: (CMDTEXT) Text Formatting Services
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3,10,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */
/*  hashlook.c	1.2  com/bsd.d/spell.d,3.1,8950 11/18/89 17:16:07"; */
#include <stdio.h>
#include "hash.h"
#include "huff.h"

unsigned        tablearea[ND];
unsigned       *table = tablearea;
int             index[NI];

#define B (BYTE*sizeof(unsigned))
#define L (BYTE*sizeof(long)-1)
#define MASK (~(1L<<L))

/*
 * Don't use stdio on 16 bit machines.  The fetch macro below is also set up
 * for 16/32 bits depending on whether DONT_USE_STDIO is defined. 
 */
#ifdef pdp11
#define	DONT_USE_STDIO
#endif
#ifdef M_I286
#define	DONT_USE_STDIO
#endif
#ifdef M_I86
#define	DONT_USE_STDIO
#endif
#ifdef	DONT_USE_STDIO
#define fetch(wp,bp)\
	(((((long)wp[0]<<B)|wp[1])<<(B-bp))|(wp[2]>>bp))
#else				/* sizeof(unsigned)==sizeof(long) */
#define rshift(a,bp)	(((bp) >= B) ? 0 : (a) >> (bp))
#define fetch(wp,bp) (((wp)[0] << (B-(bp))) | rshift((wp)[1],(bp)))
#define	USE_STDIO
#endif

hashlook (s)
    char           *s;
{
    long            h;
    long            t;
    register        bp;
    register unsigned *wp;
    int             i;
    long            sum;
    unsigned       *tp;

    h = hash (s);
    t = h >> (HASHWIDTH - INDEXWIDTH);
    wp = &table[index[t]];
    tp = &table[index[t + 1]];
    bp = B;
    sum = (long) t << (HASHWIDTH - INDEXWIDTH);
    for (;;)
    {
	{			/* this block is equivalent to bp -=
				 * decode((fetch(wp,bp)>>1)&MASK, &t); */
	    long            y;
	    long            v;

	    y = (fetch (wp, bp) >> 1) & MASK;
	    if (y < cs)
	    {
		t = y >> (L + 1 - w);
		bp -= w - 1;
	    } else
	    {
		for (bp -= w, v = v0; y >= qcs; y = (y << 1) & MASK, v += n)
		    bp -= 1;
		t = v + (y >> (L - w));
	    }
	}
	while (bp <= 0)
	{
	    bp += B;
	    wp++;
	}
	if (wp >= tp && (wp > tp || bp < B))
	    return (0);
	sum += t;
	if (sum < h)
	    continue;
	return (sum == h);
    }
}


prime (argc, argv)
    char          **argv;
{
    register FILE  *f;
    register        fd;


    if (argc <= 1)
	return (0);
#ifndef USE_STDIO		/* because of insufficient address space for
				 * buffers */
    fd = dup (0);
    close (0);
    if (open (argv[1], 0) != 0)
	return (0);
    f = stdin;
    if (rhuff (f) == 0
	|| read (fileno (f), (char *) index, NI * sizeof (*index)) != NI * sizeof (*index)
	|| read (fileno (f), (char *) table, sizeof (*table) * index[NI - 1])
	!= index[NI - 1] * sizeof (*table))
	return (0);
    close (0);
    if (dup (fd) != 0)
	return (0);
    close (fd);
#else
    if ((f = fopen (argv[1], "ri")) == NULL)
	return (0);
    if (rhuff (f) == 0
	|| fread ((char *) index, sizeof (*index), NI, f) != NI
	|| fread ((char *) table, sizeof (*table), index[NI - 1], f)
	!= index[NI - 1])
	{
fprintf(stderr, "NI =%d  ; index[NI-1] = %d \n", NI, index[NI - 1]);
	return (0);
	}
    fclose (f);
#endif
    hashinit ();
    return (1);
}
