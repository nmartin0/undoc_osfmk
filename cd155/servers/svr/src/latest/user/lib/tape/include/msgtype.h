/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      msgtype.h : 11.1 94/03/25 15:52:04
 */
/** static char	msgtypeh_ver[] = "@(#) msgtype.h 11.1 94/03/25 15:52:04";
/**/

/*
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

#define SIG_TYPE	      	(FORCE_BASE_TYPE   + 50)
#define PGSIGNAL_TYPE         	(FORCE_BASE_TYPE   + 51) /* process group sig */

/*
 *  Remote host types
 */
#define RQ_NEED_FORCE		(MSG_BASE_TYPE   + 60)
#define RP_NEED_FORCE		(MSG_BASE_TYPE   + 61)
#define CFS_RHARP_TYPE		(MSG_BASE_TYPE   + 62)

/* File I/O mode message types */
#define GETOFFS_TYP		(MSG_BASE_TYPE   + 74)
#define GETOPS_TYP		(MSG_BASE_TYPE   + 75)
#define GETEOF_TYP		(MSG_BASE_TYPE   + 76)

/*
 * 	Debugger message types
 */
#define IPD_DATA_TYPE		(MSG_BASE_TYPE  + 80)
#define IPD_EVENT_TYPE 		(MSG_BASE_TYPE  + 81)
#define IPD_READD_RQ_TYPE	(MSG_BASE_TYPE  + 82)
#define IPD_READD_RP_TYPE	(MSG_BASE_TYPE  + 83)


/*
 *	Special adminproc types
 */
#define PLOG_FILENAME		(MSG_BASE_TYPE	+ 90)
#define IPHOSTS			(MSG_BASE_TYPE	+ 91)
#define SETHOST_NAME		(MSG_BASE_TYPE	+ 92)
#define CFS_NDRP_TYPE           (MSG_BASE_TYPE  + 98)

/* CFS multiple reply type - reserve range from 100 to 199 */
#define CFS_MRP_TYPE 		(FORCE_BASE_TYPE + 100)

/*
 * Commutil
 */

#define COMM_TYPE_BASE		500
#define COMM_TYPE_1		(MSG_BASE_TYPE + COMM_TYPE_BASE + 1)
#define COMM_TYPE_2		(MSG_BASE_TYPE + COMM_TYPE_BASE + 2)
#define COMM_TYPE_3		(MSG_BASE_TYPE + COMM_TYPE_BASE + 3)
#define COMM_TYPE_4		(MSG_BASE_TYPE + COMM_TYPE_BASE + 4)
#define COMM_TYPE_5		(MSG_BASE_TYPE + COMM_TYPE_BASE + 5)
#define COMM_TYPE_6		(MSG_BASE_TYPE + COMM_TYPE_BASE + 6)
#define COMM_TYPE_7		(MSG_BASE_TYPE + COMM_TYPE_BASE + 7)
#define COMM_TYPE_8		(MSG_BASE_TYPE + COMM_TYPE_BASE + 8)
#define COMM_TYPE_9		(MSG_BASE_TYPE + COMM_TYPE_BASE + 9)
#define COMM_TYPE_10		(MSG_BASE_TYPE + COMM_TYPE_BASE + 10)
#define COMM_TYPE_11		(MSG_BASE_TYPE + COMM_TYPE_BASE + 11)
#define COMM_TYPE_12		(MSG_BASE_TYPE + COMM_TYPE_BASE + 12)
#define COMM_TYPE_13		(MSG_BASE_TYPE + COMM_TYPE_BASE + 13)
#define COMM_TYPE_14		(MSG_BASE_TYPE + COMM_TYPE_BASE + 14)

/*
* CFS IOMODE messages:
*/
#define CFS_IOMODE1_BASE        1000

#define GETPOS_TYP              (MSG_BASE_TYPE   + CFS_IOMODE1_BASE) /* 1000 */
#define XMTPOS_TYP              (GETPOS_TYP + FILE_TABLE_SIZE) /* 1100 */
#define NEWPOS_TYP              (XMTPOS_TYP + FILE_TABLE_SIZE) /* 1200 */
#define SETMODE_TYP             (NEWPOS_TYP + FILE_TABLE_SIZE) /* 1300 */
