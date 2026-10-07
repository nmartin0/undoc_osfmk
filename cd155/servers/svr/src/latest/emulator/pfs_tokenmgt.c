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
 *	used to implement file tokens.
 *
 * HISTORY
 * $Log: pfs_tokenmgt.c,v $
 * Revision 1.30  1994/12/12  17:11:28  rlg
 * In the routine  pfs_token_acquire(),  one code path allowed the
 * fdte->token  field to be set to the value of an uninitialized
 * variable.  The routine was changed to prevent this from happening.
 *
 *  Risk:  low
 *  Benefit or PTS #:  11781
 *  Testing:  pfs and fileio EATs
 *            iomode integration test
 *            rw performance test
 *            failing test case
 *  Module(s):  emulator/pfs_tokenmgt.c
 *
 * Revision 1.29  1994/11/18  20:24:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.28  1994/10/18  15:23:10  rlg
 * Changed pfs_conn_token_mgr() to not arbitrarily set the fdte->must_release
 * field to zero.  This field is first tested and if it is non-zero, then
 * tmgr_release_to_server() is called.
 *
 *  Reviewer:	    Dave Minturn
 *  Risk:		    low
 *  Benefit or PTS #:  PTS #10445
 *  Testing:	    pfs and fileio EATs, failing test case
 *  Module(s):	    emulator/pfs_tokenmgt.c
 *
 * Revision 1.27  1994/08/31  22:46:16  mtm
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
 * Revision 1.26.2.1  1994/08/19  22:49:13  dbm
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
 * Revision 1.26  1994/06/13  15:22:27  rlg
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
 * Revision 1.25  1994/03/29  18:11:35  rlg
 * Merged the changes from 1.13.2.8 on the R1.2 branch into R1.3.
 *
 * Revision 1.24  1994/03/09  01:16:28  dbm
 * Mainline commit of R1.2 version 1.13.2.7
 *
 * Revision 1.23  1994/03/04  22:25:11  dbm
 * Mainline merge for bug 6919, (1.2 rev 1.13.2.6)
 *
 * Revision 1.22  1994/02/16  00:51:46  dbm
 * Merge from 1.2 branch, revision 1.13.2.5
 *
 * Revision 1.21  1994/02/04  19:46:48  brad
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
 * Revision 1.20  1994/02/02  21:33:32  jlitvin
 * Remove  meaningless cast again due to merge error.  This makes lint happy.
 *
 * Revision 1.19  1994/01/26  21:14:46  brad
 * Merge of revision 1.13.2.4 from the R1.2 branch.
 *
 * Revision 1.18  1994/01/26  19:21:37  dbm
 * Added missing log entries that were removed during merge.
 *
 * Revision 1.17  1994/01/07  00:05:13  dbm
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Merged fix from 1.2 of bug #5686.  This was
 * 		   originally checked in as 1.13.2.2
 *  Testing: See 1.13.2.2 entry.
 *  Module(s):
 * 	pfs_iomode.c
 * 	pfs_tokenmgt.
 * 	fsvr_user_side.c
 *
 * Revision 1.16  1994/01/05  17:09:08  brad
 * Fixed lint warnings in PFS-related code.
 *
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
 * Revision 1.15  1993/12/20  19:08:34  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: remove meaningless casts to keep 'lint' happy
 *  Testing: builds
 *  Module(s): pfs_tokenmgt.c
 *
 * Revision 1.14  1993/12/08  21:31:26  dbm
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 7332
 *  Testing: Specific test case, PFS EATs
 *  Module(s): pfs_tokenmgt.c
 *
 * Revision 1.13.2.8  1994/03/29  16:22:34  rlg
 * The warning messages from lint were evaluated and corrections made as
 * required.
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #:  7719
 *
 * Revision 1.13.2.7  1994/03/09  00:37:43  dbm
 * Added support for O_SYNC in M_SYNC PFS I/O mode.  This is for mapped files only.
 *  Reviewer: Brad Rullman
 *  Risk: Low
 *  Benefit or PTS #: 8420
 *  Testing: Specific test case using O_SYNC and M_SYNC.
 *  Module(s):
 * 	pfs_tokenmgt.c
 *
 * Revision 1.13.2.6  1994/03/04  21:46:21  dbm
 * Added logic to allow M_RECORD to have own revoke ports, fixed up area
 * where bsd_task_by_pid called to allow unix suspend to work properly.
 *  Reviewer: Brad Rullman
 *  Risk: M
 *  Benefit or PTS #:6919
 *  Testing: PFS EATs, Overlapping PFS Sats.
 *  Module(s):
 * 	pfs_tokenmgt.c
 *
 * Revision 1.13.2.5  1994/02/16  00:03:40  dbm
 * Added code to allow LSIZE operations not to be synchronized.  Also added
 * code to allow PFS synchronizing operations to work when non terminating
 * signals arrive.
 *
 *  Reviewer:Bob Godley
 *  Risk:L
 *  Benefit or PTS #:6919, 7975, 8067
 *  Testing: Specific test cases.  Ran PFS Eats several times.
 *  Module(s):
 * 	pfs_tokenmgt.c
 *
 * Revision 1.13.2.4  1994/01/26  05:49:39  brad
 * Remove extra argument to set_stripefile_offsets() call in
 * pfs_iomode_token_release().
 *
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 7829
 *  Testing: Ran PFS SAT multiple times on 8 nodes, ran PFS EAT on 64 nodes.
 *  Module(s): emulator/pfs_tokenmgt.c
 *
 * Revision 1.13.2.3  1994/01/09  00:20:21  brad
 * Fixed bug found by lint (wrong number pf params to pfs_free() in
 * pfs2_user_side.c); also fixed lint warnings in PFS-related code.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 7221
 *  Testing: ran PFS EATs on 64 nodes on 2 different systems
 *  Module(s): emulator/emul_callback.c
 *             emulator/emul_stack_alloc.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_fdt.h
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_iomode.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *             server/uxkern/fsvr_types.defs
 *
 * Revision 1.13.2.2  1994/01/06  22:51:16  dbm
 *  Reviewer: Bob Godley
 *  Risk: Medium
 *  Benefit or PTS #:5686
 *  Testing: Specific test case also ran PFS Eats on PFS files and NFS files
 * 	  (16 nodes for NFS files).
 *  Module(s): pfs_tokenmgt.c
 *
 * Revision 1.13.2.1  1993/12/08  17:35:23  dbm
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Fixes bug #7332.  Closing a PFS file sometimes hangs.
 *  Testing: Specific test case, PFS Eats.
 *  Module(s): pfs_tokenmgt.c
 *
 * Revision 1.13  1993/09/27  18:06:49  dbm
 * Fixed bug #6730, emulator was crashing if using mode 3 with NFS file.
 *
 * Revision 1.12  1993/09/21  22:00:21  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.11  1993/09/08  21:01:14  dbm
 * Added PFS_TOKENMGT macro to dissable M_RECORD mode for UFS files.  Also
 * added logic to remove the deadlock due to the callback thread trying to
 * revoke token from self in M_RECORD mode.
 *
 * Revision 1.10.6.2  1993/09/21  21:53:05  cfj
 * Workaround for PTS #6422.  Reduced usage of bsd_task_by_pid() rpc
 * to avoid race condition.
 *
 * Revision 1.10.6.1  1993/09/07  22:10:57  cfj
 * Added use of PFS_TOKENMGT() to dissable PFS M_RECORD mode for UFS files.
 * (Bug #6499)  Also fixed token revoke race condition. (Bug #6217)
 * dbm@ssd.intel.com
 *
 * Revision 1.10  1993/08/04  01:27:07  dbm
 * Fixed a problem in PFS I/O mode 3 which caused the port rights to be released
 * during token revoke/acquires.
 *
 * Revision 1.9  1993/07/29  16:37:59  dbm
 * Put in fixes for PTS 5672 bug.  Made sure file was in correct state
 * when being closed in M_RECORD mode.
 *
 * Revision 1.8  1993/07/27  19:58:04  dbm
 * Fixed a bug in M_RECORD mode that was causing the token to be revoked
 * during the close operation.
 *
 * Revision 1.7  1993/07/21  21:45:08  dbm
 * Fixed Bug with M_RECORD mode hanging on close.
 *
 * Revision 1.6  1993/07/16  03:03:52  dbm
 * Added token optimization functionality.
 *
 * Revision 1.5  1993/06/16  20:34:16  dbm
 * Changed all references to pfs_iomode to pfs_iomode_info to allow single
 * node applications to obtain the PFS I/O mode info.
 *
 * Revision 1.4  1993/05/11  01:05:58  brad
 * Removed references to obsolete fdte->mappable flag.
 *
 * Revision 1.3  1993/04/15  18:38:36  dbm
 * Added locks to protect fdte token data structures.
 *
 * Revision 1.2  1993/04/03  03:18:43  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.11  1993/03/11  00:35:39  dbm
 * Changed pfs_token_acquire to use symbol name VNOVAL instead of -1 when
 * checking for length of the file.
 *
 * Revision 1.1.2.10  1993/03/10  06:30:38  brad
 * Replaced _ecmp() usage with LESS, GREATER, EQUAL macros.
 *
 * Revision 1.1.2.9  1993/02/12  17:17:32  dbm
 * Put in some fixed for M_RECORD I/O mode.  Removed old code that dealt with
 * non mapped files.
 *
 * Revision 1.1.2.8  1993/02/09  22:36:39  brad
 * Changed pfs_fd argument to pfs_get_size to an fdte.
 *
 * Revision 1.1.2.7  1993/02/04  00:39:46  brad
 * Changed function name to pfs_set_stripefile_offsets(), for consistency.
 *
 * Revision 1.1.2.6  1993/01/20  19:51:59  brad
 * Added support for extended PFS offsets and file sizes.
 *
 * Revision 1.1.2.5  1993/01/11  17:25:39  dbm
 * Added changes to support PFS files with I/O modes.
 *
 * Revision 1.1.2.4  1992/12/22  02:24:58  dbm
 * Changed parameter order on file_token_release() function to make it
 * consistent with the file_token_acquire() function.
 *
 * Revision 1.1.2.3  1992/12/21  22:11:47  brad
 * Added initialization of stripefile offsets to pfs_token_acquire().
 *
 * Revision 1.1.2.2  1992/12/16  23:16:23  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1  1992/12/11  21:07:10  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.1  1992/12/03  00:22:09  dbm
 * Initial revision
 *
 */
#ifdef	PFS
#include <mach_init.h>
#include <mach/mig_errors.h>
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
#include "emul.h"
#include "fdt.h"
#include "pfs_iomode.h"

/* #define DEBUG_PFSTOKEN */

extern fdt_slot_t	fdt[NOFILE];		/* file descriptor table */
extern int 		fdt_lastfile;		/* HWM of fdt */
extern spin_lock_t 	fdt_lock;		/* lock protecting fd table */	
extern int		edebug;
extern esize_t		ex_neg_one;
extern boolean_t 	must_suspend;
extern boolean_t 	suspend_is_exit;
extern mach_port_t	callback_port_set;
#ifdef DEBUG_PFS
extern int		e_printf();
#endif
int	pfs_async_dflt=0;			/* Used to determine the 
						 * default I/O mode according
						 * to a bootmagic setting.
						 */
/*
 * Forward Declarations:
 */
void 	tmgr_release_to_server();
static void	setup_tmgr_from_fdte();

/*
 * NAME:	file_token_acquire
 *
 *
 * DESCRIPTION:
 *		This function is used to obtain the token to the 
 *		file specified by fdte.  Depending on the type
 *		of operation being performed and the type of file
 *		being performed on, the meaning of what the "token"
 *		is varies. 
 *
 *		In the case where the operations are read or write
 *		operations, and the file type is a mapped or PFS file,
 * 		then the token signifies that the calling task has
 *		exclusive rights to the file offset.  For special files
 *		this routine is basically a NOP.
 *
 *		In non i/o operations and for all operations on standard
 *		files, (offset kept on the server), the token just means
 *		that we have a snapshot of the current file offset value
 * 
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table pointer.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 *		op_type		Type of i/o operation.
 *
 *		count		Count, (Used for read/write).
 *
 *		offset		Pointer to the file offset.
 *
 *		length		Pointer to the file length.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
file_token_acquire(fdte, interrupt, op_type, count, offset, length)
	fdt_entry_t	*fdte;		/* fdte entry. */
	boolean_t	*interrupt;	/* Interrupt variable. */
	int		op_type;	/* Operation type. */
	int		count;		/* Number of bytes in operation. */	
	esize_t		*offset;	/* Pointer where to put offset */
	esize_t		*length;	/* Pointer where to put length. */
{
	int		error = ESUCCESS;
	int 		flags;
	int		debug_value;

	/*
	 * Compute the value of flags:
	 */
	if (	(op_type == PFS_OP_READ)   ||
		(op_type == PFS_OP_READV)  ||
		(op_type == PFS_OP_OFFSET) ||
		(op_type == PFS_OP_LSEEK)) {
			flags = TOK_OFFSET | TOK_DATA_READ;
			debug_value = 1;

	} else if ((op_type == PFS_OP_WRITE) ||
		   (op_type == PFS_OP_WRITEV)||
		   (op_type == PFS_OP_TOKMGR)||
		   (op_type == PFS_OP_LSIZE)) {
			flags = TOK_OFFSET | TOK_DATA_WRITE;
			debug_value = 2;
	} else {
		flags = TOK_OFFSET;
		debug_value = 0;
	}

	/*
	 * Determine the type of token handling required.
	 */
	if ((fdte->pfs_iomode_info) && (op_type != PFS_OP_TOKMGR)) {
		/*
	 	 * Handle tokens in PFS I/O modes.
	 	 */
		error = pfs_iomode_token_acquire(fdte,interrupt,op_type,
						 flags, count, offset, length);

#ifdef DEBUG_PFSTOKEN
	e_printf("file_token_acquire, after pfs: offset = (%d,%d), length = (%d,%d)\n",
			offset->shigh, offset->slow, length->shigh, length->slow);
#endif
	} else if (fdte->pfs_fd) {
		/* 
		 * Handle tokens for a PFS file. 
		 */
		error = pfs_token_acquire(fdte, interrupt, flags, offset,
					  length);

	} else if (fdte->iomode == VIO_MAPPED) { 
		/*
		 * Handle tokens for a mapped file.
		 */
		error = token_acquire(fdte, interrupt, flags, &offset->slow,
				      &length->slow, debug_value);

		offset->shigh = 0;
		length->shigh = 0;

	} else {
		/*
		 * Handle offset for Non-mapped or NFS files:
		 */
		if (error = pfs_get_offlen(fdte, offset, length)) {
			return error;
		}
	}
	/*
	 * See if token needs to be released right away:
	 */
	if ((op_type == PFS_OP_OFFSET) && (!error)) {
		if (PFS_TOKENMGT(fdte)) {
                        if (--fdte->token_refcnt == 0) {
                                if (fdte->must_release) {
                                        error = token_release_to_tmgr(fdte);
                                }
                                fdte->can_revoke = 1;
			}

		} else {
			if (fdte->iomode == VIO_MAPPED) { 
				error = token_release(fdte, interrupt, flags,
						      -1, -1);

			} else if (fdte->pfs_fd) {
#ifdef DEBUG_PFSTOKEN
        e_printf("file_token_acquire, offset = (%d,%d), length = (%d,%d)\n",
                offset->shigh, offset->slow,
                length->shigh, length->slow);
#endif
				error = pfs_token_release(fdte, interrupt,
							  flags, &ex_neg_one,
							  &ex_neg_one);
			}
		}
	}
	return error;
}


/*
 * NAME:	file_token_release
 *
 *
 * DESCRIPTION:
 *		This function is used to release the token to the 
 *		file specified by fdte.  Depending on the type
 *		of operation that was performed and the type of file
 *		that was performed on, the meaning of what the "token"
 *		is varies. 
 *
 *		In the case where the operation is read or write
 *		and the file type is a mapped or PFS file, then the
 *		token signifies that the calling task has possibly
 *		modified the file offset or length. For special files
 *		this routine is basically a NOP.
 *
 *		In non i/o operations and for all operations on standard
 *		files, (offset kept on the server), releasing the token 
 *		means that the file offset value on the server needs to
 *		be updated.
 * 
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table pointer.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 *		op_type		Type of i/o operation.
 *
 *		amount		Amount of data, (Used for read/write).
 *
 *		offset		Pointer to the file offset.
 *
 *		length		Pointer to the file length.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
file_token_release(fdte, interrupt, op_type, amount, offset, length)
	fdt_entry_t	*fdte;
	boolean_t	*interrupt;
	int		op_type;
	int		amount;
	esize_t		*offset;	
	esize_t		*length;	
{
	int		error=ESUCCESS;
	int		flags;
	
 	/*
	 * Determine the token flag settings:
	 */
	if (	(op_type == PFS_OP_READ)   ||
		(op_type == PFS_OP_READV)  ||
		(op_type == PFS_OP_OFFSET) ||
		(op_type == PFS_OP_LSEEK)) {
			flags = TOK_OFFSET | TOK_DATA_READ;

	} else if ((op_type == PFS_OP_WRITE)  ||
		   (op_type == PFS_OP_WRITEV) ||
		   (op_type == PFS_OP_TOKMGR) ||
		   (op_type == PFS_OP_LSIZE)) {
			flags = TOK_OFFSET | TOK_DATA_WRITE;
	} else {
		flags = TOK_OFFSET;
	}
	if ((fdte->pfs_iomode_info) && (op_type != PFS_OP_TOKMGR)) {
		/*
		 * Token has PFS I/O mode associated with it.
		 */
#ifdef DEBUG_PFSTOKEN
	e_printf("file_token_release, offset = (%d,%d), length = (%d,%d)\n",
		offset->shigh, offset->slow,
		length->shigh, length->slow);
#endif
		error = pfs_iomode_token_release(fdte, interrupt, flags, offset,
						 length, op_type, amount);

	} else if (fdte->pfs_fd) {
		/*
		 * Token is associated with a PFS file.
		 */
#ifdef DEBUG_PFSTOKEN
	e_printf("file_token_release, offset = (%d,%d), length = (%d,%d)\n",
		offset->shigh, offset->slow,
		length->shigh, length->slow);
#endif
		error = pfs_token_release(fdte, interrupt, flags, offset,
					  length); 

	} else if (fdte->iomode == VIO_MAPPED) {
		/*
		 * Token is associated with a mapped file:
		 */
#ifdef DEBUG_PFSTOKEN
	if (fdte->pfs_iomode == M_RECORD) {
		e_printf("file_token_release, offset = (%d), length = (%d)\n",
			offset->slow,
			length->slow);
	}
#endif
		error = token_release(fdte, interrupt, flags, offset->slow,
				      length->slow);
	} else {
		/*
		 * No token was required: (normal or special file)
		 */
		error = 0;
	}
	return error;
}


