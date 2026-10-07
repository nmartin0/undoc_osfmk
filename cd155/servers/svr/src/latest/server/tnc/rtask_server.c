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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */

/*
 * $Log: rtask_server.c,v $
 * Revision 1.19  1995/02/01  21:49:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.18  1994/12/19  17:14:30  johannes
 * dpvpop_reap_multi(): Disabling busy waiting until process to be repeaped
 * 		     has become a zombie is no longer necessary to avoid
 *                      a deadlock with PARACORE, but it's good to increase
 *                      core dumping performance, because with trying reaping
 *                      over and over again core dumping slows down extremely.
 *                      Thus no change.
 *
 *  Reviewer: Bob Yazzi
 *  Risk: High (locking issue)
 *  Benefit or PTS #: 11799
 *  Testing: test case, developer testing, corefile EAT, controlc EAT
 *  Module(s): svr/server/tnc/pvp.ops
 *             svr/server/tnc/dvp_pvpops.c
 *             svr/server/paracore/dvp_pvpcore.c
 *
 * Revision 1.17  1994/11/18  20:43:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/07/27  16:47:04  johannes
 * In svr_rfork() the call of rfork_server() had to be extended.
 * In dpvpsop_rforkmulti() the call of rforkmulti_server() had to extended.
 * In svr_migrate() the call of migrate_server() had to be extended.
 *
 *  Reviewer: Nandini
 *  Risk: H
 *  Benefit or PTS #: information for absolute exec path in core files
 *  Testing: developer
 *  Module(s): server/sys: user.h
 *             server/bsd: kern_exec.c, kern_exit.c, kern_fork.c
 *             server/tnc: pvps.ops, tnc.defs, rtask_server.c
 *                         rtask_cli_pproc.c, rtask_cli_vproc.c
 *                         rtask_svr_pproc.c, rtask_svr_vproc.c
 *                         chkpnt_vproc.c
 *             server/paracore: core.c
 *
 * Revision 1.15  1994/07/26  14:10:00  johannes
 * dpvpop_reap_multi(): don't repeat local reap in case of EAGAIN
 *
 * This is to resolve a deadlock between waitmulti() and PARACORE:
 * The first core dumping child does a pgrp_core() RPC to the proxy node.
 * pgrp_core() calls pgrp_abort() which tries to get the vproc lock of the
 * process group leader (proxy). This lock is hold by waitmulti() while a
 * reap_multi is running. reap_multi() had repeated the reap of the first
 * faulting child over and over again because the child leaves EXITING.
 *
 *  Reviewer: Chris Peak (proposed the fix)
 *  Risk: M (performance impact: waitmulti() will do an additional loop)
 *  Benefit or PTS #: fix deadlock between waitmulti() and PARACORE
 *  Testing: special test cases
 *  Module(s): tnc/rtask_server.c
 *
 * Revision 1.14  1994/06/08  14:32:15  cfj
 * Correct a merge problem in dpvpsop_rforkmulti_long() which
 * caused it to return with an error incorrectly.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.13  1994/06/02  22:29:23  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.12  1994/05/25  03:52:14  mag
 *  Reviewer: Charlie Johnson
 *  Risk: Low
 *  Benefit or PTS #: 9586
 *  Testing: EATS
 *  Module(s): server/tnc/rtask_server.c
 *
 * Revision 1.11  1994/05/09  04:01:23  yazz
 * Merge R1.2 revision 1.9.4.1 into main stem.
 *
 * Revision 1.10  1994/03/14  02:06:02  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.9.4.1  1994/04/27  22:05:01  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/tnc/rtask_server.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.9  1993/07/14  18:33:42  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  20:46:21  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/06  19:23:32  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:46:14  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/04/16  16:39:29  cfj
 * Merge with T9.5.
 *
 * Revision 1.5.6.1  1993/04/16  15:42:51  cfj
 * Fix a port leak and vm leak.
 *
 * Revision 1.6  1993/04/03  03:09:07  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.2.2.1.2.1  1992/12/16  06:02:36  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.5  1992/12/14  19:27:41  cfj
 * Added the passing of the nodelist and applinfo which got dropped in the merge.
 *
 * Revision 1.4  1992/12/11  03:02:04  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:48:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.2.2.1  1992/11/10  01:47:14  cfj
 * Put into NX branch.
 *
 * Revision 1.2  1992/11/06  20:31:42  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1  1992/11/05  22:46:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.2  1993/04/22  16:39:31  roman
 * [Bug 225] Leak of a page of virtual memory on rforkmulti() with a
 * 	long enough node list.
 * [Bug 225] Port leak of parent's task port on rfork and rforkmulti.
 *
 * Revision 3.1  92/11/13  10:35:29  chrisp
 * [Bug #102] In error cases for migrate() and rexec(), return correct
 * 	receive rights to proc, cred and vproc ports taking account
 * 	of potential renames.
 * 
 * Revision 3.0  92/11/02  11:23:01  roman
 * New file that provides the server interface for server-to-server
 * MiG calls and calls the routines in rtask_svr_vproc.c.
 * 
 */


#include <sys/errno.h>
#include <sys/syscall.h>
#include <tnc/rtask.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/syscall.h>
#include <uxkern/proc_to_task.h>


