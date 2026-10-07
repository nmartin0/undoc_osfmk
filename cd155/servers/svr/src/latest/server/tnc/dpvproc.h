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
 * HISTORY
 * $Log: dpvproc.h,v $
 * Revision 1.29  1995/02/01  21:40:05  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.28  1994/11/18  20:43:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.27  1994/08/31  22:47:05  mtm
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
 * Revision 1.25.2.1  1994/08/03  15:51:26  nandy
 * Merged from the main stem
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.26  1994/08/03  15:32:47  nandy
 * Added flag PV_NX_ATH_PGRP
 *
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 10047
 *  Testing: PTS test case
 *  Module(s): bsd/kern_sig.c
 * 	bsd/kern_exit.c
 * 	tnc/dpvproc.h
 * 	tnc/dvp_vpops.c
 * 	nx/nx.c
 *
 * Revision 1.25  1994/06/29  17:01:31  johannes
 * 'struct pvproc': definition of extra 'pvp_core_data' field
 *
 *  Initial check-in of parallel core dumping
 *  Reviewer: stefan, jlitvin
 *  Risk: Medium
 *  Benefit or PTS #: OS support for Postmortem Debugging
 *  Testing: developer tests
 *  Module(s):
 * 	svr/server/conf/MASTER
 * 	svr/server/conf/MASTER.i860
 * 	svr/server/conf/files.i860
 * 	svr/server/paracore/core_types.h
 * 	svr/server/paracore/allocinfo.c
 * 	svr/server/paracore/core.c
 * 	svr/server/paracore/dump.c
 * 	svr/server/paracore/dvp_pvpcore.c
 * 	svr/server/sys/allocinfo.h
 * 	svr/server/sys/core.h
 * 	svr/server/sys/user.h
 * 	svr/server/nx/nx.defs
 * 	svr/server/nx/nx.c
 * 	svr/server/bsd/kern_exit.c
 * 	svr/server/bsd/kern_fork.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/tnc/dpvproc.h
 * 	svr/server/tnc/dvp_init.c
 * 	svr/server/tnc/dvp_pvpops.c
 * 	svr/server/tnc/pvp.ops
 * 	svr/server/uxkern/fsvr_msg.c
 * 	cmds_libs/src/usr/sbin/allocator/alloc.defs
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/Makefile
 * 	cmds_libs/src/usr/include/README.locate
 * 	cmds_libs/src/usr/include/sys/Makefile
 *
 * Revision 1.24  1994/06/17  23:47:35  jlitvin
 * Remove embedded comment character to make lint happier.
 *
 * Revision 1.23  1994/06/02  22:27:50  chrisp
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
 * Revision 1.22  1994/04/28  19:13:52  chrisp
 * tncgen: autugenerated code now derived from *.ops definition files
 * and processed by tncgen and associated gen*.sh scripts. #ifdefs
 * introduced where previously they were ignored.
 *
 *  Reviewer: dleslie, cfj
 *  Risk: M
 *  Benefit or PTS #: 9188
 *  Testing: Builds, builds, builds.
 *  Module(s):
 *      Modified Files:
 *  	dpvproc.h dvp_pvpsops.c dvp_vpops.c rvp_pvpops.c
 *  	rvp_pvpops_server.c rvp_pvpsops.c rvp_pvpsops_server.c
 *  	tnc.defs tnc_mig.c
 *      Added Files:
 *  	gen_client_stubs.sh gen_mig.sh gen_migrate_wrappers.sh
 *  	gen_server_stubs.sh pvp.ops pvps.ops tncgen.mk
 *      Removed Files:
 *  	dpvproc_struct.h makeTNCtables.sh
 *
 * Revision 1.21  1993/10/21  23:33:17  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 3.39  93/09/16  08:29:35  chrisp
 * [SPE 0030] Generic Spanning Trees:
 * 	Contents relocated to dpvproc_struct.h. This file now includes
 * 	that and PVPOP/PVPSOP macros generated from makeTNCtables.sh.
 * 
 * $EndLog$
 */
