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
 * $Log: rvp_pvpops.c,v $
 * Revision 1.24  1995/02/01  21:50:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.23  1994/11/18  20:44:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.22  1994/06/18  00:45:25  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.21  1994/04/28  19:14:46  chrisp
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
 * Revision 1.20  1994/03/14  02:06:39  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.19  1994/02/18  17:49:58  cfj
 * Put the log level in the calls to the log() function.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.18  1994/01/11  01:57:49  cfj
 * Merge bug fix from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17.2.1  1994/01/11  01:55:21  cfj
 * Modify the END_RPVP_CLIENT_OP and END_RPVP_CLIENT_OP_SEND macro to
 * check for MIG_SERVER_DIED and return ESRCH when it is returned.
 *
 *  Reviewer: nandy, Locus
 *  Risk:M
 *  Benefit or PTS #:7655
 *  Testing:#7655 test case.
 *  Module(s): server/tnc/rvp_pvpops.c
 *
 * Revision 1.17  1993/12/01  15:34:42  cfj
 * Modified a error panic message so two identical messages could be
 * differentiated.
 *
 *  Reviewer:
 *  Risk:L
 *  Benefit or PTS #: Being able to differentiate between panics
 *  Testing:
 *  Module(s):
 *
 * Revision 1.16  1993/10/21  23:36:21  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.15  1993/08/20  16:51:12  cfj
 * Fix for PTS bug #6252 to avoid possible server panic.
 *
 * Revision 1.14  1993/07/20  21:41:22  nandy
 * Instead of printing the message directly, the timeout code calls nx_print_fatal_message.
 *
 * Revision 1.13  1993/07/14  18:34:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:47:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.12  1993/06/09  00:10:07  cfj
 * Change occurances of #include <i860ipsc/mcmsg> to #include <i860paragon/mcmsg>
 *
 * Revision 1.11  1993/06/04  21:27:52  nandy
 * Made a mistake lat time.
 *
 * Revision 1.10  1993/06/04  21:19:20  nandy
 * Fixed a merge problem with timeout logic.
 *
 * Revision 1.9  1993/05/25  15:16:32  cfj
 * Put a line feed in the reboot message
 *
 * Revision 1.8  1993/05/20  16:03:27  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.7  1993/05/06  19:24:32  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:46:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:20  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/31  21:47:34  nandy
 * If the RPC fails with a timeout, print out the bad node number on the
 * console.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:02:57  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  03:02:25  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:48:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:32:00  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:46:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.26  93/09/16  09:12:53  chrisp
 * [SPE 0030] Generic Spanning Trees: client operation macros extended
 * 	for _SEND and _RECEIVE asynchronous flavors.
 * 
 * Revision 3.25  93/06/23  09:33:49  slively
 * Change the comment from the last installation of this file to reflect
 * reality.  There is no TABLE_SET in rvp_pvpops.c and therefore no need
 * to have PRE and POST macros (in fact I don't think that there ever were
 * PRE and POST macros for TABLE_SET.
 * 
 * Revision 3.24  93/05/14  12:14:57  slively
 * TABLE_SET is not needed in rvp_pvpops.c 
 * 
 * Revision 3.23  92/11/02  12:17:34  roman
 * Get rid of type buffer_t; it's really a char_array.
 * Fix RCS comments for last submission for accuracy.
 * 
 * Revision 3.22  92/10/28  15:30:04  roman
 * Add comments to describe the new "user exits" allowed for pvpops.
 * Add "user exits" to deal with out-of-line memory for the table_get()
 * 	pvpop.
 * Change name of C file included with the "real" code to match the
 * 	new file names used by makeTNCtables.sh.
 * 
 * Revision 3.21  92/10/08  17:06:46  chrisp
 * Contents gutted and replaced by #include of generated client stubs.
 * 
 * Revision 3.20  92/10/01  10:32:29  roman
 * Fix up types for clean compilation under gcc.
 * Fix bug where cli_rpvpop_get_attr() was called rather than 
 * 	cli_rpvpop_set_attr().
 * 
 * Revision 3.19  92/09/29  08:16:35  roman
 * Minor formatting fixes.
 * Get rid of tnc_get_vproc_port() routine - equivalent functionality now
 * 	found in pvps ops.
 * Add new pvproc ops for adding and removing foster children.
 * Change name of set_state pvproc op to report_state.
 * Get rid of setpinit and killall pvproc ops.
 * Add terminal_sigpgrp pvproc op.
 * Parameter changes to ctty_getattr pvproc op.
 * 
 * Revision 3.18  92/07/08  09:09:55  roman
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.17  92/06/26  14:01:18  chrisp
 * [Bug #35] Zombie or stopped children assigned to a new parent must have
 * 	their correct state recorded in their vproc on the parent's node.
 * 
 * Revision 3.16  92/06/17  09:14:01  roman
 * [Bug 0026] New vproc op VPOP_GET_TASK_PORT() added to get task_by_pid() 
 *	system call working.
 * 
 * Revision 3.15  92/05/07  10:47:52  chrisp
 * [Bug #10] Add optional second signal number to PVPOP_SIGPROC().
 * 
 * Revision 3.14  92/04/30  10:49:58  chrisp
 * [Bug #3] PVPOP_REAP() now takes an additional parameter requiring a reaped
 * 	process belong to a specified pgid (if not WAIT_ANY); error ESRCH
 * 	is returned if this is not the case and the process is not reaped.
 * 	This avoids VPOP_WAIT() using PVPOP_GETATTR() for each child's pgid.
 * 
 * Revision 3.13  92/03/19  14:59:52  chrisp
 * Change macro PVPOP_OP() to release and retake recursive unix_master locks
 * across remote operations.
 * 
 * Revision 3.12  92/03/18  12:20:38  roman
 * Restructure the code to invokde START_RPVPOP_CLIENT() and END_RPVPOP_CLIENT()
 * at the start and end of each remote client pvproc op.
 * For *_RPVPOP_CLIENT macros themselves, make sure the master lock is
 * not held when doing a remote op.
 * 
 * Revision 3.11  92/03/12  15:34:02  roman
 * Remove resign_pgrp() pvproc op.
 * Add set_attr() pvproc op.
 * Move validate_vproc_port() into rvp_subr.c.
 * rpvpop_sigproc and rpvpop_sigpgrp now honor the VPROC_CANT_BLOCK flag by
 * 	deferring a remote operation to be performed by the local tnc
 * 	server thread at a later time.
 * 
 * Revision 3.11  92/03/09  11:09:04  chrisp
 * PVPOP_CTTY_GETATTR() and PVPOP_SET_CTTY() parameter changes.
 * Add ADD_CHILD_TO_PARENT remote op for remote init.
 * Special case handling added for pid 1 (use root_fs_node).
 * 
 * Revision 3.9  92/01/30  14:19:15  chrisp
 * Add ptrace.
 * 
 * Revision 3.8  91/12/24  10:33:02  roman
 * Change name of tnc_establish_vproc_port() to tnc_fetch_vproc_port().
 * 
 * Revision 3.7  91/12/20  16:50:17  chrisp
 * PVPOP changes reflected in remote ops also.
 * 
 * Revision 3.6  91/12/13  11:04:31  roman
 * Change validate_vproc_port to establish_vproc_port.
 * Remove source code for validate_vproc_port.
 * Change relevent code to allow for NULL pointers in calls (NULL output
 * parameters are note handled well by MiG).
 * 
 * Revision 3.5  91/12/04  07:59:10  chrisp
 * Add remote pvpops set_ctty and ctty_getattr.
 * 
 * Revision 3.4  91/11/26  11:59:20  chrisp
 * setsid added as a pvpop.
 * 
 * Revision 3.3  91/11/22  10:26:18  chrisp
 * set_stop_state becomes set_state with 3 params.
 * 
 * Revision 3.2  91/11/18  11:33:41  chrisp
 * SIG* extra param.
 * 
 * Revision 3.1  91/11/04  12:54:15  roman
 * Fixed bug where code read "xxxx | yyyy" rather than "xxxx |= yyyy".
 * 
 * Revision 3.0  91/10/25  10:31:06  roman
 * Initial submission of client side of remote pvproc ops.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/syslog.h>
#include <uxkern/import_mach.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <uxkern/proc_to_task.h>
#include <sys/user.h>
#include <kern/parallel.h>
#include <tnc/tnc.h>
#include <tnc/tnc_async.h>

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>
#endif /* NX */

/*
 * Macros defining the MIG interface - to make the source code more portable.
 */
#define	RPVP_DESTINATION(vp) vproc_to_port_lookup(vp), &_rval

#define RPVP_CLIENT_OP(op,args)						\
		_ret = cli_##op args

#define START_RPVP_CLIENT_OP(vp,str)					\
	{								\
		char		*_id_string = str; 			\
		int		_rval;					\
		int		_node;					\
		kern_return_t	_ret;					\
		struct vproc    *_vproc =  vp;				\
		if (tnc_fetch_vproc_port(vp) != ESUCCESS)		\
			return(ESRCH);					\
		ux_server_thread_blocking()

#ifdef NX
#define END_RPVP_CLIENT_OP 						\
		ux_server_thread_unblocking();				\
		switch (_ret) {						\
		    case KERN_SUCCESS:					\
			return(_rval);					\
		    case MACH_SEND_INVALID_DEST:			\
		    case MIG_SERVER_DIED:				\
			return(ESRCH);					\
		    case MACH_RCV_TIMED_OUT:				\
			if(norma_port_location_hint(mach_task_self(),	\
				vproc_to_port_lookup(_vproc), &_node) ==\
				KERN_SUCCESS ) {			\
				 printf(                         \
"\nFATAL ERROR: %s : Node %d cannot recover.\nPlease Reboot Immediately\n",\
                                            _id_string,_node);       \
				 nx_print_fatal_message(_node, _id_string);	\
				} else					\
					panic("%s:norma_port_location_hint(): bad rpc, ret=0x%x",	\
					 _id_string, _ret);		\
			return(ESUCCESS);				\
		    default:						\
			log(LOG_ERR, "%s: bad rpc, ret=0x%x", _id_string, _ret);\
			return(ESRCH);					\
		}							\
	}
#else /* NX */
#define END_RPVP_CLIENT_OP 						\
		ux_server_thread_unblocking();				\
		switch (_ret) {						\
		    case KERN_SUCCESS:					\
			return(_rval);					\
		    case MACH_SEND_INVALID_DEST:			\
		    case MIG_SERVER_DIED:				\
			return(ESRCH);					\
		    default:						\
			log(LOG_ERR, "%s: bad rpc, ret=0x%x", _id_string, _ret);\
			return(ESRCH);					\
		}							\
	}
#endif /* NX */

/*
 * For asynchronous operations, send and receive variants of the above
 * macros also exist:
 */
/*
 * The destination macros include the reply port involved. 
 */
#define	RPVP_DEST_SEND(vp)	vproc_to_port_lookup(vp), _reply_port, &_rval
#define	RPVP_DEST_RECEIVE(vp)	vproc_to_port_lookup(vp), _reply_port, &_rval

/*
 * Different client MIG stubs are involved.
 */
#define RPVP_CLIENT_OP_SEND(op,args)					\
		_ret = cli_##op##_msg_send args
#define RPVP_CLIENT_OP_RECEIVE(op,args)					\
		_ret = cli_##op##_msg_receive args

/*
 * When sending, we allocate a reply port and return it to our caller
 * by way of the handle parameter. We don't declare that the thread will block.
 */
#define START_RPVP_CLIENT_OP_SEND(vp,handle,str)			\
	{								\
		char		*_id_string = str; 			\
		int		_rval;					\
		kern_return_t	_ret;					\
		mach_port_t	_reply_port;				\
		if (tnc_fetch_vproc_port(vp) != ESUCCESS)		\
			return(ESRCH);					\
		_reply_port = mach_reply_port();			\
		*handle = (msg_handle_t) _reply_port

/*
 * When sending, we should deallocate the reply port upon failure.
 */
#define END_RPVP_CLIENT_OP_SEND						\
		switch (_ret) {						\
		    case KERN_SUCCESS:					\
			return(ESUCCESS);				\
		    case MIG_SERVER_DIED:				\
			return(ESRCH);					\
		    case MACH_SEND_INVALID_DEST:			\
			(void) mach_port_destroy(mach_task_self(),	\
						 _reply_port);		\
			return(ESRCH);					\
		    default:						\
			log(LOG_ERR,"%s: bad rpc, ret=0x%x", _id_string, _ret);\
			return(ESRCH);					\
		}							\
	}

/*
 * When receiving, we use the reply port again given by the handle parameter,
 * and destroy it afterwards.
 */
#define START_RPVP_CLIENT_OP_RECEIVE(vp,handle,str)			\
	{								\
		char		*_id_string = str; 			\
		int		_rval;					\
		kern_return_t	_ret;					\
		mach_port_t	_reply_port = (mach_port_t) *handle;	\
		ux_server_thread_blocking()

#define END_RPVP_CLIENT_OP_RECEIVE 					\
		ux_server_thread_unblocking();				\
		(void) mach_port_destroy(mach_task_self(), _reply_port);\
		switch (_ret) {						\
		    case KERN_SUCCESS:					\
			return(_rval);					\
		    case MACH_SEND_INVALID_DEST:			\
		    case MIG_SERVER_DIED:				\
			return(ESRCH);					\
		    default:						\
			log(LOG_ERR,"%s: bad rpc, ret=0x%x", _id_string, _ret);\
			return(ESRCH);					\
		}							\
	}

/*
 * Include the client stubs generated from the TNC header files.
 * Each operation client stub is of the form:
 *	int r<op_name>(
 *		struct vproc *<parm_name_1>,
 *		<parm_type_2> <parm_name_2>,
 *			... repeated 
 *		<parm_type_n> <parm_name_n>)
 *	{
 *		START_RPVP_CLIENT_OP(<parm_name_1>,"<op_name>");
 *		RPVP_CLIENT_OP(cli_r<op_name>(RPVP_DESTINATION(<parm_name_1>,
 *				<parm_name_2>,...,<parm_name_n>));
 *		END_RPVP_CLIENT_OP;
 *	}
 *	Noting, in addition, that OUT parameters appear in the client RPC
 *	in the form "<parm_name> ? <parm_name> : (<parm_type> *)&dummy" to
 *	avoid null pointers being passed to the RPC. Furthermore, IN
 *	parameters of type struct vproc are transformed to be:
 *	"<parm_name> ? <parm_name>->vp_pid : 0".
 *
 *	In addition, ASYNC operations will have stubs generated for send
 *	and receive modes for which, in effect, <op_name> in appended
 *	with "_send" and "_receive" in turn. 
 */
#include <tnc/rvp_pvpops_gen.c>
