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
static char sccsid[] = "@(#)xlink.c	1.5  com/lib/PW,3.1,8943 9/11/89 09:44:47";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS:  xlink
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

# include	"errno.h"

/*
 * FUNCTION: Interface to link(II).
 *
 * RETURN VALUE DESCRIPTIONS:
 *	Returns 0 on success,
 *	fatal() on failure.
 */
/*
	Interface to link(II) which handles all error conditions.
	Returns 0 on success,
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

xlink(f1,f2)
char *f1, *f2;
{
	extern errno;
	extern char Error[];

	if (link(f1,f2)) {
		if (errno == EEXIST || errno == EXDEV) {
#ifdef MSG
			catd = NLcatopen(MF_PW, 0);
			NLsprintf(Error, MSGSTR(XNOLINK,
				"cannot link `%s' to `%s' (ut%d)"),
				f2, f1, errno == EEXIST ? 20 : 21);
#else
			NLsprintf(Error,"cannot link `%s' to `%s' (ut%d)",
				f2,f1,errno == EEXIST ? 20 : 21);
#endif
			return(fatal(Error));
		}
		if (errno == EACCES)
			f1 = f2;
		return(xmsg(f1,"xlink"));
	}
	return(0);
}
