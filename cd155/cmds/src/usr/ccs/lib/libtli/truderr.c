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
 ** truderr.c 1.5, last change 1/29/90
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
t_rcvuderr (fd, uderr)
	int	fd;
	struct t_uderr * uderr;
{
	char	buf[TLI_STACK_BUF_SIZE];
	struct T_uderror_ind	* tudei;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	struct tli_st	tli;
	int	iflags;
	int	ret;
        int     code;
        int     event;

        code = -1;

	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_servtype != T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
	event = t_ilook(fd);	
	if (event != T_UDERR) {
		if ( event != -1 )
			t_errno = TNOUDERR;
		goto rtn;
	}
	databuf.buf = nilp(char);
	databuf.len = 0;
	databuf.maxlen = 0;
	
	tudei = (struct T_uderror_ind *)buf;
	ctlbuf.buf = (char *)tudei;
	ctlbuf.len = 0;
	ctlbuf.maxlen = sizeof(buf);

	iflags = 0;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if (ret
	||  ctlbuf.len < sizeof(struct T_uderror_ind)
	||  tudei->PRIM_type != T_UDERROR_IND) {
		t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		if (t_errno == TNODATA  ||  t_errno == TLOOK)
			t_errno = TNOUDERR;
		goto rtn;
	}
#ifdef XTI
	tli_ioctl(fd, TI_XTI_CLEAR_EVENT, nilp(char), 0);
#endif
	if (uderr) {
		if (uderr->addr.maxlen <= 0) {
			t_errno = TBUFOVFLW;
			goto rtn;
		}
		uderr->error = tudei->ERROR_type;
		if (tudei->DEST_length > 0  &&  uderr->addr.maxlen > 0) {
			if (uderr->addr.maxlen < tudei->DEST_length) {
				t_errno = TBUFOVFLW;
				goto rtn;
			}
			uderr->addr.len = tudei->DEST_length;
			memcpy(uderr->addr.buf, &ctlbuf.buf[tudei->DEST_offset], uderr->addr.len);
		} else
			uderr->addr.len = 0;
		if (tudei->OPT_length > 0  &&  uderr->opt.maxlen > 0) {
			if (uderr->opt.maxlen < tudei->OPT_length) {
				t_errno = TBUFOVFLW;
				goto rtn;
			}
			uderr->opt.len = tudei->OPT_length;
			memcpy(uderr->opt.buf, &ctlbuf.buf[tudei->OPT_offset], uderr->opt.len);
		} else
			uderr->opt.len = 0;
	}
	code = 0;
rtn:
#ifdef XTIDBG
	tr_rcvuderr (fd, uderr, code);
#endif
	return code;
}
