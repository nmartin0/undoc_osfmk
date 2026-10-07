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
static char rcsid[] = "@(#)$RCSfile: NLstrcmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:56 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: NLstrcmp, NCstrcmp 
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
 * NLstrcmp.c	1.11  com/lib/c/str,3.1,9013 2/11/90 17:34:34
 */

#include <sys/types.h>
#include <string.h>
#include <NLchar.h>

/*
 *
 *  Compare strings.  Returns:  s1>s2; >0  s1==s2; 0  s1<s2; <0.
 *  Two versions here:  NLstrcmp (operates on ASCII with embedded NLS
 *  code points) and NCstrcmp (operates on NLchars).
 *
 */


int NLstrcmp(const char *s1, const char *s2) 
{
	return (strcoll(s1, s2));
} 
	
int NCstrcmp(wchar_t *s1, wchar_t *s2)
{
	return (wcscoll(s1, s2));
}

#ifdef _KJI
#include <wchar.h>
#undef wstrcmp

int wstrcmp(s1, s2)
wchar_t *s1, *s2;
{
	return (NCstrcmp(s1, s2));
}
#endif
