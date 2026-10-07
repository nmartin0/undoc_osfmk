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
static char rcsid[] = "@(#)$RCSfile: raise.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:39 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: raise
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
 * raise.c	1.8  com/lib/c/gen,3.1,8943 9/9/89 09:27:13
 */

#include	<sys/types.h>

/*
 *
 * FUNCTION: sends the signal sig to the executing program
 *
 * PARAMETERS: 
 *	     int    sig - signal to be sent
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns 0 if successful
 *	     - returns nonzero if unsuccessful
 *
 */


int
raise(int sig)
{
	pid_t pid;
	pid = getpid();

	if (kill(pid,sig))
		return(-1);
	else
		return(0);
}
