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
/*
 * Copyright (c) 1990 SecureWare, Inc.
 *   All rights reserved.
 *
 * Library routine to print security error message.
 * If security error is not defined, print regular error number instead.
 */

#ident "@(#)psecerror.c	1.1 22:32:38 5/28/90 SecureWare, Inc."

#include "libsecurity.h"

#define MESSAGELENGTH	32

void
psecerror(s)
	char *s;
{
	char unknown_buf[MESSAGELENGTH];
	unsigned n;
	char *c;
	extern int sys_nsecerr;
	extern int sec_errno;
	extern char *sys_secerrlist[];
#ifdef MSG
	extern int sys_secerrnums[];
#endif
	
	if (sec_errno == 0) {
		perror(s);
		return;
	}

	if (sec_errno < sys_nsecerr)
		c = MSGSTR(sys_secerrnums[sec_errno],
				sys_secerrlist[sec_errno]);
	else {
		c = unknown_buf;
		sprintf(c, MSGSTR(PSECERROR_1, "Security error %3d occurred."),
			sec_errno);
	}

	if (s && (n = strlen(s))) {
		write(2, s, n);
		write(2, ": ", 2);
	}

	write(2, c, strlen(c));
	write(2, "\n", 1);
}
