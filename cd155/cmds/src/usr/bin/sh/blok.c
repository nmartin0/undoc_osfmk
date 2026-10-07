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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: blok.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:55 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include	"defs.h"


/*
 *	storage allocator
 *	(circular first fit strategy)
 */

#define BUSY 01

#define busy(x)	(Rcheat((x)->word) & BUSY)

unsigned	brkincr = BRKINCR;

#ifndef	_SHRLIB
void setup_mem();	/* Perform pre-malloc setup. */
struct blk *blokp = (struct blk *)NULL;/*current search pointer*/
struct blk *bloktop;		/* top of arena (last blok) */

uchar_t		*brkbegin;
uchar_t		*setbrk();

uchar_t *
alloc(nbytes)
	unsigned nbytes;
{
	register unsigned rbytes = round(nbytes+BYTESPERWORD, BYTESPERWORD);

	/* Initialize memory allocation on 1st call. */
	if (blokp == (struct blk *)NULL) setup_mem();

	for (;;)
	{
		int	c = 0;
		register struct blk *p = blokp;
		register struct blk *q;

		do
		{
			if (!busy(p))
			{
				while (!busy(q = p->word))
					p->word = q->word;
				if ((char *)q - (char *)p >= rbytes)
				{
					blokp = (struct blk *)((uchar_t *)p + rbytes);
					if (q > blokp)
						blokp->word = p->word;
					p->word = (struct blk *)(Rcheat(blokp) | BUSY);
					return((uchar_t *)(p + 1));
				}
			}
			q = p;
			p = (struct blk *)(Rcheat(p->word) & ~BUSY);
		} while (p > q || (c++) == 0);
		addblok(rbytes);
	}
}

addblok(reqd)
	unsigned reqd;
{
	if (stakbot == 0)
	{
		brkbegin = setbrk(BRKINCR);
		bloktop = (struct blk *)brkbegin;
	}

	if (stakbas != staktop)
	{
		register uchar_t *rndstak;
		register struct blk *blokstak;

		pushstak(0);
		rndstak = (uchar_t *)round(staktop, BYTESPERWORD);
		blokstak = (struct blk *)(stakbas) - 1;
		blokstak->word = stakbsy;
		stakbsy = blokstak;
		bloktop->word = (struct blk *)(Rcheat(rndstak) | BUSY);
		bloktop = (struct blk *)(rndstak);
	}
	reqd += brkincr;
	reqd &= ~(brkincr - 1);
	needmem (Rcheat(bloktop)+reqd+BRKINCR+
	  (staktop ? (int)(staktop-stakbot) : 0));
	blokp = bloktop;
	bloktop = bloktop->word = (struct blk *)(Rcheat(bloktop) + reqd);
	bloktop->word = (struct blk *)(brkbegin + 1);
	{
		register uchar_t *stakadr = (uchar_t *)(bloktop + 2);

		if (stakbot != staktop)
			staktop = movstr(stakbot, stakadr);
		else
			staktop = stakadr;

		stakbas = stakbot = stakadr;
	}
}

void
free(vap)
	void *vap;
{
	struct blk *ap = (struct blk *) vap;
	register struct blk *p;

	if ((p = ap) && (p < bloktop && p >= (struct blk *)brkbegin))
	{
#ifdef DEBUG
		chkbptr(p);
#endif
		--p;
		p->word = (struct blk *)(Rcheat(p->word) & ~BUSY);
	}


}


#ifdef DEBUG

chkbptr(ptr)
	struct blk *ptr;
{
	int	exf = 0;
	register struct blk *p = (struct blk *)brkbegin;
	register struct blk *q;
	int	us = 0, un = 0;

	for (;;)
	{
		q = (struct blk *)(Rcheat(p->word) & ~BUSY);

		if (p+1 == ptr)
			exf++;

		if (q < (struct blk *)brkbegin || q > bloktop)
			abort(3);

		if (p == bloktop)
			break;

		if (busy(p))
			us += q - p;
		else
			un += q - p;

		if (p >= q)
			abort(4);

		p = q;
	}
	if (exf == 0)
		abort(1);
}


chkmem()
{
	register struct blk *p = (struct blk *)brkbegin;
	register struct blk *q;
	int	us = 0, un = 0;

	for (;;)
	{
		q = (struct blk *)(Rcheat(p->word) & ~BUSY);

		if (q < (struct blk *)brkbegin || q > bloktop)
			abort(3);

		if (p == bloktop)
			break;

		if (busy(p))
			us += q - p;
		else
			un += q - p;

		if (p >= q)
			abort(4);

		p = q;
	}

	prs("un/used/avail ");
	prn(un);
	blank();
	prn(us);
	blank();
	prn((char *)bloktop - brkbegin - (un + us));
	newline();

}
#endif /* DEBUG */

/*
 * Setup memory allocation for the shell's alloc (malloc)
 */
void
setup_mem()
{
	/* Only do this once. */
	if (blokp != (struct blk *)NULL) 
		return;

	/*
	 * initialise storage allocation 
	 */

	stakbot = 0;
	addblok((unsigned)0);

}

#else	/* IF _SHRLIB */

	extern void	*end;
	uchar_t *brkbegin = (uchar_t *) &end;
	uchar_t *bloktop  = (uchar_t *) &end;
#undef	free
/* in the shared library version, the library malloc is used to allocate.
 * However, some shell routines free pointers which where not malloc'd and
 * expect this to be ignored.  We test by checking for addresses within
 * the heap.  To reduce the cost of the test, we only use sbrk to check
 * for the true end of heap when the check fails.  Eventually, blktop
 * should converge to the high water mark of the heap.
 */
#ifdef KJI
void
#endif
alloc_free(ap)
	uchar_t	*ap;
{
	if ( ap >= (uchar_t *)brkbegin && ap <= (uchar_t *)bloktop )
		free(ap);
	else {
		bloktop = (uchar_t *)sbrk(0);
		if ( ap >=(uchar_t *) brkbegin && ap <=(uchar_t *) bloktop )
			free(ap);
	}
	
}

/* use only to init stak */
addblok(reqd)
	unsigned reqd;
{
	if (stakbot == 0)
	{
		growstak();
	}
}
#endif	/* IF _SHRLIB */

#ifndef	_SHRLIB

	/*	The following code is for an unshared version of
	 *	the sh. This is a primitive version of realloc and
	 *	is not intended to be a final solution for anything.
	 *	This code should be deleted when the shared library
	 *	solution has been obtained.
	 */

void *realloc ( void *buf_adr , size_t nbytes )
{
	void	*new_adr ;

	if ( new_adr = malloc (nbytes))
		if ( new_adr != buf_adr )
			bcopy ( buf_adr , new_adr , nbytes ) ;
	return ( new_adr );
}

#endif
