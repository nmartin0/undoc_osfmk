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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */

/*
 * HISTORY
 * $Log: tnc_svipc.c,v $
 * Revision 1.5  1995/02/01  21:56:55  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.4  1994/11/18  20:44:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/25  23:30:11  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: High -- many lines changed in many files
 *  Benefit or PTS #: 9853
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/bsd/init_main.c
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_fork.c
 * 	server/bsd/kern_prot.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/sys/user.h
 * 	server/tnc/chkpnt_pproc.c
 * 	server/tnc/rvp_subr.c
 * 	server/tnc/tnc_svipc.c
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/syscall_subr.c
 * Side-thread changes.  Init (and zero) u.u_procp at the correct time to
 * respect more thorough assertions in several thread setup/teardown routines.
 *
 * Revision 1.2  1993/10/28  03:11:27  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.1  1993/07/01  20:48:53  cfj
 * Adding new code from vendor
 *
 * Revision 3.0  93/06/02  09:38:37  yazz
 * First appearance.  Provides Sys V IPC support routines for TNC.
 * 
 */

#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/bsd_types.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <kern/zalloc.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/syscalltrace.h>
#include <tnc/dpvproc.h>

extern zone_t		svipc_zone;
extern struct svipc	*svipc_head;
extern struct mutex	svipc_lock;

/*
 * A one-time-only call from the emulator, for those tasks that actually
 * attempt a Sys V IPC operation.  Emulator has asked us (the proc's local
 * server) for a Sys V IPC port (snd rt) to communicate with the Sys V IPC
 * server.  We send a server-to-server message to the SVIPC server to obtain
 * a new SVIPC port and flag this task as having an svipc port.  Then we
 * return the svipc send right to the emulator.  SVIPC ports are unique by
 * pid and, once any Sys V IPC call is made, normally last the lifetime of
 * the process.  Note that fork/rfork'd child tasks do NOT inherit their
 * parent's svipc port, but that migrate'd and rexec'd tasks DO retain
 * the same svipc port.
 */
get_my_svipc_port(
	mach_port_t	*svipc_portp)
{
	mach_port_t	server_port;
	extern node_t	svipc_node;
	int		error = ESUCCESS;

	/*
	 * Get send rt to the System V IPC node's server port.
	 */
	error = tnc_get_server_port(svipc_node, &server_port);
	if (error != ESUCCESS) {
		panic("get_my_svipc_port: tnc_get_server_port (%d)",
				error);
	}

	error = cli_tnc_create_svipc_port(server_port, u.u_procp->p_pid,
			u.u_shmsegs, svipc_portp);

	u.u_svipc_flag = TRUE;		/* know to tell SVIPC server on exit */
	return(error);
}

/*
 * Process on local server is exiting.  Tell SVIPC server so it can
 * remove this proc's svipc structure and port, and do any required
 * semaphore adjustments.
 */
void
tnc_semexit()
{
	mach_port_t	server_port;
	extern node_t	svipc_node;
	int		error = ESUCCESS;

	if (!u.u_svipc_flag)
		return;		/* this proc never made any svipc calls */

	/*
	 * Tell Sys V IPC server task is exiting.
	 * Get send rt to the System V IPC node's server port.
	 */
	error = tnc_get_server_port(svipc_node, &server_port);
	if (error != ESUCCESS) {
		panic("get_my_svipc_port: tnc_get_server_port (%d)",
				error);
	}
	cli_tnc_destroy_svipc_port(server_port, u.u_procp->p_pid);
}

/*
 * Establish an svipc structure and its associated svipc port.
 * Mark the svipc structure to indicate that it has a port attached
 * and add the port to the server's port set.
 */
