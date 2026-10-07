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
 
#if	defined(REFCLOCK) && defined(HEATH)
#ifndef	lint
static char *rcsid = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/ntpd/read_heath.c,v 1.2 1994/11/19 03:13:20 mtm Exp $";
static char *sccsid = "@(#)read_heath.c	1.2     	7/28/89";
#endif	lint

#define	ERR_RATE	60	/* Repeat errors once an hour */
#define WWV_STR_LEN	24	/* length of the string from the WWV clock */

/*
 * read_heath.c
 *     January 1988 -- orignal by Jeffrey I. Schiller <JIS@BITSY.MIT.EDU>
 *     January 1989 -- QU version by Doug Kingston <DPK@Morgan.COM>
 *     July    1989 -- Heath clock port by Jim Knutson <knutson@mcc.com>
 *
 *   This module facilitates reading a Heath Company WWV/WWVH radio clock.
 *   We assume that clock is configured for 9600 baud, no parity and running
 *   in continuous transfer mode with time displayed in GMT. Output is
 *   accepted in either 24 or 12 hour format.  Time is expected and processed
 *   in GMT.
 *
 *   Select is used to prevent hanging in a read when there are
 *   no characters to read.  The line is run in raw mode to
 *   reduce overhead.
 *
 *   Routines defined:
 *	init_clock_heath(): Open the tty associated with the clock and
 *			    set its tty mode bits. Returns fd on success
 *			    and -1 on failure.
 *	read_clock_heath(): Reads the clock and returns either 0 on success
 *			    or non-zero on error.  On success, pointers are
 *			    provided to the reference and local time.
 */
#include <stdio.h>
#include <syslog.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#if defined(sun)
#include <termio.h>
#endif

#ifdef	DEBUG
extern int debug;
#endif	DEBUG

static int nerrors = 0;
static int lostsync = 0;
static char clockdata[WWV_STR_LEN+1];
static char extradata[WWV_STR_LEN+1];
#define	MIN_READ WWV_STR_LEN

static double reltime();

#ifdef STANDALONE

#ifndef CLOCKDEV
#define CLOCKDEV "/dev/radioclock"
#endif

#define DEBUG	1
int debug = 1;

main(argc, argv)
int argc;
char **argv;
{
	struct timeval *tvp, *otvp;

	debug = argc;
	if (openclock(CLOCKDEV))
		do {
			(void)readclock(&tvp, &otvp);
			sleep(1);
		} while (debug>1);
	exit(0);
}
#endif STANDALONE