#ifndef _H_DPVPROC
#define _H_DPVPROC


#include <sys/types.h>
#include <sys/vproc.h>
#include <tnc/tnc_types.h>

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>
#endif /* NX */

#ifdef PARACORE
#include <paracore/core_types.h>
#endif /* PARACORE */

/*
 * These null defines declares that an RPC operation does not expect
 * a reply, or may be performed with asynchronous send and receive.
 */
#define	NO_REPLY
#define	ASYNC
/*
 * These are null defines for keywords used only to inform the RPC
 * stub generation procedure of a parameter requiring special handling;
 * they supplement the IN, OUT and INOUT keywords defined in vproc.h.
 */
#define RPC_VAR_ARRAY
#define RPC_VAR_ARRAY_OUT_OF_LINE
#define RPC_VAR_ARRAY_LENGTH

/*
 * The following are the lock types used by the pvproc structure.
 */
typedef struct mutex pvp_lock_t;
typedef struct mutex pvp_slock_t;
typedef struct mutex pvp_rw_lock_t;
typedef struct pvp_movement_lock {
	struct mutex	 ml_lock;	/* lock for all fields of this ...
					   structure */
	unsigned long	 ml_flag;	/* flag controlling locking */
	struct condition ml_shr_cond;	/* condition used to wake up threads ...
					   waiting on shared lock */
	int		 ml_shr_count;	/* number of shared lockers */
	struct condition ml_excl_cond;	/* condition used to wake up threads
					   waiting on exclusive lock */
	int		 ml_excl_count;	/* number pending exclusive lockers */
	unsigned long	 ml_proc_sequence;  /* proc port message count */
	unsigned long	 ml_vproc_sequence; /* vproc port message count */
	struct condition ml_port_cond;	/* condition used to wake up threads ...
					   waiting for mg_*_port_sequence to ...
					   match the actual port seqno's */
		} pvp_movement_lock_t;

/*
 * The following are the "real" names of the locking routines.
 */
#define pvp_slock_init			mutex_init
#define pvp_slock_lock			mutex_lock
#define pvp_slock_unlock		mutex_unlock

#define pvp_lock_init			mutex_init
#define pvp_lock_lock			mutex_lock
#define pvp_lock_unlock			mutex_unlock

#define pvp_rw_lock_init		mutex_init
#define pvp_rw_lock_lock_excl		mutex_lock
#define pvp_rw_lock_unlock_excl		mutex_unlock
#define pvp_rw_lock_lock_shared		mutex_lock
#define pvp_rw_lock_unlock_shared	mutex_unlock

#define PVP_LOCK_INITIALIZER		MUTEX_INITIALIZER

/*
 * flag values for pvproc ml_flag field
 */
#define ML_EXCL_LOCK_HELD	    0x0001 /* Exclusive lock held */
#define ML_SHR_LOCK_HELD_OR_PENDING 0x0002 /* Shared lock held or awaiting ...
					      someone to give up exclusive ...
					      lock */
#define ML_EXCL_LOCK_PENDING_COUNT  0x0004 /* Exclusive lock is awaiting the ...
                                              completion of pending proc or ...
                                              vproc port operations */

/*
 * This structure is a simple FIFO cache of pid/sctty pairs implemented
 * as a circular buffer.
 */
#define N_PIDSCTTY_CACHE 3
struct pidsctty {
	struct {
		pid_t	  pid;		/* pid */
		boolean_t has_sctty;	/* has this process a controlling tty */
	} cache_array[N_PIDSCTTY_CACHE];
	unsigned int	next_in;	/* FIFO pointer */
	pvp_slock_t	lock;		/* cache lock */
};

/*
 * Number of operations that should be done before spanning trees come into
 * use (for asynchronous operations).
 */
