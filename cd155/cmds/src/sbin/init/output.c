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
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs 
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 3, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM Confidential Restricted
 * when combined with the aggregated modules for this product) OBJECT CODE
 * ONLY SOURCE MATERIALS (C) COPYRIGHT International Business Machines Corp.
 * 1989 All Rights Reserved 
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp. 
 *
 */

#include	<sys/limits.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<stdio.h>
#include	<sys/termios.h>
#include	<sys/fcntl.h>
#include	<ctype.h>
#define TTYDEFCHARS
#include	"init.h"
#if defined(NLS) || defined(KJI)
#include	<sys/NLchar.h>
#endif

#ifdef	MSG
#include	"init_msg.h"
#endif


/*************************/
/****    switchcon    ****/
/*************************/

switchcon(int sig)
{
	extern pid_t    own_pid;/* changed by staffan@osf.de */
	extern int      fd_syscon;
	extern int      switchcon(int);
	struct sigaction action;

#ifdef	XDEBUG
	debug("We have entered switchcon().\n");
#endif

	action.sa_flags = 0;	/* no flags */
	action.sa_handler = SIG_IGN;

	sigaction(sig, &action, (struct sigaction *) NULL);

	/*
	 * If this is the first time a <del> has been typed on the
	 * /dev/syscon, then issue the console change command to
	 * /dev/syscon.  Also reestablish file pointers.
	 */
	if (fd_syscon != -1) {
		reset_console();
		openconsole();

		/*
		 * Set fd_syscon to -1 so that we ignore any deletes from it
		 * in the future as far as changing /dev/console to /dev/syscon.
		 */
		fd_syscon = -1;
	}
	action.sa_handler = (void (*) (int)) switchcon;

	sigaction(sig, &action, (struct sigaction *) NULL);
}

struct termios  curterm;	/* current terminal state */

/**************************/
/****    openconsole    ****/
/**************************/

/*
 * "openconsole" opens stdin, stdout, and stderr, making sure
 * that their file descriptors are 0, 1, and 2, respectively.
 */

openconsole()
{
	int             fd;

	close(0);
	close(1);
	close(2);

	fd = open(CONSOLE, O_RDWR);
	dup(fd);
	dup(fd);
	setbuf(stdin, (char *) NULL);
	setbuf(stdout, (char *) NULL);
	setbuf(stderr, (char *) NULL);

	/*
	 * Make sure the hangup on last close is off.  Then restore
	 * the modes that were on console when the signal was sent.
	 */
	tcgetattr(fd, &curterm);
	termios.c_cflag &= ~HUPCL;
	tcsetattr(fd, TCSANOW, &termios);
	return;
}


/********************************/
/****    get_ioctl_console    ****/
/********************************/

/* "get_ioctl_console" initializes the /dev/console settings */

get_ioctl_console()
{
	/* get the default internal states */
	memcpy(&termios, &dflt_termios, sizeof(dflt_termios));
	memcpy(&termios.c_cc, ttydefchars, sizeof(dflt_termios.c_cc));
}

/****************************/
/****    reset_console    ****/
/****************************/

/*
 * "reset_console" changes /dev/console to /dev/syscon and puts
 * the default ioctl setting back into /etc/ioctl.syscon and
 * the incore arrays.
 */

reset_console()
{
}

/***********************/
/****    console    ****/
/***********************/

/*
 * "console" forks a child if it finds that it is the main "init"
 * and outputs the requested message to the system console.
 * Note that the number of arguments passed to "console" is
 * determined by the print format.
 */
console(format, arg1, arg2, arg3, arg4)
	char           *format;
	int             arg1, arg2, arg3, arg4;
{
	extern struct proc *efork();
	struct proc *p;
	extern pid_t    own_pid;/* changed by staffan@osf.de */
	char            outbuf[BUFSIZ];
	extern int      childeath(void);
#ifdef	UDEBUG
	extern int      abort();
	struct sigaction action;
#endif

#ifdef	XDEBUG
	debug("entered console().\n");
#endif
	/*
	 * Are we the original "init"?  If so, fork a child to do the
	 * printing for us.
	 */
	if (own_pid == SPECIALPID) {
		while ((p = efork(NULLPROC, NOCLEANUP)) == NO_ROOM)
			timer(5);
		if (p == NULLPROC) {
#ifdef	UDEBUG
			action.sa_flags = 0;
			action.sa_handler = (void (*) (int)) abort;
			sigaction(SIGUSR1, &action, (struct sigaction *) NULL);
			sigaction(SIGUSR2, &action, (struct sigaction *) NULL);
#endif
			/*
			 * Close the standard descriptors and open the system
			 * console. 
			 */
			openconsole();	/* open proper file descriptors for
					 * console */
			setbuf(stdout, outbuf);

			/* Output the message to the console. */
			NLfprintf(stdout, "\nINIT: ");
			NLfprintf(stdout, format, arg1, arg2, arg3, arg4);
			fflush(stdout);
			tcsetattr(1, TCSANOW, &curterm);
			exit(0);

			/* The parent waits for the message to complete. */
		}
		else {
			while (waitproc(p) == FAILURE);
			freeproc(p);
		}

		/*
		 * If we are some other "init", then just print directly to
		 * the standard output.
		 */
	}
	else {
		openconsole();
		setbuf(stdout, outbuf);
		NLfprintf(stdout, "\nINIT: ");
		NLfprintf(stdout, format, arg1, arg2, arg3, arg4);
		fflush(stdout);
		tcsetattr(1, TCSANOW, &curterm);
	}
#ifdef	ACCTDEBUG
	debug(format, arg1, arg2, arg3, arg4);
#endif
#ifdef	XDEBUG
	debug("Exit-> console()\n");
#endif
}

#ifdef DEBUGGER

/*********************/
/****    debug    ****/
/*********************/

debug(format, arg1, arg2, arg3, arg4, arg5, arg6)
	char           *format;
	int             arg1, arg2, arg3, arg4, arg5, arg6;
{
	static FILE	*fp;
	static	int	opened=0;
	extern int      errno;

#ifndef	i860
	if ((fp = fopen(DBG_FILE, "a+")) == NULL) {
		int	errnum;

		errnum = errno;
		console("Can't open \"%s\".  errno: %d\n", DBG_FILE, errnum);
		return;
	}
	fprintf(fp, format, arg1, arg2, arg3, arg4, arg5, arg6);
	fclose(fp);
#else
	if ( opened == 0 ) {
		if ((fp = fopen(DBG_FILE, "w")) == NULL) {
			int	errnum;

			errnum = errno;
			console("Can't open \"%s\".  errno: %d\n", DBG_FILE, errnum);
			return;
		}
		opened=1;
	}
	fprintf(fp, format, arg1, arg2, arg3, arg4, arg5, arg6);
	fflush( fp );
#endif
}

/*****************/
/**** vetchar ****/
/*****************/
/*
 * This little routine takes a char pointer input and makes
 * sure that each character is printable (non-control).
 */
char *
vetchar(id)
	register char  *id;
{
	static char     answer[12];
	register char  *ptr;
	register int    i;

#ifdef	XDEBUG
	debug("We have entered vetchar().\n");
#endif
	for (i = 6, ptr = answer; --i >= 0; id++) {
		if (isprint(*id) == 0) {
			*ptr++ = '^';
			*ptr++ = *id + 0100;
		}
		else
			*ptr++ = *id;
	}
	*ptr++ = '\0';
	return (answer);
}
#endif				/* DEBUGGER */
