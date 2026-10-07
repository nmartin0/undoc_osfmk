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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: last.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:42 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if	!lint
static char sccsid[] = "last.c	1.3  com/cmd/stat,3.1,8943 9/8/89 07:58:29";
#endif
/*
 * COMPONENT_NAME: (CMDSTAT) status
 *
 * FUNCTIONS:
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <utmp.h>
#include <time.h>
#include <locale.h>


#define NMAX    sizeof(buf[0].ut_user)
#define LMAX	sizeof(buf[0].ut_line)
#define HMAX    sizeof(buf[0].ut_host)
#define NWIDTH	8			/* printing width of ut_user */
#define LWIDTH	12			/* printing width of ut_line */
#define HWIDTH	16			/* printing width of ut_host */

#define	SECDAY	(24*60*60)
#define MAXTTYS 256

#define	lineq(a,b)	(!strncmp(a,b,LMAX))
#define	nameq(a,b)	(!strncmp(a,b,NMAX))
#define	hosteq(a,b)	(!strncmp(a,b,HMAX))

#include "last_msg.h"
#define MSGSTR(Num, Str)     catgets(catd, MS_LAST, Num, Str)
nl_catd	catd;

char	**argv;
int	argc;
int	nameargs;

struct	utmp buf[128];
char	ttnames[MAXTTYS][LMAX+1];
long	logouts[MAXTTYS];

char	*strspl();
void	onintr();
extern struct tm *gmtime();

main(ac, av)
	int ac;
	char **av;
{
	register int i, k;
	int bl, wtmp;
	char dt[11];
	char tt[6];
	register struct utmp *bp;
	long otime;
	struct stat stb;
	int print;
	char * crmsg = (char *)0;
	long crtime;
	long outrec = 0;
	long maxrec = 0x7fffffffL;
 
#ifndef KJI
	(void) setlocale (LC_ALL,"");
#endif
	catd = catopen(MF_LAST,0);

	(void)time(&buf[0].ut_time);
	ac--, av++;
	nameargs = argc = ac;
	argv = av;
	for (i = 0; i < argc; i++) {
		if (argv[i][0] == '-' &&
		    argv[i][1] >= '0' && argv[i][1] <= '9') {
			maxrec = atoi(argv[i]+1);
			nameargs--;
			continue;
		}
		if (strlen(argv[i])>2)
			continue;
		if (!strcmp(argv[i], "~"))
			continue;
		if (!strcmp(argv[i], "ftp"))
			continue;
		if (!strcmp(argv[i], "uucp"))
			continue;
		if (getpwnam(argv[i]))
			continue;
		argv[i] = strspl("tty", argv[i]);
	}
	wtmp = open(WTMP_FILE, O_RDONLY);
	if (wtmp < 0) {
		perror(WTMP_FILE);
		exit(1);
	}
	(void)fstat(wtmp, &stb);
	bl = (stb.st_size + sizeof (buf)-1) / sizeof (buf);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN) {
		(void)signal(SIGINT, onintr);
		(void)signal(SIGQUIT, onintr);
	}
	for (bl--; bl >= 0; bl--) {
		(void)lseek(wtmp, (off_t)(bl * sizeof (buf)), 0);
		bp = &buf[read(wtmp, buf, sizeof (buf)) / sizeof(buf[0]) - 1];
		for ( ; bp >= buf; bp--) {
			print = want(bp);
			if (print) {
				(void)strftime( dt, 11, "%a %h %d",
					localtime(&bp->ut_time) );
				(void)strftime( tt, 6, "%H:%M",
					localtime(&bp->ut_time) );
				(void)printf("%-*.*s  %-*.*s %-*.*s %10.10s %5.5s ",
				    NWIDTH, NWIDTH, bp->ut_name,
				    LWIDTH, LWIDTH, bp->ut_line,
				    HWIDTH, HWIDTH, bp->ut_host,
				    dt, tt );
			}
			for (i = 0; i < MAXTTYS; i++) {
				if (ttnames[i][0] == 0) {
					(void)strncpy(ttnames[i], bp->ut_line,
					    sizeof(bp->ut_line));
					otime = logouts[i];
					logouts[i] = bp->ut_time;
					break;
				}
				if (lineq(ttnames[i], bp->ut_line)) {
					otime = logouts[i];
					logouts[i] = bp->ut_time;
					break;
				}
			}
			if (print) {
				if (lineq(bp->ut_line, "~"))
					(void)printf("\n");
				else if (otime == 0)
					(void)printf(MSGSTR(M_LOGGEDIN,
						"  still logged in\n" ));
				else {
					long delta;
					if (otime < 0) {
						otime = -otime;
						(void)printf("- %s", crmsg);
					} else
						(void)strftime( tt, 6, "%H:%M",
						    localtime(&otime) );
						(void)printf("- %5.5s", tt );
					delta = otime - bp->ut_time;
					if (delta < SECDAY)
					    (void)printf("  (%5.5s)\n",
						asctime(gmtime(&delta))+11);
					else
					    (void)printf(" (%ld+%5.5s)\n",
						delta / SECDAY,
						asctime(gmtime(&delta))+11);
				}
				(void)fflush(stdout);
				if (++outrec >= maxrec)
					return(0);
			}
			if (lineq(bp->ut_line, "~")) {
				for (i = 0; i < MAXTTYS; i++)
					logouts[i] = -bp->ut_time;
				if (nameq(bp->ut_name, "shutdown"))
					crmsg = MSGSTR( M_DOWN, "down " );
				else
					crmsg = MSGSTR( M_CRASH, "crash" );
			}
		}
	}
	(void)strftime( dt, 11, "%a %h %d", localtime(&buf[0].ut_time) );
	(void)strftime( tt, 6, "%H:%M", localtime(&buf[0].ut_time) );

	(void)printf(MSGSTR(M_BEGIN,"\nwtmp begins %10.10s %5.5s \n"),dt,tt);
	return(0);
}