#define SPANNING_TREE_BENEFITS 2

/*
 * Array size that can comfortably fit on the stack (array of longs). Should
 * be at least 13 for spanning trees to be used for large number of operations.
 */
#define VP_STACK_ARRAY_SIZE 20

/*
 * pvproc data structure
 */
struct pvproc {
	unsigned long	pvp_flag;	/* pvproc attributes */
	struct proc	*pvp_pproc;	/* physical process structure */
	struct vproc	*pvp_head_childl; /* first child of parent */
	struct vproc	*pvp_childl;	/* next child of parent */
	struct vproc	*pvp_head_foster_childl; /* first foster child of ...
					   parent */
	struct vproc	*pvp_foster_childl; /* next foster child of parent */
					/* NOTE: foster children are ...
					   orphaned children that will ...
					   contribute their accounting ...
					   stats to this process when they ...
					   are reaped by the init process */
	struct vproc	*pvp_head_pgrpl; /* pointer to first pgrp member */
	struct vproc	*pvp_pgrpl;	/* next pgrp member */
	struct vproc	*pvp_sessionl;	/* ptr to pgrp leaders in session */
	struct pvproc_ops_vector *pvp_ops; /* pvproc operations */
	pid_t		pvp_ppid;	/* parent process id */
	pid_t		pvp_epid;	/* elder process id (if extant) */
	pid_t		pvp_foster_ppid;/* pid of process that will inherit ...
					   accounting stats when this proc ...
					   is reaped (normally == pvp_ppid) */
	pid_t		pvp_sid;	/* session leader id */
	pid_t		pvp_pgid;	/* process group leader id */
	pid_t		pvp_pp_sid;	/* session leader id of parent */
	pid_t		pvp_pp_pgid;	/* process group leader id of parent */
	struct tty	*pvp_cttyp;	/* controlling TTY tty structure ...
					   (valid for session vproc on ...
					   node of the controlling terminal ...
					   only) */
	node_t		pvp_cttynode;	/* controlling TTY node number ...
					   (valid for exec. node of session ...
					   leader only) */
	struct pidsctty	pvp_sctty_cache;/* cache of pid/has_ctty pairs for ...
					   session members on controlling ...
					   node (valid for session vproc on ...
					   node of the controlling terminal ...
					   only) */
	unsigned long	pvp_jobc;	/* vprocs contributing to non-orphan ...
					   status (valid on execution node ...
					   of group leader only) */
	pvp_slock_t	pvp_flag_lock;	/* lock used by ...
					   VPROC_LOCK_FLAG() macros below */
	pvp_lock_t	pvp_generic_lock; /* lock used by ...
					   VPROC_LOCK_EXCL() macros below */
	pvp_lock_t	pvp_foster_list_lock; /* lock used by ...
					   VPROC_LOCK_FOSTER_LIST_EXCL() ...
					   macros below */
	pvp_rw_lock_t	pvp_pgrp_list_lock; /* lock used by ...
					   VPROC_LOCK_PGRP_LIST*() macros ...
					   below */
	pvp_lock_t	pvp_session_list_lock; /* lock used by ...
					   VPROC_LOCK_SESSION_LIST*() macros ...
					   below */
	pvp_movement_lock_t pvp_movement_lock; /* structure to implement ...
					   VP_HOLD_MOVEMENT() and ...
					   VP_START_MOVEMENT() concepts */
	int		pvp_pgrp_ldr_seqno; /* for process group LEADER, the ...
					   number of pgrp signals SENT */
	int		pvp_pgrp_mem_seqno; /* for process group MEMBER, the ...
					   number of pgrp signals RECEIVED */
	pvp_slock_t	pvp_pgrp_mem_seqno_lock;
					/* lock used by the VPROC_LOCK_-
					   PGRP_MEM_SEQNO() macros below */
	sigset_t	pvp_fork_sigset; /* for forking parent, pgrp sigs ...
					   that arrive during fork (they ...
					   MUST be sent to new children) */
	long		pvp_fork_sigmigarg; /* arg to SIGMIGRATE in the above */
#ifdef PARACORE
	pvp_core_data_t	*pvp_core_data;		/* only valid in proxy */
#endif /* PARACORE */
};

