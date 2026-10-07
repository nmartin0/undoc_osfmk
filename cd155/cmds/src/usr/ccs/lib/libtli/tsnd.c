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
 ** tsnd.c 1.2, last change 1/29/90
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/tlistate.h>
#include <errno.h>
#ifdef XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif
#ifdef XTIDBG
#include <tli/tdbg.h>
#endif


int
t_snd (fd, buf, nbytes, flags)
	int	fd;
	char	* buf;
	unsigned nbytes;
	int	flags;
{
	struct strbuf		ctlbuf;
	struct strbuf		databuf;
	long			max_tsdu_size;
	struct T_data_req	tdr;
	struct tli_st	tli;
	long			total;
        int    code;

        code = -1;
	if(iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
                t_errno = TBADF;
                goto rtn;
        }

	if ( tli.tlis_servtype == T_CLTS ) {
		t_errno = TNOTSUPPORT;
		goto rtn;
	}
	
#ifdef XTI
	if ( tli.tlis_state != T_DATAXFER  &&  tli.tlis_state != T_INREL ) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
	if (flags & ~(T_MORE | T_EXPEDITED)) {
		t_errno = TBADFLAG;
		goto rtn;
	}
#else
	if ( tli.tlis_state != T_DATAXFER  &&  tli.tlis_state != T_INREL ) {
		t_errno = TSYSERR;
		errno = EPROTO;
		goto rtn;
	}
#endif
	if (nbytes == 0) {
		t_errno = TBADDATA;
		goto rtn;
	}

	max_tsdu_size = (flags & T_EXPEDITED) ? 
		tli.tlis_etsdu_size : tli.tlis_tsdu_size;
	if (max_tsdu_size == -2
	|| (max_tsdu_size > 0  &&  nbytes > max_tsdu_size)) {
		t_errno = TSYSERR;
		errno = EPROTO;
		goto rtn;
	}

	if(flags & T_EXPEDITED)
		tdr.PRIM_type = T_EXDATA_REQ;
	else
		tdr.PRIM_type = T_DATA_REQ;
	ctlbuf.buf = (char *)&tdr;
	ctlbuf.len = sizeof(tdr);
	databuf.buf = buf;
	
	total = 0;
        do {
                databuf.len = MIN(nbytes, tli.tlis_tidu_size);
                nbytes -= databuf.len;
		if ( (flags & T_MORE) || nbytes != 0)
                        tdr.MORE_flag = T_MORE;
		else
                        tdr.MORE_flag = 0;
			
                if (putmsg(fd, &ctlbuf, &databuf, 0) == -1) {
                        if (total > 0)
                                return total;
#ifdef  XTI
                        if ( t_errno == ERANGE ) {
                                /* Secret message from TIMOD!
                                ** T_DISCONNECT or T_ORDREL waiting
                                */
                                (void)t_sync(fd);
                                t_errno = TLOOK;
                                return -1;
                        }
#endif
                        t_unix_to_tli_error();
                        if ( t_errno == TNODATA ) {
                                t_errno = TFLOW;
#ifdef XTI
                                if (flags & T_EXPEDITED)
					iostate_sw(fd, &tli, IOSTATE_CLEARFLAG,
					TLIS_EXDATA_STOPPED);
                                else
					iostate_sw(fd, &tli, IOSTATE_CLEARFLAG,
					TLIS_DATA_STOPPED);
#endif
                        }
                        return -1;
                }
                databuf.buf += databuf.len;
                total += databuf.len;
        } while (nbytes > 0);
#ifdef XTI
	if (flags & T_EXPEDITED)
		iostate_sw(fd, &tli, IOSTATE_CLEARFLAG, TLIS_EXDATA_STOPPED);
	else
		iostate_sw(fd, &tli, IOSTATE_CLEARFLAG, TLIS_DATA_STOPPED);
#endif
	code = total;
rtn:

#ifdef XTIDBG
	tr_snd (fd, buf, nbytes, flags, code);
#endif
	return code;
}