/*
 * NAME:	pfs_token_acquire
 *
 *
 * DESCRIPTION:
 *		This function is used to acquire a token to a 
 *		PFS file specified by fdte.  
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table pointer.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 *		flags		Flags indicating type of token.
 *
 *		offset		Pointer to the file offset.
 *
 *		length		Pointer to the file length.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
pfs_token_acquire(fdte, interrupt, flags, offset, length)
fdt_entry_t	*fdte;
boolean_t	*interrupt;
int		flags;
esize_t		*offset;	
esize_t		*length;	
{
	int		curflags;
	pfs_fd_t	*pfs_fd	 = fdte->pfs_fd;
	mach_port_t	newtoken = MACH_PORT_NULL;
	int		error	 = ESUCCESS;


	fdte_lock(fdte);
	fdte->can_revoke = 0;		/* keep revokers away */
	fdte->token_refcnt++;		/* Increment the reference count. */
	fdte_unlock(fdte);

	curflags = fdte->flags;	        /* current access */

	flags &= ~TOK_OFFSET;		/* turn off temporarily */

	if (flags == TOK_DATA_READ) {
		/*
		 * Must acquire data read access.  Having either data
		 * read or data write access does the job.
		 */
		if (curflags & (TOK_DATA_READ | TOK_DATA_WRITE)) {
			length->slow 	=  pfs_fd->p_length.slow;
			length->shigh 	=  pfs_fd->p_length.shigh;
			flags &= ~TOK_DATA_READ;
		}
	} else if (flags == TOK_DATA_WRITE) {
		/*
		 * Must acquire data write access.
		 */
		if (curflags & TOK_DATA_WRITE) {
			length->slow  = pfs_fd->p_length.slow;
			length->shigh = pfs_fd->p_length.shigh;
			flags &= ~TOK_DATA_WRITE;
		}
	} else {
		EPRINT(("pfs_token_acquire: bad flags=%d",flags));
		emul_panic("pfs_token_acquire: bad flags");
	}

	if (flags == 0) {
		offset->slow  = pfs_fd->p_offset.slow;	
		offset->shigh = pfs_fd->p_offset.shigh;	
		return (ESUCCESS);
	}
	flags |= TOK_OFFSET;	

	/*
	 * Determine whether to acquire a new (or different) token.
	 */
	if (curflags == 0) {
		/*
		 * Request a new token from the server.
		 */
		if (edebug & EDEBUG_MF) {
			EPRINT((" Calling token_acquire: flags=%d", flags));
		}

		if (pfs_fd->p_use_token) {

			error = fsvr_pfs_token_acquire(fdte->fp, 
					       credentials_port, 
					       flags, revoke_port, 
					       &newtoken,
					       &pfs_fd->p_offset, 
				  	       &pfs_fd->p_length);
			if (error) {
				e_printf("can't acquire: error = %d\n", error);
				emul_panic("pfs_token_acquire");
			}

		} else {
			/*
			 * We just need to get the offset:
			 */
			error = fsvr_pfs_get_off(fdte->fp, 
					       &pfs_fd->p_offset); 
			if (error) {
				e_printf("pfs_get_off: error = %d\n", error);
				emul_panic("pfs_token_acquire");
			}
			/* 
			 * We need to recompute the length.
			 */
			pfs_fd->p_length.slow = VNOVAL;
		    	pfs_fd->p_length.shigh = VNOVAL;
		}
#ifdef DEBUG_PFSTOKEN
		e_printf("after fsvr_pfs_token_acquire, p_offset = (%d,%d), p_length = (%d,%d)\n",
		pfs_fd->p_offset.shigh, pfs_fd->p_offset.slow, 
		pfs_fd->p_length.shigh, pfs_fd->p_length.slow); 
#endif
		/*
		 * Check to see if the file length needs to be 
		 * updated.
		 */
		if ((pfs_fd->p_length.slow == VNOVAL) &&
		    (pfs_fd->p_length.shigh == VNOVAL)) {
			/*
			 * Need to obtain the file length from all of the
			 * stripe files and update the token value.
			 */
#ifdef DEBUG_PFSTOKEN
			e_printf("Getting the size\n");
#endif
			error = pfs_get_size(interrupt, fdte, 
					     &pfs_fd->p_length);

#ifdef DEBUG_PFSTOKEN
			e_printf("Got the size, it is (%d,%d)\n",
				 pfs_fd->p_length.shigh,
				 pfs_fd->p_length.slow);
#endif
			fdte_lock(fdte);
			fdte->modified =1; 	/*  Set modified so that
						 *  the length gets updated
						 *  back to the server.
						 */
			fdte_unlock(fdte);
			if (error) {
				EPRINT(("can't get size: error = 0x%x", error));
				emul_panic("pfs_token_acquire");
			}
			
		}

		/*
		 * Initialize the stripefile offsets from the PFS file offset.
		 */
		pfs_set_stripefile_offsets(fdte, pfs_fd->p_offset);

	} else if (pfs_fd->p_use_token) {
		/*
		 * Request a different token from the server.
		 * The send right to the old token is moved to the
		 * server via the second arg to fsvr_token_change.
		 *
		 * This code assumes the only possible change is from
		 * read to write access.
		 */
		if (curflags & TOK_DATA_WRITE) {
			EPRINT(("pfs_token_acquire: bad curflags=%d",curflags));
			emul_panic("pfs_token_acquire: bad curflags");
		}

		if (edebug & EDEBUG_MF)
			EPRINT((" pfs_token_acquire: Calling token_change: flags=%d cur=%d",  
				flags, curflags));

		error = fsvr_token_change(fdte->token, fdte->token, flags,
                                          &newtoken);

		if (error)  {
		    EPRINT(("can't change tokens: error = 0x%x", error));
		    emul_panic("pfs_token_acquire");
		}

		fdte_lock(fdte);
		if (fdte->must_release) {
			/*
			 * Someone tried to revoke the token that we just 
			 * changed.  Force the server thread to retry.
			 */
			if (edebug & EDEBUG_MF)
			     EPRINT(("pfs_token_acquire change: race with revoke"));

			fdte->must_release = 0;
			
			/* async. message */
			error = fsvr_token_not_found(fdte->token, fdte->token);
			if (error) 
				/*
				 * There may have been an error because the 
				 * server blew away it's receive right.  
				 * That's ok.
				 */
			     EPRINT(("pfs_token_acquire: release err=0x%x",error));
		}
		fdte_unlock(fdte);
	}

	fdte_lock(fdte);
	fdte->flags = flags;
	fdte->token = newtoken;
	fdte_unlock(fdte);

