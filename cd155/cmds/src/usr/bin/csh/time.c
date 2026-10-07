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
static char rcsid[] = "@(#)$RCSfile: time.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:47 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: settimes dotime donice ptimes
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

#include "sh.h"
#define MICRO	1000000L
#define HEC	100L
#define HS 	(long)(MICRO/HEC)
/*
 * C Shell - routines handling process timing and niceing
 */
settimes()
{
	struct rusage ruch;

	(void)gettimeofday(&time0, (struct timezone *)0);
	(void) getrusage(RUSAGE_SELF, &ru0);
	(void) getrusage(RUSAGE_CHILDREN, &ruch);
	ruadd(&ru0, &ruch);
}

/*
 * dotime is only called if it is truly a builtin function and not a
 * prefix to another command
 */
dotime()
{
	struct timeval timedol;
	struct rusage ru1, ruch;

	(void) getrusage(RUSAGE_SELF, &ru1);
	(void) getrusage(RUSAGE_CHILDREN, &ruch);
	ruadd(&ru1, &ruch);
	(void)gettimeofday(&timedol,(struct timezone *)0);
	prusage(&ru0, &ru1, &timedol, &time0);
}

/*
 * donice is only called when it on the line by itself or with a +- value
 */
donice(v)
	register uchar_t **v;
{
	register uchar_t *cp;

	v++, cp = *v++;
	if (cp == 0) {
#if _BSD
		nice(4);
#else
		setnice(24);
#endif
	} else if (*v == 0 && any(cp[0], "+-")) {
#if _BSD
		nice(getn(cp));
#else
		setnice(getn(cp));
#endif
	}
}

ruadd(ru, ru2)
	register struct rusage *ru, *ru2;
{
	register long *lp, *lp2;
	register int cnt;

	tvadd(&ru->ru_utime, &ru2->ru_utime);
	tvadd(&ru->ru_stime, &ru2->ru_stime);
	if (ru2->ru_maxrss > ru->ru_maxrss)
		ru->ru_maxrss = ru2->ru_maxrss;
	cnt = &ru->ru_last - &ru->ru_first + 1;
	lp = &ru->ru_first; lp2 = &ru2->ru_first;
	do
		*lp++ += *lp2++;
	while (--cnt > 0);
}

prusage(r0, r1, e, b)
	register struct rusage *r0, *r1;
	struct timeval *e, *b;
{
	register time_t t =
	    (r1->ru_utime.tv_sec-r0->ru_utime.tv_sec)*HEC+
	    (r1->ru_utime.tv_usec-r0->ru_utime.tv_usec)/HS+
	    (r1->ru_stime.tv_sec-r0->ru_stime.tv_sec)*HEC+
	    (r1->ru_stime.tv_usec-r0->ru_stime.tv_usec)/HS;
	register uchar_t *cp;
	register int i;
	register struct varent *vp = adrof("time");
	time_t ms = 
	    (e->tv_sec-b->tv_sec)*HEC + (e->tv_usec-b->tv_usec)/HS;

	cp = (uchar_t *)"%Uu %Ss %E %P %X+%Dk %I+%Oio %Fpf+%Ww";
	if (vp && vp->vec[0] && vp->vec[1])
		cp = vp->vec[1];
	for (; *cp; cp++)
	if (*cp != '%')
		putchar(*cp);
	else if (cp[1]) switch(*++cp) {

	case 'U':
		pdeltat(&r1->ru_utime, &r0->ru_utime);
		break;

	case 'S':
		pdeltat(&r1->ru_stime, &r0->ru_stime);
		break;

	case 'E':
		psecs((long)(ms / HEC));
		break;

	case 'P':
		printf("%d%%", (int) (t*HEC / ((ms ? ms : 1))));
		break;

	case 'W':
		i = r1->ru_nswap - r0->ru_nswap;
		printf("%d", i);
		break;

	case 'X':
		printf("%d", t == 0 ? 0 : (r1->ru_ixrss-r0->ru_ixrss)/t);
		break;

	case 'D':
		printf("%d", t == 0 ? 0 :
		    (r1->ru_idrss+r1->ru_isrss-(r0->ru_idrss+r0->ru_isrss))/t);
		break;

	case 'K':
		printf("%d", t == 0 ? 0 :
		    ((r1->ru_ixrss+r1->ru_isrss+r1->ru_idrss) -
		    (r0->ru_ixrss+r0->ru_idrss+r0->ru_isrss))/t);
		break;

	case 'M':
		printf("%d", r1->ru_maxrss/2);
		break;

	case 'F':
		printf("%d", r1->ru_majflt-r0->ru_majflt);
		break;

	case 'R':
		printf("%d", r1->ru_minflt-r0->ru_minflt);
		break;

	case 'I':
		printf("%d", r1->ru_inblock-r0->ru_inblock);
		break;

	case 'O':
		printf("%d", r1->ru_oublock-r0->ru_oublock);
		break;
	}
	putchar('\n');
}

pdeltat(t1, t0)
	struct timeval *t1, *t0;
{
	struct timeval td;

	tvsub(&td, t1, t0);
	printf("%d.%01d", td.tv_sec, td.tv_usec/HS);
}

tvadd(tsum, t0)
	struct timeval *tsum, *t0;
{

	tsum->tv_sec += t0->tv_sec;
	tsum->tv_usec += t0->tv_usec;
	if (tsum->tv_usec > MICRO)
		tsum->tv_sec++, tsum->tv_usec -= MICRO;
}

tvsub(tdiff, t1, t0)
	struct timeval *tdiff, *t1, *t0;
{

	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	if (tdiff->tv_usec < 0)
		tdiff->tv_sec--, tdiff->tv_usec += MICRO;
}
