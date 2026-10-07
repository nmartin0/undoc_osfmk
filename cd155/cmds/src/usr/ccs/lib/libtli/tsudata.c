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
 ** tsudata.c 1.2, last change 1/29/90
 **/

#include <sys/stream.h>
#include <tli/common.h>
#include <errno.h>
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
t_sndudata (fd, ud)
	int	fd;
	struct t_unitdata * ud;
{
	char	buf[MAX_STACK_BUF];
	struct strbuf		ctlbuf;
	struct strbuf		databuf;
	struct T_unitdata_req	* tudr;
	int	len;
	struct tli_st	tli;
        int     code;
	int	ret;

        code = -1;
	if(iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if ( tli.tlis_servtype != T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
#ifdef	XTI
	if ( tli.tlis_state != T_IDLE ) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#else
	if ( tli.tlis_state != T_IDLE ) {
		errno = EPROTO;
		t_errno = TSYSERR;
		goto rtn;
	}
#endif
	if ( ud->udata.len == 0 ) {
		t_errno = TBADDATA;
		goto rtn;
	}

	/* NOTE: this check may need to be against tsdu for XTI */
	if ( ud->udata.len > tli.tlis_tidu_size ) {
		t_errno = TSYSERR;
		errno = EPROTO;
		goto rtn;
	}
	len = sizeof(struct T_unitdata_req) + ud->addr.len + ud->opt.len;
	if (len > sizeof(buf)) {
		t_errno = TBUFOVFLW;
		goto rtn;
	}
	ctlbuf.len = len;
	tudr = (struct T_unitdata_req *)&buf[0];
	ctlbuf.buf = (char *)tudr;
	tudr->PRIM_type = T_UNITDATA_REQ;
	databuf.buf = ud->udata.buf;
	databuf.len = ud->udata.len;
	len = ud->addr.len;
	if (len > 0) {
		tudr->DEST_offset = sizeof(struct T_unitdata_req);
		tudr->DEST_length = len;
		memcpy(&buf[sizeof(struct T_unitdata_req)], ud->addr.buf, len);
	} else {
		tudr->DEST_offset = 0;
		tudr->DEST_length = 0;
	}
	len = ud->opt.len;
	if (len > 0) {
		tudr->OPT_length = len;
		tudr->OPT_offset = sizeof(struct T_unitdata_req) + tudr->DEST_length;
		memcpy(&buf[tudr->OPT_offset], ud->opt.buf, len);
	} else {
		tudr->OPT_length = 0;
		tudr->OPT_offset = 0;
	}
	if ((ret = putmsg(fd, &ctlbuf, &databuf, 0)) == -1) {
#ifdef	XTI
		if ( errno == ERANGE ) {
			/* Secret message from TIMOD!  T_UDERR waiting. */
			(void)t_sync(fd);
			t_errno = TLOOK;
			goto rtn;
		}
#endif
		t_unix_to_tli_error();
		if ( t_errno == TNODATA ) {
#ifdef XTI
			iostate_sw(fd, &tli, IOSTATE_SETFLAG, TLIS_DATA_STOPPED);
#endif
			t_errno = TFLOW;
			goto rtn;
		}
		return -1;
	}
	code = 0;
rtn:
#ifdef XTIDBG
	tr_sndudata(fd, ud, code);
#endif
#ifdef XTI
	iostate_sw(fd, &tli, IOSTATE_CLEARFLAG, TLIS_DATA_STOPPED);
#endif
	return code;
}
