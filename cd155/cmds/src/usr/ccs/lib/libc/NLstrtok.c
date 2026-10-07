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
static char rcsid[] = "@(#)$RCSfile: NLstrtok.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:06 $";
#endif

/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: NLstrtok, NCstrtok
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
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
 * sccsid[] = "NLstrtok.c	1.13  com/lib/c/str,3.1,9021 4/26/90 15:45:41";
 */

#include <string.h>
#include <NLchar.h>

/*
 *  Uses strpbrk and strspn to break string into tokens on sequentially
 *  subsequent calls.  Returns NULL when no non-separator characters
 *  remain.  `Subsequent' calls are calls with first argument NULL.
 *  Two versions here:  NLstrtok (works with ASCII containing embedded
 *  NLS code points) and NCstrtok (works with NLchars).
 */

#define	NULL	0

unsigned char *
#ifdef _NO_PROTO
NLstrtok(string, sepset)
unsigned char	*string, *sepset;
#else /* _NO_PROTO */
NLstrtok(unsigned char *string, unsigned char *sepset)
#endif /* _NO_PROTO */
{
	register unsigned char	*p, *q, *r;
	static unsigned char	*savept;

	/*first or subsequent call*/

	p = (string == NULL) ? savept : string;

	if(p == 0)		/* return if no tokens remaining */
		return(NULL);

	q = p + NLstrspn(p, sepset);	/* skip leading separators */

	if(*q == '\0')		/* return if no tokens remaining */
		return(NULL);

	if((r = NLstrpbrk(q, (char *)sepset)) == NULL)	/* move past token */
		savept = 0;	/* indicate this is last token */
	else {
		*r = '\0';
		savept = ++r;
	}
	return(q);
}

NLchar *
#ifdef  _NO_PROTO
NCstrtok(string, sepset)
NLchar *string;
unsigned char *sepset;
#else /* _NO_PROTO */
NCstrtok(NLchar *string, unsigned char *sepset)
#endif /* _NO_PROTO */
{
	register NLchar	*p, *q, *r;
	static NLchar *savept;

	/*first or subsequent call*/

	p = (string == NULL) ? savept : string;

	if(p == 0)		/* return if no tokens remaining */
		return(NULL);

	q = p + NCstrspn(p, sepset);	/* skip leading separators */

	if(*q == '\0')		/* return if no tokens remaining */
		return(NULL);

	if((r = NCstrpbrk(q, (char *)sepset)) == NULL)	/* move past token */
		savept = 0;	/* indicate this is last token */
	else {
		*r = '\0';
		savept = ++r;
	}
	return(q);
}

#ifdef _KJI
#include <wstring.h>

wchar_t *
wstrtok(string, sepset)
wchar_t *string, *sepset;
{
	register wchar_t	*p, *q, *r;
	static wchar_t *savept;

	/*first or subsequent call*/

	p = (string == NULL) ? savept : string;

	if(p == 0)		/* return if no tokens remaining */
		return(NULL);

	q = p + wstrspn(p, sepset);	/* skip leading separators */

	if(*q == '\0')		/* return if no tokens remaining */
		return(NULL);

	if((r = wstrpbrk(q, sepset)) == NULL)	/* move past token */
		savept = 0;	/* indicate this is last token */
	else {
		*r = '\0';
		savept = ++r;
	}
	return(q);
}
#endif
