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
 * HISTORY
 * $Log: pfs_user_side.c,v $
 * Revision 1.34  1995/04/04  18:46:58  rlg
 * The gopen step (for the M_UNIX, M_RECORD, and M_ASYNC I/O modes) of
 * duplicating the file server's file table was redesigned.  This change
 * results in only one "many to one" messages (sent from N-1 compute nodes
 * to logical node 0 in the application's partition) and one set of "one to
 * many" messages (sent from each file server to those N-1 compute nodes).
 * The old code required each of the compute nodes in the application's
 * partition (minus one) to execute a sequence of four RPCs to the file
 * servers containing the header file and each stripe file.  This resulted
 * in four "many to one" messages sent to each of those file servers.
 *
 *  Reviewer:  Balaji Narasimhan, Stan Smith
 *  Risk:      Medium (number of modules changed, scope of redesign)
 *  PTS#       9637
 *  Testing:   pfs and fileio EATs, five Eval gopen tests,
 *             rw and iomode integration tests
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnx/_gopen.c
 *             cmds_libs/src/usr/ccs/lib/libnx/pfs_iomode.h
 *             server/svr/src/svr/emulator/pfs_user_side.c
 *             server/svr/src/svr/server/conf/syscalls.master
 *             server/svr/src/svr/server/uxkern/fsvr.defs
 *             server/svr/src/svr/server/uxkern/fsvr_server_side.c
 *             server/svr/src/svr/server/uxkern/pfs2.defs
 *             server/svr/src/svr/server/uxkern/pfs2_server_side.c
 *
 * Revision 1.33  1995/03/08  21:51:46  stans
 *  'pfs_scanpath()' now checks for <null> string arguments.
 *   Updated function comments to identify return codes are used.
 *
 *  Reviewer:terry,jlitvin
 *  Risk:low
 *  Benefit or PTS #:12606
 *  Testing:WW09 sats + bug specific tests; unlink(0), etc.
 *
 * Revision 1.32  1995/03/02  18:49:11  stans
 *  Vnode caching support: pfs_host_init() syscall.
 *
 *  Reviewer:jlitvin,suri,cfj
 *  Risk:medium
 *  Benefit or PTS #:8129
 *  Testing: WW07 sats
 *
 * Revision 1.31  1994/11/18  20:24:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.30  1994/06/13  15:25:33  rlg
 * Added the M_ASYNC I/O mode for shared files.  This mode is characterized by:
 *     o	each node has a unique file pointer,
 *     o	nodes are not synchronized
 *     o	file access is unrestricted
 *     o	standard UNIX file sharing semantics requiring atomicity of I/O
 * 		are not preserved.
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
 * Revision 1.29  1994/02/18  00:55:59  brad
 * Merged revision 1.25.2.3 from the R1.2 branch.
 *
 * Revision 1.25.2.3  1994/02/18  00:46:22  brad
 * Fixed improper initialization of extended ex_neg_int_max variable.
 *
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 8162
 *  Testing: Ran developer tests, PTS test, and PFS EATs on 8 nodes.
 *  Module(s): emulator/pfs_user_side.c
 *
 * Revision 1.25.2.2  1994/01/09  00:20:41  brad
 * Fixed bug found by lint (wrong number pf params to pfs_free() in
 * pfs2_user_side.c); also fixed lint warnings in PFS-related code.
 *
 * Revision 1.28  1994/02/04  19:46:53  brad
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
 * Revision 1.27  1994/01/05  17:07:57  brad
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
 * Revision 1.26  1993/12/23  01:41:24  brad
 * Fix bug introduced in previous check-in: coding typo referencing 'offset'
 * esize_t pointer in e___eseek().  Also fix lint errors.
 *
 *  Reviewer: Dave Minturn.
 *  Risk: Low.
 *  Benefit or PTS #: 7469
 *  Testing: Reran all tests run for previous checkin; also ran new
 *     test case that caught the 7469 bug.
 *  Module(s): emulator/pfs_user_side.c
 *
 * Revision 1.25  1993/11/11  22:59:49  brad
 * Fixed bogus check in eseek(); now lseek() is only called on non-PFS files.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: 7079 - now eseek() works on all valid offsets on PFS files.
 *  Testing: Performed eseeks on many offsets, positive and negative, up to
 *           16 GB, and verified that the succeeded.  Focused on ranges known
 *           not to work previously, as described in bug report for PTS #7079.
 *  Module(s): emulator/{pfs_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.24  1993/10/06  21:50:09  dbm
 * Put in fix for bug #5944 which was incorrect check for iodone() when
 * reading pas eof.
 *
 * Revision 1.23  1993/09/21  19:18:13  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.22  1993/09/14  22:42:05  cfj
 * Merge R1.1 bug fix into main stem.
 *
 *
 * Revision 1.21  1993/09/08  20:59:24  dbm
 * Added PFS_TOKENMGT macro to dissable M_RECORD mode for UFS files.
 *
 * Revision 1.20.4.3  1993/09/21  19:14:50  cfj
 * Put #ifdef PFS around call to pfs_scanpath() in estat_call().
 *
 * Revision 1.20.4.2  1993/09/14  22:36:58  cfj
 * Fixed bug #6635, e__iseof() had incorrect check for M_LOG I/O mode.
 *
 * Revision 1.20.4.1  1993/09/07  22:11:43  cfj
 * Added use of PFS_TOKENMGT() to dissable PFS M_RECORD mode for UFS files.
 * (Bug #6499) dbm@ssd.intel.com
 *
 * Revision 1.20  1993/08/06  23:49:31  brad
 * Hacked #ifdef PFS's into this module so that estat and its variants are
 * always defined, even if PFS isn't.  This hack was requested to work around
 * the totally bogus way the RAM disk file system is built for the
 * installation process.
 *
 * Revision 1.19  1993/08/04  01:29:01  dbm
 * Fixed a problem in PFS I/O mode 3 which caused an incorrect file length
 * to be given to non zero nodes when opening a file for read.
 *
 * Revision 1.18  1993/07/16  03:04:23  dbm
 * Added token optimization functionality.
 *
 * Revision 1.17  1993/07/14  21:49:01  wunder
 * fixed pfs_user_side.c (e_lsize) to use _eadd1 to set extended offset,
 * which fixes bug 5723.
 *
 * Revision 1.16  1993/06/17  00:31:51  brad
 * Removed the e__pfs_unshare function, which was never needed.  Syscall
 * also removed (see syscalls.master).
 *
 * Revision 1.15  1993/06/16  22:33:19  dbm
 * Changed all references to pfs_iomode to pfs_iomode_info to allow single
 * node applications to obtain the PFS I/O mode info.
 *
 * Revision 1.14  1993/06/15  22:31:02  nandy
 * Initialize mem_obj for gopen to work with open_with_acquire
 *
 * Revision 1.13  1993/06/10  18:31:04  brad
 * Added support for _lestat().
 *
 * Revision 1.12  1993/06/07  18:45:03  dbm
 * Initialized some fdte fields, (flags, token) to allow the gopen() function
 * to work properly with new open_with_token functionality.
 *
 * Revision 1.11  1993/06/06  01:12:52  brad
 * Changed statpfs() and fstatpfs() so that statfs() is only called if
 * necessary.
 *
 * Revision 1.10  1993/06/04  18:34:06  dbm
 * Added support for passing a NULL to statpfs and fstatpfs to avoid having
 * to obtain the file system information.  This allows the mount command to
 * work properly without having to worry about permissions.
 *
 * Revision 1.9  1993/05/25  18:43:00  dbm
 * Added interrupt support for asyncronous calls.
 *
 * Revision 1.8  1993/05/12  00:12:00  brad
 * Modifed PFS debug stuff.
 *
 * Revision 1.7  1993/05/06  20:15:27  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/27  16:56:36  brad
 * Added support for PFS debug levels.
 *
 * Revision 1.5  1993/04/09  17:13:49  brad
 * Removed redundant #ifdef's.
 *
 * Revision 1.4  1993/04/06  17:57:23  wunder
 * Modified calls to dequeue_sync and added ifdef PFS around async changes.
 *
 * Revision 1.3  1993/04/03  03:18:49  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.13  1993/03/20  23:07:59  brad
 * Got rid of e__cread,e__creadv,e__cwrite,e__cwritev.  They are emulated in
 * libnx now.
 *
 * Revision 1.1.2.2.2.12  1993/03/16  18:20:26  wunder
 * Added ireadv creadv iwritev cwritev changes to use async_readv sync_readv
 * async_writev sync_writev interfaces.
 *
 * Revision 1.1.2.2.2.11  1993/03/16  01:48:39  wunder
 * Added user asynchronous I/O support.
 *
 * Revision 1.1.2.2.2.10  1993/03/11  00:34:36  dbm
 * Changed interface to statpfs and fstatpfs to use new estatfs structure.
 * Also turned debug flag on by default.
 *
 * Revision 1.1.2.2.2.9  1993/03/10  06:38:29  brad
 * Changed pfs_multi_eseek() calls to pfs_multi_lseek().  Added
 * support for lsize'ing PFS files.  Removed unused e__gopen().
 *
 * Revision 1.1.2.2.2.8  1993/02/12  22:48:16  dbm
 * Fixed eseek to return proper offset when using I/O modes.
 *
 * Revision 1.1.2.2.2.7  1993/02/12  17:19:07  dbm
 * Added system call to turn debug on or off.
 *
 * Revision 1.1.2.2.2.6  1993/02/09  22:44:07  brad
 * Implemented estat and festat.
 *
 * Revision 1.1.2.2.2.5  1993/02/04  00:51:36  brad
 * First cut at a "real" eseek() implementation.
 *
 * Revision 1.1.2.2.2.4  1993/01/14  20:38:55  dbm
 * Added pfs_get_fdte_info, pfs_get_fdte_size, and pfs_put_fdte_info system
 * calls to support the gopen() function.
 *
 * Revision 1.1.2.2.2.3  1992/12/14  22:57:10  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.1.2.2.2.2  1992/12/03  00:17:01  dbm
 * Updated for pfs i/o mode information in the fdte entry.
 *
 * Revision 1.2  1992/11/30  22:09:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2.2.1  1992/11/25  23:01:30  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.3  1992/11/25  02:47:23  dbm
 * Added changes to support mapped files with PFS I/O modes.
 *
 * Revision 1.1.2.2  1992/11/13  18:22:08  cfj
 * Fixup number of params to calls to isc_deregister.
 *
 * Revision 1.1.2.1  1992/11/10  16:30:03  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:16:12  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.1  1992/10/22  15:17:22  dbm
 * New for PFS functionality.
 *
 * Revision 1.4  92/08/06  17:59:37  brad
 * Added #ifdef PFS wrapper.
 *
 * Revision 1.3  92/08/05  16:24:46  brad
 * Removed debug prints.
 * 
 * Revision 1.2  92/08/04  16:43:35  dbm
 * Fixed up function header comments.
 * 
 * Revision 1.1  92/07/31  12:21:59  brad
 * Initial revision
 * 
 */

#include <mach_init.h>
#include <mach/mig_errors.h>
#include <uxkern/fsvr.h>

#include <mach/message.h>


/* WARNING: the lengthy path name below is necessary to
 * prevent varargs.h from being found in the mk/release
 * subdirectory.  For some reason, even though CPATH has
 * "../server/include" before "mk/release/.../include",
 * the file is found in the latter directory, if it is
 * not qualified with a path prefix.
 */
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
#ifdef	PFS
#include "pfs_iomode.h"
#endif

#define GOPEN_1   0xa5a50000


#ifdef	DEBUG_PFS
/*
 * Macros for PFS debugging that are private to this module.  The following
 * debug levels are valid:
 *
 *	0 => no debug
 *	1 => a function call trace is printed to stderr
 *	2 => a function call trace is printed to the console
 *	3 => detailed debug output is printed to stderr
 *	4 => detailed debug output is printed to the console
 */
#define PFS_TRACE(s) \
	if (pfs_debug_flag >= 1) e_printf s
#define PFS_DEBUG(s) \
	if (pfs_debug_flag >= 3) e_printf s
#define PFS_DEBUG_SATTR(s, sattr) \
	if (pfs_debug_flag >= 3) { \
		e_printf s; \
		dump_pfsattr(sattr); \
	}
#define PFS_DEBUG_RECV(s1, s2, err) \
	if (pfs_debug_flag >= 3) { \
		e_printf s1; \
		(err) ? e_printf("\n") : e_printf s2; \
	}

#else

#define PFS_TRACE(s)   
#define PFS_DEBUG(s)   
#define PFS_DEBUG_SATTR(s, sattr)
#define PFS_DEBUG_RECV(s1, s2, err)

#endif	DEBUG_PFS

/*
 * External Declarations.
 */
extern int		nullcompat;
#ifdef	PFS
extern void		ref_file();
extern void		unref_file();
extern int		async_done();
extern int		async_wait();
extern int		async_read();
extern int		async_write();
extern size_t		strlen();
extern char *		mach_error_string();

extern int		dont_care;


#ifdef	DEBUG_PFS
/* forward references */
void			display_port_rights();
#endif


/*
 * Global variables.
 */
esize_t		ex_zero 	 = { 0, 0 };		/* extended zero */
esize_t		ex_neg_one 	 = { -1, -1 };		/* extended -1 */
esize_t		ex_int_max	 = { INT_MAX, 0 };	/* extended 2GB-1 */
esize_t		ex_neg_int_max	 = { -INT_MAX, -1 };	/* extended -(2GB-1) */
int		pfs_debug_flag	 = 0;

mach_port_t	gopen_reply_port = MACH_PORT_NULL;	/* Port used during
							   gopen  */


/*
 * NAME:	e_printf
 *
 * DESCRIPTION:
 *	      This function is used to print debug information to the user's
 *		standard error or console depending on the value of
 *		pfs_debug_flag.
 *
 *		If pfs_debug_flag is odd, output is sent to standard error;
 *		if pfs_debug_flag is even, output is sent to the console.
 *
 *		See the debug macros in pfs_fdt.h (which call this function)
 *		for more information on valid values for pfs_debug_flag.
 *
 * PARAMETERS:
 *		Same as printf but limited to 10 variables.
 *	
 * RETURNS:
 *		Nothing.
 *
 */
void 
e_printf(string, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
	char	*string;
	int	p0;
	int  	p1;
	int	p2;
	int	p3;
	int	p4;
	int	p5;
	int	p6;
	int	p7;
	int	p8;
	int	p9;
{
	char	buf[256];
	int	temp;

	if (!pfs_debug_flag) { 
		return;
	}

	if (pfs_debug_flag % 2) {	/* flag is odd */
		/*
		 * Print to standard error of the program:
		 */
		sprintf(buf, string, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
       		e_write(our_bsd_server_port, 0, 2, buf, (uint_t)strlen(buf),
			&temp);
	} else {	/* flag is even */
		/*
		 * Print to the console:
		 */
		sprintf(buf, string, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		e_emulator_error("%s", buf);
	}
}


/*
 * NAME:	e_pfs_set_debug
 *
 *
 * DESCRIPTION:
 *	      This function is used to set the pfs_debug_flag which
 *		enables debug printing.
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt;      Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *
 *		debug_val;	Value to set debug variable to.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either zero 
 *				or -1 if an error occured.
 *
 * RETURNS:
 *		ESUCCESS;
 *
 */
int
e_pfs_set_debug(bsd_serv_port, interrupt, debug_val, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;     /* out */
	int		debug_val;	
	int		*rval;		/* out */
{
	pfs_debug_flag = debug_val;
	return ESUCCESS;
}


/*
 * NAME:	e___eseek
 *
 * DESCRIPTION:
 *		This function is used to perform a seek operation on an
 *		extended file, and is called via a system call using the
 *		___eseek value from syscalls.h.  
 *
 *		The eseek functionality is the same as OSF lseek except that
 *		it handles files greater than 2G bytes.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A file descriptor identifying the
 *				file to be written.		
 *
 *		offset:		Pointer to the esize_t structure that
 *				contains the value, in bytes, to be used
 *				in conjuction with the whence parameter
 *				to set the file pointer.
 *
 *		whence:		Specifies how offset affects the file
 *				pointer.  Possible values are as follows:
 *
 *				SEEK_SET:	Sets the file pointer
 *						to offset bytes from the
 *						beginning of the file.
 *				SEEK_CUR:	Sets the file pointer to
 *						its current location plus
 *						offset bytes.
 *				SEEK_END:	Sets the file pointer to
 *						offset bytes beyond the
 *						end of the file.
 *
 *		ret_offset:	Pointer to the esize_t structure that
 *				is used to return the new position of 
 *				the file pointer.  Both fields of this
 *				structure will be set to -1 on error.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either 0  if successful
 *				or -1 if an error occured.
 *
 * RETURNS:
 *		ESUCCESS if operation was successful else the value
 *		to set errno to.
 */
int 
e___eseek(proc_port, interrupt, fildes, offset, whence, ret_offset, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		fildes;
	esize_t		*offset;
	int		whence;
	esize_t		*ret_offset;	/* OUT */
	int		*rval;		/* OUT */
{
	fdt_entry_t	*fdte;
	int		error;
	int		queued = 0;
	async_req	dummy_req;

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	/*
	 * Currently only PFS files support extended offsets.  If this is not a
	 * PFS file, e_lseek() must be used and the offset must lie in the
	 * range -(2G-1) ... +(2G-1).
	 */
	if (fdte->pfs_fd == NULL) {	/* not a PFS file */

		/*
		 * Release fdte reference ... e_lseek will grab its own.
		 */
		(void) fdt_unref_entry(fdte);

		if ((GREATER(*offset, ex_int_max)) || 
		    (LESS(*offset, ex_neg_int_max))) {
			return(EINVAL);
		}

		error = e_lseek(proc_port, interrupt, fildes,
				(off_t)offset->slow, whence, rval);
		if ((error == ESUCCESS) && (*rval != -1)) {
			ret_offset->shigh = 0;
			ret_offset->slow = *rval;
		} else {
			ret_offset->shigh = -1;
			ret_offset->slow = -1;
		}

		return(error);
	}

	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue,
				       &dummy_req, &queued))
			goto out;
	}

	if ( (fdte->pfs_iomode_info) &&
	     ((fdte->pfs_iomode == M_RECORD) ||
	      (fdte->pfs_iomode == M_SYNC)   ||
	      (fdte->pfs_iomode == M_GLOBAL)) ) {

		error = pfs_iomode_lseek(fdte, offset, whence, ret_offset);

	} else {

		error = pfs_multi_lseek(interrupt, fdte, *offset,
					whence, TRUE, ret_offset);
	}

	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);
out:
	(void) fdt_unref_entry(fdte);
	return(error);
}



/*
 * NAME:	e___esize
 *
 * DESCRIPTION:
 *		Increase the size of an extended file, preallocating disk
 *		storage if necessary (if the size of the file is increasing).
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server.
 *
 *		interrupt:	Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred.
 *		
 *		fildes:		A file descriptor identifying the file to
 *				increase the size of.
 *
 *		offset:		Pointer to the esize_t structure that contains
 *				the value, in bytes, to be used in conjuction
 *				with the whence parameter to increase the size
 *				of the file.
 *
 *		whence:		Specifies how offset affects the file size.
 *				Values for whence parameter are:
 *	
 *				SIZE_SET: 	Set the file size to the 
 *						greater of the current size
 *						or offset.
 *
 *				SIZE_CUR:	Set the file size to the
 *						greater of the current size
 *						or the current location of
 *						the file pointer plus offset.
 *
 *				SIZE_END:	Set the file size to the 
 *						greater of the current size 
 *						or the current size plus
 *						offset.
 *
 *		ret_esize:	Pointer to the esize_t structure that is used
 *				to return the new size, in bytes, of the file.
 *				Both fields of this structure will be set to
 *				-1 on error.
 *
 *		rval:		Pointer that is used to return the value of
 *				the function call.  This value will be either 0
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int 
e___esize(bsd_serv_port, interrupt, fildes, offset, whence, ret_esize, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fildes;
	esize_t		*offset;
	int		whence;
	esize_t		*ret_esize;	/* OUT */
	int		*rval;		/* OUT */
{
	int		error;
	transaction_id_t trans_id;
	fdt_entry_t	*fdte;

	ret_esize->shigh = -1;
	ret_esize->slow = -1;

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	if (fdte->pfs_fd) {
		error = pfs_multi_lsize(interrupt, fdte,
					*offset, whence, TRUE, ret_esize);
	} else {

		/*
		 * Not a PFS file.  Invoke lsize() directly, limiting the
		 * offset to the 2GB-1 file size limit.
		 */
		*offset = EMIN(*offset, ex_int_max);

		isc_register(fdte->fp, &trans_id);
		error = fsvr__lsize(fdte->fp,
				    credentials_port,
				    trans_id,
				    offset->slow,
				    whence,
				    (size_t *)rval);
		isc_deregister(interrupt);

		if ((error == ESUCCESS) && (*rval != -1)) {
			ret_esize->shigh = 0;
			ret_esize->slow = *rval;
		}
	}

	(void) fdt_unref_entry(fdte);
	return(error);
}

#endif	PFS


/*
 * NAME:	e__estat
 *
 * DESCRIPTION:
 *		Get the "extended" status of a file.  Follow symbolic links,
 *		i.e. if the file is a symbolic link return information about
 *		the file the link references.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server.
 *
 *		interrupt:	Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred.
 *
 *		path:		Pointer to the pathname identifying the file.
 *
 *		estatbuf:	Pointer to the estat structure in which the
 *				extended status information is placed.
 *
 *		rval:		Pointer that is used to return the value of
 *				the function call.  This value will be either 0
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
e__estat(bsd_serv_port, interrupt, path, estatbuf, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	char		*path;
	struct estat	*estatbuf;
	int		*rval;		/* OUT */
{
	return(estat_call(bsd_serv_port, interrupt, path, estatbuf, TRUE));
}


/*
 * NAME:	e__lestat
 *
 * DESCRIPTION:
 *		Get the "extended" status of a file.  Don't follow symbolic
 *		links, i.e. if the file is a symbolic link return information
 *		about the link rather than the file the link references.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server.
 *
 *		interrupt:	Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred.
 *
 *		path:		Pointer to the pathname identifying the file.
 *
 *		estatbuf:	Pointer to the estat structure in which the
 *				extended status information is placed.
 *
 *		rval:		Pointer that is used to return the value of
 *				the function call.  This value will be either 0
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
e__lestat(bsd_serv_port, interrupt, path, estatbuf, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	char		*path;
	struct estat	*estatbuf;
	int		*rval;		/* OUT */
{
	return(estat_call(bsd_serv_port, interrupt, path, estatbuf, FALSE));
}


/*
 * NAME:	estat_call
 *
 * DESCRIPTION:
 *		Get the "extended" status of a file.  This is done by 
 *		performing a stat operation on the file, and copying the 
 *		results to the given estat buffer.  If the file is a PFS file,
 *		pfs_multi_stat() is then called in order to determine the
 *		full extended PFS file size.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server.
 *
 *		interrupt:	Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred.
 *
 *		path:		Pointer to the pathname identifying the file.
 *
 *		estatbuf:	Pointer to the estat structure in which the
 *				extended status information is placed.
 *
 *		follow:		TRUE if a symbolic link is to be followed,
 *				otherwise FALSE.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
estat_call(bsd_serv_port, interrupt, path, estatbuf, follow)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	char		*path;
	struct estat	*estatbuf;
	boolean_t	follow;
{
	int			error, len_path;
	mach_port_t		start_port;
	transaction_id_t 	trans_id;
	struct stat		statbuf;
	char			pfs_path[MAXPATHLEN];
	ulong_t			iomode;
	
	if (!user_strlen(path, &len_path))
		return EFAULT;

#ifdef PFS
	if (pfs_scanpath(path, pfs_path, &len_path))
		return EFAULT;
	path = pfs_path;
#endif /* PFS */

	if (!user_rwcheck(estatbuf, sizeof(struct estat)))
		return EFAULT;

	if (*path == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*path == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
#ifdef	PFS
	error = fsvr_stat(start_port, credentials_port, trans_id, rootdir_port,
			  path, len_path + 1, follow, &statbuf, &iomode);
#else
	error = fsvr_stat(start_port, credentials_port, trans_id, rootdir_port,
			  path, len_path + 1, follow, &statbuf);
#endif
	isc_deregister(interrupt);

	if (error)
		return(error);

	/*
	 * Copy out stat fields.
	 */
	estatbuf->st_dev	= statbuf.st_dev;
	estatbuf->st_ino	= statbuf.st_ino;
	estatbuf->st_mode	= statbuf.st_mode;
	estatbuf->st_nlink	= statbuf.st_nlink;
	estatbuf->st_uid	= statbuf.st_uid;
	estatbuf->st_gid	= statbuf.st_gid;
	estatbuf->st_rdev	= statbuf.st_rdev;
	estatbuf->st_size.shigh	= 0;
	estatbuf->st_size.slow	= statbuf.st_size;
	estatbuf->st_atime	= statbuf.st_atime;
	estatbuf->st_mtime	= statbuf.st_mtime;
	estatbuf->st_ctime	= statbuf.st_ctime;
	estatbuf->st_blksize	= statbuf.st_blksize;
	estatbuf->st_blocks	= statbuf.st_blocks;
	estatbuf->st_flags	= statbuf.st_flags;
	estatbuf->st_gen	= statbuf.st_gen;

#ifdef	PFS
	if (iomode == VIO_PFS) {
		error = pfs_multi_stat(interrupt, path, len_path, 
				       (caddr_t)estatbuf, follow, TRUE);
	}
#endif

	return(error);
}


/*
 * NAME:	e__festat
 *
 * DESCRIPTION:
 *		Get the "extended" status of the file associated with a given
 *		file descriptor.  This is done by performing an fstat
 *		operation on the file, and copying the results to the given
 *		estat buffer.  If the file is a PFS file, pfs_multi_fstat() is
 *		then called in order to determine the full extended PFS file
 *		size.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server.
 *
 *		interrupt:	Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred.
 *
 *		fildes:		A file descriptor representing an open file.
 *
 *		estatbuf:	Pointer to the estat structure in which the
 *				extended status information is placed.
 *
 *		rval:		Pointer that is used to return the value of
 *				the function call.  This value will be either 0
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__festat(bsd_serv_port, interrupt, fildes, estatbuf, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fildes;
	struct estat	*estatbuf;	/* OUT */
	int		*rval;		/* OUT */
{
	int		error;
	transaction_id_t trans_id;
	fdt_entry_t	*fdte;
	struct stat	statbuf;

	if (!user_rwcheck(estatbuf, sizeof(struct estat)))
		return EFAULT;

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fstat(fdte->fp, credentials_port, trans_id, &statbuf);
	isc_deregister(interrupt);

	if (error)
		goto out;

	/*
	 * Copy out stat fields.
	 */
	estatbuf->st_dev	= statbuf.st_dev;
	estatbuf->st_ino	= statbuf.st_ino;
	estatbuf->st_mode	= statbuf.st_mode;
	estatbuf->st_nlink	= statbuf.st_nlink;
	estatbuf->st_uid	= statbuf.st_uid;
	estatbuf->st_gid	= statbuf.st_gid;
	estatbuf->st_rdev	= statbuf.st_rdev;
	estatbuf->st_size.shigh	= 0;
	estatbuf->st_size.slow	= statbuf.st_size;
	estatbuf->st_atime	= statbuf.st_atime;
	estatbuf->st_mtime	= statbuf.st_mtime;
	estatbuf->st_ctime	= statbuf.st_ctime;
	estatbuf->st_blksize	= statbuf.st_blksize;
	estatbuf->st_blocks	= statbuf.st_blocks;
	estatbuf->st_flags	= statbuf.st_flags;
	estatbuf->st_gen	= statbuf.st_gen;

#ifdef	PFS
	if (fdte->pfs_fd)
		error = pfs_multi_fstat(interrupt, fdte, (caddr_t)estatbuf,
					TRUE);
#endif

out:
	(void) fdt_unref_entry(fdte);
	return(error);	
}

#ifdef	PFS


/*
 * NAME:	e__iodone
 *
 * DESCRIPTION:
 *		This function is used to determine whether an asynchrounous 
 *		read or write operation is complete.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		id:		A non-negative I/O ID returned by 
 *				iread() or iwrite().
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be:
 *					0:	I/O operation not completed.
 *					1:	I/O operation completed.
 *					-1:	Error occured.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__iodone(bsd_serv_port, interrupt, id, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		id;
	int		*rval;		/* OUT */
{
	int		error;

	if (error = async_done(bsd_serv_port, interrupt, id, rval))
		return(error);

	/* convert return value into CFS compatible results */
	if (rval[0] >= 0)
		rval[0] = 1;
	else if (rval[0] < 0)
		rval[0] = 0;

	return(ESUCCESS);
}


/*
 * NAME:	e__iomode
 *
 * DESCRIPTION:
 *		This function is used to get the iomode of a file.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A file descriptor representing an open
 *				file.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be the value of the 
 *				current iomode if successful or -1
 *				if an error occured.
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__iomode(bsd_serv_port, interrupt, fildes, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fildes;
	int		*rval;		/* OUT */
{
	int		error;
	fdt_entry_t	*fdte;

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	*rval = fdte->pfs_iomode;

	(void) fdt_unref_entry(fdte);
	return(ESUCCESS);
}


/*
 * NAME:	e__iowait
 *
 * DESCRIPTION:
 *		This function is used to wait for an asynchrounous 
 *		read or write operation is complete.
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		id:		A non-negative I/O ID returned by 
 *				iread() or iwrite().
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be zero if the operation
 *				was successful or -1 in an error
 *				occured.
 *
 * RETURNS:
 *	
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__iowait(bsd_serv_port, interrupt, id, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		id;
	int		*rval;		/* OUT */
{
	return(async_wait(bsd_serv_port, interrupt, id, rval));
}


/*
 * NAME:	e__lsize
 *
 * DESCRIPTION:
 *		Increase the size of a file, preallocating disk storage if
 *		necessary (if the size of the file is increasing).
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A file descriptor identifying the
 *				file to increase the size of.
 *
 *		offset:		The value, in bytes, to be used in 
 *				conjunction with the whence parameter
 *				to increase the size of the file.
 *
 *		whence:		Specifies how offset affects the file
 *				size.  Values for whence parameter are:
 *	
 *				SIZE_SET: 	Set the file size to the 
 *						greater of the current size
 *						or offset.
 *
 *				SIZE_CUR:	Set the file size to the
 *						greater of the current size
 *						or the current location of
 *						the file pointer plus offset.
 *
 *				SIZE_END:	Set the file size to the 
 *						greater of the current size 
 *						or the current size plus
 *						offset.
 *
 *		rval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either the new file size, 
 *				in bytes, if successful or a -1 if an 
 *				error occured.
 *
 * RETURNS:
 *
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__lsize(bsd_serv_port, interrupt, fildes, offset, whence, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;	/* out */
	int		fildes;
	off_t		offset;
	int		whence;
	int		*rval;		/* out */
{
	int		error;
	transaction_id_t trans_id;
	fdt_entry_t	*fdte;

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	if (fdte->pfs_fd) {
		esize_t	ex_offset;
		esize_t	ex_actual;

		ex_offset = __eadd1(ex_zero, (long)offset, &dont_care);
		error = pfs_multi_lsize(interrupt, fdte, ex_offset, whence,
					FALSE, &ex_actual);
		*rval = (error) ? -1 : ex_actual.slow;
	} else {
		isc_register(fdte->fp, &trans_id);
		error = fsvr__lsize(fdte->fp, credentials_port, trans_id,
				    offset, whence, (size_t *)rval);
		isc_deregister(interrupt);
	}

	(void) fdt_unref_entry(fdte);
	return(error);
}


/*
 * NAME:	e__iread
 *
 *
 * DESCRIPTION:
 *		This function is used to perform a asynchronous read
 *		operation.  This function is called via a system 
 *		call using the __iread value from syscalls.h.  
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt;      Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *
 *		fildes;		A file descriptor identifying the
 *				file to be read.		
 *
 *		buffer:		Pointer to the buffer in which
 *				the data is stored after it is
 *				read.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either an I/O ID number
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__iread(bsd_serv_port, interrupt, fildes, buffer, nbytes, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;     /* out */
	int		fildes;		/* File descriptor. */
	char		*buffer;	/* Pointer to input buffer. */
	uint_t		nbytes;		/* Number of bytes to read. */
	int		*rval;	  	/* out */
{
	return(async_read(bsd_serv_port, interrupt, fildes, buffer, nbytes,
			  rval));
}


/*
 * NAME:	e__ireadv
 *
 *
 * DESCRIPTION:
 *		This function is used to perform a asynchronous read
 *		scatter operation to an irregular buffer.  This function
 *		is called via a system call using the __ireadv value from
 *		syscalls.h.  
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt       Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *
 *		fildes		A file descriptor identifying the
 *				file to be read.		
 *
 *		iov:		Pointer to an array of iovec structures
 *				that identify the buffers into which data
 *				is to be placed.
 *
 *		iovcnt:		The number of iovec structures pointed
 *				to by the iov parameter.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either the I/O ID
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__ireadv(bsd_serv_port, interrupt, fildes, iov, iovcount, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fildes;
	struct iovec	*iov;
	unsigned	iovcount;
	int		*rval;		/* OUT */
{
	/*
	 * Call the async read management code which will indirectly
	 * call the synchronous read interface.
	 */
	return(async_readv(bsd_serv_port, interrupt, fildes, iov, iovcount,
			   rval));
}


/*
 * NAME:	e__iseof
 *
 *
 * DESCRIPTION:
 *		This function is used to determine if a file pointer is
 *		at end-of_file. This function is called via a system
 *		call using the __iseof value from syscalls.h.  
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt       Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *
 *		fildes		A file descriptor identifying the
 *				file.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be:
 *					0:	If the file pointer
 *						is not at end-of-file.
 *					1:	If the file pointer is
 *						at end-of-file.
 *					-1:	If an error occured.
 * RETURNS:
 *
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int 
e__iseof(bsd_serv_port, interrupt, fildes, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;     /* out */
	int		fildes;
	int		*rval;		/* out */
{
	esize_t  	curr_offset;
	esize_t  	curr_length;
	esize_t		max_offset;
	esize_t		max_length;
	int		error = ESUCCESS;
	fdt_entry_t	*fdte;
	int		queued = 0;
	async_req	dummy_req;

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	/*
	 * Test to see if asynchronous fdte wait queue must be used.
	 */
	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue, &dummy_req, 
				       &queued)) {
			goto out;
		}
	}

	/*
	 * Get my current offset and length into the file:
	 */
	error = file_token_acquire(fdte, interrupt, PFS_OP_OFFSET, 0,
				   &curr_offset, &curr_length);
	if (error != ESUCCESS) {  
		goto out;
	}

	if ((fdte->pfs_iomode_info) && (fdte->pfs_iomode != M_LOG))  { 

		if (PFS_TOKENMGT(fdte)) {
			/*
			 * In order to determine the length of the file in 
			 * the M_RECORD and M_ASYNC I/O modes, the maximum
			 * file length value must be obtained from all of the
			 * nodes.  This is because the length is maintained 
			 * locally until either the to tokens are revoked or
			 * the file is closed.
			 */ 
			pfs_iomode_max(fdte,
				       &curr_offset,
				       &curr_length,
				       &max_offset, 
				       &max_length);
			/*
			 * Now that we have the maximum length, perform
			 * a global check for iseof using the max_length
			 * value given.
			 */
			pfs_iomode_iseof(fdte, 
					&curr_offset,
					&max_length, 
					rval);
		} else {

			pfs_iomode_iseof(fdte, 
					 &curr_offset,
					 &curr_length, 
					 rval);
		} 

	} else {
		/*
		 * Compare the two offsets:
		 */
		if (LESS(curr_offset, curr_length)) {
			/*
			 * Not at EOF.
			 */ 
			*rval = 0;
		} else {
			/*
			 * At EOF.
			 */
			*rval = 1;
		}
	}

out:
	/*
	 * If we had been queued previously we must exit the queue.
	 */
	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);

	(void) fdt_unref_entry(fdte);
	return(error);
}



