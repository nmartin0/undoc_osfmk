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

#include <errno.h>
#include <tli/common.h>
#ifdef	XTI
#include <xti.h>
#define	T_BIND		T_BIND_STR
#define	T_CALL		T_CALL_STR
#define	T_OPTMGMT	T_OPTMGMT_STR
#define	T_DIS		T_DIS_STR
#define	T_UNITDATA	T_UNITDATA_STR
#define	T_UDERROR	T_UDERROR_STR
#define	T_INFO		T_INFO_STR
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

#ifdef	_NO_PROTO

extern	void	free();

#else

extern	void	free(char * ptr);

#endif


int
t_free (ptr, struct_type)
	char	* ptr;
	int	struct_type;
{
reg	struct netbuf	* addrp = nilp(struct netbuf);
	struct netbuf	* optp = nilp(struct netbuf);
	struct netbuf	* datap = nilp(struct netbuf);
	union {
		struct t_bind	* t_b;
		struct t_call	* t_c;
		struct t_info	* t_i;
		struct t_optmgmt * t_o;
		struct t_discon	* t_d;
		struct t_unitdata * t_u;
		struct t_uderr	* t_ud;
	} up;
        int    code;

        code = -1;
	if (!(up.t_b = (struct t_bind *)ptr)) {
	        code = 0;
		goto rtn;
	}
	switch (struct_type) {
	case T_BIND:
		addrp = &up.t_b->addr;
		break;
	case T_CALL:
		datap = &up.t_c->udata;
		optp = &up.t_c->opt;
		addrp = &up.t_c->addr;
		break;
	case T_OPTMGMT:
		optp = &up.t_o->opt;
		break;
	case T_DIS:
		datap = &up.t_d->udata;
		break;
	case T_INFO:
		break;
	case T_UNITDATA:
		datap = &up.t_u->udata;
		optp = &up.t_u->opt;
		addrp = &up.t_u->addr;
		break;
	case T_UDERROR:
		optp = &up.t_ud->opt;
		addrp = &up.t_ud->addr;
		break;
	default:
		errno = EINVAL;
		t_errno = TSYSERR;
		goto rtn;
	}
	if (datap  &&  datap->buf)
		free(datap->buf);
	if (optp  &&  optp->buf)
		free(optp->buf);
	if (addrp  &&  addrp->buf)
		free(addrp->buf);
	free((char *)up.t_b);
        code = 0;
rtn:

#ifdef XTIDBG
	tr_free (struct_type, code);
#endif
	return code;
}
