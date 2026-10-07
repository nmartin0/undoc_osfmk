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
 ** tconnect.c 1.4, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/tlistate.h>
#ifdef	XTI
#include <xti.h>
#else
#include <tiuser.h>
#define t_ilook t_look
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_connect (fd, sndcall, rcvcall)
	int	fd;
	struct t_call	* sndcall;
	struct t_call	* rcvcall;
{
	char	buf[TLI_STACK_BUF_SIZE];
	struct strbuf		ctlbuf;
	struct strbuf		databuf;
	struct strbuf		*dataptr;
	int	iflags;
	int	ret;
	struct T_conn_con	* tcc;
	struct T_conn_req	* tcr;
	int	total_len;
	int     code;
	struct	tli_st	tli;

	code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_state == T_UNBND) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
	if (t_ilook(fd) == T_DISCONNECT) {
		t_errno = TLOOK;
		goto rtn;
	}
	total_len = sizeof(struct T_conn_req);
	if (sndcall->addr.len > 0)
		total_len += sndcall->addr.len;
	if (sndcall->opt.len > 0)
		total_len += sndcall->opt.len;
	if (total_len > sizeof(buf)) {
		t_errno = TBUFOVFLW;
		goto rtn;
	}
	tcr = (struct T_conn_req *)&buf[0];
	tcr->PRIM_type = T_CONN_REQ;
	if (sndcall->addr.len > 0) {
		tcr->DEST_length = sndcall->addr.len;
		tcr->DEST_offset = sizeof(struct T_conn_req);
		memcpy(&buf[tcr->DEST_offset], sndcall->addr.buf, tcr->DEST_length);
	} else {
		tcr->DEST_length = 0;
		tcr->DEST_offset = 0;
	}
	if (sndcall->opt.len > 0) {
		tcr->OPT_length = sndcall->opt.len;
		tcr->OPT_offset = tcr->DEST_offset + tcr->DEST_length;
		memcpy(&buf[tcr->OPT_offset], sndcall->opt.buf, tcr->OPT_length);
	} else {
		tcr->OPT_length = 0;
		tcr->OPT_offset = 0;
	}
	ctlbuf.buf = (char *)tcr;
	ctlbuf.len = total_len;
	databuf.buf = sndcall->udata.buf;
	databuf.len = sndcall->udata.len;
	if(rcvcall)
		dataptr = &databuf;
	else
		dataptr = NULL;
		
	if (putmsg(fd, &ctlbuf, dataptr, 0) == -1) {
		t_unix_to_tli_error();
		goto rtn;
	}

	/* Check for connection request acknowledgment */
	if ( t_chk_ack(fd, T_CONN_REQ) == -1 ) {
		goto rtn;
	}
	/* If the stream is in non-blocking mode, get out now. */
	if (t_is_nonblocking(fd)) {
		t_errno = TNODATA;
		return -1;
	}

	/* Get the connection confirm */
	tcc = (struct T_conn_con *)buf;
	ctlbuf.buf = (char *)tcc;
	ctlbuf.maxlen = sizeof(buf);
	ctlbuf.len = 0;
	if ( rcvcall ) {
		rcvcall->udata.len = rcvcall->udata.maxlen;
		databuf.buf = rcvcall->udata.buf;
		databuf.len = 0;
		databuf.maxlen = rcvcall->udata.maxlen;
	} else
		databuf.maxlen = -1;
	iflags = 0;
	ret = getmsg(fd, &ctlbuf, dataptr, &iflags);
	if (ret
	||  ctlbuf.len < sizeof(struct T_conn_con)
	||  tcc->PRIM_type != T_CONN_CON){
		code = t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
	        goto rtn;
        }
	if ( rcvcall ) {
		if (tcc->RES_length > 0  &&  rcvcall->addr.maxlen > 0) {
			if (rcvcall->addr.maxlen < tcc->RES_length) {
				(void)t_sync(fd);
				t_errno = TBUFOVFLW;
				goto rtn;
			}
			rcvcall->addr.len = tcc->RES_length;
			memcpy(rcvcall->addr.buf, &ctlbuf.buf[tcc->RES_offset], rcvcall->addr.len);
		} else
			rcvcall->addr.len = 0;
		if (tcc->OPT_length > 0  &&  rcvcall->opt.maxlen > 0) {
			if (rcvcall->opt.maxlen < tcc->OPT_length) {
				(void)t_sync(fd);
				t_errno = TBUFOVFLW;
				goto rtn;
			}
			rcvcall->opt.len = tcc->OPT_length;
			memcpy(rcvcall->opt.buf, &ctlbuf.buf[tcc->OPT_offset], rcvcall->opt.len);
		} else
			rcvcall->opt.len = 0;
		rcvcall->udata.len = dataptr->len;
	}
	(void)t_sync(fd);

	code = 0;
rtn:
#ifdef XTIDBG
	tr_connect (fd, sndcall, rcvcall, code);
#endif
	if (rcvcall && (rcvcall->addr.maxlen <= 0)) {
		t_errno = TBUFOVFLW;
		code = -1;
	}
	return code;
}
