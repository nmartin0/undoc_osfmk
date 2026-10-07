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
 * Mach Operating System
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: syscall.c,v $
 * Revision 1.8  1995/01/20  20:20:18  yazz
 *  Reviewer: none
 *  Risk: 0
 *  Benefit or PTS #: init local used as return from nosys(), was uninitted.
 *  Testing: EATs controlc, sched, os_interfaces
 *  Module(s): server/uxkern/syscall.c
 * Init local used as return from nosys(), was uninitted.
 *
 * Revision 1.7  1994/11/18  20:49:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/18  00:25:36  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.5  1994/01/12  17:45:59  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.4  1993/07/14  18:44:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:33:13  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:53:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.14  1993/04/24  18:48:47  klh
 * 	Revision 2.13  93/01/06  10:37:08  loverso
 * 		Fix notice.
 *
 * Revision 2.13  92/10/06  12:23:20  roman
 * Fix RCS comments.
 * 
 * Revision 2.12  92/10/05  13:39:35  klh
 * 	Revision 2.12  92/09/24  16:51:30  rabii
 * 		Added code to take/release master lock around call to psignal3
 *		in nosys to fix bug #427 (rabii)
 * 
 * Revision 2.11  92/07/30  16:21:22  chrisp
 * end_vprocserver_op() now requires the vproc port to be passed as its first
 * 	parameter.
 * 
 * Revision 2.10  92/05/31  18:59:53  loverso
 * 	Remove duplicated include of parallel.h (pjg).
 * 
 * Revision 2.9  92/05/24  14:00:09  pjg
 * 	Call unix_master and unix_release if NCPUS == 1.
 * 
 * 	If server_thread_register fails in bsd_get_bootstrap_directories,
 * 	return MIG_REMOTE_ERROR to ux_server_loop to destroy the message.
 * 
 * Revision 2.8  92/03/01  18:34:09  pjg
 * 	Moved bsd_get_bootstrap_directories here.
 * 
 * Revision 2.7  92/01/05  20:13:54  roy
 * 	92/01/02  17:19:45  roy
 * 	Removed syscall tracing from this file.
 * 
 * 	1991/12/16  22:24:05  noemi
 * 	Removed ux_fs_generic_server.
 * 
 * 	1991/12/07  03:50:44  noemi
 * 	syscalltrace fixes.
 * 
 * 	1991/10/14  21:03:18  noemi
 * 	Updated to use new start_fileserver_op and end_fileserver_op functions.
 * 	Removed obsolete uses of msgh_kind in message headers.
 * 
 * 	1991/09/22  21:53:58  noemi
 * 	OSF1/ADFS update
 * 
 * Revision 2.6  91/12/18  14:52:54  roy
 *      91/12/13  13:04:13  sp
 *      Remove CMUCS conditionals and old unused BSD code and resource
 *      pausing code
 *
 *      91/11/13  14:56:13  barbou
 *      Fixed nosys(): SIGSYS is a per-thread signal (in fact, the server
 *      didn't care about this bug: there are no per-thread signals !).
 *
 *      91/10/10  15:59:25  barbou
 *      Initialize the errno for nosys.
 *
 *      91/10/09  18:42:23  emcmanus
 *      Removed diagnostic from nosys().
 *
 * Revision 2.5  91/11/22  15:20:36  rabii
 *      locus merge
 *      Change call from end_server_op to end_vprocserver_op (left as
 *      end_server_op accidently. (roman)
 *
 * Revision 2.4  91/10/04  15:24:34  chrisp
 * Get rid of references to msgh_kind field, since this is now a
 * sequence number.
 * 
 * Revision 2.3  91/09/17  09:13:14  sjs
 * integrate Locus changes	roman
 * Added the new routines ux_fs_generic_server() and
 * ux_vp_generic_server() for handling generic file and vproc
 * system calls (i.e. calls where the destination port is a file or
 * a vproc rather than a proc port). Simplified the commonly used
 * "return(FALSE)" path for generic_server().
 * 
 * Revision 2.2  91/08/31  14:28:06  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.6  91/08/27  15:39:04  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.5  91/06/25  17:14:06  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.4  91/06/07  15:54:19  barbou
 * Asynchronous posting of SIGSYS by nosys() because parallelized syscall.
 * 
 * Revision 3.3  91/05/24  11:38:30  jose
 * Added array for syscall return values.
 * 
 * Revision 3.2  91/03/08  16:06:35  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:28:45  condict
 * Remove CMU syscalls.
 * 
 * Revision 3.0  91/01/17  12:06:17  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.10  91/03/20  14:36:03  dbg
 * 	Include syscalltrace.h control file, else we do a half job.
 * 	[91/02/21  11:47:54  af]
 * 
 * Revision 2.9  90/11/05  16:58:23  rpd
 * 	Make work w/o MAP_UAREA.
 * 	[90/11/01            rwd]
 * 
 * Revision 2.8  90/08/06  15:35:11  rwd
 * 	Added share lock panic.
 * 	[90/07/05            rwd]
 * 
 * Revision 2.7  90/06/19  23:16:10  rpd
 * 	Purged CMUCS conditionals.
 * 	[90/06/08            rpd]
 * 
 * Revision 2.6  90/06/02  15:28:22  rpd
 * 	Revamped uu_reply_msg handling in ux_generic_server.
 * 	[90/04/27            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  20:23:01  rpd]
 * 
 * Revision 2.5  90/05/29  20:25:19  rwd
 * 	Added panic for attempt to exit while holding the master lock.
 * 	[90/05/09            rwd]
 * 
 * Revision 2.4  90/01/23  00:05:33  af
 * 	Do not initialize syscalltrace here, or else we conflict
 * 	with its other definition in syscall_subr.c
 * 	[90/01/20  23:30:08  af]
 * 
 * Revision 2.3  89/11/29  15:31:35  af
 * 	Added rval2, because some syscalls do not modify it and it
 * 	must be preserved.
 * 	[89/11/20            af]
 * 	Use "extern" for decls of external variables.
 * 	[89/11/16  15:31:02  af]
 * 
 * Revision 2.2  89/10/17  11:27:12  rwd
 * 	Add interrupt return parameter.
 * 	[89/09/21            dbg]
 * 
 * $EndLog$
 */

#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/systm.h>

#include <uxkern/proc_to_task.h>
#include <uxkern/syscall_subr.h>
#include <uxkern/bsd_msg.h>
#include <uxkern/syscalltrace.h>

extern struct sysent	sysent[];
int			nsysent;


/*
 * Generic UX server message handler.
 * Generic system call.
 */
boolean_t
ux_generic_server(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
{
	register struct uthread		*uth = &u;
	register struct bsd_request	*req = (struct bsd_request *)InHeadP;
	register struct bsd_reply	*rep = (struct bsd_reply *)OutHeadP;
	register struct sysent		*callp;
	register int			syscode = req->syscode;
	int				error, retval[2];

	/*
	 * Fix the reply message.
	 */
	static mach_msg_type_t bsd_rep_int_type = {
	    /* msgt_name */		MACH_MSG_TYPE_INTEGER_32,
	    /* msgt_size */		32,
	    /* msgt_number */		4,
	    /* msgt_inline */		TRUE,
	    /* msgt_longform */		FALSE,
	    /* msgt_deallocate */	FALSE,
	    /* msgt_unused */		0
	};

	if (InHeadP->msgh_id != BSD_REQ_MSG_ID)
	    return (FALSE);

	/*
	 * Set up standard reply.
	 */
	rep->hdr.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(InHeadP->msgh_bits), 0);
	rep->hdr.msgh_remote_port = InHeadP->msgh_remote_port;
	rep->hdr.msgh_local_port = MACH_PORT_NULL;
	rep->hdr.msgh_id = InHeadP->msgh_id + 100;
	rep->int_type = bsd_rep_int_type;
	rep->hdr.msgh_size = sizeof(struct bsd_reply);

	/*
	 * Set up server thread to handle process
	 */
	if ((rep->retcode = start_server_op(req->hdr.msgh_local_port,
					    req->syscode))
		!= KERN_SUCCESS)
	{
	    return (TRUE);
	}

	/*
	 * Save the reply msg and initialize current_output.
	 * The user_copy/user_reply_msg code uses them for copyout.
	 */
	uth->uu_reply_msg = &rep->hdr;
	uth->uu_current_size = 0;

	retval[0] = 0;
	retval[1] = req->rval2;

	/*
	 * Find the system call table descriptor for this call.
	 */
	if (syscode < 0)
	    callp = &sysent[0];
	else {
	    if (syscode >= nsysent)
		callp = &sysent[0];
	    else
		callp = &sysent[syscode];
	}

	/*
	 * Switch to master if needed.
	 */
	if (NCPUS == 1 || callp->sy_parallel == 0) {
	    unix_master();
	}

	/*
	 * Catch any signals.  If no other error and not restartable,
	 * return EINTR.
	 */
	error = (*callp->sy_call)(
				  port_to_proc_lookup(req->hdr.msgh_local_port),
				  req->arg,
				  retval);

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || callp->sy_parallel == 0) {
	    unix_release();
	}

#if	MAP_UAREA
	if (uth->uu_share_lock_count) {
	    panic("Share lock still held", uth->uu_share_lock_count);
	    uth->uu_share_lock_count = 0;
	}
#endif	MAP_UAREA

	if (uth->uu_master_lock)
	    panic("Master still held", uth->uu_master_lock);

	rep->retcode = end_server_op(error, &rep->interrupt);
	rep->rval[0] = retval[0];
	rep->rval[1] = retval[1];

	/*
	 * Wrap up any trailing data in the reply message.
	 */
	finish_reply_msg();

	return (TRUE);
}


nosys()
{
	int error = ESUCCESS;

	if ((signal_disposition(SIGSYS) == SIG_IGN) ||
	    (signal_disposition(SIGSYS) == SIG_HOLD))
		error = EINVAL;

	unix_master();	/* we need master lock for psignal3 */
	psignal3(u.u_procp, SIGSYS, FALSE);
	unix_release();
	return(error);
}


/*
 * Generic vproc server message handler.
 * Generic vproc-directed system call.
 */
boolean_t
ux_vproc_generic_server(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
{
	register struct uthread		*uth = &u;
	register struct bsd_request	*req = (struct bsd_request *)InHeadP;
	register struct bsd_reply	*rep = (struct bsd_reply *)OutHeadP;
	register struct sysent		*callp;
	register int			syscode = req->syscode;
	int				error, retval[2];

	/*
	 * Fix the reply message.
	 */
	static mach_msg_type_t bsd_rep_int_type = {
	    /* msgt_name */		MACH_MSG_TYPE_INTEGER_32,
	    /* msgt_size */		32,
	    /* msgt_number */		4,
	    /* msgt_inline */		TRUE,
	    /* msgt_longform */		FALSE,
	    /* msgt_deallocate */	FALSE,
	    /* msgt_unused */		0
	};

	if (InHeadP->msgh_id != VPROC_REQ_MSG_ID)
	    return (FALSE);

	/*
	 * Set up standard reply.
	 */
	rep->hdr.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(InHeadP->msgh_bits), 0);
	rep->hdr.msgh_remote_port = InHeadP->msgh_remote_port;
	rep->hdr.msgh_local_port = MACH_PORT_NULL;
	rep->hdr.msgh_id = InHeadP->msgh_id + 100;
	rep->int_type = bsd_rep_int_type;
	rep->hdr.msgh_size = sizeof(struct bsd_reply);

	/*
	 * Set up server thread to handle process
	 */
	if ((rep->retcode = start_vprocserver_op(req->hdr.msgh_local_port,
						 req->syscode))
		!= KERN_SUCCESS)
	{
	    return (TRUE);
	}

	/*
	 * Save the reply msg and initialize current_output.
	 * The user_copy/user_reply_msg code uses them for copyout.
	 */
	uth->uu_reply_msg = &rep->hdr;
	uth->uu_current_size = 0;

	retval[0] = 0;
	retval[1] = req->rval2;

	if (syscode < 0)
	    callp = &sysent[0];
	else {
	    if (syscode >= nsysent)
		callp = &sysent[0];
	    else
		callp = &sysent[syscode];
	}

	/*
	 * Switch to master if needed.
	 */
	if (NCPUS == 1 || callp->sy_parallel == 0) {
	    unix_master();
	}

	error = (*callp->sy_call)(
			port_to_vproc_lookup(req->hdr.msgh_local_port),
			req->arg,
			retval);

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || callp->sy_parallel == 0) {
	    unix_release();
	}

#if	MAP_UAREA
	if (uth->uu_share_lock_count) {
	    panic("Share lock still held", uth->uu_share_lock_count);
	    uth->uu_share_lock_count = 0;
	}
#endif	MAP_UAREA

	if (uth->uu_master_lock)
	    panic("Master still held", uth->uu_master_lock);

	rep->retcode = end_vprocserver_op(req->hdr.msgh_local_port,
					  error, &rep->interrupt);
	rep->rval[0] = retval[0];
	rep->rval[1] = retval[1];

	/*
	 * Wrap up any trailing data in the reply message.
	 */
	finish_reply_msg();

	return (TRUE);
}


