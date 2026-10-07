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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_urdma.h,v 1.2 1994/11/18 20:44:02 mtm Exp $
 */

/*
 * mcmsg_config_urdma.h
 *
 * URDMA module configuration
 */

#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_URDMA, mcmsg_urdma_init)
	    INQ_NULL(MCMSG_MODULE_URDMA)

#endif	INIT_SECTION


#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_URDMASETUP	(URDMA_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMASETUP, "URDMASETUP",
			mcmsg_urdma_setup, 2, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMAACCEPT	(URDMA_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMAACCEPT, "URDMAACCEPT",
			mcmsg_urdma_accept, 1, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMACONNECT (URDMA_POST_BASE+2)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMACONNECT, "URDMACONNECT",
			mcmsg_urdma_connect, 2, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMADISCO	(URDMA_POST_BASE+3)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMADISCO, "URDMADISCO",
			mcmsg_urdma_disconnect, 1, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMAFLUSH	(URDMA_POST_BASE+4)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMAFLUSH, "URDMAFLUSH",
			mcmsg_urdma_flush, 1, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMASEND	(URDMA_POST_BASE+5)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMASEND, "URDMASEND",
			mcmsg_urdma_send, 3, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMARECV	(URDMA_POST_BASE+6)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMARECV, "URDMARECV",
			mcmsg_urdma_recv, 3, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMAAS	(URDMA_POST_BASE+7)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMAAS, "URDMAAS",
			mcmsg_urdma_accept_send, 3, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMAAR	(URDMA_POST_BASE+8)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMAAR, "URDMAAR",
			mcmsg_urdma_accept_recv, 3, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMARCS	(URDMA_POST_BASE+9)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMARCS, "URDMARCS",
			mcmsg_urdma_reconnect_send, 4, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMARCR	(URDMA_POST_BASE+10)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMARCR, "URDMARCR",
			mcmsg_urdma_reconnect_recv, 4, MCMSG_MODULE_URDMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_URDMAPUT	(URDMA_POST_BASE+11)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_URDMAPUT, "URDMAPUT",
			mcmsg_urdma_put, 5, MCMSG_MODULE_URDMA, USER)

	    POST_NULL(URDMA_POST_BASE+12)

	    POST_NULL(URDMA_POST_BASE+13)

	    POST_NULL(URDMA_POST_BASE+14)

	    POST_NULL(URDMA_POST_BASE+15)

	    POST_NULL(URDMA_POST_BASE+16)

	    POST_NULL(URDMA_POST_BASE+17)

	    POST_NULL(URDMA_POST_BASE+18)

	    POST_NULL(URDMA_POST_BASE+19)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_URDMAC	(URDMA_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_URDMAC, "URDMAC     ",    "MCTRL_URDMAC", \
				mcmsg_send_urdmac, mcmsg_recv_urdmac)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_URDMAR	(URDMA_PACKET_BASE+1)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_URDMAR, "URDMAR     ",    "MCTRL_URDMAR", \
				mcmsg_send_urdmar, mcmsg_recv_urdmar)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_URDMAD	(URDMA_PACKET_BASE+2)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_URDMAD, "URDMAD     ",    "MCTRL_URDMAD", \
				mcmsg_send_urdmad, mcmsg_recv_urdmad)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_URDMAP	(URDMA_PACKET_BASE+3)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_URDMAP, "URDMAP     ",    "MCTRL_URDMAP", \
				mcmsg_send_urdmap, mcmsg_recv_urdmap)

		PACKET_FILL(URDMA_PACKET_BASE+4)

		PACKET_FILL(URDMA_PACKET_BASE+5)

		PACKET_FILL(URDMA_PACKET_BASE+6)

		PACKET_FILL(URDMA_PACKET_BASE+7)

		PACKET_FILL(URDMA_PACKET_BASE+8)

		PACKET_FILL(URDMA_PACKET_BASE+9)

#endif	PACKET_SECTION
