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
static char rcsid[] = "@(#)$RCSfile: NLisNLcp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:51 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NLisNLcp
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
 * NLisNLcp.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:35:43
 */

#include <NLchar.h>

#ifdef NLisNLcp
#undef NLisNLcp
#endif

int
#ifdef _NO_PROTO
NLisNLcp(c)
unsigned char *c;
#else
NLisNLcp(char *c)
#endif
{
#ifdef KJI
	if ( (unsigned char) c[0] < 0x80 )
		return (0);
	if ( ((unsigned char) c[0] >= 0xa0) && ((unsigned char) c[0] <= 0xdf) )
		return (1);
	if ( (_jctype1_[_jctype0_[ (unsigned char)c[0] ]] [ (unsigned char)c[1] ] != 0)
						&& ( (unsigned char)c[0] || (unsigned char)c[1] > 0x7f) )
		return (2);
	else	return (0);
#else
	return( (c[0] & 0x80) ? 1 : 0 );
#endif
}
