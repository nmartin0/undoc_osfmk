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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_setiomode.c,v 1.12 1994/11/19 02:28:58 mtm Exp $
 *
 * Set the I/O mode and perform a global syncronization.
 *
 *      HISTORY:
 *      $Log: _setiomode.c,v $
 * Revision 1.12  1994/11/19  02:28:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/08/31  20:22:34  bradf
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
 * Revision 1.10.2.1  1994/08/19  22:56:36  dbm
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
 * Revision 1.10  1994/06/13  15:45:53  rlg
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
 * Revision 1.9  1993/10/26  16:54:06  shala
 * Fixed the type definition, so it matches usr/include/nx.h for R4.5 compiler
 * to compile correctly.
 *
 * Revision 1.8  1993/06/23  23:19:20  dbm
 * Modified to work correctly with one node.
 *
 * Revision 1.7  1993/04/02  22:41:34  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.6.2.2  1993/01/08  23:33:58  dbm
 * Added changes to support PFS files and iomodes.
 *
 * Revision 1.6.2.1  1993/01/08  21:50:36  dbm
 * Added space to test PFS branch.
 *
 * Revision 1.6  1992/09/13  21:12:06  brad
 * Changed iomode value check to return error on M_GLOBAL, for now.
 *
 * Revision 1.5  1992/08/06  13:35:10  dbm
 * Changed communication functions to use the '_' versions and handle
 * the error messages.
 *
 * Revision 1.4  92/08/05  09:04:54  dbm
 * Added History also added setting of errno to ENOMEM if can't malloc memory.
 * 
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <syscall.h>
#include "nx.h"
#include "pfs_iomode.h"

/*
 * _setiomode.c
 *
 * Description:
 *		This function is used to set the I/O mode of the file 
 *		specified by the 'fd' parameter.  At the same time, 
 *		a global synchronizing operation is performed.
 *		This function will return a non zero return
 *		value on success. On failure, a negative value will
 *		be returned and errno will contain the error code.
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
long 
_setiomode(fdes, iomode)
int fdes;
int iomode;
{
	int			fp_info_size;
	int			global_size;
	pfs_setio_info_t	local_iomode_info;

	int			status = 0;
	pfs_setio_info_t	*global_iomode_info = NULL;


	/*
	 * Make sure valid iomode:
	 */
	if ((iomode < M_UNIX) || (iomode > M_ASYNC)) {
		errno = EINVAL;
		return  -1;
	}

	/*
	 * Get the file descriptor information from the emulation library.
	 */
	if (syscall(SYS__get_local_iomode_info,
		    fdes, 
		    &local_iomode_info,
		    &fp_info_size) < 0) {
		return -1;
	}

	/*
	 * Determine if we are switching between modes and then let
	 * the emulation library know.
	 */
	if (local_iomode_info.iomode != M_UNIX) {  /* Resetting iomodes. */
			if (_gsync()< 0) {	/* Sync everyone up first */
				return -1;
			}
			/*
			 * Get local information again to make sure correct:
			 */
			if (syscall(SYS__get_local_iomode_info,
				    fdes, 
				    &local_iomode_info,
				    &fp_info_size) < 0) {
				return -1;
			}

			if (syscall(SYS__reset_iomode_info,
				    fdes, 
				    iomode) < 0 ) {
				printf("error from reset\n");
				return -1;
			}
	} 

	/*
	 * Check for file consistency.
	 */
        if ((numnodes() > 1)    &&
	    (iomode != M_UNIX)  && 
	    (iomode != M_ASYNC)) {
                /*
                 * Allocate the memory for the setiomode info:
                 */
                global_size = sizeof(pfs_setio_info_t) * numnodes();
		global_iomode_info = (pfs_setio_info_t *)malloc(global_size);
                if (global_iomode_info == NULL) {
                        errno = ENOMEM;
                        return -1;
                }

		/*
		 * Distribute the data and compare the results:
		 */
                if ((status = exchange(&local_iomode_info,
			               global_iomode_info, numnodes())) < 0) { 

			if (global_iomode_info) {
				free(global_iomode_info);
			}
			return status;
		}

	}

	status = _pfs_setiomode(fdes, iomode, fp_info_size);

	if (global_iomode_info) {
		free(global_iomode_info);
	}

	if (_gsync() < 0) {	/* Synchronize everyone up. */
		return -1;
	}
	return status;
}



/*
 * exchange
 *
 * Description:
 *
 *		This is a local function that is used to exchange the iomode
 *		information using nx message passing. 
 *
 * Parameters:	
 *		local_iomode_info,	local iomode information.
 *		global_iomode_info:	collected iomode information.
 *		number_of_nodes:	Number of nodes in the 
 *
 * Returns:
 *
 *		Status of the operation.
 *		This function will return a 0 on success, on failure a -1
 *		will be returned and errno will be set to the error code.
 */
static int 
exchange(local_iomode_info, global_iomode_info, number_of_nodes)
pfs_setio_info_t	*local_iomode_info;	/* Local iomode info  */
pfs_setio_info_t	*global_iomode_info;	/* Global iomode info */
int 			number_of_nodes;	/* Number of nodes    */
{
	int i;
	int *lengths;


	lengths = (int *)malloc(sizeof(int) * number_of_nodes);
	if (lengths == NULL) {
		errno = ENOMEM;
		return -1;
	}

	for(i=0;i<number_of_nodes;i++) {
		lengths[i] = sizeof (pfs_setio_info_t);
	}

	/*
	 * Get all of the information from every node.
	 */ 
	if (_gcolx((char *)local_iomode_info,
		   (long *)lengths,
		   (char *)global_iomode_info) < 0) {
		free(lengths);
		return -1;
	}

	free(lengths);

	/*
	 * Make sure all of the parameters are correct:
	 */
	for (i=0; i< number_of_nodes; i++ ) {
		if ((local_iomode_info->device_id != 
		     global_iomode_info[i].device_id)   ||
		    (local_iomode_info->inode_id != 
		     global_iomode_info[i].inode_id) 	||
		    (local_iomode_info->node_id !=
		     global_iomode_info[i].node_id)	||
		    (local_iomode_info->offset.slow !=
			 global_iomode_info[i].offset.slow) ||
		    (local_iomode_info->offset.shigh !=
			 global_iomode_info[i].offset.shigh) ||
		    (local_iomode_info->iomode !=
			 global_iomode_info[i].iomode)) { 	

			errno = EMIXIO;
			return -1;

		}
	}
	return 0;
}
