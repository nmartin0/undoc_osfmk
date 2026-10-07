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
 ** tbind.c 1.3, last change 10/16/89
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
t_bind (fd, req, ret)
	int	fd;
	struct t_bind	* req;
	struct t_bind	* ret;
{
	char	buf[TLI_STACK_BUF_SIZE];
	struct T_bind_req	* tbindr;
	struct T_bind_ack	* tbinda;
	struct	tli_st	tli;
	int	total_len;
	int	code;

	code = -1;
	if (iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
		t_errno = TBADF;
		goto rtn;
	}
	if (tli.tlis_state != T_UNBND) {
		t_errno = TOUTSTATE;
		goto rtn;
	}
		
	total_len = req ? req->addr.len : 0;
	if (ret  &&  ret->addr.maxlen > total_len)
		total_len = ret->addr.maxlen;
	total_len += sizeof(struct T_bind_req);
	if (total_len > sizeof(buf)) {
		t_errno = TBUFOVFLW;
		goto rtn;
	}
	tbindr = (struct T_bind_req *)&buf[0];
	tbindr->PRIM_type = T_BIND_REQ;
	if (req) {
		tbindr->CONIND_number = req->qlen;
		tbindr->ADDR_length = req->addr.len;
		tbindr->ADDR_offset = (char *)&tbindr[1] - (char *)tbindr;
		memcpy((char *)&tbindr[1], req->addr.buf, req->addr.len);
	} else {
		tbindr->CONIND_number = 0;
		tbindr->ADDR_length = 0;
		tbindr->ADDR_offset = 0;
	}
	if (tli_ioctl(fd, TI_BIND, tbindr, total_len) == -1)
		goto rtn;
	if (ret) {
		tbinda = (struct T_bind_ack *)&buf[0];
		ret->qlen = tbinda->CONIND_number;
		if (tbinda->ADDR_length > 0  &&  ret->addr.maxlen > 0) {
			if (ret->addr.maxlen < tbinda->ADDR_length) {
				t_errno = TBUFOVFLW;
				goto rtn;
			}
			ret->addr.len = tbinda->ADDR_length;
			memcpy(ret->addr.buf, &buf[tbinda->ADDR_offset], ret->addr.len);
		} else
			ret->addr.len = 0;
	}
	(void)t_sync(fd);
	code = 0;
rtn:

#ifdef XTIDBG
	tr_bind (fd, req, ret, code);
#endif
	if(ret && (ret->addr.maxlen == 0)) {
		t_errno = TBUFOVFLW;
		code = -1;
	}
	return code;
}
