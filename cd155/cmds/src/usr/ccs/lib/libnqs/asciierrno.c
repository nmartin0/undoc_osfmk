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
 
/*++ asciierrno.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/asciierrno.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Format present errno value in ASCII diagnostic string.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 27, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:18 $ $State: Exp $)
 * $Log: asciierrno.c,v $
 * Revision 1.3  1994/11/19  02:26:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:23  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:14  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:40  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>

extern int errno;			/* System call error# */


/*** asciierrno
 *
 *
 *	char *asciierrno():
 *	Return formatted ASCII value of present errno.
 */
char *asciierrno()
{
	static char formatbuffer [20];

	sprintf (formatbuffer, "Errno value: %1d", errno);
	return (formatbuffer);
}