kern_return_t
svr_rfork(
	mach_port_t	server_port,		/* server-to-server port */
	int		*rvalp,			/* OUT */
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		pp_task,		/* parent task */
	task_t		*ch_taskp,		/* OUT */
	thread_t	*ch_threadp,		/* OUT */
	mach_port_t	*ch_vproc_portp,	/* OUT */
	pid_t		*ch_pidp,		/* OUT */
	thread_state_t	ch_state,		/* state of parent */
	unsigned int	ch_state_count,
	mach_port_t	vproc_port_name,	/* name within emulator */
	mach_port_t	cred_port_name,		/* name within emulator */
	struct rf_data	*rf_data,		/* inherited data */
	struct mmap_struct mmap_structs[],	/* memory mapped file data */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname		/* login name string */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name,		/* exec program name */
	unsigned int	exec_prg_name_len	/* size of above file name */
#endif /* PARACORE */
)
{
	kern_return_t	ret;

	ret = start_tncserver_op(server_port, SYS_rfork);
	if (ret != KERN_SUCCESS)
		return(ret);

	*rvalp = rfork_server(pp_vproc_port, pgrp_vproc_port,
			      rdir_port, cdir_port,
			      pp_task,
			      ch_taskp,
			      ch_threadp,
			      ch_vproc_portp,
			      ch_pidp,
			      ch_state, ch_state_count,
			      vproc_port_name, cred_port_name,
			      rf_data,
			      mmap_structs, mmap_struct_array_size,
			      mmap_pagers, mmap_array_size,
			      command_name, logname
#ifdef NX
,                             applinfo,
                              nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
,			      exec_rdir_port, exec_cdir_port,
			      exec_prg_name
#endif /* PARACORE */
			      );

	(void) mach_port_deallocate(mach_task_self(), (mach_port_t) pp_task);

	end_tncserver_op(*rvalp);
	return(KERN_SUCCESS);
}

kern_return_t
svr_migrate(
	mach_port_t	server_port,		/* server-to-server port */
	int		*rvalp,			/* OUT */
	pid_t		cur_pid,		/* pid of migrating process */
	int		is_pgrpleader,		/* proc is pgrp leader */
	mach_port_t	cur_vproc_port,		/* proc's vproc port (Rcv rt) */
	mach_port_t	vproc_port_name,
	mach_port_t	cur_proc_port,		/* proc's proc port (Rcv rt) */
						/* bootstrap pt needs no name */
	mach_port_t	cur_cred_port,		/* proc's cred port (Rcv rt) */
	mach_port_t	cred_port_name,
	int		cur_cred_cache[],	/* proc's cred cache */
	unsigned int	cur_cred_cache_size,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	fpp_vproc_port,		/* foster parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	sess_vproc_port,	/* session leader */
	pid_t		child_pid[],		/* pids of children */
	unsigned int	child_pid_cnt,
	mach_port_t	child_port[],		/* ports of childrens' vprocs */
	unsigned int	child_port_cnt,
	int		child_stat[],		/* stat (ZOMBIE or STOP) ... */
	unsigned int	child_stat_cnt,		/* ... of childrens' vprocs */
	pid_t		foster_child_pid[],	/* pids of children */
	unsigned int	foster_child_pid_cnt,
	mach_port_t	foster_child_port[],	/* ports of foster children */
	unsigned int	foster_child_port_cnt,
	pid_t		pgrp_member_pid[],	/* pids of pgrp members */
	unsigned int	pgrp_member_pid_cnt,
	mach_port_t	pgrp_member_port[],	/* ports of pgrp members' ... */
	unsigned int	pgrp_member_port_cnt,	/* ... vprocs */
	pid_t		sess_member_pid[],	/* pids of session members */
	unsigned int	sess_member_pid_cnt,
	mach_port_t	sess_member_port[],	/* ports of session ... */
	unsigned int	sess_member_port_cnt,	/* ... members' vprocs */
	int		pgrp_jobc,		/* job control count */
	int		cttynode,		/* node of controlling tty */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		old_task,		/* actual task port and... */
	task_t		old_task_name,		/* old emulator's name for it */
	thread_state_t	cur_state,		/* state of thread */
	unsigned int	cur_state_count,
	struct mi_data	*mi_data,		/* proc and u struct fields */
	struct mmap_struct mmap_structs[],	/* mmap areas of proc */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname,		/* login name string */
	mach_port_t	*return_vproc_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_cred_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_proc_portp	/* Rcv rt returned if error */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name,		/* exec program name */
	unsigned int	exec_prg_name_len	/* size of above file name */
#endif /* PARACORE */
)
{
	kern_return_t	ret;

	*return_vproc_portp = MACH_PORT_NULL;
	*return_proc_portp = MACH_PORT_NULL;
	*return_cred_portp = MACH_PORT_NULL;

	ret = start_tncserver_op(server_port, SYS_migrate);
	if (ret != KERN_SUCCESS) {
		struct vproc	*v;

		*rvalp = EINVAL;
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */

		/*
		 * Fool around with the vproc port to arrange to send the
		 * receive right back. Note that we leave a dangling
		 * reference to the vproc around (we do not do the final
		 * VPROC_RELEASE()). We count on this being done on the
		 * client (remote) node later.
		 */
		v = LOCATE_VPROC_PID(cur_pid);
		if (v == NULL)
			panic("svr_migrate: cannot find vproc");
		tnc_install_vproc_port(v,
				       vproc_to_port_lookup(v),
				       INSTALL_RECEIVE_RT);
		tnc_bestow_vproc_port(v, BESTOW_RECEIVE_RT);
		*return_vproc_portp = vproc_to_port_lookup(v);
		return(KERN_SUCCESS);
	}

	*rvalp = migrate_server(cur_pid,
			        is_pgrpleader,
			        cur_vproc_port, vproc_port_name,
			        cur_proc_port, 
			        cur_cred_port, cred_port_name,
			        cur_cred_cache, cur_cred_cache_size,
			        pp_vproc_port,
			        fpp_vproc_port,
			        pgrp_vproc_port,
			        sess_vproc_port,
			        child_pid, child_pid_cnt,
			        child_port, child_port_cnt,
			        child_stat, child_stat_cnt,
			        foster_child_pid, foster_child_pid_cnt,
			        foster_child_port, foster_child_port_cnt,
			        pgrp_member_pid, pgrp_member_pid_cnt,
			        pgrp_member_port, pgrp_member_port_cnt,
			        sess_member_pid, sess_member_pid_cnt,
			        sess_member_port, sess_member_port_cnt,
			        pgrp_jobc,
			        cttynode,
			        rdir_port, cdir_port,
			        old_task, old_task_name,
			        cur_state, cur_state_count,
			        mi_data,
			        mmap_structs, mmap_struct_array_size,
			        mmap_pagers, mmap_array_size,
			        command_name, logname
#ifdef NX
,                               applinfo,
                                nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
,				exec_rdir_port, exec_cdir_port,
				exec_prg_name
#endif /* PARACORE */
				);
	if (*rvalp != ESUCCESS) {
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */
		/*
		 * Return the vproc port right, which probably has been
		 * renamed by this time so cur_vproc_port can't be quoted.
		 */
		*return_vproc_portp = vproc_to_port_lookup(VPROCPTR(cur_pid));
	}

	end_tncserver_op(*rvalp);
	return(KERN_SUCCESS);
}

