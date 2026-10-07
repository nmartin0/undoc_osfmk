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

#ident "@(#)strdup.c	2.1 16:18:28 4/20/90 SecureWare"
/*
 * Based on:
 *   "@(#)strdup.c	1.1.1.1 13:44:53 12/18/89 SecureWare"
 */

#include <sys/types.h>

#if defined(AUX) || defined(_OSF_SOURCE)

extern char	*malloc();

/*
 * Return a copy of the passed string
 */
char *
strdup(s)
        char	*s;
{
	register char	*new;

	if (new = malloc(strlen(s) + 1))
		strcpy(new, s);
	return new;
}

#endif
