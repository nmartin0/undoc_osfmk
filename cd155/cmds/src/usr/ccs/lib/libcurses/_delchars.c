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
static char sccsid[] = "@(#)_delchars.c	1.6  com/lib/curses,3.1,8943 10/16/89 22:54:48";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _delchars
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
 * NAME:        _delchars
 *
 * FUNCTION:
 *
 *      Delete n characters.
 *
 */

_delchars (n)
{
#ifdef DEBUG
	if(outf) fprintf(outf, "_delchars(%d).\n", n);
#endif
	if (enter_delete_mode) {
		if (strcmp(enter_delete_mode, enter_insert_mode) == 0) {
			if (SP->phys_irm == 0) {
				tputs(enter_delete_mode,1,_outch);
				SP->phys_irm = 1;
			} 
		}
		else {
			if (SP->phys_irm == 1) {
				tputs(exit_insert_mode,1,_outch);
				SP->phys_irm = 0;
			}
			tputs(enter_delete_mode,1,_outch);
		}
	}
	while (--n >= 0) {
		/* Only one line can be affected by our delete char */
		tputs(delete_character, 1, _outch);
	}
	if (exit_delete_mode) {
		if (strcmp(exit_delete_mode, exit_insert_mode) == 0)
			SP->phys_irm = 0;
		else
			tputs(exit_delete_mode, 1, _outch);
	}
}
