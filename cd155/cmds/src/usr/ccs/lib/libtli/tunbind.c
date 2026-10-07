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

/** Copyright (c) 1989  Mentat Inc.
 ** tunbind.c 1.2, last change 11/8/89
 **/

#include <sys/stream.h>
#include <tli/common.h>
#include <tli/tihdr.h>
#include <tli/timod.h>
#include <tli/tlistate.h>
#ifdef  XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_unbind (fd)
	int	fd;
{
	union T_primitives	tunbindr;
        int   code;
	struct tli_st tli;

        code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_state == T_UNBND) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
	tunbindr.type = T_UNBIND_REQ;
	code = tli_ioctl(fd, TI_UNBIND, &tunbindr, sizeof(struct T_unbind_req));
	(void)t_sync(fd);
rtn:
#ifdef XTIDBG
	tr_unbind (fd, code);
#endif
	return code;
}
