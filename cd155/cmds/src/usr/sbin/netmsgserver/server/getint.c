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
static char rcsid[] = "@(#)$RCSfile: getint.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:10:58 $";
#endif

/*  getint --  prompt user for int
 *
 *  Usage:  i = getint (prompt,min,max,defalt)
 *	int i,min,max,defalt;
 *	char *prompt;
 *
 *  Getint prints the message:  prompt  (min to max)  [defalt]
 *  and accepts a line of input from the user.  If the input
 *  is not null or numeric, an error message is printed; otherwise,
 *  the value is converted to an int (or the value "defalt" is
 *  substituted if the input is null).  Then, the value is
 *  checked to ensure that is lies within the range "min" to "max".
 *  If it does not, an error message is printed.  As long as
 *  errors occur, the cycle is repeated; when a legal value is
 *  entered, this value is returned by getint.
 *  On error or EOF in the standard input, the default is returned.
 */

#include <stdio.h>
#include <ctype.h>

int getint (prompt,min,max,defalt)
int min,max,defalt;
char *prompt;
{
	char input [200];
	register char *p;
	register int i,err;

	fflush (stdout);
	do {

		fprintf (stderr,"%s  (%d to %d)  [%d]  ",prompt,min,max,defalt);
		fflush (stderr);

		if (gets (input) == NULL) {
			i = defalt;
			err = (i < min || max < i);
		}
		else {
			err = 0;
			for (p=input; *p && (isdigit(*p) || *p == '-' || *p == '+'); p++) ;
	
			if (*p) {		/* non-numeric */
				err = 1;
			} 
			else {
				if (*input)	i = atol (input);
				else		i = defalt;
				err = (i < min || max < i);
			}
		}

		if (err) fprintf (stderr,"Must be a number between %d and %d\n",
		min,max);
	} 
	while (err);

	return (i);
}
