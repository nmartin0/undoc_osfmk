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
 
/*++ mkdefault.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/mkdefault.c,v $
 *
 * DESCRIPTION:
 *
 *	Manufacture a default output file name.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:16 $ $State: Exp $)
 * $Log: mkdefault.c,v $
 * Revision 1.3  1994/11/19  02:27:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:19  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:12  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:32  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*** mkdefault
 *
 *
 *	void mkdefault():
 *	Make a default output file name.
 */
void mkdefault (cp, path, reqname, seqno, suffix_char)
register char *cp;		/* Destination for name */
register char *path;		/* Path for file name */
register char *reqname;		/* Req name */
long seqno;			/* Req sequence number */
char suffix_char;		/* Suffix character: 'e' or 'o' */
{
	register int i;

	if (*path != '\0') {
		do {
			*cp++ = *path++;
		} while (*path);
		*cp++ = '/';
	}
	for (i=0; i < 7 && *reqname != '\0'; i++) {
		*cp++ = *reqname++;
	}
	*cp++ = '.';
	*cp++ = suffix_char;
	sprintf (cp, "%1ld", seqno);
}
