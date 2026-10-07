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
 
/*++ parseserv.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/parseserv.c,v $
 *
 * DESCRIPTION:
 *
 *	This module parses a server invocation command line, placing
 *	the broken-out arguments into the specified argv[] array.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 31, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:31 $ $State: Exp $)
 * $Log: parseserv.c,v $
 * Revision 1.3  1994/11/19  02:27:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:06  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:31  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:11  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* NQS constants and data types */

extern char *strcpy();			/* String copy command */
extern int strlen();			/* String length */

/*** parseserv
 *
 *
 *	int parseserv ():
 *	Parse server command line into argv[].
 *
 *	Returns:
 *		0: if successful.
 *	       -1: if there are too many arguments, or the server
 *		   specification is too long.
 */
int parseserv (servercmd, argv)
register char *servercmd;		/* Server command */
char *argv [MAX_SERVERARGS+1];		/* Argument pointers */
{
	char *scanwhite();		/* Scan whitespace */

	static char serverspec [MAX_SERVERNAME+1];
					/* Server command and arguments */
	register int i;			/* Index */
	register char *cp;		/* Character pointer */

	if (strlen (servercmd) > MAX_SERVERNAME) return (-1);
	strcpy (serverspec, servercmd);	/* Copy the server command into */
	i = 0;				/* the static buffer area */
	cp = serverspec;
	do {
		cp = scanwhite (cp);	/* Scan white space */
		if (*cp) {
			/*
			 *  We have an argument.
			 */
			argv [i] = cp;	/* Record start of argument */
			do {		/* Scan argument */
				cp++;
			} while (*cp > ' ');
			if (*cp) {
				/*
				 *  Null terminate argument.
				 */
				*cp = '\0';
				cp++;
			}
		}
		i++;
	} while (*cp && i < MAX_SERVERARGS);
	argv [i] = (char *) 0;		/* Null terminate argv[] list */
	cp = scanwhite (cp);
	if (*cp != '\0') {
		/*
		 *  Too many server arguments in server command line.
		 */
		return (-1);
	}
	return (0);			/* Parse was successful */
}


/*** scanwhite
 *
 *
 *	char *scanwhite():
 *
 *	Scan white space returning pointer to first non-white character
 *	or null character.  This function only works for 7-bit ASCII
 *	characters.
 */
static char *scanwhite (ch)
register char *ch;
{
	while (*ch && *ch <= ' ') ch++;
	return (ch);
}
