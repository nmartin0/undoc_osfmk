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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 *
 *
 *	This module contains the emulator pfs functions that are
 *	used to implement PFS iomodes.
 *
 * HISTORY
 * $Log: pfs_iomode.c,v $
 * Revision 1.31  1995/02/15  00:03:38  rlg
 * Fixed bug in the  e__set_local_iomode_info()  routine, which was introduced
 * with the PFS_ASYNC_DFLT enhancement.  The  fdte->flags)  field was being set
 * to zero without first making a call to token_release_to_server(), causing
 * the test case to hang.
 *
 *  Reviewer:  none
 *  Risk:  low
 *  PTS #12275
 *  Testing:  failing test case; pfs and fileio EATs
 *  Module(s):  pfs_iomode.c()
 *
 * Revision 1.30  1994/12/12  17:16:57  rlg
 * In  e__set_local_iomode_info(), if the  fdte->pfs_fd->p_use_token  global
 * variable is zero and it is going to be change to one, we first call the
 * routine token_release_to_server() if this process owns the file token.
 * An identical block of code was added to another section of code in this
 * routine for PTS #10771
 *
 *  Risk:  low
 *  Benefit or PTS #:  11808
 *  Testing:  pfs and fileio EATs
 *            iomode integration test
 *            rw performance test
 *            failing test case
 *
 *  Module(s):  emulator/pfs_iomode.c
 *
 * Revision 1.29  1994/11/18  20:24:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.28  1994/09/06  21:43:29  rlg
 * Two problems, described in the PTS reports listed below, were found
 * in two different routines in this module.
 *
 * In e__set_local_iomode_info(), a timing window was found in the
 * initialization code and after the fdte->pfs_iomode field was set to
 * the value of the input iomode argument and the call to release the
 * token.  If a server sent a 'token revoke' message before the call,
 * the state of the fdte was not consistent.
 *
 * In gop_close(), if a EMIXIO condition was detected in the M_RECORD
 * case the code would break out of the switch statement.  If this
 * happened then a call to pfs_rlse_token_mgr() was not made, resulting
 * in an inconsistent state.
 *
 * These problems were fixed for the R1.3 WW37 build.
 *
 *  Reviewer:          Dave Minturn
 *  Risk:              low
 *  Benefit or PTS #:  10708, 10709, 10714, 10810, and 10812
 *  Testing:           pfs and fileio EATs, failing test cases, iomode
 *                       integration test
 *  Module(s):         emulator/pfs_iomode.c
 *
 * Revision 1.27  1994/09/02  22:29:25  dbm
 * Fixed problems using M_ASYNC mode with mapped files.  These were
 * introduced by the changes for PTS #10568
 *  Reviewer: Bob Godley
 *  Risk:Low
 *  Benefit or PTS #:10771,10782
 *  Testing: Specific test case, PFS EATS.
 *  Module(s):
 * 	emulator/pfs_iomode.c
 *
 * Revision 1.26  1994/08/31  22:46:11  mtm
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
 * Revision 1.25.2.1  1994/08/19  22:48:21  dbm
 * Added support for a new bootmagic, PFS_ASYNC_DFLT, this allows setting
 * the default PFS I/O mode to M_ASYNC.
 *
 *  Reviewer:Bob Godley
 *  Risk:M
 *  Benefit or PTS #:10569
 *  Testing: Specific test cases. PFS EATS (With and without bootmagic set)
 *  Module(s):
 *
 *     (server)
 *         uxkern/boot_config.c
 *         uxkern/fsvr_server_side.c
 *         uxkern/fsvr.defs
 *     (emulator)
 *         emul_init.c
 *         fsvr_user_side.c
 *         pfs2_user_side.c
 *         pfs_iomode.c
 *         pfs_tokenmgt.c
 *         pfs_iomode.h
 *         pfs_fdt.h
 *     (libnx)
 *         _pfs_setio.c
 *         _setiomode.c
 *
 * Revision 1.25  1994/06/13  15:16:50  rlg
 * Added the M_ASYNC I/O mode for shared files.  This mode is characterized by:
 *     o	each node has a unique file pointer,
 *     o	nodes are not synchronized
 *     o	file access is unrestricted
 *     o	standard UNIX file sharing semantics requiring atomicity of I/O
 * 	are not preserved.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  medium
 *  Benefit or PTS #:  7480
 *  Testing:  I/O mode unit test; 132 Eval I/O tests; rw performance test;
 *  Module(s):  emulator/fsvr_user_side.c		libnx/_gopen.c
 * 		      pfs2_user_side.c		      _pfs_setio.c
 * 		      pfs_iomode.c		      _setiomode.c
 * 		      pfs_iomode.h		      gopen.c
 * 		      pfs_tokenmgt.c		      gopen_.c
 * 		      pfs_user_side.c		      pfs_iomode.h
 * 						      setiomode.c
 *
 * Revision 1.24  1994/05/18  18:20:39  rlg
 * In the pfs_rlse_fdte() routine, the mach_port_deallocate() was being used
 * to deallocate all the ports associated with an fdte.  However, the ports
 * were not destroyed.  In this routine the four calls to mach_port_deallocate()
 * were changed to mach_port_destroy().
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #:  PTS #9176
 *  Testing:  Failing test case; pfs and fileio EATs
 *  Module(s):  emulator/pfs_iomode.c
 *
 * Revision 1.23  1994/03/29  17:47:34  rlg
 * Merged the changes from 1.14.2.6 on the R1.2 branch into R1.3.
 *
 * Revision 1.22  1994/03/21  18:19:18  rlg
 * Merged the changes in revision 1.14.2.5 (R1.2) into the trunk.
 *
 * Revision 1.21  1994/03/04  22:24:09  dbm
 * Mainline merge for bug 6919, (1.2 rev 1.14.2.4)
 *
 * Revision 1.20  1994/02/16  00:35:39  dbm
 * Checkin of merge from 1.2 branch, revision 1.14.2.3
 *
 * Revision 1.19  1994/02/04  19:46:43  brad
 * Modified extended math support so that: 1) Emath routines set a new
 * error parameter instead of relying on a return value of -1 on overflow.
 * The latter method did not handle valid return values of -1 (this caused
 * eseek with resulting offset of -1 to return EQESIZE instead of EINVAL,
 * for example).  2) The emath code can be reused by libesize.a and libnx.a,
 * instead of having multiple copies of the same code in different places.
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #:
 *  Testing: Ran PFS EATs, ran emath tests.
 *  Module(s): fsvr_user_side.c pfs2_user_side.c pfs_emath.c pfs_fdt.h
 *             pfs_iomode.c pfs_tokenmgt.c pfs_user_side.c
 *
 * Revision 1.18  1994/01/26  19:16:17  dbm
 * Added some missing log entries.
 *
 * Revision 1.17  1994/01/06  23:52:06  dbm
 *  Reviewer: None.
 *  Risk: Low
 *  Benefit or PTS #:Merged fix from 1.2 of bug #5686.  This was originally
 * 		  checked in as 1.14.2.1.
 *  Testing: See 1.14.2.1 entry.
 *  Module(s):
 * 	pfs_iomode.c
 * 	pfs_tokenmgt.c
 * 	fsvr_user_side.c
 *
 * Revision 1.16  1994/01/05  17:09:00  brad
 * Fixed lint warnings in PFS-related code.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Some PFS source now passes lint
 *  Testing: Ran PFS EATs
 *  Module(s): emulator/emul_callback.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_fdt.h
 *             emulator/pfs_iomode.c
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.15  1993/12/20  19:07:49  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: remove meaningless casts to keep 'lint' happy
 *  Testing: builds
 *  Module(s):
 *
 * Revision 1.14.2.6  1994/03/29  16:15:44  rlg
 * The warning messages from lint were evaluated and corrections made as
 * required.
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #:  7719
 *
 * Revision 1.14.2.5  1994/03/21  17:54:47  rlg
 * Added calls to  mach_port_request_notification()  when the syncin and
 * datain ports are allocated.  This change is in conjunction with checks
 * for a MACH_NOTIFY_NO_SENDERS response from calls to  mach_msg().
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  medium
 *  Benefit or PTS #:  8431
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  emulator/pfs_msgutil.c [rcv_iomode_msg(), rcv_iomode_token(),
 *                                      rcv_global(), rcv_global_vec()],
 *              emulator/pfs_iomode.c  [pfs_init_co()]
 *
 * Revision 1.14.2.4  1994/03/04  21:42:24  dbm
 * Added emul_blocking/unblocking around bsd_task_by_pid() to handle
 * task suspends.
 *  Reviewer:Brad Rullman
 *  Risk:M
 *  Benefit or PTS #:6919
 *  Testing:PFS EATS, Overlapping PFS Sats.
 *  Module(s):
 * 	pfs_iomode.c
 *
 * Revision 1.14.2.3  1994/02/15  23:51:34  dbm
 * Made modifications to allow M_RECORD mode to work correctly with FAPPEND
 * mode and also added missing fdte_lock()'s around get setiomode() function
 * calls.
 *  Reviewer:Bob Godley
 *  Risk: Low
 *  Benefit or PTS #:8049, 7906
 *  Testing: Specific test cases, ran PFS Eats several times.
 *  Module(s):
 *      pfs_iomode.c
 *
 * Revision 1.14.2.2  1994/01/09  00:19:53  brad
 * Fixed bug found by lint (wrong number pf params to pfs_free() in
 * pfs2_user_side.c); also fixed lint warnings in PFS-related code.
 *
 * Revision 1.14  1993/09/21  22:00:13  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.13  1993/09/08  21:00:09  dbm
 * Added PFS_TOKENMGT macro to dissable M_RECORD mode for UFS files.
 *
 * Revision 1.12.6.2  1993/09/21  21:52:25  cfj
 * Workaround for PTS #6422.  Reduced usage of bsd_task_by_pid() rpc
 * to avoid race condition.
 *
 * Revision 1.12.6.1  1993/09/07  22:09:16  cfj
 * Added use of PFS_TOKENMGT() to dissable PFS M_RECORD mode for UFS files.
 * (Bug #6499) dbm@ssd.intel.com
 *
 * Revision 1.12  1993/07/29  16:37:21  dbm
 * Put in fixes for PTS 5672 bug.  Made sure file was in correct state
 * when being closed in M_RECORD mode.
 *
 * Revision 1.11  1993/07/27  19:57:05  dbm
 * Fixed a bug in M_RECORD mode that was causing a token to be revoked
 * during closing of the file.
 *
 * Revision 1.10  1993/07/21  21:44:23  dbm
 * Fixed problem with M_RECORD mode hanging on close.
 *
 * Revision 1.9  1993/07/16  03:02:50  dbm
 * Added token optimization functionality.
 *
 * Revision 1.8  1993/06/16  20:33:34  dbm
 * Changed all references to pfs_iomode to pfs_iomode_info to allow single
 * node applications to obtain the PFS I/O mode info.
 *
 * Revision 1.7  1993/06/07  18:49:24  dbm
 * Removed check for setting I/O modes on mapped/pfs files.  This was causing
 * the I/O modes to not work on NFS files.
 *
 * Revision 1.6  1993/05/11  01:02:30  brad
 * Changed occurrences of fdte->mappable to check fdte->iomode.
 *
 * Revision 1.5  1993/05/06  20:15:21  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:18:31  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.3  1993/03/15  18:50:06  cfj
 * Merge with T9.
 *
 * Revision 1.2.8.2  1993/03/15  18:42:56  cfj
 * Fix problem where sync close block CFS emulation.
 *
 * Revision 1.1.2.2.2.13  1993/03/11  23:39:36  dbm
 * Added check for file type in PFS I/O modes.
 *
 * Revision 1.1.2.2.2.12  1993/03/10  06:31:48  brad
 * Replaced _ecmp() usage with GREATER, LESS, EQUAL macros.
 *
 * Revision 1.2.8.1  1993/02/24  23:21:01  cfj
 * Fixes for bugs #4122 and #3314.
 *
 * Revision 1.1.2.2.2.11  1993/02/12  17:13:03  dbm
 * Added M_GLOBAL I/O modes functionality.  Made some fixes to lseek.
 *
 * Revision 1.2  1992/11/30  22:09:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2.2.10  1993/02/09  22:38:56  brad
 * Changed fsvr_pfs_fstat() to fsvr_pfs_fdevstat, due to a name clash
 * with the PFS asynchronous RPC stubs.
 *
 * Revision 1.1.2.2.2.9  1993/02/04  00:38:56  brad
 * Changed function name to pfs_set_stripefile_offsets(), for consistency.
 *
 * Revision 1.1.2.2.2.8  1993/01/20  21:25:51  brad
 * Removed extended math functions that are now defined in pfs_emath.c.
 *
 * Revision 1.1.2.2.2.7  1993/01/14  20:29:00  dbm
 * Added checks for the file port being MACH_PORT_NULL in PFS I/O modes to
 * support the gopen() function.
 *
 * Revision 1.1.2.2.2.6  1993/01/11  17:29:51  dbm
 * Added changes to support PFS files with I/O modes.
 *
 * Revision 1.1.2.2.2.5  1992/12/22  02:26:04  dbm
 * Changed parameter order on file_token_release() function to make it
 * consistent with the file_token_acquire() function.
 *
 * Revision 1.1.2.2.2.4  1992/12/14  22:56:16  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.1.2.2.2.3  1992/12/11  21:04:33  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.1.2.2.2.2  1992/12/03  00:15:44  dbm
 * Updated for pfs i/o mode information in the fdte entry and common token
 * functions.
 *
 * Revision 1.1.2.2.2.1  1992/11/25  23:01:20  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.3  1992/11/25  02:48:50  dbm
 * Added changes to support mapped files with PFS I/O modes.
 *
 * Revision 1.1.2.2  1992/11/13  18:22:03  cfj
 * Fixup number of params to calls to isc_deregister.
 *
 * Revision 1.1.2.1  1992/11/10  16:34:23  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:16:07  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.1  1992/10/22  15:14:19  dbm
 * New for PFS functionality.
 *
 * Revision 1.3  92/08/06  17:58:50  brad
 * Added #ifdef PFS wrapper.
 * 
 * Revision 1.2  92/08/04  16:38:17  dbm
 * Added standard function headers to all of the function and fixed up
 * the comments so that they looked like OSF style.
 * 
 * Revision 1.1  92/07/31  12:11:35  brad
 * Initial revision
 * 
 *
 */
#ifdef	PFS

#include <mach_init.h>
#include <mach/mig_errors.h>
#include <mach/thread_switch.h>
#include <uxkern/fsvr.h>
#include <../server/include/varargs.h>
#include <sys/stat.h>
#include <sys/estat.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <machine/vmparam.h>
#include <cthreads.h>
#include <machine/psl.h>
#include "emul_stack.h"
#include "emul.h"
#include "fdt.h"
#include "pfs_iomode.h"
#ifdef MAP_UAREA
#include <sys/ushared.h>
extern int shared_enabled;
extern spin_lock_t in_emulator_lock;
extern struct ushared_ro *shared_base_ro;
extern struct ushared_rw *shared_base_rw;
#endif

extern fdt_slot_t	fdt[NOFILE];		/* file descriptor table */
extern int 		fdt_lastfile;		/* HWM of fdt */
extern spin_lock_t 	fdt_lock;		/* lock protecting fd table */	
extern int		edebug;
extern esize_t	 	ex_neg_one;
extern boolean_t	stack_init_done;
extern boolean_t	must_suspend;

extern void		ref_file();
extern void		unref_file();
extern int		pfs_init_token_mgr();
extern int		pfs_conn_token_mgr();
extern void		pfs_rlse_token_mgr();
extern int		pfs_async_dflt;

/*
 * Forward Declarations:
 */
int	gop_syncoff(), gop_reset_iomode(), gop_close();
int	gop_lseek(), gop_iseof(), gop_max();
void	pfs_token_mgr();
void	pfs_rlse_fdte();



/*
 * NAME:	e__get_local_iomode_info
 *
 * DESCRIPTION:
 *              This function is used to return the local iomode
 *		information associated with a file descriptor.
 *
 *
 * PARAMETERS:
 *              bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A descriptor representing the open file.
 *
 *		pfs_setio_info:	A pointer to the iomode information 
 *				structure that will be filled in by
 *				this function.
 *
 *		fp_info_size:	A pointer used to return the 
 *				size required for the pfs_fp_info_t
 *				structure to be used in subsequent
 *				set iomode calls.
 *
 *		rval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either:
 *				ESUCCESS 	- if successful 
 *				error number 	- if an error occurred.
 *
 * RETURNS:
 *	
 *	ESUCCESS 	-	if successful
 *	error number 	- 	if an error occurred.
 *
 */
int
e__get_local_iomode_info(bsd_serv_port, interrupt, fildes, pfs_setio_info,
			 fp_info_size, rval)
	mach_port_t		bsd_serv_port;
	boolean_t		*interrupt;
	int			fildes;
	pfs_setio_info_t	*pfs_setio_info;
	int			*fp_info_size;
	int			*rval;
{
	int			error = ESUCCESS;
	fdt_entry_t		*fdte;
        struct pfs_stat 	hdr_stat;
	esize_t			length;

	/*
	 * Make sure fdes is valid:
	 */
        if (error = fdt_ref_entry(fildes, &fdte)){
                return(error);
	}

	/*
	 * Retrieve the file device and ionode information from 
	 * the server.
	 */

	error = pfs_fdevstat_internal(interrupt, fdte->fp, &hdr_stat);
	if (error) {
		return (error);
	} 


	/*
	 * Make sure the file is a regular file:
	 */
	if (S_ISREG(hdr_stat.pst_mode)) {
		pfs_setio_info->node_id		= hdr_stat.pst_node;
		pfs_setio_info->device_id	= hdr_stat.pst_dev;
		pfs_setio_info->inode_id	= hdr_stat.pst_ino;
	} else {
		error = EINVAL;
		goto out;
	}

	/*
	 * The remainder of the information is local:
	 */
	pfs_setio_info->iomode = fdte->pfs_iomode;

	*fp_info_size = sizeof(pfs_fp_info_t);
	if (fdte->pfs_fd) {
		/* 
		 * Compute the total number of fp's 
		 * associated with the file.
		 */
		*fp_info_size += 
			(fdte->pfs_fd->p_stripe_factor * sizeof(mach_port_t));

	} 

	error = file_token_acquire (fdte,
				    interrupt,
				    PFS_OP_OFFSET,
				    0,
				    &pfs_setio_info->offset,
				    &length);

out:
        (void) fdt_unref_entry(fdte);
	return (error);
}



/*
 * NAME:	e__set_local_iomode_info
 *
 * DESCRIPTION:
 *              This function is used to setup the local iomode
 *		information.
 *
 * PARAMETERS:
 *              bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A descriptor representing the open file.
 *
 *		my_node_number: My logical node number.
 *
 *		number_of_nodes: Number of nodes in application.
 *
 *		comm_info:	A pointer to the communications iomode 
 *				information 
 *
 *		fp_info:	A pointer to where the common file pointer 
 *				information will be stored.
 *
 *		rval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be:
 *				ESUCCESS 	- if successful 
 *				error number 	- if an error occurred.
 *
 * RETURNS:
 *	
 *	ESUCCESS 	-	if successful
 *	error number 	- 	if an error occurred.
 *
 */
int
e__set_local_iomode_info(bsd_serv_port, interrupt, fildes, iomode, 
			 my_node_number, number_of_nodes, comm_info, fp_info,
			 rval)
	mach_port_t		bsd_serv_port;
	boolean_t		*interrupt;
	int			fildes;
	int			iomode;
	int			my_node_number;
	int			number_of_nodes;
	pfs_comm_info_t		*comm_info;
	pfs_fp_info_t		*fp_info;
	int			*rval;
{
	int		error = ESUCCESS;
	fdt_entry_t     *fdte;
	esize_t			curr_offset,curr_length;

	/*
	 * Make sure fdes is valid:
	 */
        if (error = fdt_ref_entry(fildes, &fdte)){
                return(error);
	}

	/*
	 * Keep the fdte lock until we are sure that this process no
	 * longer holds the file token.  This guarantees that the fdte
	 * is in a good state should we receive a 'token release message'
	 * from the server during this initialization step.
	 */
	fdte_lock(fdte);

	fdte->pfs_iomode = iomode;

	if ((iomode != M_ASYNC) && 
	    ((number_of_nodes == 1) || (iomode == M_UNIX))){
		if (fdte->pfs_fd) {

			if (!(fdte->pfs_fd->p_use_token)) {
				/*
		 		 * We have to release any token that we had
				 * acquired earlier since ...
				 * 
				 * 
		 	 	 */
                		if (fdte->flags) {
                        		/*
                         	 	 * Release the token.
                         	 	 */
                        		token_release_to_server(fdte, FALSE);
				}
			}

			fdte->pfs_fd->p_use_token = 1;
		}

		fdte_unlock(fdte);

	} else if (iomode == M_ASYNC) {

		if (fdte->pfs_fd) {		/* PFS File */
		       /*
		 	* If the token was acquired earlier and we are in the
		 	* use_token mode, then the token needs
		 	* to be released back to the server prior to 
		 	* setting the I/O mode into M_ASYNC.
		 	*/
			if (fdte->pfs_fd->p_use_token) {
				/*
		 		 * We have to release any token that we had
				 * acquired earlier since from now on we will
				 * not be doing any more regular token acquires
				 * or releases.
		 	 	 */
                		if (fdte->flags) {
                        		/*
                         	 	 * Release the token.
                         	 	 */
                        		token_release_to_server(fdte, FALSE);
				}
			}
			fdte->pfs_fd->p_use_token = 0;

		} else { 	/* Non-PFS file */
			/*
			 * For Non-PFS files we will act the 
			 * same as M_UNIX mode.
			 */
		}

		fdte_unlock(fdte);

	} else {
		/*
		 * Go through here for M_LOG/M_SYNC/M_RECORD/M_GLOBAL.
		 */

		if (my_node_number != 0) {
			/*
			 * First, if this in not node 0, then make sure
			 * that this node does not hold the token:
			 */
			if (fdte->flags) {
				/*
				 * Release the token.
				 */
              	         	token_release_to_server(fdte, FALSE);
			}
		}

		fdte_unlock(fdte);


		if (fdte->pfs_fd && pfs_async_dflt) { 
			/*
			 * If the bootmagic pfs_async_dflt is set check the 
			 * value of the token flags to make sure they 
			 * indicate no token held.  This is needed because
			 * just the offset was obtained earlier.
			 */
			fdte_lock(fdte);
			if (fdte->flags) {
				token_release_to_server(fdte, FALSE);
				fdte->flags = 0;	
			}
			fdte_unlock(fdte);
			fdte->pfs_fd->p_use_token = 1;
		}

		/*
 		 * Initialize the pfs_iomode entry in the fdte:
		 */
		error = pfs_init_fdte(fdte, my_node_number, number_of_nodes);
		if (error != ESUCCESS) {
			goto err_out;
		}

		/*
 		 * Initialize the nearest neighbor communication.
		 */
		error = pfs_init_mach_com(fdte, IOMODE_COM_NEIGHBOR, comm_info,
					  interrupt);

		if (error != ESUCCESS) {
			goto err_out;
		}
		
		if (iomode == M_GLOBAL) {
			/*
			 * Initialize the spanning tree:
			 */
			error = pfs_init_mach_com(fdte, IOMODE_COM_SPANNING, 
						  comm_info, interrupt);

			if (error != ESUCCESS) {
				goto err_out;
			}
		}

		/*
		 * Initialize the common file pointer info.
		 */
		error = pfs_init_fp_info(fdte, fp_info);

		if (error != ESUCCESS) {
			goto err_out;
		}
		/*
		 * Put the memory object into the fp_info:
	 	*/
		fp_info->mem_obj = fdte->mem_obj;

		/*
		 * Initialize the iomode manager.
		 */

		error = pfs_init_token_mgr(fdte, fp_info);

		if (error != ESUCCESS) {
			goto err_out;
		}

		if (my_node_number == 0) {

			/*
		 	 * Obtain the token for the file, if not
		 	 * already obtained.  The main thing is to
			 * take the token away from all of the other
			 * nodes so that the file ports and token 
			 * information can be correctly setup.
			 */
			if ((error = file_token_acquire(fdte, 
							interrupt, 
							PFS_OP_TOKMGR, 
							0, 
							&curr_offset, 
							&curr_length)) < 0) {
				return error;
			}

			if (PFS_TOKENMGT(fdte)) {
				/*
				 * If sharing token, then need to check to
				 * the mode to determine where our offset 
				 * should start.  Offset will be set to 
				 * EOF if WRITE and APPEND mode. 
				 */
				if ((fdte->fmode & FAPPEND) &&
				    (fdte->fmode & FWRITE)) {
					/* 
					 * Update the offset to the end of 
					 * the file.
				         */
					if (fdte->pfs_fd) {
					  error = pfs_set_stripefile_offsets(
								  fdte,
								  curr_length);
					  if (!error) {
					    fdte->pfs_fd->p_offset = 
								curr_length;
					  }
					} else {
						fdte->offset = curr_length.slow;
					}
				}
			} else {
				/*
				 * Only hold onto the token if sharing with
				 * other nodes in the application.  
				 */
				file_token_release(fdte,
						   interrupt,
						   PFS_OP_TOKMGR,
						   0,
						   &ex_neg_one,
						   &ex_neg_one);
			}
		}
	}
        (void) fdt_unref_entry(fdte);
	return error;

err_out:
        (void) fdt_unref_entry(fdte);
	(void) pfs_rlse_fdte(fdte);
	return error;
}


/*
 * NAME:	e__set_global_iomode_info
 *
 * DESCRIPTION:
 *              This function is used to setup the the local iomode
 *		information associated with a file descriptor.
 *
 * PARAMETERS:
 *              bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A descriptor representing the open file.
 *
 *		comm_info:	The array of global communication information 
 *				for this file descriptor.
 *
 *		fp_info:	The array of global file table information.
 *
 *		fp_info_size:	The size (in bytes) of each array element of
 *				the fp_info array.
 *
 *		rval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be:
 *				ESUCCESS 	- if successful 
 *				error number 	- if an error occurred.
 *
 * RETURNS:
 *	
 *	ESUCCESS 	-	if successful
 *	error number 	- 	if an error occurred.
 *
 */
int
e__set_global_iomode_info(bsd_serv_port, interrupt, fildes, comm_info, fp_info,
			  fp_info_size, rval)
	mach_port_t		bsd_serv_port;
	boolean_t		*interrupt;
	int			fildes;
	pfs_comm_info_t		comm_info[];
	pfs_fp_info_t		fp_info[];
	int			fp_info_size;
	int			*rval;
{
	int		error = 0;
	fdt_entry_t     *fdte;
	int		new_iomode;
	pfs_iomode_t	*iomode_p;
	mach_port_t	task0_port = MACH_PORT_NULL;

	/*
	 * Make sure fdes is valid:
	 */
        if (error = fdt_ref_entry(fildes, &fdte)){
                return(error);
	}

	/*
	 * Get the task port for process 0:
	 */

	emul_blocking();
	bsd_task_by_pid(our_bsd_server_port,
			interrupt,
			comm_info[0].unix_pid,
			&task0_port);
	emul_unblocking();

	iomode_p = fdte->pfs_iomode_info;
	new_iomode = fdte->pfs_iomode;

	if (iomode_p) {
		/*
		 * Finish connecting up the inter-emulator communications.
		 */
		error = pfs_connect_mach_com(fdte, IOMODE_COM_NEIGHBOR,
					     comm_info, interrupt);

		if (error != ESUCCESS) {
#ifdef DEBUG_SETIO
			e_printf("Error from pfs_connect_mach_com\n");
#endif
		
			goto err_out;
		}

		if (new_iomode == M_GLOBAL) {
			error = pfs_connect_mach_com(fdte, IOMODE_COM_SPANNING,
						     comm_info, interrupt);
			if (error != ESUCCESS) {
#ifdef DEBUG_SETIO
			        e_printf("Error from pfs_connect_mach_com\n");
#endif
				goto err_out;
			}
		}

		/*
		 * Connect up the shared file port if needed:
		 */
		if ((new_iomode == M_LOG) || (new_iomode == M_SYNC) ||
		    (new_iomode == M_GLOBAL)) {
			error = pfs_connect_fp_info(fdte, fp_info, 
						    task0_port,
						    interrupt);
			if (error != ESUCCESS) {
#ifdef DEBUG_SETIO
				e_printf("Error from pfs_connect_fp_info\n");
#endif
				goto err_out;
			}
		}

		if (fdte->iomode == VIO_MAPPED) {
			error = pfs_conn_mem_obj(fdte, fp_info, task0_port);
			if (error != ESUCCESS) {
			        goto err_out;
			}
		}

		/*
		 * Connect up the iomode manager:
		 */
		error = pfs_conn_token_mgr(fdte, fp_info, fp_info_size,
					   task0_port, comm_info);	
		if (error != ESUCCESS) {
#ifdef DEBUG_SETIO
			e_printf("Error from pfs_conn_token_mgr\n");
#endif
			goto err_out;
		}
	}
	if ((PFS_TOKENMGT(fdte)) && (fdte->fmode & FAPPEND) && 
	                            (fdte->fmode & FWRITE)) {
		/*
		 * Need to adjust offsets to new token value.
		 */
		if (fdte->pfs_fd) {
			  error = pfs_set_stripefile_offsets(fdte, 
						     fdte->pfs_fd->p_length);
			if (!error) {
				fdte->pfs_fd->p_offset = fdte->pfs_fd->p_length;
			}
		} else {
			fdte->offset = fdte->length;
		}
	}
        (void) fdt_unref_entry(fdte);
	/*
	 * Free up the task ports:
	 */
	if (task0_port != MACH_PORT_NULL) {
		mach_port_deallocate(mach_task_self(),
				     task0_port);
	}
	return (error);

err_out:
        (void) fdt_unref_entry(fdte);
	(void) pfs_rlse_fdte(fdte);
	if (task0_port != MACH_PORT_NULL) {
		mach_port_deallocate(mach_task_self(), task0_port);
	}
	return error;
}


/*
 * NAME:	e__release_iomode_info
 *
 * DESCRIPTION:
 *              This function is used to release any iomode 
 *		resources for the specified file descriptor.
 *		These resources may be Mach port references.
 *
 * PARAMETERS:
 *              bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A descriptor representing the open file.
 *
 *		comm_info:	The communication information 
 *				for this file descriptor.
 *
 *		fp_info:	The file table information.
 *
 *		rval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either:
 *				ESUCCESS 	- if successful 
 *				error number 	- if an error occurred.
 *
 * RETURNS:
 *	
 *	ESUCCESS 	-	if successful
 *	error number 	- 	if an error occurred.
 *
 */
int
e__release_iomode_info(bsd_serv_port, interrupt, fildes, comm_info, fp_info, rval)
	mach_port_t		bsd_serv_port;
	boolean_t		*interrupt;
	int			fildes;
	pfs_comm_info_t		*comm_info;
	pfs_fp_info_t		*fp_info;
	int			*rval;
{
	int			error=ESUCCESS;
	fdt_entry_t		*fdte;
	/*
	 * Make sure fdes is valid:
	 */
        if (error = fdt_ref_entry(fildes, &fdte)){
                return(error);
	}

	if (fdte->pfs_iomode_info) {
		(void) pfs_rlse_fdte(fdte);
	}
        (void) fdt_unref_entry(fdte);
	return ESUCCESS;
}


/*
 * NAME:	e__reset_iomode_info
 *
 * DESCRIPTION:
 *              This function is used when switching from one i/o mode
 *		to another.  The mach resources used for the previous
 *		iomode are released.  
 *
 * PARAMETERS:
 *              bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A descriptor representing the open file.
 *
 *		new_mode:	The new iomode for the file descriptor.
 *
 *		iomode_info:	A pointer to the iomode information 
 *				structure.
 *
 *		rval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either:
 *				ESUCCESS 	- if successful 
 *				error number 	- if an error occurred.
 *
 * RETURNS:
 *	
 *	ESUCCESS 	-	if successful
 *	error number 	- 	if an error occurred.
 *
 */
int
e__reset_iomode_info(bsd_serv_port, interrupt, fildes, new_mode, rval)
	mach_port_t		bsd_serv_port;
	boolean_t		*interrupt;
	int			fildes;
	int			new_mode;
	int			*rval;
{
	int			error = ESUCCESS;
	pfs_iomode_t		*iomode_p;
	RESETIO_DATA		rio_data;
	transaction_id_t 	trans_id;
	mach_port_t		new_fp;
	fdt_entry_t		*fdte;
	int			old_mode;

	/*
	 * Make sure fdes is valid:
	 */

        if (error = fdt_ref_entry(fildes, &fdte)){
                return(error);
	}
#ifdef DEBUG_RESETIO
	e_printf("reset_iommode: fdte->lock = %d, fdte->refcnt = %d\n",
		fdte->lock, fdte->refcnt);
#endif

	iomode_p	= fdte->pfs_iomode_info;

        /*
         * See if we need to reset the state of the token:
         */
        if (fdte->pfs_fd && fdte->pfs_iomode == M_ASYNC) {
                fdte_lock(fdte);
                if (fdte->flags) {
                        /*
                         * Release the token.
                         */
                        EASSERT(!fdte->must_release);
                        token_release_to_server(fdte, FALSE);
                }
                fdte_unlock(fdte);
                fdte->pfs_fd->p_use_token = 1;
		fdte->pfs_iomode = M_UNIX;
        	(void) fdt_unref_entry(fdte);
		return ESUCCESS;
        }

	if (iomode_p == NULL) { 
		/*
		 * Single node, just reset the iomode value and
		 * return. 
		 */
		fdte->pfs_iomode = M_UNIX;
        	(void) fdt_unref_entry(fdte);
		return ESUCCESS;
	}


	/*
	 * Sync everyone up first:
	 */
	if ((fdte->pfs_iomode_info->my_node_number == 0) &&  
	    (fdte->pfs_iomode == M_LOG)) {
		/*
		 * Make sure node 0 has the token:
		 */
		if (error = file_token_acquire(fdte, 
					       interrupt,
					       PFS_OP_OFFSET,
					       0,
					       &rio_data.offset,
					       &rio_data.length)){ 

		}
	} else {
		/*
		 * Current offset's and length already obtained by
		 * the get_local_iomode_info call.
		 */
		if (fdte->iomode == VIO_MAPPED) {
			rio_data.offset.shigh = 0;
			rio_data.offset.slow = fdte->offset;
			rio_data.length.shigh = 0;
			rio_data.length.slow = fdte->length;
		} else if (fdte->pfs_fd) {
			rio_data.offset = fdte->pfs_fd->p_offset;
			rio_data.length = fdte->pfs_fd->p_length;
		} else {
			/*
			 *  Non-Mapped or NFS file:
			 */
			error = pfs_get_offlen(fdte, &rio_data.offset, 
					       &rio_data.length);
			if (error) {
				goto err_out;
			}
		}
	}

	if (error = giomode_op(fdte, gop_reset_iomode, &rio_data)) {
		goto err_out;
	} 
#ifdef DEBUG_RESETIO
	e_printf("reset_iommode, after giomode_op: fdte->lock = %d, fdte->refcnt = %d\n",
		fdte->lock, fdte->refcnt);
#endif

	old_mode = fdte->pfs_iomode;

	/*
	 * Release the mach communication resources:
	 */
	(void) pfs_rlse_fdte(fdte);
	iomode_p = NULL;
#ifdef DEBUG_RESETIO
	e_printf("reset_iommode, after giomode_op: fdte->lock = %d, fdte->refcnt = %d\n",
		fdte->lock, fdte->refcnt);
#endif

	/*
	 * Check for switch to iomode 0, 3, or 5 (No common file pointer):
	 */
	if (((new_mode == M_UNIX)     || 
	     (new_mode == M_RECORD)   ||
	     (new_mode == M_ASYNC)) && 
	    ((old_mode == M_LOG)  || 
	     (old_mode == M_SYNC) || 
	     (old_mode == M_GLOBAL))) {

		/*
		 * We need to make the common file pointer into
		 * individual file pointers as if everyone had
		 * opened the file and set the file positions.
		 */
        	isc_register(fdte->fp, &trans_id);
        	if (error = fsvr_unshare(fdte->fp,
					 credentials_port,
					 trans_id,
					 &new_fp)) {

	       		isc_deregister(interrupt);
			goto err_out;
		}
	       	isc_deregister(interrupt);

		/*
		 * See if the token needs to be released:
		 */
		fdte_lock(fdte);
                if (fdte->flags) {
                       	/*
			 * Release the token.
			 */
                       	EASSERT(!fdte->must_release);
                       	token_release_to_server(fdte, FALSE);
		}
		fdte_unlock(fdte);

		/*
		 * Release the old file port:
		 */
		unref_file(&fdte->fp);

		/*
		 * Assign the reference to the new port.
		 */

		fdte->fp = new_fp;	/* Store the new file port. */

		/*
		 * See if file is a pfs file, if so then we
		 * need to create an individual file pointer
		 * for each of the stripe files as well.
		 */

		if (fdte->pfs_fd) {
               		int sfactor = fdte->pfs_fd->p_stripe_factor;
                	int sfd;
			mach_port_t 	*fp;
                	for(sfd = 0; sfd < sfactor; sfd++) {
                                fp = &fdte->pfs_fd->p_stripe_fdt[sfd].s_fp;
        			isc_register(*fp, &trans_id);
        			if (error = fsvr_unshare(*fp,
                   	        			 credentials_port,
                       		    			 trans_id,
                           			         &new_fp)) {

	       				isc_deregister(interrupt);
					goto err_out;
				}
	       			isc_deregister(interrupt);

				unref_file(fp);

                               	*fp = new_fp;
			}
                }
	}
#ifdef DEBUG_RESETIO
	e_printf("reset_io: fdte->refcnt = %d\n, fdte->lock= %d\n",
		fdte->refcnt, fdte->lock);
#endif
        (void) fdt_unref_entry(fdte);
 	return ESUCCESS;

err_out:
	if (iomode_p) {
		(void) pfs_rlse_fdte(fdte);
	}
       	(void) fdt_unref_entry(fdte);
	return error;
}


/*
 * NAME:	pfs_init_fdte
 *
 *
 * DESCRIPTION:
 *		This function is used to initialize the pfs_iomode
 *		entry in the fdte entry.  This function is called
 *		whenever a new non M_UNIX iomode is established.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor entry.
 *		
 *		iomode		New iomode value.
 *
 *		my_node_number	My task's logical node number.
 *
 *		number_of_nodes Number of nodes in the ptype.
 *
 *
 * RETURNS:
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_init_fdte(fdte, my_node_number, number_of_nodes) 
fdt_entry_t     *fdte;
int 		my_node_number;
int 		number_of_nodes;
{
	int error = ESUCCESS;
	int i;

	/*
	 * Allocate the iomode structure:
	 */
	fdte->pfs_iomode_info = (pfs_iomode_t *)malloc(sizeof(pfs_iomode_t));
	if (fdte->pfs_iomode_info == NULL) {
		error = ENOMEM;
	} else { 
		fdte->pfs_iomode_info->tmgr_id_port     = MACH_PORT_NULL;
		fdte->pfs_iomode_info->tmgr_req_port    = MACH_PORT_NULL;
		fdte->pfs_iomode_info->revoke_port      = MACH_PORT_NULL;
		fdte->pfs_iomode_info->my_node_number 	= my_node_number;
		fdte->pfs_iomode_info->number_of_nodes 	= number_of_nodes;
		fdte->pfs_iomode_info->syncin_port	= MACH_PORT_NULL;
		fdte->pfs_iomode_info->syncout_port	= MACH_PORT_NULL;
		fdte->pfs_iomode_info->datain_port	= MACH_PORT_NULL;

		for (i=0; i<IOMODE_MAXPORT; i++) {
			fdte->pfs_iomode_info->dataout_ports[i] = MACH_PORT_NULL;
		}
	}
	return (error);
}