kern_return_t
svr_migrate_long(
	mach_port_t	server_port,		/* server-to-server port */
	int		*rvalp,			/* OUT */
	pid_t		cur_pid,		/* pid of migrating process */
	int		is_pgrpleader,		/* proc is pgrp leader */
	mach_port_t	cur_vproc_port,		/* proc's vproc port (Rcv rt) */
	mach_port_t	vproc_port_name,
	mach_port_t	cur_proc_port,		/* proc's proc port (Rcv rt) */
						/* bootstrap pt needs no name */
	mach_port_t	cur_cred_port,		/* proc's cred port (Rcv rt) */
	mach_port_t	cred_port_name,
	int		cur_cred_cache[],	/* proc's cred cache */
	unsigned int	cur_cred_cache_size,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	fpp_vproc_port,		/* foster parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	sess_vproc_port,	/* session leader */
	pid_t		child_pid[],		/* pids of children */
	unsigned int	child_pid_cnt,
	mach_port_t	child_port[],		/* ports of childrens' vprocs */
	unsigned int	child_port_cnt,
	int		child_stat[],		/* stat (ZOMBIE or STOP) ... */
	unsigned int	child_stat_cnt,		/* ... of childrens' vprocs */
	pid_t		foster_child_pid[],	/* pids of children */
	unsigned int	foster_child_pid_cnt,
	mach_port_t	foster_child_port[],	/* ports of foster children */
	unsigned int	foster_child_port_cnt,
	pid_t		pgrp_member_pid[],	/* pids of pgrp members */
	unsigned int	pgrp_member_pid_cnt,
	mach_port_t	pgrp_member_port[],	/* ports of pgrp members' ... */
	unsigned int	pgrp_member_port_cnt,	/* ... vprocs */
	pid_t		sess_member_pid[],	/* pids of session members */
	unsigned int	sess_member_pid_cnt,
	mach_port_t	sess_member_port[],	/* ports of session ... */
	unsigned int	sess_member_port_cnt,	/* ... members' vprocs */
	int		pgrp_jobc,		/* job control count */
	int		cttynode,		/* node of controlling tty */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		old_task,		/* actual task port and... */
	task_t		old_task_name,		/* old emulator's name for it */
	thread_state_t	cur_state,		/* state of thread */
	unsigned int	cur_state_count,
	struct mi_data	*mi_data,		/* proc and u struct fields */
	struct mmap_struct mmap_structs[],	/* mmap areas of proc */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname,		/* login name string */
	mach_port_t	*return_vproc_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_cred_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_proc_portp	/* Rcv rt returned if error */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name,		/* exec program name */
	unsigned int	exec_prg_name_len	/* size of above file name */
#endif /* PARACORE */
)
{
	kern_return_t	ret;

	*return_vproc_portp = MACH_PORT_NULL;
	*return_proc_portp = MACH_PORT_NULL;
	*return_cred_portp = MACH_PORT_NULL;

	ret = start_tncserver_op(server_port, SYS_migrate);
	if (ret != KERN_SUCCESS) {
		struct vproc	*v;

		*rvalp = EINVAL;
		ret = KERN_SUCCESS;
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */

		/*
		 * Fool around with the vproc port to arrange to send the
		 * receive right back. Note that we leave a dangling
		 * reference to the vproc around (we do not do the final
		 * VPROC_RELEASE()). We count on this being done on the
		 * client (remote) node later.
		 */
		v = LOCATE_VPROC_PID(cur_pid);
		if (v == NULL)
			panic("svr_migrate_long: cannot find vproc");
		tnc_install_vproc_port(v,
				       vproc_to_port_lookup(v),
				       INSTALL_RECEIVE_RT);
		tnc_bestow_vproc_port(v, BESTOW_RECEIVE_RT);
		*return_vproc_portp = vproc_to_port_lookup(v);
		goto out;
	}

	*rvalp = migrate_server(cur_pid,
			        is_pgrpleader,
			        cur_vproc_port, vproc_port_name,
			        cur_proc_port, 
			        cur_cred_port, cred_port_name,
			        cur_cred_cache, cur_cred_cache_size,
			        pp_vproc_port,
			        fpp_vproc_port,
			        pgrp_vproc_port,
			        sess_vproc_port,
			        child_pid, child_pid_cnt,
			        child_port, child_port_cnt,
			        child_stat, child_stat_cnt,
			        foster_child_pid, foster_child_pid_cnt,
			        foster_child_port, foster_child_port_cnt,
			        pgrp_member_pid, pgrp_member_pid_cnt,
			        pgrp_member_port, pgrp_member_port_cnt,
			        sess_member_pid, sess_member_pid_cnt,
			        sess_member_port, sess_member_port_cnt,
			        pgrp_jobc,
			        cttynode,
			        rdir_port, cdir_port,
			        old_task, old_task_name,
			        cur_state, cur_state_count,
			        mi_data,
			        mmap_structs, mmap_struct_array_size,
			        mmap_pagers, mmap_array_size,
			        command_name, logname
#ifdef NX
,                               applinfo,
                                nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
,				exec_rdir_port, exec_cdir_port,
				exec_prg_name
#endif /* PARACORE */
				);
	if (*rvalp != ESUCCESS) {
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */
		/*
		 * Return the vproc port right, which probably has been
		 * renamed by this time so cur_vproc_port can't be quoted.
		 */
		*return_vproc_portp = vproc_to_port_lookup(VPROCPTR(cur_pid));
	}

out:
	if (cur_cred_cache_size != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) cur_cred_cache,
				     cur_cred_cache_size*sizeof(int));
	if (child_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) child_pid,
				     child_pid_cnt*sizeof(pid_t));
	if (child_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) child_port,
				     child_port_cnt*sizeof(mach_port_t));
	if (child_stat_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) child_stat,
				     child_stat_cnt*sizeof(pid_t));
	if (foster_child_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) foster_child_pid,
				     foster_child_pid_cnt*sizeof(pid_t));
	if (foster_child_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) foster_child_port,
				     foster_child_port_cnt*sizeof(mach_port_t));
	if (pgrp_member_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) pgrp_member_pid,
				     pgrp_member_pid_cnt*sizeof(pid_t));
	if (pgrp_member_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) pgrp_member_port,
				     pgrp_member_port_cnt*sizeof(mach_port_t));
	if (sess_member_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) sess_member_pid,
				     sess_member_pid_cnt*sizeof(pid_t));
	if (sess_member_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) sess_member_port,
				     sess_member_port_cnt*sizeof(mach_port_t));
	
	end_tncserver_op(*rvalp);
	return(ret);
}

