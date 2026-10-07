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
static char rcsid[] = "@(#)$RCSfile: hash.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:07 $";
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
/*   hash.c	1.2  com/bsd.d/spell.d,3.1,8950 11/18/89 17:49:43";  */

#include "hash.h"

#define LOCHWIDTH 3
#define HICHWIDTH 3
#define CHARWIDTH (LOCHWIDTH+HICHWIDTH)
#define LOCHMASK ((1<<LOCHWIDTH)-1)

/*
 * if HASHWIDTH + CHARWIDTH < bitsizeof(long) one could make LOCHWIDTH=6 and
 * HICHWIDTH=0 and simplify accordingly; the hanky-panky is to avoid overflow
 * in long multiplication 
 */
#define NC 30

long            hashsize = HASHSIZE;
long            pow2[NC * 2];

static char     hashtab[] = {
			     -1, -1, -1, -1, -1, -1, 0, 31,	/* &' */
			     -1, -1, -1, -1, -1, -1, -1, -1,
			     2, 25, 20, 35, 54, 61, 40, 39,	/* 0-7 */
			     42, 33, -1, -1, -1, -1, -1, -1,
			     -1, 60, 43, 30, 5, 16, 47, 18,	/* A-G */
			     41, 36, 51, 6, 13, 56, 55, 58,
			     49, 12, 59, 46, 21, 32, 63, 34,
			     57, 52, 3, -1, -1, -1, -1, -1,
			     -1, 22, 29, 8, 7, 10, 1, 28,	/* a-g */
			     11, 62, 37, 48, 15, 50, 9, 4,
			     19, 38, 45, 24, 23, 26, 17, 44,
			     27, 14, 53, -1, -1, -1, -1, -1
};


long 
hash (s)
    char           *s;
{
    register        c;
    register long  *lp;
    long            h = 0;
    for (lp = pow2; c = *s++;)
    {
	c = hashtab[c - ' '];
	h += (c & LOCHMASK) * *lp++;
	h += (c >> LOCHWIDTH) * *lp++;
	h %= hashsize;
    }
    return (h);
}

hashinit ()
{
    register        i;
    if (1L << (HASHWIDTH + LOCHWIDTH) == 0
	|| 1L << (HASHWIDTH + HICHWIDTH) == 0)
	abort ();		/* overflow is imminent */
    pow2[0] = 1L << (HASHWIDTH - CHARWIDTH - 2);
    for (i = 0; i < 2 * NC - 3; i += 2)
    {
	pow2[i + 1] = (pow2[i] << LOCHWIDTH) % hashsize;
	pow2[i + 2] = (pow2[i + 1] << HICHWIDTH) % hashsize;
    }
}
