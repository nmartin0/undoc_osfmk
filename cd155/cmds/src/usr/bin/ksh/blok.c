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
static char rcsid[] = "@(#)$RCSfile: blok.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:08 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
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
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Rewritten by David Korn
 *	AT&T Bell Laboratories
 *
 */

#include	"defs.h"

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/*
 *	storage allocator
 *	(circular first fit strategy)
 */

#define SLOP	8
#ifdef cray
    typedef struct
    {
	char *cword;
    } cptr;
#   define BUSY 0x2000000000000000
#   define busy(x)	(Rcheat(((cptr*)(x))->cword)&BUSY)
#   define setbusy(x,y)	(((cptr*)(x))->cword = ADR(Rcheat(y)|BUSY))
#else
#  ifdef        univac
#   define BUSY (01<<34)
#  else
#   define BUSY 01
#  endif /* univac */
#   define busy(x)	(Rcheat((x)->word)&BUSY)
#   define setbusy(x,y)	((x)->word = BLK(Rcheat(y)|BUSY))
#endif /* cray */

#define Rcheat(x)	((sizeof(char*)==sizeof(int))? \
				((int)(x)):\
				((long)(x)))
#define BLK(x)	((struct blk*)(x))

#ifdef DBUG
    void	chkmem();
#endif	/* DBUG */

static char		*brkbegin;	/* first sbrk() value */
static struct blk	*blokp=0;
static struct blk	*bloktop;	/*top of arena (last blok) */

/*
 * equivalent to malloc(3) except that a data area stack is
 * maintained on top of the heap
 */

void *
malloc(size_t nbytes)
{
	register int  rbytes = round(nbytes+BYTESPERWORD,BYTESPERWORD);
		/*	PTM 24032
		*	setlocale is calling malloc and ksh's malloc
		*	depends upon sh_addblok, therefore check to
		*	see is sh_addblok has been called by checking blokp;
		*	this replaces the call in main to sh_addblok
		*/
	if ( blokp == 0 )
		sh_addblok (0);
	while(1)
	{
		register struct blk *p = blokp;
		register struct blk *q;
		register int c=0;
		do
		{
			if(!busy(p))
			{
				while((q=p->word),!busy(q))
					p->word = q->word;
				if(ADR(q)-ADR(p) >= rbytes)
				{
					blokp = BLK(ADR(p)+rbytes);
					if(q > blokp)
						blokp->word = p->word;
					setbusy(p,blokp);
					return(ADR(p+1));
				}
			}
			q = p; p = BLK(Rcheat(p->word)&~BUSY);
		}
		while(p>q || (c++)==0);
		sh_addblok(rbytes);
	}
}

/*
 * add more space to the heap and move the stack to the top of the heap
 */

void	sh_addblok(int reqd)
/*@
	assume reqd!=0;
@*/
{
	register STKPTR	stakadr;
	if(sh.stakbot == 0)
	{
		sh_addmem(8*BRKINCR);
		blokp = BLK(brkbegin);
		stakadr = brkbegin + 6*BRKINCR;
		blokp->word = BLK(stakadr);
		goto first;
	}
	if(sh.stakbas!=sh.staktop)
	{
		register struct blk *blokstak;
		stak_push(0);
		stakadr= sh.stakbas + round(sh.staktop-sh.stakbas,BYTESPERWORD);
		blokstak=BLK(sh.stakbas)-1;
		blokstak->word=sh.stakbsy; sh.stakbsy=blokstak;
		setbusy(bloktop,stakadr);
		bloktop=BLK(stakadr);
	}
	reqd += (sh.staktop-sh.stakbot);
	reqd = round(reqd,BRKINCR);
        if(reqd) {
                sh_addmem((int)reqd+BRKINCR);
#ifdef  KSH_88D
                reqd -= (sh.staktop - sh.stakbot);
#endif /* KSH_88D */
        }
	blokp=bloktop;
	bloktop->word = bloktop+(reqd/sizeof(struct blk*));
	stakadr = (STKPTR)(bloktop->word);
first:
	bloktop = BLK(stakadr);
	setbusy(bloktop,brkbegin);
	stakadr=(STKPTR)(bloktop+2);
	{
		register STKPTR sp = stakadr;
		if(reqd = (sh.staktop-sh.stakbot))
		{
			while(reqd-- > 0)
				*sp++ = *sh.stakbot++;
			sp--;
		}
		sh.staktop = sp;
		sh.stakbas=sh.stakbot=stakadr;
	}
}

/*
 * mark the block free if address is in the heap
 */

void	free(void *ap)
{
	register struct blk *p;
	if((char *)ap>brkbegin && (char *)ap<ADR(bloktop))
	{
		p = (struct blk*)((char *)ap-sizeof(p->word));
		p->word = (struct blk*)(Rcheat(p->word)&~BUSY);
	}
}

