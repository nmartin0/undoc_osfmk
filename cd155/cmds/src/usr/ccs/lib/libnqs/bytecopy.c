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
 
/*++ bytecopy.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/bytecopy.c,v $
 *
 * DESCRIPTION:
 *
 *	Copy exactly N bytes.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:23 $ $State: Exp $)
 * $Log: bytecopy.c,v $
 * Revision 1.3  1994/11/19  02:26:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:31  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:19  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:04  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*** bytecopy
 *
 *
 *	bytecopy():
 *	Copy exactly N bytes.
 */
void bytecopy (to, from, n)
register char *to;
register char *from;
register int n;
{
	while (n--) {
		*to++ = *from++;
	}
}
