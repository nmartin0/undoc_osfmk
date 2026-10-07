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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)xopen.c	1.5  com/lib/PW,3.1,8943 9/11/89 09:45:36";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: xopen
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

# include "errno.h"

/*
 * FUNCTION: Interface to open(II).
 *
 * RETURN VALUE DESCRIPTIONS:
 *	Returns file descriptor on success,
 *	fatal() on failure.
 */
/*
	Interface to open(II) which differentiates among the various
	open errors.
	Returns file descriptor on success,
	fatal() on failure.
*/


#ifdef MSG
#include "pw_msg.h"
nl_catd	catd;
#define MSGSTR(Num, Str) NLcatgets(catd, MS_PW, Num, Str)
#else
#define MSGSTR(Num, Str) Str
#endif

#ifndef	NLS
#define NLsprintf sprintf
#endif

xopen(name,mode)
char name[];
int mode;
{
	register int fd;
	extern int errno;
	extern char Error[];

	if ((fd = open(name,mode)) < 0) {
		if(errno == EACCES) {
#ifdef MSG
			catd = NLcatopen(MF_PW, 0);
#endif
			if(mode == 0)
#ifdef MSG
				NLsprintf(Error, MSGSTR(XNORD,
					"`%s' unreadable (ut5)"), name);
#else
				NLsprintf(Error,"`%s' unreadable (ut5)",name);
#endif
			else if(mode == 1)
#ifdef MSG
				NLsprintf(Error, MSGSTR(XNOWRT,
					"`%s' unwritable (ut6)"), name);
#else
				NLsprintf(Error,"`%s' unwritable (ut6)",name);
#endif
			else
#ifdef MSG
				NLsprintf(Error, MSGSTR(XNORDWRT,
					"`%s' unreadable or unwritable (ut7)"),
					name);
#else
				NLsprintf(Error,"`%s' unreadable or unwritable (ut7)",name);
#endif
			fd = fatal(Error);
		}
		else
			fd = xmsg(name,"xopen");
	}
	return(fd);
}
