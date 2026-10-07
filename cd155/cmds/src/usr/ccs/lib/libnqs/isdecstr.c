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
 
/*++ isdecstr.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/isdecstr.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine if a string of length n characters contains
 *	ASCII decimal digit characters and nothing but ASCII
 *	decimal digit characters.
 *
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:02 $ $State: Exp $)
 * $Log: isdecstr.c,v $
 * Revision 1.3  1994/11/19  02:27:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:50  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:54  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:35  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*** isdecstr
 *
 *
 *	int isdecstr():
 *
 *	Returns:
 *		1: if the specified string of length n characters
 *		   contains decimal digits and nothing but decimal digits;
 *		0: otherwise.
 */
int isdecstr (name, len)
register char *name;
register int len;
{
	if (len <= 0) return (0);
	while (len && *name >= '0' && *name <= '9') {
		len--;
		name++;
	}
	if (len == 0) return (1);
	return (0);
}
