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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_packet.h,v 1.7 1994/11/18 20:41:51 mtm Exp $
 */

/*
 * mcmsg_packet.h
 *
 * Packet type definition macros
 */

PACKET_NULL(	0)

PACKET_DEFINE(	1,	MCTRL_IPC,	SENDMETH_IPC, \
	"IPC     ",    "SENDMETH_IPC",	ipcreq_send_intr, ipcreq_recv_intr)

PACKET_DEFINE(	2,	MCTRL_IPCQ,	SENDMETH_IPCQ, \
	"IPCQ    ",    "SENDMETH_IPCQ",	mcmsg_send_ipcq, mcmsg_recv_ipcq)

PACKET_DEFINE(	3,	MCTRL_IPCA,	SENDMETH_IPCA, \
	"IPCA    ",    "SENDMETH_IPCA",	mcmsg_send_ipca, mcmsg_recv_ipca)

PACKET_FILL(	4)

PACKET_FILL(	5)

PACKET_FILL(	6)

PACKET_FILL(	7)

PACKET_FILL(	8)

PACKET_FILL(	9)

PACKET_DEFINE(	10,	MCTRL_PNQ,	SENDMETH_PNQ, \
	"PNQ     ",    "SENDMETH_PNQ",	mcmsg_send_pnq, mcmsg_recv_pnq)

PACKET_DEFINE(	11,	MCTRL_PNQL,	SENDMETH_PNQL, \
	"PNQL     ",   "SENDMETH_PNQL",	mcmsg_send_pnq, mcmsg_recv_pnq)

PACKET_DEFINE(	12,	MCTRL_PAK,	SENDMETH_PAK, \
	"PAK     ",    "SENDMETH_PAK",	mcmsg_send_pak, mcmsg_recv_pak)

PACKET_DEFINE(	13,	MCTRL_PAKL,	SENDMETH_PAKL, \
	"PAKL     ",   "SENDMETH_PAKL",	mcmsg_send_pak, mcmsg_recv_pak)

PACKET_DEFINE(	14,	MCTRL_PNA,	SENDMETH_PNA, \
	"PNA     ",    "SENDMETH_PNA",	mcmsg_send_pna, mcmsg_recv_pna)

PACKET_DEFINE(	15,	MCTRL_PRM,	SENDMETH_PRM, \
	"PRM     ",    "SENDMETH_PRM",	mcmsg_send_prm, mcmsg_recv_prm)

PACKET_FILL(	16)

PACKET_FILL(	17)

PACKET_FILL(	18)

PACKET_FILL(	19)

PACKET_DEFINE(	20,	MCTRL_RK1,	SENDMETH_RK1, \
	"RK1     ",    "SENDMETH_RK1",	mcmsg_send_rk1, mcmsg_recv_rk1)

PACKET_DEFINE(	21,	MCTRL_RKN,	SENDMETH_RKN, \
	"RKN     ",    "SENDMETH_RKN",	mcmsg_send_rkn, mcmsg_recv_rkn)

PACKET_DEFINE(	22,	MCTRL_RKS,	SENDMETH_RKS, \
	"RKS     ",    "SENDMETH_RKS",	mcmsg_send_rks, mcmsg_recv_rks)

PACKET_DEFINE(	23,	MCTRL_RKR,	SENDMETH_RKR, \
	"RKR     ",    "SENDMETH_RKR",	mcmsg_send_rkr, mcmsg_recv_rkr)

PACKET_DEFINE(	24,	MCTRL_RKA,	SENDMETH_RKA, \
	"RKA     ",    "SENDMETH_RKA",	mcmsg_send_rka, mcmsg_recv_rka)

PACKET_DEFINE(	25,	MCTRL_NXS,	SENDMETH_NXS, \
	"NXS     ",    "SENDMETH_NXS",	mcmsg_send_nxs, mcmsg_recv_nxs)

PACKET_DEFINE(	26,	MCTRL_NXM,	SENDMETH_NXM, \
	"NXM     ",    "SENDMETH_NXM",	mcmsg_send_nxm, mcmsg_recv_nxm)

PACKET_DEFINE(	27,	MCTRL_NXQ,	SENDMETH_NXQ, \
	"NXQ     ",    "SENDMETH_NXQ",	mcmsg_send_nxq, mcmsg_recv_nxq)

PACKET_DEFINE(	28,	MCTRL_NXC,	SENDMETH_NXC, \
	"NXC     ",    "SENDMETH_NXC",	mcmsg_send_nxc, mcmsg_recv_nxc)

PACKET_DEFINE(	29,	MCTRL_NXF,	SENDMETH_NXF, \
	"NXF     ",    "SENDMETH_NXF",	mcmsg_send_nxf, mcmsg_recv_nxf)

PACKET_DEFINE(	30,	MCTRL_NX1,	SENDMETH_NX1, \
	"NX1     ",    "SENDMETH_NX1",	mcmsg_send_nx1, mcmsg_recv_nx1)

PACKET_DEFINE(	31,	MCTRL_NXN,	SENDMETH_NXN, \
	"NXN     ",    "SENDMETH_NXN",	mcmsg_send_nxn, mcmsg_recv_nxn)

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

PACKET_DEFINE(	60,	MCTRL_CONW,	SENDMETH_CONW, \
	"CONW     ",   "SENDMETH_CONW",	mcmsg_send_conw, mcmsg_recv_conw)

PACKET_DEFINE(	61,	MCTRL_CONT,	SENDMETH_CONT, \
	"CONT     ",   "SENDMETH_CONT",	mcmsg_send_cont, mcmsg_recv_cont)

PACKET_DEFINE(	62,	MCTRL_CONR,	SENDMETH_CONR, \
	"CONR     ",   "SENDMETH_CONR",	mcmsg_send_conr, mcmsg_recv_conr)

PACKET_FILL(	63)

PACKET_DEFINE(	64,	MCTRL_BT0,	SENDMETH_BT0, \
	"BT0     ",    "SENDMETH_BT0",	mcmsg_send_boot, mcmsg_recv_boot)

PACKET_DEFINE(	65,	MCTRL_BT1,	SENDMETH_BT1, \
	"BT1     ",    "SENDMETH_BT1",	mcmsg_send_boot, mcmsg_recv_boot)

PACKET_DEFINE(	66,	MCTRL_BT2,	SENDMETH_BT2, \
	"BT2     ",    "SENDMETH_BT2",	mcmsg_send_boot, mcmsg_recv_boot)

PACKET_DEFINE(	67,	MCTRL_BT3,	SENDMETH_BT3, \
	"BT3     ",    "SENDMETH_BT3",	mcmsg_send_boot, mcmsg_recv_boot)

PACKET_FILL(	68)

PACKET_FILL(	69)

PACKET_END(	70)


#undef	PACKET_DEFINE
#undef	PACKET_NULL
#undef	PACKET_FILL
#undef	PACKET_END
