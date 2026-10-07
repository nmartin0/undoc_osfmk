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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pty.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:38:21 $";
#endif
/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
pty.c	1.2 (Berkeley) 6/29/90";
 */


#include <sys/file.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <grp.h>
#include <errno.h>

openpty(amaster, aslave, name, termp, winp)
	int *amaster, *aslave;
	char *name;
	struct termios *termp;
	struct winsize *winp;
{
	char line[ sizeof ("/dev/ptyXX") ];
	register char *cp1, *cp2;
	register master, slave, ruid, ttygid;
	struct group *gr;
	extern errno;

	strcpy (line, "/dev/ptyXX");
	if ((gr = getgrnam("tty")) != NULL)
		ttygid = gr->gr_gid;
	else
		ttygid = -1;
	ruid = getuid();

	for (cp1 = "pqrstuvwxy"; *cp1; cp1++) {
		line[8] = *cp1;
		for (cp2 = "0123456789abcdef"; *cp2; cp2++) {
			line[9] = *cp2;
			if ((master = open(line, O_RDWR, 0)) == -1) {
#ifdef DEBUG
				printf ("Openpty, line %s, errno = %d, %s\n", line,
					errno, strerror (errno));
#endif
				if (errno == ENXIO || errno == ENOENT)
					return (-1);	/* out of ptys */
			} else {
				line[5] = 't';
				(void) chown(line, ruid, ttygid);
				(void) chmod(line, 0620);
				(void) revoke(line);
				if ((slave = open(line, O_RDWR, 0)) != -1) {
					*amaster = master;
					*aslave = slave;
					if (name)
						strcpy(name, line);
					if (termp)
						(void) tcsetattr(slave, 
							TCSAFLUSH, termp);
					if (winp)
						(void) ioctl(slave, TIOCSWINSZ, 
							(char *)winp);
#ifdef DEBUG
					printf ("Openpty, found pty %s\n", line);
#endif
					return (0);
				}
				(void) close(master);
				line[5] = 'p';
			}
		}
	}
	errno = ENOENT;	/* out of ptys */
	return (-1);
}

forkpty(amaster, name, termp, winp)
	int *amaster;
	char *name;
	struct termios *termp;
	struct winsize *winp;
{
	int master, slave, pid;

	if (openpty(&master, &slave, name, termp, winp) == -1)
		return (-1);
	switch (pid = fork()) {
	case -1:
		return (-1);
	case 0:
		/* 
		 * child
		 */
		(void) close(master);
		(void) login_tty(slave);
		return (0);
	}
	/*
	 * parent
	 */
	*amaster = master;
	(void) close(slave);
	return (pid);
}
