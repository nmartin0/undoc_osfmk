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
static char rcsid[] = "@(#)$RCSfile: getgrent.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:05:32 $";
#endif

/*
 * FUNCTIONS: setgrent, endgrent, getgrent, getgrgid, getgrnam
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
 * "@(#)getgrent.c	5.2 (Berkeley) 3/9/86"
 * "@(#)getgrgid.c	5.2 (Berkeley) 3/9/86"
 * "@(#)getgrnam.c	5.2 (Berkeley) 3/9/86"
 *
 * static char sccsid[] = "@(#)getgrent.c	1.16  com/lib/c/adm,3.1,9013 2/20/90 17:10:48";
 */

#include <stdio.h>
#include <grp.h>
#include <fcntl.h>
# include <errno.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _group_rmutex;

#define GR_FP           (*gr_fp)
#define GRSCAN(g, l)    grscan(g, l, len, GR_FP)

/* align an address onto a valid char * value */
#define	ALIGN(a)	(char *)(((unsigned)(a))&~(sizeof(char *) - 1))
#else

#define MAXLINELENGTH   1024
#define	MAXGRP	2000

static char line[BUFSIZ+1];
static struct group group;
static char *gr_mem[MAXGRP];
#endif

static FILE *grf = NULL;
static char GROUP[] = "/etc/group";

static char *
grskip(p,c)
register char *p;
register c;
{
	while (*p && (*p != c))
		++p;
	if (*p)
		*p++ = 0;
	return(p);
}

static int
grscan(struct group *grent, char *line, int len, FILE *fp)
{
        register char   *p, **q;
        char            **end;
        register int    c;
#ifdef _THREAD_SAFE
        char            **gr_mem;
#endif  /* _THREAD_SAFE */

        for (;;) {
                if (!(p = fgets(line, len, fp)))
                        return (0);
                /* skip lines that are too big */
                if (!index(line, '\n')) {
                        while ((c = getc(fp)) != '\n' && c != EOF)
                                ;
                        continue;
                }
                break;
        }

#ifdef _THREAD_SAFE
        gr_mem = (char **)ALIGN(p + strlen(p) + sizeof(char *) - 1);
        end = (char **)ALIGN(p + len) - sizeof(char *);
#else
        end = &gr_mem[MAXGRP - 1];
#endif  /* _THREAD_SAFE */

        grent->gr_name = p;
        grent->gr_passwd = p = grskip(p, ':');
        grent->gr_gid = strtoul(p = grskip(p, ':'), NULL, 10);
        grent->gr_mem = gr_mem;
        p = grskip(p, ':');
        grskip(p,'\n');
        q = gr_mem;
        while (*p) {
                if (q < end)
                        *q++ = p;
                p = grskip(p, ',');
        }
        *q = NULL;
        return (1);
}

#ifdef _THREAD_SAFE
int
fgetgrent_r(FILE *fp, struct group *grent, char *line, int len)
{
        FILE    **gr_fp = &fp;
	if (grent == 0 || line == 0 || len <= 0) {
		seterrno(EINVAL);
		return -1;
	}
        if (!GRSCAN(grent, line)) {
		seterrno(ESRCH);
                return -1;
        }
	return(0);
}
#else
struct group *
fgetgrent(FILE *gr_fp)
{
        register struct group   *grent = &group;
        if (!grscan(grent, line, MAXLINELENGTH, gr_fp)) {
		return 0;
        }
        return (grent);
}
#endif /* _THREAD_SAFE */

static FILE *
open_groupfile()
{
	register int  flags;
	FILE	*fd;

	/* open /etc/group for close on exec */
	if ((fd = fopen(GROUP, "r")) == NULL) {
#ifdef _THREAD_SAFE
		seterrno(ENOENT);
#else
		errno = ENOENT;
#endif
		return(NULL);
	}

	flags = fcntl(fileno(fd), F_GETFD, 0);
	flags |= FD_CLOEXEC;
	if (fcntl(fileno(fd), F_SETFD, flags) < 0) {
#ifdef _THREAD_SAFE
		seterrno(EIO);
#else
		errno = EIO;
#endif
		return(NULL);
	}
	return(fd);
}

