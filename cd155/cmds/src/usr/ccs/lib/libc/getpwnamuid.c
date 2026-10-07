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
static char rcsid[] = "@(#)$RCSfile: getpwnamuid.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:05:52 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *#if defined(LIBC_SCCS) && !defined(lint)
 *static char sccsid[] = "@(#)getpwnamuid.c	5.3 (Berkeley) 12/21/87";
 *#endif LIBC_SCCS and not lint
 */

#include <stdio.h>
#include <pwd.h>
#include <ndbm.h>

#include <sys/file.h>
#include <sys/fcntl.h>

#ifdef _THREAD_SAFE
#include "rec_mutex.h"
# include <errno.h>

extern struct rec_mutex	_passwd_rmutex;
#else
static char line[BUFSIZ+1];
static struct passwd passwd;
#endif

/*
 * The following are shared with getpwent.c
 */

#ifdef _THREAD_SAFE
extern	char *_pw_file_r;
#define SETPWENT()              setpwent_r(pw_fp)
#define ENDPWENT()              endpwent_r(pw_fp)
#else
#define SETPWENT()              setpwent()
#define ENDPWENT()              endpwent()
extern	char *_pw_file;
#endif
DBM	*_pw_db;
int	_pw_stayopen;

#ifdef _THREAD_SAFE
#define	FAIL	-1
#else
#define FAIL	((struct passwd *)NULL)
#endif

#ifdef _THREAD_SAFE
static int
fetchpw(datum key, struct passwd *passwd, char *line, int len)
#else
static struct passwd *
fetchpw(datum key)
#endif
{
	register char *cp, *tp;
	int i;

#ifdef _THREAD_SAFE
	if ((passwd == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(FAIL);
	}
	rec_mutex_lock(&_passwd_rmutex);
#endif

	if (key.dptr == 0) {
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_passwd_rmutex);
		seterrno(EINVAL);
#endif
		return(FAIL);
	}
	key = dbm_fetch(_pw_db, key);
	if (key.dptr == 0) {
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_passwd_rmutex);
		seterrno(EINVAL);
#endif
		return(FAIL);
	}

	cp = key.dptr;
	tp = line;

#ifdef _THREAD_SAFE
#define PASSWD(field)	passwd->field
#else
#define PASSWD(field)	passwd.field
#endif

#define	EXPAND(e)	PASSWD(e) = tp; while (*tp++ = *cp++);

	EXPAND(pw_name);
	EXPAND(pw_passwd);

	bcopy(cp, (char *)&i, sizeof(i));
	cp += sizeof(i);
	PASSWD(pw_uid) = i;
	bcopy(cp, (char *)&i, sizeof(i));
	cp += sizeof(i);
	PASSWD(pw_gid) = i;
	bcopy(cp, (char *)&i, sizeof(i));
	cp += sizeof(i);
	PASSWD(pw_quota) = i;

	EXPAND(pw_comment);
	EXPAND(pw_gecos);
	EXPAND(pw_dir);
	EXPAND(pw_shell);
#ifdef _THREAD_SAFE
	return(0);
#else
	return (&passwd);
#endif
}

