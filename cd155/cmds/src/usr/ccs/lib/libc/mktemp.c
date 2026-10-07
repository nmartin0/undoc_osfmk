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
static char rcsid[] = "@(#)$RCSfile: mktemp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:13 $";
#endif
/*
 * FUNCTIONS: mktemp
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
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * static char sccsid[] = "@(#)mktemp.c	1.9  com/lib/c/gen,3.1,9013 2/26/90 16:21:20";
 *
 */

#include <stdio.h>		/* for NULL		*/
#include <sys/access.h>		/* for access(,F_OK)...	*/
#include <sys/types.h>
#ifdef _THREAD_SAFE
#include "lib_lock.h"

extern lib_lock_functions_t    libc_lock_funcs;
#endif

#define TMPSIZE	6

#ifdef KJI
#include <sys/NLchar.h>
#endif

/*
 * NAME:	mktemp
 *
 * FUNCTION:	mktemp - construct a unique filename
 *
 * NOTES:	Mktemp expects a string of length at least 6, with
 *		six trailing 'X's, and overwrites the X's with a
 *		(hopefully) unique encoding of the process' pid and
 *		a pseudo-random number.
 *		If this method yields an existing file name, your string
 *		is shortened to a slash.
 *
 * RETURN VALUE DESCRIPTION:
 *		The filename is returned.
 */
char *
#ifdef _NO_PROTO
mktemp(template)
char *template;
#else
mktemp(char *template)		
#endif
{
	extern char * rmktemp();
	char *rv;
	char *save;
	save = (char *) malloc(strlen(template) + 1);
	strcpy(save, template);
	rv = rmktemp(template);
	while (rv ==  NULL)  {
		strcpy(template,save);
		rv = rmktemp(template);
	}
	free(save);
	return (template);
}


extern int strlen(), access(), getpid();

char *
rmktemp(template)
char *template;
{
#ifdef KJI
	char *sp;		/* current data point in s */
	char *xptr = NULL;	/* pointer to first 'X' in string */
#endif
	char *pt, *s = template;
	pid_t pid;
	char t[TMPSIZE + 1];	/* hold a unique number	*/
	int i;

#ifdef _THREAD_SAFE
	/*
	 * pid is not unique, use thread id from the threads provider
	 * we cannot use the kernel thread id as this may be a coroutine
	 * threads package or we may be have many threads multiplexed on
	 * fewer kernel threads.
	 */
	pid = (pid_t)lib_thread_id(libc_lock_funcs);
#else
	pid = getpid();
#endif

	for(i = 1; i <= TMPSIZE; i++)  {
		t[TMPSIZE - i] = (pid % 10) + '0';
		pid /= 10;
	}

	t[TMPSIZE] = '\0';

	pt = t;

#ifdef KJI
	s += strlen(template);	/* point at the terminal null */
	pt += strlen(pt);

	/* search forward for the first of the trailing 'X's in s */
	for(sp = template; *sp != '\0'; sp+=NLchrlen(sp))
		if(NCdechr(sp) == 'X') {
			if(xptr == NULL) {
				xptr = sp;
				break;
			}
		} else
			xptr = NULL;
	if(xptr == NULL)
		xptr = s;

	while(--s >= xptr)
#else
	s += strlen(template);	/* point at the terminal null */
	pt += strlen(pt);
	while((*--s == 'X') && (pt > t))
#endif
		*s = *--pt;

        s++;
        i = 'a';

	while(access(template, F_OK) != -1) {
                if (i=='z')
                        return("/");
                *s = i++;
	}

	return(s);
}
