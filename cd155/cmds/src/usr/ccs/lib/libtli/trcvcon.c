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
 ** trcvcon.c 1.3, last change 10/16/89
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
t_rcvconnect (fd, call)
	int	fd;
reg	struct t_call * call;
{
	char	buf[TLI_STACK_BUF_SIZE];
	int		iflags;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	struct T_conn_con * tcc;
	int		ret;
	struct tli_st	tli;
        int             code;

        code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}	
	if (tli.tlis_servtype == T_CLTS) {
		t_errno = TNOTSUPPORT;
	        goto rtn;
	}
#ifdef XTI
	if (tli.tlis_state != T_OUTCON) {
		t_errno = TOUTSTATE;
	        goto rtn;
	}
#endif
	
	if (call) {
		databuf.buf = call->udata.buf;
		databuf.len = 0;
		databuf.maxlen = call->udata.maxlen;
	} else
		databuf.maxlen = -1;
	
	tcc = (struct T_conn_con *)buf;
	ctlbuf.buf = (char *)tcc;
	ctlbuf.len = 0;
	ctlbuf.maxlen = sizeof(buf);

	iflags = 0;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);

	/* If the stream is in non-blocking mode, get out now. */
	if (ret == -1 && (errno  == EAGAIN) && (t_is_nonblocking(fd))) {
		t_errno = TNODATA;
		return -1;
	}

	if (ret == -1
	|| (ret & MORECTL)
	|| ctlbuf.len < sizeof(struct T_conn_con)
	|| tcc->PRIM_type != T_CONN_CON) {
		code = t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		goto rtn;
	}
	if (call) {
		if (databuf.len < 0)
			call->udata.len = 0;
		else
			call->udata.len = databuf.len;
		if (ret == MOREDATA) {
			t_errno = TBUFOVFLW;
		        goto rtn;
		}
		if (tcc->RES_length  &&  call->addr.maxlen > 0) {
			if (call->addr.maxlen < tcc->RES_length) {
				t_errno = TBUFOVFLW;
			        goto rtn;
			}

			call->addr.len = tcc->RES_length;
			memcpy(call->addr.buf, &ctlbuf.buf[tcc->RES_offset], call->addr.len);
		} else
			call->addr.len = 0;
		if (tcc->OPT_length  &&  call->opt.maxlen > 0) {
			if (call->opt.maxlen < tcc->OPT_length) {
				t_errno = TBUFOVFLW;
			        goto rtn;
			}
			call->opt.len = tcc->OPT_length;
			memcpy(call->opt.buf, &ctlbuf.buf[tcc->OPT_offset], call->opt.len);
		} else
			call->opt.len = 0;
	}
	(void)t_sync(fd);
	code = 0;
rtn:

#ifdef XTIDBG
	tr_rcvcon (fd, call, code);
#endif
	if (call && call->addr.maxlen <= 0) {
		t_errno = TBUFOVFLW;
	        code = -1;
	}
	return code;
}
