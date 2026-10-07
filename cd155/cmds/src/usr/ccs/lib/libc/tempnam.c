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
static char rcsid[] = "@(#)$RCSfile: tempnam.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:06 $";
#endif

/*
 *static char sccsid[] = "@(#)tempnam.c	1.11  com/lib/c/io,3.1,9013 1/16/90 18:05:25";
 *
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: tempnam 
 *
 * ORIGINS: 3, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*LINTLIBRARY*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/access.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex	_tempnam_rmutex;
#endif

#ifdef _KJI 
#include <sys/NLchar.h>
#endif /* _KJI */

extern char *mktemp();

static char *pcopy();
#define MKTLEN 6		/* six characters for mktemp() */
#define SEEDLEN 3		/* length of following seed */

static char seed[] = { 'A', 'A', 'A', '\0' };

/*                                                                    
 * FUNCTION: Returns temporary file name.
 *                                                                    
 * RETURN VALUE DESCRIPTION: char * filename, NULL on error.
 */  

char *
tempnam(
char *dir,		/* use this directory please (if non-NULL) */
char *pfx)		/* use this (if non-NULL) as filename prefix */
{
	register char *p, *q, *tdir;
	int pfxmax, x = 0;

	if ((tdir = getenv("TMPDIR")) != NULL)
		if ((x = strlen(tdir)) > 0 && 
				access(tdir, W_ACC | X_ACC) == 0)
			goto OK;
	if ((tdir = dir) != NULL)
		if ((x = strlen(tdir)) > 0 && 
				access(tdir, W_ACC | X_ACC) == 0)
			goto OK;
	if ((tdir = P_tmpdir) != NULL)
		if ((x = strlen(tdir)) > 0 && 
				access(tdir, W_ACC | X_ACC) == 0)
			goto OK;
	if (access(tdir = "/tmp", W_ACC | X_ACC) != 0)
		return(NULL);
	else
		x = 4;	/* strlen("/tmp") */

OK:
	if ((pfxmax = pathconf(tdir, _PC_NAME_MAX)) < 0)
		pfxmax = 14;	/* System V NAME_MAX */
	pfxmax -= (SEEDLEN + MKTLEN); /* max chars to use of pfx argument */
	if ((p = (char *)malloc((size_t)(x+pfxmax+SEEDLEN+MKTLEN+2))) == NULL)
		return(NULL);
	pcopy(p, tdir);

	(void)strcat(p, "/");
	if(pfx) {
#ifdef KJI
		int pfxlen, charlen = NLchrlen(pfx);
		char *pfxptr = pfx;

		/* determine how many bytes of prefix, up to pfxmax, to keep */
		/* (do not truncate prefix mid-character) */
		for (pfxlen=charlen; pfxlen+charlen<=pfxmax; pfxlen+=charlen) {
			pfxptr+=charlen;
			charlen = NLchrlen(pfxptr);
		}

		*(p+strlen(p)+pfxlen) = '\0';
		(void)strncat(p, pfx, pfxlen);
#else
		*(p+strlen(p)+pfxmax) = '\0';
		(void)strncat(p, pfx, (size_t)pfxmax);
#endif
	}
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_tempnam_rmutex);
#endif
	(void)strcat(p, seed);
	(void)strcat(p, "XXXXXX");

	/* update seed */
	q = seed;
	while(*q == 'Z')  {
		*q++ = 'A';
	}
	if(*q != '\0')  {
		++*q;
	}

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_tempnam_rmutex);
#endif

	if(*mktemp(p) == '\0')
		return(NULL);

	return(p);
}

static char*
pcopy(space, arg)
char *space, *arg;
{
	char *p;

	if(arg) {
		(void)strcpy(space, arg);
		p = space-1+strlen(space);
		if(*p == '/')
			*p = '\0';
	}
	return(space);
}