init_clock_heath(timesource)
char *timesource;
{
	int cfd;
#ifdef TCSETA
	struct termio tty;
#else
	struct sgttyb tty;
#endif

	if ((cfd = open(timesource, 0)) < 0) {
#ifdef DEBUG
		if (debug) perror(timesource); else
#endif DEBUG
		syslog(LOG_ERR, "can't open %s: %m", timesource);
		return(-1);
	}

	if (ioctl(cfd, TIOCEXCL, 0) < 0) {
#ifdef DEBUG
		if (debug) perror("TIOCEXCL on Heath radioclock failed"); else
#endif DEBUG
		syslog(LOG_ERR, "TIOCEXCL on %s failed: %m", timesource);
		return(-1);
	}

#ifdef TCSETA
	if (ioctl(cfd, TCGETA, &tty) < 0) {
#ifdef DEBUG
		if (debug) perror("ioctl on Heath radioclock failed"); else
#endif DEBUG
		syslog(LOG_ERR, "ioctl on %s failed: %m", timesource);
		return(-1);
	}
	tty.c_cflag = (B9600<<16)|B9600|CS8|CLOCAL|CREAD;
	tty.c_iflag = ICRNL;
	tty.c_oflag = 0;
	tty.c_lflag = 0;
	bzero((char *)tty.c_cc, sizeof tty.c_cc);
	tty.c_cc[VMIN] = MIN_READ;
	tty.c_cc[VTIME] = 0;
	if (ioctl(cfd, TCSETA, &tty) < 0) {
#else TCSETA	/* Use older Berkeley style IOCTL's */
	bzero((char *)&tty, sizeof tty);
	tty.sg_ispeed = tty.sg_ospeed = B9600;
	tty.sg_flags = ANYP|RAW;
	tty.sg_erase = tty.sg_kill = '\0';
	if (ioctl(cfd, TIOCSETP, &tty) < 0) {
#endif TCSETA
#ifdef DEBUG
		if (debug) perror("ioctl on Heath radioclock failed"); else
#endif DEBUG
		syslog(LOG_ERR, "ioctl on %s failed: %m", timesource);
		return(-1);
	}
	return(cfd);			/* Succeeded in opening the clock */
}

/*
 * read_clock_heath() -- Read the Heath Radio Clock.
 *
 * Strategy Notes:
 *
 * When the heath clock is set at 9600 baud and is in continuous mode,
 * it will output an alternating set of two or three time strings per tenth
 * of a second.  For instance:
 *
 * 22:20:04.2     07/26/89<CR>
 * 22:20:04.2     07/26/89<CR>
 * 22:20:04.3     07/26/89<CR>
 * 22:20:04.3     07/26/89<CR>
 * 22:20:04.3     07/26/89<CR>
 * 22:20:04.4     07/26/89<CR>
 * 22:20:04.4     07/26/89<CR>
 * 22:20:04.5     07/26/89<CR>
 * 22:20:04.5     07/26/89<CR>
 * 22:20:04.5     07/26/89<CR>
 *
 * We assume that the best place to take a time sample is at the start of
 * the first string of a 3 string sequence.  This is based solely on the
 * guess that fitting 3 strings in 1/10th of a second means that the first
 * string is closer to "on-time" than if just two strings fit.
 */
read_clock_heath(cfd, tvpp, otvpp)
int cfd;
struct timeval **tvpp, **otvpp;
{
	static struct timeval radiotime;
	static struct timeval mytime;
	static struct timeval posttime;
	struct timeval timeout;
	struct tm *mtm;
	struct tm radio_tm, *rtm = &radio_tm;
	register int i;
	register int millis;
	register double diff;
#define MAXLOOPS 5
	int loop = 0;
	int resync = 0;
	int match = 0;
	int shortstr = 0;
	int n;
	int chrcount = 0;
	fd_set readfds;
	char tenths = '\0';
	char message[256];
#ifndef TCSETA
	register char *cp;
	int  need;
#endif TCSETA

	FD_ZERO(&readfds);
	FD_SET(cfd, &readfds);
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	(void) ioctl(cfd, TIOCFLUSH, 0);	/* scrap the I/O queues */

	/* BEGIN MINOR TIME CRITICAL CODE SECTION!!!!!! */
	/* Be extra careful about what code gets added here */

	/* We can't read anything till it's here. */
	if(select(cfd+1, &readfds, 0, 0, &timeout) != 1) {
#ifdef DEBUG
		if (debug) printf("Heath radioclock poll timed out\n"); else
#endif DEBUG
		if ((nerrors++%ERR_RATE) == 0)
			syslog(LOG_ERR, "poll of Heath radioclock failed: %m");
		return(1);
	}

	/* Information sync with the clock output.  Setup for reading
	   whole time string packets.  Note: as long as we read fast
	   enough, the buffer should remain empty.  Hint: pray for
	   not getting swapped, and a light load */
	for (i = 0; i == 0 || (i == 1 && *clockdata != '\r'); ) {
	  	i = read(cfd, clockdata, 1);
		*clockdata &= 0177;
	}

	/* Every cycle between here and the time string read is lost
	   accuracy.  Of course, this assumes that the input buffer
	   is empty.  If it's not, we've already lost accuracy. */

	while (loop < MAXLOOPS) {

	  	/* The "on-time" value for the time string is at the beginning.
		   Hence, the gettimeofday call is first */
	  	(void) gettimeofday(&mytime, (struct timezone *)0);

		/* Raw mode will only give us what's in the buffer. We
		   have to wait 'til the buffer has 1 full string */
		chrcount = 0;
		while (chrcount < WWV_STR_LEN)
		  	ioctl(cfd, FIONREAD, &chrcount);

		/* Read the time string */
		if ((i = read(cfd, clockdata, WWV_STR_LEN)) < WWV_STR_LEN) {
		  	shortstr++;
			loop++;
			continue;
		}

		/* The rest of this loop is used to check whether or not we
		   got an optimal string value.  Any code added here will
		   affect accuracy if we have to loop back to the beginning
		   and try again.  This is done at least once because we
		   have to find when the tenth of a second changes. */

		/* At 9600 baud, the Heath clock blows time strings out the
		   RS232 interface alternating between two and three strings
		   per tenth of a second.  What we assume is that 1st string of
		   3 time strings with the same tenths of a second must be
		   closer to "on-time" than 2 strings.  The loop below
		   attempts to make sure we are synced with the first string
		   of a three string sequence.  If we're not, we have to try
		   again (TIME CRITICAL). */

		if (tenths != '\0' && clockdata[9] != tenths) {
recheck:
			/* Count number of matching time strings */
			extradata[9] = clockdata[9];
			for (match = 0; extradata[9] == clockdata[9]; ) {
				match++;

				/* This is a time critical section only if we
				   have to try again.  We have to save time
				   and char counts in case we need to restart
				   the loop. */
		 
				(void) gettimeofday(&posttime, (struct timezone *)0);

				chrcount = 0;
				while (chrcount < WWV_STR_LEN)
					ioctl(cfd, FIONREAD, &chrcount);

				n = read(cfd, extradata, WWV_STR_LEN);
			}

			/* Optimal time is when we get 3 or more (?) strings
			   which say that the tenths digit hasn't changed. */
			if (match > 2)
				break; /* HURRAY! */
			else {

				/* Crap.  We must have been syncing with a 2
				   string sequence.  This is probably the 3
				   string sequence.  Let's start over. */

				bcopy(extradata,clockdata,n);
				i = n;
				mytime = posttime;
				resync++;
				if (++loop <= MAXLOOPS)
					goto recheck;
				else
					break;
			}
		}

		/* if we make it down here, then we haven't seen the first
		   string of a sequence yet. */
		tenths = clockdata[9];
		loop++;
	}

	/* END OF TIME CRITICAL CODE SECTION!!!! */

	if (loop > MAXLOOPS && clockdata[9] != '?') {
#ifdef DEBUG
		if (debug) printf("Heath radioclock read error\n");
#endif DEBUG
		if ((nerrors++%ERR_RATE) == 0)
			syslog(LOG_ERR, "Heath radioclock read error\n");
		return(1);
	}
	else {
#ifdef DEBUG
		if (debug > 2)
			printf("Read %s\ntries=%d, resyncs=%d, matches=%d, short=%d\n",
			       clockdata,loop,resync,match,
			       shortstr);
#endif
	}

	/* Now that we have a time string, verify that it is what we expect. */
	if (!verify(clockdata,i)) {
	  	return(1);
	}

	/* If the clock hasn't synced with WWV yet, the time string will show
	   all ?s */
	if (clockdata[1] == '?' && (nerrors++%ERR_RATE)==0) {
	  	lostsync = 1;
#ifdef DEBUG
		if (debug) printf("Heath radioclock fault #%d No initial sync\n",
				  nerrors);
		else
#endif DEBUG
		{
			sprintf(message, "Heath radioclock fault #%d No initial sync\n",
				nerrors);
			syslog(LOG_ERR, message);
		}
		return(1);
	}

	/* The Heath clock sets tenths to ? if it loses sync */
	if (clockdata[9] == '?' && (nerrors++%ERR_RATE)==0) {
	  	lostsync = 1;
#ifdef DEBUG
		if (debug) printf("Heath radioclock fault #%d Lost sync\n",
				  nerrors);
		else
#endif DEBUG
		{
			sprintf(message, "Heath radioclock fault #%d Lost sync\n",
				nerrors);
			syslog(LOG_ERR, message);
		}
		return(1);
	}
	  
	if (lostsync == 1) {
	  	lostsync = 0;
#ifdef DEBUG
		if (debug) printf("Heath radioclock gained sync\n");
#endif DEBUG
		syslog(LOG_ERR, "Heath radioclock gained sync\n");
	}

	rtm->tm_hour = ((clockdata[0]-'0')*10) + (clockdata[1]-'0');
	rtm->tm_min = ((clockdata[3]-'0')*10) + (clockdata[4]-'0');
	rtm->tm_sec = ((clockdata[6]-'0')*10) + (clockdata[7]-'0');
	millis = (clockdata[9]-'0') * 100;
	rtm->tm_mon = ((clockdata[15]-'0')*10) + (clockdata[16]-'0') - 1;
	rtm->tm_mday = ((clockdata[18]-'0')*10) + (clockdata[19]-'0');
	rtm->tm_year = ((clockdata[21]-'0')*10) + (clockdata[22]-'0');

	/*
	 * Correct "hours" based on whether or not AM/PM mode is enabled.
	 * If clock is in 24 hour (military) mode then no correction is
	 * needed.
	 */
	if (clockdata[12] == 'M') { /* Map AM/PM time to Military */
		if (clockdata[11] == 'P') {
			if (rtm->tm_hour != 12) rtm->tm_hour += 12;
		} else {
			if (rtm->tm_hour == 12) rtm->tm_hour = 0;
		}
	}

	/* We cheat to figure out the day of the year.  The chance that
	   clock and system time will be on seperate days is so slim, that
	   we don't care.  If they do end up on seperate days, we'll just
	   say the clock is at fault for not giving us the day of the year. */
	mtm = gmtime(&mytime.tv_sec);
	if (mtm->tm_mday == rtm->tm_mday && mtm->tm_mon == rtm->tm_mon)
		rtm->tm_yday = mtm->tm_yday;
	else
		if ((nerrors++%ERR_RATE) == 0) {
#ifdef DEBUG
			if (debug)
				printf("Heath radioclock fault #%d can't figure yday\n",
				       nerrors);
			else
#endif DEBUG
			{
				sprintf(message,
					"Heath radioclock fault #%d can't figure yday\n",
					nerrors);
				syslog(LOG_ERR, message);
			}
			return(1);
		}
	
	/* The Heath clock year is dip switch selected.  We better check
	   this or the year will be incorrect every Jan 1. */
	if (rtm->tm_year != mtm->tm_year && (nerrors++%ERR_RATE)==0) {
#ifdef DEBUG
		if (debug) printf("Heath radioclock fault #%d Year mismatch\n",
				  nerrors);
		else
#endif DEBUG
		{
			sprintf(message,
				"Heath radioclock fault #%d Year mismatch\n",
				nerrors);
			syslog(LOG_ERR, message);
		}
		return(1);
	}

	if ((millis > 999 || rtm->tm_sec > 60 || rtm->tm_min > 60 ||
	     rtm->tm_hour > 23 || rtm->tm_yday > 365) && (nerrors++%ERR_RATE)==0) {
#ifdef DEBUG
		if (debug) printf("Heath radioclock bogon #%d: %dd %dh %dm %ds %dms\n",
			nerrors, rtm->tm_yday, rtm->tm_hour,
			rtm->tm_min, rtm->tm_sec, millis);
		else
#endif DEBUG
		sprintf(message, "Heath radioclock bogon #%d: %dd %dh %dm %ds %dms\n",
			nerrors, rtm->tm_yday, rtm->tm_hour,
			rtm->tm_min, rtm->tm_sec, millis);
		syslog(LOG_ERR, message);
		return(1);
	}

	mtm = gmtime(&mytime.tv_sec);
	diff =  reltime(rtm, millis*1000) - reltime(mtm, mytime.tv_usec);
#ifdef DEBUG
	if (debug > 1)
		printf("Heath clock time:  19%d day %03d %02d:%02d:%02d.%03d diff %.3f\n",
			rtm->tm_year, rtm->tm_yday, rtm->tm_hour,
			rtm->tm_min, rtm->tm_sec, millis, diff);
#endif DEBUG
	
	if (diff > (90*24*60*60.0) && (nerrors++%ERR_RATE)==0) {
#ifdef DEBUG
		if (debug)
			printf("Heath clock offset excessive (system 19%d/%d, clock 19%d/%d)\n",
				mtm->tm_year, mtm->tm_yday,
				rtm->tm_year, mtm->tm_yday);
		else
#endif DEBUG
		  syslog(LOG_ERR, "Heath clock offset excessive (system 19%d/%d, clock 19%d/%d)\n",
			 mtm->tm_year, mtm->tm_yday,
			 rtm->tm_year, mtm->tm_yday);
		return(1);
	}

	diff += (double)mytime.tv_sec + ((double)mytime.tv_usec/1000000.0);
	radiotime.tv_sec = diff;
	radiotime.tv_usec = (diff - (double)radiotime.tv_sec) * 1000000;
#ifdef DEBUG
	if (debug > 1) {
		printf("System time:       19%d day %03d %02d:%02d:%02d.%03d\n",
			mtm->tm_year, mtm->tm_yday, mtm->tm_hour,
			mtm->tm_min, mtm->tm_sec, mytime.tv_usec/1000);
	}
#endif DEBUG

	if (nerrors) {
#ifdef DEBUG
	  if (debug)
		  printf("Heath radioclock OK (after %d errors)\n", nerrors);
	  else
#endif DEBUG
	  syslog(LOG_ERR, "Heath radioclock OK (after %d errors)", nerrors);
	  nerrors = 0;
	}
	*tvpp = &radiotime;
	*otvpp = &mytime;
	return(0);
}

/*
 * Verify that the string looks like what we expect.
 * That would be:
 * HH:MM:SS.T     MM/DD/YY<CR>
 */
static verify(str, nr)
register	char	*str;
register	int	nr;
{
	int error = 0;
	int i;
	
	if (nr < WWV_STR_LEN)
		return(0);
	
	for(i=0; i < nr; i++) {
		str[i] &= 0177;	/* remove any parity */
		
		switch(i) {
		case 0: case 1: case 3: case 4: case 6: case 7: case 9:
		case 15: case 16: case 18: case 19: case 21: case 22:
			if (!isdigit(str[i]) && str[i] != '?')
				error++;
			break;
			
		case 2: case 5:
			if (str[i] != ':')
				error++;
			break;
			
		case 8:
			if (str[i] != '.')
				error++;
			break;
			
		case 17: case 20:
			if (str[i] != '/')
				error++;
			break;
			
		case 23:
			if (str[i] != '\r')
				error++;
			else
				str[i] = '\n';
		}
	}
	
	if (error) {
#ifdef DEBUG
		if (debug) printf("Heath radioclock format error\n"); else
#endif DEBUG
		if ((nerrors++%ERR_RATE) == 0)
			syslog(LOG_ERR, "Heath radioclock format error\n");
		return(0);
	}
	return(1);
}

static double
reltime(tm, usec)
register struct tm *tm;
register int usec;
{
	return(tm->tm_year*(366.0*24.0*60.0*60.0) +
	       tm->tm_yday*(24.0*60.0*60.0) +
	       tm->tm_hour*(60.0*60.0) +
	       tm->tm_min*(60.0) +
	       tm->tm_sec +
	       usec/1000000.0);
}
#endif HEATH
