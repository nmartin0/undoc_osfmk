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
 
/*++ logdaemon.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/logdaemon.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains all of the code for the NQS log daemon.
 *	The log daemon is execd over a child of the NQS local daemon.
 *
 *
 *	    File descriptor #0 is used to receive messages and
 *	    logfile requests from the NQS local daemon.
 *
 *	    File descriptor #1 is used to write messages to the
 *	    original stdout destination of the NQS local daemon.
 *
 *	    File descriptor #2 is used to respond to logfile
 *	    requests from the NQS local daemon.
 *	
 *
 *	This module reads lines of input from stdin, and echoes them
 *	appropriately to the original stdout of the NQS local daemon,
 *	and/or to the NQS log file.
 *
 *	Each line read from stdin MUST begin with a special 2-character
 *	code as discussed below (or must only consist of a newline
 *	character):
 *
 *
 *	    1.	If the first 2 characters of the line are: "D$",
 *		then the rest of the line is written only to the NQS
 *		logfile, with the prepended phrase:
 *
 *			fprintf (file, "%s(DEBUG): ", Nqslog_prefix);
 *
 *
 *	    2.	If the first 2 characters of the line are: "E$",
 *		then the rest of the line is written to the original
 *		stdout file of the NQS daemon, and to the NQS logfile,
 *		with the prepended phrase:
 *
 *			fprintf (file, "%s(ERROR): ", Nqslog_prefix);
 *
 *
 *	    3.	If the first 2 characters of the line are: "F$",
 *		then the rest of the line is written to the original
 *		stdout file of the NQS local daemon, and to the NQS
 *		logfile, with the prepended phrase:
 *
 *			fprintf (file, "%s(FATAL): ", Nqslog_prefix);
 *
 *
 *	    4.	If the first 2 characters of the line are: "I$",
 *		then the rest of the line is written to the original
 *		stdout file of the NQS local daemon, and to the NQS
 *		logfile, with the prepended phrase:
 *
 *			fprintf (file, "%s(INFO):  ", Nqslog_prefix);
 *
 *
 *	    5.	If the first 2 characters of the line are: "L$",
 *		then the rest of the line is written only to the NQS
 *		logfile, with the prepended phrase:
 *
 *			fprintf (file, "%s(LOG):   ", Nqslog_prefix);
 *
 *
 *	    6.	If the first 2 characters of the line are: "W$",
 *		then the rest of the line is written to the original
 *		stdout file of the NQS local daemon, and to the NQS
 *		logfile, with the prepended phrase:
 *
 *			fprintf (file, "%s(WARN):  ", Nqslog_prefix);
 *
 *		is echoed as appropriate.
 *
 *
 *	    7.	If the first 2 characters of the line are: "$$",
 *		then all of the following characters up to the term-
 *		inating newline character ('\n'), are interpreted
 *		as the new path to be defined for the NQS logfile.
 *
 *		NO NQS SERVER MUST EVER, CHANGE THE NQS LOGFILE!!!!!
 *		THIS ACTION MUST ONLY BE PERFORMED BY THE NQS LOCAL DAEMON.
 *
 *
 *
 *	Lines of zero length (lines consisting only of a newline character),
 *	are simply ignored.  This characteristic is exploited when the
 *	upp_setlogfil() function tells this process to change to a new
 *	logfile....
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 29, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:52:29 $ $State: Exp $)
 * $Log: logdaemon.c,v $
 * Revision 1.4  1994/11/19  02:52:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/07  00:58:55  kremenek
 *  Reviewer: davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s):  cmds_libs/src/usr/lib/nqs/logdaemon.c
 *
 * Revision 1.2  1992/10/09  22:24:18  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:18  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:01:50  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)logdaemon.c	1.2 (logdaemon.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <sys/types.h>			/* Get time_t definition */
#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <time.h>
#include <fcntl.h>			/* File open/create flags */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#include <sys/file.h>			/* File open/create flags */
#else
BAD SYSTEM TYPE
#endif
#endif
#include <signal.h>
#include <errno.h>

#ifdef SDSC
#include <syslog.h>
#endif

extern char *asciierrno();		/* Return ASCII errno */
extern void bufstdout();		/* Block buffer stdout */
extern char *fmttime();			/* Format time */
extern struct tm *localtime();		/* Get local time */
extern time_t time();			/* Get time since Jan 1, 1970 */

static char *Nqslog_prefix = "NQS";	/* NQS log message prefix */
static FILE *logfile = NULL;		/* Logfile; Not null if log */
 					/* file defined */

/*** main
 *
 *
 *	int main ():
 *	NQS logdaemon.
 */
