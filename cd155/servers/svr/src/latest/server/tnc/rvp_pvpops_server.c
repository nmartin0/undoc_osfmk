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
 * $Log: rvp_pvpops_server.c,v $
 * Revision 1.14  1995/02/01  21:51:13  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.13  1994/11/18  20:44:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/04/28  19:14:52  chrisp
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
 * Revision 1.11  1994/04/26  14:23:43  cfj
 * Merge revision 1.7.2.2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10  1994/03/14  02:06:45  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.9  1994/02/16  17:22:24  jlitvin
 * Remove extra RCS Log line found by Chris Peak of Locus.
 *
 * Revision 1.8  1993/12/08  20:13:00  nandy
 * LOCATE_VALID_VPROC_PID() used to panic if LOCATE_VPROC_PID() returned null.
 * Instead, it should just return EINVAL.
 *
 * Revision 1.7.2.2  1994/04/26  14:18:52  cfj
 * Change POST_PVPOP_SIGPROC_MULTI to POST_RPVPOP_SIGPROC_MULTI to plug
 * a memory leak.  It was incorrectly named in the first place.
 *
 *  Reviewer:chrisp@locus.com
 *  Risk:L
 *  Benefit or PTS #:9108
 *  Testing:sched EAT, VSX EAT
 *  Module(s):server/tnc/rvp_pvpops_server.c
 *
 * Revision 1.7.2.1  1993/12/08  20:09:31  nandy
 * Merged from the main stem. LOCATE_VALID_VPROC_PID() used to panic if
 * LOCATE_VPROC_PID() returned null. Instead, it should just return EINVAL.
 *
 *  Reviewer: Chris Peak
 *  Risk: Low
 *  Benefit or PTS #: 3704
 *  Testing: Ran the test in the PTS report
 *  Module(s): rvp_pvpops_server.c
 *
 * Revision 1.7  1993/10/21  23:36:30  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.6  1993/07/14  18:34:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:47:21  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/20  16:03:29  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.4  1993/05/06  19:24:38  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:47:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1992/11/30  22:48:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:32:04  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:46:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.26  1993/04/22  16:40:19  roman
 * [Bug 227] Leak of vproc (and associated port) if we hit a process
 * 	that is migrating.
 *
 * Revision 3.28  93/09/16  09:15:36  chrisp
 * [SPE 0030] Generic Spanning Trees: POST_PVPOP_SIGPROC_MULTI() defined
 * 
 * Revision 3.27  93/05/14  12:15:56  slively
 * Add the macros PRE and POST for RPVPOP_TABLE_SET.
 * 
 * Revision 3.26  93/04/22  16:40:19  roman
 * [Bug 227] Leak of vproc (and associated port) if we hit a process
 * 	that is migrating.
 * 
 * Revision 3.25  92/11/02  12:17:53  roman
 * Fix RCS comments in last submission for accuracy.
 * 
 * Revision 3.24  92/10/28  15:44:46  roman
 * Change comments to reflect the "user exits" that are possble for
 * 	special pvpop processing.
 * Add "user exit" code for table_get() pvpop to deal with out-of-line
 * 	memory used by MiG.
 * Change name of C file included with the "real" code to match the
 * 	new file names used by makeTNCtables.sh.
 * Add comments describing the asynchronous signalling routines.
 * Add missing server_port parameters to asynchronous signalling
 * 	calls (causes unexpected failures).
 * 
 * Revision 3.23  92/10/08  17:12:50  chrisp
 * Contents gutted and replaced by #include of generated server stubs.
 * 
 * Revision 3.22  92/09/29  09:55:47  roman
 * Fix prior RCS comment.
 * 
 * Revision 3.21  92/09/28  16:08:35  roman
 * First submission of this file under this name - came from rvp_server.c.
 * Change name of set_state pvproc op to report_state.
 * Add pvproc ops for adding and removing foster children.
 * Remove setpinit pvproc op (work is done entirely by vproc op.
 * Add terminal_sigpgrp pvproc op.
 * Change name of "migrate" locks to "movement" locks.
 * 
 * Revision 3.20  92/07/30  16:16:09  chrisp
 * Increment the reference count of the target vproc of a remote operation
 * 	so that a VPOP_FREE performed on it will be deferred until the
 * 	operation has ended and the port sequence count is up-to-date.
 * 
 * Revision 3.19  92/07/08  09:10:13  roman
 * Fix prior RCS comment.
 * 
 * Revision 3.18  92/06/26  14:01:31  chrisp
 * [Bug #35] Zombie or stopped children assigned to a new parent must have
 * 	their correct state recorded in their vproc on the parent's node.
 * 
 * Revision 3.17  92/06/17  09:14:13  roman
 * [Bug 0026] New vproc op VPOP_GET_TASK_PORT() added to get task_by_pid() 
 *	system call working.
 * 
 * Revision 3.16  92/05/07  10:48:05  chrisp
 * [Bug #10] Add optional second signal number to PVPOP_SIGPROC().
 * 
 * Revision 3.15  92/04/30  10:50:15  chrisp
 * [Bug #3] PVPOP_REAP() now takes an additional parameter requiring a reaped
 * 	process belong to a specified pgid (if not WAIT_ANY); error ESRCH
 * 	is returned if this is not the case and the process is not reaped.
 * 	This avoids VPOP_WAIT() using PVPOP_GETATTR() for each child's pgid.
 * 
 * Revision 3.14  92/04/06  16:06:35  chrisp
 * Remove master locking support from START_/END_RPVPOP_SERVER_OP.
 * 
 * Revision 3.13  92/03/18  12:23:29  roman
 * Restructure all server routines to use START_RPVPOP_SERVER_OP() and
 * END_RPVPOP_SERVER_OP() macros at the beginning and end of every
 * routine.
 * For the *RPVPOP_SERVER_OP() macros themselves, make sure the server
 * routines do not interract badly with process migration that may
 * be occurring almost concurrently, and also make sure the master
 * lock is acquired and released wherever appropriate.
 * 
 * Revision 3.12  92/03/12  15:36:42  roman
 * Remove resign_pgrp pvproc op.
 * Add set_attr pvproc op.
 * Add missing parameter to tnc_bestow_vproc_port().
 * svr_tnc_sigp***_async() added to process signalling deferred from a
 * 	non-blocking (interrupt) process to a remote process or process
 * 	group.
 * 
 * Revision 3.11  92/03/03  16:26:34  chrisp
 * Add ADD_CHILD_TO_PARENT.
 * Changes to parameters for the CTTY operations.
 * Addition of tnc server ops to access controlling tty node session vproc.
 * 
 * Revision 3.10  92/01/30  14:19:36  chrisp
 * Add ptrace.
 * 
 * Revision 3.9  92/01/15  10:31:43  chrisp
 * Add svr_tnc_remote_vproc_free()
 * 
 * Revision 3.8  91/12/24  10:34:20  roman
 * Use new routine tnc_bestow_vproc_port() to give away send right to a
 * vproc rather than doing the work by hand.
 * 
 * Revision 3.7  91/12/20  16:50:59  chrisp
 * PVPOP changes reflected in server ops.
 * 
 * Revision 3.6  91/12/13  11:05:27  roman
 * Change incorrect order of parameters to VPROC_RELEASE().
 * Get rid of first parameter to end_rpvpop_server_op().
 * 
 * Revision 3.5  91/12/05  10:37:41  chrisp
 * Add VPROC_RELEASE()s to match LOCAT_VPROC_PID().
 * 
 * Revision 3.4  91/12/04  08:06:56  chrisp
 * Add remote server support for pvpops set_ctty and ctty_getattr.
 * 
 * Revision 3.3  91/11/26  11:59:42  chrisp
 * setsid added as a pvpop.
 * 
 * Revision 3.2  91/11/22  10:27:07  chrisp
 * set_stop_state becomes set_state with 3 params.
 * 
 * Revision 3.1  91/11/18  11:34:08  chrisp
 * Extra has_sctty param to GET_ATTR.
 * 
 * Revision 3.0  91/10/25  10:31:40  roman
 * Initial submission of server side of remote pvproc ops.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <uxkern/import_mach.h>
#include <sys/vproc.h>
#define	PVPOP_VIA_TABLE
#include <tnc/dpvproc.h>
#include <uxkern/proc_to_task.h>
#include <sys/user.h>
#include <kern/parallel.h>


/*
 * Macros defining the MIG interface - to make the source code more portable.
 */
#define	RET (*_rval)

#define	DECL_RPVP_SERVER_OP(op) int svr_##op(

#define	DECL_RPVP_DESTINATION						\
	mach_port_t	_port,						\
	int		*_rval

#define	END_DECL )

#define	PVP_DESTINATION _vp

/*
 * Macros defining the generic interface between server stub and underlying
 * local private virtual process operations.
 */
#define START_RPVP_SERVER_OP(str)					\
	struct vproc	*_vp = port_to_vproc_lookup(_port);		\
	char		*_id_string = str;				\
	int		_error = ESUCCESS;				\
	VPROC_HOLD(_vp,_id_string);					\
	RET = VPROC_HOLD_MOVEMENT_LOCAL(_vp,_id_string);		\
	if (RET != ESUCCESS) {						\
		VPROC_RELEASE(_vp,_id_string);				\
		return(KERN_SUCCESS);					\
	}								\
	start_rpvpop_server_op(_port)

#define	RPVP_SERVER_OP(op)						\
	if (!_error)							\
		_error = op

#define END_RPVP_SERVER_OP						\
	RET = _error;							\
	end_rpvpop_server_op();						\
	VPROC_RELEASE_MOVEMENT(_vp,_id_string);				\
	VPROC_RELEASE(_vp,_id_string);					\
	return(KERN_SUCCESS)

#define	LOCATE_VALID_VPROC_PID(pid, vp)					\
	if (pid != 0) {							\
		vp = LOCATE_VPROC_PID(pid);				\
		if (vp == NULL)						\
			_error = EINVAL;				\
	} else								\
		vp = NULL

#define	VALID_VPROC_RELEASE(vp)						\
	if (vp)								\
		VPROC_RELEASE(vp, _id_string)


/*
 * Include the server stubs generated automatically from header files.
 * Each operation server stub is of the form:
 *	DECL_RPVP_SERVER_OP(r<op_name>)
 *		DECL_RPVP_DESTINATION,
 *		<parm_type_2> <parm_name_2>,
 *			... repeated 
 *		<parm_type_n> <parm_name_n>
 *	END_DECL
 *	{
 *		START_RPVP_SERVER_OP("<op_name>");
 *		RPVP_SERVER_OP(<OP_NAME>(PVP_DESTINATION,
 *				<parm_name_2>,...,<parm_name_n>));
 *		END_RPVP_SERVER_OP;
 *	}
 *	}
 *	IN parameters of type struct vproc are specially treated with the
 *	following transformations being performed:
 *	    o	the formal declaration becomes "<parm_name>_pid"
 *	    o	"LOCATE_VALID_VPROC_PID(<parm_name>_pid,<parm_name>)"
 *		appears before the RPVP_SERVER_OP() invocation
 *	    o	"VALID_VPROC_RELEASE(<parm_name>)"
 *		appears following the RPVP_SERVER_OP() invocation
 */
#include <tnc/rvp_pvpops_server_gen.c>


/*
 * The following 2 routines define the server stubs for asynchronous
 * signaling. Since remote pvproc operations block in their RPC,
 * non-blocking vproc signaling operations are handed off to the
 * server on the requesting node via these asynchronous RPCs whose
 * responsibility it is to perform the requested remote operation.
 *
 */

#define START_ASYNC_SERVER_OP(str)					\
	int		_error = ESUCCESS;				\
	char		*_id_string = str

#define END_ASYNC_SERVER_OP						\
	return(KERN_SUCCESS)

DECL_RPVP_SERVER_OP(tnc_sigproc_async)
	mach_port_t	server_port,
	pid_t		target_pid,
	int		signo,
	int		arg,
	uid_t		effuid,
	uid_t		realuid,
	pid_t		pid,
	pid_t		sid,
	int		flag
END_DECL
{
	struct vproc	*vp;

	START_ASYNC_SERVER_OP("tnc_sigproc_async");

	LOCATE_VALID_VPROC_PID(target_pid, vp);

	(void) PVPOP_SIGPROC(vp, signo,0, arg, effuid, realuid, pid, sid, flag);

	VALID_VPROC_RELEASE(vp);

	END_ASYNC_SERVER_OP;
}

DECL_RPVP_SERVER_OP(tnc_sigpgrp_async)
	mach_port_t	server_port,
	pid_t		target_pid,
	int		signo,
	int		arg,
	uid_t		effuid,
	uid_t		realuid,
	pid_t		pid,
	pid_t		sid,
	int		flag
END_DECL
{
	struct vproc	*vp;

	START_ASYNC_SERVER_OP("tnc_sigproc_async");

	LOCATE_VALID_VPROC_PID(target_pid, vp);

	(void) PVPOP_SIGPGRP(vp,signo,arg,effuid,realuid,pid,sid,NULL,flag);

	VALID_VPROC_RELEASE(vp);

	END_ASYNC_SERVER_OP;
}

