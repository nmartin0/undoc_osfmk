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
static char rcsid[] = "@(#)$RCSfile: NLstrpbrk.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:04 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: NLstrpbrk, NCstrpbrk
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
 * sccsid[] = "NLstrpbrk.c       1.9  com/lib/c/str,3.1,9021 1/18/90 09:53:06";
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 *  Return ptr to first occurrence of any character from `brkset'
 *  in the character string `string'; NULL if none exists.  Two versions
 *  here:  NLstrpbrk (works with ASCII containing embedded NLS code
 *  points) and NCstrpbrk (works with NLchars).
 */

#define	NULL	0

unsigned char *
#ifdef _NO_PROTO
NLstrpbrk(string, brkset)
register unsigned char *string;
char *brkset;
#else /* _NO_PROTO */
NLstrpbrk(register unsigned char *string, char *brkset)
#endif /* _NO_PROTO */
{
	register NLchar *p;
	NLchar buf[NLCSETMAX], sc;
	NLchar *nlbrkset = _NCbufstr(brkset, buf, NLCSETMAX);
	register int i;

	for (; ; ) {
		if (!*string)
			break;
		i = NCdec(string, &sc);
		for(p = nlbrkset; *p != 0 && *p != sc; ++p)
			;
		if(*p != 0) {
			_NCfreebuf(nlbrkset);
			return(string);
		}
		string += i;
	}
	_NCfreebuf(nlbrkset);
	return(NULL);
}

NLchar *
#ifdef _NO_PROTO
NCstrpbrk(string, brkset)
register NLchar *string;
char *brkset;
#else /* _NO_PROTO */
NCstrpbrk(register NLchar *string, char *brkset)
#endif /* _NO_PROTO */
{
	register NLchar *p;
	NLchar buf[NLCSETMAX];
	NLchar *nlbrkset = _NCbufstr(brkset, buf, NLCSETMAX);

	do {
		for(p = nlbrkset; *p != 0 && *p != *string; ++p)
			;
		if(*p != 0) {
			_NCfreebuf(nlbrkset);
			return(string);
		}
	}
	while(*string++);
	_NCfreebuf(nlbrkset);
	return(NULL);
}

#ifdef KJI
#include <wchar.h>

wchar_t *
#ifdef _NO_PROTO
wstrpbrk(string, brkset)
register wchar_t *string, *brkset;
#else /* _NO_PROTO */
wstrpbrk(register wchar_t *string, register wchar_t *brkset)
#endif /* _NO_PROTO */
{
	register wchar_t *p;
	wchar_t buf[NLCSETMAX];
	wchar_t *nlbrkset = _NCbufstr(brkset, buf, NLCSETMAX);

	do {
		for(p = brkset; *p != 0 && *p != *string; ++p)
			;
		if(*p != 0) 
			return(string);
	}
	while(*string++);
	return(NULL);
}
#endif
