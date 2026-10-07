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
 ** tlisten.c 1.5, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/tlistate.h>
#ifdef	XTI
#include <xti.h>
#include <tli/timod.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_listen (fd, call)
	int		fd;
reg	struct t_call	* call;
{
	char	buf[TLI_STACK_BUF_SIZE];
	struct T_conn_ind * tci = (struct T_conn_ind *)buf;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int	iflags;
	int	ret;
	struct t_info	tinfo;
	struct tli_st	tli;
#ifdef XTI
	XTIS	xtis;
#endif
        int    code;

        code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if ( tli.tlis_servtype == T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
#ifdef XTI
	if (tli_ioctl(fd, TI_XTI_GET_STATE, (char *)&xtis, sizeof(xtis)) == -1
	||  xtis.xtis_qlen <= 0) {
		t_errno = TBADQLEN;
		goto rtn;
	}
	if (tli.tlis_state != T_IDLE &&  tli.tlis_state != T_INCON) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#endif

	databuf.buf = call->udata.buf;
	databuf.len = 0;
	databuf.maxlen = call->udata.maxlen;
	
	ctlbuf.buf = (char *)tci;
	ctlbuf.len = 0;
	ctlbuf.maxlen = sizeof(buf);

	iflags = 0;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if (ret == -1
	|| (ret & MORECTL)
	|| ctlbuf.len < sizeof(struct T_conn_ind)
	|| tci->PRIM_type != T_CONN_IND) {
		code = t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		goto rtn;
	}
	call->udata.len = databuf.len;
	call->sequence = tci->SEQ_number;
	if (ret == MOREDATA) {
		(void)t_sync(fd);
		t_errno = TBUFOVFLW;
		goto rtn;
	}
	if (tci->SRC_length > 0  &&  call->addr.maxlen > 0) {
		if (call->addr.maxlen < tci->SRC_length) {
			(void)t_sync(fd);
			t_errno = TBUFOVFLW;
			goto rtn;
		}
		call->addr.len = tci->SRC_length;
		memcpy(call->addr.buf, &ctlbuf.buf[tci->SRC_offset], call->addr.len);
	} else
		call->addr.len = 0;
	if (tci->OPT_length > 0  &&  call->opt.maxlen > 0) {
		if (call->opt.maxlen < tci->OPT_length) {
			(void)t_sync(fd);
			t_errno = TBUFOVFLW;
			goto rtn;
		}
		call->opt.len = tci->OPT_length;
		memcpy(call->opt.buf, &ctlbuf.buf[tci->OPT_offset], call->opt.len);
	} else
		call->opt.len = 0;
	(void)t_sync(fd);
        code = 0;
rtn:
#ifdef XTIDBG
	tr_listen (fd, call, code);
#endif
	if(call->addr.maxlen <= 0) {
		t_errno = TBUFOVFLW;
		code = -1;
	}
	return code;
}
