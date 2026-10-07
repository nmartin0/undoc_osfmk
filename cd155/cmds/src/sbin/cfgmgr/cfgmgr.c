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
static char rcsid[] = "@(#)$RCSfile: cfgmgr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:08 $";
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strings.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <loader.h>
#include <AFdefs.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <locale.h>

#include "cfgmgr.h"
#include "cm.h"

/*
 *	CFGMGR:	static global data
 */
cmgr_common_t	CMGR;
void		cmgr_reset();
void		cmgr_mainexit();


/*
 *	CFGMGR:	command line usage
 */
void
cmgr_usage()
{
	fprintf(stderr, cm_msg(MSG_CFGMGR), CMGR.progname);
	exit(2);
}


void
cmgr_init(pname)
	char *	pname;
{
	/*
	 *	Initialize global data
	 */
        CMGR.progname = strrchr(pname,'/') ? strrchr(pname,'/') +1: pname;
	CMGR.database	= CFGMGR_DATABASE;
	CMGR.maxrec	= CFGMGR_MAXRECSIZ;
	CMGR.maxatr	= CFGMGR_MAXATRNUM;
	CMGR.dflg	= FALSE;
	CMGR.vflg	= FALSE;
	CMGR.fflg	= FALSE;
	CMGR.reset	= FALSE;
	CMGR.exitval	= 0;
	CMGR.pidfd	= -1;
}

/*
 *	CFGMGR:	option parsing and initialization
 */
void
cmgr_options(argc, argv)
	int	argc;
	char *	argv[];
{
	register int	c;
	register int	i;
	extern char *	optarg;
	extern int	optind;

	while ((c = getopt(argc, argv, "fvdc:R:A:")) != EOF) {
		switch (c) {
		case 'f':
			CMGR.fflg = TRUE;
			break;
		case 'v':
			CMGR.vflg = TRUE;
			break;
		case 'd':
			CMGR.dflg = TRUE;
			break;
		case 'c':
			CMGR.database = optarg;
			break;
		case 'R':
			if ((i=atoi(optarg)) > CFGMGR_MAXRECSIZ)
				CMGR.maxrec = i;
			break;
		case 'A':
			if ((i=atoi(optarg)) > CFGMGR_MAXATRNUM)
				CMGR.maxatr = i;
			break;
		case '?':
		default:
			cmgr_usage();
		}
	}
	if (optind != argc)
		cmgr_usage();
}


/*
 *	CFGMGR:	error log
 */
void
cfgmgr_log( int level, char * format, ...)
{
	va_list 	args;
        char    	message[BUFSIZ +1];
	static int	syslog_opened = FALSE;


	if (!syslog_opened) {
		openlog(CFGMGR_SYSLOG, CMGR.dflg ? (LOG_PID|LOG_CONS) : LOG_PID, 
			LOG_DAEMON);
		syslog_opened = TRUE;
	}

	va_start(args, format);
	vsprintf(message, format, args);
	va_end(args);

	if (CMGR.fflg && CMGR.dflg && stderr != NULL) {
		fprintf(stderr, message);
		fflush(stderr);
	}

	syslog(level, message);
}


/*
 *	CFGMGR:	lock pid run file
 */
void
cmgr_lock_pid_file()
{
	struct flock flock;
	char buf[32];

	/*
 	 *	Open pid file
	 */
	if ((CMGR.pidfd = open(CMGR_PID_PATHNAME, (O_CREAT|O_WRONLY),
		0644)) == -1) {
		cfgmgr_log(LOG_ERR, "%s: %s: %s\n", CMGR.progname,
			CMGR_PID_PATHNAME, strerror(errno));
		CMGR.exitval = 1;
		cmgr_mainexit();
	}

	/*
 	 *	Exclusive lock pid file
	 */
	flock.l_type = F_WRLCK;
	flock.l_whence = SEEK_SET;
	flock.l_start = 0;
	flock.l_len = 0;

	if (fcntl(CMGR.pidfd, F_SETLK, &flock) == -1) {
		cfgmgr_log(LOG_ERR, "%s: %s\n", CMGR.progname,
			cm_msg(MSG_ALREADYRUNNING));
		(void) close(CMGR.pidfd);
		CMGR.pidfd = -1;
		CMGR.exitval = 1;
		cmgr_mainexit();
	}

	/*
 	 *	Write PID to pid file (warning if we can't write PID out)
	 */
	(void)sprintf(buf, "%d\n", (int) getpid());

	if (write(CMGR.pidfd, buf, strlen(buf)) == -1) {
		cfgmgr_log(LOG_ERR, "%s: %s: %s\n", CMGR.progname,
			CMGR_PID_PATHNAME, strerror(errno));
	}
}


/*
 *	CFGMGR:	unlock pid run file, if we have it locked (opened)
 */
void
cmgr_unlock_pid_file()
{
	(void) close(CMGR.pidfd);
	if (CMGR.pidfd >= 0)
		(void) unlink(CMGR_PID_PATHNAME);
}


/*
 *	CFGMGR:	check priv
 */
cmgr_check_priv()
{
        if (getuid() && geteuid()) {
		fprintf(stderr, "%s: %s\n", CMGR.progname,
			cm_msg(MSG_MUSTBEROOT));
                exit(1);
        }
}




/*
 *	CFGMGR:	detach from controlling terminal
 */
void
cmgr_detach()
{
	register int	f;

	cfgmgr_log(LOG_INFO, cm_msg(MSG_DETACHING), CMGR.progname); 

	if (fork())
		exit(0);

	(void)setsid();

	for (f = 0; f < 10; f++)
		(void) close(f);
	(void) open("/dev/null", O_RDONLY);
	(void) open("/dev/null", O_WRONLY);
	(void) dup(1);
}


/*
 *	CFGMGR:	main exit point
 */
void
cmgr_mainexit()
{
	cfgmgr_log(LOG_ERR, "\r%s: %s\n", CMGR.progname, cm_msg(MSG_EXITING));
	cmgr_unlock_pid_file();
	exit(CMGR.exitval);
}

/*
 *	CFGMGR:	reset point
 */
void
cmgr_reset()
{
	CMGR.reset = TRUE;
	return;
}




/*
 *	CFGMGR:	main entry point
 */
main(argc, argv)
	int		argc;
	char *		argv[];
{
	int			rc;
	struct sigaction	sigact_reset;
	struct sigaction	sigact_abort;
	extern void		startup_mode();
	extern void		request_mode();

	cmgr_init(argv[0]);

	setlocale(LC_ALL, "");

	cmgr_check_priv();

	cmgr_options(argc, argv);

	cmgr_lock_pid_file();

	/*
	 *	Set to reread database on receit of SIGHUP
	 */
	sigact_reset.sa_handler = cmgr_reset;
	sigfillset(&sigact_reset.sa_mask);
	sigact_reset.sa_flags = 0;
	sigaction(SIGHUP, &sigact_reset, (struct sigaction *)NULL);

	/*
	 *	Abort process on receit of SIGINT or SIGQUIT
	 */
	sigact_abort.sa_handler = cmgr_mainexit;
	sigfillset(&sigact_abort.sa_mask);
	sigact_abort.sa_flags = 0;
	sigaction(SIGINT, &sigact_abort, (struct sigaction *)NULL);
	sigaction(SIGQUIT, &sigact_abort, (struct sigaction *)NULL);

	startup_mode();

	if (CMGR.fflg == FALSE)
	    	cmgr_detach();

	request_mode();

	cmgr_mainexit();
}


