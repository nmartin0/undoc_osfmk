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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/globals.h,v 1.4 1994/11/19 03:16:44 mtm Exp $
 *
 * HISTORY
 * $Log: globals.h,v $
 * Revision 1.4  1994/11/19  03:16:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/01/19  20:12:45  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.2  1992/10/05  23:43:12  rkl
 * Added copyright header.
 *
 *
 */

#ifndef _GLOBALS_H
#define	_GLOBALS_H

#include <sys/types.h>

/*
 *  errno
 */
extern	int	errno;

/*
 *  Interface globals.
 */
int	req_fd;			/* service request file descriptor */
int	max_fds;		/* maximum number of open FDs */
char	*iface_name;		/* service request path name */

/*
 *  Read buffer for incoming messages.
 */
union {
	smd_req_t	req;
	allocator_ind_t	ind;
} msg;

/*
 *  Database globals.
 */
int		total_apps;
smd_user_t	*user_db;		/* pointer to head of user database */
smd_app_t	*a_table[ A_HASH_SIZE     ];
smd_service_t	*p_table[ P_HASH_SIZE + 1 ];	/* extra slot is for services */
						/* with unspecified partitions*/
/*
 *  Timer interval
 */
int	timer_interval;

/*
 *  Socket globals.
 */
fd_set	readmask;		/* read select mask */
fd_set	writemask;		/* write select mask */
fd_set	exceptmask;		/* exception select mask */

/*
 *  Debug globals.
 */
int	debug_level;

#endif