#ifdef DEBUG_PFSTOKEN
	e_printf("pfs_token_acquire: success: offset=%d,%d, length=%d,%d",
			offset->shigh, offset->slow, 
			length->shigh, length->slow);
#endif

	offset->slow  = pfs_fd->p_offset.slow;
	offset->shigh = pfs_fd->p_offset.shigh;
	length->slow  = pfs_fd->p_length.slow;
	length->shigh = pfs_fd->p_length.shigh;
	return (ESUCCESS);
}



/*
 * NAME:	pfs_token_release
 *
 *
 * DESCRIPTION:
 *		This function is used to release a token to a 
 *		PFS file specified by fdte.  
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table pointer.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 *		flags		Flags indicating type of token.
 *
 *		offset		Pointer to the file offset.
 *
 *		length		Pointer to the file length.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
pfs_token_release(fdte, interrupt, flags, offset, length)
fdt_entry_t	*fdte;
boolean_t	*interrupt;
int		flags;
esize_t		*offset;	
esize_t		*length;	
{
	pfs_fd_t	*pfs_fd	= fdte->pfs_fd;


	if (edebug & EDEBUG_MF)
		EPRINT(("pfs_token_release: flags=%d, offset=%d, length=%d",
			flags, offset, length));

#ifdef DEBUG_PFSTOKEN
	e_printf("pfs_token_release: offset = (%d,%d), length = (%d,%d)\n",
			offset->shigh, offset->slow, 
			length->shigh, length->slow);
#endif
	fdte_lock(fdte);
	/*
	 * offset or length values of -1 say not to update the fdte values.
	 */
	if ((flags & TOK_OFFSET) && (!EQUAL(*offset, ex_neg_one))) {
		pfs_fd->p_offset.slow  = offset->slow;
		pfs_fd->p_offset.shigh = offset->shigh;
	}

	if ((flags & (TOK_DATA_READ | TOK_DATA_WRITE)) && 
	    (!EQUAL(*length, ex_neg_one))) {
		pfs_fd->p_length.slow  = length->slow;
		pfs_fd->p_length.shigh = length->shigh;
	}
#ifdef DEBUG_PFSTOKEN
	e_printf("pfs_token_release: p_offset = (%d,%d), p_length = (%d,%d)\n",
			pfs_fd->p_offset.shigh, 
			pfs_fd->p_offset.slow, 
			pfs_fd->p_length.shigh, 
			pfs_fd->p_length.slow); 
#endif

	if (--fdte->token_refcnt == 0) {
		if (fdte->must_release) {
			token_release_to_server(fdte, TRUE);
		} 
		fdte->can_revoke = 1;/* allow revokers to do their job*/
	}

	fdte_unlock(fdte);
	return (ESUCCESS);
}



/*
 * NAME:	pfs_iomode_token_acquire
 *
 *
 * DESCRIPTION:
 *		This function is used to obtain the token to a
 *		file specified by fdte that has a PFS I/O mode
 *		associated with it. This function also provides
 *		the necessary synchronization for the PFS I/O modes.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor index pointer.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 *		op_type		Type of i/o operation.
 *
 *		flags		Token flags.
 *
 *		count		Count, (Used for read/write).
 *
 *		new_base	Pointer to the new base offset.
 *
 *		new_length	Pointer to the new length offset.
 *
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 */
int
pfs_iomode_token_acquire(fdte, interrupt, op_type, flags, count, newbase, newlength)
fdt_entry_t	*fdte;		/* File Descriptor entry. 		*/
boolean_t 	*interrupt;	/* Interrupt variable. 			*/
int		op_type;	/* Operation type being performed. 	*/
int		flags;
int		count;		/* Byte count, used for read/write.	*/
esize_t		*newbase;	/* Current file offset. 		*/
esize_t		*newlength;	/* Current file length. 		*/

