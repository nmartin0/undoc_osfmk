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
static char sccsid[] = "@(#)line_alloc.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:24:08";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _line_alloc
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
 * NAME:        _line_alloc
 *
 * FUNCTION:
 *
 *      Return a pointer to a new line structure.
 */

struct line *
_line_alloc ()
{
	register struct line   *rv = SP->freelist;
	char *calloc();

#ifdef DEBUG
	if(outf) fprintf(outf,
	"mem: _line_alloc (), prev SP->freelist %x\n", SP->freelist);
#endif
	if (rv) {
		SP->freelist = rv -> next;
	} else {
#ifdef NONSTANDARD
		_ec_quit("No lines available in line_alloc", "");
#else
		rv = (struct line *) calloc (1, sizeof *rv);
		rv -> body = (chtype *) calloc (columns, sizeof (chtype));
#endif
	}
	rv -> length = 0;
	rv -> hash = 0;
	return rv;
}