/*
 * NAME:	e__iwrite
 *
 * DESCRIPTION:
 *		This function is used to perform an asynchronous write
 *		operation.  This function is called via a system 
 *		call using the __iwrite value from syscalls.h.  
 *
 * PARAMETERS:
 *		bsd_serv_port:	Mach port to the bsd server,
 *				(our_bsd_server_port).
 *
 *		interrupt:	Pointer to interrupt variable,  
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *		
 *		fildes:		A file descriptor identifying the
 *				file to be written.		
 *
 *		buffer:		Pointer to the buffer which contains
 *				the data to be written.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either an I/O ID
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__iwrite(bsd_serv_port, interrupt, fildes, buffer, nbytes, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;     /* out */
	int		fildes;		/* File descriptor. */
	char		*buffer;	/* Pointer to input buffer. */
	uint_t		nbytes;		/* Number of bytes to read. */
	int		*rval;		/* out */
{
	/*
	 * Call the async write management code which will indirectly
	 * call the synchronous write interface.
	 */
	return(async_write(bsd_serv_port, interrupt, fildes, buffer, nbytes,
			   rval));
}


/*
 * NAME:	e__iwritev
 *
 *
 * DESCRIPTION:
 *		This function is used to perform an asynchronous write
 *		gather operation to an irregular buffer.  This function
 *		is called via a system call using the __iwritev value from
 *		syscalls.h.  
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt       Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *
 *		fildes		A file descriptor identifying the
 *				file to be written.		
 *
 *		iov:		Pointer to an array of iovec structures
 *				that identify the buffers containing the
 *				data to be written.
 *
 *		iovcnt:		The number of iovec structures pointed
 *				to by the iov parameter.
 *
 *		retval:		Pointer that is used to return the
 *				value of the function call.  This
 *				value will be either the I/O ID
 *				if successful or -1 if an error occured.
 *
 * RETURNS:
 *
 *	ESUCCESS if successful, otherwise the value to set errno to.
 *
 */
