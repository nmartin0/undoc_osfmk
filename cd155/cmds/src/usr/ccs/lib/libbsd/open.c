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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */

#include <syscall.h>

#define SYS_o_open	5	/* The OLD Berkeley open */

extern int syscall(int, ...);

/*
 * This routine exists to allow Berkeley programs (which assumed that
 * opening /dev/tty would give you a controlling terminal) to work.
 *
 * n.b. that the correct way to work in OSF/1 would be to do the following:
 *
 *	setsid();
 *	fd = open("/dev/tty", O_RDWR );
 *	ioctl(fd, TIOCSCTTY, 0);
 */

int open( const char *path, int flags, int mode)
{
  return syscall(SYS_o_open, path, flags, mode);
}