/*
 * Shortcut for getting directly from vproc to pvproc structure member.
 */
#define PVP(vp)		((struct pvproc *)((vp)->vp_data))

/*
 * Actual pvproc table
 */
struct pvproc *pvproc, *pvprocNPVPROC;

/*
 * flag values for the pvproc pvp_flag field
 */
#define	PV_SCTTY		0x0001	/* Process has controlling terminal
					   (if defined for the session) */
#define PV_PGRPLEADER		0x0002	/* Process is a pgrp leader and this
					   is also the execution node */
#define PV_SESSIONLEADER	0x0004	/* Process is a session leader and this
					   is also the execution node */
#define	PV_IS_ORIGIN		0x0008	/* Marks origin node of process */
#define	PV_SZOMB		0x0010	/* Process is in zombie state. Set in
					   child vproc on execution node of
					   parent when child process exits
					   and also on the actual
					   process's execution node*/
#define	PV_SSTOP		0x0020	/* Process is in stopped state. Set in
					   child vproc on execution node of
					   parent when child process becomes
					   stopped and also on the actual
					   process's execution node. */
#define PV_IS_LOCAL		0x0040	/* This is execution node of process */
#define	PV_CTTY_NODE		0x0080	/* Session leader vproc on the node of
					   the session's controlling terminal */
#define	PV_HAS_PORT_RIGHT	0x0100	/* Server has a send or receive right
					   to the vproc port */
#define PV_HAS_REMOTE_RIGHT	0x0200	/* Server either gave away vproc port
					   rights to another node, or
					   initially received its vproc port
					   rights from another node */
#define PV_EXITING		0x0400	/* Process is engaged in exiting, an
					   operation that must occur only
					   once */
#define PV_HAS_REPORTED		0x0800	/* Process has an elder to which a
					   change of state has been reported */
#ifdef NX
#define PV_NX_APPLICATION       0x4000  /* This is an NX application. */
#define PV_NX_PARTITIONED       0x8000  /* This process runs in a partition. */
#define PV_NX_NO_SIGCHLD        0x1000  /* Do not send SIGCHLD to parent.    */
#define PV_NX_GANG_STOP         0x2000  /* Mark the process as gang stopped. */
#define PV_NX_ATH_PGRP          0x10000  /* Mark the process as gang stopped. */
#endif /* NX */

/*
 * Flag values modifying semantics of PVPOP_ADJUST_JOB_CONTROL_COUNT() and
 * PVPOP_REASSIGN_CHILD()
 */
#define	PV_EXIT			0x0001	/* Requests special processing
					   associated with process exit */
#define	PV_ADD			0x0002	/* Add to pgrp job count */
#define	PV_SUBTRACT		0x0004	/* Subtract from pgrp job count */
#define	PV_FOSTER_ONLY		0x0008	/* Just reassign to new foster parent */

/*
 * Routine to initialize a pvproc structure.
 */
void dpvproc_struct_init(struct vproc *vp);

/*
 * Flags used for tnc_install_vproc_port() and bestow_vproc_port()
 */
#define	INSTALL_RECEIVE_RT	0x0001	/* Receive right has arrived here */
#define	BESTOW_RECEIVE_RT	0x0002	/* Receive right being given away */

/*
 * Defines and macros for use with process table entries
 */
extern node_t	root_fs_node;		/* origin node of init for OSF/1 AD */
#define init_node	root_fs_node

