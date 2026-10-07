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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)cntcostfn.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:08:46";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _countchar, _cost_fn
 *
 * ORIGINS: 3, 10, 27
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
 */

#include "cursesext.h"

static counter = 0;

/*
 * NAME:        _countchar
 */

_countchar(ch)
char ch;
{
	counter++;
}

/*
 * NAME:        _cost_fn
 *
 * FUNCTION:
 *
 *      Figure out the _cost in characters to print this string.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Due to padding, we can't just use strlen, so instead we
 *      feed it through tputs and trap the results.
 *      Even if the terminal uses xon/xoff handshaking, count the
 *      pad chars here since they estimate the real time to do the
 *      operation, useful in calculating costs.
 */

_cost_fn(str, affcnt)
char *str;
{
	int save_xflag = xon_xoff;

	if (str == NULL)
		return INFINITY;
	counter = 0;
	xon_xoff = 0;
	tputs(str, affcnt, _countchar);
	xon_xoff = save_xflag;
	return counter;
}