kern_return_t
svr_rexecve(
	mach_port_t	server_port,		/* server-to-server port */
	int		*rvalp,			/* OUT */
	char		*fname,			/* file we are exec-ing */
	unsigned int	fname_count,		/* size of above file name */
	vm_offset_t	arg_addr,		/* parameters for later use */
	vm_size_t	arg_size,
	int		arg_count,
	int		env_count,
	unsigned int	char_count,
	pid_t		cur_pid,		/* pid of migrating process */
	int		is_pgrpleader,		/* proc is pgrp leader */
	mach_port_t	cur_vproc_port,		/* process vproc port */
	mach_port_t	vproc_port_name,
	mach_port_t	cur_proc_port,		/* process port */
	mach_port_t	cur_cred_port,		/* process cred port */
	mach_port_t	cred_port_name,
	int		cur_cred_cache[],	/* proc's cred cache */
	unsigned int	cur_cred_cache_size,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	fpp_vproc_port,		/* foster parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	sess_vproc_port,	/* session leader */
	pid_t		child_pid[],		/* pids of children */
	unsigned int	child_pid_cnt,
	mach_port_t	child_port[],		/* ports of childrens' vprocs */
	unsigned int	child_port_cnt,
	int		child_stat[],		/* stat (ZOMBIE or STOP) ... */
	unsigned int	child_stat_cnt,		/* ... of childrens' vprocs */
	pid_t		foster_child_pid[],	/* pids of children */
	unsigned int	foster_child_pid_cnt,
	mach_port_t	foster_child_port[],	/* ports of foster children */
	unsigned int	foster_child_port_cnt,
	pid_t		pgrp_member_pid[],	/* pids of pgrp members */
	unsigned int	pgrp_member_pid_cnt,
	mach_port_t	pgrp_member_port[],	/* ports of pgrp members' ... */
	unsigned int	pgrp_member_port_cnt,	/* ... vprocs */
	pid_t		sess_member_pid[],	/* pids of session members */
	unsigned int	sess_member_pid_cnt,
	mach_port_t	sess_member_port[],	/* ports of session ... */
	unsigned int	sess_member_port_cnt,	/* ... members' vprocs */
	int		pgrp_jobc,		/* job control count */
	int		cttynode,		/* node of controlling tty */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		old_task,
	task_t		old_task_name,		/* just a name, not a right */
	thread_state_t	cur_state,		/* state of thread */
	unsigned int	cur_state_count,
	struct re_data	*re_data,		/* proc and u struct fields */
	struct mmap_struct mmap_structs[],	/* mmap areas of proc */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*logname,		/* login name string */
	mach_port_t	*return_vproc_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_cred_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_proc_portp	/* Rcv rt returned if error */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
)
{
	kern_return_t	ret;

	*return_vproc_portp = MACH_PORT_NULL;
	*return_proc_portp = MACH_PORT_NULL;
	*return_cred_portp = MACH_PORT_NULL;

	ret = start_tncserver_op(server_port, SYS_rexecve);
	if (ret != KERN_SUCCESS) {
		struct vproc	*v;

		*rvalp = EINVAL;
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */

		/*
		 * Fool around with the vproc port to arrange to send the
		 * receive right back. Note that we leave a dangling
		 * reference to the vproc around (we do not do the final
		 * VPROC_RELEASE()). We count on this being done on the
		 * client (remote) node later.
		 */
		v = LOCATE_VPROC_PID(cur_pid);
		if (v == NULL)
			panic("svr_rexecve: cannot find vproc");
		tnc_install_vproc_port(v,
				       vproc_to_port_lookup(v),
				       INSTALL_RECEIVE_RT);
		tnc_bestow_vproc_port(v, BESTOW_RECEIVE_RT);
		*return_vproc_portp = vproc_to_port_lookup(v);
		return(KERN_SUCCESS);
	}

	*rvalp = rexecve_server(fname, fname_count,
			        arg_addr, arg_size, arg_count,
			        env_count, char_count,
			        cur_pid,
			        is_pgrpleader,
			        cur_vproc_port, vproc_port_name,
			        cur_proc_port,
			        cur_cred_port, cred_port_name,
			        cur_cred_cache, cur_cred_cache_size,
			        pp_vproc_port,
			        fpp_vproc_port,
			        pgrp_vproc_port,
			        sess_vproc_port,
			        child_pid, child_pid_cnt,
			        child_port, child_port_cnt,
			        child_stat, child_stat_cnt,
			        foster_child_pid, foster_child_pid_cnt,
			        foster_child_port, foster_child_port_cnt,
			        pgrp_member_pid, pgrp_member_pid_cnt,
			        pgrp_member_port, pgrp_member_port_cnt,
			        sess_member_pid, sess_member_pid_cnt,
			        sess_member_port, sess_member_port_cnt,
			        pgrp_jobc,
			        cttynode,
			        rdir_port, cdir_port,
			        old_task, old_task_name,
			        cur_state, cur_state_count,
			        re_data,
			        mmap_structs, mmap_struct_array_size,
			        mmap_pagers, mmap_array_size,
			        logname
#ifdef NX
,                               applinfo,
                                nodelist, nodelistcnt
#endif /* NX */
				); 
	if (*rvalp != ESUCCESS) {
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */
		/*
		 * Return the vproc port right, which probably has been
		 * renamed by this time so cur_vproc_port can't be quoted.
		 */
		*return_vproc_portp = vproc_to_port_lookup(VPROCPTR(cur_pid));
	}

	end_tncserver_op(*rvalp);
	return(KERN_SUCCESS);
}

