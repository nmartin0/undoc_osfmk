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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: NCstrdup.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:20 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: NCstrdup
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
 *
 * NCstrdup.c	1.6 com/lib/c/str,3.1,8943 10/20/89 14:12:08
 */

#include <NLchar.h>
#include <stdlib.h>

/*
 *  returns a pointer to a NLchar string which is a duplicate of the wchar 
 *  string pointed to by s1.  Space for the new string is allocated by using 
 *  MALLOC (BA_OS).  When a new string cannot be created a NULL pointer is 
 *  returned.
 */
#define NULL 0

NLchar *
NCstrdup(s1)
register NLchar *s1;
{
	register NLchar *ns;

	if ((ns = (NLchar *)malloc((size_t)(NCstrlen (s1) * sizeof (NLchar)))) == NULL)
	    return (NULL);
	
	NCstrcpy (ns, s1);
	return (ns);
}

#ifdef KJI
#include <wchar.h>
#undef wstrdup

wchar_t *
wstrdup(s1)
register wchar_t *s1;
{
	return (NCstrdup (s1));
}
#endif
