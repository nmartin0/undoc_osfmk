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
static char rcsid[] = "@(#)$RCSfile: strerror.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:08:35 $";
#endif
/*
 * FUNCTIONS: strerror
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * strerror.c	1.10  com/lib/c/str,3.1,9013 11/28/89 16:12:03
 */

/*
 *
 * FUNCTION: Maps the error number in errnum to an error message string.
 *
 * NOTES:    The ANSI Programming Language C standard requires this routine.
 *
 * PARAMETERS:
 *	     int errnum - error number to be associated with error message
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to the error message
 *	     string associated with the error number.
 */
/*LINTLIBRARY*/
#ifdef MSG
#include "libc_msg.h"
#include <limits.h>
#include <mesg.h>
#endif
#ifdef _THREAD_SAFE
#include <errno.h>
#endif

extern int sys_nerr;
extern char *sys_errlist[];

#ifndef _THREAD_SAFE
#ifdef MSG
char strerrbuf[NL_TEXTMAX];
#else
char strerrbuf[80];
#endif
#endif

#ifdef _THREAD_SAFE
int
strerror_r(int errnum, char *strerrbuf, int buflen)
#else
char 	*
strerror(int errnum)
#endif
{
	char *c;
#ifdef MSG
	nl_catd	catd;
#endif

#ifdef _THREAD_SAFE
#ifdef MSG
	if ((strerrbuf == NULL) || (buflen < NL_TEXTMAX)) {
#else
	if ((strerrbuf == NULL) || (buflen < 80)) {
#endif
		seterrno(EINVAL);
		return(-1);
	}
#endif

	c = strerrbuf;

#ifdef MSG
	catd = catopen(MF_LIBC, 0);
	if((errnum > 0) && (errnum < sys_nerr))
		c = catgets(catd, MS_LIBC, errnum, sys_errlist[errnum]);
	else
		(void) sprintf(c,catgets(catd, MS_LIBC, M_PERROR, "Error %d "), errnum);
	catclose(catd);
#else
	if((errnum > 0) && (errnum < sys_nerr))
		c = sys_errlist[errnum];
	else
		(void) sprintf(c,"Error %d ", errnum);
#endif

#ifdef _THREAD_SAFE
	if (c != strerrbuf)
		strcpy(strerrbuf, c);
	return(0);
#else
	return(c);
#endif

}