{
	int		error = ESUCCESS;
	off_t		offset;
	int		length;
	pfs_iomode_t	*iomode_p;
	int		debug_value = 0;

	iomode_p	= fdte->pfs_iomode_info;

	/*
	 * Check if need to sync with neighbor node, (M_SYNC).
	 */
	if (	(fdte->pfs_iomode == M_SYNC) 	&&
	   	(iomode_p->my_node_number)   	&&
	   	(iomode_p->number_of_nodes > 1) &&
	    	((op_type == PFS_OP_READ)  || 
		 (op_type == PFS_OP_READV) ||
	     	 (op_type == PFS_OP_WRITE) || 
		 (op_type == PFS_OP_WRITEV)
		)){
		/*
		 * Handshake with previous to get the token.
		 */

		error = rcv_iomode_token(iomode_p->syncin_port, newbase, 
				         newlength);

#ifdef PFS_DEBUGSYNC
	e_printf("acquire: received offset = (%d, %d), length = (%d, %d)\n",
			newbase->shigh, newbase->slow,
			newlength->shigh, newlength->slow);
#endif
		if (!error) {
			if (fdte->iomode == VIO_MAPPED) {
				fdte->offset = newbase->slow;
				fdte->length = newlength->slow;

			} else if (fdte->iomode == VIO_PFS) {
				fdte->pfs_fd->p_offset = *newbase;
				fdte->pfs_fd->p_length = *newlength;

				pfs_set_stripefile_offsets(
						fdte, fdte->pfs_fd->p_offset);

			} else {
				/*
				 * Normal UFS or NFS file, no further action
				 * is required.
				 */
			}
		}
		return error;

	} else if (fdte->pfs_iomode == M_RECORD) {
		/*
		 * M_RECORD PFS I/O mode uses the token manager
		 * so a special interface is required to obtain the token.
		 */
		if (PFS_TOKENMGT(fdte)) {
			error = tmgr_token_acquire(fdte, newbase, newlength);
			if (error) 
				return error;
                } else if (fdte->iomode == VIO_MAPPED) {
			error = token_acquire(fdte, interrupt, flags, &offset,
					      &length, debug_value);
			if (error)
				return error;
			newbase->shigh = 0;
			newbase->slow = offset;
			newlength->shigh = 0;
			newlength->slow = length;
		} else {
			/*
			 * Non-Mapped or NFS files:
			 */
			error = pfs_get_offlen(fdte, newbase, newlength);
			if (error)
				return error;
		}
		/*
		 * See if offset needs to be adjusted:
	 	 */
		if ( (fdte->pfs_iomode == M_RECORD) &&
		     ((op_type == PFS_OP_READ)  ||
		      (op_type == PFS_OP_READV) ||
		      (op_type == PFS_OP_WRITE) ||
		      (op_type == PFS_OP_WRITEV)) ) {
			/*
			 * Compute the new base starting offset:
			 */
			*newbase = __eadd1(
				*newbase,
				(long)iomode_p->my_node_number * count,
				&error);

			if (fdte->iomode == VIO_MAPPED) {
				fdte->offset = newbase->slow;
			} else if (fdte->pfs_fd) {
                        	/*
                       		 * Compute the new base starting offset:
                         	 */
                        	fdte->pfs_fd->p_offset.slow  = newbase->slow;
                        	fdte->pfs_fd->p_offset.shigh = newbase->shigh;
                        	/*
                         	 * Set the stripe file offsets:
                         	 */
				pfs_set_stripefile_offsets(fdte, *newbase);
			} else { 
				/*
				 * Non-Mapped or NFS files:
				 */
				if (error = pfs_setoff(fdte,newbase)) {
					return error;
				}
			}
		}
	} else {
		/*
		 * Normal token acquire used:
		 */

		if (fdte->iomode == VIO_MAPPED) {
			/*
		 	 * Acquire the token:
		 	 */
       			if ((error = token_acquire(fdte, interrupt, flags, 
						   &offset, &length, 
						   debug_value)) < 0) {

				return error;
			}
			newbase->shigh 		= 0;
			newbase->slow 		= offset;
			newlength->shigh 	= 0;
			newlength->slow 	= length;

		} else if (fdte->pfs_fd) {

			/*
	 		 * Acquire the PFS token:
	 		 */
       			if ((error = pfs_token_acquire(fdte, interrupt, flags, 
					       newbase, newlength))< 0) {
				return error;
			}
		} else {
			/*
			 * Non-Mapped or NFS files:
			 */
			error = pfs_get_offlen(fdte, newbase, newlength);
		}
	} 
	return error;
}


/*
 * NAME:	pfs_iomode_token_release
 *
 *
 * DESCRIPTION:
 *		This function is used to release an iomode token
 *		that was acquired using the pfs_iomode_token_acquire()
 *		function.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		interrupt	Pointer to interrupt variable.
 *
 *		flags		The type of token that was acquired.
 *
 *		offset		New offset.
 *
 *		length		New length.
 *
 *		op_type		Type of operation.
 *
 *		amount		Amount of data, in bytes, that was written.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_iomode_token_release(fdte, interrupt, flags, offset, length, op_type, amount )
fdt_entry_t	*fdte;		/* File descriptor. 	*/
boolean_t	*interrupt;	/* Interrupt variable.  */
int		flags;		/* Token flags. */
esize_t		*offset;	/* New offset.		*/
esize_t		*length;	/* New length.		*/
int		op_type;	/* Type of token. 	*/
int		amount;		/* Amount of data. 	*/
{
	int			error = ESUCCESS;
	pfs_iomode_t		*iomode_p;
	int			update=1;
	esize_t			new_offset;
	esize_t			new_length;
	int 			total_amount;
	int			rdwr_flag = 0; /* Set to indicate a read or 
						  write operation. */

	iomode_p = fdte->pfs_iomode_info;

	new_offset.slow  = offset->slow;
	new_offset.shigh = offset->shigh;
	new_length.slow  = length->slow;
	new_length.shigh = length->shigh;
	/*
	 * See if special handling is needed for the PFS I/O mode:
	 */
	if ((op_type == PFS_OP_READ)  || (op_type == PFS_OP_READV) ||
	    (op_type == PFS_OP_WRITE) || (op_type == PFS_OP_WRITEV)) {

		rdwr_flag = 1;

		if (fdte->pfs_iomode == M_SYNC) {
			if ((op_type == PFS_OP_READ) || 
			    (op_type == PFS_OP_READV)) {
				if (fdte->iomode == VIO_MAPPED) {
					new_length.shigh = 0;
					new_length.slow = fdte->length;
				} else if (fdte->pfs_fd) {
					new_length = fdte->pfs_fd->p_length; 
				}
			}
			/*
	 		 * Release the token to the next node.
	 		 */
			if (EQUAL(new_offset, ex_neg_one)) {
				/*
				 * Set to current offset, if released
				 * with -1 for offset.
				 */
				if (fdte->iomode == VIO_MAPPED) {
					new_offset.shigh = 0;
					new_offset.slow = fdte->offset;
				} else if (fdte->pfs_fd) { 
					new_offset = fdte->pfs_fd->p_offset;
				}
			}
#ifdef PFS_DEBUGSYNC
	e_printf("sending offset = (%d,%d), length = (%d,%d)\n",
		new_offset.shigh, new_offset.slow,
		new_length.shigh, new_length.slow);
#endif
			(void) snd_iomode_token(iomode_p->syncout_port,
						&new_offset, 
						&new_length);
#ifdef PFS_DEBUGSYNC
	e_printf("sent offset = (%d,%d), length = (%d,%d)\n",
		new_offset.shigh, new_offset.slow,
		new_length.shigh, new_length.slow);
#endif
			if (iomode_p->my_node_number == 0) {

				/*
				 * Wait for token from the last node.
				 */
				error = rcv_iomode_token(iomode_p->syncin_port,
							 &new_offset, 
							 &new_length);
#ifdef PFS_DEBUGSYNC
	e_printf("received offset = (%d,%d), length = (%d,%d)\n",
		new_offset.shigh, new_offset.slow,
		new_length.shigh, new_length.slow);
#endif
				if (error) {
					/*
					 * Set offset and length to -1;
					 */
					new_length = ex_neg_one;
					new_offset = ex_neg_one;

				} else {
					/*
					 * Update the offset and length with
					 * the new values.
					 */
					if (fdte->iomode == VIO_MAPPED) {
						fdte->offset = new_offset.slow;
						fdte->length = new_length.slow;
                                		/*
                                 		 * Update the VA range that
						 * needs to be cleaned.
                                 		*/
                                		if (fdte->offset < 
						    fdte->min_offset)
                                        	      fdte->min_offset = 
								fdte->offset;
                                		if (fdte->offset > 
						    fdte->max_offset)
                                        	      fdte->max_offset = 
								fdte->offset;
                        			/*
                         			 * See if file was opened with
						 * O_SYNC, if so then the file
					         * data needs to be flushed to
						 * the disk before returning to
						 * the user.  
						 */
                        			if (fdte->fmode & FSYNC) {
						  transaction_id_t    trans_id;

                                		  isc_register_chk_async(
						  		     fdte->fp, 
								     &trans_id);

                                		  error = fsvr_sync_data(
							      fdte->token,
                                                              fdte->length,
                                                              fdte->min_offset,
                                                              fdte->max_offset);
                                		  if (error) {
                                        	  	  EPRINT(("fsvr_token_sync error = 0x%x",error));
                                        		  emul_panic("fsvr_token_sync error");
                                		  }
						  /*
						   * Reset modified since the
						   * fsvr_sync_data() already 
						   * updated the file.
						   */
						  fdte->modified = 0;
                                		  isc_deregister(interrupt);
                        			}
					} else if (fdte->pfs_fd) {
						fdte->pfs_fd->p_offset = 
							new_offset;
						fdte->pfs_fd->p_length = 
							new_length;

       	         				pfs_set_stripefile_offsets(
							fdte, 
							fdte->pfs_fd->p_offset);
					} else {
						/*
  						 * Non mapped/non PFS files:
						 * The read or write already
						 * updated the offset so we
						 * dont need to do it again
						 */ 	
						update = 0;
					}
				} 


			} else  {
					update = 0;	/* Don't update the 
							 * token to the server.
						 	 */
			} 


		} else if (fdte->pfs_iomode == M_RECORD) {
			/*
			 * Compute a new offset:
			 */
			if (!EQUAL(new_offset, ex_neg_one)) {
				/* 
				 *  Update the offset by adding on all the
				 *  other nodes offsets.
		 		 */
				total_amount = 	amount * 
						(iomode_p->number_of_nodes - 
						 iomode_p->my_node_number - 1);

				new_offset = __eadd1(*offset,
						     (long)total_amount,
						     &error);

				if (fdte->iomode == VIO_MAPPED) {
					/*
				 	 * Update length of mapped file. 
					 * Only update the length to the
					 * Actual amount written.
				 	 */
					if (flags & TOK_DATA_WRITE) {
						new_length.shigh = 0;
						if (length->slow < 
						    offset->slow) {
							fdte->length = 
								offset->slow;
							new_length.slow =
								fdte->length;
						} 

						if (new_offset.slow < 
						    fdte->min_offset) {
       		                       			fdte->min_offset = 
								new_offset.slow;
						}
						if (new_offset.slow > 	
						    fdte->max_offset) {
							fdte->max_offset = 
								new_offset.slow;
						}
					}
					fdte->offset = new_offset.slow;
					if (new_length.slow != -1 ) {
						fdte->length = new_length.slow;
					}

				} else if (fdte->pfs_fd) {
			  		/*
			   		 * Update length of PFS file.
			   		 */
			   		if (flags & TOK_DATA_WRITE) {
						if (GREATER(*offset, 
							    *length)) {
							new_length = *offset;
						}
			   		}
			   		/*
			    		 * Adjust the pfs stripe file
			    		 * offsets to the new positions.
			    		 */
			    		pfs_set_stripefile_offsets(fdte,
								   new_offset);
					fdte->pfs_fd->p_offset = new_offset; 
					if (!EQUAL(new_length, ex_neg_one)) {
						fdte->pfs_fd->p_length = 
							new_length; 
					}

				} else {
					/*
					 * Non-Mapped or NFS file
					 */
					esize_t		new_amount;
					esize_t		newbase;
					esize_t		newlength;

					error = pfs_get_offlen(fdte,
							       &newbase,
							       &newlength);
					if (error)
						return error;

					new_amount.shigh=0;

					new_amount.slow =
					  newbase.slow +
					((iomode_p->number_of_nodes -
					  (iomode_p->my_node_number +1))
					       * amount);

					error = pfs_setoff(fdte, &new_amount);

					if (error) {
						return error;
					}
					update = 0;
				}
			} 

			if (PFS_TOKENMGT(fdte)) {
				/*
			 	 * Release the token to the token manager:
			 	 */
				if (--fdte->token_refcnt == 0) {
					if (fdte->must_release) {
						/*
						 * Make sure the length is up
						 * to date:
						 */
						if (fdte->iomode == VIO_MAPPED){
							fdte->length = 
								new_length.slow;

						} else if (fdte->pfs_fd) {
							fdte->pfs_fd->p_length =
								new_length;
						}
						error = 
						    token_release_to_tmgr(fdte);
					}
					fdte->can_revoke = 1;
				}
				/*
			 	 * Don't update the token to the server.
			 	 */
				update = 0;
			}
		}
	}


	if (update) {
		/* 
	 	 * Update the token back to the server:
	 	 *
	 	 */
		if (fdte->iomode == VIO_MAPPED) {
			/*
			 * Update the mapped file token:
			 */
			error = token_release(	fdte, 
						interrupt, 
						flags, 
						new_offset.slow,	
						new_length.slow);

		} else if (fdte->pfs_fd) {

			/*
		 	 * Update the PFS file token:
		 	 */
			error = pfs_token_release( fdte,
						   interrupt,
				 		   flags, 
						   &new_offset, 
						   &new_length);
		} else {
			/*
			 * Non mapped and non PFS file,  update
			 * the offset to the server if it has 
			 * not been already, the reads and writes
			 * update the values by just doing the I/O.
			 */
				if (!rdwr_flag) {
					error = pfs_setoff(fdte, &new_offset);
			}
		}
	}
	return error;
}




