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
static char rcsid[] = "@(#)$RCSfile: date.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:10 $";
#endif
/*
 * COMPONENT_NAME: (CMDSTAT) status
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989,1990
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
 * date.c      1.27  com/cmd/stat,3.1,9021 3/13/90 14:29:50
 */
#include <NLchar.h>
#define BUFSIZE 100
static char buffer[BUFSIZE];

extern char *strcpy();
extern char *strchr();
extern char *malloc();

extern char *getlogin();

#define BLEN	64
/*
**	date - with format capabilities
*/

#include        <sys/secdefines.h>
#if SEC_BASE
#include        <sys/security.h>
#endif

#include	<sys/types.h>
#include	<fcntl.h>
#include	<utmp.h>
#include	<stdio.h>
#include	<time.h>
#include	<locale.h>
#include	<errno.h>
#include	<sys/syslog.h>

#include        <nl_types.h>
#include        "date_msg.h"

#define MONTH	itoa(tim->tm_mon+1,cp,2)
#define DAY	itoa(tim->tm_mday,cp,2)
#define YEAR	itoa(tim->tm_year,cp,2)
#define HOUR	itoa(tim->tm_hour,cp,2)
#define MINUTE	itoa(tim->tm_min,cp,2)
#define SECOND	itoa(tim->tm_sec,cp,2)
#define JULIAN	itoa(tim->tm_yday+1,cp,3)
#define WEEKDAY itoa(tim->tm_wday,cp,1)
#define MODHOUR itoa(h,cp,2)
#define dysize(A) (((A) % 4 == 0 && (A) % 100 != 0 || (A) % 400 == 0) ? 366:365)

#define MSGSTR(num,str) catgets(catd,MS_DATE,num,str)  /*MSG*/

int dmsize[12] = { 0,31,59,90,120,151,181,212,243,273,304,334};
int uflag=0, nflag=0;


char *NLgetenv();
char *pnltime, *pnldate;

char	*itoa();
static void	usage();
char	*cbp;
time_t	timbuf;

struct	utmp	wtmp[2] = { {"","",OTIME_MSG,OLD_TIME,0,0,0,0,""},
			    {"","",NTIME_MSG,NEW_TIME,0,0,0,0,""} };

nl_catd catd;

