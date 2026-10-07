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
static char rcsid[] = "@(#)$RCSfile: spellinprg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:20 $";
#endif
/*
 * COMPONENT_NAME: (CMDTEXT) Text Formatting Services
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3,10,13,27
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
/*  spellinprg.c	1.2  com/bsd.d/spell.d,3.1,8950 11/18/89 17:39:20"; */
#include <stdio.h>
#include <locale.h>
#include "hash.h"

#define S (BYTE*sizeof(long))
#define B (BYTE*sizeof(unsigned))
unsigned        tablearea[ND];
unsigned       *table = tablearea;
int             index[NI];
unsigned        wp;		/* word pointer */
int             bp = B;		/* bit pointer */
int             ignore;
int             extra;

#include "spell_msg.h"
#define MSGSTR(num, str)    catgets(catd, MS_SPELL, num, str)

/*
 * usage: hashin N where N is number of words in dictionary and standard
 * input contains sorted, unique hashed words in octal 
 */
main (argc, argv)
    char          **argv;
{
    long            h,
                    k,
                    d;
    register        i;
    long            count;
    long            w;
    long            x;
    int             t,
                    u;
    extern double   huff ();
    double          atof ();
    double          z;
    nl_catd         catd;

    setlocale (LC_ALL, "");
    catd = catopen (MF_SPELL, 0);
    k = 0;
    u = 0;
    if (argc != 2)
    {
	fprintf (stderr, MSGSTR (SPIN_ARG, "spellin: arg count\n"));
	exit (1);
    }
      z = huff ( (double)(1L << HASHWIDTH) / (double) atof (argv[1]));
    fprintf (stderr,
	     MSGSTR (SPIN_WID, "spellin: expected code widths = %f\n"), z);
    for (count = 0; scanf ("%lo", &h) == 1; ++count)
    {
	if ((t = h >> (HASHWIDTH - INDEXWIDTH)) != u)
	{
	    if (bp != B)
		wp++;
	    bp = B;
	    while (u < t)
		index[++u] = wp;
	    k = (long) t << (HASHWIDTH - INDEXWIDTH);
	}
	d = h - k;
	k = h;
	for (;;)
	{
	    for (x = d;; x /= 2)
	    {
		i = encode (x, &w);
		if (i > 0)
		    break;
	    }
	    if (i > B)
	    {
		if (!(
		      append ((unsigned) (w >> (i - B)), B) &&
		      append ((unsigned) (w << (B + B - i)), i - B)))
		    ignore++;
	    } else
	    if (!append ((unsigned) (w << (B - i)), i))
		ignore++;
	    d -= x;
	    if (d > 0)
		extra++;
	    else
		break;
	}
    }
    if (bp != B)
	wp++;
    while (++u < NI)
	index[u] = wp;
    whuff ();
    fwrite ((char *) index, sizeof (*index), NI, stdout);
    fwrite ((char *) table, sizeof (*table), wp, stdout);
    fprintf (stderr, MSGSTR (SPIN_ITMS,
	   "spellin: %ld items, %d ignored, %d extra, %u words occupied\n"),
	     count, ignore, extra, wp);
    count -= ignore;
    fprintf (stderr, MSGSTR (SPIN_BITS,
		      "spellin: %f table bits/item, %f table+index bits\n"),
	     ((float) BYTE * wp) * sizeof (*table) / count,
	     BYTE * ((float) wp * sizeof (*table) + sizeof (index)) / count);
    return (0);
}

append (w, i)
    register unsigned w;
    register        i;
{
    while (wp < ND - 1)
    {
	table[wp] |= w >> (B - bp);
	i -= bp;
	if (i < 0)
	{
	    bp = -i;
	    return (1);
	}
	w <<= bp;
	bp = B;
	wp++;
    }
    return (0);
}
