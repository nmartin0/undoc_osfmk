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
static char rcsid[] = "@(#)$RCSfile: fgetpos.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:42 $";
#endif
/*
 * FUNCTIONS: fgetpos 
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
 * fgetpos.c	1.8  com/lib/c/io,3.1,8943 10/10/89 15:18:53
 */

#include <stdio.h>
#include <errno.h>

/*                                                                    
 * FUNCTION: Stores the current value of the file position indicator for the
 *           stream pointed to by stream in the object pointed to by pos.
 *
 * PARAMETERS: FILE *stream - stream to be searched
 *
 *	       fpos_t  *pos      - current value of file position
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      zero if successful
 *	      non-zero if not successful
 */

int	
fgetpos(FILE *stream, fpos_t *pos)
{
	if (stream != NULL && (*pos = ftell(stream)) >= 0) 
		return(0);
#ifdef _THREAD_SAFE
	seterrno(EINVAL);
#else
	errno = EINVAL;
#endif
	return(-1);
}
