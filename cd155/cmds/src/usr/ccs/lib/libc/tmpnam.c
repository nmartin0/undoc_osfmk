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
static char rcsid[] = "@(#)$RCSfile: tmpnam.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:09 $";
#endif

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex	_tmpnam_rmutex;
#endif

extern char *mktemp(), *strcpy(), *strcat();
static char str[L_tmpnam], seed[] = { 'a', 'a', 'a', '\0' };

char *
tmpnam(char *s)
{
	register char *p, *q;

#ifdef _THREAD_SAFE
	if (s == NULL) {
		seterrno(EINVAL);
		return(NULL);
	}

	p = s;
#else
	p = (s == NULL)? str: s;
#endif
	(void) strcpy(p, P_tmpdir);
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_tmpnam_rmutex);
#endif
	(void) strcat(p, seed);
	(void) strcat(p, "XXXXXX");

	q = seed;
	while(*q == 'z')
		*q++ = 'a';
	if(*q != '\0')  {
		++*q;
	}
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_tmpnam_rmutex);
#endif

	(void) mktemp(p);
	return(p);
}
