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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_send.h,v 1.13 1994/11/18 20:41:58 mtm Exp $
 */

/*
 * mcmsg_send.h
 *
 * Declarations for sending
 */

#ifndef MCMSG_SEND_H
#define MCMSG_SEND_H

#if	PARAGON860

#define	SENDMETH_IPC	MCTRL_IPC
#define	SENDMETH_IPCQ	MCTRL_IPCQ
#define	SENDMETH_IPCA	MCTRL_IPCA
#define	SENDMETH_PNQ	MCTRL_PNQ
#define	SENDMETH_PNQL	MCTRL_PNQL
#define	SENDMETH_PAK	MCTRL_PAK
#define	SENDMETH_PAKL	MCTRL_PAKL
#define	SENDMETH_PNA	MCTRL_PNA
#define	SENDMETH_PRM	MCTRL_PRM
#define	SENDMETH_RK1	MCTRL_RK1
#define	SENDMETH_RKN	MCTRL_RKN
#define	SENDMETH_RKS	MCTRL_RKS
#define	SENDMETH_RKR	MCTRL_RKR
#define	SENDMETH_RKA	MCTRL_RKA
#define	SENDMETH_NXS	MCTRL_NXS
#define	SENDMETH_NXM	MCTRL_NXM
#define	SENDMETH_NXQ	MCTRL_NXQ
#define	SENDMETH_NXC	MCTRL_NXC
#define	SENDMETH_NXF	MCTRL_NXF
#define	SENDMETH_NX1	MCTRL_NX1
#define	SENDMETH_NXN	MCTRL_NXN
#define	SENDMETH_CONW	MCTRL_CONW
#define	SENDMETH_CONT	MCTRL_CONT
#define	SENDMETH_CONR	MCTRL_CONR
#define	SENDMETH_BT0	MCTRL_BT0
#define	SENDMETH_BT1	MCTRL_BT1
#define	SENDMETH_BT2	MCTRL_BT2
#define	SENDMETH_BT3	MCTRL_BT3

#define SENDMETH_NULL	0
#define SENDMETH_RK1A	MCTRL_END
#define SENDMETH_END	(MCTRL_END+1)
#define SENDMETH_BOOT	SENDMETH_BT0

#else	PARAGON860

#define SENDMETH_NULL	50
#define SENDMETH_PNQ	51
#define SENDMETH_PNA	52
#define SENDMETH_PAK	53
#define	SENDMETH_RK1	54
#define SENDMETH_RKN	55
#define SENDMETH_RKS	56
#define SENDMETH_RKR	57
#define SENDMETH_RK1A	58
#define SENDMETH_RKA	59
#define SENDMETH_NXS	60
#define SENDMETH_NXM	61
#define SENDMETH_NX1	62
#define SENDMETH_NXF	63
#define SENDMETH_NXN	64
#define SENDMETH_NXQ	65
#define SENDMETH_NXC	66
#define SENDMETH_BOOT	67
#define SENDMETH_CONW	68
#define SENDMETH_CONT	69
#define SENDMETH_CONR	70
#define SENDMETH_PRM	71
/* Remember to update mcmsg_trace */

#endif	PARAGON860

#endif MCMSG_SEND_H
