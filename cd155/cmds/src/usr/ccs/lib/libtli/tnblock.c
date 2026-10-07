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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */

#include <fcntl.h>
#include <tli/common.h>
#include <stropts.h>

int
t_nonblocking (fd)
	int	fd;
{
	register int flags;

	/*
	 * Get descriptor's current flag settings
	 */
	if ((flags = fcntl( fd, F_GETFL, 0 )) == -1)
		return -1;

	/*
	 * Now ensure that the NO DELAY bit is set, ie. make
	 * descriptor do nonblocking I/O
	 */
	return fcntl( fd, F_SETFL, (flags | FNDELAY) );
}
