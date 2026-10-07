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
static char rcsid[] = "@(#)$RCSfile: gtty.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:05 $";
#endif
/*
 * LIBCTTY: gtty
 *                                                                    
 * ORIGIN: IBM
 *
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1988
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 *
 *
 * FUNCTION:	Berkeley backward compatable routine to get a tty's
 *		characteristics.
 *
 * RETURN VALUE DESCRIPTION:	
 *		Returns 0 open success.  Possible errors are EFAULT, EPERM,
 *		and EINVAL.
 *
 * gtty.c	1.1  com/lib/c/tty,3.1,8943 3/22/89 13:42:32
 */

#include <sgtty.h>
int gtty(int fd, struct sgttyb *sg)
{
    return ioctl(fd, TIOCGETP, sg);
}
