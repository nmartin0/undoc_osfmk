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
 
/*++ shlexefai.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/shlexefai.c,v $
 *
 * DESCRIPTION:
 *
 *	Inform the shepherd process of a shell process created to
 *	execute an NQS batch request, that the execve() system
 *	call to invoke the actual shell failed.
 *
 *	This program MUST run as setuid to root program, in order
 *	to signal the NQS shepherd process running as root.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	January 22, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:36 $ $State: Exp $)
 * $Log: shlexefai.c,v $
 * Revision 1.3  1994/11/19  02:53:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:27:05  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:59:12  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:37  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <signal.h>

/*
 *	External functions and variables:
 */
extern int atoi();
#ifndef SGI
extern void (*signal())();
#endif

/*** main
 *
 *
 *	Invocation from a failed shell execve() call is as follows:
 *
 *	  $(NQS_LIBEXE)/shlexefai <ASCII-errno-code>
 *
 */
main (argc, argv)
int argc;
char *argv[];
{
	int parent_pid;			/* Process-id of NQS shepherd */

	parent_pid = getppid();
	if (parent_pid > 1) {
		/*
		 *  The NQS shepherd process for this failed shell
		 *  process has not exited.
		 */
		kill (parent_pid, SIGPIPE);
					/* Send a SIGPIPE signal to the */
					/* NQS shepherd process */
	}
	_exit (atoi (argv [1]));	/* Exit */
}
