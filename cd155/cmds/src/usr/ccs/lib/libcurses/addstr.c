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
static char sccsid[] = "@(#)addstr.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:03:19";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   waddstr
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

# include	"cursesext.h"

/*
 * NAME:        waddstr
 *
 * FUNCTION:
 *
 *      This routine adds a string starting at (_cury,_curx)
 *
 */

waddstr(win,str)
register WINDOW	*win; 
register char	*str;
{
# ifdef DEBUG
	if(outf)
	{
		if( win == stdscr )
		{
			fprintf(outf, "WADDSTR(stdscr, ");
		}
		else
		{
			fprintf(outf, "WADDSTR(%o, ", win);
		}
		fprintf(outf, "\"%s\")\n", str);
	}
# endif	DEBUG
	while( *str )
	{
		if( waddch( win, ( chtype ) *str++ ) == ERR )
		{
			return ERR;
		}
	}
	return OK;
}