/*
 * NAME:	pfs_init_mach_com
 *
 *
 * DESCRIPTION:
 *		This is a local emulator function which is used to 
 *		create the mach communication channels needed to
 *		communicate iomode information.  The mach communication
 *		channels consist of Mach ports which are used to 
 *		exchange information between nodes.  Each node 
 *		attempts to allocate a set of local ports starting
 *		at a know name space,(PFS_MACH_PORT_BASE).  The
 *		ports used for communication out to other nodes are
 *		allocated on an odd namespace value where as the ports
 *		for communication from other nodes are allocated
 *		on an even namespace value.
 *
 * PARAMETERS:
 *		fdte		File descriptor entry.
 *
 *		com_type	Type of communications to establish,
 *				  IOMODE_COM_NEIGHBOR: nearest neighbor.
 *				  IOMODE_COM_SPANNING: spanning tree.
 *
 *		pfs_comm_info_t	Pointer to local iomode information.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_init_mach_com(fdte, com_type, com_info, interrupt)
fdt_entry_t		*fdte;		/* File table entry */
int 			com_type;	/* Type of communications required. */
pfs_comm_info_t 	*com_info;	/* Table of comm information. */
boolean_t		*interrupt;	/* Interrupt. */
{
	pfs_iomode_t		*iomode_p;
	kern_return_t		mach_error;
	int			name;
	mach_port_t		port_name;
	mach_port_t             previous;


	iomode_p = (pfs_iomode_t *)fdte->pfs_iomode_info;

	switch (com_type) {

		case IOMODE_COM_NEIGHBOR:
			/*
			 * This mode requires a ring communication topology
			 * of Mach port which allows node N to send to node
			 * N+1.  The last node will send back to node 0.  
			 */
			name = PFS_MACH_PORT_BASE;
			mach_error = KERN_NAME_EXISTS;
			/*
			 * Allocate a port pair starting at 
  			 * PFS_MACH_PORT_BASE.
			 */
			while(mach_error == KERN_NAME_EXISTS) {

				mach_error = mach_port_allocate_name( 
						mach_task_self(),
						MACH_PORT_RIGHT_RECEIVE,
						name);

				name+=2;	/* Increment the name space
						 * value by two, each
						 * connection takes two
						 * Mach ports.
						 */	
			}

			if (mach_error != KERN_SUCCESS) {
				e_printf("pfs_init_mach_com: port_allocate %s\n",
					 mach_error_string(mach_error));
				return EIO;
			}
				
			iomode_p->syncin_port = name - 2;	/* adjust for
								 * correct 
								 * value. 
								 */
			/*
			 * Insert a send right, to be given away later.
			 */
			mach_error = mach_port_insert_right (
						mach_task_self(),
						iomode_p->syncin_port,
						iomode_p->syncin_port,
						MACH_MSG_TYPE_MAKE_SEND);
			if (mach_error != KERN_SUCCESS) {
				e_printf("pfs_init_mach_com: insert_right %s\n",
					 mach_error_string(mach_error));
				return EIO;
			}

			/*
			 * Arrange for a 'no-more-senders' notification :
			 */
			mach_error = mach_port_request_notification(
						mach_task_self(),
						iomode_p->syncin_port,
						MACH_NOTIFY_NO_SENDERS,
						1, iomode_p->syncin_port,
						MACH_MSG_TYPE_MAKE_SEND_ONCE,
						&previous);
			if ((mach_error != KERN_SUCCESS) ||
			    (previous != MACH_PORT_NULL)) {
			        e_printf("pfs_init_mach_com: req notification failed %s\n",
					 mach_error_string(mach_error));
			}

			/*
			 * Fill in the port right namespace values:
			 */
			iomode_p->syncout_port 	= iomode_p->syncin_port+1;
			com_info->syncin_port 	= iomode_p->syncin_port;
			com_info->syncout_port  = iomode_p->syncout_port;
			break;

		case IOMODE_COM_SPANNING:
			/*
			 * This mode requires a spanning tree communication
			 * topology. Every node will have one receive port
			 * and possibly numberous sending ports.  The 
			 * receiving port is created in this step so that
			 * it's name can be distributed to all of the other
			 * nodes.  This is done to allow the sending node
			 * to obtain a send right to this port using it's
			 * name.
			 */
			mach_error = mach_port_allocate( 
						mach_task_self(),
					   	MACH_PORT_RIGHT_RECEIVE,
						&port_name);
			if (mach_error != KERN_SUCCESS) {
				e_printf("pfs_init_mach_com: data port allocate %s\n",
					 mach_error_string(mach_error));
				return EIO;
			}

			/*
			 * Insert a send right, to be given away later.
			 */
			mach_error = mach_port_insert_right (
						mach_task_self(),
						port_name,
						port_name,
						MACH_MSG_TYPE_MAKE_SEND);
			if (mach_error != KERN_SUCCESS) {
				e_printf("pfs_init_mach_com: insert data right %s\n",
					 mach_error_string(mach_error));
				return EIO;
			}

			/*
			 * Arrange for a 'no-more-senders' notification :
			 */
			mach_error = mach_port_request_notification(
						mach_task_self(),
						port_name,
						MACH_NOTIFY_NO_SENDERS,
						1, port_name,
						MACH_MSG_TYPE_MAKE_SEND_ONCE,
						&previous);
			if ((mach_error != KERN_SUCCESS) ||
			    (previous != MACH_PORT_NULL)) {
			        e_printf("pfs_init_mach_com: req notification failed %s\n",
					 mach_error_string(mach_error));
			}

			iomode_p->datain_port	= port_name;
			com_info->datain_port 	= port_name;
			break;

	} /* end switch */

	return ESUCCESS;
}


