/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	msgtype.h : 8.4 91/03/14 15:02:49
 *
 * .{ TEXT
 * .H 4 msgtype.h
 *
 * This module defines all system message TYPEs.
 * 
 * .} TEXT
 */

/*
 * First number in the Force type range
 */

#ifndef FORCE_TYPE
#define FORCE_TYPE		0x40000000	/* 1073741824 decimal */
#endif

/* The Reserved System Message Types Bases */
#define MSG_BASE_TYPE		1000000000
#define FORCE_BASE_TYPE		2000000000
#define IFORCE_TYPE		2100000000

/*
 * Commutil
 */

#define COMM_TYPE_1		(MSG_BASE_TYPE   +  1)
#define COMM_TYPE_2		(MSG_BASE_TYPE   +  2)
#define COMM_TYPE_3		(MSG_BASE_TYPE   +  3)
#define COMM_TYPE_4		(MSG_BASE_TYPE   +  4)
#define COMM_TYPE_5		(MSG_BASE_TYPE   +  5)
#define COMM_TYPE_6		(MSG_BASE_TYPE   +  6)
#define COMM_TYPE_7		(MSG_BASE_TYPE   +  7)

/*
 * Loader
 */

#define COMMAND_TYPE		(FORCE_BASE_TYPE + 10)
#define RESPONSE_TYPE		(FORCE_BASE_TYPE + 11)
#define CONTENTS_TYPE		(MSG_BASE_TYPE   + 12)
#define ARGS_TYPE		(MSG_BASE_TYPE   + 13)
#define LOAD_SYS_TYPE		(FORCE_BASE_TYPE + 14)
#define LOADER_RQ_TYPE		(MSG_BASE_TYPE   + 15)
#define LOADER_RP_TYPE		(FORCE_BASE_TYPE + 16)
#define WAIT_RP_TYPE		(MSG_BASE_TYPE   + 17)

/*
 * Fileio
 */

#define FILE_RQ_TYPE		(MSG_BASE_TYPE   + 20)
#define FILE_RP_TYPE		(FORCE_BASE_TYPE + 21)
#define DATA_RQ_TYPE		(FORCE_BASE_TYPE + 22)
#define FDS_TYPE		(MSG_BASE_TYPE   + 23)
#define FPS_TYPE		(MSG_BASE_TYPE   + 24)
#define SOCK_RQ_TYPE		(MSG_BASE_TYPE   + 25)
#define SOCK_RP_TYPE		(MSG_BASE_TYPE   + 26)
#define SOCK_RW_TYPE		(MSG_BASE_TYPE   + 27)
#define SIO_TYPE		(MSG_BASE_TYPE   + 28)
#define INHERIT_TYPE          (MSG_BASE_TYPE   + 29)

/*
 * CFS
 */

#define CFS_DRQ_TYPE		(MSG_BASE_TYPE   + 30)
#define CFS_DRP_TYPE		(FORCE_BASE_TYPE + 31)
#define CFS_NRQ_TYPE		(MSG_BASE_TYPE   + 32)
#define CFS_NRP_TYPE		(FORCE_BASE_TYPE + 33)
#define CFS_OWNER_TYPE		(MSG_BASE_TYPE   + 34)
#define CFS_INVALIDATE_TYPE	(MSG_BASE_TYPE   + 35)
#define CFS_DRQ2_TYPE		(MSG_BASE_TYPE   + 36)
#define CFS_ARQ_TYPE		(MSG_BASE_TYPE   + 37)
#define CFS_ARP_TYPE		(FORCE_BASE_TYPE + 38)
#define CFS_IRP_TYPE		(FORCE_BASE_TYPE + 39)

/*
 * Commser and partitioning utils
 */

#define COMM_RQ_TYPE		(FORCE_BASE_TYPE + 40)
#define COMM_RP_TYPE		(FORCE_BASE_TYPE + 41)
#define GETCUBE_RQ_TYPE		(FORCE_BASE_TYPE + 42)
#define RELCUBE_RQ_TYPE		(FORCE_BASE_TYPE + 43)
#define ACCOUNT_RQ_TYPE		(FORCE_BASE_TYPE + 44)

/*
 * Signal
 */

#define SIG_TYPE		(MSG_BASE_TYPE   + 50)
#define PGSIGNAL_TYPE         (FORCE_BASE_TYPE   + 51) /* process group sig */

/*
 *  Remote host types
 */
#define RQ_NEED_FORCE		(MSG_BASE_TYPE   + 60)
#define RP_NEED_FORCE		(MSG_BASE_TYPE   + 61)
#define CFS_RHARP_TYPE		(MSG_BASE_TYPE   + 62)

/* File I/O mode message types */
#define SETMODE_TYP		(MSG_BASE_TYPE   + 70)
#define GETPOS_TYP		(MSG_BASE_TYPE   + 71)
#define XMTPOS_TYP		(MSG_BASE_TYPE   + 72)
#define NEWPOS_TYP		(MSG_BASE_TYPE   + 73)
#define GETOFFS_TYP		(MSG_BASE_TYPE   + 74)
#define GETOPS_TYP		(MSG_BASE_TYPE   + 75)
#define GETEOF_TYP		(MSG_BASE_TYPE   + 76)

/* CFS multiple reply type - reserve range from 100 to 199 */
#define CFS_MRP_TYPE 		(FORCE_BASE_TYPE + 100)

/*
 * 	Debugger message types
 */
#define DECON_DATA_TYPE		(MSG_BASE_TYPE  + 80)
#define DECON_EVENT_TYPE 	(MSG_BASE_TYPE  + 81)
#define DECON_READD_RQ_TYPE     (MSG_BASE_TYPE  + 82)
#define DECON_READD_RP_TYPE     (MSG_BASE_TYPE  + 83)


/*
 *	Special adminproc types
 */
#define PLOG_FILENAME		(MSG_BASE_TYPE	+ 90)
#define IPHOSTS			(MSG_BASE_TYPE	+ 91)
#define SETHOST_NAME		(MSG_BASE_TYPE	+ 92)

