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
 
#ifndef lint
static char *rcsid = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libcs/getbool.c,v 1.2 1994/11/19 02:15:30 mtm Exp $";
#endif


/*
 * $Log: getbool.c,v $
 * Revision 1.2  1994/11/19  02:15:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/03  23:54:12  stans
 * Initial revision
 *
 *
 */

/*  getbool -- ask user a yes/no question
 *
 *  Usage:  i = getbool (prompt, defalt);
 *
 *  Example:  do {...} while (getbool ("More?",1));
 *
 *  Prints prompt string, asks user for response.  Defalt is
 *  0 (no) or 1 (yes), and is used if user types just carriage return,
 *  or on end-of-file or error in the standard input.
 *
 *  HISTORY
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now uses stderr for output.
 *
 * 23-Oct-82  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added code to return default if gets returns NULL.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.  Possible changes for the future:  accept "t" (true)
 *	and "f" (false), 0 and 1, etc.
 *
 */

#include <stdio.h>
#include <c.h>

int getbool (prompt, defalt)
char *prompt;
int defalt;
{
	register int valu;
	register char ch;
	char input [100];

	fflush (stdout);
	if (defalt != TRUE && defalt != FALSE)  defalt = TRUE;
	valu = 2;				/* meaningless value */
	do {
		fprintf (stderr,"%s  [%s]  ",prompt,(defalt ? "yes" : "no"));
		fflush (stderr);			/* in case it's buffered */
		if (gets (input) == NULL) {
			valu = defalt;
		}
		else {
			ch = *input;			/* first char */
			if (ch == 'y' || ch == 'Y')		valu = TRUE;
			else if (ch == 'n' || ch == 'N')	valu = FALSE;
			else if (ch == '\0')		valu = defalt;
			else fprintf (stderr,"Must begin with 'y' (yes) or 'n' (no).\n");
		}
	} 
	while (valu == 2);			/* until correct response */
	return (valu);
}