#define NODESHIFT	16		/* bits to shift for node number */
#define LOCPIDMAX	((1<<NODESHIFT)-1) /* number at which pids recycle */
#define VPROCLOCPID(pid) ((pid)&LOCPIDMAX) /* local pid extracted from pid */
extern	node_t	this_node;		/* local node number */
#define VPROCNODE(pid)	((pid == 1) ? init_node : (pid)>>NODESHIFT)
					/* node number extracted from pid */
#define	VPROC_IS_AT_ORIGIN(v)	(PVP(v)->pvp_flag & PV_IS_ORIGIN)
					/* is v at its origin node ? */

/*
 * Locking macros.
 *
 * The VPROC LOCK, PGRP LIST LOCK, SESSION LOCK, and FOSTER LOCK are locks
 * that are held across RPC calls, and thus the process must be able to
 * block while holding this lock.
 *
 * The FLAG LOCK is used when the underlying system has multi-threaded
 * processes, is operating in a "pre-emptive scheduler" environment,  or
 * is operating in a SMP (Symmetric MultiProcessor) environment.
 *
 * The SCTTY CACHE LOCK (on systems that have a session controlling tty cache)
 * is of the same type as the FLAG LOCK, and locks the session controlling tty
 * cache against simultaneous access by mulitple threads.
 *
 * The locks:
 *	VPROC LOCK	  Generic lock that locks the pvp_pp_pgid and
 *			  pvp_pp_sid fields, the parent-child-sibling list,
 *			  and prevents setpgid() and setsid() system calls.
 *			  Non-recursive lock. Up to three such locks can
 *			  be held by a thread concurrently - for the
 *			  vproc being operated on, for a child of the
 *			  vproc, and for the "init" process.
 *	FOSTER LIST LOCK  Locks the foster child list on the node of
 *			  of the foster parent on a very short-term basis.
 *			  Non-recursive. Only one such lock can be held by
 *			  a thread concurrently.
 *	PGRP LOCK	  Locks the pvp_jobc field and the process group
 *			  list (on the node where the pgrp list is).
 *			  Non-recursive. Only one such lock can be
 *			  held by a thread concurrently. The "shared" use
 *			  of this lock is for parsing the pgrp list
 *			  without changing it.
 *	SESSION LOCK	  Locks the session list (on the node where the
 *			  session list is). Non-recursive. Only one such
 *			  lock can be held by a thread concurrently.
 *	FLAG LOCK	  Locks the vproc flag field on a very short-term
 *			  basis.
 *	SCTTY CACHE  LOCK Locks the vproc flag field on a very short-term
 *			  basis.
 *
 * The locks are in a lock hierarchy:
 *	A thread holding a VPROC LOCK can obtain a PGRP LOCK.
 *	A thread holding a PGRP LOCK can obtain a SESSION LOCK.
 *	A thread holding a VPROC LOCK can obtain a FOSTER LIST LOCK.
 *	A thread cannot hold the FOSTER LOCK and PGRP LOCK simultaneously.
 *	A thread cannot hold the FOSTER LOCK and SESSION LOCK simultaneously.
 *	If a thread is holding more than one VPROC LOCK (according to the
 *		rules above), is cannot aquire a PGRP LOCK, SESSION LOCK,
 *		or FOSTER LIST LOCK.
 *	A thread holding a FLAG LOCK cannot obtain any other lock. Any other
 *		lock can be held while obtaining the FLAG LOCK.
 *	A thread holding a SCTTY CACHE LOCK cannot obtain any other lock. Any
 *		other lock can be held while obtaining the STTY CACHE LOCK.
 */
#define VPROC_LOCK_INIT(v) do {						\
		pvp_lock_init(&PVP((v))->pvp_generic_lock);		\
		} while(0)
#define VPROC_LOCK_EXCL(v,s) do {					\
		VPROC_LOCK_DEBUG(v,s,"lock generic");			\
		pvp_lock_lock(&PVP((v))->pvp_generic_lock);		\
		} while(0)
