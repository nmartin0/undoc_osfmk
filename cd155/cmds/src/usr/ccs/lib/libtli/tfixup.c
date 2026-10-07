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
 ** tfixup.c 1.3, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <errno.h>
#ifdef	XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

void
t_unix_to_tli_error () {
	switch (errno) {
	case EBADF:
	case ENOSTR:
		t_errno = TBADF;
		break;
	case EACCES:
		t_errno = TACCES;
		break;
	case EAGAIN:
	case ENODATA:
		t_errno = TNODATA;
		break;
	default:
		t_errno = TSYSERR;
		break;
	}
}

int
t_fixup (fd, ctlbuf, databuf, flags, ret)
	int	fd;
	struct strbuf	* ctlbuf;
	struct strbuf	* databuf;
	int	flags;
	int	ret;
{
	char		buf[128];
	struct strbuf	cbuf;
	u32		* up;

	if (ret == -1) {
		t_unix_to_tli_error();
		(void)t_sync(fd);
		return -1;
	}
	cbuf.len = ctlbuf->len > 0 ? ctlbuf->len : 0;
	cbuf.len += (2 * sizeof(u32));
	if (cbuf.len <= sizeof(buf))
		up = (u32 *)buf;
	else {
		while (!(up = (u32 *)malloc(cbuf.len)))
			noop;
	}
	cbuf.buf = (char *)up;
	up[0] = T_FEEDBACK_REQ;
	up[1] = ret;
	if (ctlbuf->len > 0)
		memcpy(&up[2], ctlbuf->buf, ctlbuf->len);
	(void)t_sync(fd);
	if (putmsg(fd, &cbuf, databuf, flags) == -1)
		t_errno = TSYSERR;	/* big trouble here ... */
	else
		t_errno = TLOOK;
	if (up != (u32 *)&buf[0])
		free(up);
	return -1;
}

int
t_chk_ack (fd, prim_type)
	int	fd;
	int	prim_type;
{
	char			buf[MAX_STACK_BUF];
	struct strbuf		ctlbuf;
	struct strbuf		databuf;
	int			iflags;
	int			ret;
	struct T_error_ack	* tea;
	struct T_ok_ack		* toa;
	
	/* Check for accept acknowledgment */
	toa = (struct T_ok_ack *)buf;
	ctlbuf.buf = (char *)toa;
	ctlbuf.maxlen = sizeof(buf);
	ctlbuf.len = 0;
	databuf.buf = nilp(char);
	databuf.maxlen = -1;
	iflags = RS_HIPRI;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if ( ret != 0
	||  ctlbuf.len < sizeof(toa->PRIM_type) )
		return t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
	if ( toa->PRIM_type == T_ERROR_ACK ) {
		struct T_error_ack * tea = (struct T_error_ack *)buf;
		if ( ctlbuf.len < sizeof(struct T_error_ack)
		||  tea->ERROR_prim != prim_type )
			return t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		(void)t_sync(fd);
		t_errno = tea->TLI_error;
		errno = tea->UNIX_error;
		return -1;
	}
	if ( ctlbuf.len < sizeof(struct T_ok_ack)
	||  toa->PRIM_type != T_OK_ACK
	||  toa->CORRECT_prim != prim_type )
		return t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
	return 0;
}
