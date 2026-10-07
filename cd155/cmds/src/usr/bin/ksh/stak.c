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
static char rcsid[] = "@(#)$RCSfile: stak.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:30 $";
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
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
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
 * UNIX shell
 *
 * S. R. Bourne
 * Rewritten by David Korn
 * AT&T Bell Laboratories
 *
 */

#include	"defs.h"


/* ========	storage allocation	======== */

/*
 * allocate requested stack
 */

STKPTR	stak_alloc(unsigned  asize)
{
	register STKPTR oldstak;
	register int 	size;
	size=round(asize,BYTESPERWORD);
	while((sh.staktop=sh.stakbot+size) > sh.brkend)
		sh_addmem((int)round(sh.staktop-sh.brkend,BRKINCR));
	oldstak=sh.stakbot;
	sh.stakbot = sh.staktop;
	return(oldstak);
}

/*
 * set up stack for local use
 * should be followed by `stak_end'
 */

STKPTR	stak_begin(void)
{
	if(sh.brkend-sh.stakbot<BRKINCR)
		sh_addmem(BRKINCR);
	return(sh.staktop=sh.stakbot);
}


/*
 * tidy up after `stak_begin'
 */

STKPTR	stak_end(char *argp)
{
	register STKPTR oldstak;
	*argp++=0;
	oldstak=sh.stakbot;
	sh.stakbot += round((STKPTR)argp-sh.stakbot,BYTESPERWORD);
	sh.staktop = sh.stakbot;
	return(oldstak);
}

/*
 * try to bring stack back to x
 */

void	stak_reset(STKPTR  x)
{
	while(ADR(sh.stakbsy)>ADR(x))
	{
		free((void *)sh.stakbsy);
		sh.stakbsy = sh.stakbsy->word;
	}
	sh.staktop=sh.stakbot=max(ADR(x),ADR(sh.stakbas));
	if(st.iotemp > (struct ionod*)x)
		io_rmtemp((struct ionod*)x);
}

stak_check(void)
{
	register int size = -3*BRKINCR;
#ifdef INT16
	if((sh.brkend-sh.stakbas) >= BRKMAX)
		size = BRKMAX;
	else
#endif /* INT16 */
		size += round(sh.brkend-sh.stakbas,BRKINCR);
	if(size > 0)
		sh_addmem(-size);
}

STKPTR	stak_copy(const char *x)
{
	register const char *y=x;
	while(*y++);
	return((STKPTR)strcpy(stak_alloc((unsigned)(y-x)),x));
}