#define VPROC_UNLOCK_EXCL(v,s) do {					\
		VPROC_LOCK_DEBUG(v,s,"unlock generic");			\
		pvp_lock_unlock(&PVP((v))->pvp_generic_lock);		\
		} while(0)

#define VPROC_LOCK_PGRP_LIST_INIT(v) do {				\
		pvp_rw_lock_init(&PVP((v))->pvp_pgrp_list_lock);	\
		} while(0)
#define VPROC_LOCK_PGRP_LIST_EXCL(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"lock pgrp list");			\
		pvp_rw_lock_lock_excl(&PVP((v))->pvp_pgrp_list_lock);	\
		} while(0)
#define VPROC_UNLOCK_PGRP_LIST_EXCL(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"unlock pgrp list");		\
		pvp_rw_lock_unlock_excl(&PVP((v))->pvp_pgrp_list_lock);	\
		} while(0)
#define VPROC_LOCK_PGRP_LIST_SHARED(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"lock pgrp list");			\
		pvp_rw_lock_lock_shared(&PVP((v))->pvp_pgrp_list_lock);	\
		} while(0)
#define VPROC_UNLOCK_PGRP_LIST_SHARED(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"unlock pgrp list");		\
		pvp_rw_lock_unlock_shared(&PVP((v))->pvp_pgrp_list_lock); \
		} while(0)

#define VPROC_LOCK_SESSION_LIST_INIT(v) do {				\
		pvp_lock_init(&PVP((v))->pvp_session_list_lock);	\
		} while(0)
#define VPROC_LOCK_SESSION_LIST_EXCL(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"lock session list");		\
		pvp_lock_lock(&PVP((v))->pvp_session_list_lock);	\
		} while(0)
#define VPROC_UNLOCK_SESSION_LIST_EXCL(v,s) do {			\
		VPROC_LOCK_DEBUG(v,s,"unlock session list");		\
		pvp_lock_unlock(&PVP((v))->pvp_session_list_lock);	\
		} while(0)

#define VPROC_LOCK_FOSTER_LIST_INIT(v) do {				\
		pvp_slock_init(&PVP((v))->pvp_foster_list_lock);	\
		} while(0)
#define VPROC_LOCK_FOSTER_LIST_EXCL(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"lock foster list");		\
		pvp_slock_lock(&PVP((v))->pvp_foster_list_lock);	\
		} while(0)
#define VPROC_UNLOCK_FOSTER_LIST_EXCL(v,s) do {			\
		VPROC_LOCK_DEBUG(v,s,"unlock foster list");		\
		pvp_slock_unlock(&PVP((v))->pvp_foster_list_lock);	\
		} while(0)

#define VPROC_LOCK_FLAG_INIT(v) do {					\
		pvp_slock_init(&PVP((v))->pvp_flag_lock);		\
		} while(0)
#define VPROC_LOCK_FLAG(v,s) do {					\
		pvp_slock_lock(&PVP((v))->pvp_flag_lock);		\
		} while(0)
#define VPROC_UNLOCK_FLAG(v,s) do {					\
		pvp_slock_unlock(&PVP((v))->pvp_flag_lock);		\
		} while(0)

#define VPROC_LOCK_SCTTY_CACHE_INIT(ps) do {				\
		pvp_slock_init(&(ps)->lock);				\
		} while(0)
#define VPROC_LOCK_SCTTY_CACHE(ps) do {					\
		pvp_slock_lock(&(ps)->lock);				\
		} while(0)
#define VPROC_UNLOCK_SCTTY_CACHE(ps) do {				\
		pvp_slock_unlock(&(ps)->lock);				\
		} while(0)
#define VPROC_LOCK_PGRP_MEM_SEQNO_INIT(v) do {				\
		pvp_slock_init(&PVP((v))->pvp_pgrp_mem_seqno_lock);	\
		} while(0)
#define VPROC_LOCK_PGRP_MEM_SEQNO(v,s) do {				\
		pvp_slock_lock(&PVP((v))->pvp_pgrp_mem_seqno_lock);	\
		} while(0)
