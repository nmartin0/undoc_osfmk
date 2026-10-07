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
 * $Id: msgp.h,v 1.7 1994/11/18 20:46:30 mtm Exp $: 
 *
 */

/*
 * msgp.h
 *
 * Message processor specific declarations
 */

#ifndef MSGP_H
#define MSGP_H

#include <mach_assert.h>	/* MACH_ASSERT */
#include <cpus.h>		/* NCPUS */

/*
 * Define the modes which the message processor can exist in. The message
 * processor will always be in on of these modes.
 */
#define MSGP_NO_PREFERENCE	0
#define MSGP_TURBO_MODE		1
#define MSGP_MCP_MODE		2
#define MSGP_IN_TRANSITION	3


#if	MACH_ASSERT && MCMSG && NCPUS == 1

#undef assert
#define assert(t) if (!(t)) { mcmsg_msgp_assert(__FILE__, __LINE__); }

#endif	/* MACH_ASSERT && MCMSG && NCPUS == 1 */

#endif MSGP_H