/*#ifdef FD_3D*/
/*
 *	reallocates a block obtained from malloc()
 *	to have new size nbytes, and old content
 *	returns new location, or NULL on failure
 */

char *realloc(ap, nbytes)
char	*ap;
unsigned nbytes;
{
	register unsigned size;
	register struct blk *p, *q;
	char *cp;
	unsigned osize;

	p = BLK(ap)-1;
	if(busy(p))
	{
		free((void *)ap);
		blokp = p;
	}
	osize = ADR(p->word) - ap;
	cp = malloc((size_t)nbytes);
	if(cp && cp!=ap)
	{
		char *old, *new;
		old = ADR(ap);
		new = ADR(cp);
		size = round(nbytes,BYTESPERWORD);
		if(osize < size)
			size = osize;
		while(size-- != 0)
			*new++ = *old++;
	}
	return(cp);
}
/*#endif*/ /* FD_3D */

void sh_addmem(int incr)
{
	register char *a;
#ifdef malloc
	if(incr < 0)
	{
		a = (char *)(sbrk(0));
		if(a != (sh.brkend+SLOP))
			incr = 0;
	}
#endif /* malloc */
	a = (char *)(sbrk(incr));
	if((int)a == -1)
		sh_fail(MSGSTR(E_SPACE, (char *)e_space),NIL); /*MSG*/
	if(brkbegin==0)
	{
		/*
		 * The following shouldn't loop if sbrk() returns
		 * an aligned address with the busy bit off
		 */
		while(Rcheat(a)&BUSY)
		{
#ifdef DBUG
			p_setout(ERRIO);
			p_str("Unaligned initial sbrk() address",NL);
			p_flush();
#endif /* DBUG */
			a++;
		}
		brkbegin = a;
	}
	else	/* keep word aligned */
	{
		a = (char*)bloktop + round(a-(char*)bloktop,BYTESPERWORD);
#ifdef DBUG
		if(a != (sh.brkend+SLOP))
		{
			write(2,"addmem: address not contiguous\n",30);
			if((bloktop+2) != BLK(sh.stakbas))
				write(2,"bloktop wrong\n",14);
		}
#endif /* DBUG */
#ifdef malloc
		/*
		 * Standard malloc may also call sbrk(), so check that
		 * space is contiguous
		 */
		if(a != (sh.brkend+SLOP))
		{
			register struct blk *bp = bloktop+1;
			bp->word = sh.stakbsy;
			sh.stakbsy = bp--;
			bp->word = BLK(sh.brkend+SLOP)-1;
			bp = bp->word;
			setbusy(bp,a);
			bloktop = bp = BLK(a);
			setbusy(bp,brkbegin);
			sh.stakbot = sh.stakbas = (STKPTR)(bp+2);
		}
#endif /* malloc */
#ifdef DBUG
		if(a < (char*)bloktop)
		{
			p_setout(ERRIO);
			p_str("sbrk() address not monotonic",NL);
			p_flush();
		}
#endif /* DBUG */
	}
	sh.brkend=a+incr-SLOP;
#ifndef INT16
	if(sh.brkend > brkbegin + BRKMAX)
	{
		sh_fail(MSGSTR(E_SPACE, (char *)e_space),NIL); /*MSG*/
	}
#endif	/* INT16 */
}

#ifdef DBUG
void chkmem()
{
	register struct blk *p = (struct blk*)brkbegin;
	register struct blk *q;
	register int 	us=0, un=0;
	char *cp;

	while(1)
	{
		q = (struct blk*) (Rcheat(p->word)&~BUSY);

		if(q<BLK(brkbegin) || q>bloktop)
			abort();
		if(p==bloktop)
			break;
		if(busy(p))
			us += q-p;
		else
		  	 un += q-p;
		if(p>=q)
		{
			p_flush();
			abort();
		}
		 p=q;
	}
	un *= sizeof(*q);
	us *= sizeof(*q);
	write(ERRIO,"free/used/missing:",18);
	cp = sh_itos(un);
	write(ERRIO,cp,strlen(cp));
	write(ERRIO," ",1);
	cp = sh_itos(us);
	write(ERRIO,cp,strlen(cp));
	write(ERRIO," ",1);
	cp = sh_itos((char*)bloktop - (char*)brkbegin - (un+us));
	write(ERRIO,cp,strlen(cp));
	write(ERRIO,"\n",1);
}

/*
 * returns 1 if <ap> is on heap and is free
 * returns 2 if <ap> is on heap and not beginning of a block
 * otherwise returns 0
 */

int	chkfree(ap)
register char	*ap;
{
	register struct blk* p;
	if(ap>brkbegin && ap<(char*)bloktop)
	{
		p = (struct blk*)(ap-sizeof(p->word));
		if(p->word<BLK(brkbegin) || p->word>bloktop)
			return(2);
		return(!busy(p));
	}
	return(0);
}
#endif	/* DBUG */
