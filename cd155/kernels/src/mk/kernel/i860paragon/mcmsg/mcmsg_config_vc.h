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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_vc.h,v 1.2 1994/11/18 20:44:03 mtm Exp $
 */

/*
 * mcmsg_config_vc.h
 *
 * VC module configuration
 */

#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_VC, mcmsg_vc_init)
	    INQ_DEFINE(MCMSG_MODULE_VC, mcmsg_vc_sendopen)

#endif	INIT_SECTION


#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_VCOPEN	(VC_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_VCOPEN, "VCOPEN",
			mcmsg_vc_open, 2, MCMSG_MODULE_VC, SYS)

#ifndef MCMSG_CONFIG_H
#define			POST_VCSEND	(VC_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_VCSEND, "VCSEND",
			mcmsg_vc_send, 3, MCMSG_MODULE_VC, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_VCRECV	(VC_POST_BASE+2)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_VCRECV, "VCRECV",
			mcmsg_vc_recv, 3, MCMSG_MODULE_VC, USER)

	    POST_NULL(VC_POST_BASE+3)

	    POST_NULL(VC_POST_BASE+4)

	    POST_NULL(VC_POST_BASE+5)

	    POST_NULL(VC_POST_BASE+6)

	    POST_NULL(VC_POST_BASE+7)

	    POST_NULL(VC_POST_BASE+8)

	    POST_NULL(VC_POST_BASE+9)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_VCO	(VC_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_VCO, "VCO     ",    "MCTRL_VCO", \
				mcmsg_send_vco, mcmsg_recv_vco)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_VCR	(VC_PACKET_BASE+1)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_VCR, "VCR     ",    "MCTRL_VCR", \
				mcmsg_send_vcr, mcmsg_recv_vcr)

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_VCD	(VC_PACKET_BASE+2)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_VCD, "VCD     ",    "MCTRL_VCD", \
				mcmsg_send_vcd, mcmsg_recv_vcd)

		PACKET_FILL(VC_PACKET_BASE+3)

		PACKET_FILL(VC_PACKET_BASE+4)

		PACKET_FILL(VC_PACKET_BASE+5)

		PACKET_FILL(VC_PACKET_BASE+6)

		PACKET_FILL(VC_PACKET_BASE+7)

		PACKET_FILL(VC_PACKET_BASE+8)

		PACKET_FILL(VC_PACKET_BASE+9)

#endif	PACKET_SECTION
