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
t_blocking (fd)
	int	fd;
{
	register int status;

	/*
	 * Get descriptor's current flag settings
	 */
	if ((status = fcntl( fd, F_GETFL, 0 )) == -1)
		return status;

	/*
	 * Now ensure that the NO DELAY bit is cleared, ie. make
	 * descriptor do blocking I/O
	 */
	return fcntl( fd, F_SETFL, (status & ~FNDELAY) );
}


/** Copyright (c) 1990  Mentat Inc.
 ** tcblock.c 2.1, last change 11/14/90
 **/

#include <tli/common.h>
#include <fcntl.h>

int
t_is_nonblocking (fd)
	int	fd;
{
	int	flags;
	
	flags = fcntl(fd, F_GETFL, 0);
	if ( flags == -1 )
		return -1;
	return (flags & O_NDELAY) ? 1 : 0;
}