/*
 *	pfs_init_tokenmgr:
 *
 *		This function is used to initialize the PFS
 *		i/o mode manager thread prior to the exchange
 *		of global information by the libnx library.  
 *		The subsequent function used to complete the
 *		initialization is pfs_conn_token_mgr().
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
pfs_init_token_mgr( fdte, fp_info)
fdt_entry_t	*fdte;
pfs_fp_info_t	*fp_info;
{
	int 		error = ESUCCESS;
	kern_return_t	mach_error;


	if ((fdte->pfs_iomode == M_UNIX) 	||
	    (fdte->pfs_iomode == M_GLOBAL) 	||  
	    (fdte->pfs_iomode == M_SYNC) 	||  
	    (fdte->pfs_iomode == M_LOG)		||
	    (!PFS_TOKENMGT(fdte))) {
		/*
		 * Token Manager not needed for i/o modes M_UNIX,
		 * M_GLOBAL, M_LOG, or M_SYNC PFS I/O modes or 
		 * for mapped files.
		 */
		return ESUCCESS;
	}
	/*
	 * Create a new port to be used for M_RECORD
	 * token acquires and revokes.  This new port is necessary
	 * because the standard revoke_port is destroyed when the
	 * task goes through release state.
	 */
	mach_error = mach_port_allocate( mach_task_self(),
					 MACH_PORT_RIGHT_RECEIVE,
					 &fp_info->revoke_port);
	if (mach_error != KERN_SUCCESS) {
		EPRINT(("pfs_init_token_mgr: mach_port_allocate%s\n",
               		 mach_error_string(mach_error)));
		return(EIO);
	}
	/*
	 * Create a send right, this send right will be used
	 * by other nodes to enable them to send token revoke,
	 * release, and acquire RPC's to this node.
	 */
        mach_error = mach_port_insert_right( mach_task_self(),
                                             fp_info->revoke_port,
                                             fp_info->revoke_port,
                                             MACH_MSG_TYPE_MAKE_SEND);

	if (mach_error != KERN_SUCCESS) {
		EPRINT(("pfs_init_tokenmgr: mach_port_allocate%s\n",
               		 mach_error_string(mach_error)));

		mach_port_deallocate(mach_task_self(), 
				     fp_info->revoke_port);
		return(EIO);
	}

	/*
	 * Put the new port into the callback_port_set so that
	 * the callback thread recognizes it.   This will enable
	 * it to be used for the server side of the RPC's.
	 */
	mach_error = mach_port_move_member(mach_task_self(), 
					   fp_info->revoke_port,
                                   	   callback_port_set);
	if (mach_error != KERN_SUCCESS) {
		mach_port_deallocate(mach_task_self(), 
				     fp_info->revoke_port);
		EPRINT(("pfs_init_tokenmgr: mach_port_dallocate%s\n", 
			mach_error_string(mach_error)));
		return (EIO);
	}

	fdte->pfs_iomode_info->revoke_port = revoke_port;

#ifdef DEBUG_SETIO
	e_printf("[%d], revoke_port = %d\n", 
			fdte->pfs_iomode_info->my_node_number,
			fp_info->revoke_port);
#endif
	if (fdte->pfs_iomode_info->my_node_number == 0) {
		fp_info->fdte_ref = (int)fdte;
		mach_error = mach_port_allocate(mach_task_self(),
						MACH_PORT_RIGHT_RECEIVE,
						&fp_info->tmgr_id_port);

		if (mach_error != KERN_SUCCESS) {
			e_printf("pfs_init_token_mgr: port_alloc, %s\n",
				 mach_error_string(mach_error));
			mach_port_deallocate(mach_task_self(), 
					     fp_info->revoke_port);
			return (EIO);
		}

		/*
		 * Insert a send right into the port:
		 */
		mach_error = mach_port_insert_right (mach_task_self(),
						fp_info->tmgr_id_port,
						fp_info->tmgr_id_port,
						MACH_MSG_TYPE_MAKE_SEND);

		if (mach_error != KERN_SUCCESS) {
			mach_port_deallocate(mach_task_self(), 
					     fp_info->revoke_port);
			mach_port_deallocate(mach_task_self(), 
					     fp_info->tmgr_id_port);
			e_printf("pfs_init_token_mgr: insert_right%s\n",
				mach_error_string(mach_error));
				return EIO;
		}
	}
	return error;
}



