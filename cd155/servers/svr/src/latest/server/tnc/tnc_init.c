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
 * $Log: tnc_init.c,v $
 * Revision 1.8  1995/02/01  21:54:43  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:44:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:34:47  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:48:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:25:54  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:24  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:09:36  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:15  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:02:44  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:48:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:46:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.6  1992/12/02  11:11:21  chrisp
 * Add tnc_startup() which is called by startup() prior to allocating
 * 	system tables and allows TNC to dynamically determine the
 * 	size of the vproc table.
 *
 * Revision 3.5  92/07/09  11:34:47  roman
 * Add call to tnc_exception_setup(), which sets up the emulator exception
 * addresses. The call is performed on all nodes not running the init process.
 * 
 * Revision 3.4  92/07/08  09:11:38  roman
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.3  92/03/12  09:51:39  klh
 * Allocate a vproc for pid 1 (init) on nodes other than its execution node.
 * 
 * Revision 3.2  91/12/13  11:07:15  roman
 * Add external of tnc_mynode.
 * 
 * Revision 3.1  91/10/25  10:46:28  roman
 * Added call to tnc_pidgen_init().
 * 
 * Revision 3.0  91/10/21  13:19:58  roman
 * Initialize TNC environment. Initial submission.
 * 
 */

/*
 * Single initialization routine used for all TNC initialization.
 */

#include <sys/types.h>
#include <tnc/dpvproc.h>
#include <uxkern/import_mach.h>

void
tnc_init()
{
	extern node_t root_fs_node;

	tnc_ipc_init();
	tnc_pidgen_init();

	/*
	 * If this is not the root filesystem node on which init runs,
	 * we must create a vproc for init (pid 1) so that VPROCPTR(1)
	 * can henceforth succeed. Additonally, do the server exception
	 * handler set-up for non-init nodes.
	 */
	if (this_node != root_fs_node) {
		(void) LOCATE_VPROC_PID(1);
		tnc_exception_setup(root_fs_node);
	}
}

/*
 * This routine is called by the system startup() routine
 * prior to allocating system tables. Here we can re-assess
 * system parameters - particularly the size of the vproc table
 * according to the number of nodes.
 */
void
tnc_startup()
{
	extern int	node_array_entries;
	extern int	node_array[][2];
	extern int	nproc;
	extern int	nvproc;
	int		i;
	int		num_nodes;

	/*
	 * Determine the number of nodes in the configuration
	 */
	num_nodes = node_array_entries;
	for (i = 0; i < node_array_entries; i++)
		num_nodes += node_array[i][1] - node_array[i][0];

	/*
	 * The maximum number of vprocs possibly required is determined
	 * to be:
	 *   o	the maximum number of physical procs (nproc) times the number
	 *	of nodes, plus
	 *   o	an allocation of nproc for pgroups with defunct leaders, plus
	 *   o  an allocation of nproc required for kill()s to unknown
	 *	processes.
	 */
	nvproc = (nproc * num_nodes) + (nproc * 2);

}
