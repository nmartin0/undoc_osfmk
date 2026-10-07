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
static char rcsid[] = "@(#)$RCSfile: hashcheck.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:09 $";
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
/* hashcheck.c	1.2  com/bsd.d/spell.d,3.1,8950 11/18/89 17:14:33"; */
#include <stdio.h>
#include <locale.h>
#include "hash.h"
long            fetch ();
int             index[NI];
unsigned        tablearea[ND];
unsigned       *table = tablearea;
unsigned        wp;
int             bp;
#define U (BYTE*sizeof(unsigned))
#define L (BYTE*sizeof(long))

main ()
{
    int             i;
    long            v;
    long            a;
    setlocale (LC_ALL, "");
    rhuff (stdin);
    fread ((char *) index, sizeof (*index), NI, stdin);
    fread ((char *) table, sizeof (*table), index[NI - 1], stdin);
    for (i = 0; i < NI - 1; i++)
    {
	bp = U;
	v = (long) i << (HASHWIDTH - INDEXWIDTH);
	for (wp = index[i]; wp < index[i + 1];)
	{
	    if (wp == index[i] && bp == U)
		a = fetch ();
	    else
	    {
		a = fetch ();
		if (a == 0)
		    break;
	    }
	    if (wp > index[i + 1] ||
		wp == index[i + 1] && bp < U)
		break;
	    v += a;
	    printf ("%.9lo\n", v);
	}
    }
}

long 
fetch ()
{
    long            w;
    long            y = 0;
    int             empty = L;
    int             i = bp;
    int             tp = wp;
    while (empty >= i)
    {
	empty -= i;
	i = U;
	y |= (long) table[tp++] << empty;
    }
    if (empty > 0)
	y |= table[tp] >> i - empty;
    i = decode ((y >> 1) & (0x7fffffff), &w);
    bp -= i;
    while (bp <= 0)
    {
	bp += U;
	wp++;
    }
    return (w);
}
