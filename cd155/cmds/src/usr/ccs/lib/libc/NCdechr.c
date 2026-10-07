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
static char rcsid[] = "@(#)$RCSfile: NCdechr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:01:58 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCdechr
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * NCdechr.c	1.1.1.1  com/lib/c/nls,3.1,9013 2/28/90 18:30:59
 */

/* This function converts 1 or 2 chars to a "wchar_t" and returns it. */


/*	NOTE! This function is dependent on NLchar.h definition of
 *	what constitutes a multi-byte character (NCisshift) which
 *      differs between NLS and JLS...
 */
#include <sys/types.h>
#include <NLchar.h>

#ifdef NCdechr
#undef NCdechr
#endif

int
#ifdef _NO_PROTO
NCdechr(c)
char *c;
#else
NCdechr(char *c)
#endif
{
	if (NCisshift(c[0]))   				 /* multi-byte? */
		 return( ((unsigned char)c[0] << 8) | (c[1] & 0xff) );  
	else 	 return( (unsigned char) c[0] ); 
}
