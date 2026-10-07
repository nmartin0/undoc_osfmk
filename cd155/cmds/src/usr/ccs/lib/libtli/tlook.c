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
 ** tlook.c 1.2, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/tlistate.h>
#ifdef XTI
#include <xti.h>
#include <sys/poll.h>
#else
#include <tiuser.h>
#endif
#include <errno.h>

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

#ifndef	XTI
#define	t_ilook	t_look
#endif


int
t_ilook (fd)
	int	fd;
{
	long	type;
	struct strpeek	strp;
	struct tli_st	tli;
        int    code;

        code = -1;
	 if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	strp.ctlbuf.buf = (char *)&type;
	strp.ctlbuf.maxlen = sizeof(type);
	strp.databuf.buf = nilp(char);
	strp.databuf.maxlen = -1;
	strp.flags = 0;
	switch (stream_ioctl(fd, I_PEEK, (char *)&strp)) {
	case -1:
#ifndef XTI
		if (errno == EPROTO)
			code = T_ERROR;
#endif
	        t_unix_to_tli_error();
		goto rtn;
	case 0:
		code = 0;
		goto rtn;
	case 1:
		break;
	}
	if (strp.ctlbuf.len != sizeof(type)) {
		code = 0;
		goto rtn;
        }
	switch (type) {
	case T_CONN_CON:
		code = T_CONNECT;
		break;
	case T_CONN_IND:
		code = T_LISTEN;
		break;
	case T_DATA_IND:
		code = T_DATA;
		break;
	case T_DISCON_IND:
		code = T_DISCONNECT;
		break;
	case T_EXDATA_IND:
		code = T_EXDATA;
		break;
	case T_ORDREL_IND:
		code = T_ORDREL;
		break;
	case T_UNITDATA_IND:
		code = T_DATA;
		break;
	case T_UDERROR_IND:
		code = T_UDERR;
		break;
	default:
		errno = EPROTO;
		t_errno = TSYSERR;
		break;
	}
rtn:
#ifndef XTI
#ifdef XTIDBG
	tr_look (fd, code);
#endif /* XTIDBG */
#endif /* XTI */
	return code;
}

#ifdef	XTI
int
t_look (fd)
	int	fd;
{
	int		i1;
	struct tli_st	tli;
	struct pollfd	fds[1];

	i1 = t_ilook(fd);
	if ( i1 != 0 )
		return i1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0)
	&&  (tli.tlis_flags & (TLIS_DATA_STOPPED | TLIS_EXDATA_STOPPED))) {
		fds[0].fd = fd;
		fds[0].events = POLLOUT;
		if (poll(fds, 1L, 0L) == 1
		&&  fds[0].revents == POLLOUT) {
			if (tli.tlis_flags & TLIS_EXDATA_STOPPED) {
				iostate_sw(fd, &tli, IOSTATE_CLEARFLAG, TLIS_EXDATA_STOPPED);
				i1 = T_GOEXDATA;
			}
			else {
				iostate_sw(fd, &tli, IOSTATE_CLEARFLAG, TLIS_DATA_STOPPED);
				i1 = T_GODATA;
		        }
		}
	}
#ifdef XTIDBG
	tr_look (fd, i1);
#endif /* XTIDBG */
	return i1;
}
#endif
