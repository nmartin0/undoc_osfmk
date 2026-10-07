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
static char sccsid[] = "@(#)curses.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:09:43";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   curses.c
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
 */

# include       "cursesext.h"

/*
 * NAME:        curses.c
 *
 * FUNCTION:
 *
 *      Define global variables.
 */

char	*Def_term	= "unknown";	/* default terminal type	*/
WINDOW *stdscr, *curscr;
int	LINES, COLS;
struct screen *SP;

/* char *curses_version = "Packaged for USG UNIX 6.0, 3/6/83"; */
char *curses_version = " ";

# ifdef DEBUG
FILE	*outf;			/* debug output file			*/
# endif

struct	term _first_term;
struct	term *cur_term = &_first_term;

WINDOW *lwin;

int _endwin = FALSE;

int	tputs();