/*
 * NAME:	pfs_conn_token_mgr
 *
 *
 * DESCRIPTION:
 *		This function is used to initialize the PFS
 *		i/o mode manager thread after the exchange
 *		of global information by the libnx library.  
 *		The main functionality consists of establishing
 *		the Mach port rights for the internode connections
 *		and also starting up the pfs_iomode_manager thread.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		fp_info		Pointer to where the file information
 *				 will be stored.
 *
 *		fp_info_size	Size of each of the file information
 *				entries.
 *
 *		task0_port	Task ports to node 0.
 *
 *		comm_info	Pointer to comm info.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
int
pfs_conn_token_mgr( fdte, fp_info, fp_info_size, task0_port, comm_info)
fdt_entry_t	*fdte;			/* File descriptor entry */
pfs_fp_info_t	fp_info[];		/* Global fp information */
int		fp_info_size;		/* Size, in bytes, of fp_info */
mach_port_t	task0_port;		/* Task port of node 0's process */
pfs_comm_info_t comm_info[];		/* Comm info from setiomode. */
{
	int error = ESUCCESS;
	kern_return_t			mach_error;
	mach_msg_type_name_t    	new_type;
	pfs_token_mgr_info 		*tmgr_info;
	int				num_nodes, mynode;
	int				i;
	char 				*fp_info_ptr;
	boolean_t			interrupt;			
	mach_port_t			task_port;


	/*
	 * Only the M_RECORD I/O mode uses the token manager since
	 * they it is the only modes that require parallel access.
	 */
	if (!PFS_TOKENMGT(fdte)) {
		return ESUCCESS;
	}

	mynode    = fdte->pfs_iomode_info->my_node_number;
	num_nodes = fdte->pfs_iomode_info->number_of_nodes;

	fdte->pfs_iomode_info->tmgr_id_port  = MACH_PORT_NULL;
	fdte->pfs_iomode_info->tmgr_req_port = MACH_PORT_NULL;

	if (mynode == 0) {
		/*
	 	 * Malloc a new pfs_token_mgt_t structure to
	 	 * hold the token information.   This structure
		 * is variable size due to the # of revoke_ports
		 * and token_state entries being dependent on the 
		 * number of nodes in the application.
		 */
		tmgr_info = (pfs_token_mgr_info *)
			    malloc(sizeof(pfs_token_mgr_info) +
				   (sizeof(clnt_tok_info) * num_nodes));

#ifdef DEBUG_SETIO
	e_printf("in pfs_conn_token_mgr: tmgr_info = %x\n",
		  tmgr_info);
#endif
        	if (tmgr_info == NULL) { 
                	error = ENOMEM;
			goto err_ret;
		}

		fdte->pfs_iomode_info->token_mgr_info = tmgr_info;
		for (i=0; i<num_nodes; i++) {
			tmgr_info->clnt_tok[i].revoke_ports = MACH_PORT_NULL;
		}
		tmgr_info->token_cnt = num_nodes;
		/*
		 * Copy the fdte information into the tmgr_info structure:
		 */
		fdte_lock(fdte);
		setup_tmgr_from_fdte(fdte);
		fdte_unlock(fdte);
		/*
		 * Get port rights to all of the revoke ports.
		 */
		fp_info_ptr  = (char *)&fp_info[0];
		for (i=0; i<num_nodes; i++ ) {
#ifdef DEBUG_SETIO
	e_printf("[%d], revoke_ports = %d\n",
			i, ((pfs_fp_info_t *)fp_info_ptr)->revoke_port);
#endif

			emul_blocking();
			bsd_task_by_pid(our_bsd_server_port,
					&interrupt,
					comm_info[i].unix_pid,
					&task_port);
			emul_unblocking();

	
        		mach_error = mach_port_extract_right(
				task_port, 
				((pfs_fp_info_t *)fp_info_ptr)->revoke_port,
                                MACH_MSG_TYPE_COPY_SEND,
                                &tmgr_info->clnt_tok[i].revoke_ports,
                               	&new_type);
#ifdef DEBUG_SETIO
	e_printf("tmgr_info->cntl_tok[%d].revoke_ports = %d\n",
			i, tmgr_info->clnt_tok[i].revoke_ports);
#endif
			mach_port_deallocate(mach_task_self(),
					     task_port);

        		if (mach_error != KERN_SUCCESS) {
                	  EPRINT(("pfs_conn_token_mgr: extract_right%s\n",
                         	   mach_error_string(mach_error)));
                	  error = EIO;
			  goto err_ret;
			}
			fp_info_ptr += fp_info_size;
		}
		/*
	 	 * Check to see if we have to token acquired, if not reacquire
		 * it. This is necessary due to the emul_blocking() calls
		 * around the bsd_task_by_pid() function.  It is possible that
		 * this task was suspended during the function call which
		 * resulted in the token being taken away by release_all_tokens.
		 */
		fdte_lock(fdte);
		if (!tmgr_info->flags) {
			esize_t offset, length;
			fdte_unlock(fdte);
			/*
			 * Reacquire the token from the server:
			 */
                	error = tmgr_token_acquire_from_server(fdte,
							       &offset,
                                                    	       &length);
		} else {
			fdte_unlock(fdte);
		}
		/*
		 * Mark the state of the client tokens as acquired:
		 */
		for (i=0; i<num_nodes ; i++) {
			tmgr_info->clnt_tok[i].token_state = PFS_TOKEN_ACQUIRED;
		}
	}
	fdte->pfs_iomode_info->fdte_ref = fp_info[0].fdte_ref;
	/*
	 * Get a send right the the iomode_manager request port,
	 * this send right will be used to identify the port
	 * during the token_revoke operation.
	 * 
	 */
	mach_error = mach_port_extract_right(task0_port, 
					   fp_info[0].tmgr_id_port,
                                	   MACH_MSG_TYPE_COPY_SEND,
                                	   &fdte->pfs_iomode_info->tmgr_id_port,
                                	   &new_type);

	if (mach_error != KERN_SUCCESS) {
		EPRINT(("pfs_conn_token_mgr: extract_right token_mgr%s\n",
			 mach_error_string(mach_error)));
		error = EIO;
		goto err_ret;
	}

	/*
	 * The revoke port for node zero is also used as the 
	 * request port.
	 */
	mach_error = mach_port_extract_right(task0_port, 
					  fp_info[0].revoke_port,
                                	  MACH_MSG_TYPE_COPY_SEND,
                                	  &fdte->pfs_iomode_info->tmgr_req_port,
                                	  &new_type);

	if (mach_error != KERN_SUCCESS) {
		EPRINT(("pfs_conn_token_mgr: extract_right token_mgr%s\n",
			 mach_error_string(mach_error)));
		error = EIO;
		goto err_ret;
	}

	/*
	 * Set up the token info in the fdte entry to reflect the 
	 * new shared token.
	 */
	/*
	 * Mark token as revokable, since only the token_mgr will 
	 * be doing this:
	 */
	if (mynode) { 
		fdte->token = fdte->pfs_iomode_info->tmgr_id_port;
	} else {
		fdte->pfs_iomode_info->token_mgr_info->client_token = 
			fdte->pfs_iomode_info->tmgr_id_port;
	}
#ifdef DEBUG_SETIO
	e_printf("[%d]conn_token_mgr returning ok\n",mynode);
#endif
        fdte_lock(fdte);
	fdte->can_revoke = 1;
	fdte->flags = TOK_OFFSET | TOK_DATA_WRITE;

	if (fdte->must_release) {
	  tmgr_release_to_server(fdte, 1);
	  fdte->must_release = 0;
	}

	fdte->token_refcnt = 0;
	fdte_unlock(fdte);
	return ESUCCESS;

err_ret:
	if (fdte->pfs_iomode_info->tmgr_req_port != MACH_PORT_NULL) {
		mach_port_deallocate(mach_task_self(),
			fdte->pfs_iomode_info->tmgr_req_port);
	}

	if (fdte->pfs_iomode_info->tmgr_id_port != MACH_PORT_NULL) {
		mach_port_deallocate(mach_task_self(),
			fdte->pfs_iomode_info->tmgr_id_port);
	}


	if (tmgr_info) {
		/*
		 * See what needs to be released to clean up from an
		 * error.
		 */
		for(i=0; i< num_nodes; i++ ) {
			if (tmgr_info->clnt_tok[i].revoke_ports != 
			    MACH_PORT_NULL) {
				mach_port_deallocate(mach_task_self(),
						tmgr_info->clnt_tok[i].revoke_ports);
			}
		}
		
		if (fdte->token == PFS_TOKEN_MAGIC) {
	       		fdte->token = tmgr_info->server_token;
		}

		free((void *)tmgr_info);

	} 

	fdte_lock(fdte);
	fdte->can_revoke = 1;
	if (fdte->must_release) {
	  tmgr_release_to_server(fdte, 1);
	  fdte->must_release = 0;
	}
	fdte_unlock(fdte);

	return error;
} /* pfs_conn_token_mgr */


/*
 * NAME:	pfs_rlse_token_mgr
 *
 *
 * DESCRIPTION:
 *		This function is used to release the resources
 *		associated with the PFS i/o mode manager.  These
 *		resources differ depending on the locality of 
 *		the process, i.e. node 0's process does more work
 *		than node 1..N's processes due to the token manager
 *		residing on node 0.  This function is typically called
 *		during the closing of the file.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 *		issig		Indicates if being called due to
 *				a signal or from the normal (close)
 *				path.
 *
 * RETURNS:
 *
 *		ESUCCESS 	- If successful.
 *		error number	- If error occurred.
 *
 */
void
pfs_rlse_token_mgr( fdte, issig )
fdt_entry_t	*fdte;			/* File descriptor entry */
int		issig;			/* Indicates if updating due to
					 * a signal exit or normal. 
					 */
{
	pfs_token_mgr_info	*token_info;
	int			i;


#ifdef DEBUG_SETIO
	e_printf("pfs_rlse_token_mgr: fdte->pfs_iomode_info->my_node_number = %d\n", fdte->pfs_iomode_info->my_node_number);
#endif

	if (fdte->pfs_iomode_info->my_node_number == 0) {
		token_info = fdte->pfs_iomode_info->token_mgr_info;
		/*
		 * See if the token manager has the token:
		 */
		fdte->token = token_info->server_token;
#ifdef DEBUG_SETIO
	e_printf("pfs_rlse_token_mgr: token_info->flags = %d, fdte = %x\n",
		token_info->flags, fdte);
#endif
		if (token_info->flags) {
			/*
		 	 * Update the fdte information from the token
		 	 * manager information.  If we are holding onto
		 	 * the token, the token will be released by the
		 	 * fdte_unref_entry() call after this function
		 	 * returnes.
		 	 */
			fdte->flags 		= token_info->flags;
			fdte->accessed 		= token_info->accessed;
			fdte->modified 		= token_info->modified;
			fdte->must_release	= token_info->must_release;
			fdte->can_revoke	= token_info->can_revoke;
			/*
			 * Update the length:
			 */
#ifdef DEBUG_SETIO
	e_printf("[%d], token_info->length = %d,%d, offset = %d,%d\n",
			fdte->pfs_iomode_info->my_node_number,
			token_info->length.shigh,
			token_info->length.slow,
			token_info->offset.shigh,
			token_info->offset.slow);
#endif
			if (fdte->iomode == VIO_MAPPED){
				if (issig) {
					/*
					 * Length is MAX of fdte->length.
					 */
					if (fdte->length < 
					    token_info->length.slow) {
						fdte->length = 
							token_info->length.slow;
					}
				} else { 
					fdte->length = token_info->length.slow;
				}
				fdte->max_offset = token_info->max_offset;
				fdte->offset     = token_info->offset.slow;
#ifdef DEBUG_SETIO
				e_printf("[%d], fdte:length = %d, offset = %d,min_offset = %d, max_offset = %d, modified: %d, accessed: %d\n",
					fdte->pfs_iomode_info->my_node_number,
					fdte->length, fdte->offset,
					fdte->min_offset, fdte->max_offset,
					fdte->modified, fdte->accessed);
#endif
			} else if (fdte->pfs_fd){
				if (issig) {
					/*
					 * Set the length to VNOVAL,VNOVAL
					 * to allow the length to be recomputed
					 * when reacquired.
					 */
					fdte->pfs_fd->p_length.shigh = VNOVAL;
					fdte->pfs_fd->p_length.slow  = VNOVAL;
					fdte->modified = 1;
				} else {
					fdte->pfs_fd->p_length = 
						token_info->length;
				}
#ifdef DEBUG_SETIO
				e_printf("[%d], length = %d,%d, offset = %d,%d\n",
					fdte->pfs_iomode_info->my_node_number,
					fdte->pfs_fd->p_length.shigh,
					fdte->pfs_fd->p_length.slow,
					fdte->pfs_fd->p_offset.shigh,
					fdte->pfs_fd->p_offset.slow);
#endif
			}
			/*
			 * Deallocate the port resources:
			 */
			mach_port_deallocate(mach_task_self(),
					    fdte->pfs_iomode_info->tmgr_id_port);

			mach_port_deallocate(mach_task_self(),
					     token_info->client_token);

			for(i=0; i<fdte->pfs_iomode_info->number_of_nodes; i++){
				mach_port_deallocate(mach_task_self(),
						  token_info->clnt_tok[i].revoke_ports);
			}
		}

		/*
		 * Release the token manager state information
		 * from the fdte table.
		 */
		free((void *)token_info);
		fdte->pfs_iomode_info->token_mgr_info = NULL;

		/*
		 * If token was acquired, release it back to the server:
		 */
		if (fdte->flags) {
#ifdef DEBUG_SETIO
	e_printf("Releasing the token back to the server, fdte->must_release = %d\n", fdte->must_release);
#endif
			if (fdte->must_release) {
				token_release_to_server(fdte,TRUE);
			} else {
				token_release_to_server(fdte,FALSE);
			}
		}
	} else {
		/*	
	 	 * Non zero nodes.
	 	 */ 
		if (fdte->flags) {

			mach_port_deallocate(mach_task_self(),
					     fdte->token);

			fdte->flags 		= 0;
			fdte->accessed  	= 0;
			fdte->modified		= 0;
			fdte->must_release 	= 0;
		}

		mach_port_deallocate(mach_task_self(),
				     fdte->pfs_iomode_info->tmgr_id_port);
		mach_port_deallocate(mach_task_self(),
				     fdte->pfs_iomode_info->tmgr_req_port);
		mach_port_deallocate(mach_task_self(),
				     fdte->pfs_iomode_info->revoke_port);
	}
}