kern_return_t
svr_rexecve_long(
	mach_port_t	server_port,		/* server-to-server port */
	int		*rvalp,			/* OUT */
	char		*fname,			/* file we are exec-ing */
	unsigned int	fname_count,		/* size of above file name */
	vm_offset_t	arg_addr,		/* parameters for later use */
	vm_size_t	arg_size,
	int		arg_count,
	int		env_count,
	unsigned int	char_count,
	pid_t		cur_pid,		/* pid of migrating process */
	int		is_pgrpleader,		/* proc is pgrp leader */
	mach_port_t	cur_vproc_port,		/* process vproc port */
	mach_port_t	vproc_port_name,
	mach_port_t	cur_proc_port,		/* process port */
	mach_port_t	cur_cred_port,		/* process cred port */
	mach_port_t	cred_port_name,
	int		cur_cred_cache[],	/* proc's cred cache */
	unsigned int	cur_cred_cache_size,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	fpp_vproc_port,		/* foster parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	sess_vproc_port,	/* session leader */
	pid_t		child_pid[],		/* pids of children */
	unsigned int	child_pid_cnt,
	mach_port_t	child_port[],		/* ports of childrens' vprocs */
	unsigned int	child_port_cnt,
	int		child_stat[],		/* stat (ZOMBIE or STOP) ... */
	unsigned int	child_stat_cnt,		/* ... of childrens' vprocs */
	pid_t		foster_child_pid[],	/* pids of children */
	unsigned int	foster_child_pid_cnt,
	mach_port_t	foster_child_port[],	/* ports of foster children */
	unsigned int	foster_child_port_cnt,
	pid_t		pgrp_member_pid[],	/* pids of pgrp members */
	unsigned int	pgrp_member_pid_cnt,
	mach_port_t	pgrp_member_port[],	/* ports of pgrp members' ... */
	unsigned int	pgrp_member_port_cnt,	/* ... vprocs */
	pid_t		sess_member_pid[],	/* pids of session members */
	unsigned int	sess_member_pid_cnt,
	mach_port_t	sess_member_port[],	/* ports of session ... */
	unsigned int	sess_member_port_cnt,	/* ... members' vprocs */
	int		pgrp_jobc,		/* job control count */
	int		cttynode,		/* node of controlling tty */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		old_task,
	task_t		old_task_name,		/* just a name, not a right */
	thread_state_t	cur_state,		/* state of thread */
	unsigned int	cur_state_count,
	struct re_data	*re_data,		/* proc and u struct fields */
	struct mmap_struct mmap_structs[],	/* mmap areas of proc */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*logname,		/* login name string */
	mach_port_t	*return_vproc_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_cred_portp,	/* Rcv rt returned if error */
	mach_port_t	*return_proc_portp	/* Rcv rt returned if error */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
)
{
	kern_return_t	ret;

	*return_vproc_portp = MACH_PORT_NULL;
	*return_proc_portp = MACH_PORT_NULL;
	*return_cred_portp = MACH_PORT_NULL;

	ret = start_tncserver_op(server_port, SYS_rexecve);
	if (ret != KERN_SUCCESS) {
		struct vproc	*v;

		*rvalp = EINVAL;
		ret = KERN_SUCCESS;
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */

		/*
		 * Fool around with the vproc port to arrange to send the
		 * receive right back. Note that we leave a dangling
		 * reference to the vproc around (we do not do the final
		 * VPROC_RELEASE()). We count on this being done on the
		 * client (remote) node later.
		 */
		v = LOCATE_VPROC_PID(cur_pid);
		if (v == NULL)
			panic("svr_rexecve_long: cannot find vproc");
		tnc_install_vproc_port(v,
				       vproc_to_port_lookup(v),
				       INSTALL_RECEIVE_RT);
		tnc_bestow_vproc_port(v, BESTOW_RECEIVE_RT);
		*return_vproc_portp = vproc_to_port_lookup(v);
		goto out;
	}

	*rvalp = rexecve_server(fname, fname_count,
			        arg_addr, arg_size, arg_count,
			        env_count, char_count,
			        cur_pid,
			        is_pgrpleader,
			        cur_vproc_port, vproc_port_name,
			        cur_proc_port,
			        cur_cred_port, cred_port_name,
			        cur_cred_cache, cur_cred_cache_size,
			        pp_vproc_port,
			        fpp_vproc_port,
			        pgrp_vproc_port,
			        sess_vproc_port,
			        child_pid, child_pid_cnt,
			        child_port, child_port_cnt,
			        child_stat, child_stat_cnt,
			        foster_child_pid, foster_child_pid_cnt,
			        foster_child_port, foster_child_port_cnt,
			        pgrp_member_pid, pgrp_member_pid_cnt,
			        pgrp_member_port, pgrp_member_port_cnt,
			        sess_member_pid, sess_member_pid_cnt,
			        sess_member_port, sess_member_port_cnt,
			        pgrp_jobc,
			        cttynode,
			        rdir_port, cdir_port,
			        old_task, old_task_name,
			        cur_state, cur_state_count,
			        re_data,
			        mmap_structs, mmap_struct_array_size,
			        mmap_pagers, mmap_array_size,
			        logname
#ifdef NX
,                               applinfo,
                                nodelist, nodelistcnt
#endif /* NX */
				); 
	if (*rvalp != ESUCCESS) {
		*return_proc_portp = cur_proc_port;	/* return this Rcv rt */
		*return_cred_portp = cur_cred_port;	/* return this Rcv rt */
		/*
		 * Return the vproc port right, which probably has been
		 * renamed by this time so cur_vproc_port can't be quoted.
		 */
		*return_vproc_portp = vproc_to_port_lookup(VPROCPTR(cur_pid));
	}


out:
	if (cur_cred_cache_size != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) cur_cred_cache,
				     cur_cred_cache_size*sizeof(int));
	if (child_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) child_pid,
				     child_pid_cnt*sizeof(pid_t));
	if (child_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) child_port,
				     child_port_cnt*sizeof(mach_port_t));
	if (child_stat_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) child_stat,
				     child_stat_cnt*sizeof(pid_t));
	if (foster_child_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) foster_child_pid,
				     foster_child_pid_cnt*sizeof(pid_t));
	if (foster_child_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) foster_child_port,
				     foster_child_port_cnt*sizeof(mach_port_t));
	if (pgrp_member_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) pgrp_member_pid,
				     pgrp_member_pid_cnt*sizeof(pid_t));
	if (pgrp_member_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) pgrp_member_port,
				     pgrp_member_port_cnt*sizeof(mach_port_t));
	if (sess_member_pid_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) sess_member_pid,
				     sess_member_pid_cnt*sizeof(pid_t));
	if (sess_member_port_cnt != 0)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) sess_member_port,
				     sess_member_port_cnt*sizeof(mach_port_t));
	
	end_tncserver_op(*rvalp);
	return(ret);
}

