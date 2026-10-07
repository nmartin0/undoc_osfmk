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
static char sccsid[] = "@(#)doupdate.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:14:35";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   doupdate
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

#include        "cursesext.h"

extern  WINDOW *lwin;

/*
 * NAME:        doupdate
 *
 * FUNCTION:
 *
 *      Make the current screen look like "win" over the area covered by
 *      win.
 */

doupdate()
{
	int rc;
	extern int _endwin;
	int _outch();

#ifdef	DEBUG
	if(outf) fprintf( outf, "doupdate()\n" );
#endif	/* DEBUG */

	if( lwin == NULL )
	{
		return ERR;
	}

	if( _endwin )
	{
		/*
		 * We've called endwin since last refresh.  Undo the
		 * effects of this call.
		 */

		_fixdelay(FALSE, SP->fl_nodelay);
		if (stdscr->_use_meta)
			tputs(meta_on, 1, _outch);
		_endwin = FALSE;
		SP->doclear = TRUE;
		reset_prog_mode();
	}

	/* Tell the back end where to leave the cursor */
	if( lwin->_leave || lwin->_flags&_ISPAD )
	{
#ifdef	DEBUG
		if(outf) fprintf( outf, "'_ll_move(-1, -1)' being done.\n" );
#endif	/* DEBUG */
		_ll_move(-1, -1);
	}
	else
	{
#ifdef	DEBUG
if(outf) fprintf( outf,
"'lwin->_cury+lwin->_begy, lwin->_curx+lwin->_begx' being done.\n" );

#endif	/* DEBUG */
		_ll_move( lwin->_cury+lwin->_begy, lwin->_curx+lwin->_begx );
	}
#ifdef	DEBUG
if(outf) fprintf( outf, "doing 'rc = _ll_refresh(lwin->_use_idl)'.\n" );

#endif	/* DEBUG */
	rc = _ll_refresh(lwin->_use_idl);
#ifdef	DEBUG
	_dumpwin(lwin);
#endif	/* DEBUG */
	return rc;
}
