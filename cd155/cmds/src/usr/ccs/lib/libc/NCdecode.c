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
static char rcsid[] = "@(#)$RCSfile: NCdecode.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:01:59 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCdecode
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * NCdecode.c	1.11  com/lib/c/nls,3.1,9013 2/11/90 17:07:57
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 * NAME: NCdecode
 *
 * FUNCTION: Convert 1 or 2 chars to a wchar_t
 *
 * NOTE:     Macro NCisshift tests shift char 
 *
 * RETURN VALUE DESCRIPTION: Return the number of characters converted.
 */
/*
 * Convert 1 or 2 chars to a wchar_t; return # chars converted.
 */
int
NCdecode(c, nlc)
register unsigned char *c;	/* char type string */
register wchar_t *nlc;   /* wchar_t type string */
{
	if ( NCisshift(c[0]) ) {
		nlc[0] = (c[0] << 8) | (c[1] & 0xff);
		return(2);
	}
	nlc[0] = c[0] & 0xff;
	return(1);
}
