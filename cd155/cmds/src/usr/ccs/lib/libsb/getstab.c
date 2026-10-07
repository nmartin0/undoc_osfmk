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
/*  getstab  --  ask user for string in table
 *
 *  Usage:  i = getstab (prompt,table,defalt)
 *	int i;
 *	char *prompt, **table, *defalt;
 *
 *  Getstab prints the messge:	prompt  [defalt]
 *  and asks the user to type in a line.  This input text
 *  is compared to all the strings in the table to see which
 *  (if any) it matches; the stablk() routine is used
 *  for the matching.  If the string is ambiguous or invalid
 *  (i.e. matches zero strings, or more than one), the cycle
 *  is repeated.  When a valid string is typed, the index
 *  of the string it matches is returned.  If the user just
 *  types carriage return, the default string is used for matching.
 *  The default is also used on error or EOF in standard input.
 *  The string table may be declared in this way:
 * 	char *table[] = {"string1","string2",...,0};
 *
 *  Originally by klg (Ken Greer) on IUS/SUS UNIX.
 */

#include <strings.h>
#include <stdio.h>

int getstab (prompt,table,defalt)
char *prompt, **table, *defalt;
{
	char input[200];
	register int ix;

	fflush (stdout);
	do {
		fprintf (stderr,"%s  [%s]  ",prompt, defalt);
		fflush (stderr);
		if (gets (input) == NULL)  strcpy (input,defalt);
		if (*input == '\0')  strcpy (input,defalt);
		ix = stablk (input,table,0);
	} 
	while (ix < 0);

	return (ix);
}
