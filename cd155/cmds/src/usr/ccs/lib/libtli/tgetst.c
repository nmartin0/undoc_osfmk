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
 ** tgetst.c 1.2, last change 1/29/90
 **/

#include <sys/stream.h>
#include <sys/errno.h>
#include <tli/common.h>
#include <stropts.h>
#include <tli/tihdr.h>
#include <tli/timod.h>
#include <tli/tlistate.h>
#ifdef	XTI
#include <xti.h>
#else
#include <tiuser.h>
#define t_ilook t_look
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif


int
t_getstate (fd)
	int	fd;
{
	struct T_info_ack	tinfoack;
	struct	tli_st	tli;
	int 	ret = -1;
	int	tlk;

	if(iostate_sw(fd, &tli, IOSTATE_VERIFY, 0) == -1) {
                t_errno = TBADF;
                goto rtn;
        }
	tlk = t_ilook(fd);

	switch ( tlk) {
	case T_LISTEN:
		ret = T_INCON;
		goto rtn;
	case T_CONNECT:
		ret = T_OUTCON;
		goto rtn;
	case T_DATA:
	case T_EXDATA:
		ret = T_DATAXFER;
		goto rtn;
	case T_DISCONNECT:
	case T_ORDREL:
	case T_UDERR:
#ifdef T_ERROR
	case T_ERROR:
#endif
	default:
		break;
	}
	tinfoack.PRIM_type = T_INFO_REQ;
	if (tli_ioctl(fd, TI_GETINFO, &tinfoack, sizeof(tinfoack)) == -1)
		goto rtn;
	ret = _txstate(tinfoack.CURRENT_state);
rtn:

#ifdef XTIDBG
	tr_getstate (fd, ret);
#endif
	return ret;
}
