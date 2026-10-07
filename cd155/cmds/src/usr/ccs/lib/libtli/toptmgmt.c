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
 ** toptmgmt.c 1.3, last change 10/16/89
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <tli/tlistate.h>
#include <tli/tihdr.h>
#include <tli/timod.h>
#ifdef XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_optmgmt (fd, req, ret)
	int	fd;
	struct t_optmgmt * req;
	struct t_optmgmt * ret;
{
	char	buf[MAX_STACK_BUF];
	struct T_optmgmt_req	* toptmgmtr;
	struct T_optmgmt_ack	* toptmgmta;
	struct	tli_st	tli;
	int	total_len;
        int     code;

        code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_state == T_UNBND) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
	total_len = req ? req->opt.len : 0;
	if (ret  &&  ret->opt.maxlen > total_len)
		total_len = ret->opt.maxlen;
	total_len += sizeof(struct T_optmgmt_req);
	if (total_len > sizeof(buf)) {
		t_errno = TBUFOVFLW;
		goto rtn;
	}
	if(ret && ret->opt.maxlen <= 0) {
		 t_errno = TBUFOVFLW;
		goto rtn;
        }
	toptmgmtr = (struct T_optmgmt_req *)&buf[0];
	toptmgmtr->PRIM_type = T_OPTMGMT_REQ;
	if (req) {
		toptmgmtr->MGMT_flags = req->flags;
		toptmgmtr->OPT_length = req->opt.len;
		toptmgmtr->OPT_offset = (char *)&toptmgmtr[1] - (char *)toptmgmtr;
		memcpy((char *)&toptmgmtr[1], req->opt.buf, req->opt.len);
	} else {
		toptmgmtr->MGMT_flags = 0;
		toptmgmtr->OPT_length = 0;
		toptmgmtr->OPT_offset = 0;
	}
	if (tli_ioctl(fd, TI_OPTMGMT, toptmgmtr, total_len) == -1)
		goto rtn;
	if (ret) {
		toptmgmta = (struct T_optmgmt_ack *)&buf[0];
		ret->flags = toptmgmta->MGMT_flags;
		if (toptmgmta->OPT_length > 0  &&  ret->opt.maxlen > 0) {
			if (ret->opt.maxlen < toptmgmta->OPT_length) {
				(void)t_sync(fd);
				t_errno = TBUFOVFLW;
				goto rtn;
			}
			ret->opt.len = toptmgmta->OPT_length;
			memcpy(ret->opt.buf, &buf[toptmgmta->OPT_offset], ret->opt.len);
		} else
			ret->opt.len = 0;
	}
	(void)t_sync(fd);
        code = 0;
rtn:

#ifdef XTIDBG
	tr_optmgmt (fd, req, ret, code);
#endif
	return code;
}
