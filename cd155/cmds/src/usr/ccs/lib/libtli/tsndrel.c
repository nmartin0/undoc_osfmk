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
 ** tsndrel.c 1.2, last change 1/29/90
 **/

#include <sys/stream.h>
#include <tli/common.h>
#include <stropts.h>
#include <tli/tihdr.h>
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
t_sndrel (fd)
	int	fd;
{
	struct strbuf	ctlbuf;
	struct T_ordrel_req	tor;
	struct tli_st	tli;
	int    code;
	int ret, event;

        code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_servtype != T_COTS_ORD) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
#ifdef XTI
	if (tli.tlis_state != T_DATAXFER  &&  tli.tlis_state != T_INREL) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#endif
	if ((event = t_look(fd)) == T_DISCONNECT) {
		t_errno = TLOOK;
		goto rtn;
	}
	tor.PRIM_type = T_ORDREL_REQ;
	ctlbuf.buf = (char *)&tor;
	ctlbuf.len = sizeof(tor);
	if ((ret = putmsg(fd, &ctlbuf, nilp(struct strbuf), 0)) == -1) {
		t_unix_to_tli_error();
		if ( t_errno == TNODATA )
			t_errno = TFLOW;
		goto rtn;
	}
	(void)t_sync(fd);
	code = 0;
rtn:

#ifdef XTIDBG
	tr_sndrel (fd, code);
#endif
	return code;
}
