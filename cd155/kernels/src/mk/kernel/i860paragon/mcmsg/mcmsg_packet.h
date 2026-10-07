/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_packet.h,v 1.8 1994/11/18 20:44:30 mtm Exp $
 */

/*
 * mcmsg_packet.h
 *
 * Packet type definition macros
 */

#ifndef	MCMSG_PACKET_H

#define	MCTRL_ALLOW_LO	MCTRL_BT0
#define	MCTRL_ALLOW_HI	MCTRL_BT3
#define	MCTRL_END	70

#define	SENDMETH_NULL	0
#define	SENDMETH_RK1A	MCTRL_END
#define	SENDMETH_END	(MCTRL_END+1)
#define	SENDMETH_BOOT	SENDMETH_BT0

#endif	MCMSG_PACKET_H

#ifndef PACKET_DEFINE
#define	PACKET_DEFINE(n, name, sendname, send, recv)
#endif	PACKET_DEFINE

#ifndef PACKET_NULL
#define	PACKET_NULL(x)
#endif	PACKET_NULL

#ifndef PACKET_FILL
#define	PACKET_FILL(x)
#endif	PACKET_FILL

#ifndef PACKET_END
#define	PACKET_END(x)
#endif	PACKET_END



/*
 * Packet definitions
 */

		PACKET_NULL(	0)

#if	NORMA
#ifndef	MCMSG_PACKET_H
#define				MCTRL_IPC	1
#define				SENDMETH_IPC	MCTRL_IPC
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_IPC, "IPC     ",    "SENDMETH_IPC", \
				mcmsg_send_ipc, mcmsg_recv_ipc)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_IPCQ	2
#define				SENDMETH_IPCQ	MCTRL_IPCQ
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_IPCQ, "IPCQ    ",    "SENDMETH_IPCQ", \
				mcmsg_send_ipcq, mcmsg_recv_ipcq)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_IPCA	3
#define				SENDMETH_IPCA	MCTRL_IPCA
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_IPCA, "IPCA    ",    "SENDMETH_IPCA", \
				mcmsg_send_ipca, mcmsg_recv_ipca)
#else	NORMA
		PACKET_FILL(    1)

		PACKET_FILL(    2)

		PACKET_FILL(    3)
#endif	NORMA

		PACKET_FILL(	4)

		PACKET_FILL(	5)

		PACKET_FILL(	6)

		PACKET_FILL(	7)

		PACKET_FILL(	8)

		PACKET_FILL(	9)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_PNQ	10
#define				SENDMETH_PNQ	MCTRL_PNQ
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_PNQ, "PNQ     ",    "SENDMETH_PNQ", \
				mcmsg_send_pnq, mcmsg_recv_pnq)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_PNQL	11
#define				SENDMETH_PNQL	MCTRL_PNQL
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_PNQL, "PNQL     ",   "SENDMETH_PNQL", \
				mcmsg_send_pnq, mcmsg_recv_pnq)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_PAK	12
#define				SENDMETH_PAK	MCTRL_PAK
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_PAK, "PAK     ",    "SENDMETH_PAK", \
				mcmsg_send_pak, mcmsg_recv_pak)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_PAKL	13
#define				SENDMETH_PAKL	MCTRL_PAKL
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_PAKL, "PAKL     ",   "SENDMETH_PAKL", \
				mcmsg_send_pak, mcmsg_recv_pak)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_PNA	14
#define				SENDMETH_PNA	MCTRL_PNA
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_PNA, "PNA     ",    "SENDMETH_PNA", \
				mcmsg_send_pna, mcmsg_recv_pna)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_PRM	15
#define				SENDMETH_PRM	MCTRL_PRM
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_PRM, "PRM     ",    "SENDMETH_PRM", \
				mcmsg_send_prm, mcmsg_recv_prm)

		PACKET_FILL(	16)

		PACKET_FILL(	17)

		PACKET_FILL(	18)

		PACKET_FILL(	19)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_RK1	20
#define				SENDMETH_RK1	MCTRL_RK1
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_RK1, "RK1     ",    "SENDMETH_RK1", \
				mcmsg_send_rk1, mcmsg_recv_rk1)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_RKN	21
#define				SENDMETH_RKN	MCTRL_RKN
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_RKN, "RKN     ",    "SENDMETH_RKN", \
				mcmsg_send_rkn, mcmsg_recv_rkn)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_RKS	22
#define				SENDMETH_RKS	MCTRL_RKS
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_RKS, "RKS     ",    "SENDMETH_RKS", \
				mcmsg_send_rks, mcmsg_recv_rks)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXR	23
#define				SENDMETH_NXR	MCTRL_NXR
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXR, "NXR     ",    "SENDMETH_NXR", \
				mcmsg_send_nxr, mcmsg_recv_nxr)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXA	24
