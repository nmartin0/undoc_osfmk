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
static char sccsid[] = "@(#)addch.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:03:02";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   waddch
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
#ifdef NLS
static char copy[2];
#endif

/*
 * NAME:        waddch
 *
 * FUNCTION:
 *
 *      This routine prints the character in the current position.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Think of it as putc.
 */

waddch(win, c)
register WINDOW	*win;
register chtype		c;
{
	register int		x, y;
	char *uctr;
#ifdef NLS
	short isshift;
	register NLchar rawc = c & A_CHARTEXT;
#else
	register char rawc = c & A_CHARTEXT;
#endif

	x = win->_curx;
	y = win->_cury;
#ifdef NLS
	isshift = (x > 0) && (NCisshift(win->_y[y][x-1]));
#endif
# ifdef DEBUG
	if (outf)
		if (c == rawc)
			fprintf(outf, "'%c'", rawc);
		else
			fprintf(outf, "'%c' %o, raw %o", c, c, rawc);
# endif
#ifdef NLS
	/* 
	 * Don't worry about overflow if there is a pending shift 
	 * character.
	 */
	if (!isshift &&
		(y >= win->_maxy || x >= win->_maxx || y < 0 || x < 0))
#else
	if (y >= win->_maxy || x >= win->_maxx || y < 0 || x < 0)
#endif
	{
# ifdef DEBUG
if(outf)
{
fprintf(outf,"off edge, (%d,%d) not in (%d,%d)\n",y,x,win->_maxy,win->_maxx);
}
# endif
		return ERR;
	}
	switch( rawc )
	{
	case '\t':
		{
			register int newx;

			for( newx = x + (8 - (x & 07)); x < newx; x++ )
			{
				if( waddch(win, ' ') == ERR )
				{
					return ERR;
				}
			}
			return OK;
		}
	  default:
#ifdef NLS
		if( (rawc < ' ' && !NCisshift(rawc)) || rawc == 0177 )
#else
		if( rawc < ' ' || rawc > '~' )
#endif
		{
			uctr = unctrl(rawc);
			waddch(win, (chtype)uctr[0]|(c&A_ATTRIBUTES));
			waddch(win, (chtype)uctr[1]|(c&A_ATTRIBUTES));
			return OK;
		}
		if( win->_attrs )
		{
#ifdef	DEBUG
if(outf) fprintf(outf,
	"(attrs %o, %o=>%o)", win->_attrs, c, c | win->_attrs);
#endif	/* DEBUG */
			c |= win->_attrs;;
		}
		if( win->_y[y][x] != c )
		{
			if( win->_firstch[y] == _NOCHANGE )
			{
				win->_firstch[y] = win->_lastch[y] = x;
			}
			else
			{
				if( x < win->_firstch[y] )
				{
					win->_firstch[y] = x;
				}
				else
				{
					if( x > win->_lastch[y] )
					{
						win->_lastch[y] = x;
					}
				}
			}
		}
#ifdef NLS
		/*
		 * If we receive one byte and there is a pending shift
		 * character, form an NLS character and put it in the
		 * window struct, or replace the pending shift character
		 * if the follwing byte does not have the 8th bit on
		 * or if it does not represent a value on the P0 code page.
		 */
		if (isshift)
			if (rawc <= 255 && rawc & 0x80) {
				copy[1] = rawc;
				copy[0] = win->_y[y][x-1];
				win->_y[y][x-1] =
				(NCdechr(copy) | (c & A_ATTRIBUTES));
			}
			else
				win->_y[y][x-1] = c;
		else
			win->_y[y][x++] = c;
#else
		win->_y[y][x++] = c;
#endif
#ifdef NLS
		/*
	   	 * Don't worry about overflowing if we're adding a
		 * pending shift character.
		 */

		if (!NCisshift(rawc) && x >= win->_maxx)
#else
		if (x >= win->_maxx)
#endif
		{
			x = 0;
new_line:
			if (++y > win->_bmarg)
			{
				if (win->_scroll && !(win->_flags&_ISPAD))
				{
#ifdef	DEBUG
	if(outf) {
		fprintf( outf, "Calling  _tscroll(  0%o  )\n", win );
	}
#endif	/* DEBUG */
					_tscroll( win );
					--y;
				}
				else
				{
# ifdef DEBUG
					int i;
					if(outf)
					{
					    fprintf(outf,
					    "ERR because (%d,%d) > (%d,%d)\n",
					    x, y, win->_maxx, win->_maxy);
					    fprintf(outf, "line: '");
					    for (i=0; i<win->_maxy; i++)
						fprintf(outf, "%c",
							win->_y[y-1][i]);
					    fprintf(outf, "'\n");
					}
# endif	/* DEBUG */
					return ERR;
				}
			}
		}
# ifdef DEBUG
if(outf) fprintf(outf,
	"ADDCH: 2: y = %d, x = %d, firstch = %d, lastch = %d\n", y, x,
		win->_firstch[y], win->_lastch[y]);
# endif	/* DEBUG */
		break;
	  case '\n':
		wclrtoeol(win);
		x = 0;
		goto new_line;
	  case '\r':
		x = 0;
		break;
	  case '\b':
		if (--x < 0)
			x = 0;
		break;
	}
	win->_curx = x;
	win->_cury = y;
	return OK;
}