/*
 * NAME:	tmgr_revoke
 *
 *
 * DESCRIPTION:
 *		This function is used to perform the revoke operation 
 *		when using the token manager logic. This function is 
 *		entered with the fdte_lock held.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry.
 *
 * RETURNS:
 *
 *		Nothing.
 *
 */
void
tmgr_revoke(fdte)
fdt_entry_t	*fdte;

{
	kern_return_t		mach_error;
	pfs_token_mgr_info	*info;
	int			node;
	int			node_cnt;
	int			revoke_cnt = 0;
	mach_port_t		port, token;
	esize_t			offset,length;

	info = fdte->pfs_iomode_info->token_mgr_info;
#ifdef DEBUG_REVOKE
	e_printf("tmgr_revoke:, fdte = %x, info->flags = %x, info->can_revoke = %d, info->token_cnt = %d\n",
		fdte,
		info->flags,
		info->can_revoke,
		info->token_cnt);
#endif

	if(!info->can_revoke) {
		info->must_release = 1;
		return;
	}

	/*
	 * See if the token can be revoked from the local node first.
	 * If so, then go through the list of other node clients and 
	 * revoke the token.  Otherwise, we will have to wait until
	 * the local node releases the token to revoke it from the 
	 * other nodes.
	 */
	if (!fdte->can_revoke) {
		/*
		 * If we cannot revoke the token, then just set the
		 * must release flag to inform the other thread to 
		 * release the token when done (and tell the other
		 * clients to do the same).  The state of the token
		 * manager is set the RIP so that no more acquires
		 * sneak in while we are waiting to revoke.
		 */
		info->tmgr_token_state = PFS_TOKEN_RIP; 
		fdte->must_release = 1;
		return;
	} else {
		/* 
		 * Mark my local state as released.	
		 */
		info->clnt_tok[0].token_state = PFS_TOKEN_RELEASED;
                info->accessed |= fdte->accessed;
                info->modified |= fdte->modified;
                info->token_cnt--;
		if (fdte->iomode == VIO_MAPPED) {
			offset.shigh = 0;
			offset.slow  = fdte->offset;
			length.shigh = 0;
			length.slow = fdte->length;
		} else if (fdte->pfs_fd) {
			offset = fdte->pfs_fd->p_offset;
			length = fdte->pfs_fd->p_length;
		}
                info->offset = EMAX(info->offset, offset);
                info->length = EMAX(info->length, length);
		/*
		 * Update local token info:
		 */
		fdte->flags 	= 0;
		fdte->accessed  = 0;
		fdte->modified  = 0;
		fdte->must_release = 0;

	}
	/*
	 *  See if any clients have the token, if so
	 *  send the clients a revoke message and set
	 *  token manager state to indicate that a 
	 *  revoke is in progress and to expect a 
	 *  release request from each client prior
	 *  to releasing the token.
	 */
	if (info->token_cnt) {		

		info->tmgr_token_state = PFS_TOKEN_RIP;
		/*
		 *  Use the PFS I/O mode to determine which
		 *  clients will get the revoke message(s).
		 */
		node = 0;
		node_cnt = info->token_cnt;

		/*
		 * Go through the list of states and send revoke
		 * messages to each client that has the token.  
 		 * Remember the number of revoke messages that
		 * were sent so that the token can be released
		 * after the last release message arrives.
		 */
		while(node_cnt) {
#ifdef DEBUG_REVOKE
	e_printf("tmgr_revoke: node [%d] token_state = %d\n",
			node, info->clnt_tok[node].token_state);
#endif
			
			if (info->clnt_tok[node].token_state == 
			    PFS_TOKEN_ACQUIRED) {
				/*
				 * Send the revoke request to the node.
				 */
				port = info->clnt_tok[node].revoke_ports;
			        token = fdte->pfs_iomode_info->tmgr_id_port;
				/*
				 * Need to make a send right because the
				 * uemul_token_revoke() RPC causes the
				 * send right to be moved to the target.
				 * This way the token is held onto.
				 */
				mach_port_insert_right(mach_task_self(),
						token,
						token,
						MACH_MSG_TYPE_MAKE_SEND);

#ifdef DEBUG_REVOKE
	e_printf("tmgr_revoke: Sending revoke to node %d\n",node);
#endif
                       		mach_error = uemul_token_revoke(port, token);
				if (mach_error != KERN_SUCCESS) {
			      		EPRINT(("tmgr_token_revok error = %s\n",
 				       		mach_error_string(mach_error)));
				} else {
					info->clnt_tok[node].token_state = 
							PFS_TOKEN_RIP;
					revoke_cnt++;
				}
				node_cnt--;
			} /* end if (info->token_state) */
			node++;
		} /* end while(node_cnt) */
#ifdef DEBUG_REVOKE
	e_printf("tmgr_revoke: revoke_cnt = %d\n",revoke_cnt);
#endif

		info->token_cnt = revoke_cnt;

	} else {
		/*
		 * No clients are holding onto the token, so it
		 * can be released back to the server. 
		 */
		info->tmgr_token_state = PFS_TOKEN_RELEASED;
		tmgr_release_to_server(fdte, 1);
	}/* end if (info->token_cnt) */		
}


/*
 * NAME:	tmgr_token_acquire_from_server
 *
 *
 * DESCRIPTION:
 *		This function is used by the token manager thread to 
 *		reacquire a token from the server.  This function is 
 *		only used by PFS I/O mode M_RECORD to
 *		reacquire a token after it has been revoked.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		offset		New value of offset.
 *
 *		length		New value of file length.	
 *
 * RETURNS:
 *
 *		ESSUCCESS, if successful.
 *		ERRNO 	otherwise.
 */
int tmgr_token_acquire_from_server(fdte, offset, length) 
fdt_entry_t	*fdte;
esize_t		*offset;
esize_t		*length;
{
	pfs_token_mgr_info              *info;
	int				error = ESUCCESS;
	int				flags;


	info = fdte->pfs_iomode_info->token_mgr_info;
	flags = TOK_OFFSET | TOK_DATA_WRITE;

#ifdef DEBUG_TOKEN
	e_printf("tmgr acquire from server\n");
#endif
	/*
	 * 
	 */
	fdte_lock(fdte);

	if (fdte->iomode == VIO_MAPPED) {
		/*
		 * Mapped file token acquire:
		 */
		offset->shigh = 0;
		length->shigh = 0;
		error = fsvr_token_acquire(fdte->fp,
					   credentials_port,
					   flags,
					   revoke_port,
					   &info->server_token,
					   (int *)&offset->slow,
					   (int *)&length->slow);
	} else if (fdte->pfs_fd) {
		/*
		 * PFS token acquire:
		 */
               	error = fsvr_pfs_token_acquire(	fdte->fp, 
						credentials_port,
                                               	flags, 
						revoke_port, 
						&info->server_token,
                                               	offset,
                                               	length);
	}

	if (!error) {
		info->offset = *offset;
		info->length = *length;
		info->flags  = flags;
		info->tmgr_token_state = PFS_TOKEN_ACQUIRED;
	}

	fdte_unlock(fdte);
	return error;
}


/*
 * NAME:	tmgr_release_to_server
 *
 *
 * DESCRIPTION:
 *		This function is used to release the token held by the
 *		token manager back to the server.  This function is only
 *		called if the token has been revoked.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *		revoked		Set to 1 if being revoked 0 if not.
 *
 * RETURNS:
 *
 *		Nothing.
 */
void tmgr_release_to_server(fdte, revoked)
	fdt_entry_t	*fdte;
	int		revoked;
{
	kern_return_t		mach_error;
        pfs_token_mgr_info	*info;

	/* 
	 * The token is held and it can be revoked.
	 */
        info = fdte->pfs_iomode_info->token_mgr_info;
	info->tmgr_token_state = PFS_TOKEN_RELEASED;
#ifdef DEBUG_TOKEN
	e_printf("tmgr_release_to_server, offset = (%d,%d), length = (%d,%d), accessed = %d, modified = %d\n",
		info->offset.shigh,
		info->offset.slow,
		info->length.shigh,
		info->length.slow,
		info->accessed,
		info->modified);
#endif

	if (fdte->pfs_fd) {
		/* 
	 	 * Send an async. message to release the token. 
	 	 * We have at least one send right from the acquire, and
		 * perhaps another (due to a revoke), that are "deallocated"
		 * by moving them to the server.  
	 	 */
		mach_error = fsvr_pfs_token_release(	info->server_token,
							info->server_token,
							revoked ? 
							  info->server_token:
							  MACH_PORT_NULL, 
							info->offset, 
					   		info->length, 
							info->accessed,
							info->modified);
		if (mach_error) {
			EPRINT(("tmgr: fsvr_pfs_token_release error = 0x%x",
				 mach_error));
		}
		info->flags = 0;       /* indicates no token is held */
		info->accessed = 0;
		info->modified = 0;
		info->must_release = 0;
		return;
	} else {
		/* 
		 * Send an async. message to release the token. 
		 * We have at least one send right from the acquire, and 
		 * perhaps another (due to a revoke), that are "deallocated" 
		 * by moving them to the server.  
	  	 */
		if (info->offset.slow <  info->min_offset) {
			info->min_offset = info->offset.slow;
		}

		if (info->offset.slow > info->max_offset) {
			info->max_offset = info->offset.slow;
		}

		mach_error = fsvr_token_release( info->server_token,
						 info->server_token,
					   	 info->server_token,
					   	 info->offset.slow, 
					   	 info->length.slow, 
					   	 info->accessed, 
						 info->modified,
					   	 info->min_offset, 
						 info->max_offset);
		if (mach_error) {
			EPRINT(("tmgr: fsvr_token_release error = 0x%x",
				mach_error));
		}

		info->flags = 0;       /* indicates no token is held */
		info->min_offset = INT_MAX;
		info->max_offset = 0;
		info->accessed = 0;
		info->modified = 0;
		info->must_release = 0;
	}
}


