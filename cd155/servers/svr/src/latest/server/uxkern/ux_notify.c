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
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: ux_notify.c,v $
 * Revision 1.10  1994/11/18  20:49:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/10/19  21:22:48  jlitvin
 * Put a DEBUG printf behind the appropriate #ifdef.  This is how the
 * rest of the debug messages are handled in this file.
 *
 *  Reviewer: nandy
 *  Risk: low
 *  Benefit or PTS #: 8718
 *  Testing: developer
 *  Module(s): server/uxkern/ux_notify.c
 *
 * Revision 1.8  1994/08/31  22:47:53  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.6.2.1  1994/08/30  16:10:47  nandy
 * Added call to task_to_proc_remove() in do_mach_notify_dead_name(). This
 * change got lost by the last merge
 *
 *  Reviewer: John Loverso
 *  Risk: L
 *  Benefit or PTS #: 10297 + 10687
 *  Testing: control-C EATs
 *  Module(s): ux_notify.c
 * 	 	kern_exit.c
 *
 * Revision 1.6  1994/03/14  02:08:53  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.5  1993/07/14  18:45:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:07:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:34:14  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:54:06  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2  1992/11/06  20:34:29  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:45:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.4  92/10/16  11:31:37  chrisp
 * For TNC, don't print a message for send-once notification.
 * 
 * Revision 2.6  93/06/16  13:54:49  klh
 * 	Revision 2.7  93/05/16  20:59:45  loverso
 * 		Add missing call to task_to_proc_remove() in dead-name,
 * 		and missing mach_port_deallocate() in port-deleted. (loverso)
 * 
 * Revision 2.5  93/04/24  18:45:42  klh
 * 	Revision 2.6  93/04/08  11:43:53  loverso
 * 		Avoid race conditions with system call messages: 
 *		invalidate the dead task port.
 * 		[1992/09/23  12:58:57  barbou]
 *
 * 		Also, add #include <profiling.h> missing from last merge.
 *
 * 	Revision 2.5  93/01/07  11:16:06  condict
 * 		Update notify_no_senders for new Mach profiling interface.
 * 		[1992/12/04  14:19:00  condict]
 *
 * Revision 2.4  92/10/16  11:31:37  chrisp
 * For TNC, don't print a message for send-once notification.
 * 
 * Revision 2.3  92/08/06  16:44:10  roman
 * Fix to not do printf() when a proc port is migrated for TNC.
 * 
 * Revision 2.2  92/07/28  20:00:34  rabii
 * 	New file from grV4.0.
 * 
 * Revision 1.2  1992/05/12  14:22:27  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/05  10:16:17  condict]
 *
 * Revision 3.1  92/04/22  16:50:26  barbou
 * 	Added missing inclusion of kern/parallel.h.
 * 
 * Revision 3.0  92/04/08  20:43:58  barbou
 * 	Created. Notification server routines.
 * 
 * $EndLog$
 */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <kern/parallel.h>

#include <uxkern/import_mach.h>

/* co-exist with the use of notify_server by lnsvr */
#define do_mach_notify_port_deleted	ux_do_mach_notify_port_deleted
#define do_mach_notify_msg_accepted	ux_do_mach_notify_msg_accepted
#define do_mach_notify_port_destroyed	ux_do_mach_notify_port_destroyed
#define do_mach_notify_no_senders	ux_do_mach_notify_no_senders
#define do_mach_notify_send_once	ux_do_mach_notify_send_once
#define do_mach_notify_dead_name	ux_do_mach_notify_dead_name
#define notify_server			ux_notify_server
#define notify_server_routine 		ux_notify_server_routine
#include <mach/notify_server.c>

#include <profiling.h>

/*
 *	Mach port notification handler.
 */

mach_port_t	ux_notify_port = MACH_PORT_NULL;

/*
 * Sets the notification port.
 */
void ux_notify_init()
{
	register kern_return_t	r;

	/*
	 *	Allocate the notify port.
	 */
	r = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			       &ux_notify_port);
	if (r != KERN_SUCCESS)
		panic("ux_notify_init: can't allocate");

	/*
	 * Add it to the server port set.
	 */
	ux_server_add_port(ux_notify_port);
}

/*
 * Dead name notification.
 */
kern_return_t
do_mach_notify_dead_name(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
	extern struct proc	*task_to_proc_lookup();
	struct proc 		*p;
	register struct uthread	*uth = &u;

	p = task_to_proc_lookup(name);
	if (p == 0) {
#if 0
		printf("do_mach_notify_dead_name: name=0x%x ???\n", name);
#endif 
		(void) mach_port_deallocate(mach_task_self(), name);
		return KERN_FAILURE;
	}

	if (server_thread_register(uth, p) != 0) {
		/* process doesn't accept system calls anymore */
		(void) mach_port_deallocate(mach_task_self(), name);
		return KERN_SUCCESS;
	}
	uarea_init(uth, p);
#ifdef	DEBUG
	printf("do_mach_notify_dead_name: killing terminated proc %d(%s).\n",
	       p->p_pid, p->p_utask.uu_comm);
#endif
	unix_master();
	task_to_proc_remove(name);
	p->p_task = MACH_PORT_DEAD;
	(void) proc_exit(p, SIGKILL, FALSE);
	unix_release();

	(void) mach_port_deallocate(mach_task_self(), name);
	if (uth->uu_procp == 0) {
		uarea_terminate(uth, p);
	} else {
		uarea_terminate(uth, p);
		server_thread_deregister(uth, p);
	}
	return KERN_SUCCESS;
}

/*
 * Port deleted notification.
 */
kern_return_t
do_mach_notify_port_deleted(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
#ifdef DEBUG
	printf("do_mach_notify_port_deleted: name=0x%x ???\n", name);
#endif
	(void) mach_port_deallocate(mach_task_self(), name);
	return KERN_FAILURE;
}

/*
 * Message accepted notification.
 */
kern_return_t
do_mach_notify_msg_accepted(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
#ifdef DEBUG
	printf("do_mach_notify_msg_accepted: name=0x%x ???\n", name);
#endif
	(void) mach_port_deallocate(mach_task_self(), name);
	return KERN_FAILURE;
}

/*
 * No senders notification.
 */
kern_return_t
do_mach_notify_no_senders(notify, mscount)
	mach_port_t		notify;
	mach_port_mscount_t	mscount;
{
#if PROFILING
	if (sample_done(notify))
		return KERN_SUCCESS;
#endif
#ifdef DEBUG
	printf("do_mach_notify_no_senders: mscount=0x%x ???\n", mscount);
#endif
	return KERN_FAILURE;
}

/*
 * Port destroyed notification.
 */
kern_return_t
do_mach_notify_port_destroyed(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
#ifdef DEBUG
	printf("do_mach_notify_port_destroyed: name=0x%x ???\n", name);
#endif
	(void) mach_port_deallocate(mach_task_self(), name);
	return KERN_FAILURE;
}

/*
 * Send once notification.
 */
kern_return_t
do_mach_notify_send_once(notify)
	mach_port_t	notify;
{
#ifdef	DEBUG
	printf("do_mach_notify_send_once: ???\n");
#endif
	return KERN_FAILURE;
}
