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
static char rcsid[] = "@(#)$RCSfile: qsort.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:37 $";
#endif
/*
 * FUNCTIONS: qsort
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * qsort.c	1.12  com/lib/c/gen,3.1,8943 10/16/89 09:20:05
 */

#include <stdlib.h>			/* for size_t */

/*
 * FUNCTION:	Qsort sorts a table using the "quicker-sort" algorithm.
 *                                                                    
 * NOTES:	'Base' is the base of the table, 'nmemb' is the number
 *		of elements in it, 'size' is the size of an element and
 *		'compar' is the comparision function to call to compare
 *		2 elements of the table.  'Compar' is called with 2
 *		arguments, each a pointer to an element to be compared.
 *		It must return:
 *			< 0	if the first argument is less than the second
 *			> 0	if the first argument is greater than the second
 *			= 0	if the first argument is equal to the second
 *
 *
 */  

struct qdata {
	size_t	qses;				     /* element size */
	int	(*qscmp)(const void *, const void *); /* comparison function */
};

static void qstexc( char *i, char *j, char *k, struct qdata *qd);
static void qsexc( char *i, char *j, struct qdata *qd);
static void qs1( char *a, char *l, struct qdata *qd);


void 
qsort(void *base, size_t nmemb, size_t size,
	 int(*compar)(const void *, const void *))
{
	struct qdata	qsdata;

	qsdata.qscmp = compar;	/* save off the comparison function	*/
	qsdata.qses = size;	/* save off the element size		*/

	qs1((char *)base, (char *)base + nmemb * size, &qsdata);
}

static void
qs1( char *a, char *l, struct qdata *qsd)
{
	char *i, *j;
	size_t es;
	char	*lp, *hp;
	int	c;
	unsigned n;

	es = qsd->qses;
start:
	if((n=l-a) <= es)
		return;
	n = es * (n / (2*es));
	hp = lp = a+n;
	i = a;
	j = l-es;
	while(1) {
		if(i < lp) {
			if((c = (*(qsd->qscmp))((void*)i, (void*)lp)) == 0) {
				qsexc(i, lp -= es, qsd);
				continue;
			}
			if(c < 0) {
				i += es;
				continue;
			}
		}

loop:
		if(j > hp) {
			if((c = (*(qsd->qscmp))((void*)hp, (void*)j)) == 0) {
				qsexc(hp += es, j, qsd);
				goto loop;
			}
			if(c > 0) {
				if(i == lp) {
					qstexc(i, hp += es, j, qsd);
					i = lp += es;
					goto loop;
				}
				qsexc(i, j, qsd);
				j -= es;
				i += es;
				continue;
			}
			j -= es;
			goto loop;
		}

		if(i == lp) {
			if(lp-a >= l-hp) {
				qs1(hp+es, l, qsd);
				l = lp;
			} else {
				qs1(a, lp, qsd);
				a = hp+es;
			}
			goto start;
		}

		qstexc(j, lp -= es, i, qsd);
		j = hp -= es;
	}
}

static void
qsexc( char *i, char *j, struct qdata *qsd)
{
	char *ri, *rj, c;
	size_t n;

	n = qsd->qses;
	ri = i;
	rj = j;
	do {
		c = *ri;
		*ri++ = *rj;
		*rj++ = c;
	} while(--n);
}

static void
qstexc( char *i, char *j, char *k, struct qdata *qsd)
{
	char *ri, *rj, *rk;
	int c;
	size_t n;

	n = qsd->qses;
	ri = i;
	rj = j;
	rk = k;
	do {
		c = *ri;
		*ri++ = *rk;
		*rk++ = *rj;
		*rj++ = c;
	} while(--n);
}
