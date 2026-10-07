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
static char sccsid[] = "@(#)_scrollf.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:00:06";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _scrollf
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

extern	int	_outch();

/*
 * NAME:        _scrollf
 *
 * FUNCTION:
 *
 *      Scroll the terminal forward n lines, bringing up blank lines from
 *      bottom.  This only affects the current scrolling region.
 */

_scrollf(n)
int n;
{
	register int i;

	if( scroll_forward )
	{
		_setwind();
		_pos( SP->des_bot_mgn, 0 );
		for( i=0; i<n; i++ )
		{
			tputs(scroll_forward, 1, _outch);
		}
		SP->ml_above += n;
		if( SP->ml_above + lines > lines_of_memory )
		{
			SP->ml_above = lines_of_memory - lines;
		}
	}
	else
	{
		/* If terminal can't do it, try delete line. */
		_pos(0, 0);
		_dellines(n);
	}
}
