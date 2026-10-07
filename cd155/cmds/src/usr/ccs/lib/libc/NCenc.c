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
static char rcsid[] = "@(#)$RCSfile: NCenc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:00 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCenc
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
 * NCenc.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:28:55
 */

/* Note that this function is NLS/JLS independent */

#include <sys/types.h>
#include <sys/NLchar.h>

#ifdef NCenc
#undef NCenc
#endif

int
#ifdef _NO_PROTO
NCenc(x, c)
wchar_t *x;
char    *c;
#else
NCenc(wchar_t *x, char *c)
#endif
{
	if (*x > 0xff) {			/* multi-byte char? */
		 c[0] = ( (*x >> 8) & 0xff );	/* yes, shift top & store */
		 c[1] = ( *x & 0xff );		/*      store bottom */
		 return (2);			/* return length */
	} else { c[0] = ( *x & 0xff );		/* only one char to string */
		 return (1);			/* return length */
	}
}
