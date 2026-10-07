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
static char sccsid[] = "@(#)tputs.c  1.11  com/lib/curses,3.1,9008 1/30/90 19:54:27";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   tputs, _tpad
 *
 * ORIGINS: 3, 10, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1979 Regents of the University of California.
 */

#include <ctype.h>
#include "curses.h"
#include "term.h"
#ifdef NONSTANDARD
# include "ns_curses.h"
#endif

/*
 * NAME:        tputs
 *
 * FUNCTION:
 *
 *      Put the character string cp out, with padding.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      The number of affected lines is affcnt, and the routine
 *      used to output one character is outc.
 */

tputs(cp, affcnt, outc)
	register char *cp;
	int affcnt;
	int (*outc)();
{
	static char *_tpad();   /* uncommented static -- TEJ */
					/* support non-VTD only.        */
	if (cp == NULL || *cp == (char)(0x80) ||
		*cp == (char)(0x00))
		return;

	/*
	 * The guts of the string.
	 */
	while (*cp)
		if (*cp == '$' && cp[1] == '<')
			cp = _tpad(cp, affcnt, outc);
		else
			(*outc)(*cp++);
}

/*
 * NAME:        _tpad
 */

static char *
_tpad(cp, affcnt, outc)
	register char *cp;
	int affcnt;
	int (*outc)();
{
	register int delay = 0;
	register char *icp = cp;

	/* Eat initial $< */
	cp += 2;

	/*
	 * Convert the number representing the delay.
	 */
	if (isdigit(*cp)) {
		do
			delay = delay * 10 + *cp++ - '0';
		while (isdigit(*cp));
	}
	delay *= 10;
	if (*cp == '.') {
		cp++;
		if (isdigit(*cp))
			delay += *cp - '0';
		/*
		 * Only one digit to the right of the decimal point.
		 */
		while (isdigit(*cp))
			cp++;
	}

	/*
	 * If the delay is followed by a `*', then
	 * multiply by the affected lines count.
	 */
	if (*cp == '*')
		cp++, delay *= affcnt;
	if (*cp == '>')
		cp++;	/* Eat trailing '>' */
	else {
		/*
		 * We got a "$<" with no ">".  This is usually caused by
		 * a cursor addressing sequence that happened to generate
		 * $<.  To avoid an infinite loop, we output the $ here
		 * and pass back the rest.
		 */
		(*outc)(*icp++);
		return icp;
	}

	/*
	 * If no delay needed, or output speed is
	 * not comprehensible, then don't try to delay.
	 */
	if (delay == 0)
		return cp;
	/*
	 * Let handshaking take care of it - no extra cpu load from pads.
	 * Also, this will be more optimal since the pad info is usually
	 * worst case.  We only use padding info for such terminals to
	 * estimate the cost of a capability in choosing the cheapest one.
	 */
	if (xon_xoff)
		return cp;
	(void) _delay(delay, outc);
	return cp;
}
