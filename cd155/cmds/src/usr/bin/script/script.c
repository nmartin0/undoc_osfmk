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
static char rcsid[] = "@(#)$RCSfile: script.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:29 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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
 *
 * script.c	1.6  com/cmd/sh,3.1,9021 1/17/90 17:18:29
 */
/*
 * script.c - makes a typescript of everything printed on the terminal.
 *
 */
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <termios.h>

#include <sys/time.h>
#include <sys/file.h>
#include <sys/wait.h>

#include "script_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_SCRIPT,n,s) 

#ifdef CTIME
#undef CTIME
#endif

unsigned char	*NLctime();
#define CTIME NLctime

struct	termios tt;
struct	winsize win;

char	*shell;
FILE	*fscript;
int	master;
int	slave;
int	child;
int	subchild;
char	*fname;
int	finish(void);

struct	tchars tc;
struct	ltchars lc;
int	lb;
int	l;
char	line[12];
int	aflg;

main(argc, argv)
	int argc;
	char *argv[];
{
	extern char *optarg;
	extern int optind;
	int ch;
	struct sigaction	chld;

	strcpy(line, "/dev/ptyXX");
	setlocale(LC_ALL, "");
	catd = catopen(MF_SCRIPT,0);
	while ((ch = getopt(argc, argv, "a")) != EOF) {
		switch ((char)ch) {
		case 'a':
			aflg++;
			break;
		case '?':
		default:
			fprintf(stderr,
			    MSGSTR(USAGE, "usage: script [ -a ] [ file ]\n"));
			exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 0)
		fname = argv[0];
	else
		fname = "typescript";
	if ((fscript = fopen(fname, aflg ? "a" : "w")) == NULL) {
		perror(fname);
		fail();
	}
	shell = getenv("SHELL");
	if (shell == NULL)
		shell = "/bin/sh";

	getmaster();
	printf(MSGSTR(SCRIPTSTART, "Script started, file is %s\n"), fname);
	fixtty();

	sigaction(SIGCHLD, (struct sigaction *)0, &chld);
	chld.sa_handler = (void (*)(int))finish ;
	sigaction(SIGCHLD, &chld, (struct sigaction *)0);
	child = fork();
	if (child < 0) {
		perror("fork");
		fail();
	}
	if (child == 0) {
		subchild = child = fork();
		if (child < 0) {
			perror("fork");
			fail();
		}
		if (child)
			dooutput();
		else 
		 	doshell();
	}
	doinput();
}

/*
 * NAME: doinput
 *
 * FUNCTION:  Gets input from the keyboard and puts it into a buffer.
 *
 */

doinput()
{
	char ibuf[BUFSIZ];
	int cc;

	(void) fclose(fscript);
	while ((cc = read(0, ibuf, (unsigned) BUFSIZ)) > 0)
		(void) write(master, ibuf, (unsigned)cc);
	done();
}

/*
 * NAME: finish 
 *
 * FUNCTION:  Waits for all children to die before exiting. 
 *
 */

finish(void)
{
	int status;
	register int pid;
	register int die = 0;

	while ((pid = wait3(&status, WNOHANG, 0)) > 0)
		if (pid == child)
			die = 1;

	if (die)
		done();
}

/*
 * NAME:  dooutput
 *
 * FUNCTION:  Writes terminal output to file.
 *
 */

dooutput()
{
	time_t tvec;
	char obuf[BUFSIZ];
	int cc;

	(void) close(0);
	tvec = time((time_t *)0);
	fprintf(fscript, MSGSTR(STARTSCR, "Script started on %s"), CTIME(&tvec));
	for (;;) {
		cc = read(master, obuf, sizeof (obuf));
		if (cc <= 0)
			break;
		(void) write(1, obuf, (unsigned)cc);
		(void) fwrite((void *)obuf, (size_t)1, (size_t)cc, fscript);
	}
	done();
}

/*
 * NAME:  doshell
 *
 * FUNCTION:  Run a shell to execute commands in.
 *
 */

doshell()
{
	getslave();
	(void) close(master);
	(void) fclose(fscript);
	(void) dup2(slave, 0);
	(void) dup2(slave, 1);
	(void) dup2(slave, 2);
	(void) close(slave);
	execl(shell, "sh", "-is", 0);
	perror(shell);
	fail();
}

/*
 * NAME:  fixtty
 *
 * FUNCTION:  Set up the terminal.
 *
 */

fixtty()
{
	struct termios sbuf;

	sbuf = tt;

	sbuf.c_iflag &= ~(INLCR|IUCLC|ISTRIP|IXON|BRKINT|ICRNL|PARMRK);
	sbuf.c_oflag &= ~(ONLCR|OXTABS);
	sbuf.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN); 
	sbuf.c_cc[VMIN] = 1;
	if (tcsetattr((int)fileno(stdin), TCSAFLUSH, &sbuf) < 0)
		perror("tcsetattr 1");
}

/*
 * NAME:  fail
 *
 * FUNCTION:  Kill the program on failure.
 *
 */

fail()
{

	(void) kill(0, SIGTERM);
	done();
}

/*
 * NAME:  done
 *
 * FUNCTION:  Cleanup on exit. Close output file and respond to
 *            user with exit message.
 *
 */

done()
{
	time_t tvec;
 
	if (subchild) {
		tvec = time((time_t *)0);
		fprintf(fscript,MSGSTR(ENDSCR, "\nscript done on %s"), CTIME(&tvec));
		(void) fclose(fscript);
		(void) close(master);
	} else {
		if (tcsetattr((int)fileno(stdin), TCSANOW, &tt) < 0)
			perror("tcsetattr 3");
		printf(MSGSTR(DONESCRIPT, "Script done, file is %s\n"), fname);
	}
	exit(0);
}

/*
 * NAME:  getmaster
 *
 * FUNCTION:  Get a controller pty.
 *
 */

getmaster()
{

	char *pty, *bank, *cp;
	struct stat stb;

	pty = &line[strlen("/dev/ptyp")];
	for (bank = "pqrs"; *bank; bank++) {
		line[strlen("/dev/pty")] = *bank;
		*pty = '0';
		if (stat(line, &stb) < 0)
		{
			break;
		}
		for (cp = "0123456789abcdef"; *cp; cp++) {
			*pty = *cp;
			master = open(line, O_RDWR);
			if (master >= 0) {
				char *tp = &line[strlen("/dev/")];
				int ok;

				/* verify slave side is usable */
				*tp = 't';
				ok = access(line, R_OK|W_OK) == 0;
				*tp = 'p';
				if (ok) {
					if (tcgetattr((int)fileno(stdin), &tt) < 0) { 
						perror("tcgetattr");
						fail();
					}
					if (ioctl(fileno(stdin), TIOCGWINSZ, (char *)&win) < 0) {
						perror("ioctl TIOCGWINSZ");
					}
					return;
				}
				(void) close(master);
			}

		}
	}
	fprintf(stderr, "Out of pty's\n");
	fail();
}

/*
 * NAME:  getslave
 *
 * FUNCTION:  Get a slave pty.
 *
 */

getslave()
{

	line[strlen("/dev/")] = 't';
	slave = open(line, O_RDWR);
	if (slave < 0) {
		perror(line);
		fail();
	}
	if (tcsetattr(slave, TCSANOW, &tt) < 0) {
		perror("tcsetattr 2");
	}
	if (ioctl(slave, TIOCSWINSZ, (char *)&win) < 0)
	{
		perror("ioctl TIOCSWINSZ");
	}
	(void) setsid();
	(void) ioctl(slave, TIOCSCTTY, 0);
}
