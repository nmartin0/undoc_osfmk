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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_puma.h,v 1.7 1995/01/17 16:43:12 robboy Exp $
 */

/*
 * mcmsg_config_puma.h
 *
 * PUMA module configuration
 */

#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_PUMA, mcmsg_puma_init)
	    INQ_NULL(MCMSG_MODULE_PUMA)

#endif	INIT_SECTION


#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_PUMASETUP	(PUMA_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PUMASETUP, "PUMASETUP",
			mcmsg_puma_setup, 6, MCMSG_MODULE_PUMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_PUMASEND	(PUMA_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PUMASEND, "PUMASEND",
			mcmsg_puma_send, 6, MCMSG_MODULE_PUMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_PUMARST	(PUMA_POST_BASE+2)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PUMARST, "PUMARST",
			mcmsg_puma_reset, 1, MCMSG_MODULE_PUMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_PUMARLS	(PUMA_POST_BASE+3)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PUMARLS, "PUMARLS",
			mcmsg_puma_release, 1, MCMSG_MODULE_PUMA, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_PUMATRC	(PUMA_POST_BASE+4)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_PUMATRC, "PUMATRC",
			mcmsg_puma_trace, 1, MCMSG_MODULE_PUMA, USER)

	    POST_NULL(PUMA_POST_BASE+5)

	    POST_NULL(PUMA_POST_BASE+6)

	    POST_NULL(PUMA_POST_BASE+7)

	    POST_NULL(PUMA_POST_BASE+8)

	    POST_NULL(PUMA_POST_BASE+9)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_PUMA1	(PUMA_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_PUMA1, "PUMA1     ",    "MCTRL_PUMA1", \
				mcmsg_send_puma1, mcmsg_recv_puma1)

		PACKET_FILL(PUMA_PACKET_BASE+1)

		PACKET_FILL(PUMA_PACKET_BASE+2)

		PACKET_FILL(PUMA_PACKET_BASE+3)

		PACKET_FILL(PUMA_PACKET_BASE+4)

		PACKET_FILL(PUMA_PACKET_BASE+5)

		PACKET_FILL(PUMA_PACKET_BASE+6)

		PACKET_FILL(PUMA_PACKET_BASE+7)

		PACKET_FILL(PUMA_PACKET_BASE+8)

		PACKET_FILL(PUMA_PACKET_BASE+9)

#endif	PACKET_SECTION
