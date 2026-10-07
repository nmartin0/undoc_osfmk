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

/*
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
 */

/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_getaent.c,v 1.4 1995/03/17 22:37:56 davidl Exp $
 *
 * HISTORY
 * $Log: nx_getaent.c,v $
 * Revision 1.4  1995/03/17  22:37:56  davidl
 * Removed printf() introduced as part of fix to PTS 9445; it was causing the
 * MACS EATs to fail.
 *
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 12763
 *  Testing: none
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnx/nx_getaent.c
 *
 * Revision 1.3  1995/03/14  01:06:11  davidl
 * Changed "macadmin" (the only command that modifies /etc/nxaccount) to
 * set /etc/nxaccount's permissions to 644 after each modification.  This
 * avoids problems that would otherwise occur if the user's umask is set
 * restrictively.
 *
 * Changed libnx.a to print a more informative error message if NX
 * accounting permission is denied because /etc/nxaccount can't be
 * opened.
 *
 *  Reviewer: sdh
 *  Risk: Low
 *  Benefit or PTS #: 9445
 *  Testing: Limited developer testing, EATs ("macs" and "rmcall" EATs)
 *  Module(s): cmds_libs/src/usr/ccs/lib/libmacs/agdb.c
 * 	    cmds_libs/src/usr/ccs/lib/libnx/nx_getaent.c
 *
 * Revision 1.2  1994/11/19  02:31:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/01  17:53:41  rkl
 * Initial revision
 *
 */

#include <stdio.h>
#include <nx/nxacct.h>
#include <fcntl.h>
# include <errno.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _nx_acct_rmutex;

/* align an address onto a valid char * value */
#define	ALIGN(a)	(char *)(((unsigned)(a))&~(sizeof(char *) - 1))
#else

#define	MAXACCT	2000
#endif

static char ACCOUNT[] = "/etc/nxaccount";
static FILE *acctf = NULL;

#ifndef _THREAD_SAFE
static char line[BUFSIZ+1];
static struct nxacct nxacct;
static char *acct_mem[MAXACCT];
#endif

static FILE *
open_acctfile()
{
	register int  flags;
	FILE	*fd;

	/* open /etc/nxaccount for close on exec */
	if ((fd = fopen(ACCOUNT, "r")) == NULL) {
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

void
nx_setaent()
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_nx_acct_rmutex);
#endif

	if (!acctf)
		acctf = open_acctfile();
	else
		rewind (acctf);

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_nx_acct_rmutex);
#endif
}

void
nx_endaent()
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_nx_acct_rmutex);
#endif
	if (acctf) {
		fclose(acctf);
		acctf = NULL;
	}

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_nx_acct_rmutex);
#endif
}

static char *
acct_skip(p,c)
register char *p;
register c;
{
	while (*p && (*p != c))
		++p;
	if (*p)
		*p++ = 0;
	return(p);
}

#ifdef _THREAD_SAFE
int
nx_getaent_r(struct nxacct *nxacct, char *line, int len)
{
	register char *p, **q;
	char	**acct_mem;
	char	**end;

	if ((nxacct == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_nx_acct_rmutex);

	if (!acctf)
		if ((acctf = open_acctfile()) == NULL) {
			rec_mutex_unlock(&_nx_acct_rmutex);
			return(-1);
		}

	if ((p = fgets(line, len, acctf)) == NULL) {
		rec_mutex_unlock(&_nx_acct_rmutex);
		seterrno(EIO);
		return(-1);
	}
	acct_mem = (char **)ALIGN(p + strlen(p) + sizeof(char *) - 1);
	end = (char **)ALIGN(p + len) - sizeof(char *);
	nxacct->acct_name = p;
	nxacct->acct_passwd = p = acct_skip(p,':');
	nxacct->acct_id = atoi(p = acct_skip(p,':'));
	nxacct->acct_mem = acct_mem;
	p = acct_skip(p,':');
	acct_skip(p,'\n');
	q = acct_mem;
	while (*p) {
		if (q < end)
			*q++ = p;
		p = acct_skip(p,',');
	}
	*q = NULL;
	rec_mutex_unlock(&_nx_acct_rmutex);
	return(0);
}

#else /* _THREAD_SAFE */

struct nxacct *
nx_getaent()
{
	register char *p, **q;

	if (!acctf)
		if ((acctf = open_acctfile()) == NULL)
			return((struct nxacct *)NULL);

	if (!(p = fgets(line, BUFSIZ, acctf)))
		return(NULL);
	nxacct.acct_name = p;
	nxacct.acct_passwd = p = acct_skip(p,':');
	nxacct.acct_id = atoi(p = acct_skip(p,':'));
	nxacct.acct_mem = acct_mem;
	p = acct_skip(p,':');
	acct_skip(p,'\n');
	q = acct_mem;
	while (*p) {
		if (q < &acct_mem[MAXACCT-1])
			*q++ = p;
		p = acct_skip(p,',');
	}
	*q = NULL;
	return(&nxacct);
}
#endif  /* _THREAD_SAFE */

#ifdef  _THREAD_SAFE 
int 
nx_getagid_r(acct_t acct_id, struct nxacct *nxacct, char *line, int len)
{
	if ((nxacct == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_nx_acct_rmutex);
	nx_setaent();
	while (nx_getaent_r(nxacct, line, len) == 0)
	       if (nxacct->acct_id == acct_id) {
	            endgrent();
		    rec_mutex_unlock(&_nx_acct_rmutex);
		    return(0);
               }
	nx_endaent();
	rec_mutex_unlock(&_nx_acct_rmutex);
	seterrno(ENOENT);
	return(-1);
}
#else	/* _THREAD_SAFE */
struct nxacct *
nx_getaid(register acct_t acct_id)
{
	register struct nxacct *p;

	nx_setaent();
	while ((p = nx_getaent()) && p->acct_id != acct_id)
		;
	nx_endaent();
	return(p);
}
#endif	/* _THREAD_SAFE */

#ifdef _THREAD_SAFE 
int
nx_getanam_r(char *name, struct nxacct *nxacct, char *line, int len)
{
	if ((name == NULL) || (nxacct == NULL) || (line == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}
	rec_mutex_lock(&_nx_acct_rmutex);
	nx_setaent();
	while (nx_getaent_r(nxacct, line, len) == 0)
	    if (!strcmp(nxacct->acct_name, name)) {
	        nx_endaent();
	        rec_mutex_unlock(&_nx_acct_rmutex);
        	return(0);
            }		  
	nx_endaent();
        rec_mutex_unlock(&_nx_acct_rmutex);
	seterrno(ENOENT);
	return(-1);
}
#else /* _THREAD_SAFE   */

struct nxacct *
nx_getanam(char *name)
{
	register struct nxacct *p;

	nx_setaent();
	while ((p = nx_getaent()) && strcmp(p->acct_name, name))
		;
	nx_endaent();
	return(p);
}
#endif /* _THREAD_SAFE  */