main(argc, argv)
int	argc;
int	**argv;
{
	register char	*aptr, *cp, c;
	int	 h, hflag, i, tfailed, wf;
	time_t	tbuf;
	long	lseek();
	void	exit();
	struct	tm  *tim;
	char	buf[200], *tzn;
	char	str[32];
	char nlsbuf[15];
#if SEC_BASE
        int             privs_raised;
        privvec_t       saveprivs;
        extern priv_t   *privvec();
#endif

	(void) setlocale (LC_ALL,"");

	if ((pnltime = NLgetenv("NLTIME")) == (char *)0)
		pnltime = "%H:%M:%S";
	if ((pnldate = NLgetenv("NLDATE")) == (char *)0)
		pnldate = "%B/%D/%Y";

	catd = catopen(MF_DATE,0);

#if SEC_BASE
        set_auth_parameters(argc, argv);
        initprivs();
#endif

	tfailed = 0;
	argv++;
	cbp = (char *)argv[0];

	while (cbp && *cbp == '-') {
		cbp++;
		argc--;
		while (cbp && *cbp) {
			if (*cbp == 'u')
				uflag++;
			else if (*cbp == 'n')
				nflag++;
			else {
				usage();
				exit(1);
			}
			cbp++;
		}
		argv++;
		cbp = (char *) argv[0];
	}

	if(argc > 1) {
		if(*cbp == '+') {
			hflag = 0;
			for(cp=buf;cp< &buf[200];)
				*cp++ = '\0';
			(void) time(&tbuf);
			tim = uflag ? gmtime(&tbuf) : localtime(&tbuf);
			/*
			aptr = (char *)argv[1];
			*/
			aptr = cbp;
			aptr++;
			cp = buf;
			while(c = *aptr++) {
			if(c == '%')
			    switch(*aptr++) {
			case '%':
				*cp++ = '%';
				continue;
			case 'a':
				(void)strftime(nlsbuf, BLEN, "%a", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'A':
				(void)strftime(nlsbuf, BLEN, "%A", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'b':
				(void)strftime(nlsbuf, BLEN, "%b", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'B':
				(void)strftime(nlsbuf, BLEN, "%B", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'c':
				(void)strftime(nlsbuf, BLEN, "%c", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'd':
				cp = DAY;
				continue;
			case 'D':
				(void)strftime(nlsbuf, BLEN, "%D", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'h': 
				(void)strftime(nlsbuf, BLEN, "%h", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'H':
				cp = HOUR;
				continue;
			case 'I':
				(void)strftime(nlsbuf, BLEN, "%I", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'j':
				cp = JULIAN;
				continue;
			case 'm' :
				cp = MONTH;
				continue;
			case 'M':
				cp = MINUTE;
				continue;
			case 'n':
				*cp++ = '\n';
				continue;
			case 'p':
				(void)strftime(nlsbuf, BLEN, "%p", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'r':
				(void)strftime(nlsbuf, BLEN, "%r", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'S':
				cp = SECOND;
				continue;
			case 't':
				*cp++ = '\t';
				continue;
			case 'T':
				(void)strftime(nlsbuf, BLEN, "%T", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'U':
				(void)strftime(nlsbuf, BLEN, "%U", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'w':
				cp = WEEKDAY;
				continue;
			case 'W':
				(void)strftime(nlsbuf, BLEN, "%W", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'x':
				(void)strftime(nlsbuf, BLEN, "%x", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'X':
				(void)strftime(nlsbuf, BLEN, "%X", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'y' :
				cp = YEAR;
				continue;
		        case 'Y':
				(void)strftime(nlsbuf, BLEN, "%Y", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			case 'Z':
				(void)strftime(nlsbuf, BLEN, "%Z", tim);
				strcat(buf,nlsbuf);
				cp += strlen(nlsbuf);
				continue;
			default:
				(void) fprintf(stderr, MSGSTR(BADFORMAT,
					  "date: bad format character - %c\n"),
					  *--aptr);
				usage();
				exit(2);
			}	/* endsw */
			*cp++ = c;
			}	/* endwh */

			*cp = '\n';
			printf("%s",buf);
			exit(0);
		}

		if(gtime()) {
			(void) fprintf(stderr, MSGSTR(ECONV,
				       "date: bad conversion\n"));
			usage();
			exit(2);
		}

		if (uflag == 0){
	/* convert to Greenwich time, on assumption of Standard time. */
			timbuf += timezone;

	/* Now fix up to local daylight time. */
			if (localtime(&timbuf)->tm_isdst)
				timbuf += -1*60*60;
		}

		(void) time(&wtmp[0].ut_time);

#if SEC_BASE
                /*
                 * If user is authorized to set the date, raise the
                 * necessary privileges.  SEC_SYSATTR is required to
                 * set it locally, and SEC_REMOTE is required to create
                 * a privileged socket to talk to the time daemon.
		 * The access control overrides allow us to make entries
		 * in the utmp and wtmp files.
                 */
                if (authorized_user("sysadmin")) {
                        if (forceprivs(privvec(SEC_SYSATTR, SEC_REMOTE,
						SEC_ALLOWDACACCESS,
#if SEC_MAC
						SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
						SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
						SEC_ALLOWNCAVACCESS,
#endif
						-1), saveprivs)) {
                                fprintf(stderr, MSGSTR(EPRIV,
                                        "date: insufficient privileges\n"));
                                exit(1);
                        }
			privs_raised = 1;
                } else
			privs_raised = 0;
#endif

		if (nflag || !settime(timbuf)) {
#if SEC_BASE
                        disablepriv(SEC_SUSPEND_AUDIT);
#endif
			if(stime(&timbuf) < 0) {
#if SEC_BASE
				forcepriv(SEC_SUSPEND_AUDIT);
#endif
				tfailed++;
				(void) fprintf(stderr, MSGSTR(BADPERM,
						"date: no permission\n"));
			} else 	{
#if SEC_BASE
				forcepriv(SEC_SUSPEND_AUDIT);
#endif
				(void) time(&wtmp[1].ut_time);

/*	Attempt to write entries to the utmp file and to the wtmp file. */

				pututline(&wtmp[0]);
				pututline(&wtmp[1]);

				if ((wf = open(WTMP_FILE, O_WRONLY|O_APPEND)) >= 0) {
					(void) write(wf, (char *)wtmp, sizeof(wtmp));
				}
			}
		}
#if SEC_BASE
		if (privs_raised)
			seteffprivs(saveprivs, (priv_t *) 0);
#endif

		if (!tfailed){
			char *username;
			username = getlogin();
			/* single user or no tty */
			if (username == NULL || *username == '\0')
				username = "root";
			syslog(LOG_NOTICE, "set by %s", username);
		}

	}

	(void) time(&timbuf);

	(void)strftime(buffer, BLEN, "%a %sD %X \0", 
		uflag == 0 ? localtime(&timbuf) : gmtime(&timbuf));

	cbp=buffer;
	printf("%s",cbp);
	if (uflag == 0)
		tzn = tzname[localtime(&timbuf)->tm_isdst];
	else
		tzn = "CUT";	/* Coordinated Universal Time */
	if (tzn)
		printf("%s",tzn);
	(void)strftime(buffer, BLEN, " %Y\n\0",
		uflag == 0 ? localtime(&timbuf) : gmtime(&timbuf));
	cbp=buffer;
	printf("%s",cbp);
	exit(tfailed?2:0);
	/* NOTREACHED */
}




/*
 *  NAME:  gtime
 *
 *  FUNCTION:  convert the time string given on the command line into
		a timebuf structure.
 *	      
 *  RETURN VALUE:  	 0   - succeeded in conversion
 *			 1   - conversion failed
 */

gtime()
{
	register char *s;
	register struct tm *tm;
	register int j, i;
	int ind=1;
	int year_at_end=0;
	int set_year=0,dot_flg=0;
	time_t nt;
	int buf[7];

       int swtch = 0;

	tzset();
	(void) time(&nt);
	tm = localtime(&nt);

	buf[0] = buf[1] = buf[5] = 0;
	i = 0;
	for (s = cbp; ; )
	{   
	    if (dot_flg) 
	    {
		if (set_year)
			break;
		j = 6;
	    	if (s[0] < '0' || s[0] > '9')
		{
/*
 * Year may be set at beginning in buf[1] for yyMMddhhmm.ss format or
 * may be set at end in buf[5] to support MMddhhmmyy.ss format.
*/
			if ( buf[1] < 88 && buf[5] < 88 )
				buf[j] = tm->tm_year;	/* use current year */
		}
		else set_year++;
	    }
	    else
	    if (*s == '.')
	    {	j = 0;
		i++;
		dot_flg++;
		s++;
	    } else {
		j = ++i;
		if (j > 6)
			return(1);
	      }
           /*
            * If the character is out of the bounds of proper date
            * format, check to see if it's a blank.  If it's a blank
            * then just break otherwise it's an error.
            */
	    if (s[0] < '0' || s[0] > '9') {
                if (s[0] == '\0')
                        break;
                else
                        return(1);
            }
	    if (s[1] < '0' || s[1] > '9' || i >= 7)
		return(1);
	    buf[j] = (*s++ - '0') * 10;
	    buf[j] += *s++ - '0';
	}
		if(switchdate() == 1)
		{
			if ( buf[1] >= 88 && buf[1] <= 99 )
			{
				swtch = buf[2];
				buf[2] = buf[3];
				buf[3] = swtch;
			}
			else {
				swtch = buf[1];
				buf[1] = buf[2];
				buf[2] = swtch;
			}
		}
		if(switchtime() == 1)
		{
			if ( buf[1] >= 88 && buf[1] <= 99 )
			{
				swtch = buf[4];
				buf[4] = buf[5];
				buf[5] = swtch;
			}
			else {
				swtch = buf[3];
				buf[3] = buf[4];
				buf[4] = swtch;
			}
		}
	
	if (set_year)		/* year was specified after the seconds */
		tm->tm_year = buf[6];

	switch (--i)
	{
	case 5:			/* year was specified in the beginning */
	    if (buf[ind] > 87) 
		tm->tm_year = buf[ind++];
	    else
		year_at_end++;
	case 4:
	    tm->tm_mon = buf[ind++] - 1;
	    if (tm->tm_mon < 0 || tm->tm_mon > 11)
		return(1);
	case 3:
	    tm->tm_mday = buf[ind++];
	    if (tm->tm_mday < 1 || tm->tm_mday > 31)
		return(1);
	case 2:
	    tm->tm_hour = buf[ind++];
/* This am/pm stuff may not be documented */

	    if (tm->tm_hour < 12 && *s == 'p')
		tm->tm_hour += 12;
	    else if (tm->tm_hour >= 12 && *s == 'a')
		tm->tm_hour -= 12;
	    if ((tm->tm_hour >= 24) && ((*s == 'a') || (*s == 'p')))
	    {	tm->tm_hour -= 24;
		tm->tm_mday += 1;
	    }
	    if (tm->tm_hour < 0 || tm->tm_hour > 23)
		return(1);
	case 1:
	    tm->tm_min = buf[ind++];
	    if (tm->tm_min < 0 || tm->tm_min > 59)
		return(1);
	case 0:
	    if (year_at_end)
		tm->tm_year = buf[ind++];
	    tm->tm_sec = buf[0];
	    if (tm->tm_sec < 0 || tm->tm_sec > 59)
		return(1);
	}
	timbuf = 0;
	i = tm->tm_year+1900;
	for (j=1970; j<i; j++)
	    timbuf += dysize(j);
	if (tm->tm_mon >= 2 && dysize(i) == 366)
	    timbuf += 1;
	timbuf += dmsize[tm->tm_mon];
	timbuf += tm->tm_mday - 1;
	timbuf *= 24;
	timbuf += tm->tm_hour;
	timbuf *= 60;
	timbuf += tm->tm_min;
	timbuf *= 60;
	timbuf += tm->tm_sec;
	return(0);
}

/*
 *  NAME:  switchdate
 *
 *  FUNCTION:  Check the NLDATE environment variable to see if the month and
 *              day must change positions.
 *	      
 *  RETURN VALUE:  	 0  no swap is necessary
 *                       1  swap the month and day 
 */
 int

 switchdate()
 {
        int bfound = 0;
        char nlsbuf[12];
        int swtch = 0;
        int j = 0;

	if (pnldate != NULL)
	{
           (void)strcpy(nlsbuf, pnldate);
	   while(nlsbuf[j] != '\0')
	   {
	      if (nlsbuf[j] == 'd' || nlsbuf[j] == 'D')
		   bfound++;
	      if (nlsbuf[j] == 'b' || nlsbuf[j] == 'B') 
		{
		   if (bfound) 
			swtch = 1;
		   break;
		}
	    	j++;
	   }
        }
        return(swtch);
}
/*
 *  NAME:  switchtime
 *
 *  FUNCTION:  Check the NLTIME environment variable to see if the hour and
 *              minute must change positions.
 *	      
 *  RETURN VALUE:  	 0  no swap is necessary
 *                       1  swap the hour and minute 
 */
 int

 switchtime()
 {
        int hfound = 0;
        char nlsbuf[12];
        int swtch = 0;
        int j = 0;

	if (pnltime != NULL)
	{
           (void)strcpy(nlsbuf, pnltime);
	   while(nlsbuf[j] != '\0')
	   {
	      if (nlsbuf[j] == 'm' || nlsbuf[j] == 'M')
		   hfound++;
	      if (nlsbuf[j] == 'h' || nlsbuf[j] == 'H') 
		{
		   if (hfound) 
			swtch = 1;
		   break;
		}
	    	j++;
	   }
        }
        return(swtch);
}

/*
 *  NAME:  itoa
 *
 *  FUNCTION:  Change an integer to an ascii
 *	      
 *  RETURN VALUE:  	 A pointer to the converted integer.
 */

char *
itoa(i,ptr,dig)
register  int	i;
register  int	dig;
register  char	*ptr;
{
	switch(dig)	{
		case 3:
			*ptr++ = i/100 + '0';
			i = i - i / 100 * 100;
		case 2:
			*ptr++ = i / 10 + '0';
		case 1:
			*ptr++ = i % 10 + '0';
	}
	return(ptr);
}


#include <sys/param.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define TSPTYPES
#include <protocols/timed.h>

#define WAITACK		2	/* seconds */
#define WAITDATEACK	5	/* seconds */

extern	int errno;
/*
 *  NAME:  settime
 *
 *  FUNCTION:  
 * 	Set the date in the machines controlled by timedaemons
 * 	by communicating the new date to the local timedaemon. 
 * 	If the timedaemon is in the master state, it performs the
 * 	correction on all slaves.  If it is in the slave state, it
 * 	notifies the master that a correction is needed.
 *	      
 *  RETURN VALUE:
 *	 	Returns 1 on success, 0 on failure.
 */

settime(timbuf)
	time_t timbuf;
{
	int s, length, port, timed_ack, found, err;
	long waittime;
	fd_set ready;
	char hostname[MAXHOSTNAMELEN];
	struct timeval tout;
	struct timeval tv;
	struct servent *sp;
	struct tsp msg;
	struct sockaddr_in sin, dest, from;

	tv.tv_sec = timbuf;
	tv.tv_usec = 0;

	sp = getservbyname("timed", "udp");
	if (sp == 0) {
		fprintf(stderr, MSGSTR(UNK_SERVICE,"date: udp/timed: unknown service\n"));
		return (0);
	}	
	dest.sin_port = sp->s_port;
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl((u_long)INADDR_ANY);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		if (errno != EPROTONOSUPPORT)
			perror("date: socket");
		goto bad;
	}
	bzero((char *)&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	for (port = IPPORT_RESERVED - 1; port > IPPORT_RESERVED / 2; port--) {
		sin.sin_port = htons((u_short)port);
		if (bind(s, (struct sockaddr *)&sin, sizeof (sin)) >= 0)
			break;
		if (errno != EADDRINUSE) {
			if (errno != EADDRNOTAVAIL)
				perror("date: bind");
			goto bad;
		}
	}
	if (port == IPPORT_RESERVED / 2) {
		fprintf(stderr, MSGSTR(NOPORTS,"date: all ports in use\n"));
		goto bad;
	}
	msg.tsp_type = TSP_SETDATE;
	msg.tsp_vers = TSPVERSION;
	(void) gethostname(hostname, sizeof (hostname));
	(void) strncpy(msg.tsp_name, hostname, sizeof (hostname));
	msg.tsp_seq = htons((u_short)0);
	msg.tsp_time.tv_sec = htonl((u_long)tv.tv_sec);
	msg.tsp_time.tv_usec = htonl((u_long)tv.tv_usec);
	length = sizeof (struct sockaddr_in);
	if (connect(s, &dest, length) < 0) {
		perror("date: connect");
		goto bad;
	}
	if (send(s, (char *)&msg, sizeof (struct tsp), 0) < 0) {
		if (errno != ECONNREFUSED)
			perror("date: send");
		goto bad;
	}
	timed_ack = -1;
	waittime = WAITACK;
loop:
	tout.tv_sec = waittime;
	tout.tv_usec = 0;
	FD_ZERO(&ready);
	FD_SET(s, &ready);
	found = select(FD_SETSIZE, &ready, (fd_set *)0, (fd_set *)0, &tout);
	length = sizeof(err);
	if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&err, &length) == 0
	    && err) {
		errno = err;
		if (errno != ECONNREFUSED)
			perror("date: send (delayed error)");
		goto bad;
	}
	if (found > 0 && FD_ISSET(s, &ready)) {
		length = sizeof (struct sockaddr_in);
		if (recvfrom(s, (char *)&msg, sizeof (struct tsp), 0, &from,
		    &length) < 0) {
			if (errno != ECONNREFUSED)
				perror("date: recvfrom");
			goto bad;
		}
		msg.tsp_seq = ntohs(msg.tsp_seq);
		msg.tsp_time.tv_sec = ntohl(msg.tsp_time.tv_sec);
		msg.tsp_time.tv_usec = ntohl(msg.tsp_time.tv_usec);
		switch (msg.tsp_type) {

		case TSP_ACK:
			timed_ack = TSP_ACK;
			waittime = WAITDATEACK;
			goto loop;

		case TSP_DATEACK:
			(void)close(s);
			return (1);

		default:
			fprintf(stderr,
			    MSGSTR(WRONG_ACK,"date: wrong ack received from timed: %s\n"), 
			    tsptype[msg.tsp_type]);
			timed_ack = -1;
			break;
		}
	}
	if (timed_ack == -1)
		fprintf(stderr,
		    MSGSTR(CANTREACH,"date: can't reach time daemon - time set locally\n"));
bad:
	(void)close(s);
	return (0);
}


/*
 *  NAME:  usage
 *
 *  FUNCTION:	Print out the command options.  They are
 *		different for the super user.
 *	      
 *  RETURN VALUE:  	 void
 */

static void
usage()
{
   char prompt[8];
   int ts_flg = 0;
   int ds_flg = 0;


	if (getuid() == 0) {
	   sprintf(prompt,"%s%s%s%s",MSGSTR(USAGEMONTH,"MM"),MSGSTR(USAGEDAY,"dd"),MSGSTR(USAGEHOUR,"hh"),MSGSTR(USAGEMINUTE,"mm"));

		ds_flg = switchdate();
		ts_flg = switchtime();

		sprintf(prompt,"%s%s%s%s", ds_flg ? MSGSTR(USAGEDAY,"dd") : MSGSTR(USAGEMONTH,"MM"), ds_flg ? MSGSTR(USAGEMONTH,"MM") : MSGSTR(USAGEDAY,"dd"), ts_flg ? MSGSTR(USAGEMINUTE,"mm") : MSGSTR(USAGEHOUR,"hh"), ts_flg ? MSGSTR(USAGEHOUR,"hh") : MSGSTR(USAGEMINUTE,"mm"));


		fprintf(stderr, MSGSTR(USAGEROOT,"usage: date [-n][-u][%s.ss][yy] [+field descriptors]\n"),prompt);
		fprintf(stderr, MSGSTR(USAGEROOT2,"usage: date [-n][-u][yy%s.ss] [+field descriptors]\n"),prompt);
	}
	else {
		fprintf(stderr, MSGSTR(USAGE,"usage: date [-u] [+field descriptors]\n"));
	}
}
