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
static char rcsid[] = "@(#)$RCSfile: getservent.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:05:55 $";
#endif

/* 
 * COMPONENT_NAME: CMDNET getservent.c
 * 
 * FUNCTIONS: any, endservent, getservbyname, getservbyport, 
 *            getservent, setservent
 *
 * ORIGINS: 24  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * sccsid[] = "getservent.c      1.11  com/lib/c/net,3.1,9021 5/9/90 12:59:30";
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
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
 * getservbyname.c     5.6 (Berkeley) 6/1/90
 * getservbyport.c     5.6 (Berkeley) 6/1/90
 * getservent.c        5.8 (Berkeley) 6/1/90
 */

#include <stdio.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>		/* for htons() macro (need types.h) */

int _serv_stayopen;

#define	MAXALIASES	35

struct servent *
getservbyname(name, proto)
	const char *name, *proto;
{
	register struct servent *p;
	register char **cp;

	setservent(_serv_stayopen);
	while (p = getservent()) {
		if (strcmp(name, p->s_name) == 0)
			goto gotname;
		for (cp = p->s_aliases; *cp; cp++)
			if (strcmp(name, *cp) == 0)
				goto gotname;
		continue;
gotname:
		if (proto == 0 || strcmp(p->s_proto, proto) == 0)
			break;
	}
	if (!_serv_stayopen)
		endservent();
	return (p);
}

struct servent *
getservbyport(port, proto)
	int port;
	const char *proto;
{
	register struct servent *p;

	setservent(_serv_stayopen);
	while (p = getservent()) {
		if (port != NULL && p->s_port != port)
			continue;
		if (proto == 0 || strcmp(p->s_proto, proto) == 0)
			break;
	}
	if (!_serv_stayopen)
		endservent();
	return (p);
}


static FILE *servf = NULL;
static char line[BUFSIZ+1];
static struct servent serv;
static char *serv_aliases[MAXALIASES];
static char *any();

setservent(f)
	int f;
{
	if (servf == NULL)
                servf = fopen(_PATH_SERVICES, "r" );
	else
		rewind(servf);
	_serv_stayopen |= f;
}

endservent()
{
	if (servf) {
		fclose(servf);
		servf = NULL;
	}
	_serv_stayopen = 0;
}

struct servent *
getservent()
{
	char *p;
	register char *cp, **q;

        if (servf == NULL && (servf = fopen(_PATH_SERVICES, "r" )) == NULL)
		return (NULL);
again:
	if ((p = fgets(line, BUFSIZ, servf)) == NULL)
		return (NULL);
	if (*p == '#')
		goto again;
	cp = any(p, "#\n");
	if (cp == NULL)
		goto again;
	*cp = '\0';
	serv.s_name = p;
	p = any(p, " \t");
	if (p == NULL)
		goto again;
	*p++ = '\0';
	while (*p == ' ' || *p == '\t')
		p++;
	cp = any(p, ",/");
	if (cp == NULL)
		goto again;
	*cp++ = '\0';
	serv.s_port = htons((u_short)atoi(p));
	serv.s_proto = cp;
	q = serv.s_aliases = serv_aliases;
	cp = any(cp, " \t");
	if (cp != NULL)
		*cp++ = '\0';
	while (cp && *cp) {
		if (*cp == ' ' || *cp == '\t') {
			cp++;
			continue;
		}
		if (q < &serv_aliases[MAXALIASES - 1])
			*q++ = cp;
		cp = any(cp, " \t");
		if (cp != NULL)
			*cp++ = '\0';
	}
	*q = NULL;
	return (&serv);
}

static char *
any(cp, match)
	register char *cp;
	char *match;
{
	register char *mp, c;

	while (c = *cp) {
		for (mp = match; *mp; mp++)
			if (*mp == c)
				return (cp);
		cp++;
	}
	return ((char *)0);
}