int
dpvpsop_rforkmulti(
	node_t		node,
	boolean_t	forkfamily,
	int		np_array[],		/* additional nodes */
	int		np_array_len,
	int		np_idx,		/* array index of local node */
	int		base_idx,		/* base index for subtree */
	int		proc_count,		/* total number of nodes */
	int		rval_array[],		/* OUT */
	int		*rval_count,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		pp_task,		/* parent task */
	mach_port_t	ch_vproc_port_array[],	/* OUT */
	int		*ch_vproc_count,
	pid_t		ch_pid_array[],		/* OUT */
	int		*ch_pid_count,
	mach_port_t	file_port_name[],	/* file port names */
	int		file_port_name_count,
	mach_port_t	file_port_right[],	/* file port rights */
	int		file_port_right_count,
	thread_state_t	ch_state,		/* state of parent */
	int		ch_state_count,
	mach_port_t	vproc_port_name,	/* name within emulator */
	mach_port_t	cred_port_name,		/* name within emulator */
	struct rf_data	*rf_data,		/* inherited data */
	struct mmap_struct mmap_structs[],	/* memory mapped file data */
	int		mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	int		mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname,		/* login name string */
#ifdef NX
        APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt,            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name,		/* exec program name */
	unsigned int	exec_prg_name_len,	/* size of above file name */
#endif /* PARACORE */
	msg_handle_t	*h
)
{
	int		error = ESUCCESS;

	error = rforkmulti_server(forkfamily,
				  np_array, np_array_len,
				  np_idx,
				  base_idx,
				  proc_count,
				  rval_array, rval_count,
				  pp_vproc_port,
				  pgrp_vproc_port,
				  rdir_port, cdir_port,
				  pp_task,
				  ch_vproc_port_array, ch_vproc_count,
				  ch_pid_array,	ch_pid_count,
				  file_port_name, file_port_name_count,
				  file_port_right, file_port_right_count,
				  ch_state, ch_state_count,
				  vproc_port_name, cred_port_name,
				  rf_data,
				  mmap_structs, mmap_struct_array_size,
				  mmap_pagers, mmap_array_size,
				  command_name, logname
#ifdef NX
,                               applinfo,
                                nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
,				   exec_rdir_port, exec_cdir_port,
				   exec_prg_name, exec_prg_name_len
#endif /* PARACORE */
				  );

	(void) mach_port_deallocate(mach_task_self(), (mach_port_t) pp_task);

	return(error);
}

