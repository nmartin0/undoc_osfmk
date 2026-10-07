/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * sm_init_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/sm_init_defs.h,v 1.2 1994/11/19 03:12:13 mtm Exp $
 *
 */

/*
 * Definitions for secure Mach initialisation.
 */


#ifndef	_SM_INIT_DEFS_
#define	_SM_INIT_DEFS_

#include <mach/mach_param.h>

#define NAME_SERVER_INDEX	0
#define AUTH_SERVER_INDEX	1
#define SM_INIT_INDEX_MAX	1

#undef NAME_SERVER_SLOT
#undef ENVIRONMENT_SLOT
#undef SERVICE_SLOT

#define OLD_NAME_SERVER_SLOT	0
#define NETNAME_SLOT		0
#define SM_INIT_SLOT		1
#define AUTH_PRIVATE_SLOT	1
#define OLD_ENV_SERVER_SLOT	1
#define AUTH_SERVER_SLOT	2
#define OLD_SERVICE_SLOT	2
#define KM_SERVICE_SLOT		2
#define NAME_SERVER_SLOT	3
#define NAME_PRIVATE_SLOT	3
#define SM_INIT_SLOTS_USED	4

#if (SM_INIT_SLOTS_USED > TASK_PORT_REGISTER_MAX)
Things are not going to work!
#endif


#define SM_INIT_FAILURE		11881

#endif	_SM_INIT_DEFS_
