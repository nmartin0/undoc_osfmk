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
static char rcsid[] = "@(#)$RCSfile: isatty.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:16 $";
#endif
/*
 * FUNCTIONS: isatty 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * isatty.c	1.7  com/lib/c/io,3.1,8943 9/9/89 13:29:08
 */

/*LINTLIBRARY*/

#include <sgtty.h>
#include <errno.h>
        
extern int ioctl();

int
isatty(int f)
{
	struct sgttyb	tty;

	if(ioctl(f, TIOCGETP, &tty) < 0) {
                if (errno == ENODEV)
                        errno = ENOTTY;
		return(0);
	}
	return(1);
}
