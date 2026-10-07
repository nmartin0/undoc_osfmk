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
 ** trcvdis.c 1.4, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/tlistate.h>
#ifdef XTI
#include <xti.h>
#include <tli/timod.h>
#else
#include <tiuser.h>
#define	t_ilook	t_look
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_rcvdis (fd, discon)
	int	fd;
reg	struct t_discon * discon;
{
	struct T_discon_ind	tdi;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int	iflags;
	int	ret;
	struct tli_st	tli;
        int     code;
	int event;
	char	discardbuf[TLI_STACK_BUF_SIZE];

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
	if (tli.tlis_state != T_IDLE) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#endif
	if ((event = t_ilook(fd)) != T_DISCONNECT) {
		if (event != -1)
			t_errno = TNODIS;
		goto rtn;
	}
	if ( discon ) {
		databuf.buf = discon->udata.buf;
		databuf.len = 0;
		databuf.maxlen = discon->udata.maxlen;
	} else {
		databuf.buf = discardbuf;
		databuf.maxlen = sizeof(discardbuf);
	}
	
	ctlbuf.buf = (char *)&tdi;
	ctlbuf.len = 0;
	ctlbuf.maxlen = sizeof(tdi);

	iflags = 0;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if(ret & MOREDATA) {
		t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		_tgetdmsg(fd);
		if (discon ) {
			t_errno = TBUFOVFLW;
			goto rtn;
		}
	} else if (ret & MORECTL 
	|| ctlbuf.len != sizeof(struct T_discon_ind) 
	||  tdi.PRIM_type != T_DISCON_IND) {
		t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		 if (t_errno == TNODATA  ||  t_errno == TLOOK)
			t_errno = TNODIS;
		goto rtn;
	}
	if ( discon ) {
		discon->udata.len = databuf.len;
		discon->reason = tdi.DISCON_reason;
		discon->sequence = tdi.SEQ_number;
	}
#ifdef XTI
	tli_ioctl(fd, TI_XTI_CLEAR_EVENT, nilp(char), 0);
#endif
	(void)t_sync(fd);
	code = 0;
rtn:
#ifdef XTIDBG
	tr_rcvdis (fd, discon, code);
#endif
	return code;
}
/**
 ** this is ugly but for svid we must recieve any data
 ** left on the queue within the function call.
 **/
_tgetdmsg(fd)
int fd;
{


	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int	flags;
	char	dbuf[TLI_STACK_BUF_SIZE], dbuf2[TLI_STACK_BUF_SIZE];

	databuf.buf = dbuf;
	databuf.maxlen = sizeof(dbuf);
	
	ctlbuf.buf = dbuf2;
	ctlbuf.len = 0;
	ctlbuf.maxlen = sizeof(dbuf2);
	flags = 0;
	getmsg(fd, &ctlbuf, &databuf, &flags);
}

