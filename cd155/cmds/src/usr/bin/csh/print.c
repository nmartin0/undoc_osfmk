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
static char rcsid[] = "@(#)$RCSfile: print.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:39 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: p60ths psecs p2dig putchar draino flush plist
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

#include <sys/ioctl.h>
#include "sh.h"

/*
 * C Shell
 */

p60ths(l)
	long l;
{

	l += 3;
	printf("%d.%d", (int) (l / 60), (int) ((l % 60) / 6));
}

psecs(l)
	long l;
{
	register int i;

	i = l / 3600;
	if (i) {
		printf("%d:", i);
		i = l % 3600;
		p2dig(i / 60);
		goto minsec;
	}
	i = l;
	printf("%d", i / 60);
minsec:
	i %= 60;
	printf(":");
	p2dig(i);
}

p2dig(i)
	register int i;
{

	printf("%d%d", i / 10, i % 10);
}

uchar_t	linbuf[128];
uchar_t	*linp = linbuf;

putchar(c)
	register int c;
{
#if	defined(NLS) || defined(KJI)
	static int quoted = 0;

	/*  if c is NLQUOTE, don't output it, but TRIM the next character.
	 *  if previous character was not NOQUOTE, don't TRIM this character.
	 */

	if(quoted) {
	    quoted = 0;
	    c &= TRIM;
	}
	else {
	     if (c==NLQUOTE) {
		quoted++;
		return;
	     }
#endif	/* NLS || KJI */
	    /* The following ^ processing is not done for quoted character */
#ifdef	NLS
        if ((c & QUOTE) == 0 &&
            (c == 0177 || c < 034 && c != '\t' && c != '\n' && c != '\b'))
	{
#else
        if ((c & QUOTE) == 0 &&
            (c == 0177 || c < ' ' && c != '\t' && c != '\n' && c != '\b'))
	{
#endif
		putchar('^');
		if (c == 0177)
			c = '?';
		else
			c |= 'A' - 1;
	}
#if	defined(NLS) || defined(KJI)
	}
#else
	c &= TRIM;
#endif
	*linp++ = c;
	if (c == '\n' || linp >= &linbuf[sizeof linbuf - 2])
		flush();
	fflush(stdout);
}

draino()
{

	linp = linbuf;
}

flush()
{
	register int unit;
	int lmode = 0;

	if (linp == linbuf)
		return;
	if (haderr)
		unit = didfds ? 2 : SHDIAG;
	else
		unit = didfds ? 1 : SHOUT;
        if (didfds==0 && ioctl(unit, TIOCLGET, &lmode)==0 && lmode & LFLUSHO) 
	{
                lmode = LFLUSHO;
                ioctl(unit, TIOCLBIC, &lmode);
                write(unit, "\n", 1);
        }

	write(unit, linbuf, linp - linbuf);
	linp = linbuf;
}

plist(vp)
	register struct varent *vp;
{

	if (setintr)
		sigrelse(SIGINT);
	for (vp = vp->link; vp != 0; vp = vp->link) {
		int len = blklen(vp->vec);

		printf((char *)vp->name);
		printf("\t");
		if (len != 1)
			putchar('(');
		blkpr(vp->vec);
		if (len != 1)
			putchar(')');
		printf("\n");
	}
	if (setintr)
		sigrelse(SIGINT);
}
