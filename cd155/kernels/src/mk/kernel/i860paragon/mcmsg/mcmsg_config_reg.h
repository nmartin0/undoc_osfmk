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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config_reg.h,v 1.2 1994/11/18 20:43:58 mtm Exp $
 */

/*
 * mcmsg_config_reg.h
 *
 * REG module configuration
 */

#if	INIT_SECTION

	    INIT_DEFINE(MCMSG_MODULE_REG, mcmsg_reg_init)
	    INQ_NULL(MCMSG_MODULE_REG)

#endif	INIT_SECTION


#if	POST_SECTION

#ifndef MCMSG_CONFIG_H
#define			POST_REGOPEN	(REG_POST_BASE+0)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_REGOPEN, "REGOPEN",
			mcmsg_reg_open, 1, MCMSG_MODULE_REG, USER)

#ifndef MCMSG_CONFIG_H
#define			POST_REGSEND	(REG_POST_BASE+1)
#endif	MCMSG_CONFIG_H
	    POST_DEFINE(POST_REGSEND, "REGSEND",
			mcmsg_reg_send, 8, MCMSG_MODULE_REG, USER)

	    POST_NULL(REG_POST_BASE+2)

	    POST_NULL(REG_POST_BASE+3)

	    POST_NULL(REG_POST_BASE+4)

	    POST_NULL(REG_POST_BASE+5)

	    POST_NULL(REG_POST_BASE+6)

	    POST_NULL(REG_POST_BASE+7)

	    POST_NULL(REG_POST_BASE+8)

	    POST_NULL(REG_POST_BASE+9)


#endif	POST_SECTION


#if	PACKET_SECTION

#ifndef	MCMSG_CONFIG_H
#define				MCTRL_REG	(REG_PACKET_BASE+0)
#endif	MCMSG_CONFIG_H
		PACKET_DEFINE(  MCTRL_REG, "REG     ",    "MCTRL_REG", \
				mcmsg_send_reg, mcmsg_recv_reg)

		PACKET_FILL(REG_PACKET_BASE+1)

		PACKET_FILL(REG_PACKET_BASE+2)

		PACKET_FILL(REG_PACKET_BASE+3)

		PACKET_FILL(REG_PACKET_BASE+4)

		PACKET_FILL(REG_PACKET_BASE+5)

		PACKET_FILL(REG_PACKET_BASE+6)

		PACKET_FILL(REG_PACKET_BASE+7)

		PACKET_FILL(REG_PACKET_BASE+8)

		PACKET_FILL(REG_PACKET_BASE+9)

#endif	PACKET_SECTION