/*
 * NAME:	pfs_init_fp_info
 *
 *
 * DESCRIPTION:
 *		This function is used to obtain the local file table
 *		information prior to exchanging information between
 *		application processes.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		fp_info		Pointer to where the file information
 *				 will be stored.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_init_fp_info( fdte, fp_info)
fdt_entry_t	*fdte;
pfs_fp_info_t	*fp_info;
{
	/*
	 * Save the file port.
	 */
	fp_info->fp_ports[0] = fdte->fp;

	if (fdte->pfs_fd) {
		int sfactor = fdte->pfs_fd->p_stripe_factor;
		int sfd;
		/*
		 * PFS files need to save all of the stripe file
		 * port numbers.
		 */
		for(sfd = 0; sfd < sfactor; sfd++) {
			fp_info->fp_ports[sfd+1] = 
				fdte->pfs_fd->p_stripe_fdt[sfd].s_fp;

		}
	}
	return ESUCCESS;
}


/*
 * NAME:	rlse_rlse_fdte
 *
 *
 * DESCRIPTION:
 *		This function is used to ...
 *		
 *		
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 * RETURNS:
 *
 *		Nothing.
 */
void
pfs_rlse_fdte(fdte)
fdt_entry_t	*fdte;
{
	pfs_iomode_t	*iomode_p;
	int		i;

	iomode_p = fdte->pfs_iomode_info;
	/*
	 * Go through the in_port and out_port tables and
 	 * deallocate any ports that were in use.
	 */

	if (iomode_p->syncin_port != MACH_PORT_NULL) {
		mach_port_destroy(mach_task_self(),
				  iomode_p->syncin_port);
		iomode_p->syncin_port  = MACH_PORT_NULL;
	}
	if (iomode_p->syncout_port != MACH_PORT_NULL) {
		mach_port_destroy(mach_task_self(),
				  iomode_p->syncout_port);
		iomode_p->syncout_port  = MACH_PORT_NULL;
	}
	if (iomode_p->datain_port != MACH_PORT_NULL) {
		mach_port_destroy(mach_task_self(),
				  iomode_p->datain_port);
		iomode_p->datain_port  = MACH_PORT_NULL;
	}
	for(i=0; i<IOMODE_MAXPORT; i++) {
		if (iomode_p->dataout_ports[i] != MACH_PORT_NULL) {
			mach_port_destroy(mach_task_self(),
					  iomode_p->dataout_ports[i]);
			iomode_p->dataout_ports[i]  = MACH_PORT_NULL;
		}
	}
	free(fdte->pfs_iomode_info);
	fdte->pfs_iomode_info = NULL;
	fdte->pfs_iomode = M_UNIX;
}