int
e__iwritev(bsd_serv_port, interrupt, fildes, iov, iovcount, rval)
	mach_port_t	bsd_serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fildes;
	struct iovec	*iov;
	unsigned	iovcount;
	int		*rval;		/* OUT */
{
	/*
	 * Call the async write management code which will indirectly
	 * call the synchronous write interface.
	 */
	return(async_writev(bsd_serv_port, interrupt, fildes, iov, iovcount,
			    rval));
}


/*
 * NAME:	pfs_scanpath
 *
 *
 * DESCRIPTION:
 *	This function scans a pathname looking for the occurrence
 *	of the "###..." characters.  If found, the ###...'s are replaced
 *	with the node number of the processor that the calling task
 *	is running on.
 *
 * PARAMETERS:
 *
 *		old_name	Pointer to the old file name.
 *		new_name	Pointer to the new file name.
 *
 * RETURNS:
 *		-1 == error
 *		 0 == success
 */

int
pfs_scanpath(old_name, new_name, path_len)
char    *old_name;
char    *new_name;
int	*path_len;
{
	int		node;
	int		count = 0;
	int		new_pathlen = 0;
	int		tenp;
	int		num_digits;
	extern node_t	emul_tnc_mynode();

	/*
	 * valid strings?
	 */
	if ( old_name == (char *)0 || new_name == (char *)0 ) {
		if ( path_len )
                	*path_len = 0;
                return -1;
	}

	node = emul_tnc_mynode();
	while (((*new_name = *old_name) != '\0') && 
	       (++new_pathlen < MAXPATHLEN)){
		if (*new_name ==  '#') {
			if (++count == 3) {
				char scanf_str[20];
				/*
				 * Change ###'s into node numbers:
				 */
				while((*old_name == '#') &&
				      (new_pathlen <= MAXPATHLEN)) {
					count++;
					old_name++;
					new_pathlen++;
				}
				if (new_pathlen > MAXPATHLEN) {
					*path_len = 0;
					return -1;
				}
				count--;
				/*
				 * Figure out how many digits.
				 */
				tenp = 1;
				num_digits = 1;
				while (node >= tenp*10) {
					tenp *= 10;
					num_digits++;
				}

				if (count >= num_digits) {
					/*
					 * Zero fill.
					 */
					sprintf(scanf_str, "%%0%dd",count);
					sprintf(new_name - 2, scanf_str, node);
					new_name+= (count -2);
					new_pathlen--;
				} else {
					/*
					 * Need to expand the pathname. 
					 */
					if ((new_pathlen + num_digits + 1) <
					    (MAXPATHLEN - count)) {
						sprintf(scanf_str,
							"%%0%dd",num_digits);
						sprintf(new_name -2,
							scanf_str, node);
						new_name += (num_digits -2);
						new_pathlen += (num_digits -
								count - 1);
					} else {
						*path_len = 0;
						return -1; 
					}
				}
			} else {
				new_name++; old_name++;
			}
		} else {
			count = 0;
			new_name++; old_name++; 
		}
	}
	if (new_pathlen < MAXPATHLEN) {
		*path_len = new_pathlen;
		return 0;
	} else {
		*path_len = 0;
		return -1;
	}
}


