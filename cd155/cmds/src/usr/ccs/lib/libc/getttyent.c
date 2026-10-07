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
static char rcsid[] = "@(#)$RCSfile: getttyent.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:56 $";
#endif
/*
 * Copyright (c) 1985 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * getttyent.c	5.4 (Berkeley) 5/19/86
 */


#include <stdio.h>
#include <strings.h>
#include <ttyent.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex _getttyent_rmutex;
#endif

static char TTYFILE[] = "/etc/ttys";
static char zapchar;
static FILE *tf = NULL;
#define LINE 256
#ifndef _THREAD_SAFE
static char line[LINE];
static struct ttyent tty;
#endif

void
setttyent(void)
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_getttyent_rmutex);
#endif

	if (tf == NULL)
		tf = fopen(TTYFILE, "r");
	else
		rewind(tf);

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_getttyent_rmutex);
#endif
}

void
endttyent(void)
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_getttyent_rmutex);
#endif

	if (tf != NULL) {
		(void) fclose(tf);
		tf = NULL;
	}

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_getttyent_rmutex);
#endif
}

#define QUOTED	1

/*
 * Skip over the current field, removing quotes,
 * and return a pointer to the next field.
 */
static char *
skip(p)
	register char *p;
{
	register char *t = p;
	register int c;
	register int q = 0;

	for (; (c = *p) != '\0'; p++) {
		if (c == '"') {
			q ^= QUOTED;	/* obscure, but nice */
			continue;
		}
		if (q == QUOTED && *p == '\\' && *(p+1) == '"')
			p++;
		*t++ = *p;
		if (q == QUOTED)
			continue;
		if (c == '#') {
			zapchar = c;
			*p = 0;
			break;
		}
		if (c == '\t' || c == ' ' || c == '\n') {
			zapchar = c;
			*p++ = 0;
			while ((c = *p) == '\t' || c == ' ' || c == '\n')
				p++;
			break;
		}
	}
	*--t = '\0';
	return (p);
}

static char *
value(p)
	register char *p;
{
	if ((p = index(p,'=')) == 0)
		return(NULL);
	p++;			/* get past the = sign */
	return(p);
}

#ifdef _THREAD_SAFE
int
getttyent_r(struct ttyent *tte, char *line)
#else
struct ttyent *
getttyent(void)
#endif
{
	register char *p;
	register int c;
#ifndef _THREAD_SAFE
	struct ttyent  *tte;

	tte = &tty;
#endif

#ifdef _THREAD_SAFE
	if (tte == NULL) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_getttyent_rmutex);
#endif

	if (tf == NULL) {
		if ((tf = fopen(TTYFILE, "r")) == NULL) {
#ifdef _THREAD_SAFE
			rec_mutex_unlock(&_getttyent_rmutex);
			seterrno(ENOENT);
			return(-1);
#else
			return (NULL);
#endif
		}
	}
	do {
		p = fgets(line, LINE, tf);
		if (p == NULL) {
#ifdef _THREAD_SAFE
			rec_mutex_unlock(&_getttyent_rmutex);
			seterrno(ENXIO);
			return(-1);
#else
			return (NULL);
#endif
		}
		while ((c = *p) == '\t' || c == ' ' || c == '\n')
			p++;
	} while (c == '\0' || c == '#');
	zapchar = 0;
	tte->ty_name = p;
	p = skip(p);
	tte->ty_getty = p;
	p = skip(p);
	tte->ty_type = p;
	p = skip(p);
	tte->ty_status = 0;
	tte->ty_window = NULL;
	for (; *p; p = skip(p)) {
#define space(x) ((c = p[x]) == ' ' || c == '\t' || c == '\n')
		if (strncmp(p, "on", 2) == 0 && space(2))
			tte->ty_status |= TTY_ON;
		else if (strncmp(p, "off", 3) == 0 && space(3))
			tte->ty_status &= ~TTY_ON;
		else if (strncmp(p, "secure", 6) == 0 && space(6))
			tte->ty_status |= TTY_SECURE;
		else if (strncmp(p, "window=", 7) == 0)
			tte->ty_window = value(p);
		else
			break;
	}
	if (zapchar == '#' || *p == '#')
		while ((c = *++p) == ' ' || c == '\t')
			;
	tte->ty_comment = p;
	if (*p == 0)
		tte->ty_comment = 0;
	if (p = index(p, '\n'))
		*p = '\0';
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_getttyent_rmutex);
	return(0);
#else
	return(tte);
#endif
}