/*
 * NAME:	pfs_connect_mach_com
 *
 *
 * DESCRIPTION:
 *		This function is used to establish the Mach iomode
 *		communication ports that are used to communicate
 *		iomode information between tasks.  This function
 *		sets up all the local and remote rights on the
 *		Mach ports to enable the proper communication paths.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		com_type	Type of communications to establish,
 *				  IOMODE_COM_NEIGHBOR: nearest neighbor.
 *				  IOMODE_COM_SPANNING: spanning tree.
 *
 *		com_info	Pointer to global communication information.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_connect_mach_com(fdte, com_type, com_info, interrupt) 
fdt_entry_t		*fdte;		/* File table entry. */
int 			com_type;	/* Type of communications required. */
pfs_comm_info_t 	com_info[];	/* Table of comm. information. */
boolean_t		*interrupt;	/* Interrupt variable.  */
{

	int		error = ESUCCESS;
	int 		from_node;		/* Node sending to me. */
	pfs_iomode_t 	*iomode_p;		/* Pointer to iomode entry */
	kern_return_t	mach_error;
	mach_port_t	task_port;

	iomode_p = (pfs_iomode_t *)fdte->pfs_iomode_info;

	switch( com_type) {

		case IOMODE_COM_NEIGHBOR: /* Nearest neighbor communications. */

			/*
			 * Compute which node I will be receiving
			 * from.
			 */
			if (iomode_p->my_node_number) {
				from_node = iomode_p->my_node_number - 1;
			} else {
				from_node = iomode_p->number_of_nodes - 1;
			}
			/*
			 * Move the send right of my input port
  			 * to the node that will be sending information
			 * to me, i.e. upstream node. 
			 */
			emul_blocking();
                        bsd_task_by_pid(our_bsd_server_port,
					interrupt,
					com_info[from_node].unix_pid,
					&task_port);
			emul_unblocking();

			mach_error = mach_port_insert_right(
				task_port,
			       	com_info[from_node].syncout_port,
			       	com_info[iomode_p->my_node_number].syncin_port,
			       	MACH_MSG_TYPE_MOVE_SEND);

			mach_port_deallocate(mach_task_self(),
					     task_port);

			if (mach_error != KERN_SUCCESS) {
				EPRINT(("connect_mach_com: insert_right%s\n",
				 	mach_error_string(mach_error)));
				error = EIO;
			}
			/*
			 * Save the name of my output port in the fdte table
			 * for use later.
			 */ 
			iomode_p->syncout_port = 
			      com_info[iomode_p->my_node_number].syncout_port;
			break;

		case IOMODE_COM_SPANNING: /* Spanning tree communications. */
			{
			/*
			 * Need to determine which nodes that data will
			 * be sent to and obtain the send rights to the
			 * datain_port for that node.
			 */
			int node_list[PFS_MAXSPAN];
			int node_cnt;
			int i;
			mach_msg_type_name_t	new_type;

			/*
			 * Get the list of receiving nodes:
			 */

			pfs_getspanning(iomode_p->my_node_number,
					iomode_p->number_of_nodes,
					node_list,
					&node_cnt);

			for(i = 0; i< node_cnt; i++) {
				/*
			 	 * Move the send right of my input port
  			 	 * to the node that will be sending information
			 	 * to me, i.e. upstream node. 
			 	 */
				emul_blocking();
                        	bsd_task_by_pid( our_bsd_server_port,
                               	         interrupt,
                                         com_info[node_list[i]].unix_pid,
                                         &task_port);
				emul_unblocking();

				mach_error = mach_port_extract_right(
					   task_port,
			       		   com_info[node_list[i]].datain_port,
					   MACH_MSG_TYPE_MOVE_SEND,
					   &iomode_p->dataout_ports[i],
					   &new_type);

				mach_port_deallocate(mach_task_self(),
						     task_port);

				if (mach_error != KERN_SUCCESS) {
					int right_cnt = i;
					EPRINT(("conn_mach_com:extr_rights%s\n",
				 		mach_error_string(mach_error)));
					error = EIO;
					/*
					 * Clean up any ports that were
					 * allocated prior to the error.
					 */
					for (i = 0; i<right_cnt; i++){
                                  	  (void) mach_port_deallocate( 
					  	    mach_task_self(),
					   	    iomode_p->dataout_ports[i]);
					}
					return error;
				}
			} /* For */
		        }
		        break;

	} /* end switch */
	return error;
}


