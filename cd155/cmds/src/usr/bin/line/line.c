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
static char rcsid[] = "@(#)$RCSfile: line.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:15 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * line.c	1.3  com/cmd/sh,3.1,9013 11/27/89 21:06:55
 */

#include <locale.h>
#include <unistd.h>

#define	LSIZE	512		/* line buffer size		*/

char nl = '\n';			/* newline character		*/
int EOF;			/* End Of File flag		*/
char readc();			/* character read function	*/

/*
 * NAME:	line
 *
 * SYNTAX:	line
 *
 * FUNCTION:	line - read one line from standard input
 *
 * NOTES:	This program reads a single line from the standard input
 *		and writes it on the standard output.  It is probably most
 *		useful in conjunction with the Bourne shell.
 *
 * RETURN VALUE DESCRIPTION:	1 on end-of-file, else 0
 */

int
main()
{
	register char c;		/* last character read	*/
	char line[LSIZE];		/* line buffer		*/
	register char *linep, *linend;	/* pointers within line	*/

	(void) setlocale (LC_ALL, "");

	EOF = 0;			/* initialize eof flag	*/
	linep = line;			/* init current pointer	*/
	linend = line + LSIZE;		/* point to end of line	*/

	/*
	 * loop reading 'till we hit eof or eol ...
	 */
	while ((c = readc()) != nl)
	{
		/*
		 * filled line up?
		 */
		if (linep == linend)
		{
			/*
			 * yep, write it & start over...
			 */
			(void) write (1, line, (unsigned)LSIZE);
			linep = line;
		}
		/*
		 * save the character we read
		 */
		*linep++ = c;
	}

	/*
	 * write last buffer (if anything there)
	 */
	if (linep > line)
		write (1, line, (unsigned)(linep-line));

	/*
	 * write newline
	 */
	write(1, &nl, (unsigned)1);

	/*
	 * exit 1 on eof, else 0
	 */
	exit(EOF);

	/* NOTREACHED */
}


/*
 * NAME:	readc
 *
 * FUNCTION:	readc - read a character and return it
 *
 * NOTES:	Readc reads a character from standard input (fd = 0)
 *		and returns it.  On end-of-file, a newline is returned
 *		and the end-of-file flag is turned on.
 *
 * DATA STRUCTURES:	EOF is set on end of file
 *
 * RETURN VALUE DESCRIPTION:	the character read is returned.  on
 *		end of file 'nl' is returned (newline character)
 */

char
readc()
{
	char c;

	if (read (0, &c, (unsigned)1) != 1) {
		EOF = 1;
		c = nl;
	}

	return (c);
}
