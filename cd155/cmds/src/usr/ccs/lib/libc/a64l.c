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
static char rcsid[] = "@(#)$RCSfile: a64l.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:23 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV a64l
 *
 * FUNCTIONS: a64l
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * a64l.c	1.4  com/lib/c/cnv,3.1,8943 9/13/89 09:18:23
 */

#define BITSPERCHAR	6 /* to hold entire character set */

/*
 * NAME: a64l
 *                                                                    
 * FUNCTION: convert base 64 ascii to long int
 *                                                                    
 * NOTES:
 * char set is [./0-9A-Za-z]
 *
 * RETURNS: a long value
 *
 */

long
a64l(s)
register char *s;
{
	register int i, c;
	long lg = 0;

	for (i = 0; (c = *s++) != '\0'; i += BITSPERCHAR) {
		if (c > 'Z')
			c -= 'a' - 'Z' - 1;
		if (c > '9')
			c -= 'A' - '9' - 1;
		lg |= (long)(c - ('0' - 2)) << i;
	}
	return (lg);
}