#ifdef _THREAD_SAFE
int
setgrent_r(FILE **gr_fp)
{
	if (gr_fp == 0) {
		seterrno(EINVAL);
		return -1;
	}
        if (GR_FP) {
                rewind(GR_FP);
                return (0);
        }
	rec_mutex_lock(&_group_rmutex);
	GR_FP = open_groupfile();

	rec_mutex_unlock(&_group_rmutex);
}
#else
void
setgrent()
{
	if (!grf)
		grf = open_groupfile();
        else
                rewind (grf);
}
#endif

#ifdef _THREAD_SAFE
void
endgrent_r(FILE **gr_fp)
{
	rec_mutex_lock(&_group_rmutex);
	if (gr_fp == 0) return;
	if (GR_FP) {
		fclose(GR_FP);
		GR_FP = NULL;
	}

	rec_mutex_unlock(&_group_rmutex);
}
#else
void
endgrent()
{
	if (grf) {
		fclose(grf);
		grf = NULL;
	}
}
#endif


#ifdef _THREAD_SAFE
int
getgrent_r(struct group *group, char *line, int len)
{
	register char *p, **q;
	char	**gr_mem;
	char	**end;

	if ((group == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_group_rmutex);

	if (!grf)
		if ((grf = open_groupfile()) == NULL) {
			rec_mutex_unlock(&_group_rmutex);
			return(-1);
		}

	if ((p = fgets(line, len, grf)) == NULL) {
		rec_mutex_unlock(&_group_rmutex);
		seterrno(EIO);
		return(-1);
	}
	gr_mem = (char **)ALIGN(p + strlen(p) + sizeof(char *) - 1);
	end = (char **)ALIGN(p + len) - sizeof(char *);
	group->gr_name = p;
	group->gr_passwd = p = grskip(p,':');
	group->gr_gid = atoi(p = grskip(p,':'));
	group->gr_mem = gr_mem;
	p = grskip(p,':');
	grskip(p,'\n');
	q = gr_mem;
	while (*p) {
		if (q < end)
			*q++ = p;
		p = grskip(p,',');
	}
	*q = NULL;
	rec_mutex_unlock(&_group_rmutex);
	return(0);
}

#else /* _THREAD_SAFE */

struct group *
getgrent()
{
	register char *p, **q;

	if (!grf)
		if ((grf = open_groupfile()) == NULL)
			return((struct group *)NULL);

	if (!(p = fgets(line, BUFSIZ, grf)))
		return(NULL);
	group.gr_name = p;
	group.gr_passwd = p = grskip(p,':');
	group.gr_gid = atoi(p = grskip(p,':'));
	group.gr_mem = gr_mem;
	p = grskip(p,':');
	grskip(p,'\n');
	q = gr_mem;
	while (*p) {
		if (q < &gr_mem[MAXGRP-1])
			*q++ = p;
		p = grskip(p,',');
	}
	*q = NULL;
	return(&group);
}
#endif  /* _THREAD_SAFE */

#ifdef  _THREAD_SAFE 
int 
getgrgid_r(gid_t gid, struct group *group, char *line, int len)
{
	if ((group == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_group_rmutex);
	setgrent();
	while (getgrent_r(group, line, len) == 0)
	       if (group->gr_gid == gid) {
	            endgrent();
		    rec_mutex_unlock(&_group_rmutex);
		    return(0);
               }
	endgrent();
	rec_mutex_unlock(&_group_rmutex);
	seterrno(ENOENT);
	return(-1);
}
#else	/* _THREAD_SAFE */
struct group *
getgrgid(register gid_t gid)
{
	register struct group *p;

	setgrent();
	while ((p = getgrent()) && p->gr_gid != gid)
		;
	endgrent();
	return(p);
}
#endif	/* _THREAD_SAFE */

#ifdef _THREAD_SAFE 
int
getgrnam_r(char *name, struct group *group, char *line, int len)
{
	if ((name == NULL) || (group == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}
	rec_mutex_lock(&_group_rmutex);
	setgrent();
	while (getgrent_r(group, line, len) == 0)
	    if (!strcmp(group->gr_name, name)) {
	        endgrent();
	        rec_mutex_unlock(&_group_rmutex);
        	return(0);
            }		  
	endgrent();
        rec_mutex_unlock(&_group_rmutex);
	seterrno(ENOENT);
	return(-1);
}
#else /* _THREAD_SAFE   */

struct group *
getgrnam(char *name)
{
	register struct group *p;

	setgrent();
	while ((p = getgrent()) && strcmp(p->gr_name, name))
		;
	endgrent();
	return(p);
}
#endif /* _THREAD_SAFE  */
