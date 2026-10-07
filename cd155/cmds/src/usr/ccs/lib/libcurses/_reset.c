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
static char sccsid[] = "@(#)_reset.c  1.6  com/lib/curses,3.1,8943 10/16/89 22:59:30";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _reset
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
 * NAME:        _reset
 */

_reset ()
{
#ifdef DEBUG
	if(outf) fprintf(outf, "_reset().\n");
#endif
	tputs(enter_ca_mode, 0, _outch);
	tputs(cursor_visible, 0, _outch);
	tputs(exit_attribute_mode, 0, _outch);
	tputs(clear_screen, 0, _outch);
	SP->phys_x = 0;
	SP->phys_y = 0;
	SP->phys_irm = 1;
	SP->virt_irm = 0;
	SP->phys_top_mgn = 4;
	SP->phys_bot_mgn = 4;
	SP->des_top_mgn = 0;
	SP->des_bot_mgn = lines-1;
	SP->ml_above = 0;
	_setwind();
}
