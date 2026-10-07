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
 ** trcv.c 1.1, last change 8/9/89
 **/

#include <tli/common.h>
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
t_rcv (fd, buf, nbytes, flags)
	int	fd;
	char	* buf;
	unsigned nbytes;
	int	* flags;
{
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int		iflags;
	int		ret;
	struct T_data_ind tdi;
	struct tli_st	tli;
        int     code;

        code = -1;
	if(iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
#ifdef XTI
	if ( tli.tlis_state != T_DATAXFER  &&  tli.tlis_state != T_OUTREL ) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
#endif
	if ( tli.tlis_servtype == T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
	ctlbuf.buf = (char *)&tdi;
	ctlbuf.maxlen = sizeof(tdi);
	databuf.buf = buf;
	databuf.maxlen = nbytes;
	iflags = 0;
	ret = getmsg(fd, &ctlbuf, &databuf, &iflags);
	if (ret == -1  ||  ret & MORECTL
	|| (ctlbuf.len != -1 && (ctlbuf.len != sizeof(tdi) || 
	(tdi.PRIM_type != T_DATA_IND && tdi.PRIM_type != T_EXDATA_IND)))) {
		code = t_fixup(fd, &ctlbuf, &databuf, iflags, ret);
		goto rtn;
	}
	*flags = tdi.MORE_flag |  
			((tdi.PRIM_type == T_EXDATA_IND) ? T_EXPEDITED : 0);
	*flags |= ret & MOREDATA ? T_MORE: 0;
	code = databuf.len;
rtn:

#ifdef XTIDBG
        tr_rcv (fd, buf, nbytes, flags, code);
#endif
	return code;
}