#define				SENDMETH_NXA	MCTRL_NXA
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXA, "NXA     ",    "SENDMETH_NXA", \
				mcmsg_send_nxa, mcmsg_recv_nxa)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXS	25
#define				SENDMETH_NXS	MCTRL_NXS
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXS, "NXS     ",    "SENDMETH_NXS", \
				mcmsg_send_nxs, mcmsg_recv_nxs)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXM	26
#define				SENDMETH_NXM	MCTRL_NXM
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXM, "NXM     ",    "SENDMETH_NXM", \
				mcmsg_send_nxm, mcmsg_recv_nxm)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXQ	27
#define				SENDMETH_NXQ	MCTRL_NXQ
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXQ, "NXQ     ",    "SENDMETH_NXQ", \
				mcmsg_send_nxq, mcmsg_recv_nxq)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXC	28
#define				SENDMETH_NXC	MCTRL_NXC
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXC, "NXC     ",    "SENDMETH_NXC", \
				mcmsg_send_nxc, mcmsg_recv_nxc)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXF	29
#define				SENDMETH_NXF	MCTRL_NXF
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXF, "NXF     ",    "SENDMETH_NXF", \
				mcmsg_send_nxf, mcmsg_recv_nxf)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NX1	30
#define				SENDMETH_NX1	MCTRL_NX1
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NX1, "NX1     ",    "SENDMETH_NX1", \
				mcmsg_send_nx1, mcmsg_recv_nx1)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_NXN	31
#define				SENDMETH_NXN	MCTRL_NXN
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_NXN, "NXN     ",    "SENDMETH_NXN", \
				mcmsg_send_nxn, mcmsg_recv_nxn)

		PACKET_FILL(	32)

		PACKET_FILL(	33)

		PACKET_FILL(	34)

		PACKET_FILL(	35)

		PACKET_FILL(	36)

		PACKET_FILL(	37)

		PACKET_FILL(	38)

		PACKET_FILL(	39)

		PACKET_FILL(	40)

		PACKET_FILL(	41)

		PACKET_FILL(	42)

		PACKET_FILL(	43)

		PACKET_FILL(	44)

		PACKET_FILL(	45)

		PACKET_FILL(	46)

		PACKET_FILL(	47)

		PACKET_FILL(	48)

		PACKET_FILL(	49)

		PACKET_FILL(	50)

		PACKET_FILL(	51)

		PACKET_FILL(	52)

		PACKET_FILL(	53)

		PACKET_FILL(	54)

		PACKET_FILL(	55)

		PACKET_FILL(	56)

		PACKET_FILL(	57)

		PACKET_FILL(	58)

		PACKET_FILL(	59)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_CONW	60
#define				SENDMETH_CONW	MCTRL_CONW
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_CONW, "CONW     ",   "SENDMETH_CONW", \
				mcmsg_send_conw, mcmsg_recv_conw)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_CONT	61
#define				SENDMETH_CONT	MCTRL_CONT
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_CONT, "CONT     ",   "SENDMETH_CONT", \
				mcmsg_send_cont, mcmsg_recv_cont)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_CONR	62
#define				SENDMETH_CONR	MCTRL_CONR
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_CONR, "CONR     ",   "SENDMETH_CONR", \
				mcmsg_send_conr, mcmsg_recv_conr)

		PACKET_FILL(	63)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_BT0	64
#define				SENDMETH_BT0	MCTRL_BT0
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_BT0, "BT0     ",    "SENDMETH_BT0", \
				mcmsg_send_boot, mcmsg_recv_boot)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_BT1	65
#define				SENDMETH_BT1	MCTRL_BT1
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_BT1, "BT1     ",    "SENDMETH_BT1", \
				mcmsg_send_boot, mcmsg_recv_boot)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_BT2	66
#define				SENDMETH_BT2	MCTRL_BT2
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_BT2, "BT2     ",    "SENDMETH_BT2", \
				mcmsg_send_boot, mcmsg_recv_boot)

#ifndef	MCMSG_PACKET_H
#define				MCTRL_BT3	67
#define				SENDMETH_BT3	MCTRL_BT3
#endif	MCMSG_PACKET_H
		PACKET_DEFINE(  MCTRL_BT3, "BT3     ",    "SENDMETH_BT3", \
				mcmsg_send_boot, mcmsg_recv_boot)

		PACKET_FILL(	68)

		PACKET_FILL(	69)

		PACKET_END(	70)


#undef	PACKET_DEFINE
#undef	PACKET_NULL
#undef	PACKET_FILL
#undef	PACKET_END

#ifndef	MCMSG_PACKET_H
#define	MCMSG_PACKET_H
#endif	MCMSG_PACKET_H
