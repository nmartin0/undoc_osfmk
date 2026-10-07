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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config.h,v 1.15 1994/11/18 20:43:53 mtm Exp $
 */

/*
 * mcmsg_config.h
 *
 * Message module configuration
 */

#ifndef	MCMSG_CONFIG_H


#define	POST_EMPTY		0

#ifndef	MCMSG_MODULE
#define	MCMSG_MODULE		MCMSG_MODULE_GENERAL
#endif	MCMSG_MODULE

#define	MCTRL_ALLOW_LO	MCTRL_BT0
#define	MCTRL_ALLOW_HI	MCTRL_BT3
#define	POST_MAX	128
#define	MCTRL_END	128
#define METHOD_MAX	(MCTRL_END + 19)

#define	MCTRL_NULL	0

#endif	MCMSG_CONFIG_H




/*
 * Module identification for trace control
 * Init definitions
 * Inquiry definitions
 */

#define	INIT_SECTION	1

#ifndef	INIT_NULL
#define INIT_NULL(m)
#endif	INIT_NULL

#ifndef	INIT_DEFINE
#define INIT_DEFINE(m, p)
#endif	INIT_DEFINE

#ifndef	INQ_NULL
#define INQ_NULL(m)
#endif	INQ_NULL

#ifndef	INQ_DEFINE
#define INQ_DEFINE(m, p)
#endif	INQ_DEFINE



#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_GENERAL	0
#endif	MCMSG_CONFIG_H
		INIT_NULL(MCMSG_MODULE_GENERAL)
		INQ_NULL(MCMSG_MODULE_GENERAL)

#if	NORMA
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_IPC	1
#endif	MCMSG_CONFIG_H
		INIT_NULL(MCMSG_MODULE_IPC)
		INQ_NULL(MCMSG_MODULE_IPC)
#else	NORMA
		INIT_NULL(4)
		INQ_NULL(4)
#endif	NORMA

#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_BOOT	2
#endif	MCMSG_CONFIG_H
		INIT_NULL(MCMSG_MODULE_BOOT)
		INQ_NULL(MCMSG_MODULE_BOOT)

#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_CONSOLE	3
#endif	MCMSG_CONFIG_H
		INIT_NULL(MCMSG_MODULE_CONSOLE)
		INQ_NULL(MCMSG_MODULE_CONSOLE)

#if	NX
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_NX		4
#endif	MCMSG_CONFIG_H
#include <i860paragon/mcmsg/mcmsg_config_nx.h>
#else	NX
		INIT_NULL(4)
		INQ_NULL(4)
#endif	NX

#if	URDMA
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_URDMA	5
#endif	MCMSG_CONFIG_H
#include <i860paragon/mcmsg/mcmsg_config_urdma.h>
#else	URDMA
		INIT_NULL(5)
		INQ_NULL(5)
#endif	URDMA

#if	PUMA
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_PUMA	6
#endif	MCMSG_CONFIG_H
#include <i860paragon/mcmsg/mcmsg_config_puma.h>
#else	PUMA
		INIT_NULL(6)
		INQ_NULL(6)
#endif	PUMA

#if	MCMSG_ENG
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_RPC	7
#endif	MCMSG_CONFIG_H
#include <i860paragon/mcmsg/mcmsg_config_rpc.h>
#else	MCMSG_ENG
		INIT_NULL(7)
		INQ_NULL(7)
#endif	MCMSG_ENG

#if	MCMSG_ENG
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_RDMA	8
#endif	MCMSG_CONFIG_H
#include <i860paragon/mcmsg/mcmsg_config_rdma.h>
#else	MCMSG_ENG
		INIT_NULL(8)
		INQ_NULL(8)
#endif	MCMSG_ENG

#if	VCF
#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_VCF	9
#endif	MCMSG_CONFIG_H
#include <i860paragon/mcmsg/mcmsg_config_vcf.h>
#else	VCF
		INIT_NULL(9)
		INQ_NULL(9)
#endif	VCF

#ifndef MCMSG_CONFIG_H
#define	MCMSG_MODULE_END	10
#endif	MCMSG_CONFIG_H

#undef	INIT_NULL
#undef	INIT_DEFINE

#undef	INQ_NULL
#undef	INQ_DEFINE

#undef	INIT_SECTION



/*
 * Interface definitions
 */

#define	POST_SECTION	1

#ifndef	POST_NULL
#define POST_NULL(m)
#endif	POST_NULL

#ifndef	POST_DEFINE
#define POST_DEFINE(m,n,p,a,i,u)
#endif	POST_DEFINE

#define			USER 1
#define			SYS  0


	    POST_NULL(0)