/*
 * NAME:	pfs_connect_fp_info
 *
 *
 * DESCRIPTION:
 *		This function is used to convert an individual
 *		file pointer into a shared file pointer.  This
 *		function sets up the communication mechanism 
 *		between this task and the file server.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table index.
 *
 *		fp_info_array	Array of global fp information.
 *
 *		task0_port	Task port of process on node 0.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_connect_fp_info(fdte, fp_info_array, task0_port, interrupt)
fdt_entry_t		*fdte;			/* File descriptor entry. */
pfs_fp_info_t		fp_info_array[];	/* Global fp information. */
mach_port_t		task0_port;		/* Node 0's task port.	  */
boolean_t		*interrupt;		/* Interrupt variable.    */
{
	int		error = ESUCCESS;
	pfs_iomode_t	*iomode_p;
	kern_return_t	mach_error;
	mach_msg_type_name_t	new_type;


	iomode_p = fdte->pfs_iomode_info;

	/*
	 * Set up the common file pointer:
	 */
	if (iomode_p->my_node_number != 0) { 

		fdte_lock(fdte);
		/* 
		 * Release my reference to the fp port(s) and
		 * replace it with node 0s reference.
		 */
		if (fdte->fp != MACH_PORT_NULL) {
			/*
			 * Release the token, if acquired:
			 */
			unref_file(&fdte->fp);
		} /* if fdte->fp */

		/*
		 * Extract a send right from node 0.
		 */
		mach_error = mach_port_extract_right(
				task0_port,
				fp_info_array[0].fp_ports[0],
				MACH_MSG_TYPE_COPY_SEND,
				&fdte->fp,
				&new_type);

		if (mach_error != KERN_SUCCESS) {
			e_printf("pfs_connect_fp_info: extract_right%s\n",
		 	mach_error_string(mach_error));
			fdte_unlock(fdte);
			return EIO;
		} /* if mach_error */

		/*
		 * Tell the server to increment the server
		 * reference to the port used by node 0.
		 */
		ref_file(fdte->fp);

		if (fdte->pfs_fd) {
			int		sfd;
			mach_port_t 	*stripe_fp;
			int 		sfactor = fdte->pfs_fd->p_stripe_factor;

			/*
			 * Need to extract the rights for the
			 *  stripe files.
			 */
                        for (sfd = 0; sfd < sfactor; sfd++) {

                                stripe_fp = &fdte->pfs_fd->p_stripe_fdt[sfd].s_fp;
				/* 
		 		 * Release my reference to the fp port and
		 		 * replace it with node 0's reference.
		 		 */
				if (*stripe_fp != MACH_PORT_NULL) {
					unref_file(stripe_fp);
				} /* if stripe_fp */

				/*
		 		 * Extract a send right from node 0.
		 		 */
				mach_error = mach_port_extract_right(
					task0_port,
					fp_info_array[0].fp_ports[sfd+1],
					MACH_MSG_TYPE_COPY_SEND,
					stripe_fp,
					&new_type);

				if (mach_error != KERN_SUCCESS) {
					e_printf("pfs_commect_fp_info: extract_right%s\n",
				 	mach_error_string(mach_error));
					fdte_unlock(fdte);
					return EIO;
				} /* if mach_error */

				/*
		 		 * Tell the server to increment the server
		 		 * reference to the port used by node 0.
		 		 */
				ref_file(*stripe_fp);
			} /* for */
			
		}
		fdte_unlock(fdte);
	}
	return error;
}


