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
static char rcsid[] = "@(#)$RCSfile: fsetpos.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:08 $";
#endif
/*
 * FUNCTIONS: fsetpos 
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
 * fsetpos.c	1.7  com/lib/c/io,3.1,8943 9/12/89 18:04:38
 */

#include <stdio.h>
#include <errno.h>

/*                                                                    
 * FUNCTION: Sets the file position indicator for the stream pointed to by
 *           stream to the value of the object pointed to by pos.
 *
 * PARAMETERS: FILE *stream      - stream to be searched
 *	       fpos_t  *pos      - current value of file position
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      zero if successful
 *	      non-zero if not successful
 *	      errno is set on error to indicate the error
 */

int	
fsetpos(FILE *stream, const fpos_t *pos)
{
	if (fseek(stream, *pos, 0) == 0)
		return(0);
	else {
#ifdef _THREAD_SAFE
		seterrno(EINVAL);
#else
		errno = EINVAL;
#endif 
		return(-1);
	}
}