#ifndef MCMSG_CONFIG_H
#define			POST_NOP	1
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_NOP, "NOP",
			mcmsg_nop, 0, MCMSG_MODULE_GENERAL, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_PMAPUPDATE	2
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PMAPUPDATE, "PMAPUPD",
			msgp_pmap_update_interrupt, 0, MCMSG_MODULE_GENERAL, SYS)


#if	NORMA
	/*
	 *	Old low-level NORMA protocol
	 */
#ifndef MCMSG_CONFIG_H
#define			POST_IPCSEND	3
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_IPCSEND, "IPCSEND",
			ipcreq_send, 0, MCMSG_MODULE_IPC, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_IPCRECV	4
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_IPCRECV, "IPCRECV",
			ipcreq_recv, 2, MCMSG_MODULE_IPC, SYS)
#else	NORMA

	    POST_NULL(3)
	    POST_NULL(4)

#endif	NORMA


#ifndef MCMSG_CONFIG_H
#define			POST_BOOTSEND	5
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_BOOTSEND, "BOOTSEND",
			mcmsg_boot_send, 0, MCMSG_MODULE_BOOT, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_CONOPEN	6
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_CONOPEN, "CONOPEN",
			mcmsg_console_open, 2, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_CONCLOSE	7
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_CONCLOSE, "CONCLOSE",
			mcmsg_console_close, 2, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_CONREAD	8
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_CONREAD, "CONREAD",
			mcmsg_console_read, 2, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_CONWRITE	9
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_CONWRITE, "CONWRITE",
			mcmsg_console_write, 3, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_CONFIG_H
#define			POST_CONPUT	10
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_CONPUT, "CONPUT",
			mcmsg_console_do_put, 0, MCMSG_MODULE_CONSOLE, SYS)


#ifndef MCMSG_CONFIG_H
#define                 POST_DFLUSH     11
#endif  MCMSG_CONFIG_H
	    POST_DEFINE(POST_DFLUSH, "DFLUSH",
			msgp_dflush_interrupt, 0, MCMSG_MODULE_GENERAL, SYS)


#if MCMSG_MODULE_NX

#define	NX_POST_BASE	12
#include <i860paragon/mcmsg/mcmsg_config_nx.h>

#else MCMSG_MODULE_NX

	    POST_NULL(12)
	    POST_NULL(13)
	    POST_NULL(14)
	    POST_NULL(15)
	    POST_NULL(16)
	    POST_NULL(17)
	    POST_NULL(18)
	    POST_NULL(19)
	    POST_NULL(20)
	    POST_NULL(21)
	    POST_NULL(22)
	    POST_NULL(23)
	    POST_NULL(24)
	    POST_NULL(25)
	    POST_NULL(26)
	    POST_NULL(27)
	    POST_NULL(28)
	    POST_NULL(29)

#endif MCMSG_MODULE_NX

#ifndef MCMSG_CONFIG_H
#define			POST_INSTALL	30
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_INSTALL, "INSTALL",
			mcmsg_mp_install, 2, MCMSG_MODULE_GENERAL, SYS)


	    POST_NULL(31)
	    POST_NULL(32)
	    POST_NULL(33)
	    POST_NULL(34)
	    POST_NULL(35)
	    POST_NULL(36)
	    POST_NULL(37)
	    POST_NULL(38)
	    POST_NULL(39)

#if	MCMSG_MODULE_PUMA

#define	PUMA_POST_BASE	40
#include <i860paragon/mcmsg/mcmsg_config_puma.h>

#else	MCMSG_MODULE_PUMA

	    POST_NULL(40)
	    POST_NULL(41)
	    POST_NULL(42)
	    POST_NULL(43)
	    POST_NULL(44)
	    POST_NULL(45)
	    POST_NULL(46)
	    POST_NULL(47)
	    POST_NULL(48)
	    POST_NULL(49)

#endif	MCMSG_MODULE_PUMA

	    POST_NULL(50)
	    POST_NULL(51)
	    POST_NULL(52)
	    POST_NULL(53)
	    POST_NULL(54)
	    POST_NULL(55)
	    POST_NULL(56)
	    POST_NULL(57)
	    POST_NULL(58)
	    POST_NULL(59)

#if	MCMSG_MODULE_RPC

#define	RPC_POST_BASE	60
#include <i860paragon/mcmsg/mcmsg_config_rpc.h>

#else	MCMSG_MODULE_RPC

	    POST_NULL(60)
	    POST_NULL(61)
	    POST_NULL(62)
	    POST_NULL(63)
	    POST_NULL(64)
	    POST_NULL(65)
	    POST_NULL(66)
	    POST_NULL(67)
	    POST_NULL(68)
	    POST_NULL(69)

#endif	MCMSG_MODULE_RPC

#if	MCMSG_MODULE_RDMA

#define	RDMA_POST_BASE	70
#include <i860paragon/mcmsg/mcmsg_config_rdma.h>