/*
 * NAME:	pfs_conn_mem_obj
 *
 *
 * DESCRIPTION:
 *		This function is used to connect up the 
 *		mapped file memory object info.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table index.
 *
 *		fp_info_array	Array of global fp information.
 *
 *		task0_port	Task port of processes on node 0.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_conn_mem_obj(fdte, fp_info_array, task0_port)
fdt_entry_t		*fdte;			/* File descriptor entry. */
pfs_fp_info_t		fp_info_array[];	/* Global fp information. */
mach_port_t		task0_port;		/* Task port of process 0*/
{
	int		error = ESUCCESS;
	pfs_iomode_t	*iomode_p;
	kern_return_t	mach_error;
	mach_msg_type_name_t	new_type;


	iomode_p = fdte->pfs_iomode_info;

	/*
	 * Set up the common file pointer:
	 */
	if (iomode_p->my_node_number != 0) { 

		if (fdte->mem_obj != MACH_PORT_NULL) {
			/*
			 * The mem_obj will be set to non_null
			 * only for the case of setiomode.  The
			 * gopen() path will set this value to
			 * MACH_PORT_NULL.
			 */
			(void) mach_port_deallocate(mach_task_self(),
						    fdte->mem_obj);
		}
 		/*
		 * Node 0 holds onto the mem_obj right when
		 * the token was acquired.  All non_zero nodes
		 * will use the memory object port from node 0.
		 */

		mach_error = mach_port_extract_right(
				task0_port,
				fp_info_array[0].mem_obj,
				MACH_MSG_TYPE_COPY_SEND,
				&fdte->mem_obj,
				&new_type);

		if (mach_error != KERN_SUCCESS) {
		  EPRINT(("pfs_conn_mem_obj: mem_obj extract right%s\n",
		  	mach_error_string(mach_error)));
		  return EIO;
		}

		if (fdte->win_size) {
			/*
			 * Again, win_size will be set to zero
			 * if entered via the gopen path.
			 */

			if (error = vm_deallocate(mach_task_self(),
						  fdte->win_addr,
						  fdte->win_size)) {
				EPRINT(("pfs_conn_mem_obj:_fp.vm_dealc failure: addr=0x%x, size=%d, error=0x%x",
					fdte->win_addr,
					fdte->win_size,
						error));
				return EIO;
			}
			fdte->win_size = 0;
		}
	}
	return error;
}




/*
 * NAME:	pfs_iomode_syncoff
 *
 *
 * DESCRIPTION:
 *		This function is used to verify the operation type
 *		in the M_SYNC I/O mode.
 *
 * PARAMETERS:
 *
 *		fildes		File descriptor table index.
 *
 *		count		Number of bytes in i/o operation.
 *
 *		op_type		Type of i/o operation.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
pfs_iomode_syncoff(fdte, count, op_type, interrupt)
fdt_entry_t	*fdte;		/* File descriptor index, into fdt table. */
int		count; 		/* Number of bytes in operation. */
int		op_type; 	/* Type of i/o operatoins. */
boolean_t 	*interrupt;	/* Interrupt variable. */
{
	int		error = ESUCCESS;
	SYNC_DATA	sync_data;
	

	/*
	 * Get new base address for the i/o operation:
	 */ 
	switch (fdte->pfs_iomode) {
		case M_UNIX:		/* Don't check operation. */
		case M_LOG:
		case M_RECORD:
		case M_ASYNC:
			break;

		case M_SYNC:	/* Check operation type. */
		case M_GLOBAL:
			sync_data.op_type  	= op_type;
			sync_data.count 	= count;	
			sync_data.offset.shigh 	= 0;
			sync_data.offset.slow  	= 0;

			error = giomode_op(fdte, gop_syncoff, &sync_data);
			break;
	} /* switch */
	return error;
}



/*
 * NAME:	pfs_iomode_close
 *
 *
 * DESCRIPTION:
 *		This function is used to synchronize the closing
 *		of a file that is associated with a non M_UNIX 
 *		iomode.  All file descriptors that have the iomode_p
 *		variable set go through a global synchronization at
 *		the time the file descriptor is closed.  This function
 *		provides the consistency check for the M_RECORD iomode
 *		to verify that all nodes have the same file offset.  
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table index.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
pfs_iomode_close(fdte,interrupt)
fdt_entry_t	*fdte;
boolean_t	*interrupt;	/* Interrupt variable. */
{
	int		error = ESUCCESS;
	CLOSE_DATA	close_data;


	/*
	 * Initialize the close data:
	 */
	close_data.offset.shigh	= -1;
	close_data.offset.slow 	= -1;
	close_data.length.shigh	= -1;
	close_data.length.slow 	= -1;

	/*
	 * Coordinate with the other nodes to
	 * close the file. 
	 */

	error = giomode_op(fdte, gop_close, &close_data);

	/*
	 * Release any resources:
	 */

	(void) pfs_rlse_fdte(fdte);
	
	return error;
}
	


/*
 * NAME:	pfs_iomode_lseek
 *
 *
 * DESCRIPTION:
 *		This function synchronizes the lseek operation between
 *		the nodes in an application.  The iomodes M_SYNC, M_RECORD,
 *		and M_GLOBAL require that all nodes in an application call
 *		the lseek or eseek at the same time.  This function is
 *		used to coordinate the seek operation between all of 
 *		the tasks in an application.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		offset		Pointer to the current offset.
 *
 *		sbase		Sbase paramter.
 *
 *		new_offset	Pointer to new offset.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
pfs_iomode_lseek(fdte, offset, sbase, new_offset)
fdt_entry_t	*fdte;
esize_t		*offset;
int		sbase;
esize_t		*new_offset;
{
	int		error = ESUCCESS;
	LSEEK_DATA 	lseek_data;

	lseek_data.sbase		= sbase;
	lseek_data.offset.shigh 	= offset->shigh;
	lseek_data.offset.slow 		= offset->slow;

	if ((fdte->pfs_iomode == M_SYNC)  ||
	    (fdte->pfs_iomode == M_RECORD)||
	    (fdte->pfs_iomode == M_GLOBAL)) {
	    
		error = giomode_op(fdte, gop_lseek, &lseek_data); 
	}

	if (!error) {
		new_offset->shigh = lseek_data.offset.shigh;
		new_offset->slow =  lseek_data.offset.slow;
	} else {
		new_offset->shigh = -1;
		new_offset->slow = -1;
	}
	return error;
}


/*
 * NAME:	pfs_iomode_max
 *
 *
 * DESCRIPTION:
 *		This function is used to determine the maximum file length
 *		and offset for a distributed set of tokens, this function is
 *		only used in the M_RECORD PFS I/O mode.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		local_offset	Pointer to local offset value.
 *
 *		local_length	Pointer to local length value.
 *
 *		max_offset	Pointer to maximum offset value,
 *				returned as result.
 *
 *		max_length	Point to maximim length value,
 *				returned as result.
 *
 *
 * RETURNS:
 *
 *	Nothing.
 */
void
pfs_iomode_max(fdte, local_offset, local_length, max_offset, max_length)
fdt_entry_t	*fdte;
esize_t		*local_offset;
esize_t		*local_length;
esize_t		*max_offset;
esize_t		*max_length;
{
	MAX_DATA 	max_data;

	max_data.offset = *local_offset;
	max_data.length = *local_length;
	giomode_op(fdte, gop_max, &max_data); 
	*max_length = max_data.length;
	*max_offset = max_data.offset;
}


/*
 * NAME:	pfs_iomode_iseof
 *
 *
 * DESCRIPTION:
 *		This function synchronizes the iseof operation between
 *		the nodes in an application. 
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		offset		Pointer to the current offset value.
 *
 *		length		Pointer to the current length value.
 *
 *		result		Local result of the iseof function.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
pfs_iomode_iseof(fdte, offset, length, result)
fdt_entry_t	*fdte;
esize_t		*offset;
esize_t		*length;
int		*result;
{
	ISEOF_DATA 	iseof_data;

	iseof_data.offset = *offset;
	iseof_data.length = *length;
	iseof_data.result = result;
	
	return giomode_op(fdte, gop_iseof, &iseof_data); 
}


/*
 * NAME:	gop_syncoff
 *
 *
 * DESCRIPTION:
 *		This function is used to perform the global synchronization
 *		of the file pointer offset for i/o operations in the
 *		M_SYNC iomode.  This function is called three times during
 *		the the global operation with the state variable indicating
 *		the state that the global operation is in.  
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		msg		Pointer to the iomode message.
 *
 *		sync_data	Pointer to the SYNC_DATA to use in the 
 *				iomode message.
 *
 *		state		The state of the global operation.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
gop_syncoff(fdte, msg, sync_data, state)
fdt_entry_t     *fdte;		/* File descriptor table entry. */
iomode_msg	*msg;		/* Iomode Message.  */
SYNC_DATA	*sync_data;	/* Syncing data. */
int		state;		/* State. */
{
	int		error = ESUCCESS;


	switch (state) {

		case IOOP_START:	/* Node 0 build the message: */
			msg->op_type 	= sync_data->op_type;
			msg->error  	= ESUCCESS;
			msg->offset	= __eadd1(sync_data->offset,
						  (long)sync_data->count,
						  &error);
			break;
			
		case IOOP_END:		/* Node 0 final reply. */
			error = msg->error;
			break;

		case IOOP_RCV:		/* Non zero node receives. */
			if (msg->op_type != sync_data->op_type) {
				error 		= EMIXIO;
				msg->error 	= EMIXIO;
			} else {
				/*
				* Build the message:
				*/
				sync_data->offset.shigh  = msg->offset.slow;
				sync_data->offset.slow   = msg->offset.slow;
				msg->offset = __eadd1(sync_data->offset,
						      (long)sync_data->count,
						      &error);
			}
			break;
	} /* end switch */
	return error;
}


/*
 * NAME:	gop_max
 *
 *
 * DESCRIPTION:
 *		This function is used to obtain the maximum file 
 *		length and offset of a shared file.  This function is
 *		used for I/O mode M_RECORD since the 
 *		offset and length values are independent.  All other I/O 
 *		modes either use the same offset value or coordinate with
 *		the token owner to obtain the current offset.  The maximum
 *		offset and length values are returned only to node zero of the
 *		application.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		msg		Pointer to the iomode message.
 *
 *		max_data	Pointer to the MAX_DATA to use in the 
 *				iomode message.
 *
 *		state		The state of the global operation.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
gop_max(fdte, msg, max_data, state)
fdt_entry_t     *fdte;		/* File descriptor table entry. */
iomode_msg	*msg;		/* Iomode Message.  */
MAX_DATA	*max_data;	/* Maximum offset and length data. */
int		state;		/* State. */
{
	switch (state) {

		case IOOP_START:	/* Node 0 build the message: */
			msg->offset = max_data->offset;
			msg->length = max_data->length;
			break;

		case IOOP_END:		/* Node 0 final reply. */
			/*
			 * Get maximum length. 
			 */
			max_data->length = msg->length; 
			max_data->offset = msg->offset; 
			break;

		case IOOP_RCV:		/* Non zero node receives. */
			/* 
			 * Compute the max length and offset:
			 */
			msg->offset = EMAX(max_data->offset, msg->offset);
			msg->length = EMAX(max_data->length, msg->length);
			break;
	} /* end switch */
	return ESUCCESS;
}

