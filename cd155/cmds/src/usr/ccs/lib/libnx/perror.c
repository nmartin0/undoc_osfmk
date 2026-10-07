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
static char rcsid[] = "@(#)$RCSfile: perror.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:32:21 $";
#endif
/*
 * FUNCTIONS: perror
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * perror.c
 */

#include <stdio.h>

#include <string.h>
#include <errno.h>
extern int sys_nerr, write();
extern char *sys_errlist[];

/*
 * NAME:	perror
 *
 * FUNCTION:	perror writes a message to the standard error output
 *		describing the last error encountered by a system
 *		call or the last error encountered by a subroutine call
 *		that set 'errno'.
 *
 * NOTES:	Perror is very useful in describing a system error after
 *		a system call fails.
 *
 *		sys_nerr, errno and sys_errlist are all used
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void 	
perror(const char *s)
{
	int n = 0;
	char *c;		/* pointer to string to be printed	*/

	/*
	 * we assume here that 80 is big enough to hold our sprintf()
	 * down below.  if not, ints are extremely large!
	 * Also, it is assumed that the total message from perror() will not 
	 * exceed 1023 characters.
	 */
	char buf[80];

	if(errno >= 0 && errno < sys_nerr)
		c = sys_errlist[errno];
	else
		(void) sprintf(c = &buf[0], "Error %d occurred, unknown.", errno);
	if(s) {
		n = strlen(s);
	}
	if(n) {
		char *f;	/* pointer to final string to be printed */
		char fbuf[1024];
                (void) sprintf(f=&fbuf[0], "%s: %s", s, c);
                (void) fprintf(stderr, "%s\n", f);
	} else {
        	(void) fprintf(stderr, "%s\n", c);
	}
}