int main (argc, argv)
int argc;
char *argv[];
{
	static char badlog[] = "%s(ERROR): Bad log message follows.\n";

	char message [BUFSIZ+1];	/* Size of largest possible single */
					/* output line from an NQS process */
					/* +1. */
	void get_message();		/* Get next message packet line */
	void setlogfile();		/* Change log file */
	void writetime();		/* Write condensed time to logfile */


	/*
	 *  Make sure that standard output goes through block buffering.
	 *  We do this so that groups of messages from NQS can appear
	 *  indivisibly on the console, without getting broken in half
	 *  (or smaller pieces) by other programs which ALSO write to
	 *  the console.
	 */
	bufstdout();
	/*
	 *  Disable certain signals for safety and paranoia sake.
	 */
	signal (SIGHUP, SIG_IGN);
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
	signal (SIGTERM, SIG_IGN);
#if	BSD42 | BSD43 | ULTRIX
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	signal (SIGUSR1, SIG_IGN);
	signal (SIGUSR2, SIG_IGN);
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Loop to echo messages to the appropriate destinations
	 *  until the NQS local daemon, and all NQS servers and shepherd
	 *  processes exit.
	 */
	for (;;) {
		get_message (message);	/* Read the next message */
		/*
		 *  Another message line has been read.
		 */
		if (message [0] != '\0' && message [1] == '$') {
			/*
			 *  Message line may have the required
			 *  message level, or operator.
			 */
			switch (message [0]) {
			case '$':	/* New logfile request */
				setlogfile (message+2);
				break;
			case 'D':	/* Message level = DEBUG */
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, "%s(DEBUG): %s",
						 Nqslog_prefix, message+2);
				}
				else {
					printf ("%s(DEBUG): %s",
						Nqslog_prefix, message+2);
				}
				break;
			case 'E':	/* Message level = ERROR */
				printf ("%s(ERROR): %s", Nqslog_prefix,
					message+2);
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, "%s(ERROR): %s",
						 Nqslog_prefix, message+2);
				}
#ifdef SDSC
                        (void)syslog(LOG_ERR,
                        "NQS(ERROR): ",message+2);
                        (void)syslog(LOG_ERR | LOG_DAEMON,                     
                        "NQS(ERROR): ",message+2);
#endif
				break;
			case 'F':	/* Message level = FATAL */
				printf ("%s(FATAL): %s", Nqslog_prefix,
					message+2);
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, "%s(FATAL): %s",
						 Nqslog_prefix, message+2);
				}
#ifdef SDSC
                        (void)syslog(LOG_CRIT,
                        "NQS(FATAL): ",message+2);
                        (void)syslog(LOG_CRIT | LOG_DAEMON,                     
                        "NQS(FATAL): ",message+2);
#endif
				break;
			case 'I':	/* Message level = INFO  */
				printf ("%s(INFO):  %s", Nqslog_prefix,
					message+2);
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, "%s(INFO):  %s",
						 Nqslog_prefix, message+2);
				}
				break;
			case 'L':	/* Message level = LOG */
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, "%s(LOG):   %s",
						 Nqslog_prefix, message+2);
				}
				else {
					printf ("%s(LOG):   %s",
						Nqslog_prefix, message+2);
				}
				break;
			case 'W':	/* Message level = WARN */
				printf ("%s(WARN):  %s", Nqslog_prefix,
					message+2);
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, "%s(WARN):  %s",
						 Nqslog_prefix, message+2);
				}
				break;
			default:
				printf (badlog, Nqslog_prefix);
				fputs (message, stdout);
				if (logfile != NULL) {
					writetime();
					fprintf (logfile, badlog,
						 Nqslog_prefix);
					fputs (message, logfile);
				}
				break;
			}
			fflush (stdout);
			if (logfile != NULL) fflush (logfile);
		}
		else if (message [0] != '\n') {
			/*
			 *  No message level specified.
			 */
			printf (badlog, Nqslog_prefix);
			fputs (message, stdout);
			if (logfile != NULL) {
				writetime();
				fprintf (logfile, badlog, Nqslog_prefix);
				fputs (message, logfile);
			}
			fflush (stdout);
			if (logfile != NULL) fflush (logfile);
		}
	}
}


/*** get_message
 *
 *
 *	void get_message():
 *
 *	Get the next log message line, placing the result in the
 *	caller specified buffer.  This line WILL have a newline
 *	character at the end.
 *
 *	This function will exit(), if there aren't any processes
 *	with the NQS log message packet pipe open for writing.
 */
