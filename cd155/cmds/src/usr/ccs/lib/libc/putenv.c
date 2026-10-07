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
static char rcsid[] = "@(#)$RCSfile: putenv.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:26 $";
#endif
/*
 * FUNCTIONS: putenv, clearenv
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
 */

/*-
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>              /* for NULL     */
#include <memory.h>             /* for memcpy   */
#include <string.h>

#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _environ_rmutex;
#endif

extern char **environ;

extern char *strdup();
static int match(char *s1, char *s2);
static int find(char *str);

/*
 * NAME:        putenv
 *
 * FUNCTION:    putenv - change environment variables
 *
 * NOTES:       This routine changes or adds values to the environment.
 *              The argument - char *change = a pointer to a string of
 *              the form "name=value"
 *
 * DATA STRUCTURES:     'Environ' gets modified.
 *
 * RETURN VALUE DESCRIPTION:    output - 0, if successful, otherwise -1
 */

extern char **environ;          /* pointer to environment */
static reall = 0;               /* flag to reallocate space, if putenv is
                                   called more than once */

int
putenv(change)
char *change;                   /* "name=value" to add to environment */
{
        char **newenv;              /* points to new environment */
        int which;                  /* index of variable to replace */

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_environ_rmutex);
#endif

        if ((which = find(change)) < 0)  {
                /* if a new variable */
                /* which is negative of table size, so invert and
                   count new element */
                which = (-which) + 1;
                if (reall)  {
                        /* we have expanded environ before */
                        newenv = (char **)realloc((char *) environ,
                                  which*sizeof(char *));
                        if (newenv == NULL) {
#ifdef _THREAD_SAFE
				rec_mutex_unlock(&_environ_rmutex);
#endif
				return(-1);
			}
                        /* now that we have space, change environ */
                        environ = newenv;
                } else {
                        /* environ points to the original space */
                        reall++;
                        newenv = (char **)malloc(which*sizeof(char *));
                        if (newenv == NULL) {
#ifdef _THREAD_SAFE
				rec_mutex_unlock(&_environ_rmutex);
#endif
				return(-1);
			}
                        memcpy((void *)newenv, (void *)environ,
                                (size_t)(which*sizeof(char *)));
                        environ = newenv;
                }
                environ[which-2] = change;
                environ[which-1] = NULL;
        }  else  {
                /* we are replacing an old variable */
                environ[which] = change;
        }
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_environ_rmutex);
#endif
        return(0);
}

/*
 * NAME:        find
 *
 * FUNCTION:    find - find where 'str' is in environ
 *
 * NOTES:       Find looks thru the environment for the string
 *              matching 'str'.  'Str' is of the form "name=value".
 *
 * RETURN VALUE DESCRIPTION:    returns index of the pointer that
 *              matches.  if no match was found, the size of the
 *              table * -1 is returned.
 */

static int
find(char *str)
{
        int ct = 0;     /* index into environ */

        while(environ[ct] != NULL)   {
                if (match(environ[ct], str)  != 0)
                        return(ct);
                ct++;
        }
        return(-(++ct));
}

/*
 * NAME:        match
 *
 * FUNCTION:    Match compares a string 's1', which is of the
 *              form "name" or "name=value" and compares it
 *              to 's2', which is of the form "name=value".
 *
 * RETURN VALUE DESCRIPTION:    1 if the names match, else 0
 */

static int
match( char *s1, char *s2)
{
        while(*s1 == *s2++)  {
                if (*s1 == '=')
                        return(1);
                s1++;
        }
        return(0);
}


/*
 * NAME:	clearenv
 *                                                                    
 * FUNCTION:	clearenv - clear environment
 *                                                                    
 * NOTES:	This routine erases the environment only by NULLing
 *		environ. Memory management is not taken into account.
 *
 * DATA STRUCTURES:	'Environ' gets modified.
 *
 * RETURN VALUE DESCRIPTION:	output - 0, if successful, otherwise -1
 */  

int
clearenv()
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_environ_rmutex);
#endif

	environ = NULL;

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_environ_rmutex);
#endif
	return(0);
}
