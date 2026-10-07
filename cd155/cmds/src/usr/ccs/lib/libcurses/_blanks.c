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
static char sccsid[] = "@(#)_blanks.c	1.6  com/lib/curses,3.1,8943 10/16/89 22:53:12";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _blanks
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
extern	int	_sethl();
extern	int	_setmode();
extern	char	*tparm();
extern	int	tputs();

/*
 * NAME:        _blanks
 *
 * FUNCTION:    Output n blanks, or the equivalent.
 *
 *
 * EXECUTION ENVIRONMENT:
 *
 *      This is done to erase text, and
 *      also to insert blanks.  The semantics of this call do not define
 *      where it leaves the cursor - it might be where it was before, or
 *      it might be at the end of the blanks.  We will, of course, leave
 *      SP->phys_x properly updated.
 */

_blanks (n)
{
#ifdef DEBUG
	if(outf) fprintf(outf, "_blanks(%d).\n", n);
#endif
	if (n == 0)
		return;
	_setmode ();
	_sethl ();
	if (SP->virt_irm==1 && parm_ich) {
		if (n == 1)
			tputs(insert_character, 1, _outch);
		else
			tputs(tparm(parm_ich, n), n, _outch);
		return;
	}
	if (erase_chars && SP->phys_irm != 1 && n > 5) {
		tputs(tparm(erase_chars, n), n, _outch);
		return;
	}
	if (repeat_char && SP->phys_irm != 1 && n > 5) {
		tputs(tparm(repeat_char, ' ', n), n, _outch);
		SP->phys_x += n;
		return;
	}
	while (--n >= 0) {
		if (SP->phys_irm == 1 && insert_character)
			tputs (insert_character,
				columns - SP->phys_x, _outch);
		if (++SP->phys_x >= columns && auto_right_margin) {
			if (SP->phys_y >= lines-1) {
				/*
				 * We attempted to put something in the last
				 * position of the last line.  Since this will
				 * cause a scroll (we only get here if the
				 * terminal has auto_right_margin) we refuse
				 * to put it out.
				 */
				SP->phys_x--;
				return;
			}
			SP->phys_x = 0;
			SP->phys_y++;
		}
		_outch (' ');
		if (SP->phys_irm == 1 && insert_padding)
			tputs (insert_padding, 1, _outch);
	}
}
