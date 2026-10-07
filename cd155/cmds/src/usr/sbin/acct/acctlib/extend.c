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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: extend.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:45 $";
#endif
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !lint
static char sccsid[] = "extend.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:17:32";
#endif
/* Extend table pointed to by tb.  Returns NULL if unsuccessful. */

#ifdef NLS
#include <standards.h>
#endif
#ifndef NULL
#define NULL	((char *)0)
#endif
#include <sys/types.h>
#include "table.h"
char *realloc(), *malloc();

char *
extend(tb)
register struct table *tb;
{       register unsigned sz = (unsigned)tb->tb_nel * tb->tb_elsize;
	register char *space = *tb->tb_base;

	extern void free();
	/* Check for overflow */
	if ((unsigned) tb->tb_nel != sz/tb->tb_elsize)
		space = NULL;
	else if (space) {
		free(space);
		space = realloc(space,sz);
	} else
		space = malloc(sz);
	return(*tb->tb_base = space);
}

