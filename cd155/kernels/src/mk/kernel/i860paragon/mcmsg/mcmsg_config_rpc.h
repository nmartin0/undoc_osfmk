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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_rpc.h,v 1.6 1994/11/18 20:44:01 mtm Exp $
 */

/*
 * mcmsg_config_rpc.h
 *
 * RPC module configuration
 */

#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_RPC, mcmsg_rpc_init)
	    INQ_NULL(MCMSG_MODULE_RPC)

#endif	INIT_SECTION


#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_RPCSENDRQ	(RPC_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RPCSENDRQ, "RPCSENDRQ",
			mcmsg_rpc_send_request, 4, MCMSG_MODULE_RPC, SYS)

#ifndef MCMSG_CONFIG_H
#define			POST_RPCRECVRQ	(RPC_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RPCRECVRQ, "RPCRECVRQ",
			mcmsg_rpc_recv_request, 3, MCMSG_MODULE_RPC, SYS)

#ifndef MCMSG_CONFIG_H
#define			POST_RPCSENDRP	(RPC_POST_BASE+2)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RPCSENDRP, "RPCSENDRP",
			mcmsg_rpc_send_reply, 2, MCMSG_MODULE_RPC, SYS)

#ifndef	MCMSG_CONFIG_H
#define			POST_RPCREPLYRECV (RPC_POST_BASE+3)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RPCREPLYRECV, "RPCREPLYRECV",
			mcmsg_rpc_reply_recv, 3, MCMSG_MODULE_RPC, SYS)

	    POST_NULL(RPC_POST_BASE+4)

	    POST_NULL(RPC_POST_BASE+5)

	    POST_NULL(RPC_POST_BASE+6)

	    POST_NULL(RPC_POST_BASE+7)

	    POST_NULL(RPC_POST_BASE+8)

	    POST_NULL(RPC_POST_BASE+9)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RPCQ	(RPC_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RPCQ, "RPCQ     ",    "MCTRL_RPCQ", \
				mcmsg_send_rpcq, mcmsg_recv_rpcq)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RPCR	(RPC_PACKET_BASE+1)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RPCR, "RPCR     ",    "MCTRL_RPCR", \
				mcmsg_send_rpcr, mcmsg_recv_rpcr)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RPCN	(RPC_PACKET_BASE+2)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RPCN, "RPCN     ",    "MCTRL_RPCN", \
				mcmsg_send_rpcn, mcmsg_recv_rpcn)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RPCA	(RPC_PACKET_BASE+3)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RPCA, "RPCA     ",    "MCTRL_RPCA", \
				mcmsg_send_rpca, mcmsg_recv_rpca)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_RPCU	(RPC_PACKET_BASE+4)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_RPCA, "RPCU     ",    "MCTRL_RPCU", \
				mcmsg_send_rpcu, mcmsg_recv_rpcu)

		PACKET_FILL(RPC_PACKET_BASE+5)

		PACKET_FILL(RPC_PACKET_BASE+6)

		PACKET_FILL(RPC_PACKET_BASE+7)

		PACKET_FILL(RPC_PACKET_BASE+8)

		PACKET_FILL(RPC_PACKET_BASE+9)

#endif	PACKET_SECTION
