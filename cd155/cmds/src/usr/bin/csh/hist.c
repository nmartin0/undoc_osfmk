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
static char rcsid[] = "@(#)$RCSfile: hist.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:34 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: savehist enthist hfree dohist dohist1 phist
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

/*
 * C shell
 */

savehist(sp)
	struct wordent *sp;
{
	register struct Hist *hp, *np;
	int histlen;
	register uchar_t *cp;

	/* throw away null lines */
	if (sp->next->word[0] == '\n')
		return;

	cp = value("history");
	if (*cp == 0)
		histlen = 0;
	else {
		while (*cp && digit(*cp))
			cp++;
		/* avoid a looping snafu */
		if (*cp)
			set("history", "10");
		histlen = getn(value("history"));
	}
	for (hp = &Histlist; np = hp->Hnext;)
		if (eventno - np->Href >= histlen || histlen == 0)
			hp->Hnext = np->Hnext, hfree(np);
		else
			hp = np;
	enthist(++eventno, sp, 1);
}

struct Hist *
enthist(event, lp, docopy)
	int event;
	register struct wordent *lp;
	bool docopy;
{
	register struct Hist *np;

	np = (struct Hist *) calloc(1, sizeof *np);
	np->Hnum = np->Href = event;
	if (docopy)
		copylex(&np->Hlex, lp);
	else {
		np->Hlex.next = lp->next;
		lp->next->prev = &np->Hlex;
		np->Hlex.prev = lp->prev;
		lp->prev->next = &np->Hlex;
	}
	np->Hnext = Histlist.Hnext;
	Histlist.Hnext = np;
	return (np);
}

hfree(hp)
	register struct Hist *hp;
{

	freelex(&hp->Hlex);
	xfree((char *)hp);
}

dohist(vp)
	uchar_t **vp;
{
	int n, rflg = 0, hflg = 0;
	if (getn(value("history")) == 0)
		return;
	if (setintr)
		sigrelse(SIGINT);
	vp++;
	while (*vp && *vp[0] == '-') {
		if (*vp && eq(*vp, "-h")) 
			hflg++;
		else if (*vp && eq(*vp, "-r")) 
			rflg++;
		else {
			flush ();
			haderr = 1;
			printf (MSGSTR(M_OPTION,"Unknown option : %s \n"), *vp);
			return;
		}
		vp++;
	}
	if (*vp)
		n = getn(*vp);
	else {
		n = getn(value("history"));
	}
	dohist1(Histlist.Hnext, &n, rflg, hflg);
}

dohist1(hp, np, rflg, hflg)
	struct Hist *hp;
	int *np, rflg, hflg;
{
	bool print = (*np) > 0;
top:
	if (hp == 0)
		return;
	(*np)--;
	hp->Href++;
	if (rflg == 0) {
		dohist1(hp->Hnext, np, rflg, hflg);
		if (print)
			phist(hp, hflg);
		return;
	}
	if (*np >= 0)
		phist(hp, hflg);
	hp = hp->Hnext;
	goto top;
}

phist(hp, hflg)
	register struct Hist *hp;
	int hflg;
{

	if (hflg == 0)
		printf("%6d\t", hp->Hnum);
	prlex(&hp->Hlex);
}

#ifdef	OLDHIST
dohist(vp)
	uchar_t **vp;
{
	int n, rflg = 0;

	if (getn(value("history")) == 0)
		return;
	if (setintr)
		sigrelse(SIGINT);
	vp++;
	if (*vp && eq(*vp, "-r")) {
		rflg++;
		vp++;
	}
	if (*vp)
		n = getn(*vp);
	else
		n = 1000;
	dohist1(Histlist.Hnext, &n, rflg);
}

dohist1(hp, np, rflg)
	struct Hist *hp;
	int *np;
{
	bool print = (*np) > 0;
top:
	if (hp == 0)
		return;
	(*np)--;
	hp->Href++;
	if (rflg == 0) {
		dohist1(hp->Hnext, np, rflg);
		if (print)
			phist(hp);
		return;
	}
	if (*np >= 0)
		phist(hp);
	hp = hp->Hnext;
	goto top;
}

phist(hp)
	register struct Hist *hp;
{

	printf("%6d\t", hp->Hnum);
	prlex(&hp->Hlex);
}
#endif	OLDHIST
