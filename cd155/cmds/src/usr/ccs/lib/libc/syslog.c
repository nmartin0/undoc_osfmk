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
static char rcsid[] = "@(#)$RCSfile: syslog.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:49 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: syslog, setlogmask, openlog, closelog
 *
 * ORIGINS: 26 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * sccsid[] = "syslog.c	1.7  com/lib/c/gen,3.1,9021 4/18/90 17:28:02";
 */

/*
 * FUNCTION: print message on log file
 */

/*
 * SYSLOG -- print message on log file
 *
 * This routine looks a lot like printf, except that it
 * outputs to the log file instead of the standard output.
 * Also:
 *	adds a timestamp,
 *	prints the module name in front of the message,
 *	has some other formatting types (or will sometime),
 *	adds a newline on the end of the message.
 *
 * The output of this routine is intended to be read by /etc/syslogd.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/syslog.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#if defined(NLS) || defined(KJI)
#include <time.h>
#endif

#define	MAXLINE	1024			/* max message size */

#define PRIMASK(p)	(1 << ((p) & LOG_PRIMASK))
#define PRIFAC(p)	(((p) & LOG_FACMASK) >> 3)
#define IMPORTANT 	LOG_ERR

static char	logname[] = "/dev/log";
static char	ctty[] = "/dev/console";

static int	LogFile = -1;		/* fd for log */
static int	LogStat	= 0;		/* status bits, set by openlog() */
static char	*LogTag = "syslog";	/* string to tag the entry with */
static int	LogMask = 0xff;		/* mask of priorities to be logged */
static int	LogFacility = LOG_USER;	/* default facility code */

static struct sockaddr SyslogAddr;	/* AF_UNIX address of local logger */

extern	int errno, sys_nerr;
extern	char *sys_errlist[];
#ifdef MSG
#include "libc_msg.h"
#endif

#if defined(NLS) || defined(KJI)
#define SPRINTF	NLvsprintf
#else
#define SPRINTF vsprintf
#endif

syslog(int pri, const char *fmt, ...)
{
	va_list arg_list;
	char buf[MAXLINE + 1], outline[MAXLINE + 1];
	register char *b, *o;
	register const char *f;
	register int c;
	time_t   now;
	pid_t pid;
	int olderrno = errno;

	/* see if we should just throw out this message */
        if (pri < 0 || PRIFAC(pri) >= LOG_NFACILITIES || (PRIMASK(pri) & LogMask) == 0)
		return(-1);
	if (LogFile < 0)
		openlog(LogTag, LogStat | LOG_NDELAY, 0);

	/* set default facility if none specified */
	if ((pri & LOG_FACMASK) == 0)
		pri |= LogFacility;

	/* build the message */
	o = outline;
	sprintf(o, "<%d>", pri);
	o += strlen(o);
	time(&now);
	sprintf(o, "%.15s ", ctime(&now) + 4);
	o += strlen(o);
	if (LogTag) {
		strcpy(o, LogTag);
		o += strlen(o);
	}
	if (LogStat & LOG_PID) {
		sprintf(o, "[%d]", getpid());
		o += strlen(o);
	}
	if (LogTag) {
		strcpy(o, ": ");
		o += 2;
	}

	b = buf;

	/* f is the first element of the variable list
	   the rest of the variable list is stored in arg_list
	 */

	va_start(arg_list, fmt);
	
	f = fmt;

        while ((c = *f++) != '\0' && c != '\n' && b < &buf[MAXLINE]) {
		if (c != '%') {
			*b++ = c;
			continue;
		}
		if ((c = *f++) != 'm') {
			*b++ = '%';
			*b++ = c;
			continue;
		}
		if ((unsigned)olderrno > sys_nerr)
			sprintf(b, "error %d", olderrno);
		else
#ifdef MSG
			sprintf(b, NLgetamsg( MF_LIBC, MS_LIBC, errno, 
				sys_errlist[errno]));
#else
			strcpy(b, sys_errlist[olderrno]);
#endif
		b += strlen(b);
	}
	*b++ = '\n';
	*b = '\0';
	
	SPRINTF(o, buf, arg_list);
	c = strlen(outline);
	if (c > MAXLINE)
		c = MAXLINE;

	/* output the message to the local logger */
	if (sendto(LogFile, outline, c, 0, &SyslogAddr, sizeof SyslogAddr) >= 0)
		return(0);
	if (!(LogStat & LOG_CONS))
		return(0);

	/* output the message to the console */
	pid = fork();
	if (pid == -1)
		return(-1);
	if (pid == 0) {
		int fd;

		signal(SIGALRM, SIG_DFL);
		sigsetmask(sigblock(0) & ~sigmask(SIGALRM));
		alarm(5);
		fd = open(ctty, O_WRONLY);
		alarm(0);
		strcat(o, "\r");
		o = strchr(outline, '>') + 1;
		write(fd, o, c + 1 - (o - outline));
		close(fd);
		_exit(0);
	}
	if (!(LogStat & LOG_NOWAIT))
                if (waitpid(pid, NULL, 0) == -1)
                  return (-1);
                else
                  return (0);
}

/*
 * OPENLOG -- open system log
 */

openlog(ident, logstat, logfac)
	const char *ident;
	int logstat, logfac;
{
	if (ident != NULL)
		LogTag = (char *)ident;
	LogStat = logstat;
	if (logfac != 0)
		LogFacility = logfac & LOG_FACMASK;
	if (LogFile >= 0)
		return(0);
	SyslogAddr.sa_family = AF_UNIX;
	strncpy(SyslogAddr.sa_data, logname, (size_t)sizeof SyslogAddr.sa_data);
	if (LogStat & LOG_NDELAY) {
		LogFile = socket(AF_UNIX, SOCK_DGRAM, 0);
		fcntl(LogFile, F_SETFD, 1);
	}
}

/*
 * CLOSELOG -- close the system log
 */

void closelog()
{

	(void) close(LogFile);
	LogFile = -1;
}

/*
 * SETLOGMASK -- set the log mask level
 */
setlogmask(pmask)
	int pmask;
{
	int omask;

	omask = LogMask;
	if (pmask != 0)
		LogMask = pmask;
	return (omask);
}
