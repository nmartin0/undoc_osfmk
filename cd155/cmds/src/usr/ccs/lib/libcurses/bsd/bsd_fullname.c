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
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)fullname.c	5.3 (Berkeley) 6/30/88";
#endif /* not lint */

# define	reg	register

#include "curses.ext"

/*
 *	This routine fills in "def" with the full name of the terminal.
 * This is assumed to be the last name in the list of aliases.
 *
 */
char *
fullname(bp, def)
reg char	*bp, *def;
{

	reg char	*cp;

	*def = 0;			/* in case no name */

	while (*bp && *bp != ':') {
		cp = def;		/* start of answer */
		while (*bp && *bp != ':' && *bp != '|') {
			*cp++ = *bp++;	/* copy name over */
		}
		*cp = 0;		/* zero end of name */
		if (*bp == '|') {
			bp++;		/* skip over '|' if that is case */
		}
	}
	return(def);
}
