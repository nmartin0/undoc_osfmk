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
static char rcsid[] = "@(#)$RCSfile: print.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:17 $";
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
#include	<time.h>
#include	<sys/limits.h>

#define		BUFLEN		256

static uchar_t	buffer[BUFLEN];
static int	index = 0;
uchar_t		numbuf[17];	/*  changed from 12 to 14 - PTM 10017  */
                                /*  changed form 14 to 17 - PTM 12638  */

extern void	prc_buff();
extern void	prs_buff();
extern void	prn_buff();
extern void	prs_cntl();
extern void	prn_buff();

/*
 * printing and io conversion
 */
prp()
{
	if ((flags & prompt) == 0 && cmdadr)
	{
		prs_cntl(cmdadr);
		prs(colon);
	}
}

prs(as)
uchar_t	*as;
{
	register uchar_t	*s;

	if (s = as) {
		if (NLSisencoded(s))
			s = NLSndecode(s);
		write(output, s, length(s) - 1);
	}
}

prc(c)
uchar_t	c;
{
	if (c)
		write(output, &c, 1);
}

prt(t)
clock_t	t;
{
	register int hr, min, sec;

	t += CLK_TCK / 2;
	t /= CLK_TCK;
	sec = t % CLK_TCK;
	t /= CLK_TCK;
	min = t % CLK_TCK;

	if (hr = t / CLK_TCK)
	{
		prn_buff(hr);
		prc_buff('h');
	}

	prn_buff(min);
	prc_buff('m');
	prn_buff(sec);
	prc_buff('s');
}

prn(n)
	int	n;
{
	itos(n);

	prs(numbuf);
}

itos(n)
{
	register uchar_t *abuf;
	register unsigned a, i;
	int pr, d;

	abuf = numbuf;

	pr = FALSE;
	a = n;
	for (i = 1000000000; i != 1; i /= 10)
	{
		if ((pr |= (d = a / i)))
			*abuf++ = d + '0';
		a %= i;
	}
	*abuf++ = a + '0';
	*abuf++ = 0;
}

stoi(icp)
uchar_t	*icp;
{
	register uchar_t	*cp = icp;
	register int	r = 0;
	register uchar_t	c;

#if defined(NLS) || defined(KJI)
	if (NLSisencoded(icp)) NLSdecode(icp);
#endif

	while ((c = *cp, digit(c)) && c && r >= 0)
	{
		r = r * 10 + c - '0';
		cp++;
	}
	if (r < 0 || cp == icp)
		failed(icp, MSGSTR(M_BADNUM,(char *)badnum));
	else
		return(r);
}

prl(n)
long n;
{
	int i;

	i = 11;
	while (n > 0 && --i >= 0)
	{
		numbuf[i] = n % 10 + '0';
		n /= 10;
	}
	numbuf[11] = '\0';
	prs_buff(&numbuf[i]);
}

void
flushb()
{
	if (index)
	{
		buffer[index] = '\0';
		write(1, buffer, length(buffer) - 1);
		index = 0;
	}
}

void
prc_buff(c)
	uchar_t c;
{
	if (c)
	{
		if (index + 1 >= BUFLEN)
			flushb();

		buffer[index++] = c;
	}
	else
	{
		flushb();
		write(1, &c, 1);
	}
}

void
prs_buff(s)
	uchar_t *s;
{
	register int len;
#if defined(NLS) || defined(KJI)
	if (NLSisencoded(s))
		s = NLSndecode(s);
#endif
	len = length(s) - 1;

	if (index + len >= BUFLEN)
		flushb();

	if (len >= BUFLEN)
		write(1, s, len);
	else
	{
		movstr(s, &buffer[index]);
		index += len;
	}
}


clear_buff()
{
	index = 0;
}


void
prs_cntl(s)
	uchar_t *s;
{
	register uchar_t *ptr = buffer;
	register int c;

	if (NLSisencoded(s)) s++;
/*
 * this line was changed to check the boundry of the buffer so
 * that it doesn't exceed BUFLEN, because when it does it trashes
 * another static variable "cwdname" in pwd.c and causes the 
 * internal pwd command to become broken when the input is 
 * >127 control uchar_tacters, dewey coffman, 7-30-87
 */

	while((*s != '\0') && ((ptr - buffer) < (BUFLEN - 1)))
	{
		/* translate a control character into a printable sequence */
#if defined(NLS) || defined(KJI)
		c = *s;
# ifdef KJI
		if (NCisshift(c) && s[1])
# else
		if (fontshift(c) && s[1])
# endif
		{
			*ptr++ = c;
			c = *++s;
		}
#else
		c = (*s & 0177) ;
#endif
#ifdef KJI
		if ((c < '\040') && (!NLSfontshift (c))) 
		/* magic font shifts will be handled in prs() */
#else
		if (c < '\040')
#endif
		{	/* assumes ASCII uchar_t */
			*ptr++ = '^';
			*ptr++ = (c + 0100);	/* assumes ASCII uchar_t */
		}
#ifdef NLS
		/* keep 0177s in - these are FSH0 uchar_ts handled by prs() */
#else
		else if (c == 0177) 
		{	/* '\0177' does not work */
			*ptr++ = '^';
			*ptr++ = '?';
		}
#endif
		else 
		{	/* printable uchar_tacter */
			*ptr++ = c;
		}

		++s;
	}

	*ptr = '\0';
	prs(buffer);
}


void
prn_buff(n)
	int	n;
{
	itos(n);

	prs_buff(numbuf);
}
