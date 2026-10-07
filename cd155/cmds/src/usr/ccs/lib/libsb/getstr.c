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
/*  getstr --  prompt user for a string
 *
 *  Usage:  p = getstr (prompt,defalt,answer);
 *	char *p,*prompt,*defalt,*answer;
 *
 *  Getstr prints this message:  prompt  [defalt]
 *  and accepts a line of input from the user.  This line is
 *  entered into "answer", which must be a big char array;
 *  if the user types just carriage return, then the string
 *  "defalt" is copied into answer.
 *  Value returned by getstr is just the same as answer,
 *  i.e. pointer to result string.
 *  The default value is used on error or EOF in the standard input.
 */

#include <stdio.h>

char *getstr (prompt,defalt,answer)
char *prompt,*defalt,*answer;
{
	char defbuf[4000];
	register char *retval;

	fflush (stdout);
	fprintf (stderr,"%s  [%s]  ",prompt,defalt);
	fflush (stderr);
	strcpy (defbuf,defalt);
	retval = (char *) gets (answer);
	if (retval == NULL || *answer == '\0')  strcpy (answer,defbuf);
	if (retval == NULL)
	    return (retval);
	else
	    return (answer);
}
