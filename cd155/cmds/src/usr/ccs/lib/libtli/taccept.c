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
 ** taccept.c 1.2, last change 6/4/90
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
t_accept (fd, resfd, call)
	int	fd;
	int	resfd;
	struct	t_call * call;
{
	char	buf[TLI_STACK_BUF_SIZE];
	struct strfdinsert	strfd;
	struct T_conn_res	* tcr;
	struct	tli_st	tli;
	int	total_len;
	int     code;
	int	event;

	code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_servtype == T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
	if ((event = t_ilook(fd)) == T_DISCONNECT) {
		t_errno = TLOOK;
		goto rtn;
	}
	if (tli.tlis_state != T_INCON) {
		t_errno = TOUTSTATE;
		goto rtn;
	}

	total_len = (call->opt.len >= 0 ? call->opt.len : 0);
	total_len += sizeof(struct T_conn_res);
	if (total_len > sizeof(buf)) {
		t_errno = TBUFOVFLW;
		goto rtn;
	}
	tcr = (struct T_conn_res *)&buf[0];
	tcr->PRIM_type = T_CONN_RES;
	if (call->opt.len > 0) {
		tcr->OPT_length = call->opt.len;
		tcr->OPT_offset = sizeof(struct T_conn_res);
		memcpy(&buf[tcr->OPT_offset], call->opt.buf, tcr->OPT_length);
	} else {
		tcr->OPT_length = 0;
		tcr->OPT_offset = 0;
	}
	tcr->SEQ_number = call->sequence;
	strfd.ctlbuf.buf = (char *)tcr;
	strfd.ctlbuf.len = total_len;
	strfd.databuf.buf = call->udata.buf;
	strfd.databuf.len = call->udata.len;
	strfd.flags = 0;
	strfd.fildes = resfd;
	strfd.offset = sizeof(long);
	if (stream_ioctl(fd, I_FDINSERT, &strfd) == -1) {
		t_unix_to_tli_error();
		goto rtn;
	}
	/* Check for accept acknowledgment */
	if ((code = t_chk_ack(fd, T_CONN_RES)) == -1 )
		goto rtn;

	event = t_ilook(fd);
	if(event == T_LISTEN || event == T_DISCONNECT) {
		t_errno = TLOOK;
		code = -1;
		goto rtn;	
	}
	(void)t_sync(fd);
	if ( resfd != fd )
		(void)t_sync(resfd);
rtn:   
#ifdef XTIDBG
        tr_accept (fd, resfd, call, code);
#endif
	return code;
}
