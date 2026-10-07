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
 ** trcvrel.c 1.3, last change 1/29/90
 **/

#include <sys/stream.h>
#include <tli/common.h>
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
t_rcvrel (fd)
	int	fd;
{
	struct T_ordrel_ind	toi;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int	event;
	int	iflags;
	int	ret;
	struct tli_st	tli;
        int     code;

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
	if (tli.tlis_state != T_OUTREL &&  tli.tlis_state != T_INREL) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#endif
	event = t_ilook(fd);
	if (event != T_ORDREL) {
		if( event == T_DISCONNECT)
			t_errno = TLOOK;
		else if ( event != -1 )
			t_errno = TNOREL;
		goto rtn;
	}
	databuf.buf = nilp(char);
	databuf.len = 0;
	databuf.maxlen = 0;
	
	ctlbuf.buf = (char *)&toi;
	ctlbuf.len = 0;
	ctlbuf.maxlen = sizeof(struct T_ordrel_ind);

	iflags = 0;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if (ret
	||  ctlbuf.len != sizeof(struct T_ordrel_ind)
	||  toi.PRIM_type != T_ORDREL_IND)
		return t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
#ifdef XTI
	tli_ioctl(fd, TI_XTI_CLEAR_EVENT, nilp(char), 0);
#endif
	(void)t_sync(fd);
	code = 0;
rtn:

#ifdef XTIDBG
	tr_rcvrel (fd, code);
#endif
	return code;
}
