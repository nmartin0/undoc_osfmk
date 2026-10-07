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
static char rcsid[] = "@(#)$RCSfile: touch.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:43:17 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 * 
 * touch.c	1.9  com/cmd/scan,3.1,9008 1/25/90 14:51:58";
 */

/*
 *	Updates the access and modification times of each file or
 *	directory named.  Options include:
 *
 *		-a	Access time only
 *		-c	Do NOT create the file if it doesn't exist
 *		-m	Modification time only
 *		-f	force (the default case).
 */                                                                   

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <locale.h>
#include "touch_msg.h"
#define MSGSTR(Num, Str) NLgetamsg(MF_TOUCH,MS_TOUCH,Num,Str)

#define	dysize(A) (((A)%4)? 365: 366)

struct	stat	stbuf;
int	status;
int dmsize[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char	*cbp;
long	timbuf;

/*
 * NAME: gtime
 *                                                                    
 * FUNCTION: 	Convert ascii time value on command line into a
 *		the number of seconds since 1970.  This value is
 *		returned in global value:
 *
 * GLOBAL VALUE:	timebuf
 *                                                                    
 * RETURN VALUE:	1 failure
 *			2 success
 */  

int
gtime()
{
	register int i, y, t;
	int d, h, m;
	long nt;
	char * NLgetenv();
	char nlsbuf[12];
	int swtch = 0, j;

	if(*NLgetenv("NLDATE") != '?')
	{
		strcpy(nlsbuf, NLgetenv("NLDATE"));
		swtch = ((nlsbuf[1] == 'd') || (nlsbuf[1] == 'D'));
	}
	tzset();
	t = gpair();		/* get the month */
	d = gpair();		/* get the day   */
	if(swtch == 1)
	{
		swtch = t;	/* some countries like to switch the month */
		t = d;		/* and days.                               */
		d = swtch;
	}
	if(*NLgetenv("NLTIME") != '?')
	{
		strcpy(nlsbuf, NLgetenv("NLTIME"));
		j=0;
		swtch = 0;
		while(nlsbuf[j] != '\0')
		{
			if((nlsbuf[j] == 'h' || nlsbuf[j] == 'H') && swtch == 0)
				break;
			if((nlsbuf[j] == 'm' || nlsbuf[j] == 'M') && swtch == 0)
			{
				swtch = 1;
				break;
			}
			j++;
		}
	}
	if(t<1 || t>12)
		return(1);
	if(d<1 || d>31)
		return(1);
	h = gpair();		/* get the hours   */
	m = gpair();		/* get the minutes */
	if(swtch == 1)
	{
		swtch = h;	/* and some countries switch the hours */
		h = m;
		m = swtch;
	}
	if(h == 24) {
		h = 0;
		d++;
	}
	if(m<0 || m>59)
		return(1);
	y = gpair();		/* get the year */
	if (y<0) {		/* get year ourselves if not given */
		(void) time(&nt);
		y = localtime(&nt)->tm_year;
	}
	if (*cbp == 'p')
		h += 12;
	if (h<0 || h>23)
		return(1);
	timbuf = 0;
	y += 1900;
	for(i=1970; i<y; i++)
		timbuf += dysize(i);
	/* Leap year */
	if (dysize(y)==366 && t >= 3)
		timbuf += 1;
	while(--t)
		timbuf += dmsize[t-1];
	timbuf += (d-1);
	timbuf *= 24;
	timbuf += h;
	timbuf *= 60;
	timbuf += m;
	timbuf *= 60;
	return(0);
}


/*
 * NAME: gpair
 *                                                                    
 * FUNCTION: 		Converts the first two characters in a given parameter
 *			string into an integer.
 *                                                                   
 * RETURN VALUE:	success:  integer cooresponding to the first two digits
 *			failure:  -1
 */  
int
gpair()
{
	register int c, d;
	register char *cp;

	cp = cbp;
	if(*cp == 0)
		return(-1);
	c = (*cp++ - '0') * 10;
	if (c<0 || c>100)
		return(-1);
	if(*cp == 0)
		return(-1);
	if ((d = *cp++ - '0') < 0 || d > 9)
		return(-1);
	cbp = cp;
	return (c+d);
}

main(argc, argv)
int argc;
char *argv[];
{
	register int c;
	struct utbuf { long actime, modtime; } times;

	int mflg=1, aflg=1, cflg=0, nflg=0, errflg=0, optc, fd;
	extern int optind;

	(void) setlocale (LC_ALL,"");

	while ((optc=getopt(argc, argv, "amcf")) != EOF)
		switch(optc) {
		case 'm':
			mflg++;		/* Modification times only    */
			aflg--;
			break;
		case 'a':		/* Access  times only         */
			aflg++;
			mflg--;
			break;
		case 'c':		/* Don't creat if isn't there */
			cflg++;
			break;
		case 'f':		/* silent compatibility case  */
			break;
		case '?':
			errflg++;
		}

	if(((argc-optind) < 1) || errflg) {
		(void) fprintf(stderr, MSGSTR(TUSAGE, "usage: touch [-amc] [mmddhhmm[yy]] file ...\n"));
		exit(2);
	}
	status = 0;
	if(!isnumber(argv[optind]))
		if((aflg <= 0) || (mflg <= 0))
			timbuf = time((long *) 0);
		else
			nflg++;
	else {
		cbp = (char *)argv[optind++];
		if(gtime()) {
			(void) fprintf(stderr,MSGSTR(TCONV, "date: bad conversion\n"));
			exit(2);
		}
		timbuf += timezone;
		if (localtime(&timbuf)->tm_isdst)
			timbuf += -1*60*60;
	}
	for(c=optind; c<argc; c++) {
		if(stat(argv[c], &stbuf)) {
			if(cflg) {
				status++;
				continue;
			}
			else if (errno == ENOENT) {
				if ((fd = creat (argv[c], 0666)) < 0) {
					(void) fprintf(stderr, MSGSTR(TCREATE, "touch: %s cannot create\n"), argv[c]);
					status++;
					continue;
				}
				(void) close(fd);
				if(stat(argv[c], &stbuf)) {
					(void) fprintf(stderr,MSGSTR(TSTAT, "touch: %s cannot stat\n"),argv[c]);
					status++;
					continue;
				}
			}
			else {
				fprintf(stderr, "touch %s:", argv[c]);
				perror((char *)NULL);
			}
		}

		times.actime = times.modtime = timbuf;
		if (mflg <= 0)
			times.modtime = stbuf.st_mtime;
		if (aflg <= 0)
			times.actime = stbuf.st_atime;

		if(utime(argv[c], (struct utbuf *)(nflg? 0: &times))) {
			(void) fprintf(stderr,MSGSTR(TCHTIME, "touch: cannot change times on %s\n"),argv[c]); 
			status++;
			continue;
		}
	}
	exit(status);
}


/*
 * NAME: isnumber
 *                                                                    
 * FUNCTION: 		See if the passed in string is all digits.
 *                                                                    
 * RETURN VALUE:	1 it is.
 *			0 it is not.
 */  
int
isnumber(s)
char *s;
{
	register int c;

	while(c = *s++)
		if(!isdigit(c))
			return(0);
	return(1);
}
