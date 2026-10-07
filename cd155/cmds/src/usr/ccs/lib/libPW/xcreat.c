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
static char sccsid[] = "@(#)xcreat.c	1.6  com/lib/PW,3.1,9008 11/27/89 09:59:25";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: xcreat
 *
 * ORIGINS: 3 27
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
 */


/*
 * FUNCTION: Create a file.
 *
 * RETURN VALUE DESCRIPTIONS:
 *	Returns file descriptor on success,
 *	fatal() on failure.
 */

#ifdef MSG
#include "pw_msg.h"
nl_catd	catd;
#define MSGSTR(Num, Str) NLcatgets(catd, MS_PW, Num, Str)
#else
#define MSGSTR(Num, Str) Str
#endif

# include	"sys/types.h"
# include	"macros.h"

/*
	"Sensible" creat: write permission in directory is required in
	all cases, and created file is guaranteed to have specified mode
	and be owned by effective user.
	(It does this by first unlinking the file to be created.)
	Returns file descriptor on success,
	fatal() on failure.
*/

#ifndef NLS
#define NLsprintf sprintf
#endif

xcreat(name,mode)
char *name;
int mode;
{
	register int fd;
	char d[sizeof Error];
	extern char *dname();

	copy(name,d);
	if (!exists(dname(d))) {
#ifdef MSG
		catd = NLcatopen(MF_PW, 0);
		NLsprintf(Error, MSGSTR(XNODIR, 
			"directory `%s' does not exist (ut1)"), d);
#else
		NLsprintf(Error,"directory `%s' does not exist (ut1)",d);
#endif
		return(fatal(Error));
	}
	unlink(name);
	if ((fd = creat(name,mode)) >= 0)
		return(fd);
	return(xmsg(name,"xcreat"));
}