#else	MCMSG_MODULE_RDMA

	    POST_NULL(70)
	    POST_NULL(71)
	    POST_NULL(72)
	    POST_NULL(73)
	    POST_NULL(74)
	    POST_NULL(75)
	    POST_NULL(76)
	    POST_NULL(77)
	    POST_NULL(78)
	    POST_NULL(79)

#endif	MCMSG_MODULE_RDMA

#if	MCMSG_MODULE_URDMA

#define	URDMA_POST_BASE	80
#include <i860paragon/mcmsg/mcmsg_config_urdma.h>

#else	MCMSG_MODULE_URDMA

	    POST_NULL(80)
	    POST_NULL(81)
	    POST_NULL(82)
	    POST_NULL(83)
	    POST_NULL(84)
	    POST_NULL(85)
	    POST_NULL(86)
	    POST_NULL(87)
	    POST_NULL(88)
	    POST_NULL(89)
	    POST_NULL(90)
	    POST_NULL(91)
	    POST_NULL(92)
	    POST_NULL(93)
	    POST_NULL(94)
	    POST_NULL(95)
	    POST_NULL(96)
	    POST_NULL(97)
	    POST_NULL(98)
	    POST_NULL(99)

#endif	MCMSG_MODULE_URDMA

#if		MCMSG_MODULE_VCF

#define	VCF_POST_BASE	100
#include <i860paragon/mcmsg/mcmsg_config_vcf.h>

#else	MCMSG_MODULE_VCF
	    POST_NULL(100)
	    POST_NULL(101)
	    POST_NULL(102)
	    POST_NULL(103)
	    POST_NULL(104)
	    POST_NULL(105)
	    POST_NULL(106)
	    POST_NULL(107)
	    POST_NULL(108)
	    POST_NULL(109)
#endif	MCMSG_MODULE_VCF

	    POST_NULL(110)
	    POST_NULL(111)
	    POST_NULL(112)
	    POST_NULL(113)
	    POST_NULL(114)
	    POST_NULL(115)
	    POST_NULL(116)
	    POST_NULL(117)
	    POST_NULL(118)
	    POST_NULL(119)
	    POST_NULL(120)
	    POST_NULL(121)
	    POST_NULL(122)
	    POST_NULL(123)
	    POST_NULL(124)
	    POST_NULL(125)
	    POST_NULL(126)
	    POST_NULL(127)

#undef	USER
#undef	SYS
#undef	POST_DEFINE
#undef	POST_NULL

#undef	POST_SECTION


/*
 * Packet definitions
 */

#define	PACKET_SECTION	1

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



		PACKET_NULL(	0)

#if	NORMA
	/*
	 *	Old low-level NORMA protocol packets
	 */
#ifndef	MCMSG_CONFIG_H
#define				MCTRL_IPC	1
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_IPC, "IPC     ",    "MCTRL_IPC", \
				mcmsg_send_ipc, mcmsg_recv_ipc)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_IPCQ	2
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_IPCQ, "IPCQ    ",    "MCTRL_IPCQ", \
				mcmsg_send_ipcq, mcmsg_recv_ipcq)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_IPCA	3
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_IPCA, "IPCA    ",    "MCTRL_IPCA", \
				mcmsg_send_ipca, mcmsg_recv_ipca)
#else	NORMA
		PACKET_FILL(    1)
		PACKET_FILL(    2)
		PACKET_FILL(    3)
#endif	NORMA

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_BUMP	4
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_BUMP, "BUMP    ",    "MCTRL_BUMP", \
				mcmsg_send_sw_err, mcmsg_recv_bump)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_EVEN	5
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_EVEN, "EVEN    ",    "MCTRL_EVEN", \
				mcmsg_send_sw_err, mcmsg_recv_even)

		PACKET_FILL(	6)

		PACKET_FILL(	7)

		PACKET_FILL(	8)

		PACKET_FILL(	9)

#if	MCMSG_MODULE_NX

#define	NX_PACKET_BASE	10
#include <i860paragon/mcmsg/mcmsg_config_nx.h>

