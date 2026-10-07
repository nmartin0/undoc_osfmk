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
/* getlogin.c -- library stub to transform calling sequence
 *
 * user program calls:
 * 	char *name = getlogin();
 *
 * kernel-side interface:
 *	getlogin(char *namebuf, int namelen);
 *
 */

#include <sys/param.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#endif

#define MAXLOGNAME 12		/* for now; too hard to find include files */

extern int _getlogin(char *namebuf, int namelen);

#ifdef _THREAD_SAFE
int
getlogin_r(char *logname, int len)
{
	if ((logname == NULL) || (len == 0)) {
		seterrno(EINVAL);
		return(-1);
	}

	if (_getlogin(logname, len) < 0)
		return (-1);
	else
		return (0);
}
#else
char *
getlogin()
{
	static char logname[MAXLOGNAME];

	if (_getlogin(logname, sizeof(logname)) < 0)
		return (NULL);
	else
		return (logname);
}
#endif	/* _THREAD_SAFE */
