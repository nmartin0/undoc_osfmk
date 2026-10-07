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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/defs.h,v 1.6 1994/11/19 03:16:43 mtm Exp $
 *
 * HISTORY
 * $Log: defs.h,v $
 * Revision 1.6  1994/11/19  03:16:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/05/18  15:34:16  jkearns
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.4.2.2  1994/04/27  18:00:12  jkearns
 * Add a new debug flag, ALARMS, to have certain debug statements in
 * allocator.c only appear when truly necessary.
 *
 *  Reviewer: Scott Hahn
 *  Risk: Low
 *  Benefit or PTS #: 9142
 *  Module(s): allocator.c defs.h
 *
 * Revision 1.4.2.1  1994/04/06  19:51:25  jkearns
 * Added debug information for SMD logs.  (Invoke by issuing "smd -d 0x100".)
 *
 *  Reviewer: Scott Hahn
 *  Risk: Medium
 *  Benefit or PTS #: 8894
 *  Testing: SDSC ran NQS EATs against code.
 *  Module(s):  Makefile allocator.c app_db.c app_service.c debug.c
 * globals.h init.c jkdebug.c main.c part_service.c response.c service_db.c
 * socket_iface.c test_driver.c user_db.c debug2.c defs.h
 *
 * Revision 1.4  1993/11/18  20:27:23  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headrs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile  allocator.c defs.h
 *
 * Revision 1.3  1993/01/19  20:12:43  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.2  1992/10/05  23:43:12  rkl
 * Added copyright header.
 *
 *
 */

#ifndef _DEFS_H
#define	_DEFS_H

#include <nx/smd.h>

/*
 *  Hash defines
 */
#define	P_HASH_SIZE	31	/* should be a prime for best distribution */
#define	P_HASH(x)	((int) ((u_long) (x) % (u_long) P_HASH_SIZE) + 1)
#define	A_HASH_SIZE	61	/* should be a prime for best distribution */
#define	A_HASH(x)	((int) ((u_long) (x) % (u_long) A_HASH_SIZE))

/*
 *  Rollin time to NODE seconds macro.
 */
#define	NODE_SECS(p)	(time_t)(((double)(p->data.app_rollin) * \
				  (double)(p->data.app_size)) / (double)1000)
/*
 *  TRACE macro
 */
#ifdef DEBUG
#define	TRACE(lev, args)	if(debug_level & lev) printf args
#else
#define	TRACE(lev, args)
#endif

/*
 *  TRACE level defines.
 */
#define	SOCKET			0x00001
#define	INITS			0x00002
#define	REQUESTS		0x00004
#define	USERS			0x00008
#define	INDICATIONS		0x00010
#define	DUMP_SCHED_EVENT	0x00020
#define	DUMP_USER_REQUEST	0x00040
#define	DUMP_USER_RESPONSE	0x00080
#define TIMINGS			0x00100
#define ALARMS			0x00200
#define	ENTRY			0x08000
#define	VERBOSE			0x80000

/*
 *  User database typedef.
 */
typedef	struct user {
	struct user	*next;
	struct user	*prev;
	struct service	*service;
	int		fd;
} smd_user_t;

/*
 *  Application database typedef.
 */
typedef struct app {
	struct app	*next;
	struct app	*prev;
	int		state;
#define				APP_STATE_UKNOWN	(-1)
	app_data_t	data;
} smd_app_t;

/*
 *  Service database typedef.
 */
typedef struct service {
	struct service		*next;
	struct service		*prev;
	struct user		*user;
	struct service		*user_link_next;
	struct service		*user_link_prev;
	struct smd_qualifier	q_id;
	struct params		param;
} smd_service_t;

#include "globals.h"

#endif