svr_tnc_create_svipc_port(
	mach_port_t	server_port,
	pid_t		pid,
	int		shmsegs,
	mach_port_t	*svipc_portp)
{
	mach_port_t	port;
	struct svipc	*svp;
	int		error;

	/*
	 * First see if we happen to have an svipc port for this pid
	 * already.
	 */
	mutex_lock(&svipc_lock);
	for (svp = svipc_head; svp; svp = svp->sv_forw) {
		if (pid == svp->sv_pid) {
			mutex_unlock(&svipc_lock);
			SVIPC_TO_PORT_LOOKUP(svp,port);
			*svipc_portp = port;
			return(ESUCCESS);
		}
	}
	mutex_unlock(&svipc_lock);

	/*
	 * Allocate the memory for the svipc structure.
	 */
	svp = (struct svipc *)zalloc(svipc_zone);
	if (svp == (struct svipc *)ZONE_NULL) {
		panic("svr_tnc_create_svipc_port: zalloc failed");
	}

	/*
	 * Allocate a port to go with the svipc structure, that is
	 * named after the svipc structure's address.
 	 */
	SVIPC_TO_PORT_LOOKUP(svp,port);
	error = mach_port_allocate_name(mach_task_self(),
			MACH_PORT_RIGHT_RECEIVE, port);
	if (error != KERN_SUCCESS) {
		panic("svr_tnc_create_svipc_port: can't alloc file port "
				" error=0x%x portname=0x%x", error, port);
	}

	/*
	 * Make a send right to copy back to requester.
	 */
	error = mach_port_insert_right(mach_task_self(),
			port, port, MACH_MSG_TYPE_MAKE_SEND);
	if (error != KERN_SUCCESS) {
		panic("svr_tnc_create_svipc_port: can't acquire send rights (0x%x)",
				error);
	}

	/*
	 * Set the magic number in the svipc structure to indicate that
	 * it has an attached port.  Init the pid and other info too.
	 */
	svp->sv_magic = SV_MAGIC;
	svp->sv_pid = pid;
	svp->sv_shmsegs = shmsegs;
	svp->sv_refcnt = 1;
	mutex_init(&(svp->sv_lock));

	/*
	 * Add to master svipc linked list of active svipc ports.
	 */
	mutex_lock(&svipc_lock);
	svp->sv_forw = svipc_head;
	svp->sv_back = (struct svipc *)NULL;
	if (svipc_head)
		svipc_head->sv_back = svp;
	svipc_head = svp;
	mutex_unlock(&svipc_lock);

	/*
	 * Tell the server to service the port.
	 */
	ux_server_add_port(port);

	*svipc_portp = port;		/* return copied send right to caller */
	return(error);
}

/*
 * Deallocate an svipc structure.  Ref count has hit 0.
 */
void
dealloc_svp(
	struct svipc	*svp)
{
	mach_port_t	port;
	kern_return_t	ret;

	mutex_lock(&svipc_lock);
	if (svp->sv_refcnt > 0)
		panic("dealloc_svp: sv_refcnt > 0 0x%x %d",
				svp, svp->sv_refcnt);

	if (svipc_head == svp)
		svipc_head = svp->sv_forw;	/* 1st entry on list */
	else
		svp->sv_back->sv_forw = svp->sv_forw;
	if (svp->sv_forw)			/* not last entry on list */
		svp->sv_forw->sv_back = svp->sv_back;
	mutex_unlock(&svipc_lock);

	SVIPC_TO_PORT_LOOKUP(svp,port);
	ret = mach_port_destroy(mach_task_self(), port);
	if (ret != KERN_SUCCESS)
		panic("dealloc_svp: destroy of port 0x%x failed ret=%d",
				port, ret);

	svp->sv_forw = (struct svipc *)NULL;
	svp->sv_back = (struct svipc *)NULL;
	zfree(svipc_zone, svp);		/* return entry to its zone pool */
	return;
}

/*
 * Destroy an svipc structure and its associated svipc port.
 */
svr_tnc_destroy_svipc_port(
	mach_port_t	server_port,
	pid_t		pid)
{
	struct proc		*p;
	struct uthread		*uth = &u;
	struct svipc		*svp;
	mach_port_t		svipc_port;
	extern struct proc	*fsvr_get_proc();

	mutex_lock(&svipc_lock);
	for (svp = svipc_head; svp; svp = svp->sv_forw) {
		if (svp->sv_pid == pid)
			break;
	}
	mutex_unlock(&svipc_lock);

	if (!svp)
		panic("svr_tnc_destroy_svipc_port: port/pid 0x%x/%d not found",
				svipc_port);

	SVIPC_TO_PORT_LOOKUP(svp,svipc_port);

	/*
	 * Must init a whole dummy proc area, just for semexit().
	 */
	p = fsvr_get_proc(uth);

	ASSERT(p->p_rcred == NOCRED);
	p->p_rcred = crget();
	uth->uu_procp = 0;	/* uarea_init() requires this */
	uarea_init(uth, p);	/* fsvr_uarea_init() would mess up creds */

	u.u_semundo = svp->sv_semundo;	/* only this u. field is needed... */
	semexit();			/* to make semaphore adjustments */

	if (--svp->sv_refcnt <= 0)
		dealloc_svp(svp);

	/*
	 * Undo the uarea_init() done above, free creds and free the
	 * dummy proc.
	 */
	uarea_terminate(uth);
	crfree(p->p_rcred);
	p->p_rcred = NOCRED;  
	fsvr_release_proc(uth, p);

	if (uth->uu_master_lock)
		panic("svr_tnc_destroy_svipc_port: master lock still held");

	return(ESUCCESS);
}
