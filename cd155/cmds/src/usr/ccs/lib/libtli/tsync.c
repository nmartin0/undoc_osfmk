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
 ** tsync.c 1.1, last change 8/9/89
 **/

#include <tli/common.h>
#include <tli/tlistate.h>
#ifdef XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_sync (fd)
	int	fd;
{
	struct t_info	tinfo;
	struct tli_st	tli;
        int     code;

	code = -1;
	if(iostate_sw(fd, &tli, IOSTATE_CREATE, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	code = tli.tlis_state;
rtn:
#ifdef XTIDBG
	tr_sync (fd, code);
#endif
	return code;
}