/*
 * NAME:	setup_tmgr_from_fdte
 *
 *
 * DESCRIPTION:
 *		This function is used to update the token manger
 *		fdte token information from the information in the
 *		fdte.  It basically copies the token state information
 *		from the standard fdte info.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 * RETURNS:
 *
 *		Nothing.
 */
static void
setup_tmgr_from_fdte(fdte)
	fdt_entry_t	*fdte;
{
	
	pfs_token_mgr_info      *info = fdte->pfs_iomode_info->token_mgr_info;

	/*
	 * Copy the fdte information into the tmgr_info structure:
	 */
        info->server_token = fdte->token;	  /* Copy server port. */

	fdte->token = PFS_TOKEN_MAGIC;  /* Put magic value in for
       					 * the callback thread.
					 */

	info->flags		= fdte->flags;
	info->accessed 		= fdte->accessed;
	info->modified		= fdte->modified;
	info->must_release	= fdte->must_release;
	info->can_revoke	= 1;

	if (fdte->pfs_fd) {
		info->offset = fdte->pfs_fd->p_offset;
		info->length = fdte->pfs_fd->p_length;
	} else {
		info->offset.shigh = 0;
		info->offset.slow  = fdte->offset;
		info->length.shigh = 0;
		info->length.slow  = fdte->length;
		info->min_offset   = fdte->min_offset;
		info->max_offset   = fdte->max_offset;
	}
	info->tmgr_token_state = PFS_TOKEN_ACQUIRED;
}


/*
 * NAME:	tmgr_token_acquire
 *
 *
 * DESCRIPTION:
 *		This function is used by a client to reacquire a token 
 *		from the token manager.  This function is only used in
 *		PFS I/O mode M_RECORD to reacquire a 
 *		token after it has been revoked.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 *		offset		New value of offset.
 *
 *		length		New value of file length.	
 *
 * RETURNS:
 *
 *		ESSUCCESS, if successful.
 *		ERRNO 	otherwise.
 */
int 
tmgr_token_acquire(fdte, offset, length)
	fdt_entry_t	*fdte;
	esize_t		*offset;
	esize_t		*length;
{
        pfs_fd_t        *pfs_fd = fdte->pfs_fd;


        fdte_lock(fdte);
        fdte->can_revoke = 0;           /* keep revokers away */
	fdte->token_refcnt++;		/* Increment the reference count. */
        fdte_unlock(fdte);

#ifdef DEBUG_TOKEN
	e_printf("acquiring token from manager: flags = %d\n",fdte->flags);
#endif
        if ( fdte->flags ) {   
		if (fdte->iomode == VIO_MAPPED) {
			length->slow 	= fdte->length;
			length->shigh	= 0;
			offset->slow	= fdte->offset;
			offset->shigh 	= 0;
		} else if (fdte->pfs_fd) {
                        length->slow    =  pfs_fd->p_length.slow;
                        length->shigh   =  pfs_fd->p_length.shigh;
			offset->slow  	= pfs_fd->p_offset.slow;
			offset->shigh 	= pfs_fd->p_offset.shigh;
                }
		return ESUCCESS;

	} else {
		/*
		 * Need to re-acquire the token from the token manager thread:
		 * The uemul_token_acquire is in a loop because there are
		 * sometimes races between the time the token is revoked
		 * and reacquired from multiple clients.  If the offset 
		 * is returned back as -1,-1 then we must retry the 
		 * the call.
		 */
#ifdef DEBUG_REVOKE
	e_printf("calling uemul_token_acquire\n");
#endif
		*offset = ex_neg_one;
		while (EQUAL(*offset, ex_neg_one)) { 
			uemul_token_acquire(
				fdte->pfs_iomode_info->tmgr_req_port,
				fdte->pfs_iomode_info->fdte_ref,
				fdte->pfs_iomode_info->my_node_number,
				offset,
				length);
		}
	}
	/*
	 * Send back the real offset, only the local offset is used
	 * in M_RECORD mode.
	 */
#ifdef DEBUG_REVOKE
	e_printf("after uemul_token_acquire\n");
#endif

	if (fdte->iomode == VIO_MAPPED) {
		offset->slow  = fdte->offset;
		offset->shigh = 0;
	} else if (fdte->pfs_fd) {
		offset->slow  = pfs_fd->p_offset.slow;
		offset->shigh = pfs_fd->p_offset.shigh;
	}
        fdte_lock(fdte);        /* synchronize with revocation */
        fdte->flags = TOK_DATA_WRITE | TOK_OFFSET;
        fdte_unlock(fdte);
#ifdef DEBUG_REVOKE
	e_printf("fdte->flags = %d\n",fdte->flags);
#endif
	return (ESUCCESS);

}


/*
 * NAME:	token_release_to_tmgr
 *
 *
 * DESCRIPTION:
 *		This function is used by a client to release a token 
 *		back to the token manager.  This function is only used in
 *		PFS I/O mode M_RECORD to release a 
 *		token after it has been revoked.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *
 * RETURNS:
 *
 *		ESUCCESS, if successful.
 *		ERRNO 	otherwise.
 */
int 
token_release_to_tmgr(fdte)
	fdt_entry_t	*fdte;
{
	esize_t	 offset;
	esize_t	 length;

	int      error = ESUCCESS;


#ifdef DEBUG_REVOKE
	e_printf("token_release_to_tmgr\n");
#endif

	if (fdte->iomode == VIO_MAPPED) {
		offset.shigh = 0;
		offset.slow = fdte->offset;
		length.shigh = 0;
		length.slow = fdte->length;

	} else if (fdte->pfs_fd) {
		offset = fdte->pfs_fd->p_offset;
		length = fdte->pfs_fd->p_length;
	}

#ifdef DEBUG_REVOKE
	e_printf("calling uemul_token_release\n");
#endif
	/*
	 * If not on node zero, then we need to send an RPC to node
	 * zero to indicate that the token has been releases.  If 
	 * on node zero, then we must send the appropriate revoke 
	 * messages to the other client nodes and have the callback
	 * thread receive the replies.
	 */
	if (fdte->pfs_iomode_info->my_node_number) {
		error = uemul_token_release(
					fdte->pfs_iomode_info->tmgr_req_port,
					fdte->pfs_iomode_info->fdte_ref,
					fdte->pfs_iomode_info->my_node_number,
					fdte->accessed,
					fdte->modified,
					offset,
					length);
		if (error) {
       			EPRINT(("token_release_to_tmgr error = 0x%x",error));
		}

	} else {
		/* Mark this node as revoked and then send revoke messages
		 * to the other nodes in the application that are holding onto
		 * the token. 
		 */
		fdte_lock(fdte);
		fdte->can_revoke = 1;
		tmgr_revoke(fdte);
		fdte_unlock(fdte);
	}
	fdte->flags = 0;       /* indicates no token is held */
	fdte->accessed = 0;
	fdte->modified = 0;
	fdte->must_release = 0;
	/*
	 * Need to release the extra send right to token_id since
	 * the callback thread on node zero put one there when the
	 * uemul_token_revoke() RPC was called. 
	 */
	mach_port_mod_refs(mach_task_self(),
			   (fdte->pfs_iomode_info->my_node_number ? 
			   	fdte->token :
			   	fdte->pfs_iomode_info->tmgr_id_port),
			   MACH_PORT_RIGHT_SEND,
			   -1);

	return error;
}



/*
 * NAME:	pfs_get_offlen
 *
 *
 * DESCRIPTION:
 *		This function is used to get the offset and length 
 *		of a non token controlled file, i.e offset and length
 *		kept at the server.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *		offset		Pointer to file offset.
 *		length		Pointer to file length.
 *
 * RETURNS:
 *
 *		ESSUCCESS, if successful.
 *		ERRNO 	otherwise.
 */
int 
pfs_get_offlen(fdte, offset, length)
fdt_entry_t	*fdte;
esize_t	 *offset;
esize_t	 *length;
{
	transaction_id_t        trans_id;
	int                     retval;
	struct stat             statbuf;
	boolean_t		interrupt;
	int			error=0;
	/*
	 * Get the current offset:
	 */
	isc_register_chk_async(fdte->fp, &trans_id);

	error = fsvr_lseek(fdte->fp, credentials_port, trans_id, 0, L_INCR,
			   &retval);

        isc_deregister(&interrupt);

	if (error) {
		offset->slow  = 0;
		offset->shigh = 0;
		return error;
	} else if (retval < 0) {
		offset->slow  = 0;
		offset->shigh = 0;
		error = retval;
		return error;
	}

	offset->slow = retval;
	offset->shigh = 0;

	/*
	 * Get the length of the file:
	 */
	isc_register_chk_async(fdte->fp, &trans_id);
	error = fsvr_fstat(fdte->fp, credentials_port, trans_id, &statbuf);
	isc_deregister(&interrupt);
	if (!error) {
		length->slow = statbuf.st_size;
		length->shigh = 0;
	} else {
		length->slow = 0; 
		length->shigh = 0; 
	}
	return error;
}


/*
 * NAME:	pfs_setoff
 *
 *
 * DESCRIPTION:
 *		This function is used to set the offset 
 *		of a non token controlled file, i.e offset and length
 *		kept at the server.
 *
 * PARAMETERS:
 *		fdte		File descriptor table entry.
 *		offset		Pointer to file offset.
 *
 * RETURNS:
 *
 *		ESSUCCESS, if successful.
 *		ERRNO 	otherwise.
 */
int 
pfs_setoff(fdte, offset)
fdt_entry_t	*fdte;
esize_t	 *offset;
{
	transaction_id_t        trans_id;
	int                     retval;
	boolean_t		interrupt;
	int			error;


	/*
	 * If offset is equal to -1, then leave the offset alone.
	 */
	if (EQUAL(*offset, ex_neg_one)) {
		return 0;
	}
	/*
	 * Set the current offset:
	 */
	isc_register_chk_async(fdte->fp, &trans_id);
	error = fsvr_lseek(fdte->fp, credentials_port, trans_id, offset->slow,
			    L_SET, &retval);
	isc_deregister(&interrupt);
	if (error) {
		return error;
	} else if (retval < 0) {
		return retval;
	} 
	return 0;
}     
#endif	PFS
