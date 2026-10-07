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
 * $Log: rvp_pvpsops.c,v $
 * Revision 1.18  1995/02/01  21:51:35  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.17  1994/11/18  20:44:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/04/28  19:14:57  chrisp
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
 * Revision 1.15  1994/04/04  17:34:47  paul
 *  Reviewer:John Litvin
 *  Risk: M
 *  Benefit or PTS #:7046
 *  Testing: test program run on Plymouth several thousand times...
 *  Module(s):
 *         server/tnc/dvp_vpsops.c
 *         server/tnc/dpvproc_struct.h
 *         server/tnc/tnc_types.h
 *         server/tnc/tnc_types.defs
 *         server/tnc/rvp_pvpsops.c
 *         server/tnc/dvp_pvpsops.c
 *         server/bsd/kern_xxx.c
 *         server/tnc/rvp_pvpsops_server.c
 * Use spanning trees for setting global hostname, hostid, domain name,
 *     timezone, rpm offset
 *
 * Revision 1.14  1994/03/14  02:06:49  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.13  1994/01/11  15:50:02  cfj
 * Merge R1_2 change into main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12.2.1  1994/01/11  15:48:28  cfj
 * Modify the END_RPVPS_CLIENT_OP so that the node number of the failing
 * node is always printed on error.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/10/21  23:37:15  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.11  1993/07/20  21:42:28  nandy
 * Instead of printing the message directly, the timeout mechanism calls nx_print_fatal_message.
 *
 * Revision 1.10  1993/07/14  18:34:12  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:47:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/05/25  15:16:45  cfj
 * Put a line feed in the reboot message
 *
 * Revision 1.8  1993/05/20  16:03:32  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.7  1993/05/06  19:24:47  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:47:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/26  19:28:02  cfj
 * Return an error instead of panicing in START_RPVPS_CLIENT_OP if tnc_get_server_port() returns an error.
 *
 * Revision 1.5  1993/04/09  16:19:20  cfj
 * Return an error instead of panicing on RPC timeout.
 *
 * Revision 1.4  1993/03/31  21:48:10  nandy
 * If the RPC fails with a timeout, print out the bad node number on the console.
 *
 * Revision 1.3  1992/11/30  22:48:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:32:07  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:46:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.6  93/09/16  09:26:50  chrisp
 * [SPE 0030] Generic Spanning Trees: client operation macros extended
 * 	for _SEND and _RECEIVE asynchronous flavors.
 * 
 * Revision 3.5  93/05/12  13:07:38  yazz
 * 
 * Have the START_RPVPS_CLIENT_OP macro return an error, rather than
 * panic, if asked to talk to an invalid node.  Allows GDB to work on
 * i860.  (From cfj at Intel.)
 * 
 * Revision 3.4  92/11/02  12:19:16  roman
 * Fix RCS comments in last submission for accuracy.
 * Get rid of type buffer_t; it's really a char_array.
 * 
 * Revision 3.3  92/10/28  15:48:33  roman
 * Change comments to reflect the "user exits" that are possble for
 *         special pvpsop processing.
 * Add "user exit" code for table_get() pvpaop to deal with out-of-line
 *         memory used by MiG.
 * Change name of C file included with the "real" code to match the
 *         new file names used by makeTNCtables.sh.
 * 
 * Revision 3.2  92/10/08  17:13:38  chrisp
 * Contents gutted and replaced by #include of generated client stubs.
 * 
 * Revision 3.1  92/10/01  10:32:45  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.0  92/09/28  16:09:28  roman
 * Initial submission of file with client code for private virtual process
 * system operations for remote nodes.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/vproc.h>
#include <uxkern/import_mach.h>
#include <tnc/dpvproc.h>
#include <tnc/tnc.h>


/*
 * Macros defining the MIG interface - to make the source code more portable.
 */
#define RPVPS_DESTINATION(node)	_server_port, &_rval

#ifdef	NX
#define START_RPVPS_CLIENT_OP(node,str) {				\
	char		*_id_string = str;				\
	int		_rval;						\
	mach_port_t	_server_port;					\
	int		_err;						\
	kern_return_t	_ret;						\
	_err = tnc_get_server_port(node, &_server_port);		\
	if (_err != ESUCCESS) {						\
		return (_err);						\
	}								\
	ux_server_thread_blocking()

#define RPVPS_CLIENT_OP(op,args)					\
	_ret = cli_##op args