/*
 * NAME:	e_statpfs
 *
 * DESCRIPTION:
 *		Emulator interface to the statpfs() system call, which returns
 *		the stripe attributes of the PFS file system in which the
 *		given file resides.
 *
 * PARAMETERS:
 *
 *		serv_port	Mach port to the OSF/1 server.
 *
 *		interrupt       Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred
 *				to the caller of this routine (emul_syscall()).
 *
 *		path		Pathname of target file.
 *
 *		fsbuf		Pointer to user's estatfs buffer.
 *
 *		pfsbuf		Pointer to user's statpfs buffer.
 *
 *		len_pfsbuf	Length of the supplied buffer, required since
 *				the statpfs struct is variable length.
 *
 *		rval		Pointer that is used to return the value of
 *				the function call. 
 *
 * RETURNS:
 *
 *		ESUCCESS if successful, otherwise the value to set errno to.
 */
int
e_statpfs(serv_port, interrupt, path, fsbuf, pfsbuf, len_pfsbuf, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*path;
	struct estatfs	*fsbuf;
	struct statpfs	*pfsbuf;
	uint_t		len_pfsbuf;
	int		*rval;
{
	register int		error;
	mach_port_t		start_port;
	transaction_id_t	trans_id;
	int			len_path;
	char			pfs_path[MAXPATHLEN];
	boolean_t		pfs_filesys = TRUE;
	char			*migbuf;
	int			len_migbuf = STATPFS_BUFSZ;
	uint_t			mig_actual = 0;

	PFS_TRACE(("e_statpfs: path=%s fsbuf=0x%x pfsbuf=0x%x len_pfsbuf=%d\n",
		   path, fsbuf, pfsbuf, len_pfsbuf));

	if (!user_strlen(path, &len_path))
		return(EFAULT);
	if (pfs_scanpath(path, pfs_path, &len_path))
		return(EFAULT);
	path = pfs_path;

	if ((fsbuf == NULL) && (pfsbuf == NULL))
		return(ESUCCESS);
	if (fsbuf != NULL) {
		if (!user_rwcheck(fsbuf, sizeof(struct estatfs)))
			return(EFAULT);
	}
	if (pfsbuf != NULL) {
		if (len_pfsbuf > 0) {
			if (!user_rwcheck(pfsbuf, len_pfsbuf))
				return(EFAULT);
		} else {   /* indicate that we should not try to fill buf */
			pfsbuf = NULL;
		}
	}

	if (*path == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*path == '/') ? rootdir_port : currentdir_port;
		
	/*
	 * Get the PFS stripe attributes.  If this is a PFS file system, this
	 * is required in order to fill in either fsbuf or pfsbuf properly.  If
	 * this is *not* a PFS file system, we'll know because we'll get an 
	 * error here.
	 *
	 * A loop is used because the statpfs structure is variable size and
	 * may require a bigger request size to hold the information.  Also the
	 * size may change due to another mount operation taking place.
	 */
	for (;;) {
		isc_register(start_port, &trans_id);
		error = fsvr_statpfs(start_port, credentials_port, trans_id,
				     rootdir_port, path, len_path+1,
				     len_migbuf, &migbuf, &mig_actual);
		isc_deregister(interrupt);
		if (error) {
			if (error == EFSNOTSUPP) {
				pfs_filesys = FALSE;
				error = ESUCCESS;
				break;
			} else {
				return(error);
			}
		}

		/*
		 * Exit the loop if the request is satisfied.
		 */
		if (len_migbuf >= ((struct statpfs *)migbuf)->p_reclen) {
			break;
		}

		/*
		 * Else need to try a bigger request size.
		 */
		len_migbuf = ((struct statpfs *)migbuf)->p_reclen;
		(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
				     mig_actual);
	}

	/*
	 * Get the file system attributes (statfs) if so requested.
	 */
	if (fsbuf != NULL) {
		struct statfs	tmpbuf;

		isc_register(start_port, &trans_id);
		error = fsvr_statfs(start_port, credentials_port, trans_id,
				    rootdir_port, path, len_path+1, &tmpbuf);
		isc_deregister(interrupt);
		if (error)
			goto out;

		/*
		 * Copy the statfs structure into the estatfs structure:
		 */
		fsbuf->f_type		= tmpbuf.f_type;  
       		fsbuf->f_flags		= tmpbuf.f_flags;
		fsbuf->f_fsize		= tmpbuf.f_fsize;
		fsbuf->f_bsize		= tmpbuf.f_bsize;
		fsbuf->f_blocks.shigh	= 0;
		fsbuf->f_blocks.slow	= tmpbuf.f_blocks;
		fsbuf->f_bfree.shigh	= 0;
		fsbuf->f_bfree.slow	= tmpbuf.f_bfree;
		fsbuf->f_bavail.shigh	= 0;
       		fsbuf->f_bavail.slow	= tmpbuf.f_bavail;
		fsbuf->f_files		= tmpbuf.f_files;
		fsbuf->f_ffree		= tmpbuf.f_ffree;
		fsbuf->f_fsid		= tmpbuf.f_fsid;
		bcopy(tmpbuf.f_spare, fsbuf->f_spare, 9 * sizeof(long));
		bcopy(tmpbuf.f_mntonname, fsbuf->f_mntonname, MNAMELEN);
		bcopy(tmpbuf.f_mntfromname, fsbuf->f_mntfromname, MNAMELEN);

		if (pfs_filesys)
			/*
			 * Update the statfs structure to reflect the correct
			 * PFS file system information.
			 */
			error = pfs_multi_statfs(interrupt,
						 (struct statpfs *)migbuf, 
						 fsbuf);	
	}

	/*
	 * Copy the PFS stripe attributes (statpfs) into the user's buffer if
	 * so requested.
	 */
	if (pfsbuf != NULL) {
		if (pfs_filesys) {
			if (!user_bcopy(migbuf, pfsbuf, 
					MIN(len_pfsbuf, mig_actual)))
				error = EFAULT;
		} else {  /* indicate to user that this is not a PFS */
			pfsbuf->p_reclen = 0;
		}
	}

out:
	if (pfs_filesys)
		(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
				     mig_actual);
	return(error);
}



/*
 * NAME:	e_fstatpfs
 *
 * DESCRIPTION:
 *		Emulator interface to the fstatpfs() system call, which returns
 *		the stripe attributes of the PFS file system in which the
 *		given file resides.
 *
 * PARAMETERS:
 *
 *		serv_port	Mach port to the OSF/1 server.
 *
 *		interrupt       Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred
 *				to the caller of this routine (emul_syscall()).
 *
 *		fildes		File descriptor of target file.
 *
 *		fsbuf		Pointer to user's estatfs buffer. 
 *
 *		pfsbuf		Pointer to user's statpfs buffer.
 *
 *		len_pfsbuf	Length of the supplied buffer, required since
 *				the statpfs struct is variable length.
 *
 *		rval		Pointer that is used to return the value of
 *				the function call. 
 *
 * RETURNS:
 *
 *		ESUCCESS if successful, otherwise the value to set errno to.
 */
int
e_fstatpfs(serv_port, interrupt, fildes, fsbuf, pfsbuf, len_pfsbuf, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fildes;
	struct estatfs	*fsbuf;
	struct statpfs	*pfsbuf;
	uint_t		len_pfsbuf;
	int		*rval;
{
	register int		error;
	transaction_id_t	trans_id;
	fdt_entry_t		*fdte;
	boolean_t		pfs_filesys = TRUE;
	char			*migbuf;
	int			len_migbuf = STATPFS_BUFSZ;
	uint_t			mig_actual = 0;

	if ((fsbuf == NULL) && (pfsbuf == NULL))
		return(ESUCCESS);
	if (fsbuf != NULL) {
		if (!user_rwcheck(fsbuf, sizeof(struct estatfs)))
			return(EFAULT);
	}
	if (pfsbuf != NULL) {
		if (len_pfsbuf > 0) {
			if (!user_rwcheck(pfsbuf, len_pfsbuf))
				return(EFAULT);
		} else {   /* indicate that we should not try to fill buf */
			pfsbuf = NULL;
		}
	}

	if (error = fdt_ref_entry(fildes, &fdte))
		return(error);

	/*
	 * Get the PFS stripe attributes.  If this is a PFS file system, this
	 * is required in order to fill in either fsbuf or pfsbuf properly.  If
	 * this is *not* a PFS file system, we'll know because we'll get an 
	 * error here.
	 *
	 * A loop is used because the statpfs structure is variable size and
	 * may require a bigger request size to hold the information.  Also the
	 * size may change due to another mount operation taking place.
	 */
	for (;;) {
		isc_register(fdte->fp, &trans_id);
		error = fsvr_fstatpfs(fdte->fp, credentials_port, trans_id,
				      len_migbuf, &migbuf, &mig_actual);
		isc_deregister(interrupt);
		if (error) {
			if (error == EFSNOTSUPP) {
				pfs_filesys = FALSE;
				error = ESUCCESS;
				break;
			} else {
				return(error);
			}
		}

		/*
		 * Exit the loop if the request is satisfied.
		 */
		if (len_migbuf >= ((struct statpfs *)migbuf)->p_reclen) {
			break;
		}

		/*
		 * Else need to try a bigger request size.
		 */
		len_migbuf = ((struct statpfs *)migbuf)->p_reclen;
		(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
				     mig_actual);
	}

	/*
	 * Get the file system attributes (fstatfs) if so requested.
	 */
	if (fsbuf != NULL) {
		struct statfs	tmpbuf;
		long		args[7];
		int		grval[2];

		args[0] = (long)fildes;
		args[1] = (long)&tmpbuf;
		grval[1] = 0;
		error = emul_fs_generic(serv_port, interrupt, SYS_fstatfs, 
					&args, &grval);
		if (error)
			goto out;

		/*
		 * Copy the statfs structure into the estatfs structure.
		 */
		fsbuf->f_type		= tmpbuf.f_type;
		fsbuf->f_flags		= tmpbuf.f_flags;
		fsbuf->f_fsize		= tmpbuf.f_fsize;
		fsbuf->f_bsize		= tmpbuf.f_bsize;
		fsbuf->f_blocks.shigh	= 0;
		fsbuf->f_blocks.slow	= tmpbuf.f_blocks;
       	 	fsbuf->f_bfree.shigh	= 0;
		fsbuf->f_bfree.slow	= tmpbuf.f_bfree;
		fsbuf->f_bavail.shigh	= 0;
		fsbuf->f_bavail.slow	= tmpbuf.f_bavail;
		fsbuf->f_files		= tmpbuf.f_files;
		fsbuf->f_ffree		= tmpbuf.f_ffree;
		fsbuf->f_fsid		= tmpbuf.f_fsid;
		bcopy(tmpbuf.f_spare, fsbuf->f_spare, 9 * sizeof(long));
		bcopy(tmpbuf.f_mntonname, fsbuf->f_mntonname, MNAMELEN);
       		bcopy(tmpbuf.f_mntfromname, fsbuf->f_mntfromname, MNAMELEN);

		if (pfs_filesys)
			/*
			 * Update the statfs structure to reflect the correct
			 * PFS file system information.
			 */
			error = pfs_multi_statfs(interrupt,
						 (struct statpfs *)migbuf, 
						 fsbuf);	
	}

	/*
	 * Copy the PFS stripe attributes (statpfs) into the user's buffer if
	 * so requested.
	 */
	if (pfsbuf != NULL) {
		if (pfs_filesys) {
			if (!user_bcopy(migbuf, pfsbuf, 
					MIN(len_pfsbuf, mig_actual)))
				error = EFAULT;
		} else {  /* indicate to user that this is not a PFS */
			pfsbuf->p_reclen = 0;
		}
	}

out:
	if (pfs_filesys)
		(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
				     mig_actual);
	(void) fdt_unref_entry(fdte);
	return(error);
}



/*
 * NAME:	fstatpfs_internal
 *
 * DESCRIPTION:
 *		Internal interface to fstatpfs().  This routine assumes we
 *		have exclusive access to the given file port.
 *
 * PARAMETERS:
 *
 *		interrupt       Pointer to interrupt variable, initially set
 *				to FALSE.  Signals that an interrupt occurred
 *				to the caller of this routine (emul_syscall()).
 *
 *		fp		Mach file port representing the open target
 *				file.
 *
 *		buf		Pointer to user's statpfs buffer.
 *
 *		buf_len		Length of the supplied buffer, required since
 *				the statpfs struct is variable length.
 *
 *		rval		Pointer that is used to return the value of
 *				the function call. 
 *
 * RETURNS:
 *
 *		ESUCCESS if successful, otherwise the value to set errno to.
 */
int
fstatpfs_internal(interrupt, fp, buf, buf_len, rval)
	boolean_t	*interrupt;
	mach_port_t	fp;
	struct statpfs	*buf;
	uint_t		buf_len;
	int		*rval;
{
	char		*migbuf;
	register int	error;
	transaction_id_t trans_id;
	uint_t		buf_actual = 0;

	isc_register(fp, &trans_id);
	error = fsvr_fstatpfs(fp, credentials_port, trans_id,
			      buf_len, &migbuf, &buf_actual);
	isc_deregister(interrupt);

	if (error == 0) {
		if (!user_bcopy(migbuf, buf, buf_actual))
			error = EFAULT;
		(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
				     buf_actual);
	}

	return(error);
}


/*
 * NAME:	e__pfs_get_fdte_size
 *
 *
 * DESCRIPTION:
 *		This function is used to obtain the size of the fdte data
 *		structure for the gopen() function.  Note that only logical
 *		node 0 in the application partition calls this routine.
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrup	(out) Pointer to interrupt variable, initially
 *				set to FALSE.  Signals interrupt during syscall.
 *
 *		fildes		file descriptor identifying the file table
 *				entry containing the fdte information.
 *
 *		size		(out) Size, in bytes, of the fdte information.
 *
 *		rval		(out) Pointer that is used to return the
 *				value of the function call.  This value will
 *				be either zero or -1 if an error occured.
 *
 * RETURNS:
 *		ESUCCESS
 *		return from fdt_ref_entry()
 */
int
e_pfs_get_fdte_size(bsd_serv_port, interrupt,
		    fildes, size, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;
	int		fildes;		/* in   file descriptor */
	int		*size;		/* out  data structure size */
	int		*rval;
{
	int		error = ESUCCESS;
	fdt_entry_t	*fdte;


	/*
	 * Make sure fildes is valid:
	 */
	if (error = fdt_ref_entry(fildes, &fdte)){
	    *size = 0;
	    return error;
	}
	
	*size = sizeof(fdt_entry_t);

	if (fdte->pfs_fd) {
	    /*
	     * The pfs file fdte entries are variable depending
	     * on the number of stripe directories.
	     */
	    *size += sizeof(pfs_fd_t);
	    *size += (fdte->pfs_fd->p_stripe_factor - 1) * 
		      sizeof(stripe_fd_t);
	}

	(void) fdt_unref_entry(fdte);

	return error;
}


/*
 * NAME:	e__pfs_get_fdte_info
 *
 *
 * DESCRIPTION:
 *		This function is used to obtain the fdte information for
 *		use by the gopen() function.  Note that only logical node 0
 *		in the application partition calls this routine.
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt	(out) Pointer to interrupt variable, initially
 *				set to FALSE.  Signals interrupt during syscall.
 *
 *		fildes		A file descriptor identifying the file table
 *				entry containing the fdte information.
 *
 *		buffer		(out) Buffer to return the fdte
 *				information in.
 *
 *		reply_port	(out) Port used by the other N-1 compute
 *				nodes in the application partition during
 *				the processing of the pfs_dup_fdte_info()
 *				routine
 *
 *		rval		(out) Pointer that is used to return the
 *				value of the function call.  This value will
 *				be either zero or -1 if an error occured.
 *
 * RETURNS:
 *		ESUCCESS
 *		EIO		
 *		return from file_token_acquire()
 */
int
e_pfs_get_fdte_info(bsd_serv_port, interrupt,
		    fildes, buffer, num_nodes, reply_port, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;
	int		fildes;		/* in   file descriptor */
	int		num_nodes;	/* in   number of nodes in partition */
	char		*buffer;	/* out  buffer to put fdte info in */
	mach_port_t     *reply_port;	/* out  */
	int		*rval;
{
	fdt_entry_t		*fdte;
	esize_t			length;
	kern_return_t           mach_error;
	esize_t			offset;
	int			sfd;
	int			error = ESUCCESS;
	pfs_gopen_fdte_info_t	*fdte_info = (pfs_gopen_fdte_info_t *)buffer;


	/*
	 * Make sure fildes is valid:
	 */
	if (error = fdt_ref_entry(fildes, &fdte)){
	    return error;
	}

	/*
	 * Get the token so that the length is correct.
	 */
	error = file_token_acquire(fdte, 
				   interrupt,
				   PFS_OP_OFFSET,
				   0,
				   &offset,
				   &length); 
	if (error) { 
	    return error;
	}

	/*
	 * Fill in the fdt_entry information.
	 */
	bcopy(fdte, &fdte_info->fdte_info, sizeof(fdt_entry_t));

	if (fdte->pfs_fd) {
	    /*
	     * The pfs file fdte entries are variable depending
	     * on the number of stripe directories.
	     */
	    bcopy(fdte->pfs_fd, &fdte_info->pfs_info, sizeof(pfs_fd_t));

	    for(sfd=0; sfd < fdte->pfs_fd->p_stripe_factor; sfd++) {
		bcopy(&fdte->pfs_fd->p_stripe_fdt[sfd],
		      &fdte_info->pfs_info.p_stripe_fdt[sfd],
		      sizeof(stripe_fd_t));
	    }
	}
	(void) fdt_unref_entry(fdte);


	/*
	 * Allocate the gopen reply port
	 */
	mach_error = mach_port_allocate(mach_task_self(),
					MACH_PORT_RIGHT_RECEIVE,
					&gopen_reply_port);
	if (mach_error != KERN_SUCCESS) {
	    EPRINT(("e_pfs_get_fdte_info: mach_port_allocate %s\n",
		    mach_error_string(mach_error)));
	    return(EIO);
	}

	mach_error = mach_port_insert_right(mach_task_self(),
					    gopen_reply_port,
					    gopen_reply_port,
					    MACH_MSG_TYPE_MAKE_SEND);
	if (mach_error != KERN_SUCCESS) {
	    EPRINT(("e_pfs_get_fdte_info: mach_port_insert_right %s\n",
		    mach_error_string(mach_error)));
	    return(EIO);
	}

	mach_error = mach_port_mod_refs(mach_task_self(),
					gopen_reply_port,
					MACH_PORT_RIGHT_SEND,
					num_nodes);
	if (mach_error != KERN_SUCCESS) {
	    EPRINT(("e_pfs_get_fdte_info: mach_port_mod_refs %s\n",
		    mach_error_string(mach_error)));
	    return(EIO);
	}

	*reply_port = gopen_reply_port;

	return error;
}


/*
 * NAME:	e__pfs_put_fdte_info
 *
 *
 * DESCRIPTION:
 *		This function is used to transfer the fdte information
 *		from the libnx gopen() function to the emulator library.
 *		Note that this routine is only called by logical nodes
 *		1 .. N-1 of the application partition.
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt	(out) Pointer to interrupt variable, initially
 *				to FALSE.  Signals interrupt during syscall.
 *
 *		data		A pointer to the fdte information.
 *
 *		fdte_info_size	The number of bytes of fdte information.
 *
 *		pfs_iomode	The PFS I/O mode that the new file descriptor
 *				will have.
 *
 *		fildes		A file descriptor returned which identifies
 *				the open file.
 *
 *		rval		Pointer that is used to return the value of
 *				the function call.  This value will be either
 *				zero or -1 if an error occured.
 *
 * RETURNS:
 *		ESUCCESS
 *		ENOMEM
 *		return from fdt_reserve()
 */
int
e_pfs_put_fdte_info(bsd_serv_port, interrupt,
		    data, fdte_info_size, pfs_iomode, fd, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;
	char		*data;		/* Buffer containing fdte info. */
	int		fdte_info_size; /* Size of fdte info. */
	int		pfs_iomode;	/* PFS I/O mode being set. */
	int		*fd;		/* out File descriptor. */
	int		*rval;
{
	fdt_entry_t		*fdte;
	pfs_fd_t		*pfs_fd;
	int			sfd;
	int			error = ESUCCESS;
	pfs_gopen_fdte_info_t	*fdte_info = (pfs_gopen_fdte_info_t *)data;


	/*
	 * Allocate an fdte for the file.
	 */
	if (error = fdt_reserve(0, fd, &fdte))
	    return(error);

	/*
	 * Fill in the general fdte information into the new fdte entry:
	 */
	bcopy(&fdte_info->fdte_info, fdte, sizeof(fdt_entry_t));

	if (fdte_info->fdte_info.pfs_fd) {
	    /*
	     * Allocate some memory for the pfs information:
	     */
	    fdte->pfs_fd = (pfs_fd_t *) 
		    malloc(fdte_info_size - sizeof(fdt_entry_t));
	    if (fdte->pfs_fd == NULL) {
		fdt[*fd].fdte = FD_EMPTY;
		fdte_free(fdte);
		return ENOMEM;
	    }

	    /*
	     * The pfs file fdte entries are variable depending
	     * on the number of stripe directories.
	     */
	    bcopy(&fdte_info->pfs_info, fdte->pfs_fd, sizeof(pfs_fd_t));

	    for(sfd=0; sfd < fdte_info->pfs_info.p_stripe_factor; sfd++) {
		bcopy(&fdte_info->pfs_info.p_stripe_fdt[sfd],
		      &fdte->pfs_fd->p_stripe_fdt[sfd],
		      sizeof(stripe_fd_t));
	    }
	}

	fdte->refcnt = 1;
	fdte->flags = 0;
	fdte->token_refcnt = 0;
	fdte->mem_obj = MACH_PORT_NULL;
	fdte->token = MACH_PORT_NULL;
	fdte->win_size = 0;

	FDT_LOCK();
	fdt[*fd].fdte = fdte;
	if (*fd > fdt_lastfile) {
	    fdt_lastfile = *fd;
	}
	FDT_UNLOCK();

	/*
	 * Each compute node in the application maintains its own file
	 * pointer for the M_UNIX, M_RECORD, and M_ASYNC I/O modes.  This
	 * is implemented by duplication the file port on the server
	 * containing the file (and the servers containing each of the
	 * stripe files).  This duplication occurs at a later step during
	 * gopen().
	 *
	 * The other I/O modes share the file pointer and the file port(s)
	 * are not duplicated.
	 */
	if ((pfs_iomode == M_LOG)  ||
	    (pfs_iomode == M_SYNC) ||
	    (pfs_iomode == M_GLOBAL) ) {

	    /*
	     * Set the file port(s) to MACH_PORT_NULL so that the 
	     * PFS I/O mode code detects it as a gopen() table
	     * entry.
	     */
	    fdte->fp = MACH_PORT_NULL;

	    if (fdte->pfs_fd) {
		pfs_fd = fdte->pfs_fd;
		for(sfd=0;
		    sfd < fdte->pfs_fd->p_stripe_factor; sfd++) {

		    fdte->pfs_fd->p_stripe_fdt[sfd].s_fp = 
			    MACH_PORT_NULL;

		}
	    }
	}

	return error;
}



/*
 * NAME:	e__pfs_dup_fdte_info
 *
 *
 * DESCRIPTION:
 *		This function is used to duplicate the server's file table
 *		entries for logical nodes 1 .. N-1 of the applicatio partition.
 *		
 *
 * PARAMETERS:
 *
 *		bsd_serv_port	Mach port to the bsd server, 
 *				(our_bsd_server_port).	
 *
 *		interrupt	(out) Pointer to interrupt variable, initially
 *				set to FALSE.  Signals interrupt during syscall.
 *
 *		fildes		A file descriptor identifying the file table
 *				entry containing the fdte information.
 *
 *		node		Number of nodes in the application's compute
 *				partition.
 *
 *		num_nodes		Number of nodes in the application compute
 *				partition.
 *
 *		unix_pid0	Unix pid of process 0 on logical node 0.
 *
 *		reply_port0	gopen reply port for logical node 0 in the
 *				application partition.
 *
 *		rval		(out) Pointer that is used to return the value
 *				of the function call.  This value will be
 *				either zero or -1 if an error occured.
 *
 * RETURNS:
 *		ESUCCESS
 *		EIO
 *		ENOMEM
 *		return from fdt_ref_entry()
 *		return from fsvr_pfs_duplicate()
 */
int
e_pfs_dup_fdte_info(bsd_serv_port, interrupt,
		    fildes, my_node, num_nodes, unix_pid0, reply_port0, rval)
	mach_port_t	bsd_serv_port;
	int		*interrupt;
	int		fildes;		 /* File descriptor */
	int		my_node;	 /* logical node number */
	int		num_nodes;	 /* number of compute nodes */
	int		unix_pid0;	 /* Unix pid of process 0. */
	mach_port_t	reply_port0;	 /* node 0's gopen reply port */
	int		*rval;
{
	mach_port_t		acquired_right;
	mach_msg_type_name_t	acquired_type ;
	mach_port_t		*buf;
	int                     buf_size;
	fdt_entry_t		*fdte;
	int                     i1;
	int                     loop_count;
	kern_return_t           mach_error;
	mach_msg_return_t       message_rtn;
	int			msg_hdr_bits;
	int                     pfs_error;  /*  error returned to caller */
	mach_port_t             *port_ptr;
	mach_port_t             reply_port;
	int                     response_id;
	int                     sf_error;   /* stripefile operation error */
	uint_t                  sfactor;
	int			sfd;
	transaction_id_t	trans_id;
	int			refs;

	int			error = ESUCCESS;
	int                     num_sent = 0;
	mach_port_t             task0_port = MACH_PORT_NULL;

	struct dup_send_msg {
	    mach_msg_header_t    hdr;
	    mach_msg_type_t      type;
	    mach_port_t          gopen_reply_port;
	} dup_send_msg;

	struct dup_receive_msg {
	    mach_msg_header_t    hdr;
	    mach_msg_type_t      type;
	    mach_port_t          file_port;
	} dup_receive_msg;



	/*
	 * Get the fdte pointer for this file::
	 */
	if (error = fdt_ref_entry(fildes, &fdte)) {
	    return error;
	}

	if (my_node != 0) {
	    /*
	     * This section of code executed by Logical nodes 1 .. N-1
	     * of the user's application.
	     *
	     * Allocate a "reply" port send it to logical node 0.  Note
	     * that the send right to this port will be sent by
	     * logical node 0 to the file server node(s) owning the
	     * file (and its stripe files).  The file server(s) will
	     * use this port to send the duplicated file port to this
	     * compute node.
	     */
	    mach_error = mach_port_allocate(mach_task_self(),
					    MACH_PORT_RIGHT_RECEIVE,
					    &gopen_reply_port);
	    if (mach_error != KERN_SUCCESS) {
	  	EPRINT(("pfs_dup_fdte_info(): mach_port_allocate %s\n",
		       mach_error_string(mach_error)));
		return(EIO);
	    }

	    /*
	     * Extract a send right from node 0's gopen_reply_port.
	     * We first must get the task port for process 0:
	     */
	    emul_blocking();
	    bsd_task_by_pid(our_bsd_server_port,
			    interrupt,
			    unix_pid0,
			    &task0_port);
	    emul_unblocking();

	    mach_error = mach_port_extract_right(task0_port,
						 reply_port0,
						 MACH_MSG_TYPE_MOVE_SEND,
						 &acquired_right,
						 &acquired_type);
	    if (mach_error != KERN_SUCCESS) {
		EPRINT(("pfs_dup_fdte_info(): extract_right %s\n",
		       mach_error_string(mach_error)));
		return EIO;
	    }

	    /*
	     * Send this node's gopen_reply_port to node 0:
	     */
	    msg_hdr_bits = MACH_MSGH_BITS_COMPLEX |
			   MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);

	    dup_send_msg.hdr.msgh_bits        = msg_hdr_bits;
	    dup_send_msg.hdr.msgh_size        = sizeof(dup_send_msg);
	    dup_send_msg.hdr.msgh_remote_port = acquired_right;
	    dup_send_msg.hdr.msgh_local_port  = MACH_PORT_NULL;
	    dup_send_msg.hdr.msgh_seqno       = 0;
	    dup_send_msg.hdr.msgh_id          = GOPEN_1 + my_node;

	    dup_send_msg.type.msgt_name       = MACH_MSG_TYPE_MAKE_SEND;
	    dup_send_msg.type.msgt_size       = 32;
	    dup_send_msg.type.msgt_number     = 1;
	    dup_send_msg.type.msgt_inline     = TRUE;
	    dup_send_msg.type.msgt_longform   = FALSE;
	    dup_send_msg.type.msgt_deallocate = FALSE;
	    dup_send_msg.type.msgt_unused     = 0;

	    dup_send_msg.gopen_reply_port     = gopen_reply_port;

	    message_rtn = mach_msg((mach_msg_header_t *)&dup_send_msg,
				   (MACH_SEND_MSG + MACH_SEND_INTERRUPT),
				   sizeof(dup_send_msg),
				   0, MACH_PORT_NULL,
				   MACH_MSG_TIMEOUT_NONE,
				   MACH_PORT_NULL);
	    if (message_rtn != MACH_MSG_SUCCESS) {
		EPRINT(("pfs_dup_fdte_info: mach_msg %s\n",
		       mach_error_string(message_rtn)));
		return(EIO);
	    }

	    /*
	     * fix up the ports:
	     */
	    loop_count = 1;
	    if (fdte->pfs_fd) {
		loop_count += fdte->pfs_fd->p_stripe_factor;
	    }
	    for (i1 = 0; i1 < loop_count; i1++) {

		message_rtn = mach_msg((mach_msg_header_t *)&dup_receive_msg,
				       (MACH_RCV_MSG + MACH_RCV_INTERRUPT),
				       0, sizeof(dup_receive_msg),
				       gopen_reply_port,
				       MACH_MSG_TIMEOUT_NONE,
				       MACH_PORT_NULL);
		if (message_rtn != MACH_MSG_SUCCESS) {
		    EPRINT(("pfs_dup_fdte_info: mach_msg %s\n",
			     mach_error_string(message_rtn)));
		    return EIO;
		}
		response_id = dup_receive_msg.hdr.msgh_id & 0xffff;

		if (response_id >= loop_count) {
		    fdte->fp = dup_receive_msg.file_port;
		} else {
		    fdte->pfs_fd->p_stripe_fdt[response_id].s_fp =
		      dup_receive_msg.file_port;
		}
	    }

	    /*
	     * Free up the task port:
	     */
	    if (task0_port != MACH_PORT_NULL) {
	        mach_port_deallocate(mach_task_self(),
				     acquired_right);    /*  task0_port ??  */
	    }

	} else {

	    /*
	     * This section of code is executed by Logical node 0 of the
	     * application partition.
	     *
	     * Build the reply port vector and then send
	     * it to the file server(s) containing the file:
	     */

	    buf_size = (num_nodes - 1) * sizeof(mach_port_t);

	    if (vm_allocate(mach_task_self(),
			    (vm_offset_t *)&buf,
			    (vm_size_t)buf_size,
			    (boolean_t)TRUE) != KERN_SUCCESS) {
	 	error = ENOMEM;
		return error;
	    }

	    port_ptr = buf;
	    for (i1 = 0; i1 < (num_nodes-1); i1++) {

	        message_rtn = mach_msg((mach_msg_header_t *)&dup_receive_msg,
				       (MACH_RCV_MSG + MACH_RCV_INTERRUPT),
				       0, sizeof(dup_receive_msg),
				       gopen_reply_port,
				       MACH_MSG_TIMEOUT_NONE,
				       MACH_PORT_NULL);
		if (message_rtn != MACH_MSG_SUCCESS) {
		    EPRINT(("pfs_dup_fdte_info: mach_msg %s\n",
			     mach_error_string(message_rtn)));
		    return EIO;
		}

		port_ptr[i1] = dup_receive_msg.file_port;
	    }

	    /*
	     * Send the port vector to file server on which the
	     * file resides (and to all of the stripe file serverss):
	     */
	    if (fdte->pfs_fd) {
	        sfactor = fdte->pfs_fd->p_stripe_factor;
	        isc_multi_register(MACH_PORT_NULL,
				   (sfactor+1),
				   NULL,
				   &trans_id);
	    } else {
	        sfactor = 0;
	        isc_register(fdte->fp,
			     &trans_id);
	    }
	    reply_port = pfs_get_reply_port((mach_port_msgcount_t)(sfactor+1));
	    response_id = 1 + sfactor;

	    pfs_error = fsvr_pfs_duplicate_msg_send(
					  fdte->fp,
					  reply_port,
					  credentials_port,
					  trans_id++,
					  response_id,
					  buf,
					  (num_nodes - 1));
	    if (pfs_error) {
		EPRINT(("dup_fdte: error from duplicate_msg_send(%d) = 0x%x\n",
		       response_id, pfs_error));
		error = pfs_error;
		goto cleanup;
	    }

	    num_sent++;

	    if (fdte->pfs_fd) {
		/*
		 * Send in parallel the port vector to each of file's
		 * stripe directories.
		 */
		for(sfd=0; sfd < sfactor; sfd++) {

		    /*  response_id = sfd  */
		    pfs_error = fsvr_pfs_duplicate_msg_send(
					  fdte->pfs_fd->p_stripe_fdt[sfd].s_fp,
					  reply_port,
					  credentials_port,
					  trans_id++,
					  sfd,
					  buf,
					  (num_nodes - 1));
		    if (pfs_error) {
			EPRINT(("dup_fdte: duplicate_msg_send(%d) = 0x%x\n",
			       sfd, pfs_error));
			error = pfs_error;
		        break;
		    }
		    num_sent++;
		}
	    }

	    for (; num_sent > 0; num_sent--) {

	        sf_error = fsvr_pfs_duplicate_msg_receive(
					  MACH_PORT_NULL,
					  reply_port,
					  credentials_port,
					  0,
					  &sfd,
					  &buf,
					  0);
		if (pfs_error)
		    continue;	        /* toss the response      */
		if (sf_error) {
		    /*
		     * remember the error and toss the response
		     */
		    pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
		    error = pfs_error;
#ifdef  DEBUG_PFS
		    if (sf_error < 0)
		        PFS_DEBUG(("  e_pfs_dup_fdte_info: mach_msg: %s\n",
				   mach_error_string(sf_error)));
#endif
		    continue;
		}
	    }

cleanup:
	    isc_deregister(interrupt);

	    /*
	     * Clean-up the ports and memory used in the
	     * duplicate operation:
	     */
	    port_ptr = buf;
	    for (i1 = 0; i1 < (num_nodes-1); i1++) {
		mach_error = mach_port_destroy(mach_task_self(),
					       port_ptr[i1]);
		if (mach_error != KERN_SUCCESS) {
		    EPRINT(("pfs_dup_fdte_info(): mach_port_destroy %s\n",
			     mach_error_string(mach_error)));
		    return EIO;
		}
	    }

	    (void) vm_deallocate(mach_task_self(),
				 (vm_address_t)buf,
				 (vm_size_t)buf_size);

	}

	mach_error = mach_port_destroy(mach_task_self(),
				       gopen_reply_port);
	if (mach_error != KERN_SUCCESS) {
	    EPRINT(("pfs_dup_fdte_info(): mach_port_destroy %s\n",
		   mach_error_string(mach_error)));
	    return EIO;
	}

	(void) fdt_unref_entry(fdte);

	return error;
}



/*
 * NAME:	e_pfs_host_init
 *
 *
 * DESCRIPTION:
 *	      This function is used to pre-parse
 *	      the pfs strip directories and cache their vnode ports.
 *
 * PARAMETERS:
 *
 *		serv_port	Mach port to the OSF/1 server.
 *
 *		interrupt	Pointer to interrupt variable,
 *				initially set to FALSE.  Signals 
 *				interrupt.
 *
 *	        path		Path to the pfs file system
 *
 * RETURNS:
 *		ESUCCESS;
 *              EFAULT;
 *              ENAMETOOLONG;
 *              ENOENT
 */
int
e_pfs_host_init(serv_port, interrupt, path)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*path;
{
	register int		error;
	mach_port_t		start_port;
	transaction_id_t	trans_id;
	int			len_path;
	char			pfs_path[MAXPATHLEN];
	boolean_t		pfs_filesys = TRUE;
	char			*migbuf;
	int			len_migbuf = STATPFS_BUFSZ;
	uint_t			mig_actual = 0;
	struct statpfs	       *stripe_attr;	/* PFS stripe attributes */


	PFS_TRACE(("e_pfs_host_init: path=%s\n", path));

	if (!user_strlen(path, &len_path))
		return(EFAULT);
	if (pfs_scanpath(path, pfs_path, &len_path))
		return(EFAULT);
	path = pfs_path;

	if (*path == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*path == '/') ? rootdir_port : currentdir_port;

	/*
	 * Get the PFS stripe attributes.  
	 *
	 * A loop is used because the statpfs structure is variable size and
	 * may require a bigger request size to hold the information.  Also the
	 * size may change due to another mount operation taking place.
	 */
	for (;;) {
		isc_register(start_port, &trans_id);
		error = fsvr_statpfs(start_port, credentials_port, trans_id,
				     rootdir_port, path, len_path+1,
				     len_migbuf, &migbuf, &mig_actual);
		isc_deregister(interrupt);
		if (error) {
			if (error == EFSNOTSUPP) {
				pfs_filesys = FALSE;
				error = ESUCCESS;
				goto out;
			} else {
				return(error);
			}
		}

		/*
		 * Exit the loop if the request is satisfied.
		 */
		if (len_migbuf >= ((struct statpfs *)migbuf)->p_reclen) {
			break;
		}

		/*
		 * Else need to try a bigger request size.
		 */
		len_migbuf = ((struct statpfs *)migbuf)->p_reclen;
		(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
				     mig_actual);
	}

	/*
	 *  These stripe attributes need to be moved so that
	 *  they are between EMULATOR_START and EMULATOR_END so that
	 *  the cache is preserved across execs.
	 */
	error = vm_allocate(mach_task_self(), 
			    (vm_address_t *)&stripe_attr, 
			    (vm_size_t) mig_actual,
			    (boolean_t)TRUE);
	if (error)
	    goto out1;

	/* Copy the stripe attrs to the new buffer. */
	if (!user_bcopy(migbuf, stripe_attr, mig_actual)) {
	    error = EFAULT;
	    goto out1;
	}
	

	PFS_DEBUG_SATTR(("e_pfs_host_init: header contents:\n"),
			stripe_attr);

	error = pfs_multi_parse(interrupt, stripe_attr, mig_actual);

out1:
	if (error)
	    (void) vm_deallocate(mach_task_self(), (vm_address_t)stripe_attr,
				 mig_actual);

out:

	(void) vm_deallocate(mach_task_self(), (vm_address_t)migbuf,
			     mig_actual);
	return(error);
}


#ifdef	DEBUG_PFS

/*
 *	print out some information about the port name.
 */
void
display_port_rights(node, task, name)
	int		node;
	mach_port_t	task, name;
{
	mach_port_type_t	rights;
	kern_return_t	kr;
	int     s_count;
	char    p_buf[256];
	char    *s_ptr = p_buf;

	kr = mach_port_type(task, name, &rights);
	if (kr != KERN_SUCCESS) {
		e_printf("mach_port_type: [0x%x] %s\n",
			kr, mach_error_string(kr));
		return;
	}

	s_ptr += sprintf(s_ptr, "(node %d) port rights for name=0x%x:",
			  node, name);
	if (rights & MACH_PORT_TYPE_SEND)
		s_ptr += sprintf(s_ptr, " send");
	if (rights & MACH_PORT_TYPE_RECEIVE)
		s_ptr += sprintf(s_ptr, " recv");
	if (rights & MACH_PORT_TYPE_SEND_ONCE)
		s_ptr += sprintf(s_ptr, " send-once"); 
	if (rights & MACH_PORT_TYPE_PORT_SET)
		s_ptr += sprintf(s_ptr, " port-set");
	if (rights & MACH_PORT_TYPE_DEAD_NAME)
		s_ptr += sprintf(s_ptr, " dead-name");
	if (rights & MACH_PORT_TYPE_DNREQUEST)
		s_ptr += sprintf(s_ptr, " dead-name-request");
	if (rights & MACH_PORT_TYPE_MAREQUEST)
		s_ptr += sprintf(s_ptr, " msg-accepted-request");
	if (rights & MACH_PORT_TYPE_COMPAT)
		s_ptr += sprintf(s_ptr, " compat");
	s_ptr += sprintf(s_ptr, "\n");

	e_printf(p_buf);
}
#endif	DEBUG_PFS
#endif	PFS
