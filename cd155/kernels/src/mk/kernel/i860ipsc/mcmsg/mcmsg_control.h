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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_control.h,v 1.14 1994/11/18 20:41:13 mtm Exp $
 */

/*
 * mcmsg_control.h
 *
 * Control Field Values for the Common Packet Header
 */

#ifndef MCMSG_CONTROL_H
#define MCMSG_CONTROL_H

#if	PARAGON860

#define MCTRL_IPC	1
#define MCTRL_IPCQ	2
#define MCTRL_IPCA	3
#define MCTRL_PNQ	10
#define MCTRL_PNQL	11
#define MCTRL_PAK	12
#define MCTRL_PAKL	13
#define MCTRL_PNA	14
#define MCTRL_PRM	15
#define MCTRL_RK1	20
#define MCTRL_RKN	21
#define MCTRL_RKS	22
#define MCTRL_RKR	23
#define MCTRL_RKA	24
#define MCTRL_NXS	25
#define MCTRL_NXM	26
#define MCTRL_NXQ	27
#define MCTRL_NXC	28
#define MCTRL_NXF	29
#define MCTRL_NX1	30
#define MCTRL_NXN	31
#define MCTRL_CONW	60
#define MCTRL_CONT	61
#define MCTRL_CONR	62
#define MCTRL_BT0	64
#define MCTRL_BT1	65
#define MCTRL_BT2	66
#define MCTRL_BT3	67

#define MCTRL_END	70

/* Remember to update mcmsg_packet.h */

#else	PARAGON860

#define MCTRL_IPC	DCM_HDR_TYPE_MACH
#define MCTRL_PNQ	10
#define MCTRL_PNQL	11
#define MCTRL_PAK	12
#define MCTRL_PAKL	13
#define MCTRL_PNA	14
#define MCTRL_PRM	15
#define MCTRL_RK1	20
#define MCTRL_RKN	21
#define MCTRL_RKS	22
#define MCTRL_RKR	23
#define MCTRL_RKA	24
#define MCTRL_NXS	25
#define MCTRL_NXM	26
#define MCTRL_NXQ	27
#define MCTRL_NXC	28
#define MCTRL_NXF	29
#define MCTRL_NX1	30
#define MCTRL_NXN	31

#define MCTRL_CONW	60
#define MCTRL_CONT	61
#define MCTRL_CONR	62
#define MCTRL_BT0	64
#define MCTRL_BT1	65
#define MCTRL_BT2	66
#define MCTRL_BT3	67

/* Remember to update mcmsg_trace.c */

#endif	PARAGON860

#define MCTRL_ALLOW_LO	MCTRL_BT0
#define MCTRL_ALLOW_HI	MCTRL_BT3

#endif MCMSG_CONTROL_H