/*
 * NAME:	gop_close
 *
 *
 * DESCRIPTION:
 *		This function is used to perform the global synchronization
 *		for the close operation. This function is called three times
 *		during the the global operation with the state variable
 *		indicating the state that the global operation is in.  
 *		This function is called for all iomodes.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		msg		Pointer to the iomode message.
 *
 *		close_data	Pointer to the CLOSE_DATA to use in the 
 *				iomode message.
 *
 *		state		The state of the global operation.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
gop_close(fdte, msg, close_data, state)
fdt_entry_t     *fdte;		/* File descriptor table entry. */
iomode_msg	*msg;		/* Iomode Message.  */
CLOSE_DATA	*close_data;	/* Close data. */
int		state;		/* State. */
{
        int             error = ESUCCESS;
	esize_t		tmp_length;
	esize_t		tmp_offset;
        pfs_token_mgr_info      *info;
	boolean_t	interrupt;

        info = fdte->pfs_iomode_info->token_mgr_info;

#ifdef DEBUG_CLOSE
	e_printf("gop_close: info = %x, state = %d\n",
		info, state);
#endif

	switch (state) {

	  	case IOOP_START:	/* Node 0 build the message: */	
			msg->op_type 	= PFS_OP_CLOSE;
			msg->error	= ESUCCESS;

			if (fdte->pfs_iomode == M_RECORD) {
				msg->length = ex_neg_one;
				msg->modified = fdte->modified;
				msg->accessed = fdte->accessed;

				if (fdte->iomode == VIO_MAPPED){
					msg->offset.shigh = 0;
					msg->offset.slow = fdte->offset;
					if (fdte->flags) {
						msg->length.shigh = 0;
						msg->length.slow = 
							fdte->length;
					} /* if (fdte->flags */
				} else if (fdte->pfs_fd) {
					msg->offset = fdte->pfs_fd->p_offset;
					if (fdte->flags) {
						msg->length = 
							 fdte->pfs_fd->p_length;
					} /* if fdte->flags */
				} else {
					/*
					 * Non-Mapped or NFS file:
					 */
					error = pfs_get_offlen(fdte,
								&msg->offset,
								&msg->length);	

				}
				if(PFS_TOKENMGT(fdte)) {
					/*
					 * Set the token so it cannot be
					 * revoked:
					 */
					fdte->pfs_iomode_info->token_mgr_info->can_revoke = 0;
					fdte->flags = 0;
				}
#ifdef DEBUG_SETIO
	e_printf("gop_close: flags = %d, msg->offset = (%d,%d), msg->length=(%d,%d)\n",
		fdte->flags,
		msg->offset.shigh, msg->offset.slow,
		msg->length.shigh, msg->length.slow);
#endif
			}
			break;

		case IOOP_END:		/* Node 0, finished. */
#ifdef DEBUG_CLOSE
	e_printf("IOOP_END msg->error = %d\n",msg->error);
#endif
			if (PFS_TOKENMGT(fdte)) {
        			info = fdte->pfs_iomode_info->token_mgr_info;
#ifdef DEBUG_CLOSE
	e_printf("IOOP_END info = %x\n",info);
#endif
				/*
			  	 * Update the length of the file 
				 * according to the largest length
				 * given.  This is so the token can
				 * be released back to the server with
				 * the largest length of any of the
				 * clients.
				 */
#ifdef DEBUG_CLOSE
	e_printf("IOOP_END updating the length\n");
#endif
				if (!EQUAL(msg->length, ex_neg_one)) {
					/*
					 * Make sure length is greater
					 * than what we have before 
					 * updating.
					 */
					if (GREATER(msg->length, info->length)){
		    				info->length.shigh = 
							msg->length.shigh;
		    				info->length.slow = 
							msg->length.slow;
					}
				}
#ifdef DEBUG_CLOSE
	e_printf("IOOP_END: msg->length = (%d,%d)\n",
			msg->length.shigh, msg->length.slow);
#endif
				/*
			 	 *  Update the modified and accessed
				 *  flags 
			 	 */
#ifdef DEBUG_CLOSE
	e_printf("IOOP_END updating modified and accessed\n");
#endif
				info->modified = msg->modified;
				info->accessed = msg->accessed;
				if (msg->error == EMIXIO) { 
					/*
					 * This is a result of not all of
					 * the nodes doing the same operation
					 * and therefor leaving the file in
					 * a weird state.  Attempt to fix
					 * the file by truncating it to
					 * the largest know length.
					 */
					if (fdte->pfs_fd) {
						multi_ftruncate(&interrupt,
						 fdte, info->length);
					} else if (fdte->iomode == VIO_MAPPED){
						info->offset = 
							info->length;
						info->max_offset =
							info->length.slow;
					}
				}
				pfs_rlse_token_mgr(fdte, 0);
			}
			return msg->error;
			break;


		case IOOP_RCV:		/* Non Node 0 receive. */
			
#ifdef DEBUG_CLOSE
	e_printf("IOOP_RCV msg->error = %d, msg->offset = %d,%d, msg->length = %d,%d\n",msg->error,msg->offset.shigh, msg->offset.slow, msg->length.shigh, msg->length.slow);
#endif
			switch(fdte->pfs_iomode) {
				case M_SYNC:
				case M_GLOBAL:
					if (msg->op_type != PFS_OP_CLOSE) {
						error = EMIXIO;
						msg->error = error;
					} /* if msg->op_type */
					break;

				case M_RECORD:
					/*
					 * Check the file offset value
					 * against my neighbors, indicate
					 * an error if different.
					 */ 
					if (fdte->iomode == VIO_MAPPED){
						tmp_offset.shigh = 0;
						tmp_offset.slow = fdte->offset;
					} else if (fdte->pfs_fd) {
						tmp_offset = 
							 fdte->pfs_fd->p_offset;
					} else {
						/* 
						 * NFS or Non-Mapped files:
						 */
						error = pfs_get_offlen(fdte,
							&tmp_offset,
							&tmp_length);
						if (error) {
							break;
						}
					}
#ifdef DEBUG_CLOSE
	e_printf("gop_close: tmp_offset = %d,%d, msg_offset = %d,%d\n",
		tmp_offset.shigh, tmp_offset.slow, msg->offset.shigh,
		msg->offset.slow);
#endif
					if (!EQUAL(msg->offset,
						   tmp_offset)){
						error = EMIXIO;
						msg->error = error;

					} /* if !EQUAL */
					if (!(PFS_TOKENMGT(fdte))) {
						break;
					}

					/*
					 * If we have a token, then check 
					 * my local file length against 
					 * that of my neighbors, use the 
					 * greatest value.
					 */
					if (fdte->flags) {
						if (fdte->iomode == VIO_MAPPED){
							tmp_length.shigh = 0;
							tmp_length.slow = 
								fdte->length;
						} else if (fdte->pfs_fd) {
							tmp_length = 
							 fdte->pfs_fd->p_length;
#ifdef DEBUG_CLOSE
	e_printf("gop_close: p_length = %d, %d\n",
		fdte->pfs_fd->p_length.shigh,
		fdte->pfs_fd->p_length.slow);
#endif
						} /* if fdte->iomode */

						if (EQUAL(msg->length, 
							  ex_neg_one)) {
							/*
							 * Use my length since
							 * my neighbor did not
							 * update the length.
							 */
						       msg->length = tmp_length;

						} else if (GREATER(tmp_length,
								  msg->length)){
							/*
							 * Use my length if 
							 * greater than my
							 * neighbors.
							 */
						       msg->length = tmp_length;
						} /* if EQUAL */
						msg->modified |= fdte->modified;
						msg->accessed |= fdte->accessed;
#ifdef DEBUG_CLOSE
	e_printf("mylength = %d,%d, sent_length = %d,%d\n",	
		tmp_length.shigh, tmp_length.slow,
		msg->length.shigh, msg->length.slow);
#endif
					} /* end if(fdte_flags) */ 
					/*
					 * Release any token mgr info:
					 */
					pfs_rlse_token_mgr(fdte, 0);
			} /* end switch (fdte->pfs_iomode) */
	} /* end switch(state) */
	return error;
}


/*
 * NAME:	gop_reset_iomode
 *
 *
 * DESCRIPTION:
 *		This function is used to perform the global synchronization
 *		for the reset iomode operation. This function is called three
 *		times during the the global operation with the state variable
 *		indicating the state that the global operation is in.  
 *		This function is called for all iomodes except for M_UNIX.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		msg		Pointer to the iomode message.
 *
 *		resetio_data	Pointer to the RESETIO_DATA to use in the 
 *				iomode message.
 *
 *		state		The state of the global operation.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
gop_reset_iomode(fdte, msg, resetio_data, state)
fdt_entry_t	*fdte;		/* File descriptor entry. */
iomode_msg	*msg;		/* Iomode Message.  */
RESETIO_DATA	*resetio_data;	/* resetio data. */
int		state;		/* State. */
{
        int            		 error = ESUCCESS;
	pfs_token_mgr_info	*info;


	switch (state) {

		case IOOP_START:	/* Node 0 build the message: */	
			msg->op_type 	  = PFS_OP_RESETIO;
			msg->error	  = ESUCCESS;
			msg->offset.shigh = resetio_data->offset.shigh;
			msg->offset.slow  = resetio_data->offset.slow;
                        msg->length       = ex_neg_one;
                        msg->modified     = fdte->modified;
                        msg->accessed     = fdte->accessed;
			break;


		case IOOP_END:		/* Node 0, finished. */
			if (PFS_TOKENMGT(fdte)) {
				info = fdte->pfs_iomode_info->token_mgr_info;
				/*
				 * Update the length of the file
				 * according to the largest length
				 * given.  This is so the token can
				 * be released back to the server with
				 * the largest length of any of the
				 * clients.
				 */
				if (!msg->error) {
					if (!EQUAL(msg->length, ex_neg_one)) {
						info->length.shigh =
							msg->length.shigh;
						info->length.slow =
							msg->length.slow;
					}

					/*
					 *  Update the modified and accessed
					 *  flags
					 */
					info->modified = msg->modified;
					info->accessed = msg->accessed;
				}
				pfs_rlse_token_mgr(fdte, 0);
			}
			return msg->error;
			break;


		case IOOP_RCV:		/* Non Node 0 receive. */
			/*
			 * Check for errors: 	offset values must be 
			 *                       the same.
			 */
			if (msg->op_type != PFS_OP_RESETIO) {
				error = EMIXIO;
				msg->error = error;
			}
			if (!EQUAL(msg->offset, resetio_data->offset)) {
				error = EMIXIO;
				msg->error = error;
			}

			if (PFS_TOKENMGT(fdte)) {
				/*
				 * If we have a token, then check
				 * my local file length against
				 * that of my neighbors, use the
				 * greatest value.
				 */
				if (EQUAL(msg->length, ex_neg_one)) {
					/*
					 * Use my length since
					 * my neighbor did not
					 * update the length.
					 */
					msg->length = resetio_data->length;

				} else if (GREATER(msg->length, 
					           resetio_data->length)){
					/*
					 * Use my length if greater than my
					 * neighbors. 
					 */
					msg->length = resetio_data->length;
				} /* if EQUAL */
				msg->modified |= fdte->modified;
				msg->accessed |= fdte->accessed;
				/*
				 * Release any token mgr info:
				 */
				pfs_rlse_token_mgr(fdte, 0);
			}
			break;
	}	
	return error;
}


