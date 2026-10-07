/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * Mach Operating System
 * Copyright (c) 1988 Encore Computer Corporation
 * All rights reserved.
 */
/*
 * HISTORY
 * $Log: select.h,v $
 * Revision 1.7  1995/02/01  21:37:09  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:41:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/05/04  21:40:56  mjl
 * TNC select rewrite.  Add unique id and flag word to select queue entry
 * (see vsocket/vs_subr.h).
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.4  1993/08/04  03:54:40  cfj
 * 08-03-93 Code drop from Locus.
 *
 * Revision 1.3  1993/07/14  18:27:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.8  93/07/31  15:29:09  mjl
 * Move comment displaced by automagic merge back to its correct spot.
 * 
 * Revision 2.7  1992/07/26  17:28:41  bhk
 * added support for select on remote sockets
 *
 * Revision 2.6  92/05/31  18:58:55  loverso
 * 	Revision 2.5.1.2  92/05/26  15:48:20  loverso
 * 	Remove erroneous select_cleanup() declaration.
 * 
 * 	Revision 2.5.1.1  92/05/21  15:15:27  loverso
 * 	Revert to using file port instead of file pointer for select.
 * 
 * Revision 2.5  92/05/12  00:05:46  loverso
 * 	sel_queue_t loses file port, pid, and ucred, and picks up
 * 	file pointer. (loverso)
 * 
 * Revision 2.4  91/10/04  15:03:37  chrisp
 * Get rid of extraneous $Log.
 * Remove credentials port and add pid and ucred structure to selq structure.
 * This is for the new philosophy used in server/bsd/subr_select.c.
 * 
 * Revision 2.3  91/09/16  16:47:29  rabii
 * 	Merge of V2.0 and Locus (locus check-in by yazz)
 * 	Changes in information stored due to emulator handling most of
 * 	select() and poll() operation.
 * 
 * Revision 2.2  91/08/31  14:11:12  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:44:47  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/12/06  14:05:02  devrcs
 * 	Cleanup copyright and history log comments.
 * 	[90/11/15  11:11:57  gm]
 * 
 * Revision 1.3  90/10/07  14:53:44  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:44:29  gm]
 * 
 * Revision 1.2  90/06/22  20:53:52  devrcs
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Created.					alan@encore.com
 * 	[90/06/12  21:39:08  gmf]
 * 
 * $EndLog$
 */

#ifndef	_SYS_SELECT_H_
#define	_SYS_SELECT_H_

#include <kern/event.h>
#include <kern/queue.h>
#include <sys/ucred.h>
#ifdef TNC
#include <vsocket/vs_subr.h>
#endif

/*
 * Problems with select in a parallelized Unix environment.
 *
 * The existing BSD select mechanism, a set of variable contained in
 * each selectable object and a set of global variables to handle
 * "select collisions" does not work well in a parallelized Unix kernel.
 * First, there is the obvious race between one thread attempting to
 * set itself up to be notified of a change in the state of a selectable
 * object and another thread attempting to signal that a selectable
 * object's state has just changed.
 *
 * Second, and more insidious, is the problem of the task's file descriptor
 * array changing during the select due to the action of another thread
 * within the task.  For example, at the time thread A initiates a select()
 * operation, file descriptor X could be open and specified as part of one
 * of the select masks.  Thread A might then sleep, waiting for one of the
 * select'ed objects to change state.  In the meantime, thread B might close
 * file descriptor X.  According to the original code, thread A would not be
 * awakened as the result of thread B's close operation.  Possibly thread A
 * might never awaken, if it was only waiting on the object closed out by B.
 * Should thread A finally awoken, thread A will return with a select error
 * because it now has an invalid file descriptor in one of its select masks.
 *
 * The solution Encore developed for select uses a queue of select events in
 * each selectable object and a select event in the uthread structure.
 * Threads add themselves to a selectable object's select queue and then sleep
 * on the uthread's select event.  When a state change takes place, any
 * threads linked on the object's select queue are awoken.  At the end of a
 * select operation, unselect routines are invoked for each object to clean
 * up any remaining entries on the various select queues.
 *
 * Each object close routine changes to wake up any threads queued on the
 * object's select queue.  These threads will then re-poll all of the
 * objects that interest them; possibly some will then report invalid
 * file descriptors as the result of the original close.
 *
 * Changes:
 *	fo_select	now takes an additional "scanning" argument
 *	xxx_select	now takes an additional "scanning" argument
 *			and calls select_{enqueue,dequeue} as appropriate
 *	xxx_close	now must do a "select_wakeup"
 *	xxx_whatever	changes to do a "select_wakeup"
 * When "scanning" is non-zero, select() is performing the normal select
 * operation and the lower-level routine must return non-zero if the
 * object is selectable.  When "scanning" is zero, select() is cleaning
 * out old select queue entries and the lower-level routine must return zero.
 */

struct sel_queue {
	struct queue_entry	links;
	mach_port_t		selq_file_port;
					/* delayed reply port */
	mach_port_t		selq_delay_port;
					/* delayed reply port */
	mach_port_t		selq_creds_port;
					/* credentials port */
#ifdef TNC
	int			selq_flags;
	sel_id_t		selq_sel_id;
#endif
	short			selq_index;
					/* must be returned to emulator */
	short			selq_events;
					/* events requested on this fp */
};
typedef struct sel_queue	sel_queue_t;

void	select_init();		/* Initialize select subsystem */
void	select_enqueue();	/* Put current thread on a select queue */
void	select_dequeue();	/* Remove current thread from a sel queue */
void	select_dequeue_all();	/* Remove all threads from a sel queue */
void	select_wakeup();	/* Notify all threads parked on a sel queue */

#endif	/* _SYS_SELECT_H_ */
