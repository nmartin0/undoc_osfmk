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
static char rcsid[] = "@(#)$RCSfile: NCdec.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:01:58 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCdec
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
 * NCdec.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:28:06
 */

/*      This function converts one multi-byte character (1 or 2 bytes)
 *      to a wchar_t and returns the number of bytes converted.
 *      The ANSI equivalent is the mbtowc function.
 *
 *	NOTE! This function is dependent on NLchar.h definition of
 *	what constitutes a multi-byte character (NCisshift) which
 *      differs between NLS and JLS...
 */

#include <sys/types.h>
#include <NLchar.h>

#ifdef NCdec
#undef NCdec
#endif

int
#ifdef _NO_PROTO
NCdec(c, x)
unsigned char *c;
wchar_t *x;
#else
NCdec(unsigned char *c, wchar_t *x)
#endif
{
	if (NCisshift(c[0])) {				/* multi-byte? */
		 *x = ( (c[0] << 8) | (c[1] & 0xff) );  /* yes, collect */
		 return (2);				/* and return */
	} else { *x = c[0];
		 return (1);
	}
}
