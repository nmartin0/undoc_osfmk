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
static char rcsid[] = "@(#)$RCSfile: getusershll.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:57 $";
#endif
/*
 * Copyright (c) 1985 Regents of the University of California.
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
 *
 * getusershell.c	5.5 (Berkeley) 7/21/88
 */


#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"
#endif

#define SHELLS "/etc/shells"

/*
 * Do not add local shells here.  They should be added in /etc/shells
 */
static char *okshells[] =
    { "/bin/sh", "/bin/csh", 0 };

static char **shells, *strings;
static char **curshell = NULL;
static char **initshells();

#ifdef _THREAD_SAFE
extern struct rec_mutex _getusershell_rmutex;
#endif

/*
 * Get a list of shells from SHELLS, if it exists.
 */
#ifdef _THREAD_SAFE
int
getusershell_r(char *shell)
#else
char *
getusershell()
#endif
{
	char *ret;

#ifdef _THREAD_SAFE
	if (shell == NULL) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_getusershell_rmutex);
#endif

	if (curshell == NULL)
		curshell = initshells();
	ret = *curshell;
	if (ret != NULL)
		curshell++;
#ifdef _THREAD_SAFE
	if (ret == NULL) {
		seterrno(ESRCH);
		rec_mutex_unlock(&_getusershell_rmutex);
		return(-1);
	} else {
		strcpy(shell, ret);
		rec_mutex_unlock(&_getusershell_rmutex);
		return(0);
	}
#else
	return (ret);
#endif
}

endusershell()
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_getusershell_rmutex);
#endif
	
	if (shells != NULL)
		free((char *)shells);
	shells = NULL;
	if (strings != NULL)
		free(strings);
	strings = NULL;
	curshell = NULL;
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_getusershell_rmutex);
#endif
}

setusershell()
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_getusershell_rmutex);
#endif

	curshell = initshells();

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_getusershell_rmutex);
#endif
}

static char **
initshells()
{
	register char **sp, *cp;
	register FILE *fp;
	struct stat statb;
	extern char *malloc(), *calloc();

	if (shells != NULL)
		free((char *)shells);
	shells = NULL;
	if (strings != NULL)
		free(strings);
	strings = NULL;
	if ((fp = fopen(SHELLS, "r")) == (FILE *)0)
		return(okshells);
	if (fstat(fileno(fp), &statb) == -1) {
		(void)fclose(fp);
		return(okshells);
	}
	if ((strings = malloc((unsigned)statb.st_size)) == NULL) {
		(void)fclose(fp);
		return(okshells);
	}
	shells = (char **)calloc((unsigned)statb.st_size / 3, sizeof (char *));
	if (shells == NULL) {
		(void)fclose(fp);
		free(strings);
		strings = NULL;
		return(okshells);
	}
	sp = shells;
	cp = strings;
	while (fgets(cp, MAXPATHLEN + 1, fp) != NULL) {
		while (*cp != '#' && *cp != '/' && *cp != '\0')
			cp++;
		if (*cp == '#' || *cp == '\0')
			continue;
		*sp++ = cp;
		while (!isspace(*cp) && *cp != '#' && *cp != '\0')
			cp++;
		*cp++ = '\0';
	}
	*sp = (char *)0;
	(void)fclose(fp);
	return (shells);
}
