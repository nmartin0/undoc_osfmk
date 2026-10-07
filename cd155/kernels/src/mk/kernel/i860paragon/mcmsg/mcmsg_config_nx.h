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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_nx.h,v 1.11 1994/11/18 20:43:54 mtm Exp $
 */

/*
 * mcmsg_config_nx.h
 *
 * NX module configuration
 */

#ifndef MCMSG_CONFIG_H

#endif	MCMSG_CONFIG_H


#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_NX, mcmsg_nx_init)
	    INQ_DEFINE(MCMSG_MODULE_NX, mcmsg_schedule_send)

#endif	INIT_SECTION



#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_TASKINIT	(NX_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_TASKINIT, "TASKINIT",
			mcmsg_init_task, 6, MCMSG_MODULE_GENERAL, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_TASKDEST	(NX_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_TASKDEST, "TASKDEST",
			mcmsg_mp_clear_task, 0, MCMSG_MODULE_GENERAL, SYS)


	    POST_NULL(NX_POST_BASE+2)


#ifndef MCMSG_CONFIG_H
#define			POST_RESPTYPE	(NX_POST_BASE+3)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_RESPTYPE, "RESPTYPE",
			mcmsg_reserve_ptype, 3, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_PROVIDE	(NX_POST_BASE+4)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PROVIDE, "PROVIDE",
			mcmsg_provide, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_MASKTRAP	(NX_POST_BASE+5)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_MASKTRAP, "MASKTRAP",
			mcmsg_masktrap, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_NXSEND	(NX_POST_BASE+6)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_NXSEND, "NXSEND",
			mcmsg_nx_send, 8, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_NXRECVX	(NX_POST_BASE+7)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_NXRECVX, "NXRECVX",
			mcmsg_nx_recvx, 8, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_NXSENDCONT	(NX_POST_BASE+8)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_NXSENDCONT, "NXSENDCO",
			mcmsg_nx_send_continue, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_NXRECVCONT	(NX_POST_BASE+9)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_NXRECVCONT, "NXRECVCO",
			mcmsg_nx_recv_continue, 1, MCMSG_MODULE_NX, USER)