int
dpvpsop_rforkmulti_long(
	node_t		node,
	boolean_t	forkfamily,
	int		np_array[],		/* additional nodes */
	int		np_array_len,
	int		np_idx,			/* array index of local node */
	int		base_idx,		/* base index for subtree */
	int		proc_count,		/* total number of node/procs */
	int		*rval_array[],		/* OUT */
	int		*rval_count,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		pp_task,		/* parent task */
	mach_port_t	ch_vproc_port_array[],	/* OUT */
	int		*ch_vproc_count,
	pid_t		*ch_pid_array[],	/* OUT */
	int		*ch_pid_count,
	mach_port_t	file_port_name[],	/* file port names */
	int		file_port_name_count,
	mach_port_t	file_port_right[],	/* file port rights */
	int		file_port_right_count,
	thread_state_t	ch_state,		/* state of parent */
	int		ch_state_count,
	mach_port_t	vproc_port_name,	/* name within emulator */
	mach_port_t	cred_port_name,		/* name within emulator */
	struct rf_data	*rf_data,		/* inherited data */
	struct mmap_struct mmap_structs[],	/* memory mapped file data */
	int		mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	int		mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname,		/* login name string */
#ifdef NX
        APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt,            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name,		/* exec program name */
	unsigned int	exec_prg_name_len,	/* size of above file name */
#endif /* PARACORE */
	msg_handle_t	*h
)
{
	kern_return_t	ret;
	int		error = ESUCCESS;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*rval_array = 0;
	*rval_count = 0;
	*ch_vproc_port_array = 0;
	*ch_vproc_count = 0;
	*ch_pid_array = 0;
	*ch_pid_count = 0;


	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) rval_array,
			  np_array_len * sizeof(int),
			  TRUE);
	if (ret != KERN_SUCCESS)
		goto out;
	*rval_count = np_array_len;

	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) ch_vproc_port_array,
			  np_array_len * sizeof(mach_port_t),
			  TRUE);
	if (ret != KERN_SUCCESS)
		goto out;
	*ch_vproc_count = np_array_len;

	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) ch_pid_array,
			  np_array_len * sizeof(pid_t),
			  TRUE);
	if (ret != KERN_SUCCESS)
		goto out;
	*ch_pid_count = np_array_len;

	error = rforkmulti_server(forkfamily,
				  np_array, np_array_len,
				  np_idx,
				  base_idx,
				  proc_count,
				  *rval_array, rval_count,
				  pp_vproc_port,
				  pgrp_vproc_port,
				  rdir_port, cdir_port,
				  pp_task,
				  *ch_vproc_port_array, ch_vproc_count,
				  *ch_pid_array, ch_pid_count,
				  file_port_name, file_port_name_count,
				  file_port_right, file_port_right_count,
				  ch_state, ch_state_count,
				  vproc_port_name, cred_port_name,
				  rf_data,
				  mmap_structs, mmap_struct_array_size,
				  mmap_pagers, mmap_array_size,
				  command_name, logname
#ifdef NX
,                               applinfo,
                                nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
,				   exec_rdir_port, exec_cdir_port,
				   exec_prg_name, exec_prg_name_len
#endif /* PARACORE */
				  );

out:
	if (ret != KERN_SUCCESS) {
		if (*rval_array != NULL) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) *rval_array,
					     np_array_len * sizeof(int));
			*rval_array = NULL;
			*rval_count = 0;
		}
		if (*ch_vproc_port_array != NULL) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t)*ch_vproc_port_array,
					     np_array_len * sizeof(mach_port_t));
			*ch_vproc_port_array = NULL;
			*ch_vproc_count = 0;
		}
		if (*ch_pid_array != NULL) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) *ch_pid_array,
					     np_array_len * sizeof(pid_t));
			*ch_pid_array = NULL;
			*ch_pid_count = 0;
		}
	}

	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) np_array,
			     np_array_len*sizeof(node_t));

	(void) mach_port_deallocate(mach_task_self(), (mach_port_t) pp_task);

	return(ret);
}


extern boolean_t tnc_rforkmulti_binary_tree;

