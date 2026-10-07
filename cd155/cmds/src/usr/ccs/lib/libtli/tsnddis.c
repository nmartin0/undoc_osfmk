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
 ** tsnddis.c 1.4, last change 1/29/90
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
#define t_ilook t_look
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_snddis (fd, call)
	int	fd;
	struct t_call * call;
{
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	struct T_discon_req	tdr;
	struct tli_st 	tli;
        int    code;
	int	ret;

        code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_USTATE, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if ( tli.tlis_servtype == T_CLTS ) {	
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
	if (t_ilook( fd) == T_DISCONNECT) {
		t_errno = TLOOK;
		goto rtn;
	}
	if (tli.tlis_state == T_UNBND || tli.tlis_state == T_IDLE) {
		t_errno = TOUTSTATE;
		goto rtn;
	}

	tdr.PRIM_type = T_DISCON_REQ;
	ctlbuf.buf = (char *)&tdr;
	ctlbuf.len = sizeof(struct T_discon_req);
	if ( call ) {
		tdr.SEQ_number = call->sequence;
		databuf.buf = call->udata.buf;
		databuf.len = call->udata.len;
	} else {
		tdr.SEQ_number = -1;
		databuf.len = -1;
	}
	if ((ret = putmsg(fd, &ctlbuf, &databuf, 0)) == -1) {
		t_unix_to_tli_error();
		goto rtn;
	}
	code = t_chk_ack(fd, T_DISCON_REQ);
rtn:
#ifdef XTIDBG
	tr_snddis (fd, call, code);
#endif
	return code;
}

