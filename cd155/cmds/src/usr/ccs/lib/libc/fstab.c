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
static char rcsid[] = "@(#)$RCSfile: fstab.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 02:05:14 $";
#endif
/*
 * Copyright (c) 1980, 1988 Regents of the University of California.
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
 * fstab.c	5.8 (Berkeley) 8/21/89
 */


#include <fstab.h>
#include <stdio.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex _fstab_rmutex;
#endif

static FILE *_fs_fp;
#ifndef _THREAD_SAFE
static struct fstab _fs_fstab;
#endif

static
#ifdef _THREAD_SAFE
fstabscan_r(struct fstab *fsent, char *line)
#else
fstabscan()
#endif
{
	char *fgets(), *strtok();
	register char *cp;
	int typexx;
#define	MAXLINELENGTH	400
	char subline[MAXLINELENGTH];
#ifdef _THREAD_SAFE
	char *tokp;
        char *strtok_r();
#define STRTOK(s1, s2)  strtok_r(s1, s2, &tokp)
#else
	static char line[MAXLINELENGTH];
#define STRTOK(s1, s2)  strtok(s1, s2)
	struct fstab	*fsent = &_fs_fstab;
#endif

	for (;;) {
		if (!(cp = fgets(line, MAXLINELENGTH, _fs_fp)))
			return(0);
		fsent->fs_spec = STRTOK(cp, " \t\n");
		/* Dump commented lines */
		if ( fsent->fs_spec == NULL || fsent->fs_spec[0] == '#' )
			continue;
		fsent->fs_file = STRTOK((char *)NULL, " \t\n");
		fsent->fs_vfstype = STRTOK((char *)NULL, " \t\n");
		fsent->fs_mntops = STRTOK((char *)NULL, " \t\n");
		if (fsent->fs_mntops == NULL)
			goto bad;
		fsent->fs_freq = 0;
		fsent->fs_passno = 0;
		if ((cp = STRTOK((char *)NULL, " \t\n")) != NULL) {
			fsent->fs_freq = atoi(cp);
			if ((cp = STRTOK((char *)NULL, " \t\n")) != NULL)
				fsent->fs_passno = atoi(cp);
		}
		strcpy(subline, fsent->fs_mntops);
		for (typexx = 0, cp = STRTOK(subline, ","); cp;
		     cp = STRTOK((char *)NULL, ",")) {
			if (strlen(cp) != 2)
				continue;
			if (!strcmp(cp, FSTAB_RW)) {
				fsent->fs_type = FSTAB_RW;
				break;
			}
			if (!strcmp(cp, FSTAB_RQ)) {
				fsent->fs_type = FSTAB_RQ;
				break;
			}
			if (!strcmp(cp, FSTAB_RO)) {
				fsent->fs_type = FSTAB_RO;
				break;
			}
			if (!strcmp(cp, FSTAB_SW)) {
				fsent->fs_type = FSTAB_SW;
				break;
			}
			if (!strcmp(cp, FSTAB_XX)) {
				fsent->fs_type = FSTAB_XX;
				typexx++;
				break;
			}
		}
		if (typexx)
			continue;
		if (cp != NULL)
			return(1);
	bad:
		/* no way to distinguish between EOF and syntax error */
		(void)write(fileno(stderr) , "fstab: ", 7);
		(void)write(fileno(stderr) , _PATH_FSTAB,
		    sizeof(_PATH_FSTAB) - 1);
		(void)write(fileno(stderr) , ": syntax error.\n", 16);
	}
	/* NOTREACHED */
}

#ifdef _THREAD_SAFE
int
getfsent_r(struct fstab *fsent, char *buf)
#else
struct fstab *
getfsent()
#endif
{
#ifdef _THREAD_SAFE
	if (!_fs_fp && !setfsent() || !fstabscan_r(fsent, buf)) {
		seterrno(ENOENT);
		return(-1);
	}
	return(0);
#else
	if (!_fs_fp && !setfsent() || !fstabscan())
		return((struct fstab *)NULL);
	return(&_fs_fstab);
#endif
}

#ifdef _THREAD_SAFE
int
getfsspec_r(const char *name, struct fstab *fsent, char *buf)
#else
struct fstab *
getfsspec(name)
register const char *name;
#endif
{
	if (setfsent())
#ifdef _THREAD_SAFE
		while(fstabscan_r(fsent, buf))
			if(!strcmp(fsent->fs_spec, name))
				return(0);
	seterrno(ENOENT);
	return(-1);
#else
		while (fstabscan())
			if (!strcmp(_fs_fstab.fs_spec, name))
				return(&_fs_fstab);
	return((struct fstab *)NULL);
#endif
}

#ifdef _THREAD_SAFE
int
getfsfile_r(const char *name, struct fstab *fsent, char *buf)
#else
struct fstab *
getfsfile(name)
register const char *name;
#endif
{
	if (setfsent())
#ifdef _THREAD_SAFE
		while(fstabscan_r(fsent, buf))
			if(!strcmp(fsent->fs_file, name))
				return(0);
	seterrno(ENOENT);
	return(-1);

#else
		while (fstabscan())
			if (!strcmp(_fs_fstab.fs_file, name))
				return(&_fs_fstab);
	return((struct fstab *)NULL);
#endif
}

setfsent()
{
#ifdef _THREAD_SAFE
	int	rc;

	rec_mutex_lock(&_fstab_rmutex);
#endif

	if (_fs_fp) {
		rewind(_fs_fp);
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_fstab_rmutex);
#endif
		return(1);
	}
#ifdef _THREAD_SAFE
	rc = ((_fs_fp = fopen(_PATH_FSTAB, "r")) != NULL);
	rec_mutex_unlock(&_fstab_rmutex);
	return(rc);
#else
	return((_fs_fp = fopen(_PATH_FSTAB, "r")) != NULL);
#endif
}

endfsent()
{
#ifdef _THREAD_SAFE
	int	rc;

	rec_mutex_lock(&_fstab_rmutex);
#endif

	if (_fs_fp) {
		(void)fclose(_fs_fp);
		_fs_fp = NULL;
	}
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_fstab_rmutex);
#endif
}
