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
static char sccsid[] = "@(#)xmsg.c	1.6  com/lib/PW,3.1,9008 11/27/89 09:59:09";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: xmsg
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
 * FUNCTION: Print an error message based on errno.
 *
 * RETURN VALUE DESCRIPTIONS:
 *		fatal()
 */
/*
	Call fatal with an appropriate error message
	based on errno.  If no good message can be made up, it makes
	up a simple message.
	The second argument is a pointer to the calling functions
	name (a string); it's used in the manufactured message.
*/


#ifdef MSG
#include "pw_msg.h"
nl_catd	catd;
#define MSGSTR(Num, Str) NLcatgets(catd, MS_PW, Num, Str)
#else
#define MSGSTR(Num, Str) Str
#endif

# include	"errno.h"
# include	"sys/types.h"
# include	"macros.h"

#ifndef	NLS
#define NLsprintf sprintf
#endif

xmsg(file,func)
char *file, *func;
{
	register char *str;
	char buf[sizeof Error];
	extern int errno, sys_nerr;
	extern char *sys_errlist[];
	extern char *dname();

#ifdef MSG
	catd = NLcatopen(MF_PW, 0);
#endif
	switch (errno) {
	case ENFILE:
#ifdef MSG
		str = MSGSTR(XTABFULL, "file table full (ut3)");
#else
		str = "file table full (ut3)";
#endif
		break;
	case ENOENT:
#ifdef MSG
		NLsprintf(str = Error, MSGSTR(XNOFILE, 
			"`%s' does not exist (ut4)"), file);
#else
		NLsprintf(str = Error,"`%s' does not exist (ut4)", file);
#endif
		break;
	case EACCES:
		copy(file,buf);
#ifdef MSG
		NLsprintf(str = Error, MSGSTR(XNODWRT,
			"directory `%s' unwritable (ut2)"), dname(buf));
#else
		NLsprintf(str = Error,"directory `%s' unwritable (ut2)",
			dname(buf));
#endif
		break;
	case ENOSPC:
#ifdef MSG
		str = MSGSTR(XNOSPC, "no space! (ut10)");
#else
		str = "no space! (ut10)";
#endif
		break;
	case EFBIG:
#ifdef MSG
		str = MSGSTR(XTOOBIG, "file too big (ut8)");
#else
		str = "file too big (ut8)";
#endif
		break;
	default:
		if ((unsigned)errno < sys_nerr)
			str = sys_errlist[errno];
		else
#ifdef MSG
			NLsprintf(str = buf, MSGSTR(XERRNO, "errno = %d"),
				errno);
		NLsprintf(Error, MSGSTR(XFUNC, "%s, function = `%s' (ut11)"),
			str, func);
#else
			NLsprintf(str = buf,"errno = %d",errno);
		NLsprintf(Error,"%s, function = `%s' (ut11)", str, func);
#endif
		str = Error;
		break;
	}
	return(fatal(str));
}
