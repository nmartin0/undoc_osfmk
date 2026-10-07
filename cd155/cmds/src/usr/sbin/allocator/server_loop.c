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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/server_loop.c,v 1.22 1994/11/19 03:04:31 mtm Exp $
 *
 */

/* History
 * $Log: server_loop.c,v $
 * Revision 1.22  1994/11/19  03:04:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.21  1994/06/13  16:58:30  sdh
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
 * Revision 1.20  1993/11/18  20:24:04  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.19  1993/07/20  18:13:05  carbajal
 * Removed obsolete types, includes and externs
 *
 * Revision 1.18  1993/04/13  01:13:02  carbajal
 * Calculate interval after receiving a mach_msg, but before we call
 * any routines. This makes pspart work more accurately.
 *
 * Revision 1.17  1993/03/31  21:16:42  carbajal
 * Changed calculation of interval
 *
 * Revision 1.16  1993/03/25  02:45:25  carbajal
 * Cleaned up compiler warnings
 * Cleaned up code that handles readjusting layer priorities
 * server_loop.c: now calls schedule() before responding to server RPC
 *
 * Revision 1.15  1993/02/01  17:41:49  shala
 * Increase the time out value from 10 sec. to 1 hour.
 *
 * Revision 1.14  1993/01/28  19:48:35  carbajal
 * Removed multiply defined LP_MAP_T stuff. This is no longer
 * in allocator.h but is now in msmsg_appl.h
 *
 * Revision 1.13  1993/01/18  20:06:00  carbajal
 * server_loop.c: Don't stop new application here, the scheduler
 * now handles it.
 *
 * Revision 1.12  1993/01/05  22:38:12  shala
 * Support nx_pri.
 *
 * Revision 1.11  1992/12/24  03:20:17  carbajal
 * call notify_smd if new_appl first then call
 * gang_stop.
 *
 * Revision 1.10  1992/12/18  17:06:55  carbajal
 * call gang_stop instead of kill directly
 *
 * Revision 1.9  1992/10/29  01:28:48  carbajal
 * Initialized interval to last_scheduled before entering
 * main loop. This fixes some pspart inconsistencies.
 * Also added history to file.
 *
*/
/*
 *server_loop.c	4/92
 *
 * This is the MIG dispatch loop for the allocator. It is derived from 
 * mach_msg_server.c revision 2.4.
 */

/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include <mach/mach.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <signal.h>
#include <sys/errno.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/schedule.h>
#include "macros.h"
#include "debug.h"

/*
 * Smallest_rollin_quantum contains the smallest rollin quantum of any active
 * partition, but it will never be set to greater than MAX_SCHED_QUANTUM.
 */
#define	MAX_SCHED_QUANTUM	60*60*1000   /* 1 hour worth of milliseconds */
unsigned long	smallest_rollin_quantum;
unsigned long	interval;
unsigned long	last_scheduled;

double	dclock();

mach_msg_return_t
server_loop(demux, max_size, rcv_name, root)
    boolean_t (*demux)();
    mach_msg_size_t max_size;
    mach_port_t rcv_name;
	PART_T	*root;	/* root partition */
{
	register mig_reply_header_t *bufRequest, *bufReply, *bufTemp;
	register mach_msg_return_t mr;
	mach_msg_timeout_t	timeout;
	unsigned long		current_time;

	bufRequest = (mig_reply_header_t *) malloc(max_size);
	if (bufRequest == 0)
		return KERN_RESOURCE_SHORTAGE;
	bufReply = (mig_reply_header_t *) malloc(max_size);
	if (bufReply == 0)
		return KERN_RESOURCE_SHORTAGE;

	smallest_rollin_quantum =
	    find_smallest_rollin_quantum(root, MAX_SCHED_QUANTUM);
	timeout = smallest_rollin_quantum;
	last_scheduled = GET_TIME_IN_MILLISECONDS();
	interval = 0;

	for (;;) {
	get_request:
		mr = mach_msg(&bufRequest->Head, MACH_RCV_MSG|MACH_RCV_TIMEOUT,
		      0, max_size, rcv_name,
		      timeout, MACH_PORT_NULL);

		while (mr == MACH_MSG_SUCCESS) {
		/* we have a request message */

	    		current_time = GET_TIME_IN_MILLISECONDS();
	    		interval = current_time - last_scheduled;

			(void) (*demux)(&bufRequest->Head, &bufReply->Head);
			if (bufReply->RetCode != KERN_SUCCESS) {
				if (bufReply->RetCode == MIG_NO_REPLY)
					goto get_request;

				/* don't destroy the reply port right,
				 * so we can send an error message
				 */
				bufRequest->Head.msgh_remote_port = MACH_PORT_NULL;
				mach_msg_destroy(&bufRequest->Head);
			}
			if (bufReply->Head.msgh_remote_port == MACH_PORT_NULL) {
			/* no reply port, so destroy the reply */
				if (bufReply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX)
					mach_msg_destroy(&bufReply->Head);
				goto get_request;
			}

			/*
			* We serviced a request that changed state, so we will
			* reschedule. First, though, we check to see if there
			* was a new application started. If there was we must
	     		* send a signal to halt it until it is time for its
	     		* time slice.
	     		*/

	    		current_time = GET_TIME_IN_MILLISECONDS();
	    		interval = current_time - last_scheduled;
	
	    		schedule(interval, root);
	    		last_scheduled = current_time;
	    		smallest_rollin_quantum =
				find_smallest_rollin_quantum(root, MAX_SCHED_QUANTUM);
			timeout = smallest_rollin_quantum;

			/* send reply */

			bufTemp = bufRequest;
			bufRequest = bufReply;
			bufReply = bufTemp;

			mr = mach_msg(&bufRequest->Head,
	                  	MACH_SEND_MSG,
			  	bufRequest->Head.msgh_size, max_size, rcv_name,
			  	timeout, MACH_PORT_NULL);

			mr = mach_msg(&bufRequest->Head,
	                  	MACH_RCV_MSG|MACH_RCV_TIMEOUT,
			  	bufRequest->Head.msgh_size, max_size, rcv_name,
			  	timeout, MACH_PORT_NULL);

	} /* while */

	if (mr == MACH_RCV_TIMED_OUT) {
		/*
		 * We timed out, this means that it is time to reschedule.
		 */
		current_time = GET_TIME_IN_MILLISECONDS();
		interval = current_time - last_scheduled;
		schedule(interval, root);
		last_scheduled = current_time;
		smallest_rollin_quantum =
		    find_smallest_rollin_quantum(root, MAX_SCHED_QUANTUM);
		timeout = smallest_rollin_quantum;
		goto get_request;
	}

	/* a message error occurred */

	if (mr != MACH_SEND_INVALID_DEST)
	    break;

	/* the reply can't be delivered, so destroy it */
	mach_msg_destroy(&bufRequest->Head);
    }

    debug_print(DEBUG_OTHER, 0, "Allocator exiting!\n");
    mach_error("allocator:",mr);
    free((char *) bufRequest);
    free((char *) bufReply);
    return mr;
}
