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
static char rcsid[] = "@(#)$RCSfile: NLctime.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:32 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCTIME) Standard C Library Time Management Functions 
 *
 * FUNCTIONS: NLctime, NLasctime 
 *
 * ORIGINS: 27 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * sccsid[] = "NLctime.c 1.10  com/lib/c/time,3.1,9021 1/30/90 18:25:16";
 */

/*LINTLIBRARY*/
/*
 *  NLS-parameterized versions of ctime and asctime:
 *	NLctime(clk) just does the call NLasctime(localtime(clk)).
 *	NLasctime(tm) just makes the appropriate call to NLstrtime
 *	to get the current NLS equivalent of the traditional Unix
 *	date/time string.
 */
#include <time.h>

/*  Length of buffer to hold return value of NLasctime()
 */
#define BLEN	64

unsigned char *
#ifdef _NO_PROTO
NLctime(t)
long *t;
#else /* _NO_PROTO */
NLctime(long *t)
#endif /* _NO_PROTO */
{
	return(NLasctime(localtime(t)));
}

unsigned char *
NLasctime(struct tm *t)
{
	static unsigned char strbuf[BLEN];

	strftime((char *)strbuf, (size_t) BLEN, "%a %sD %X %Y\n\0", t);
	return(strbuf);
}
