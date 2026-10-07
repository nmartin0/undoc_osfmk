/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 *      Copyright 1992  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_pfs_setio.c,v 1.7 1994/11/19 02:28:56 mtm Exp $
 *
 * Set the I/O mode of a file.
 *
 *      HISTORY:
 *      $Log: _pfs_setio.c,v $
 * Revision 1.7  1994/11/19  02:28:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/08/31  20:22:33  bradf
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
 * Revision 1.5.2.1  1994/08/19  22:54:14  dbm
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
 * Revision 1.5  1994/06/13  15:44:06  rlg
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
 * Revision 1.4  1993/07/27  19:55:05  dbm
 * Fixed a race condition with the creation of the ports.
 *
 * Revision 1.3  1993/07/16  03:10:29  dbm
 * Added functionality to support parallel file access.
 *
 * Revision 1.2  1993/04/02  22:41:32  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2  1993/02/12  17:31:38  dbm
 * Fixed one system call to return the result properly.
 *
 * Revision 1.1.2.1  1993/01/08  23:42:44  dbm
 * Initial version to use pfs i/o modes with pfs files.
 *
 * Revision 1.1  1993/01/08  23:37:33  dbm
 * Initial revision
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <syscall.h>
#include "nx.h"
#include "pfs_iomode.h"


/*
 * _pfs_setio.c
 *
 * Description:
 *		This function is used to set the I/O mode of the file 
 *		specified by the 'fd' parameter. 
 *	
 * Parameters:	
 *		fildes		File descriptor.
 *
 *		iomode		The I/O mode to be assigned to the file
 *				associated with fildes.  Values for the iomode
 *				parameter are as follows:
 *
 *			M_UNIX		Individual file pointer.
 *
 *			M_LOG		Common file pointer.
 *
 *			M_SYNC		Synchronized, common file pointer,
 *						variable length requests.
 *
 *			M_RECORD	Synchronized, common file pointer,
 *						fixed length requests.
 *
 *			M_GLOBAL	Synchronized, common file pointer,
 *						synchronized requests.
 *
 *			M_ASYNC		Individual file pointer, no token
 *					synchronization on reads *or* writes.
 *
 * Returns:
 *		Status of the operation.
 */
int 
_pfs_setiomode(fdes, iomode, fp_info_size)
int	fdes;			/* File descriptor. 	*/
int 	iomode;			/* I/O mode. 		*/
int	fp_info_size;		/* Size of fp_info 	*/
{
	int status = 0;
	int save_errno;
	int release_local = 0;

	pfs_comm_info_t		local_com_info;
	pfs_comm_info_t		*global_com_info = NULL;
	pfs_fp_info_t		*local_fp_info = NULL;
	pfs_fp_info_t 		*global_fp_info = NULL;


	local_com_info.unix_pid   = getpid();

	/*
	 * Malloc some space for the local fp information.
	 */
	local_fp_info = (pfs_fp_info_t *)malloc(fp_info_size);
	if (local_fp_info == NULL) {
		errno = ENOMEM;
		goto out;
	}

	/*
	 *   Call down to the emulation library to perform local
	 *   iomode setup first.  This is to make sure that any local 
	 *   information is put into local com and fp structures prior
	 *   to the exchange of data.  
	 */
	if ((status = syscall(SYS__set_local_iomode_info,
			      fdes,
			      iomode,		
			      mynode(),
			      numnodes(),
			      &local_com_info,
			      local_fp_info)) < 0) {
		goto out;
	}

	/*
	 * Exchange iomode information with all of the other tasks, this gives
	 * us a data structure, "global_iomode_info", which has the contents
	 * of each of the other tasks local_iomode_info values. 
	 */
	if ((numnodes() > 1) &&
	    (iomode != M_UNIX) && 
	    (iomode != M_ASYNC)) {

		release_local = 1;

		/*
		 * Allocate the memory for file pointer info:
		 */
		global_fp_info = (pfs_fp_info_t *)
		        malloc(fp_info_size * numnodes());
		if (global_fp_info == NULL) {
			errno = ENOMEM;
			goto out;
		}

		/*
		 * Distribute the file pointer information.
		 */
		if (exchange(local_fp_info,
		       	     global_fp_info,
			     fp_info_size,
		       	     numnodes()) < 0) {
			goto out;
		}

		/*
		 * Allocate the memory for the communications info:
		 */
		global_com_info = (pfs_comm_info_t *)malloc(
			sizeof(pfs_comm_info_t) * numnodes());
		if (global_com_info == NULL) {
			errno = ENOMEM;
			goto out;
		}
		
		if ((status  = exchange(&local_com_info,
		       	   		global_com_info,
					sizeof(pfs_comm_info_t),
		       	  		numnodes())) < 0) {
			goto out;
		}

		/*
		 * Call down to the emulation library to setup 
		 * the global iomode information.
		 */
		status =  syscall(SYS__set_global_iomode_info,
				  fdes,
				  global_com_info,
				  global_fp_info,
				  fp_info_size);

		/*
		 * Need a gsync here so that one node does not get
		 * ahead of another and send a message to a non existing
		 * port.
		 */
		gsync();
	}
	return status;


out:
	save_errno = errno;
	/*
	 * Clean up any local iomode state if the
	 * exchange() failed.
	 */
	if (release_local) {
		syscall(SYS__release_iomode_info,
			fdes,
			&local_com_info,
			&local_fp_info);
	}

	if (local_fp_info != NULL){
		free (local_fp_info);
	}

	if (global_fp_info != NULL) {
		free (local_fp_info);
	}

	if (global_com_info != NULL) {
		free (global_com_info);
	}

	errno= save_errno;
	return status;
}



/*
 * exchange
 *
 * Description:
 *		This is a local function that is used to exchange the iomode
 *		information using nx message passing. 
 *
 * Parameters:	
 *		local__info,		local information.
 *		global_iomode_info:	global information.
 *		info_size:		number of bytes of info.
 *		number_of_nodes:	number of nodes.
 *
 * Returns:
 *		Status of the operation.
 *		This function will return a 0 on success, on failure a -1
 *		will be returned and errno will be set to the error code.
 */
static int 
exchange(local_info, global_info, info_size, number_of_nodes)
char 	*local_info;		/* Local iomode info  */
char	*global_info;		/* Global iomode info */
int	info_size;		/* Number of bytes of information. */
int 	number_of_nodes;	/* Number of nodes    */
{
	int i;
	int *lengths;


	lengths = (int *)malloc(sizeof(int) * number_of_nodes);
	if (lengths == NULL) {
		errno = ENOMEM;
		return -1;
	}

	for(i=0;i<number_of_nodes;i++) {
		lengths[i] = info_size;
	}

	/*
	 * Get all of the information from every node.
	 */ 
	if (_gcolx(local_info,
		   (long *)lengths,
		   global_info) < 0) {

		free(lengths);
		return -1;
	}

	free(lengths);
	return 0;
}
