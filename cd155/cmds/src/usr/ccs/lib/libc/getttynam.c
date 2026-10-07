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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: getttynam.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:57 $";
#endif
/*
 * FUNCTIONS: getttynam 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * getttynam.c	1.6  com/lib/c/io,3.1,8943 9/12/89 18:32:59
 */

#include <ttyent.h>
#ifdef _THREAD_SAFE
#include <stdio.h>		/* for BUFSIZ */
#include "rec_mutex.h"

extern struct rec_mutex _getttyent_rmutex;
#endif

/*
 * NAME: getttynam
 *
 * FUNCTION: searches the tty file for the name passed to it.
 *
 * RETURN VALUE DESCRIPTION: 
 *	returns a pointer to an object with ttyent structure that contains
 *	tty in the name field or a null pointer if EOF is reached or error
 */

#ifdef _THREAD_SAFE
int
getttynam_r(const char *tty, struct ttyent *tte, char *buf)
#else
struct ttyent *      
getttynam(tty)
const char *tty; /* the name of the tty description that is requested */
#endif
{
#ifdef _THREAD_SAFE
	int	rc;
#else
	register struct ttyent *tte;   /* tty description holder */
#endif

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_getttyent_rmutex);
#endif

	setttyent();
#ifdef _THREAD_SAFE
	while ((rc = getttyent_r(tte, buf)) == 0) {
#else
	while (tte = getttyent()) {
#endif
		if (strcmp(tty, tte->ty_name) == 0)
			break;
	}
	endttyent();

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_getttyent_rmutex);
	return(rc);
#else
	return(tte);
#endif
}

