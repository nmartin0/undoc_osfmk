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
 * $Log: rvp_pvpsops_server.c,v $
 * Revision 1.14  1995/02/01  21:51:59  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.13  1994/11/18  20:44:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/04/28  19:15:02  chrisp
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
 * Revision 1.11  1994/04/04  17:36:14  paul
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
 * Revision 1.10  1993/07/14  18:34:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  20:47:31  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/06/08  12:54:33  stefan
 * Reintroduced PVPSOP_TABLE_SET for static load leveling.
 *
 * Revision 1.8  1993/05/20  16:03:35  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.7  1993/05/06  19:24:53  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:47:06  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:22  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/25  23:26:21  cfj
 * T9 Merge.
 *
 * Revision 1.4.6.1  1993/03/24  23:42:29  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.2.4.2.1  1992/12/16  06:03:00  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/11/30  22:48:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.4  1992/11/13  17:01:46  cfj
 * Added RCS revision comments from Locus.
 *
 * Revision 1.1.2.3  1992/11/13  03:40:48  cfj
 * Fix rlogin crash when not on root_fs_node
 *
 * Revision 3.9  93/06/11  12:12:36  slively
 * Comment fix for SVR398 installation.
 * 
 * Revision 3.8  93/05/14  12:16:44  slively
 * [Bug 208] Remove the macros PRE and POST for the no longer valid 
 * RPVPSOP_TABLE_SET.
 * 
 * Revision 3.7  1993/02/22  17:20:08  mjl
 * Pass index argument to {PRE,POST}_RPVPSOP_TABLE_SIZE() for subtable support.
 *
 * Revision 3.6  92/12/01  11:34:53  chrisp
 * Add macros to perform dummy proc setup for reboot operation, take
 * 	the master lock for boot operation, and modify the standard
 * 	macros not to make a "RET =" assignment until immediately
 * 	returning because "rval" is not valid output parameter for the
 * 	asynchronous boot operation.
 * 
 * Revision 3.5  92/11/13  08:07:19  chrisp
 * [Bug #103] PVPSOP_RELEASE_CTTY() requires a modicum of uarea setup:
 * 	Rename [start|end]_rpvpsop_table() as a more general-sounding
 * 	[start|end]_rpvpsop_dummy_setup() and define PRE_ and POST_
 * 	operation macros to call these routines.
 * 
 * Revision 3.4  92/11/02  12:22:18  roman
 * Fix RCS comment from last submission.
 * 
 * Revision 3.3  92/10/28  15:51:12  roman
 * Change comments to reflect the "user exits" that are possble for
 *         special pvspop processing.
 * Add "user exit" code for table_get(), table_set(), and table_size() 
 * 	pvpsops to deal with out-of-line memory used by MiG and to
 * 	do appropriate uarea setup.
 * Change name of C file included with the "real" code to match the
 *         new file names used by makeTNCtables.sh.
 * 
 * Revision 3.2  92/10/08  17:14:21  chrisp
 * Contents gutted and replaced by #include of generated server stubs.
 * 
 * Revision 3.1  92/10/01  10:33:03  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.0  92/09/28  16:10:43  roman
 * Initial submission of file with server code for private virtual process
 * system operations for remote nodes.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/vproc.h>
#include <sys/tty.h>
#include <uxkern/import_mach.h>
#include <tnc/dpvproc.h>
#include <sys/user.h>
#include <kern/parallel.h>


/*
 * Macros defining the MIG interface - to make the source code more portable.
 */
#define DECL_RPVPS_SERVER_OP(op) int svr_##op(

#define DECL_RPVPS_DESTINATION						\
	mach_port_t	_server_port,					\
	int		*_rval

#define END_DECL							\
)

#define RET (*_rval)

#define PVPS_DESTINATION this_node

/*
 * Macros defining the generic interface between server stub and underlying
 * local private virtual process system operations.
 */
#define START_RPVPS_SERVER_OP(str)					\
	{								\
		char		*_id_string = str;			\
		int		_error = ESUCCESS

#define	RPVPS_SERVER_OP(op)						\
		if (!_error)						\
			_error = op

#define	LOCATE_VALID_VPROC_PID(pid, vp)					\
	if (pid != 0) {							\
		vp = LOCATE_VPROC_PID(pid);				\
		if (vp == NULL)						\
			_error = ESRCH;					\
	} else								\
		vp = NULL

#define	VALID_VPROC_RELEASE(vp)						\
	if (vp)								\
		VPROC_RELEASE(vp, _id_string)

#define END_RPVPS_SERVER_OP						\
		RET = _error;						\
		return(KERN_SUCCESS);					\
	}


/*
 * Include the server stubs generated automatically from header files.
 * Each operation server stub is of the form:
 *	DECL_RPVPS_SERVER_OP(r<op_name>)
 *		DECL_RPVPS_DESTINATION,
 *		<parm_type_2> <parm_name_2>,
 *			... repeated 
 *		<parm_type_n> <parm_name_n>
 *	END_DECL
 *	{
 *		START_RPVPS_SERVER_OP("<op_name>");
 *		RPVPS_SERVER_OP(<OP_NAME>(PVPS_DESTINATION,
 *				<parm_name_2>,...,<parm_name_n>));
 *		END_RPVPS_SERVER_OP;
 *	}
 *	IN parameters of type struct vproc are specially treated with the
 *	following transformations being performed:
 *	    o	the formal declaration becomes "<parm_name>_pid"
 *	    o	"LOCATE_VALID_VPROC_PID(<parm_name>_pid,<parm_name>)"
 *		appears before the RPVPS_SERVER_OP() invocation
 *	    o	"VALID_VPROC_RELEASE(<parm_name>_pid,<parm_name>)"
 *		appears afte the RPVPS_SERVER_OP() invocation
 *
 */
#include <tnc/rvp_pvpsops_server_gen.c>