/*
 * NAME:	gop_iseof
 *
 *
 * DESCRIPTION:
 *		This function is used to perform the global synchronization
 *		for the iseof iomode operation. This function is called three
 *		times during the the global operation with the state variable
 *		indicating the state that the global operation is in.  
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		msg		Pointer to the iomode message.
 *
 *		iseof_data	Pointer to the ISEOF_DATA to use in the 
 *				iomode message.
 *
 *		state		The state of the global operation.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
gop_iseof(fdte, msg, iseof_data, state)
fdt_entry_t	*fdte;		/* File descriptor entry. */
iomode_msg	*msg;		/* Iomode Message.  */
ISEOF_DATA	*iseof_data;	/* iseof data. */
int		state;		/* State. */
{
        int             error = ESUCCESS;
	esize_t		offset;


	switch (state) {

		case IOOP_START:	/* Node 0 build the message: */	
			msg->op_type 		= PFS_OP_ISEOF;
			msg->error		= ESUCCESS;
			/*
			 * Perform the iseof() function:
			 */
			msg->offset	= iseof_data->offset;
			msg->length	= iseof_data->length;
#ifdef DEBUG_ISEOF
	e_printf("iseof: IOOP_START: offset = (%d,%d), length = (%d,%d)\n",
			msg->offset.shigh, msg->offset.slow,
			msg->length.shigh, msg->length.slow);
#endif
			break;


		case IOOP_END:		/* Node 0, finished. */
			if (msg->error) {
				return msg->error;
			}
			/*
			 * Compute ISEOF according to the offset and
			 * length given.
			 */ 
#ifdef DEBUG_ISEOF
	e_printf("iseof: IOOP_END: offset = (%d,%d), length = (%d,%d)\n",
			iseof_data->offset.shigh, iseof_data->offset.slow,
			iseof_data->length.shigh, iseof_data->length.slow);
#endif
			if (LESS(iseof_data->offset, iseof_data->length)) {
				/* 	
				 * Not at EOF:
				 */
				*(iseof_data->result) = 0;
			} else {
				*(iseof_data->result) = 1;
			}
			break;


		case IOOP_RCV:		/* Non Node 0 receive. */
			/*
			 * Check for errors: 	offset values must be 
			 *                       the same.
			 */
			if (msg->error) {
				error = msg->error;

			} else if (msg->op_type != PFS_OP_ISEOF) {
				error = EMIXIO;
				msg->error = error;
			}


			if ((fdte->pfs_iomode == M_SYNC) ||
			    (fdte->pfs_iomode == M_GLOBAL)) { 
				/*
				 * Use the offset from node 0.
				 */
				offset = msg->offset;
			} else {
				offset = iseof_data->offset;
			}
			/*
			 * Compute ISEOF according to the offset and
			 * length given.
			 */ 
#ifdef DEBUG_ISEOF
	e_printf("iseof: IOOP_RCV: offset = (%d,%d), length = (%d,%d)\n",
			offset.shigh, offset.slow,
			msg->length.shigh, msg->length.slow);
#endif
			if (LESS(offset, msg->length)) {
				/* 	
				 * Not at EOF:
				 */
				*(iseof_data->result) = 0;
			} else {
				*(iseof_data->result) = 1;
			}
			break;
	}	
	return error;
}


/*
 * NAME:	gop_lseek
 *
 *
 * DESCRIPTION:
 *		This function is used to perform the global synchronization
 *		for the lseek iomode operation. This function is called three
 *		times during the the global operation with the state variable
 *		indicating the state that the global operation is in.  
 *		This function is called in the M_SYNC, M_RECORD, and M_GLOBAL
 *		iomodes.
 *
 * PARAMETERS:
 *		fdte		File descriptor entry.
 *
 *		msg		Pointer to the iomode message.
 *
 *		lseek_data	Pointer to the LSEEK_DATA to use in the 
 *				iomode message.
 *
 *		state		The state of the global operation.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
gop_lseek(fdte, msg, lseek_data, state)
fdt_entry_t	*fdte;		/* File descriptor entry. */
iomode_msg	*msg;		/* Iomode Message.  */
LSEEK_DATA	*lseek_data;	/* lseek data. */
int		state;
{
        int             error = ESUCCESS;
	boolean_t	interrupt;
	esize_t 	offset, length;
	int		token_op_type;


	switch (state) {

		case IOOP_START:	/* Node 0 build the message: */	
			msg->op_type 		= PFS_OP_LSEEK;
			msg->error		= ESUCCESS;
			/*
			 * Don't hold onto the token if in M_RECORD mode
			 * because all that is really needed is the
			 * offset at this point. By holding onto
			 * the token while at the same time waiting
			 * for one of the other nodes a deadlock
			 * situation may occur.
			 */
			 if (fdte->pfs_iomode == M_RECORD) {
			 	token_op_type = PFS_OP_OFFSET;
			 } else {
			 	token_op_type = PFS_OP_LSEEK;
			 }
			/*
			 * Get the current offset/length.
			 */	
			error = file_token_acquire (fdte,
						    &interrupt,
						    token_op_type,
						    0,
						    &msg->offset,
						    &msg->length);
			if (!error) {
			    (void) compute_lseek_offset(&msg->offset,
							&msg->length,
							&lseek_data->offset,
							lseek_data->sbase,
							&msg->new_offset);
#ifdef DEBUG_LSEEK
	e_printf("gop_lseek[0], msg->new_offset = (%d,%d), msg->offset = (%d, %d), lseek_data->offset = (%d,%d)\n",
	msg->new_offset.shigh, msg->new_offset.slow,
	msg->offset.shigh, msg->offset.slow,
	lseek_data->offset.shigh, lseek_data->offset.slow);
#endif DEBUG_PFS
							
			} else {
				msg->error		= error;
			}
			break;


		case IOOP_END:		/* Node 0, finished. */

			if (msg->error) {
				return msg->error;
			}
			/*
			 * Acquire the token for modification if in
			 * M_RECORD mode.  This is because we only looked
			 * at the offset when acquired earlier.
			 */
			if (fdte->pfs_iomode == M_RECORD) {
				if (error = file_token_acquire(fdte,
							       &interrupt,
							       PFS_OP_LSEEK,
							       0,
							       &offset,
							       &length)) {
				        return error;
				}
			}

			if (fdte->pfs_fd) {
				/*
				 * Update the stripe offsets for
				 * PFS files.
				 */
				if (error = pfs_set_stripefile_offsets(
						fdte, 
						msg->new_offset)) {

					file_token_release(fdte, 
							   &interrupt,
					    		   PFS_OP_LSEEK,
							   0,
					    		   &ex_neg_one,
					                   &ex_neg_one);
					return error;
				}
			}

			/*
			 * Update the token with the new offset
			 * position.
			 */
	 		file_token_release(fdte, 
					   &interrupt,
					   PFS_OP_LSEEK,
					   0,
					   &msg->new_offset,
					   &ex_neg_one);
			
		 	lseek_data->offset.shigh = msg->new_offset.shigh;
			lseek_data->offset.slow  = msg->new_offset.slow;
			return msg->error;
			break;


		case IOOP_RCV:		/* Non Node 0 receive. */
			/*
			 * Check for errors: 	offset values must be 
			 *                       the same.
			 */
			if (msg->error) {
				error = msg->error;

			} else if (msg->op_type != PFS_OP_LSEEK) {
				error = EMIXIO;
				msg->error = error;
	
			} else {
				esize_t new_offset;
				if ((fdte->pfs_iomode == M_SYNC) ||
				    (fdte->pfs_iomode == M_GLOBAL)) {
					/*
				 	 * Use the offset and length passed 
					 *  in to compute the new offset 
					 *  value.
					 */
					(void) compute_lseek_offset(
							   &msg->offset,
							   &msg->length,
							   &lseek_data->offset,
							   lseek_data->sbase,
							   &new_offset);

				} else {	/* M_RECORD */
					/*
					 * Acquire the file offset since
					 * it is different:
					 */
					error = file_token_acquire (
							   fdte,
							   &interrupt,
							   PFS_OP_LSEEK,
							   0,
							   &offset,
							   &length);

					(void) compute_lseek_offset(
							   &offset,
							   &length,
							   &lseek_data->offset,
							   lseek_data->sbase,
							   &new_offset);
				}

				/*
				 * Compare the new offset with what was
				 * passed from the last node.
				 */
				if (!EQUAL(msg->new_offset, new_offset)) {
#ifdef DEBUG_LSEEK
	e_printf("gop_lseek, msg->offset = (%d,%d), msg->new_offset = (%d, %d), new_offset = (%d,%d)\n",
		msg->offset.shigh, msg->offset.slow,
		msg->new_offset.shigh,  msg->new_offset.slow,
		new_offset.shigh, new_offset.slow);
#endif DEBUG_PFS
	
					error = ECFPS;
					msg->error 	 = error;
					new_offset.slow  = ex_neg_one.slow;
					new_offset.shigh = ex_neg_one.shigh;
				} 


				/*
				 * See if file is a pfs file, if so then
				 * we need to adjust the stripe file offsets:
				 */
				if ((!error ) && (fdte->pfs_fd)) {
					pfs_set_stripefile_offsets(
							fdte, 
							msg->new_offset);
				}

				if (fdte->pfs_iomode == M_RECORD) {
					/*
					 * Release the token.
					 */
					file_token_release(fdte,
							   &interrupt,
							   PFS_OP_LSEEK,
							   0,
							   &new_offset,
							   &ex_neg_one);
				}

				lseek_data->offset.shigh = new_offset.shigh;
				lseek_data->offset.slow  = new_offset.slow;
				
			}
	}	
	return error;
}



/*
 * NAME:	compute_lseek_offset
 *
 *
 * DESCRIPTION:
 *		This function is used to compute the offset of a seek
 *		operation.
 *
 * PARAMETERS:
 *		base_offset	Base offset position.
 *	
 *		base_length	Base length.
 *
 *		offset		offset parameter.
 *
 *		sbase		sbase parameter.
 *
 *		new_offset	Pointer to new offset
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
compute_lseek_offset(base_offset, base_length, offset, sbase, new_offset)
esize_t	*base_offset;
esize_t *base_length;
esize_t *offset;
int	sbase;
esize_t *new_offset;
{
	int error = ESUCCESS;

	switch (sbase) {

		case L_INCR:
			*new_offset = __eadd(*base_offset, *offset, &error);
			break;

		case L_XTND:
			*new_offset = __eadd(*base_length, *offset, &error);
			break;

		case L_SET:
			new_offset->slow = offset->slow;
			new_offset->shigh = offset->shigh;
			break;

		default:
			new_offset->shigh = -1;
			new_offset->slow  = -1;
	}
	return error;
}
#endif	PFS
