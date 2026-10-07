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
static char sccsid[] = "@(#)_outch.c	1.6  com/lib/curses,3.1,8943 10/16/89 22:58:42";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _outch
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

#ifdef NLS
#include <NLchar.h>
#endif /* NLS */

#include "cursesext.h"

int outchcount;

/*
 * NAME:        _outch
 *
 * FUNCTION:
 *
 *      Write out one character to the tty.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      This routine is one of the main things
 *      in this level of curses that depends on the outside
 *      environment.
 */

_outch (c)
chtype c;
{
#ifdef DEBUG
# ifndef LONGDEBUG
	if (outf)
		if (c < ' ')
			fprintf(outf, "^%c", (c+'@')&0177);
		else
			fprintf(outf, "%c", c&0177);
# else LONGDEBUG
	if(outf) fprintf(outf, "_outch: char '%s' term %x file %x=%d\n",
		unctrl(c&0177), SP, SP->term_file, fileno(SP->term_file));
# endif /* LONGDEBUG */
#endif /* DEBUG */

#ifdef NLS
	if (!NCisshift(c))
		outchcount++;
#else
	outchcount++;
#endif
	if (SP && SP->term_file)
#ifdef NLS
		putc (c, SP->term_file);
#else
		putc (c&0177, SP->term_file);
#endif
	else
#ifdef NLS
		putc (c, stdout);
#else
		putc (c&0177, stdout);
#endif
}
