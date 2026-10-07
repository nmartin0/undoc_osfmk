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
static char rcsid[] = "@(#)$RCSfile: NCencode.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:02 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCencode
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
 * NCencode.c	1.10  com/lib/c/nls,3.1,9013 2/11/90 17:10:20
 */
/*LINTLIBRARY*/

#include <sys/types.h>
#include <NLchar.h>

/*
 * NAME: NCencode
 *
 * FUNCTION: Converts NLchar to  1 or 2 chars.
 *
 * RETURN VALUE DESCRIPTION: The number of chars converted.
 */
/*
 * Convert NLchar to 1 or 2 chars; return # chars produced.
 */

int
NCencode(nlc, c)
register wchar_t *nlc;
register unsigned char *c;
{
	if ( nlc[0] > 256 ) {
		c[0] = nlc[0] >> 8;
		c[1] = nlc[0] & 0xff;
		return(2);
	}
	c[0] = nlc[0] & 0xff;
	return(1);
}