static void get_message (buffer)
char buffer [BUFSIZ+1];			/* Line buffer */
{
	void writetime();		/* Write condensed local time to */
					/* log file */

	static char badpk[] = "%s(WARN):  Bad NQS log msg follows.\n";
	static char exiting[] = "%s(INFO):  NQS log process exiting.\n";
	static char toobig[] = "%s(INFO):  Too big or contains null byte.\n";
	static char truncate[] = "%s(WARN):  Possibly truncated.\n";

	register char *cp;

	if (fgets (buffer, BUFSIZ, stdin) == NULL) {
		/*
		 *  No more messages to be logged.  All NQS
		 *  processes (including the NQS daemon), are
		 *  no longer present.
		 */
		printf (exiting, Nqslog_prefix);
		fflush (stdout);
		if (logfile != NULL) {
			writetime();
			fprintf (logfile, exiting, Nqslog_prefix);
			fflush (logfile);
		}
		exit (0);		/* Exit peacefully. */
	}
	/*
	 *  A message was read from an NQS process.
	 */
	cp = buffer;
	while (*cp && *cp != '\n') cp++;
	if (*cp == '\0') {
		/*
		 *  Packet is too large.
		 */
		printf (badpk, Nqslog_prefix);
		printf (toobig, Nqslog_prefix);
		printf (truncate, Nqslog_prefix);
		fflush (stdout);
		if (logfile != NULL) {
			writetime();
			fprintf (logfile, badpk, Nqslog_prefix);
			writetime();
			fprintf (logfile, toobig, Nqslog_prefix);
			writetime();
			fprintf (logfile, truncate, Nqslog_prefix);
			fflush (logfile);
		}
		*cp++ = '\n';	/* Truncate message packet */
		*cp = '\0';	/* Null terminate */
	}
}


/*** setlogfile
 *
 *
 *	void setlogfile():
 *
 *	Switch error and message logging to a new logfile.
 *	If the new logfile cannot be opened for writing, then
 *	do not switch!
 *
 *	A message is left on the old logfile if the switch is
 *	successful (or unsuccessful).
 *
 *	A success/fail code is returned directly to the NQS
 *	daemon.
 */
void setlogfile (newlogname)
register char *newlogname;		/* Name of new logfile. */
{
	void writetime();		/* Write condensed local time to */
					/* log file */

	static char
	fail[] = "%s(ERROR): New logfile request failed.\n";

	static char
	unable[]="%s(INFO):  Unable to open/create new logfile for writing.\n";

	static char
	switching[]="%s(INFO):  Switching to new logfile: %s.\n";

	static char
	newlog[] = "%s(INFO):  New logfile.\n";

	static char
	timestamp[] = "%s(INFO):  Time=%s.\n";

	time_t current_time;		/* Current time */
	register int newfd;		/* New error file descriptor */
	register char *cp;

	cp = newlogname;
	while (*cp && *cp != '\n') cp++;/* Look for newline character */
	*cp = '\0';			/* Zap trailing newline */
	if ((newfd = open (newlogname, O_APPEND | O_CREAT | O_TRUNC
				     | O_WRONLY, 0644)) == -1) {
		/*
		 *  Unable to open/create the new logfile.  Do NOT switch to
		 *  bad logfile!
		 */
		printf (fail, Nqslog_prefix);
		printf (unable, Nqslog_prefix);
		printf ("%s(INFO):  %s.\n", Nqslog_prefix, asciierrno());
		fflush (stdout);
		if (logfile != NULL) {
			writetime();
			fprintf (logfile, fail, Nqslog_prefix);
			writetime();
			fprintf (logfile, unable, Nqslog_prefix);
			writetime();
			fprintf (logfile, "%s(INFO):  %s.\n", Nqslog_prefix,
				 asciierrno());
			fflush (logfile);
		}
		write (2, "\0", 1);	/* Report failure to NQS daemon */
	}
	else {
		/*
		 *  We have succeeded.  In this case, we do NOT write
		 *  anything to stdout, only to the log file.
		 */
		if (logfile != NULL) {
			writetime();
			fprintf (logfile, switching, Nqslog_prefix, newlogname);
			fflush (logfile);
			close (fileno (logfile));
			fcntl (newfd, F_DUPFD, fileno (logfile));
						/* Make logfile output */
						/* refer to new logfile */
			close (newfd);		/* Close temporary error */
						/* file descriptor. */
		}
		else logfile = fdopen (newfd, "w");
		time (&current_time);		/* Get current time */
		writetime();
		fprintf (logfile, newlog, Nqslog_prefix);
		writetime();
		fprintf (logfile, timestamp, Nqslog_prefix,
			 fmttime (&current_time));
		fflush (logfile);
		write (2, "\01", 1);	/* Report success to NQS daemon */
	}
}


/*** writetime
 *
 *
 *	void writetime():
 *	Write compact form of time to the NQS log file.
 */
static void writetime()
{
	register struct tm *tm;
	time_t current_time;

	time (&current_time);		/* Get current time */
	tm = localtime (&current_time);
	fprintf (logfile, "%2d/%2d/%2d %02d:%02d ", tm->tm_mon+1,
		 tm->tm_mday, tm->tm_year, tm->tm_hour, tm->tm_min);
}
