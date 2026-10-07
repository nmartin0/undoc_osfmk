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
static char sccsid[] = "@(#)_line_free.c	1.6  com/lib/curses,3.1,8943 10/16/89 22:58:07";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _line_free
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
 * NAME:        _line_free
 *
 * FUNCTION:
 *
 *      Return a line object to the free list
 */

_line_free (p)
register struct line   *p;
{
	register int i, sl, n=0;
	register struct line **q;

	if (p == NULL)
		return;
#ifdef DEBUG
	if(outf) fprintf(outf,
	"mem: Releaseline (%x), prev SP->freelist %x", p, SP->freelist);
#endif
	sl = lines;
	for (i=sl,q = &SP->cur_body[sl]; i>0; i--,q--)
		if (p == *q)
			n++;
	for (i=sl,q = &SP->std_body[sl]; i>0; i--,q--)
		if (p == *q)
			n++;
#ifdef DEBUG
	if(outf) fprintf(outf, ", count %d\n", n);
#endif
	if (n > 1)
		return;
	p -> next = SP->freelist;
	p -> hash = -888;
	SP->freelist = p;
}
