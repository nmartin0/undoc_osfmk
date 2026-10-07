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
 
/*++ nqs_abort.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_abort.c,v $
 *
 * DESCRIPTION:
 *
 *	Abort NQS daemon execution.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1995/01/24 01:10:47 $ $State: Exp $)
 * $Log: nqs_abort.c,v $
 * Revision 1.5  1995/01/24  01:10:47  davidl
 * Cleaned up console_write() so it works the way it was (apparently)
 * originally intended to work.  Also cleaned up some of the calls to
 * console_write() (for example, added missing close parenthesis).
 *
 *  Reviewer: doyle
 *  Risk: Low
 *  Benefit or PTS #: 11648
 *  Testing: Developer testing, EATs
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/bytezero.c
 *             cmds_libs/src/usr/lib/nqs/netdaemon.c
 *             cmds_libs/src/usr/lib/nqs/nqs_abort.c
 *             cmds_libs/src/usr/lib/nqs/nqs_boot.c
 *             cmds_libs/src/usr/lib/nqs/nqs_main.c
 *
 * Revision 1.4  1994/11/19  02:52:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/07  00:46:22  kremenek
 *  Reviewer:  davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/netdaemon.c, nqs_abort.c,
 *             nqs_boot.c, nqs_mai.c, nqs_main.c
 *
 * Revision 1.2  1992/10/09  22:24:57  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:49  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:33  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"
#include "nqsxvars.h"			/* NQS global vars */

extern char *asciierrno();		/* Return ASCII version of errno */
extern int errno;			/* System call error number */

#ifdef SDSC
#include <sys/fcntl.h>
#include <syslog.h>
#endif

/*** nqs_abort
 *
 *
 *	void nqs_abort():
 *
 *	Abort execution of the NQS daemon writing a message to
 *	the NQS log process.
 */
void nqs_abort ()
{
	if (errno) printf ("I$%s.\n", asciierrno());
	printf ("I$Execution aborted.\n");
	fflush (stdout);
        fflush (stdout);

#ifdef SDSC
        (void)syslog(LOG_CRIT,
        "NQS execution aborted (please see log files in /usr/spool/nqs/log.d directory).");
        (void)syslog(LOG_CRIT | LOG_DAEMON,
        "NQS execution aborted (please see log files in /usr/spool/nqs/log.d directory).");
        console_write("NQS execution aborted (please see log files in /usr/spool/nqs/log.d directory). ");
#endif

	exit (1);
}
