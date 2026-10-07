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
/** Copyright (c) 1990  Mentat Inc.
 ** iostate.c 1.1, last change 6/4/90
 **/

/*** "iostate.c\t\t1.1"; ***/

#include <tli/common.h>
#include <sys/stream.h>
#include <tli/tlistate.h>
#include <tli/tihdr.h>
#include <tli/timod.h>
#include <errno.h>
#ifdef XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif

#define	FD_HASH_LIST_LEN	32

typedef	struct iost_s {
	struct iost_s	* iost_next;
	int		iost_fd;
	char		*iost_magic;
} IOST, * IOSTP, ** IOSTPP;

static char TLI_XTI_NUMBER[] = "_xti_tli";

static	int iostate_free();
static	IOSTP	iostate[FD_HASH_LIST_LEN];
static  boolean initialized = false;

iostate_sw(fd, tlis, cmd, flags)
int	fd;
struct	tli_st *tlis;
int	cmd, flags;
{
	reg     IOSTP   ip1;
	reg     IOSTPP  ipp;
	struct	tli_st *tli;
	struct T_info_ack       tinfoack;
	int ret;

	
	ret = 0;
	tli = NULL;
	if ( !initialized ) {
                for ( ipp = iostate; ipp < A_END(iostate); ipp++ )
                        *ipp = nilp(IOST);
                initialized = true;
        }
	if((cmd & IOSTATE_LOOKUP) == IOSTATE_LOOKUP) {
		for ( ipp = &iostate[fd % FD_HASH_LIST_LEN]; ip1 = *ipp; 
		   ipp = &ip1->iost_next ) {
                	if ( ip1->iost_fd == fd ) {
                        	if ( ip1->iost_magic == (char *)TLI_XTI_NUMBER){
					tli = (struct tli_st *)&ip1[1];
					break;
				}
                        }
        	}
	}

	switch(cmd) {

	   case IOSTATE_VERIFY:
		if(!tli) {
			errno = EBADF;
			ret = -1;
		} else
			memmove(tlis, tli, (size_t)sizeof(struct tli_st));
		break;

	   case IOSTATE_CREATE:
		if(!tli) { 
			ip1 = (IOSTP)malloc(sizeof(IOST) +  
				sizeof(struct tli_st));
			if ( !ip1 ) {
				errno = ENOMEM;
				break;
			}
			ip1->iost_next = nilp(IOST);
			ip1->iost_fd = fd;
			ip1->iost_magic = TLI_XTI_NUMBER;
			*ipp = ip1;
			tli = (struct tli_st *)&ip1[1];
			tli->tlis_flags = 0;
		}
		tinfoack.PRIM_type = T_INFO_REQ;
		if (tli_ioctl(fd, TI_GETINFO, (char *)&tinfoack, 
			sizeof(tinfoack)) == -1) {
			ret = -1;
			iostate_free(fd);
			errno = EBADF;
			break;
		}
                tli->tlis_servtype = tinfoack.SERV_type;
                tli->tlis_etsdu_size = tinfoack.ETSDU_size;
                tli->tlis_tsdu_size = tinfoack.TSDU_size;
                tli->tlis_tidu_size = tinfoack.TIDU_size;;
                if((tli->tlis_state = _txstate(tinfoack.CURRENT_state)) == -1){
			ret = -1;
			break;
		}
		memmove(tlis, tli, (size_t)sizeof(struct tli_st));
		break;

	   case IOSTATE_SETFLAG:
		if(!tli) {
			ret = -1;
			errno = EBADF;
		} else
			tli->tlis_flags |= flags;
		break;

	   case IOSTATE_CLEARFLAG:
		if(!tli) {
			ret = -1;
			errno = EBADF;
		} else
			tli->tlis_flags &= ~flags;
		break;

	   case IOSTATE_FREE:
		ret = iostate_free(fd);
		break;
	   case IOSTATE_USTATE:
		tinfoack.PRIM_type = T_INFO_REQ;
		if (tli_ioctl(fd, TI_GETINFO, (char *)&tinfoack, 
			sizeof(tinfoack)) == -1) {
			ret = -1;
			break;
		}
                tli->tlis_servtype = tinfoack.SERV_type;
                tli->tlis_etsdu_size = tinfoack.ETSDU_size;
                tli->tlis_tsdu_size = tinfoack.TSDU_size;
                tli->tlis_tidu_size = tinfoack.TIDU_size;;
                if ((tli->tlis_state = _txstate(tinfoack.CURRENT_state)) == -1){
			ret = -1;
			break;
		}
		memmove(tlis, tli, (size_t)sizeof(struct tli_st));
		break;
	   default:
		ret = -1;
	}
	return(ret);
}

static int
iostate_free (fd)
	int	fd;
{
reg	IOSTP	ip1;
reg	IOSTPP	ipp;

	for ( ipp = &iostate[fd % FD_HASH_LIST_LEN]; ip1 = *ipp; ipp = &ip1->iost_next ) {
		if ( ip1->iost_fd == fd ) {
			*ipp = ip1->iost_next;
			free(ip1);
			return 0;
		}
	}
	return -1;
}
int
_txstate(st)
int st;
{
	switch (st) {
        case TS_UNBND:
                return T_UNBND;
        case TS_IDLE:
                return T_IDLE;
        case TS_WCON_CREQ:
                return T_OUTCON;
        case TS_WRES_CIND:
                return T_INCON;
        case TS_DATA_XFER:
                return T_DATAXFER;
        case TS_WIND_ORDREL:
                return T_OUTREL;
        case TS_WREQ_ORDREL:
                return T_INREL;
        case TS_WACK_BREQ:
        case TS_WACK_CREQ:
        case TS_WACK_CRES:
        case TS_WACK_DREQ6:
        case TS_WACK_DREQ7:
        case TS_WACK_DREQ9:
        case TS_WACK_DREQ10:
        case TS_WACK_DREQ11:
        case TS_WACK_OPTREQ:
        case TS_WACK_ORDREL:
        case TS_WACK_UREQ:
		t_errno = TSTATECHNG;
		break;
        default:
                t_errno = TSYSERR;
                errno = EPROTO;
                break;
        }
	return -1;
}
