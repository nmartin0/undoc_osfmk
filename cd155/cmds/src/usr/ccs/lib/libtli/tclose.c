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

/** Copyright (c) 1989  Mentat Inc.
 ** tclose.c 1.2, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <tli/tlistate.h>
#ifdef	XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_close (fd)
	int	fd;
{
	int	retval;
	struct	tli_st	tli;

	if (iostate_sw(fd, &tli, IOSTATE_FREE, 0) == -1) {
		t_errno = TBADF;
		retval = -1;
		goto rtn;
	}
	retval = stream_close(fd);
	if (retval == -1)
		t_unix_to_tli_error();
rtn:

#ifdef XTIDBG
        tr_close (fd, retval);
#endif
	return retval;
}
