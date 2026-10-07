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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: getpwent.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:05:51 $";
#endif
/*
 * Copyright (c) 1984 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *getpwent.c	5.2 (Berkeley) 3/9/86";
 */

#include <stdio.h>
#include <pwd.h>
#include <ndbm.h>
#include <sys/types.h>

#ifdef _THREAD_SAFE

#include "rec_mutex.h"
#include <errno.h>

extern struct rec_mutex	_passwd_rmutex;

#define PW_FP                   (*pw_fp)

#else    /* _THREAD_SAFE */

#define MAXLINELENGTH   1024

static char		line[BUFSIZ+1];
static struct passwd	passwd;

#endif   /* _THREAD_SAFE */

static FILE		*pwf = NULL;
static const char	*EMPTY = "";
int			_pw_stayopen;

/*
 * The following are shared with getpwnamuid.c
 */

#ifdef	_THREAD_SAFE
char	*_pw_file_r = "/etc/passwd";
#else
char	*_pw_file = "/etc/passwd";
#endif
DBM	*_pw_db;

static char *
pwskip(p)
register char *p;
{
	while (*p && *p != ':' && *p != '\n')
		++p;
	if (*p)
		*p++ = 0;
	return(p);
}

static int
pwscan(struct passwd *pwent, char *line, int len, FILE *fp)
{
        register char   *p;
        register int    c;

        for (;;) {
                if (!(p = fgets(line, len, fp)))
                        return (0);

                /* line without colon separators is no good, so ignore it */
                if(!strchr(p,':'))
                    continue;

                /* skip lines that are too big */
                if (!index(line, '\n')) {
                        while ((c = getc(fp)) != '\n' && c != EOF)
                                ;
                        continue;

                }
                break;
        }
        pwent->pw_name = p;
        p = pwskip(p);
        pwent->pw_passwd = p;
        p = pwskip(p);
        pwent->pw_uid = (uid_t)strtoul(p, NULL, 10);
        p = pwskip(p);
        pwent->pw_gid = (gid_t)strtoul(p, NULL, 10);
        pwent->pw_quota = 0;
        p = pwskip(p);
        pwent->pw_gecos = p;
        p = pwskip(p);
        pwent->pw_dir = p;
        p = pwskip(p);
        pwent->pw_shell = p;
        while (*p && *p != '\n')
                p++;
        *p = '\0';
        pwent->pw_comment = ++p;
        *p = '\0';
        return (1);
}

#ifdef	_THREAD_SAFE
int
setpwent_r(FILE *f)
{
	FILE    **pw_fp = &f;
        if (pw_fp == 0) {
                seterrno(EINVAL);
                return (-1);
        }

        if (PW_FP) {
                rewind(PW_FP);
                return (0);
	}

	rec_mutex_lock(&_passwd_rmutex);
	PW_FP = fopen(_pw_file_r, "r");
	rec_mutex_unlock(&_passwd_rmutex);
}
#else
void
setpwent(void)
{
        if (pwf == NULL)
                pwf = fopen(_pw_file, "r");
        else
                rewind(pwf);
}
#endif

#ifdef	_THREAD_SAFE
void
endpwent_r(FILE *f)
{
	FILE    **pw_fp = &f;
	if (pw_fp != NULL)
	if (PW_FP != NULL) {
		fclose(PW_FP);
		PW_FP = NULL;
		return;
	}

	rec_mutex_lock(&_passwd_rmutex);
	if (_pw_db != (DBM *)0) {
		dbm_close(_pw_db);
		_pw_db = (DBM *)0;
		_pw_stayopen = 0;
	}

	rec_mutex_unlock(&_passwd_rmutex);
}
#else
void
endpwent(void)
{
        if (pwf != NULL) {
                fclose(pwf);
                pwf = NULL;
        }
        if (_pw_db != (DBM *)0) {
                dbm_close(_pw_db);
                _pw_db = (DBM *)0;
                _pw_stayopen = 0;
        }
}
#endif

#ifdef _THREAD_SAFE
int
getpwent_r(struct passwd *passwd, char *line, int buflen)
#else
struct passwd *
getpwent()
#endif
{
	register char *p;

#ifdef _THREAD_SAFE
	if ((passwd == NULL) || (line == NULL) || (buflen < 1)) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_passwd_rmutex);
#endif
	if (pwf == NULL) {
#ifdef _THREAD_SAFE
		if ((pwf = fopen( _pw_file_r, "r" )) == NULL){
			rec_mutex_unlock(&_passwd_rmutex);
			return(-1);
#else
		if ((pwf = fopen( _pw_file, "r" )) == NULL){
			return(NULL);
#endif
		}
	}
#ifdef _THREAD_SAFE
	p = fgets(line, buflen, pwf);
	if (p == NULL) {
		rec_mutex_unlock(&_passwd_rmutex);
		return(-1);
	}
#else
	p = fgets(line, BUFSIZ, pwf);
	if (p == NULL)
		return(NULL);
#endif
#ifdef _THREAD_SAFE
#define PASSWD(field)	passwd->field
#else
#define PASSWD(field)	passwd.field
#endif
	PASSWD(pw_name) = p;
	p = pwskip(p);
	PASSWD(pw_passwd) = p;
	p = pwskip(p);
	PASSWD(pw_uid) = (uid_t)strtoul(p, NULL, 10);
	p = pwskip(p);
	PASSWD(pw_gid) = (gid_t)atoi(p);
	PASSWD(pw_quota) = 0;
	p = pwskip(p);
	PASSWD(pw_gecos) = p;
	p = pwskip(p);
	PASSWD(pw_dir) = p;
	p = pwskip(p);
	PASSWD(pw_shell) = p;
	while (*p && *p != '\n')
		p++;
	*p = '\0';
	PASSWD(pw_comment) = ++p;
	strcpy(p, EMPTY);
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_passwd_rmutex);
	return(0);
#else
	return(&passwd);
#endif
}

#ifdef _THREAD_SAFE
int
fgetpwent_r(FILE *f, struct passwd *pwent, char *line, int len)
{
        FILE    **pw_fp = &f;	
        if (pwent == 0 || line == 0 || len <= 0) {
                seterrno(EINVAL);
                return -1;
        }
	if (!pwscan(pwent, line, len, PW_FP)) {
                seterrno(ESRCH);
                return -1;
	}
        return(0);
}
#else
struct passwd *
fgetpwent(FILE *pw_fp)
{
        register struct passwd  *pwent = &passwd;

	if (!pwscan(pwent, line, MAXLINELENGTH, pw_fp))
                return 0;
        return (pwent);
}
#endif

#ifdef _THREAD_SAFE
void
setpwfile_r(file)
#else
void
setpwfile(file)
#endif
	const char *file;
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_passwd_rmutex);
	_pw_file_r = (char *) file;
	rec_mutex_unlock(&_passwd_rmutex);
#else
	_pw_file = (char *) file;
#endif
}
