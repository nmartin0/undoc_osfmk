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
static char rcsid[] = "@(#)$RCSfile: difftime.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:13 $";
#endif
/*
 * COMPONENT_NAME: (LIBCTIME) Standard C Library Time Management Functions 
 *
 * FUNCTIONS: difftime 
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
 * difftime.c	1.8  com/lib/c/time,3.1,8943 9/13/89 13:20:35
 */

#include <time.h>
#include <stdlib.h>

/*
 * FUNCTION: Calculates the difference between two calandar times
 *
 * PARAMETERS: 
 *	     long time1  - one time value
 *	     long time0  - other time value
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the difference in the two time values
 *
 */

double 	
difftime(time_t time1, time_t time0)
{
	return((double)(time1 - time0));
}