/*
 *  NAME:  onintr
 *
 *  FUNCTION:  if last command is interrupted print the date out then
 *		exit if signal is SIGKILL.
 *  RETURN VALUE:  	 none
 */

void
onintr(signo)
	int signo;
{
	char dt[11];
	char tt[6];

	if (signo == SIGQUIT)
		(void)signal(SIGQUIT, onintr);
	(void)strftime( dt, 11, "%a %h %d", localtime(&buf[0].ut_time) );
	(void)strftime( tt, 6, "%H:%M", localtime(&buf[0].ut_time) );

	(void)printf(MSGSTR(M_INTR,"\ninterrupted %10.10s %5.5s \n"), dt, tt);
	(void)fflush(stdout);
	if (signo == SIGINT)
		exit(1);
}

/*
 *  NAME:  want
 *
 *  FUNCTION:	Determine whether the given utmp structure is one
 *		the user is requesting.
 *	      
 *  RETURN VALUE:  	1 if one is found
 *			0 otherwise
 *			
 */

want(bp)
	struct utmp *bp;
{
	register char **av;
	register int ac;

	if (bp->ut_type != USER_PROCESS && bp->ut_type != DEAD_PROCESS)
		return(0);
	if (bp->ut_line[0] == '~' && bp->ut_name[0] == '\0')
		(void)strcpy(bp->ut_name, "reboot");          /* bandaid */
	if (strncmp(bp->ut_line, "ftp", 3) == 0)
		bp->ut_line[3] = '\0';
	if (strncmp(bp->ut_line, "uucp", 4) == 0)
		bp->ut_line[4] = '\0';
	if (bp->ut_name[0] == 0)
		return (0);
	if (nameargs == 0)
		return (1);
	av = argv;
	for (ac = 0; ac < argc; ac++, av++) {
		if (av[0][0] == '-')
			continue;
		if (nameq(*av, bp->ut_name) || lineq(*av, bp->ut_line))
			return (1);
	}
	return (0);
}

/*
 *  NAME:  strspl
 *
 *  FUNCTION:  		Concatenate two strings.
 *	      
 *  RETURN VALUE:   	I pointer to the new concatenated string.
 */

char *
strspl(left, right)
	char *left, *right;
{
	char *res = (char *)malloc((unsigned)(strlen(left)+strlen(right)+1));
	if (res == NULL)
	{
		perror("malloc failed");
		exit (-1);
	}
	(void)strcpy(res, left);
	(void)strcat(res, right);
	return (res);
}
