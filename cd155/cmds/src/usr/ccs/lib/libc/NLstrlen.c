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
static char rcsid[] = "@(#)$RCSfile: NLstrlen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:58 $";
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
 * FUNCTIONS: NLstrlen, NCstrlen, NLstrdlen, NLcplen
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
 * sccsid[] = "NLstrlen.c        1.11  com/lib/c/str,3.1,9021 1/18/90 09:51:46";
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 * NAME: NLstrlen
 *
 * FUNCTION: Counts the number of bytes in the string pointed to by s before
 *	the terminating null character.  Works with strings containing embedded
 *	NLS code points and is identical to strlen.
 *
 * RETURN VALUE DESCRIPTION: An integer, the number of bytes in s before the
 *	terminating null character.
 */
int
#ifdef _NO_PROTO
NLstrlen(s)
register unsigned char *s;
#else /* _NO_PROTO */
NLstrlen(register unsigned char *s)
#endif /* _NO_PROTO */
{
	register unsigned char *s0 = s + 1;

	while (*s++ != '\0')
		;
	return (s - s0);
}

/*
 * NAME: NCstrlen
 *
 * FUNCTION: Counts the number of NLchars in the string pointed to by s before
 *	the terminating null character.  The string must be of type NLchar.
 *
 * RETURN VALUE DESCRIPTION: An integer, the number of NLchars in s before the
 *	terminating null character.
 */
int
#ifdef _NO_PROTO
NCstrlen(s)
register NLchar *s;
#else /* _NO_PROTO */
NCstrlen(register NLchar *s)
#endif /* _NO_PROTO */
{
	register NLchar *s0 = s + 1;

	while (*s++ != 0)
		;
	return (s - s0);
}

/*
 * NAME: NLstrdlen(s)
 *
 * FUNCTION: Counts the number of code points in the string pointed to by s
 *	before the terminating null character.  It can be used to compute the
 *	"display length" of a string.  This differs from NLstrlen in that
 *	multi-byte chars are one character long while being two bytes long.
 *
 * RETURN VALUE DESCRIPTION: An integer, the number of code points in s before
 *	the terminating null character.
 */
int
#ifdef _NO_PROTO
NLstrdlen(s)
register unsigned char *s;
#else /* _NO_PROTO */
NLstrdlen(register unsigned char *s)
#endif /* _NO_PROTO */
{
	register int c;

	for (c = 0; *s != '\0'; s += NLchrlen(s), ++c)
		;
	return (c);
}
#ifdef KJI

/*
 * NAME: NLcplen(s)
 *
 * FUNCTION: Counts the number of code points in the string pointed to by s
 *	before the terminating null character.  
 *
 * NOTES: Identical to NLstrdlen, included only for compatibility.
 *
 * RETURN VALUE DESCRIPTION: An integer, the number of code points in s before
 *	the terminating null character.
 */
int
#ifdef _NO_PROTO
NLcplen(s)
char *s;
#else /* _NO_PROTO */
NLcplen(char *s)
#endif /* _NO_PROTO */
{
	return (NLstrdlen(s));
}

#include <wchar.h>
#undef wstrlen

int
#ifdef _NO_PROTO
wstrlen(s)
register wchar_t *s;
#else /* _NO_PROTO */
wstrlen(register wchar_t *s)
#endif /* _NO_PROTO */
{
	return (NCstrlen(s));
}
#endif /* KJI */
