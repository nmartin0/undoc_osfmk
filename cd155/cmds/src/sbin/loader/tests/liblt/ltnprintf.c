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

extern int ltpid, ltvflag, ltqflag;	/* see ltgetopt() */
extern char *ltprogramname;

/*VARARGS10*/
ltnprintf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
	char *fmt;
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10;
{
	if (!ltqflag) {
		(void)printf("STATUS(%s:%d): ", ltprogramname, ltpid);
		(void)printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7,
			arg8, arg9, arg10);
		fflush(stdout);	/* we're testing - don't keep us in suspense */
	}
}
