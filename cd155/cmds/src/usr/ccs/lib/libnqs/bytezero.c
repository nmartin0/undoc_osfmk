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
 
/*++ bytezero.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/bytezero.c,v $
 *
 * DESCRIPTION:
 *
 * 	Zero-out exactly N bytes.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 29, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1995/01/24 01:10:27 $ $State: Exp $)
 * $Log: bytezero.c,v $
 * Revision 1.5  1995/01/24  01:10:27  davidl
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
 * Revision 1.4  1994/11/19  02:26:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/07  00:17:18  kremenek
 *  Reviewer: davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s):  cmds_libs/src/usr/ccs/lib/libnqs/bytezero.c
 *
 * Revision 1.2  1992/10/09  20:15:33  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:20  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:16  hender
 * Sterling version 4/22/87
 * 
 *
 */

#ifdef SDSC
#include <stdio.h>
#include <malloc.h>
#include <sys/fcntl.h>

/*** console_write()
 *
 *
 *      void console_write():
 *
 *      Writing a message to /dev/console
 */
void console_write(message)
char *message;
{
int ttyfd, timeofday;
char *sTmp;

        if ((ttyfd = open("/dev/console", 
                          O_WRONLY || O_NDELAY || O_SYNC)) >= 0) {
                if (fcntl(ttyfd, F_SETFL, FNDELAY) >= 0) {
                        sTmp = (char *)malloc(2048);

                        sTmp[0] = 0;
                        strcpy(sTmp, message);

                        time (&timeofday);
                        strcat(sTmp, ctime(&timeofday));

                        strcat(sTmp, "\r");

                        (void) write(ttyfd, sTmp, strlen(sTmp));

                        free(sTmp);
                }
                (void) close(ttyfd);
        }
}
#endif

/*** bytezero
 *
 *
 *	bytezero():
 *	Zero-out exactly N bytes.
 */
void bytezero (to, n)
register char *to;
register int n;
{
	while (n--) {
		*to++ = '\0';
	}
}
