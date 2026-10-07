/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_post.h,v 1.5 1994/11/18 20:44:31 mtm Exp $
 */

/*
 * Entry points to the message passing code.
 */

#ifndef MCMSG_POST_H

#define	POST_EMPTY		0

/*
 * Module identification for trace control
 */

#define	MCMSG_MODULE_GENERAL	0
#define	MCMSG_MODULE_IPC	1
#define	MCMSG_MODULE_BOOT	2
#define	MCMSG_MODULE_CONSOLE	3
#define	MCMSG_MODULE_NX		4

#ifndef	MCMSG_MODULE
#define	MCMSG_MODULE		MCMSG_MODULE_GENERAL
#endif	MCMSG_MODULE

#endif	MCMSG_POST_H

#ifndef	POST_NULL
#define POST_NULL(m)
#endif	POST_NULL

#ifndef	POST_DEFINE
#define POST_DEFINE(m,n,p,a,i,u)
#endif	POST_DEFINE

#define			USER 1
#define			SYS  0


	    POST_NULL(0)

#ifndef MCMSG_POST_H
#define			POST_NOP	1
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_NOP, "NOP",
			mcmsg_nop, 0, MCMSG_MODULE_GENERAL, USER)

#ifndef MCMSG_POST_H
#define			POST_PMAPUPDATE	2
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_PMAPUPDATE, "PMAPUPD",
			msgp_pmap_update_interrupt, 0, MCMSG_MODULE_GENERAL, SYS)


#ifndef MCMSG_POST_H
#define			POST_IPCSEND	3
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_IPCSEND, "IPCSEND",
			ipcreq_send, 0, MCMSG_MODULE_IPC, SYS)


#ifndef MCMSG_POST_H
#define			POST_IPCRECV	4
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_IPCRECV, "IPCRECV",
			ipcreq_recv, 2, MCMSG_MODULE_IPC, SYS)


#ifndef MCMSG_POST_H
#define			POST_BOOTSEND	5
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_BOOTSEND, "BOOTSEND",
			mcmsg_boot_send, 0, MCMSG_MODULE_BOOT, SYS)


#ifndef MCMSG_POST_H
#define			POST_CONOPEN	6
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_CONOPEN, "CONOPEN",
			mcmsg_console_open, 2, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_POST_H
#define			POST_CONCLOSE	7
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_CONCLOSE, "CONCLOSE",
			mcmsg_console_close, 2, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_POST_H
#define			POST_CONREAD	8
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_CONREAD, "CONREAD",
			mcmsg_console_read, 2, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_POST_H
#define			POST_CONWRITE	9
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_CONWRITE, "CONWRITE",
			mcmsg_console_write, 3, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_POST_H
#define			POST_CONPUT	10
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_CONPUT, "CONPUT",
			mcmsg_console_do_put, 0, MCMSG_MODULE_CONSOLE, SYS)

#ifndef MCMSG_POST_H
#define			POST_DFLUSH	11
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_DFLUSH, "DFLUSH",
			msgp_dflush_interrupt, 0, MCMSG_MODULE_GENERAL, SYS)


#ifndef MCMSG_POST_H
#define			POST_TASKINIT	12
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_TASKINIT, "TASKINIT",
			mcmsg_init_task, 6, MCMSG_MODULE_GENERAL, SYS)


#ifndef MCMSG_POST_H
#define			POST_TASKDEST	13
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_TASKDEST, "TASKDEST",
			mcmsg_mp_clear_task, 0, MCMSG_MODULE_GENERAL, SYS)

	    POST_NULL(14)


#ifndef MCMSG_POST_H
#define			POST_RESPTYPE	15
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_RESPTYPE, "RESPTYPE",
			mcmsg_reserve_ptype, 2, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_PROVIDE	16
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_PROVIDE, "PROVIDE",
			mcmsg_provide, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_MASKTRAP	17
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_MASKTRAP, "MASKTRAP",
			mcmsg_masktrap, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_NXSEND	18
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_NXSEND, "NXSEND",
			mcmsg_nx_send, 7, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_NXRECVX	19
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_NXRECVX, "NXRECVX",
			mcmsg_nx_recvx, 8, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_NXSENDCONT	20
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_NXSENDCONT, "NXSENDCO",
			mcmsg_nx_send_continue, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_NXRECVCONT	21
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_NXRECVCONT, "NXRECVCO",
			mcmsg_nx_recv_continue, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_POST_H
#define			POST_NXCANCEL	22
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_NXCANCEL, "NXCANCEL",
			mcmsg_nx_cancel, 1, MCMSG_MODULE_NX, USER)

#ifndef MCMSG_POST_H
#define			POST_XSEND	23
#endif	MCMSG_POST_H
	    POST_DEFINE(POST_XSEND, "XSEND",
			mcmsg_xsend, 1, MCMSG_MODULE_NX, USER)


	    POST_NULL(24)

	    POST_NULL(25)

	    POST_NULL(26)

	    POST_NULL(27)

	    POST_NULL(28)

	    POST_NULL(29)

	    POST_NULL(30)

	    POST_NULL(31)


#undef	USER
#undef	SYS
#undef	POST_DEFINE
#undef	POST_NULL
#ifndef MCMSG_POST_H

#define			POST_MAX	32

#define			MCMSG_POST_H
#endif	MCMSG_POST_H
