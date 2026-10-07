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
static char rcsid[] = "@(#)$RCSfile: isjparen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:26 $";
#endif
/*
 * COMPONENT NAME: (LIBCGEN/KJI) Standard C Library Classification Function
 *
 * FUNCTIONS: isjparen, isjparent
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
 */                                                                   
/* isjparen.c	1.1  com/lib/c/gen/KJI,3.1,9021 2/27/90 20:57:39 */

#ifdef KJI
#include <NLchar.h>

#ifdef isjparen
#undef isjparen
#endif

int
#ifdef _NO_PROTO
isjparen(c)
unsigned c;
#else
isjparen(const unsigned c)
#endif
{
	return( (0x8169<= c) && (c <=0x817a) );
}

#ifdef isjparent
#undef isjparent
#endif

int
#ifdef _NO_PROTO
isjparent(c)
unsigned c;
#else
isjparent(const unsigned c)
#endif
{
	return( (0x8169<= c) && (c <=0x817a) );
}
#endif
