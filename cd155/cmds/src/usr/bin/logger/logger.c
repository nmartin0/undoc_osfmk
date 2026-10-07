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
static char rcsid[] = "@(#)$RCSfile: logger.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:27 $";
#endif

/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: logger
 *
 * ORIGINS: 26, 27
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
 * logger.c	1.4  com/cmd/oper,3.1,9021 4/26/90 17:18:57
 */

#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <sys/syslog.h>
#include <NLctype.h>

#include "logger_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LOGGER,n,s) 

/*
**  LOGGER -- read and log utility
**
**	This routine reads from an input and arranges to write the
**	result on the system log, along with a useful tag.
*/

main(argc, argv)
	int argc;
	char **argv;
{
	extern char *optarg;
	extern int optind;
	int pri = LOG_NOTICE;
	int ch, logflags = 0;
	char *tag, buf[200];

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_LOGGER,0);
	tag = NULL;

	while ((ch = getopt(argc, argv, "f:ip:t:")) != EOF)
		switch ((char)ch) {
		  case 'f':		/* file to log */
			if (freopen(optarg, "r", stdin) == NULL) {
				fprintf(stderr, "logger: ");
				perror(optarg);
				exit(1);
			}
			break;
		  case 'i':		/* log process id also */
			logflags |= LOG_PID;
			break;
		  case 'p':		/* priority */
			pri = pencode(optarg);
			break;
		  case 't':		/* tag */
			tag = optarg;
			break;
		  case '?':
		  default:
			usage();
		}
	argc -= optind;
	argv += optind;

	/* setup for logging */
	openlog(tag ? tag : getlogin(), logflags, 0);
	(void) fclose(stdout);

	/* log input line if appropriate */
	if (argc > 0)
	{
		register char *p, *endp;
		int len;

		for (p = buf, endp = buf + sizeof(buf) - 1;;) {
			len = strlen(*argv);
			if (p + len < endp && p > buf) {
				*--p = '\0';
				syslog(pri, buf);
				p = buf;
			}
			if (len > sizeof(buf) - 1) {
				syslog(pri, *argv++);
				if (!--argc)
					break;
			} else {
				bcopy(*argv++, p, len);
				p += len;
				if (!--argc)
					break;
				*p++ = ' ';
				*--p = '\0';
			}
		}
		if (p != buf) {
			*p = '\0';
			syslog(pri, buf);
		}
		exit(0);
	}

	/* main loop */
	while (fgets(buf, (int)sizeof(buf), stdin) != NULL)
		syslog(pri, buf);

	exit(0);
}


struct code {
	char	*c_name;
	int	c_val;
};

struct code	PriNames[] = {
	"panic",	LOG_EMERG,
	"emerg",	LOG_EMERG,
	"alert",	LOG_ALERT,
	"crit",		LOG_CRIT,
	"err",		LOG_ERR,
	"error",	LOG_ERR,
	"warn",		LOG_WARNING,
	"warning",	LOG_WARNING,
	"notice",	LOG_NOTICE,
	"info",		LOG_INFO,
	"debug",	LOG_DEBUG,
	NULL,		-1
};

struct code	FacNames[] = {
	"kern",		LOG_KERN,
	"user",		LOG_USER,
	"mail",		LOG_MAIL,
	"daemon",	LOG_DAEMON,
	"auth",		LOG_AUTH,
	"security",	LOG_AUTH,
	"syslog",	LOG_SYSLOG,
	"lpr",		LOG_LPR,
	"news",		LOG_NEWS,
	"uucp",		LOG_UUCP,
	"local0",	LOG_LOCAL0,
	"local1",	LOG_LOCAL1,
	"local2",	LOG_LOCAL2,
	"local3",	LOG_LOCAL3,
	"local4",	LOG_LOCAL4,
	"local5",	LOG_LOCAL5,
	"local6",	LOG_LOCAL6,
	"local7",	LOG_LOCAL7,
	NULL,		-1
};


/*
 *  Decode a symbolic name to a numeric value
 */

pencode(s)
	register char *s;
{
	register char *p;
	int lev;
	int fac;
	char buf[100];

	for (p = buf; *s && *s != '.'; )
		*p++ = *s++;
	*p = '\0';
	if (*s++) {
		fac = decode(buf, FacNames);
		if (fac < 0)
			bailout(MSGSTR(UNKFACILITY, "unknown facility name: "), buf);
		for (p = buf; *p++ = *s++; )
			continue;
	} else
		fac = 0;
	lev = decode(buf, PriNames);
	if (lev < 0)
		bailout(MSGSTR(UNKPRIORITY, "unknown priority name: "), buf);

	return ((lev & LOG_PRIMASK) | (fac & LOG_FACMASK));
}


/*
 * NAME: decode
 * FUNCTION: search code table for name, return its value
 */
decode(name, codetab)
	char *name;
	struct code *codetab;
{
	register struct code *c;
	register char *p;
	char buf[40];

	if (NCisdigit(*name))
		return (atoi(name));

	(void) strcpy(buf, name);
	for (p = buf; *p; p++)
		if (NCisupper(*p))
			*p = NCtolower(*p);
	for (c = codetab; c->c_name; c++)
		if (!strcmp(buf, c->c_name))
			return (c->c_val);
	return (-1);
}

/*
 * NAME: bailout
 * FUNCTION: display error message and exit
 */
bailout(msg, arg)
	char *msg, *arg;
{
	fprintf(stderr, "logger: %s%s\n", msg, arg);
	exit(1);
}

usage()
{
	fputs(MSGSTR(USAGE,
		"logger: [-i] [-f file] [-p pri] [-t tag] [ message ... ]\n"),
	    stderr);
	exit(1);
}