#define VPROC_UNLOCK_PGRP_MEM_SEQNO(v,s) do {				\
		pvp_slock_unlock(&PVP((v))->pvp_pgrp_mem_seqno_lock);	\
		} while(0)


/*
 * The three macros below really need to perform locking only if shared
 * list locks for pgrps are truly implemented, which they are not.  The
 * macros have been used in the code where they would be needed and are
 * defined here to be null.
 */
#define VPROC_LOCK_PGRP_LDR_SEQNO_INIT(v)
#define VPROC_LOCK_PGRP_LDR_SEQNO(v,s)
#define VPROC_UNLOCK_PGRP_LDR_SEQNO(v,s)

/*
 * Macros used to prevent process migration (temporarily) and at the
 * start and end of process migration (on origin node only).
 *	VPROC_HOLD_MOVEMENT_INIT	Initialize relevent data structures
 *	VPROC_HOLD_MOVEMENT		Prevent migration from this node
 *					until VPROC_RELEASE_MOVEMENT is called.
 *					Note: recursion is allowed.
 *	VPROC_HOLD_MOVEMENT_LOCAL	Same as VPROC_HOLD_MOVEMENT, except that
 *					if the process is not executing on
 *					this node, then EREMOTE is returned
 *					and there is no need to perform
 *					a VPROC_RELEASE_MOVEMENT.
 *	VPROC_RELEASE_MOVEMENT		Undo the effect of a VPROC_HOLD_MOVEMENT
 *					or VPROC_HOLD_MOVEMENT_LOCAL.
 *	VPROC_START_MOVEMENT		A process is migrating. All
 *					VPROC_HOLD_MOVEMENT* calls are held
 *					until the migration completes.
 *					Note: this is not a recursive call.
 *	VPROC_END_MOVEMENT		Undoes a VPROC_START_MOVEMENT call.
 */
#define VPROC_HOLD_MOVEMENT_INIT(v)	(void)vproc_hold_movement_init(v)
#define VPROC_HOLD_MOVEMENT(v,s)	vproc_hold_movement(v,0,s)
#define VPROC_HOLD_MOVEMENT_LOCAL(v,s)	vproc_hold_movement(v,1,s)
#define VPROC_RELEASE_MOVEMENT(v,s)	vproc_release_movement(v,s)

#define VPROC_START_MOVEMENT(v,s)	vproc_start_movement(v,s)
#define VPROC_END_MOVEMENT(v,s)		vproc_end_movement(v,s)

/*
 * The following table provides indirection synchronous and asynchronous
 * operations tables.
 */
struct pvproc_ops_vector {
	struct pvproc_ops *sync,	/* perform operation synchronously */
			  *send,	/* initiate operation asynchronously */
			  *receive;	/* wait for operation completion */
};


/*
 * Macros to invoke the private virtual process operations are generated by
 *	the makeTNCtables script and placed in tnc/dpvproc_macros_gen.h
 */

/*
 * Finally, _PVPOP_, the internal macro for PVPOP_* operations.
 * There are 2 flavors:
 *    -	for mainline code, the default simply calls the corresponding
 *	pvpop_* wrapper routine;
 *    -	for wrapper and remote server routines with symbol PVPOP_VIA_TABLE
 *	defined, the operation is called indirectly through the pvproc op table.
 * The latter case is also modified if pvproc debugging is enabled.
 */
#if	MACH_ASSERT
#define PVPROC_DEBUG 1
#endif	/* MACH_ASSERT */

