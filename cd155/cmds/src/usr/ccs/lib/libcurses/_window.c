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
static char sccsid[] = "@(#)_window.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:02:45";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _window
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

/*
 * NAME:        _window
 *
 * FUNCTION:
 *
 *      Set the desired window to the box with the indicated boundaries.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      All scrolling should only affect the area inside the window.
 *      We currently ignore the last 2 args since we're only using this
 *      for scrolling and want to use the feature on vt100's as well as
 *      on concept 100's.  left and right are for future expansion someday.
 *
 *      Note that we currently assume cursor addressing within the window
 *      is relative to the screen, not the window.  This will have to be
 *      generalized if concept windows are to be used.
 */

_window(top, bottom, left, right)
int top, bottom, left, right;
{
#ifdef DEBUG
	if(outf) fprintf(outf,
		"_window old top=%d, bot %d; new top=%d, bot %d\n",
			SP->des_top_mgn, SP->des_bot_mgn, top, bottom);
#endif
	if (change_scroll_region || set_window) {
		SP->des_top_mgn = top;
		SP->des_bot_mgn = bottom;
	}
#ifdef DEBUG
	else
		if(outf) fprintf(outf, "window setting ignored\n");
#endif
}
