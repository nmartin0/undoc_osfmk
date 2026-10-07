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
static char rcsid[] = "@(#)$RCSfile: memmove.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:07:09 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: memmove
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
 * memmove.c	1.9  com/lib/c/gen,3.1,8943 10/12/89 11:21:07
 */

#include <string.h>			/* for size_t	*/

/*
 *                                                                    
 * FUNCTION:	Copy s2 to s1, always copy n bytes.
 *		Handle overlapping s2 and s1 areas.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	Return s1.
 *
 */  


void *
memmove(void *s1, const void *s2, size_t n)
{
	unsigned char *os1 = s1;
	const unsigned char *os2 = s2;

	/*
	 *	does the target overlap the source?
	 *	i.e. is the target starting address within the
	 *	source we want to copy?
	 */
	if (os2 < os1 && os1 < os2 + n) {
		/*
		 * target overlaps.  copy backwards (right to left) so
		 * that we will not destroy the source...
		 */
		os1 += n;
		os2 += n;

		while (n-- > 0)
			*--os1 = *--os2;
		}

	else
		/*
		 * target does not overlap.  call fast i860 memcpy().
		 */
		(void) memcpy( (void*) s1, (const void*) s2, (size_t) n );

	return (s1);
}
