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

#include <stdio.h>

extern int ltpid, ltvflag, ltqflag;	/* see ltgetopt() for these guys */
extern char *ltprogramname;


/*VARARGS10*/
ltprintf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
	char *fmt;
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10;
{
	if (ltvflag) {
		(void)printf("STATUS(%s:%d): ", ltprogramname, ltpid);
		(void)printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7,
			arg8, arg9, arg10);
		fflush(stdout);	/* we're testing - don't keep us in suspense */
	}
}



/*VARARGS10*/
lteprintf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
	char *fmt;
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10;
{
	/* Do it if "-v" or if neither specified as opposed to only when
	   "-v" is specified.
	*/
	if (!ltqflag) {
		(void)fprintf(stderr, "ERROR(%s:%d): ", ltprogramname, ltpid);
		(void)fprintf(stderr, fmt, arg1, arg2, arg3, arg4, arg5, arg6,
			 arg7, arg8, arg9, arg10);
		fflush(stderr);	/* shouldn't be necessary */
	}
}


ltprintusage()
{
	extern char usagestring[];	/* should be in the test program */

	if (usagestring[0])
		lteprintf("usage: %s %s\n", ltprogramname, usagestring);
	else
		lteprintf("usage: %s\n", ltprogramname);
	ltexit(1);
}
