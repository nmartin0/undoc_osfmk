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
static char rcsid[] = "@(#)$RCSfile: NLstrcpy.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:56 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: NLstrcpy, NCstrcpy
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
 * NLstrcpy.c	1.9 com/lib/c/str,3.1,8943 9/13/89 15:22:43
 */

#include <NLchar.h>

/*
 *  Copy string s2 to s1.  s1 must be large enough.
 *  return s1.  Two versions here:  NLstrcpy (operates on ASCII with
 *  embedded NLS code points and is identical to strcpy), and NCstrcpy
 *  (operates on NLchar strings).
 */

/*
 * NAME: NLstrcpy
 *
 * FUNCTION: identical to strcpy
 *
 * RETURN VALUE DESCRIPTION: returns a pointer to the target string.
 */
char * NLstrcpy(s1, s2)
register char *s1, *s2;
{
	register char *os1;

	os1 = s1;
	while(*s1++ = *s2++)
		;
	return(os1);
}

/*
 * NAME: NCstrcpy
 *
 * FUNCTION: like NLstrcpy except it copies double byte characters instead of mixed byte chars.
 *
 * RETURN VALUE DESCRIPTION: returns a pointer to the target string.
 */
NLchar * NCstrcpy(s1, s2)
register NLchar *s1, *s2;
{
	register NLchar *os1;

	os1 = s1;
	while(*s1++ = *s2++)
		;
	return(os1);
}

#ifdef KJI
#include <wchar.h>
#undef wstrcpy

wchar_t *
wstrcpy(s1, s2)
register wchar_t *s1, *s2;
{
	return (NCstrcpy(s1, s2));
}
#endif
