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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: stak.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:25 $";
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

/* ========	storage allocation	======== */

uchar_t *
getstak(asize)			/* allocate requested stack */
int	asize;
{
	register uchar_t	*oldstak;
	register ptrdiff_t	size;

#ifdef	_SHRLIB
	locstak();	/* make sure stack is big enough */
#endif
	size = round(asize, BYTESPERWORD);
	oldstak = stakbot;
	staktop = stakbot += size;
	needmem (stakbot + BRKINCR);
	return(oldstak);
}

/*
 * set up stack for local use
 * should be followed by `endstak'
 */
uchar_t *
locstak()
{
#ifdef	_SHRLIB
	/* in shared case we must have enough for any possible glob since malloc is
	 * the standard library version - so get BRKMAX always.
	 * N.B.  this must be larger than the (NLS) max path name
	 * and the CPYSTR in io.c
	 */
	if (brkend - stakbot < BRKMAX)
		if (growstak())
			error(MSGSTR(M_NOSPACE,(char *) nospace));
#else
# if !NOBACKUP
	if (brkend - stakbot < BRKINCR)
	{
		if (setbrk(brkincr) == -1)
			error(MSGSTR(M_NOSPACE,(char *)nospace));
				/*	PTM 19932
				*	do not increment brkincr, brkincr
				*	is set to allocate 5 pages
		if (brkincr < BRKMAX)
			brkincr += 256;
				*/
	}
# endif
#endif
	needmem (stakbot + BRKINCR);
	return(stakbot);
}

uchar_t *
savstak()
{
	assert(staktop == stakbot);
	return(stakbot);
}

uchar_t *
endstak(argp)		/* tidy up after `locstak' */
register uchar_t	*argp;
{
	register uchar_t	*oldstak;

	*argp++ = 0;
	oldstak = stakbot;
	stakbot = staktop = (uchar_t *)round(argp, BYTESPERWORD);
	return(oldstak);
}

tdystak(x)		/* try to bring stack back to x */
register uchar_t	*x;
{
#ifndef	_SHRLIB
	while ((uchar_t *)stakbsy > x)
	{
		free(stakbsy);
		stakbsy = stakbsy->word;
	}
#endif
	staktop = stakbot = max(x, stakbas);
	rmtemp(x);
}

stakchk()
{
#ifdef	_SHRLIB
	if (brkend - stakbas > BRKMAX+BRKMAX)
		stakbas = (uchar_t *) realloc(stakbas,(BRKMAX+BRKMAX));
#else
	if ((brkend - stakbas) > BRKINCR + BRKINCR)
		setbrk(-BRKINCR);
#endif
}

uchar_t *
cpystak(x)
uchar_t	*x;
{
	register uchar_t    *argp = locstak ();
#ifdef	_SHRLIB
	assert(argp+length(x)+1 < brkend);
	locstak();	/* make sure its big enough */
#else
	needmem (argp + length (x) + BRKINCR);
#endif
	return (endstak(movstr(x,argp)));
}

#ifdef	_SHRLIB

int
growstak()
{
	uchar_t	*oldbase;
	unsigned int	size;
	if (stakbot == 0)
	{
		size = BRKMAX+BRKMAX;
		stakbot = malloc(size);
		brkend = stakbot + size;
		stakbas = staktop = stakbot;
		return stakbas?0:-1;
	}
	size = (Rcheat(brkend) - Rcheat(stakbas));
	size += BRKMAX;
	oldbase = stakbas;
	stakbas = (uchar_t *) realloc(stakbas,size);
	if (stakbas){
		unsigned int	reloc;
		reloc = Rcheat(stakbas) - Rcheat(oldbase);
		stakbot += reloc;
		staktop += reloc;
		brkend = stakbas + size;
		return 0;
	}
	else return -1;
}

#endif	/**  _SHRLIB  **/