#else	MCMSG_MODULE_NX
		PACKET_FILL(	10)
		PACKET_FILL(	11)
		PACKET_FILL(	12)
		PACKET_FILL(	13)
		PACKET_FILL(	14)
		PACKET_FILL(	15)
		PACKET_FILL(	16)
		PACKET_FILL(	17)
		PACKET_FILL(	18)
		PACKET_FILL(	19)
		PACKET_FILL(	20)
		PACKET_FILL(	21)
		PACKET_FILL(	22)
		PACKET_FILL(	23)
		PACKET_FILL(	24)
		PACKET_FILL(	25)
		PACKET_FILL(	26)
		PACKET_FILL(	27)
		PACKET_FILL(	28)
		PACKET_FILL(	29)
		PACKET_FILL(	30)
		PACKET_FILL(	31)
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
#endif	MCMSG_MODULE_NX

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_CONW	60
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_CONW, "CONW     ",   "MCTRL_CONW", \
				mcmsg_send_conw, mcmsg_recv_conw)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_CONT	61
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_CONT, "CONT     ",   "MCTRL_CONT", \
				mcmsg_send_cont, mcmsg_recv_cont)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_CONR	62
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_CONR, "CONR     ",   "MCTRL_CONR", \
				mcmsg_send_conr, mcmsg_recv_conr)

		PACKET_FILL(	63)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_BT0	64
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_BT0, "BT0     ",    "MCTRL_BT0", \
				mcmsg_send_boot, mcmsg_recv_boot)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_BT1	65
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_BT1, "BT1     ",    "MCTRL_BT1", \
				mcmsg_send_boot, mcmsg_recv_boot)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_BT2	66
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_BT2, "BT2     ",    "MCTRL_BT2", \
				mcmsg_send_boot, mcmsg_recv_boot)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_BT3	67
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_BT3, "BT3     ",    "MCTRL_BT3", \
				mcmsg_send_boot, mcmsg_recv_boot)

		PACKET_FILL(	68)

		PACKET_FILL(	69)

#if	MCMSG_MODULE_PUMA

#define	PUMA_PACKET_BASE	70
#include <i860paragon/mcmsg/mcmsg_config_puma.h>

#else	MCMSG_MODULE_PUMA

		PACKET_FILL(70)
		PACKET_FILL(71)
		PACKET_FILL(72)
		PACKET_FILL(73)
		PACKET_FILL(74)
		PACKET_FILL(75)
		PACKET_FILL(76)
		PACKET_FILL(77)
		PACKET_FILL(78)
		PACKET_FILL(79)

#endif	MCMSG_MODULE_PUMA

#if	MCMSG_MODULE_URDMA

#define	URDMA_PACKET_BASE	80
#include <i860paragon/mcmsg/mcmsg_config_urdma.h>

#else	MCMSG_MODULE_URDMA

		PACKET_FILL(80)
		PACKET_FILL(81)
		PACKET_FILL(82)
		PACKET_FILL(83)
		PACKET_FILL(84)
		PACKET_FILL(85)
		PACKET_FILL(86)
		PACKET_FILL(87)
		PACKET_FILL(88)
		PACKET_FILL(89)

#endif	MCMSG_MODULE_URDMA

#if	MCMSG_MODULE_RPC

#define	RPC_PACKET_BASE	90
#include <i860paragon/mcmsg/mcmsg_config_rpc.h>

#else	MCMSG_MODULE_RPC

		PACKET_FILL(90)
		PACKET_FILL(91)
		PACKET_FILL(92)
		PACKET_FILL(93)
		PACKET_FILL(94)
		PACKET_FILL(95)
		PACKET_FILL(96)
		PACKET_FILL(97)
		PACKET_FILL(98)
		PACKET_FILL(99)

#endif	MCMSG_MODULE_RPC

#if	MCMSG_MODULE_RDMA

#define	RDMA_PACKET_BASE	100
#include <i860paragon/mcmsg/mcmsg_config_rdma.h>

#else	MCMSG_MODULE_RDMA

		PACKET_FILL(100)
		PACKET_FILL(101)
		PACKET_FILL(102)
		PACKET_FILL(103)
		PACKET_FILL(104)
		PACKET_FILL(105)
		PACKET_FILL(106)
		PACKET_FILL(107)
		PACKET_FILL(108)
		PACKET_FILL(109)

#endif	MCMSG_MODULE_RDMA

#if		MCMSG_MODULE_VCF

#define	VCF_PACKET_BASE	110
#include <i860paragon/mcmsg/mcmsg_config_vcf.h>

#else	MCMSG_MODULE_VCF

		PACKET_FILL(110)
		PACKET_FILL(111)
		PACKET_FILL(112)
		PACKET_FILL(113)
		PACKET_FILL(114)
		PACKET_FILL(115)
		PACKET_FILL(116)
		PACKET_FILL(117)
		PACKET_FILL(118)
		PACKET_FILL(119)

#endif	MCMSG_MODULE_VCF

		PACKET_FILL(120)
		PACKET_FILL(121)
		PACKET_FILL(122)
		PACKET_FILL(123)
		PACKET_FILL(124)
		PACKET_FILL(125)
		PACKET_FILL(126)
		PACKET_FILL(127)
		PACKET_END(128)


#undef	PACKET_DEFINE
#undef	PACKET_NULL
#undef	PACKET_FILL
#undef	PACKET_END

#undef	PACKET_SECTION

#ifndef MCMSG_CONFIG_H
#define	MCMSG_CONFIG_H
#endif	MCMSG_CONFIG_H
