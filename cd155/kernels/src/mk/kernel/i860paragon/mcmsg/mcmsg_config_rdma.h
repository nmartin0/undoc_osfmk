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
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_rdma.h,v 1.4 1994/11/18 20:43:56 mtm Exp $
 */

/*
 * mcmsg_config_rdma.h
 *
 * RDMA module configuration
 */

#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_RDMA, mcmsg_rdma_init)
	    INQ_NULL(MCMSG_MODULE_RDMA)

#endif	INIT_SECTION


#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_RDMAACCEPT	(RDMA_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMAACCEPT, "RDMAACCEPT",
			mcmsg_rdma_accept, 1, MCMSG_MODULE_RDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_RDMACONNECT (RDMA_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMACONNECT, "RDMACONNECT",
			mcmsg_rdma_connect, 2, MCMSG_MODULE_RDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_RDMADISCO	(RDMA_POST_BASE+2)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMADISCO, "RDMADISCO",
			mcmsg_rdma_disconnect, 1, MCMSG_MODULE_RDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_RDMAFLUSH	(RDMA_POST_BASE+3)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMAFLUSH, "RDMAFLUSH",
			mcmsg_rdma_flush, 1, MCMSG_MODULE_RDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_RDMASEND	(RDMA_POST_BASE+4)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMASEND, "RDMASEND",
			mcmsg_rdma_send, 5, MCMSG_MODULE_RDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_RDMARECV	(RDMA_POST_BASE+5)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMARECV, "RDMARECV",
			mcmsg_rdma_recv, 5, MCMSG_MODULE_RDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_RDMAFLUSHREMOTE	(RDMA_POST_BASE+6)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMAFLUSHREMOTE, "RDMAFLUSHREMOTE",
			mcmsg_rdma_flush_remote, 1, MCMSG_MODULE_RDMA, USER)

#ifndef	MCMSG_CONFIG_H
#define			POST_RDMARESUME		(RDMA_POST_BASE+7)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RDMARESUME, "RDMARESUME",
			mcmsg_rdma_resume, 2, MCMSG_MODULE_RDMA, USER)

	    POST_NULL(RDMA_POST_BASE+8)

	    POST_NULL(RDMA_POST_BASE+9)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RDMAC	(RDMA_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RDMAC, "RDMAC     ",    "MCTRL_RDMAC", \
				mcmsg_send_rdmac, mcmsg_recv_rdmac)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RDMAG	(RDMA_PACKET_BASE+1)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RDMAG, "RDMAG     ",    "MCTRL_RDMAG", \
				mcmsg_send_rdmag, mcmsg_recv_rdmag)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RDMAD	(RDMA_PACKET_BASE+2)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RDMAD, "RDMAD     ",    "MCTRL_RDMAD", \
				mcmsg_send_rdmad, mcmsg_recv_rdmad)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RDMAR	(RDMA_PACKET_BASE+3)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RDMAR, "RDMAR     ",    "MCTRL_RDMAR", \
				mcmsg_send_rdmar, mcmsg_recv_rdmar)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RDMAF	(RDMA_PACKET_BASE+4)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RDMAF, "RDMAF     ",    "MCTRL_RDMAF", \
				mcmsg_send_rdmaf, mcmsg_recv_rdmaf)


		PACKET_FILL(RDMA_PACKET_BASE+5)

		PACKET_FILL(RDMA_PACKET_BASE+6)

		PACKET_FILL(RDMA_PACKET_BASE+7)

		PACKET_FILL(RDMA_PACKET_BASE+8)

		PACKET_FILL(RDMA_PACKET_BASE+9)

#endif	PACKET_SECTION