#ifndef MCMSG_CONFIG_H
#define			POST_NXCANCEL	(NX_POST_BASE+10)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_NXCANCEL, "NXCANCEL",
			mcmsg_nx_cancel, 2, MCMSG_MODULE_NX, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_XSEND	(NX_POST_BASE+11)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_XSEND, "XSEND",
			mcmsg_xsend, 1, MCMSG_MODULE_NX, USER)

	    POST_NULL(NX_POST_BASE+12)

	    POST_NULL(NX_POST_BASE+13)

	    POST_NULL(NX_POST_BASE+14)

	    POST_NULL(NX_POST_BASE+15)

	    POST_NULL(NX_POST_BASE+16)

	    POST_NULL(NX_POST_BASE+17)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PNQ	(NX_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PNQ, "PNQ     ",    "MCTRL_PNQ", \
				mcmsg_send_pnq, mcmsg_recv_pnq)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PNQL	(NX_PACKET_BASE+1)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PNQL, "PNQL     ",   "MCTRL_PNQL", \
				mcmsg_send_pnq, mcmsg_recv_pnq)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PAK	(NX_PACKET_BASE+2)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PAK, "PAK     ",    "MCTRL_PAK", \
				mcmsg_send_pak, mcmsg_recv_pak)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PAKL	(NX_PACKET_BASE+3)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PAKL, "PAKL     ",   "MCTRL_PAKL", \
				mcmsg_send_pak, mcmsg_recv_pak)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PNA	(NX_PACKET_BASE+4)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PNA, "PNA     ",    "MCTRL_PNA", \
				mcmsg_send_pna, mcmsg_recv_pna)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PRM	(NX_PACKET_BASE+5)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PRM, "PRM     ",    "MCTRL_PRM", \
				mcmsg_send_prm, mcmsg_recv_prm)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NAT	(NX_PACKET_BASE+6)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NAT, "NAT     ",    "MCTRL_NAT", \
				mcmsg_send_nat, mcmsg_recv_nat)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NAA	(NX_PACKET_BASE+7)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NAA, "NAA     ",    "MCTRL_NAA", \
				mcmsg_send_naa, mcmsg_recv_naa)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NDT	(NX_PACKET_BASE+8)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NDT, "NDT     ",    "MCTRL_NDT", \
				mcmsg_send_ndt, mcmsg_recv_ndt)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NDA	(NX_PACKET_BASE+9)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NDA, "NDA     ",    "MCTRL_NDA", \
				mcmsg_send_nda, mcmsg_recv_nda)

		PACKET_FILL(NX_PACKET_BASE+10)

		PACKET_FILL(NX_PACKET_BASE+11)

		PACKET_FILL(NX_PACKET_BASE+12)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXR	(NX_PACKET_BASE+13)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXR, "NXR     ",    "MCTRL_NXR", \
				mcmsg_send_nxr, mcmsg_recv_nxr)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXA	(NX_PACKET_BASE+14)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXA, "NXA     ",    "MCTRL_NXA", \
				mcmsg_send_nxa, mcmsg_recv_nxa)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXS	(NX_PACKET_BASE+15)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXS, "NXS     ",    "MCTRL_NXS", \
				mcmsg_send_nxs, mcmsg_recv_nxs)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXM	(NX_PACKET_BASE+16)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXM, "NXM     ",    "MCTRL_NXM", \
				mcmsg_send_nxm, mcmsg_recv_nxm)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXQ	(NX_PACKET_BASE+17)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXQ, "NXQ     ",    "MCTRL_NXQ", \
				mcmsg_send_nxq, mcmsg_recv_nxq)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXC	(NX_PACKET_BASE+18)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXC, "NXC     ",    "MCTRL_NXC", \
				mcmsg_send_nxc, mcmsg_recv_nxc)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXF	(NX_PACKET_BASE+19)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXF, "NXF     ",    "MCTRL_NXF", \
				mcmsg_send_nxf, mcmsg_recv_nxf)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NX1	(NX_PACKET_BASE+20)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NX1, "NX1     ",    "MCTRL_NX1", \
				mcmsg_send_nx1, mcmsg_recv_nx1)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_NXN	(NX_PACKET_BASE+21)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_NXN, "NXN     ",    "MCTRL_NXN", \
				mcmsg_send_nxn, mcmsg_recv_nxn)


		PACKET_FILL(NX_PACKET_BASE+22)

		PACKET_FILL(NX_PACKET_BASE+23)

		PACKET_FILL(NX_PACKET_BASE+24)

		PACKET_FILL(NX_PACKET_BASE+25)

		PACKET_FILL(NX_PACKET_BASE+26)

		PACKET_FILL(NX_PACKET_BASE+27)

		PACKET_FILL(NX_PACKET_BASE+28)

		PACKET_FILL(NX_PACKET_BASE+29)

		PACKET_FILL(NX_PACKET_BASE+30)

		PACKET_FILL(NX_PACKET_BASE+31)

		PACKET_FILL(NX_PACKET_BASE+32)

		PACKET_FILL(NX_PACKET_BASE+33)

		PACKET_FILL(NX_PACKET_BASE+34)

		PACKET_FILL(NX_PACKET_BASE+35)

		PACKET_FILL(NX_PACKET_BASE+36)

		PACKET_FILL(NX_PACKET_BASE+37)

		PACKET_FILL(NX_PACKET_BASE+38)

		PACKET_FILL(NX_PACKET_BASE+39)

		PACKET_FILL(NX_PACKET_BASE+40)

		PACKET_FILL(NX_PACKET_BASE+41)

		PACKET_FILL(NX_PACKET_BASE+42)

		PACKET_FILL(NX_PACKET_BASE+43)

		PACKET_FILL(NX_PACKET_BASE+44)

		PACKET_FILL(NX_PACKET_BASE+45)

		PACKET_FILL(NX_PACKET_BASE+46)

		PACKET_FILL(NX_PACKET_BASE+47)

		PACKET_FILL(NX_PACKET_BASE+48)

		PACKET_FILL(NX_PACKET_BASE+49)

#endif	PACKET_SECTION