#define END_RPVPS_CLIENT_OP						\
	ux_server_thread_unblocking();					\
	if (_ret != KERN_SUCCESS) {					\
		(void)nx_print_fatal_message(node, _id_string);		\
		if (_ret == MACH_RCV_TIMED_OUT ) {			\
			return(_rval);					\
		}							\
		panic("%s: bad rpc, ret=0x%x", _id_string, _ret);	\
	}								\
	return(_rval);							\
	}

#else	/* !NX */

#define START_RPVPS_CLIENT_OP(node,str) {				\
	char		*_id_string = str;				\
	int		_rval;						\
	mach_port_t	_server_port;					\
	int		_err;						\
	kern_return_t	_ret;						\
	_err = tnc_get_server_port(node, &_server_port);		\
	if (_err != ESUCCESS) {						\
		return (_err);						\
	}								\
	ux_server_thread_blocking()

#define RPVPS_CLIENT_OP(op,args)					\
	_ret = cli_##op args

#define END_RPVPS_CLIENT_OP						\
	ux_server_thread_unblocking();					\
	if (_ret != KERN_SUCCESS) 					\
		panic("%s: bad rpc, ret=0x%x", _id_string, _ret);	\
	return(_rval);							\
	}
#endif	/* NX */
/*
 * For asynchronous operations, send and receive variants of the above
 * macros also exist:
 */
/*
 * The destination macros include the reply port involved.
 */
#define RPVPS_DEST_SEND(node)		_server_port, _reply_port, &_rval
#define RPVPS_DEST_RECEIVE(node)	MACH_PORT_NULL, _reply_port, &_rval

/*
 * Different client MIG stubs are involved.
 */
#define RPVPS_CLIENT_OP_SEND(op,args)					\
	_ret = cli_##op##_msg_send args
#define RPVPS_CLIENT_OP_RECEIVE(op, args)				\
	_ret = cli_##op##_msg_receive args

/*
 * When sending, we allocate a reply port and return it to our caller
 * by way of the handle parameter. We don't declare that the thread will block.
 */
#define START_RPVPS_CLIENT_OP_SEND(node,handle,str) {			\
	char		*_id_string = str;				\
	int		_rval;						\
	mach_port_t	_server_port;					\
	mach_port_t	_reply_port;					\
	int		_err;						\
	kern_return_t	_ret;						\
	_err = tnc_get_server_port(node, &_server_port);		\
	if (_err != ESUCCESS) {						\
		return (_err);						\
	}								\
	_reply_port = mach_reply_port();				\
	*handle = (msg_handle_t) _reply_port

#define END_RPVPS_CLIENT_OP_SEND					\
	return(_ret);							\
	}

/*
 * When receiving, we use the reply port again given by the handle parameter,
 * and destroy it afterward.
 */
#define START_RPVPS_CLIENT_OP_RECEIVE(node,handle,str) {		\
	char		*_id_string = str;				\
	int		_rval;						\
	mach_port_t	_reply_port = (mach_port_t) *handle;		\
	kern_return_t	_ret;						\
	ux_server_thread_blocking()


#define END_RPVPS_CLIENT_OP_RECEIVE					\
	ux_server_thread_unblocking();					\
	(void) mach_port_destroy(mach_task_self(), _reply_port);	\
	if (_ret != KERN_SUCCESS)					\
		panic("%s: bad rpc, ret=0x%x", _id_string, _ret);	\
	return(_rval);							\
	}


/*
 * Include generated remote virtual process system operations (client side).
 * Each operation client stub is of the form:
 *	int r<op_name>(
 *		struct vproc *<parm_name_1>,
 *		<parm_type_2> <parm_name_2>,
 *			... repeated 
 *		<parm_type_n> <parm_name_n>)
 *	{
 *		START_RPVPS_CLIENT_OP(<parm_name_1>,"<op_name>");
 *		RPVPS_CLIENT_OP(r<op_name>(RPVPS_DESTINATION(node),
 *				<parm_name_2>,...,<parm_name_n>));
 *		END_RPVPS_CLIENT_OP;
 *	}
 *	Noting, in addition, that OUT parameters appear in the client RPC
 *	in the form "<parm_name> ? <parm_name> : (<parm_type> *)&dummy" to
 *	avoid null pointers being passed to the RPC. Furthermore, IN
 *	parameters of type struct vproc are transformed to be:
 *	"<parm_name> ? <parm_name>->vp_pid : 0".
 *
 */
#include <tnc/rvp_pvpsops_gen.c>
