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
 ** trudata.c 1.3, last change 10/16/89
 **/

#include <tli/common.h>
#include <errno.h>
#include <sys/stream.h>
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
t_rcvudata (fd, ud, flags)
	int	fd;
reg	struct t_unitdata * ud;
	int	* flags;
{
	char	buf[sizeof(struct T_unitdata_ind) + 256];
reg	struct T_unitdata_ind	* tudi = (struct T_unitdata_ind *)buf;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int	iflags;
	int	len;
	int	ret;
	struct tli_st	tli;
        int     code;

        code = -1;
	if(iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if ( tli.tlis_servtype != T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
#ifdef XTI
	if ( tli.tlis_state != T_IDLE ) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#endif
	if((ud->addr.maxlen == 0) || (ud->opt.maxlen == 0)) {
		t_errno = TBUFOVFLW;
		goto rtn;
	}

	len = sizeof(*tudi) + ud->addr.maxlen + ud->opt.maxlen;
	if (len > sizeof(buf)) {
		ctlbuf.maxlen = len;
		if (!(tudi = (struct T_unitdata_ind *)malloc(len))) {
			t_errno = ENOMEM;
			return -1;
		}
	} else {
		ctlbuf.maxlen = sizeof(buf);
		tudi = (struct T_unitdata_ind *)buf;
	}
	ctlbuf.buf = (char *)tudi;
	ctlbuf.len = 0;

	iflags = 0;
	databuf.len = 0;
	ud->addr.len = 0;
	ud->opt.len = 0;
	databuf.buf = ud->udata.buf;
	databuf.maxlen = ud->udata.maxlen;

	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if (ret == -1
	|| ((len = ctlbuf.len) > 0  &&  len < sizeof(struct T_unitdata_ind))) {
fixup:		ret = t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		if ( (char *)tudi != buf )
                        free((char *)tudi);
		return (ret < 0) ? -1 : 0;
	}
	if ( flags )
                *flags = (ret & MOREDATA) ? T_MORE : 0;
        ud->udata.len = databuf.len;
        if (len >= sizeof(struct T_unitdata_ind)) {
		if (tudi->PRIM_type != T_UNITDATA_IND)
				goto fixup;
		len = tudi->SRC_length;
		if (len > 0  &&  ud->addr.maxlen > 0) {
			if (ud->addr.maxlen < len) {
				t_errno = TBUFOVFLW;
				if ( (char *)tudi != buf )
					free((char *)tudi);
				return -1;
			}
			ud->addr.len = len;
			memcpy(ud->addr.buf, &ctlbuf.buf[tudi->SRC_offset], len);
		}
		len = tudi->OPT_length;
		if (len > 0  &&  ud->opt.maxlen > 0) {
			if (ud->opt.maxlen < len) {
				t_errno = TBUFOVFLW;
				if ( (char *)tudi != buf )
					free((char *)tudi);
				return -1;
			}
			ud->opt.len = len;
			memcpy(ud->opt.buf, &ctlbuf.buf[tudi->OPT_offset], len);
		}
	} else if (databuf.len == 0) {
		t_errno = TNODATA;
		if ( (char *)tudi != buf )
			free((char *)tudi);
		return -1;
	}
	if ((char *)tudi != buf)
		free((char *)tudi);
	code = 0;
	if (*flags & T_MORE) {
		if (! (tli.tlis_flags & IO_MORE_RUDATA))
			iostate_sw(fd, &tli, IOSTATE_SETFLAG, IO_MORE_RUDATA);
		else {
			ud->addr.len = 0;
			ud->opt.len = 0;
		}
	} else {
		if (tli.tlis_flags & IO_MORE_RUDATA) {
			ud->addr.len = 0;
			ud->opt.len = 0;
		}
		iostate_sw(fd, &tli, IOSTATE_CLEARFLAG, IO_MORE_RUDATA);
	}
		
rtn:
#ifdef XTIDBG
	tr_rcvudata (fd, ud, flags, code);
#endif
	return code;
}
