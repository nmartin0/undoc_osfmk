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
static char rcsid[] = "@(#)$RCSfile: mbscmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:05 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mbscmp, wcscmp
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
 * mbscmp.c	1.2  com/lib/c/nls,3.1,8943 9/13/89 17:22:37
 */

#include <NLchar.h>
#include <sys/types.h>

/*
 * NAME: mbscmp
 *                                                                    
 * FUNCTION: Compare characters (code points) of one multibyte character 
 *	     string to another multibyte character string.
 *
 * PARAMETERS: 
 *	     char *s1 - string
 *	     char *s2 - string
 *
 * RETURN VALUE DESCRIPTION: Returns s1>s2; >0  s1==s2; 0  s1<s2; <0.
 * NOTE:
 *  Two versions here:  mbscmp (operates on ASCII with embedded NLS
 *  code points) and wcscmp (operates on wchar_t).
 *
 */

#define NULL	0

/*
 * In the main loop, we fetch one wchar_t from each string and determine its
 * collating value.  If there is any text in the prefix string, or if a new
 * prefix string is created from the call to _NLxcol, then we use that;
 * otherwise we already have the value from NCcollate or _NLxcol.
 */
#ifdef KJI
#define	chnext(s)	((NCisshift(s[0]) && s[1] != '\0') ? s+=2, _NCd2(s[-2], s[-1]) : *s++)
#define	conext(ch)	NCcollate(ch)
#else
#define	chnext(s)	(_NCis2(s[0], s[1]) ? s+=2, _NCd2(s[-2], s[-1]) : *s++)
#define	conext(s, ch)	NCcollate(ch = chnext(s))
#endif
int mbscmp(char *s1, char *s2) {
	register int co1, co2;
	register int tbreak;
	register wchar_t ch1, ch2;
	wchar_t *p1, *p2;

	if (s1 == s2) return (0);
	p1 = p2 = NULL;
	tbreak = 0;
	do {
#ifdef KJI
		ch1 = chnext(s1);
		if (p1 != NULL || (co1 = conext(ch1)) < 0 &&
		    (co1 = _NLxcol(co1, &s1, &p1)) == -1) {
			co1 = NCcollate(*p1);
			p1++;
#else
		if (p1 != NULL || (co1 = conext(s1, ch1)) < 0 &&
		    (co1 = _NLxcol(co1, &s1, &p1)) == -1) {
			co1 = NCcollate(*p1++);
#endif
			if (*p1 == '\0') p1 = NULL;
		}
#ifdef KJI
		ch2 = chnext(s2);
		if (p2 != NULL || (co2 = conext(ch2)) < 0 &&
		    (co2 = _NLxcol(co2, &s2, &p2)) == -1) {
			co2 = NCcollate(*p2);
			p2++;
#else
		if (p2 != NULL || (co2 = conext(s2, ch2)) < 0 &&
		    (co2 = _NLxcol(co2, &s2, &p2)) == -1) {
			co2 = NCcollate(*p2++);
#endif
			if (*p2 == '\0') p2 = NULL;
		}
		if (tbreak == 0) tbreak = ch1 - ch2;
	} while (ch1 != '\0' && ch2 != '\0' && co1 == co2);
	return (co1 != co2 ? co1 - co2 : tbreak);
}
#undef chnext
#undef conext

/*
 * Similar logic to mbscmp.
 */
#define	chnext(s)	(*s++)
#ifdef KJI
#define	conext(ch)	NCcollate(ch)
#else
#define	conext(s, ch)	NCcollate(ch = chnext(s))
#endif
int wcscmp(wchar_t *s1, wchar_t *s2) {
	register int co1, co2;
	register int tbreak;
	wchar_t ch1, ch2;
	wchar_t *p1, *p2;

	if (s1 == s2) return (0);
	p1 = p2 = NULL;
	tbreak = 0;
	do {
#ifdef KJI
		ch1 = chnext(s1);
		if (p1 != NULL || (co1 = conext(ch1)) < 0 &&
		    (co1 = _NCxcol(co1, &s1, &p1)) == -1) {
			co1 = NCcollate(*p1);
			p1++;
#else
		if (p1 != NULL || (co1 = conext(s1, ch1)) < 0 &&
		    (co1 = _NCxcol(co1, &s1, &p1)) == -1) {
			co1 = NCcollate(*p1++);
#endif
			if (*p1 == '\0') p1 = NULL;
		}
#ifdef KJI
		ch2 = chnext(s2);
		if (p2 != NULL || (co2 = conext(ch2)) < 0 &&
		    (co2 = _NCxcol(co2, &s2, &p2)) == -1) {
			co2 = NCcollate(*p2);
			p2++;
#else
		if (p2 != NULL || (co2 = conext(s2, ch2)) < 0 &&
		    (co2 = _NCxcol(co2, &s2, &p2)) == -1) {
			co2 = NCcollate(*p2++);
#endif
			if (*p2 == '\0') p2 = NULL;
		}
		if (tbreak == 0) tbreak = ch1 - ch2;
	} while (ch1 != '\0' && ch2 != '\0' && co1 == co2);
	return (co1 != co2 ? co1 - co2 : tbreak);
}