/*
* Thread-safe getpwnam
*/
#ifdef _THREAD_SAFE
int 
getpwnam_r(char *nam, struct passwd *pw, char *line, int linelen)
{
	FILE    *pw_fp = 0;
#else 
struct passwd *
getpwnam(char *nam)
{
#endif
	datum key;
#ifdef _THREAD_SAFE
	int	retval;
#else
	register struct passwd *pw;
#endif

#ifdef _THREAD_SAFE
	if ((pw == NULL) || (line == NULL) || (linelen < 1)) {
		seterrno(EINVAL);
		return(-1);
	}
	rec_mutex_lock(&_passwd_rmutex);
#endif

	if (_pw_db == (DBM *)0 &&
#ifdef 	_THREAD_SAFE
	    (_pw_db = dbm_open(_pw_file_r, O_RDONLY, 0)) == (DBM *)0) {
#else
	    (_pw_db = dbm_open(_pw_file, O_RDONLY, 0)) == (DBM *)0) {
#endif
oldcode:
		SETPWENT();
#ifdef 	_THREAD_SAFE
		while (getpwent_r(pw, line, linelen) == 0)
		       if (strcmp(nam, pw->pw_name) == 0) {
				if (!_pw_stayopen)
					ENDPWENT();
				rec_mutex_unlock(&_passwd_rmutex);
				return(0);
			}
		if (!_pw_stayopen)
			ENDPWENT();
		rec_mutex_unlock(&_passwd_rmutex);
		seterrno(ENOENT);
		return(-1);
#else
		while ((pw = getpwent()) && strcmp(nam, pw->pw_name))
			;
		if (!_pw_stayopen)
			ENDPWENT();
		return (pw);
#endif /* _THREAD_SAFE */
	}
	if (flock(dbm_dirfno(_pw_db), LOCK_SH) < 0) {
		dbm_close(_pw_db);
		_pw_db = (DBM *)0;
		goto oldcode;
	}
	key.dptr = nam;
	key.dsize = strlen(nam);
#ifdef _THREAD_SAFE
	retval = fetchpw(key, pw, line, linelen);
#else
	pw = fetchpw(key);
#endif
	(void) flock(dbm_dirfno(_pw_db), LOCK_UN);
	if (!_pw_stayopen) {
		dbm_close(_pw_db);
		_pw_db = (DBM *)0;
	}
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_passwd_rmutex);
	return(retval);
#else	
	return (pw);
#endif /* _THREAD_SAFE */
}

#ifdef _THREAD_SAFE
int
getpwuid_r(uid_t uid, struct passwd *pw, char *line, int linelen)
{
        FILE    *pw_fp = 0;
#else
struct passwd *
getpwuid(uid_t uid)
{
#endif
	datum key;
#ifdef _THREAD_SAFE
	int	retval;
#else
	register struct passwd *pw;
#endif /* _THREAD_SAFE */

#ifdef _THREAD_SAFE 
	if ((pw == NULL) || (line == NULL) || (linelen < 1)) {
		seterrno(EINVAL);
		return(-1);
	}
	rec_mutex_lock(&_passwd_rmutex);
#endif

	if (_pw_db == (DBM *)0 &&
#ifdef _THREAD_SAFE 
	    (_pw_db = dbm_open(_pw_file_r, O_RDONLY, 0)) == (DBM *)0) {
#else
	    (_pw_db = dbm_open(_pw_file, O_RDONLY, 0)) == (DBM *)0) {
#endif
oldcode:
		SETPWENT();
#ifdef _THREAD_SAFE 
		while (getpwent_r(pw, line, linelen) == 0)
			if (pw->pw_uid == uid) {
				if (!_pw_stayopen)
					ENDPWENT();
				rec_mutex_unlock(&_passwd_rmutex);
				return(0);
			}
		if (!_pw_stayopen)
			ENDPWENT();
		rec_mutex_unlock(&_passwd_rmutex);
		seterrno(ENOENT);
		return(-1);
#else
		while ((pw = getpwent()) && pw->pw_uid != uid)
			;
		if (!_pw_stayopen)
			ENDPWENT();
		return (pw);
#endif /* _THREAD_SAFE  */
	}
	if (flock(dbm_dirfno(_pw_db), LOCK_SH) < 0) {
		dbm_close(_pw_db);
		_pw_db = (DBM *)0;
		goto oldcode;
	}
	key.dptr = (char *) &uid;
	key.dsize = sizeof uid;
#ifdef _THREAD_SAFE
	retval = fetchpw(key, pw, line, linelen);
#else /* _THREAD_SAFE */
	pw = fetchpw(key);
#endif /* _THREAD_SAFE */
	(void) flock(dbm_dirfno(_pw_db), LOCK_UN);
	if (!_pw_stayopen) {
		dbm_close(_pw_db);
		_pw_db = (DBM *)0;
	}
#ifdef _THREAD_SAFE 
	rec_mutex_unlock(&_passwd_rmutex);
	return(retval);
#else
	return (pw);
#endif
}
