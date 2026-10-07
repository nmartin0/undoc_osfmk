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
static char rcsid[] = "@(#)$RCSfile: putws.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:36 $";
#endif
/*
 * LIBCGEN: putws.c
 *
 * FUNCTIONS: putws
 *
 * ORIGINS: 10, 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* putws.c	1.1  com/lib/c/io/KJI,3.1,9021 9/25/89 16:41:25 */
/*
 * NAME: putws
 *                                                                    
 * FUNCTION:
 * This function writes an NLchar string, followed by a new-line
 * character, to stdout.  The terminating null character is not
 * written.
 *                                                                    
 * RETURNS:  Putws returns the number of NLchar characters
 * written, not including the new-line character, or EOF on error. 
 */ 
#include <stdio.h>
#include <NLchar.h>

int
putws(NLchar *ptr)
{
    register NLchar *p0 = ptr + 1;

    while (*ptr != '\0'){
	if (putwc (*ptr, stdout) == EOF)
	    return (EOF);
        ptr++;
    }
    putc ('\n', stdout);
    return (ptr - p0);
}
