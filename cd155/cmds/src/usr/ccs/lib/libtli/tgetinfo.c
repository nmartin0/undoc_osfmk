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
 ** tgetinfo.c 1.2, last change 11/8/89
 **/

#include <tli/common.h>
#include <sys/stream.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/timod.h>
#include <errno.h>
#ifdef	XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif


int
t_getinfo (fd, info)
	int	fd;
	struct t_info	* info;
{
	struct T_info_ack	tinfoack;
        int    code;

        code = -1;
	tinfoack.PRIM_type = T_INFO_REQ;
	if (tli_ioctl(fd, TI_GETINFO, (char *)&tinfoack, sizeof(tinfoack)) == -1)
		goto rtn;
	info->addr = tinfoack.ADDR_size;
	info->options = tinfoack.OPT_size;
	info->tsdu = tinfoack.TSDU_size;
	info->etsdu = tinfoack.ETSDU_size;
	info->connect = tinfoack.CDATA_size;
	info->discon = tinfoack.DDATA_size;
	info->servtype = tinfoack.SERV_type;
        code = 0;
rtn:
#ifdef XTIDBG
	tr_getinfo (fd, info, code);
#endif
	return code;
}

int
tli_ioctl (fd, cmd, dp, len)
	int	fd;
	int	cmd;
	char	* dp;
	int	len;
{
	int	ret;
	struct strioctl	stri;

	stri.ic_cmd = cmd;
	stri.ic_timout = -1;
	stri.ic_len = len;
	stri.ic_dp = dp;
	ret = stream_ioctl(fd, I_STR, (char *)&stri);
	if (ret == -1) {
		t_unix_to_tli_error();
		return -1;
	}
	if (ret != 0) {
		t_errno = ret & 0xff;
		errno = ret >> 8;
		return -1;
	}
	return 0;
}