#ifndef	PVPOP_VIA_TABLE
#define __PVPOP__(op,mode,v,args,argfmt)	op##_##mode args
#else	/* PVPOP_VIA_TABLE */
#ifdef	PVPROC_DEBUG
#ifdef	__STDC__
#define __PVPOP__(op,mode,v,args,argfmt) \
	(dpvproc_db1(v, #op, argfmt), \
	 dpvproc_db2 args,dpvproc_db3(v, (*PVP(v)->pvp_ops->mode->op) args))
#else	/* __STDC__ */
#define __PVPOP__(op,mode,v,args,argfmt) \
	(dpvproc_db1(v, "op", argfmt), \
	 dpvproc_db2 args,dpvproc_db3(v, (*PVP(v)->pvp_ops->mode->op) args))
#endif	/* __STDC__ */
#else	/* PVPROC_DEBUG */
#define __PVPOP__(op,mode,v,args,argfmt) (*PVP(v)->pvp_ops->mode->op) args
#endif	/* PVPROC_DEBUG */
#endif	/* PVPOP_VIA_TABLE */
#define _PVPOP_(op, v, args, argfmt)         __PVPOP__(op,sync,v,args,argfmt)
#define _PVPOP_SEND_(op, v, args, argfmt)    __PVPOP__(op,send,v,args,argfmt)
#define _PVPOP_RECEIVE_(op, v, args, argfmt) __PVPOP__(op,receive,v,args,argfmt)

#ifdef PVPROC_DEBUG
extern int vplocktrace;
#define VPROC_LOCK_DEBUG(v,s1,s2) do {					      \
	if (vplocktrace && (vplocktrace == -1 || vplocktrace == (v)->vp_pid)) \
		printf("[%d] %s: %s\n",(v)->vp_pid,s1,s2);		      \
	} while (0)
#else	/* PVPROC_DEBUG */
#define VPROC_LOCK_DEBUG(v,s1,s2)
#endif	/* PVPROC_DEBUG */

/*
 * The following table provides indirection synchronous and asynchronous
 * operations tables.
 */
struct pvps_ops_vector {
	struct pvps_ops *sync,		/* perform operation synchronously */
			*send,		/* initiate operation asynchronously */
			*receive;	/* wait for operation completion */
};


/*
 * Macros to invoke the virtual process system operations are generated by
 *	the makeTNCtables script and placed in tnc/dpvps_macros_gen.h
 */

/*
 * To debug vproc system ops or not, that is the question...
 */
#ifdef	PVPROC_DEBUG
#ifdef	__STDC__
#define __PVPSOP__(op,mode,node,args,argfmt) \
	(dpvps_db1(#op,argfmt), dpvps_db2 args, \
	 dpvps_db3((*((struct pvps_ops_vector *) \
		(tnc_get_pvps_ops(node)))->mode->op) args))
#else	/* __STDC__ */
#define __PVPSOP__(op,mode,node,args,argfmt) \
	(dpvps_db1("op",argfmt), dpvps_db2 args, \
	 dpvps_db3((*((struct pvps_ops_vector *) \
		(tnc_get_pvps_ops(node)))->mode->op) args))
#endif	/* __STDC__ */
#else	/* PVPROC_DEBUG */
#define __PVPSOP__(op,mode,node,args,argfmt) \
	(*((struct pvps_ops_vector *)(tnc_get_pvps_ops(node)))->mode->op) args
#endif	/* PVPROC_DEBUG */
#define _PVPSOP_(op,node,args,argfmt) \
	__PVPSOP__(op,sync,node,args,argfmt)
#define _PVPSOP_SEND_(op,node,args,argfmt) \
	__PVPSOP__(op,send,node,args,argfmt)
#define _PVPSOP_RECEIVE_(op,node,args,argfmt) \
	__PVPSOP__(op,receive,node,args,argfmt)

/*
 * PVPOP and PVPSOP macros are generated automatically from the
 * operations definitions contained in files tnc/{*.ops}.
 */
#include <tnc/dpvproc_macros_gen.h>
#include <tnc/dpvps_macros_gen.h>
#include <tnc/dpvproc_struct_gen.h>
#include <tnc/dpvps_struct_gen.h>

#endif  /* _H_DPVPROC */