int
dpvpop_reap_multi(
	struct vproc	*v,		/* vproc being waited for */
	struct vproc	*ve,		/* vproc of elder process */
	pid_t		pgid,		/* conditional pgrp  reqd */
	int		options,	/* determining semantics */
	rstat_t		*rstat[],	/* reap state of procs */
	u_int		*rstat_len,
	rusage_dev_t	*ru,		/* resource usage */
	int		proc_idx,	/* index of this pid in list */
	pid_t		pid_list[],	/* list of pids */
	u_int		num_pids,	/* number of pids in list */
	int		base_idx,
	int		proc_count,	/* total number of children */
	msg_handle_t	*h)		/* message reference handle */
{
	register int	i;
	kern_return_t	ret;
	struct pvproc	*pvp = PVP(v);
	int		error = ESUCCESS;
	msg_handle_t	reply[VP_STACK_ARRAY_SIZE];
	unsigned int	cnt[VP_STACK_ARRAY_SIZE];
	int		subtree_idx[VP_STACK_ARRAY_SIZE];
	int		subtree_base[VP_STACK_ARRAY_SIZE];
	int		subtree_count = VP_STACK_ARRAY_SIZE;
	int		sbase;
	rstat_t		*subtree_rstat;
	u_int		subtree_rstat_len;
	rusage_dev_t	subtree_ru;

	ASSERT(v->vp_pid == pid_list[proc_idx]);

	if (tnc_rforkmulti_binary_tree) {
		subtree_idx[0] = 1;
		cnt[0] = num_pids >> 1;
		subtree_idx[1] = subtree_idx[0] + cnt[0];
		cnt[1] = num_pids - cnt[0] - 1;
		if (cnt[0] == 0)
			subtree_count = 0;
		else if (cnt[1] == 0)
			subtree_count = 1;
		else
			subtree_count = 2;
	} else {
		/*
		 * Find the entries in the pid array that are below this entry
		 * in the spanning tree.
		 */
		error =  get_spanning_tree(proc_count, proc_idx + base_idx,
					   subtree_idx, &subtree_count);
		if (error)
			return(EAGAIN);
	}

	/*
	 * Allocate the reap status return array.
	 */
	*rstat_len = num_pids;
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) rstat, num_pids*sizeof(rstat_t),
			  TRUE);
	if (ret)
		return(EAGAIN);

	if (ve == NULL) {
		/*
		 * Ensure that no further elder reports go higher.
		 */
		VPROC_LOCK_FLAG(v, "dpvpop_reap_multi()");
		pvp->pvp_flag |= PV_HAS_REPORTED;
		VPROC_UNLOCK_FLAG(v, "dpvpop_reap_multi()");
	}

	/*
	 * Perform the asynchronous "send" of the message to all the
	 * processes that are below this entry in the spanning tree.
	 */
	for (i=0; i<subtree_count; i++) {
		struct vproc *w;
		subtree_idx[i] -= base_idx;
		if (tnc_rforkmulti_binary_tree) {
			sbase = subtree_idx[i];
			subtree_base[i] = 0;
		} else {
			tnc_spanning_tree_info(proc_count,
					       subtree_idx[i] + base_idx,
					       &subtree_base[i], &cnt[i]);
			sbase = subtree_base[i] - base_idx;
		}

		w = LOCATE_VPROC_PID(pid_list[subtree_idx[i]]);
		ASSERT(w);
		ret = PVPOP_REAP_MULTI_SEND(w, ve ? v : NULL, pgid, options,
					    &subtree_rstat,
					    &subtree_rstat_len,
					    &subtree_ru,
					    subtree_idx[i]-sbase,
					    &pid_list[sbase], cnt[i],
					    subtree_base[i],
					    proc_count,
					    &reply[i]);
		if (ret != KERN_SUCCESS) {
			cnt[i] = 0;
			VPROC_RELEASE(w, "dpvpop_reap_multi");
		}
	}

	/*
	 * Save the pid of our elder and hold the elder vproc
	 * unless we're the eldest child or this is already done
	 * (as a result of a previous waitmulti(...WNOWAIT)).
	 */
	if (ve != NULL && pvp->pvp_epid != 0) {
		pvp->pvp_epid = ve->vp_pid;
		if (ve != v)
			VPROC_HOLD(ve, "dpvpop_reap_multi(elder)");
	}

	/*
	 * Make the local reap.
	 */
	{
		pid_t	ch_pgid = pgid;
		rstat_t	*rstatp = &(*rstat)[proc_idx];

		bzero(ru, sizeof(rusage_dev_t));

		rstatp->wstat = 0;
		rstatp->state = 0;
		rstatp->pgid = 0;
	repeat:
		switch (PVPOP_REAP(v, &ch_pgid, options,
				   &rstatp->state, &rstatp->wstat, ru)) {
		    case EAGAIN:
#ifndef PARACORE
			goto repeat;		/* Process is exiting now ...
						   ... try again */
						/* This slows down
						   PARACORE extremely. */
#endif /* PARACORE */
		    case ESUCCESS:
		    case ESRCH:
			rstatp->pgid = ch_pgid;
			break;
		    case ECHILD:
		    default:
			break;
		}
	}

	/*
	 * Perform the asynchronous "receive" of replies to the message for 
	 * all the processes that are below this entry in the spanning tree.
	 */
	for (i=0; i<subtree_count; i++) {
		struct vproc	*w;
		if (cnt[i] == 0)
			continue;
		if (tnc_rforkmulti_binary_tree)
			sbase = subtree_idx[i];
		else
			sbase = subtree_base[i] - base_idx;
		w = VPROCPTR(pid_list[subtree_idx[i]]);
		ASSERT(w);
		ret = PVPOP_REAP_MULTI_RECEIVE(w, ve ? v : NULL, pgid, options,
					       &subtree_rstat,
					       &subtree_rstat_len,
					       &subtree_ru,
					       subtree_idx[i]-sbase,
					       &pid_list[sbase], cnt[i],
					       subtree_base[i],
					       proc_count,
					       &reply[i]);
		VPROC_RELEASE(w, "dpvpop_reap_multi");
		if (ret != KERN_SUCCESS)
			continue;

		/*
		 * Add this subtree's accumulated resource usage to
		 * the total.
		 */
		ruadd(ru, &subtree_ru);

		/*
		 * Copy this subtree's reap status into the correct place
		 * in the return array.
		 */
		bcopy(subtree_rstat, &(*rstat)[sbase],
		      subtree_rstat_len*sizeof(rstat_t));
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) subtree_rstat,
				     subtree_rstat_len*sizeof(rstat_t));
	}

	if (ve == NULL) {
		/*
		 * Disable the elder reporting.
		 */
		VPROC_LOCK_FLAG(v, "dpvpop_reap_multi()");
		pvp->pvp_flag &= ~PV_HAS_REPORTED;
		VPROC_UNLOCK_FLAG(v, "dpvpop_reap_multi()");
		if (pvp->pvp_epid != 0 && pvp->pvp_epid != v->vp_pid)
			VPROC_RELEASE(VPROCPTR(pvp->pvp_epid),
				      "dpvpop_reap_multi(elder)"); 
		pvp->pvp_epid = 0;
	}

	return(error);
}