/*
 * Get bootstrap credentials port and vproc port for the initial proc. The
 * rest of the procs in the system get these ports via a fork() system
 * call.
 */
int
bsd_get_bootstrap_process_ports(proc_port, vproc_port, cred_port)
	mach_port_t	proc_port;
	mach_port_t	*vproc_port;
	mach_port_t	*cred_port;
{
	struct proc	*p = port_to_proc_lookup(proc_port);

	/*
	 * Get vproc port from vproc address.
	 */
	*vproc_port = vproc_to_port_lookup(p->p_vproc);

	/*
	 * Extract the credentials from the proc structure. It should
	 * have been set up by a previous newproc() call.
	 */
	*cred_port = p->p_cred;

	return(ESUCCESS);
}

/*
 * Handle the initial MiG bootstrap call to get an initial current working
 * directory and root directory.
 */
int
bsd_get_bootstrap_directories(proc_port, rootdir_port, currentdir_port)
	mach_port_t	proc_port;
	mach_port_t	*rootdir_port;
	mach_port_t	*currentdir_port;
{
	register struct proc	*p;
	register struct uthread	*uth = &u;
	extern	mach_port_t	root_vnode_port;

	/*
	 * Do a quick-and-dirty uarea setup
	 */
	if ((p = port_to_proc_lookup(proc_port)) == (struct proc *)0)
		return (ESRCH);
	if (server_thread_register(uth, p))
		return (MIG_REMOTE_ERROR);
	uarea_init(uth, p);

	ASSERT(root_vnode_port != MACH_PORT_NULL);
	*currentdir_port = *rootdir_port = root_vnode_port;

	/*
	 * Give up the uarea
	 */
	uarea_terminate(uth);
	server_thread_deregister(uth, p);
	return(ESUCCESS);
}

