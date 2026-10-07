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
static char rcsid[] = "@(#)$RCSfile: NLstrchr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:55 $";
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
 * FUNCTIONS: NLstrchr, NCstrchr
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
 * sccsid[] = "NLstrchr.c  1.12  com/lib/c/str,3.1,9021 3/27/90 13:43:23";
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 *  Return the ptr in sp at which the character c appears, NULL if not found.
 *  Two versions: NLstrchr() for char strings and NCstrchr() for NLchar strings.
 *  In both cases, the character being looked for must be an NLchar.
 */

#ifdef NULL
#undef NULL
#define	NULL	0
#endif /* NULL */

/*
 * NAME: NLstrchr
 *
 * FUNCTION: look for the occurrence of an NLchar in a multibyte string.
 *
 * RETURN VALUE DESCRIPTION: returns a pointer to the first occurrence
 * of the NLchar or a NULL on failure.
 *
 */
unsigned char *
#ifdef _NO_PROTO
NLstrchr(sp, c)
register unsigned char *sp;
register NLchar c;
#else /* _NO_PROTO */
NLstrchr(register unsigned char *sp, register NLchar c)
#endif /* _NO_PROTO */
{
	register int n;

	do {
		if (1 < (n = NLchrlen(sp))) {
			if (NCdechr(sp) == c)
				return (sp);
			sp += n - 1;
		} else if (*sp == c)
			return(sp);
	} while(*sp++);
	return(NULL);
}

/*
 * NAME: NCstrchr()
 *
 * FUNCTION: look for the occurrence of an NLchar character within a string of
 * NLchar characters.
 *
 * RETURN VALUE DESCRIPTION: returns a pointer to the first occurrence
 * of the NLchar or a NULL on failure.
 *
 */
NLchar *
#ifdef _NO_PROTO
NCstrchr(sp, c)
register NLchar *sp;
register NLchar c;
#else /* _NO_PROTO */
NCstrchr(register NLchar *sp, register NLchar c)
#endif /* _NO_PROTO */
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return(NULL);
}

#ifdef KJI
#include <wchar.h>

wchar_t *
#ifdef _NO_PROTO
wstrchr(sp, c)
register wchar_t *sp;
register int c;
#else /* _NO_PROTO */
wstrchr(register wchar_t *sp, register int c)
#endif /* _NO_PROTO */
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return(NULL);
}
#endif
