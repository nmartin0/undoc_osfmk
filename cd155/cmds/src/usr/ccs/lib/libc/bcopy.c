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
static char rcsid[] = "@(#)$RCSfile: bcopy.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:39 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: bcopy
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
 * bcopy.c	1.6  com/lib/c/gen,3.1,8943 10/27/89 15:47:29
 */

#include <string.h>

/*
 * NAME:	bcopy, ovbcopy
 *
 * FUNCTION:	bcopy - copy 'length' bytes from 'src' to 'dest'
 *
 * NOTES:	Bcopy copies 'length' bytes from 'src' to 'dest'.
 *		Overlapping 'src' and 'dest' are handled appropriately.
 *		Ovbcopy is provided as a compatibility interface.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
bcopy(char *src, char *dst, int length)
{
	/*
	 * let memmove do the work...
	 */

	(void) memmove( (void*)dst, (void*)src, (size_t)length );
}

void
ovbcopy(char *src, char *dst, int length)
{
	/*
	 * let memmove do the work...
	 */

	(void) memmove( (void*)dst, (void*)src, (size_t)length );
}
