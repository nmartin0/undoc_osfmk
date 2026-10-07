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
static char rcsid[] = "@(#)$RCSfile: wcswcs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:40 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcswcs
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
 * wcswcs.c	1.4  com/lib/c/nls,3.1,9013 2/27/90 21:46:08
 */

#include <sys/types.h>

/*
 * NAME: wcswcs
 *
 * FUNCTION: Locate the first occurrence in the string pointed to by
 *  string1 of the sequence of wchar_t characters in the string pointed
 *  to by string2.
 *
 * PARAMETERS:
 *	wchar_t *string1	-	the wide character string
 *	wchar_t *string2	-	the wide character string
 *
 * RETURN VALUE DESCRIPTION: the pointer to the located string or NULL if
 *  the string is not found.  If string2 points to a string with zero length
 *  the function returns string1.
 */
wchar_t *
#ifdef _NO_PROTO
wcswcs(string1, string2)
wchar_t *string1,*string2;
#else
wcswcs(wchar_t *string1,wchar_t *string2)
#endif
{
	register wchar_t *p;
	register wchar_t *q;
	register wchar_t *r;

  if( *string2 == '\0' )
    return(string1);

  for(q=string1; *q != '\0'; q++) 
  {
    for(r=q, p=string2; *r != '\0' && *p != '\0'; r++, p++)
    {
      if( *p != *r )
	break;
    }
    if( *p == '\0' )
      break;
  }
  if (*q)
      return(q);
  return(NULL);
}
