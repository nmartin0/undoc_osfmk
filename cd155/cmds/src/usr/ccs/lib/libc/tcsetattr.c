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
static char rcsid[] = "@(#)$RCSfile: tcsetattr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:03 $";
#endif
/*
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
 * tcsetattr.c	1.2  com/lib/c/tty,3.1,8943 9/23/89 16:13:46
 */                                                                   


#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/termios.h>
#include <stdio.h>

int tcsetattr(int fd, int optional_actions, struct termios *term)
{
    int how;
    
    if (!isatty(fd))
	return -1;

    switch (optional_actions) {
    case TCSANOW:
	return ioctl(fd, TIOCSETA, term);
    case TCSADRAIN:
	return ioctl(fd, TIOCSETAW, term);
    case TCSAFLUSH:
	return ioctl(fd, TIOCSETAF, term);
    default:
        errno = EINVAL;
        return -1;
    }
}
