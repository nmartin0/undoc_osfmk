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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/smd.c,v 1.13 1994/11/19 03:04:32 mtm Exp $
 *
 * HISTORY
 * $Log: smd.c,v $
 * Revision 1.13  1994/11/19  03:04:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/06/13  16:58:40  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: EATS
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 * 	cmds_libs/src/usr/sbin/allocator/allocator.c
 * 	cmds_libs/src/usr/sbin/allocator/allocutils.c
 * 	cmds_libs/src/usr/sbin/allocator/conflict.c
 * 	cmds_libs/src/usr/sbin/allocator/init_appl.c
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c
 * 	cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c
 * 	cmds_libs/src/usr/sbin/allocator/schedule.c
 * 	cmds_libs/src/usr/sbin/allocator/server_loop.c
 * 	cmds_libs/src/usr/sbin/allocator/smd.c
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 *
 * Revision 1.11  1993/11/18  20:24:06  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.10  1993/09/02  17:33:30  carbajal
 * Changed app->data.rolled_in = app->rolled_in to
 * app->data.rolled_in = appl->elapsed. PTS # 5999
 *
 * Revision 1.9  1993/08/28  01:09:17  carbajal
 * removed smd_down PTS #6141
 *
 * Revision 1.8  1993/07/20  01:14:05  carbajal
 * define double dclock()
 *
 * Revision 1.7  1993/07/19  17:38:47  carbajal
 * Set nx_acctid for MACS
 *
 * Revision 1.6  1993/07/18  19:48:14  carbajal
 * Call get_global_time
 *
 * Revision 1.5  1993/01/28  21:39:09  shala
 * Fixed multiply defined LP_MAP_T .
 *
 * Revision 1.4  1992/11/03  01:46:43  carbajal
 * Made sure all printf were surrounded by ifdef debug
 * statements.
 *
 * Revision 1.3  1992/10/23  18:14:06  rkl
 * new_appl was not getting cleared - but is now.
 *
 * Revision 1.2  1992/10/08  23:46:14  rkl
 * Removed alloc.h include which is not needed in prep for making allocator
 * server/user libraries.
 *
 * Revision 1.1  1992/10/08  22:33:19  rkl
 * Handle SMD interface.
 *
 *
 */

/*
 * smd.c	- Roy Larsen	10/92
 *
 * This file contains the Scheduler Monitor Daemon interface routines.
 *
 */
#include <mcmsg/mcmsg_appl.h>
#include <nx/schedule.h>
#include <mach/mach.h>
#include <nx/smd.h>
#include <sys/socket.h>

double	dclock();

/*
 *  Macros
 */
#define CONVERT(t)	t == APP_START   ? ALLOCATOR_START_IND   : \
			t == APP_ROLLIN  ? ALLOCATOR_ROLLIN_IND  : \
			t == APP_ROLLOUT ? ALLOCATOR_ROLLOUT_IND : \
			                   ALLOCATOR_END_IND
/*
 *  SMD socket file descriptor
 */
static	int		smd_fd = -1;
static	allocator_ind_t	ind;

/*
 * notify_smd() - Notify SMD of scheduler event.
 */
void
notify_smd(appl, event)
	APPL_T	*appl;		/* Application */
	int	event;
{
	PART_T		*part = appl->parent_lyr->part;
	double		clock;


	debug_sched(3, "notify_smd(appl=%x, event=%d)\n", appl, event);

	/*
	 *  See if the connection is open.
	 */
	if (smd_fd == -1) {
		debug_sched(3,"SMD connection is down\n");
		return;
	}

	/*
	 *  Format the indication.
	 */
	ind.hdr.len  = sizeof(allocator_ind_t) - sizeof(smd_hdr_t);
	ind.hdr.type = CONVERT(event);

	ind.data.pgid           = appl->pgroup;
	ind.data.acct_id        = appl->nx_acctid;
	ind.data.uid            = appl->uid;
	ind.data.app_size       = appl->size;
	ind.data.app_rollin     = appl->elapsed;
	ind.data.app_priority   = appl->priority;

	ind.data.part_id        = part->inode;
	ind.data.part_size      = part->nodes;
	ind.data.rollin_quantum = part->rollin_quantum;
	ind.data.part_rollin    = part->rolled_in;
	ind.data.global_time    = get_global_time();
	ind.data.event_time     = time(0);

	/*
	 *  Send it.
	 */
	if (write(smd_fd, &ind, sizeof(allocator_ind_t)) < 0) {
		perror("SMD interface error - write");
		smd_fd = -1;
	}
}

/*
 * smd_init() - Setup socket to communicate with SMD
 */
/* ARGSUSED */
smd_init(server_port, socket_addr, socket_len, ret_code)
	mach_port_t	server_port;
	u_char		*socket_addr;
	int		socket_len;
	int		*ret_code;
{
	/*
	 *  This may be a restart of SMD.  If we already had a connection
	 *  then close it .
	 */
	if (smd_fd != -1)
		close(smd_fd);

	/*
	 *  Hope for the best.
	 */
	*ret_code = 0;

	/*
	 *  Create the socket and connect.
	 */
	smd_fd = socket(((struct sockaddr*)socket_addr)->sa_family,
								SOCK_STREAM, 0);
	if (smd_fd < 0) {
		smd_fd = -1;
		perror("SMD interface error - socket");
		*ret_code = -1;
	} else if (connect(smd_fd, socket_addr, socket_len) < 0) {
		smd_fd = -1;
		perror("SMD interface error - connect");
		*ret_code = -1;
	}
	return (*ret_code);
}
