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
static char rcsid[] = "@(#)$RCSfile: fgetwc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:44 $";
#endif
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: fgetwc 
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
 * fgetwc.c	1.3  com/lib/c/io,3.1,8943 9/12/89 17:48:30
 */

#include <stdio.h>

/*
 * FUNCTION:	
 * Get a one or two byte character and return it as a wide char.
 * (NLchar data type)
 *
 * RETURN VALUE DESCRIPTION:	
 * Returns EOF if getc returns EOF on either the first or second byte read.
 */

int fgetwc(FILE *stream)
{
        return (getwc(stream));
}
