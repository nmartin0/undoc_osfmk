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
 * $Log: dvp_vpsops.c,v $
 * Revision 1.44  1995/04/11  01:31:38  toman
 * Replaced panic in dvpsop_pid_from_task() with return(ESRCH) so that
 * a call to pid_from_task() with an invalid task port or a deadname will
 * not panic but return an error to the user program.
 *
 *  Reviewer: Bob Yasi, John Litvin
 *  Risk: low
 *  Benefit or PTS #: 11707
 *  Testing: Simple testcase, server running at customer site
 *  Module(s): server/tnc/dvp_vpsops.c
 *
 * Revision 1.43  1995/02/01  21:43:16  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.42  1994/12/18  23:14:25  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 11767
 *  Testing: EATs controlc, ipd
 *  Module(s): server/tnc/dvp_vpsops.c
 * Do not allow the table() calls that specify a pid, to operate on more than
 * one pid at a time.
 *
 * Revision 1.41  1994/11/18  20:43:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.40  1994/06/17  15:00:09  paul
 * Changes in support of netstat. Fixed the TBL_RTREE code to talk to all the
 * network servers to get all the routes system wide, rather than just return
 * routes on the node the user happens to randomly get. Also, added a debug
 * version of the TBL_RTREE_NODE table() call which returns invisible MI routes.
 * Also, fixed the table() command to return tnc style <node>interface type
 * interface names i.e. <201>el0.
 *
 *  Reviewer: Bernie Keany
 *  Risk: M
 *  Benefit or PTS #: 7952 8059
 *  Testing: on Plymouth, with 1, 2, & 3 netservers, boot/non-boot configs
 *  Module(s): tnc/dvp_vpsops.c bsd/cmu_syscalls.c sys/table.h vsocket/if_mi.c
 * 	    vfs/vfs_syscalls.c
 *
 * Revision 1.39  1994/05/05  20:53:29  cfj
 * Modify dvpsop_table() so that TBL_PGINFO is always sent
 * to the root_fs_node since that is the only node which
 * has the information.
 *
 *  Reviewer:
 *  Risk:L
 *  Benefit or PTS #:9297
 *  Testing:test case
 *  Module(s):server/uxkern/fsvr_msg.c
 * 	   server/tnc/dvp_vpsops.c
 *
 * Revision 1.38  1994/04/13  19:25:13  chrisp
 * Add missing externs for node_array[][] and node_array_entries in
 * routine build_expended_node_entries to keep the i396 gcc compiler happy.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 8958
 *  Testing: Now compiles for i386.
 *  Module(s):
 *
 * Revision 1.37  1994/04/04  17:29:56  paul
 *  Reviewer:John Litvin
 *  Risk: M
 *  Benefit or PTS #:7046
 *  Testing: test program run on Plymouth several thousand times...
 *  Module(s):
 * 	server/tnc/dvp_vpsops.c
 * 	server/tnc/dpvproc_struct.h
 * 	server/tnc/tnc_types.h
 * 	server/tnc/tnc_types.defs
 * 	server/tnc/rvp_pvpsops.c
 * 	server/tnc/dvp_pvpsops.c
 * 	server/bsd/kern_xxx.c
 * 	server/tnc/rvp_pvpsops_server.c
 * Use spanning trees for setting global hostname, hostid, domain name,
 *     timezone, rpm offset
 *
 * Revision 1.36  1994/03/14  02:05:23  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.35  1994/02/07  18:36:53  stefan
 * Merged fix for PTS #7899 from R1_2 branch into main trunk.
 *
 * Revision 1.32.2.3  1994/02/07  14:42:17  stefan
 * In order for enabling load_leveld be able to map the node number of
 * ROOT_FS_NODE to the corresponding logical node number a new option
 * TBL_PHYSNODEINFO had to be added to the table() system call.
 * TBL_PHYSNODEINFO returns the list of physical nodes in a partition.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 7899
 *  Testing: developer testing
 *  Module(s): server/sys/table.h
 *             server/bsd/cmu_syscalls.c
 *             server/tnc/dvp_vpsops.c
 *
 * Revision 1.32.2.2  1993/12/21  22:33:21  dbm
 *  Reviewer: Nina Lepak , B Olsen (Locas), Dave Minturn
 *  Risk: Medium
 *  Benefit or PTS #:7424
 *  Testing: Tested for specific test case under 1.2.  This is
 * 	  the 1.2 branch checkin for bug described in the
 * 	  1.34 revision.
 *  Module(s):
 * 	dvp_vpsops.c
 *
 * Revision 1.34  1993/12/10  21:36:52  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6917, #7421, #7422, #7423
 * #7424 and #7426.  TNC FIFO hook in namei() err-
 * oneously assumed that the ni_cdir field in struct
 * nameidata is never NULL. It can be if the current
 * directory is remote.
 *
 *
 *  Reviewer:bolsen@locus.com, dbm@ssd.intel.com
 *  Risk:Medium
 *  Benefit or PTS #:7424
 *  Testing:Lachman NFS main suite, various configurations
 *  Module(s):./server/tnc/dvp_vpsops.c
 *
 * Revision 1.33  1993/12/03  20:09:40  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Removed some debug printfs from the rpm code.
 *
 * Added dvpsop_settimezone() which is the code which iterates through the
 * nodes, and will be replaced with spanning tree code.
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #: :Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/tnc/dvp_vpsops.c
 *
 * Revision 1.32  1993/10/29  11:55:21  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.31  1993/10/21  23:34:49  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.30  1993/10/15  16:05:18  cfj
 * Remove the R1.0 compatibility for TBL_IPD_MSG_INFO.
 *
 * Revision 1.29  1993/09/09  16:07:17  cfj
 * Part of the fix for PTS bug #6449.  In the function dvpsop_table() ifdef
 * out the check for super user for TBL_ARGUMENTS, TBL_ENVIRONMENT and TBL_UAREA
 * and allow the node where the target process exists do the check.
 *
 * Revision 1.28  1993/09/01  01:36:37  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.27  1993/08/18  00:03:05  hobbes
 * Added support for the hippi_showmap command .. TBL_HIPPI_ART.
 *
 * Revision 1.26  1993/08/11  18:29:18  stefan
 * Modified dvpsop_reset_boot_node_list to call PVPSOP_RESET_BOOT_NODE_LIST
 * on the local node with a copy of BOOT_NODE_LIST. This is necessary as the
 * !@#$%^&* boot magic parser trashes the input string. This is a fix for bug
 * # 5447.
 *
 * Revision 1.25  1993/07/14  18:32:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.8  1993/07/01  20:45:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.24  1993/06/09  00:09:34  cfj
 * Change occurances of #include <i860ipsc/mcmsg/*.h> to #include <i860paragon/mcmsg/*.h>
 *
 * Revision 1.23  1993/06/08  12:53:03  stefan
 * Reintroduced PVPSOP_TABLE_SET for static load leveling.
 *
 * Revision 1.22  1993/06/01  16:11:07  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.21  1993/05/27  23:39:11  cfj
 * Modify dvpsop_table() so that in the case where it is setting a
 * table entry, to change the sign of nel back to positive before
 * using it.
 *
 * Revision 1.20  1993/05/20  16:03:22  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.19  1993/05/19  21:18:39  cfj
 * Use is_tnc_node_valid() istead of is_node_valid().
 *
 * Revision 1.18  1993/05/17  19:06:59  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.17  1993/05/13  09:14:56  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.16  1993/05/11  17:53:33  cfj
 * Make changes so that TBL_PROCINFO works for processes which have called join_root_part().
 *
 * Revision 1.15  1993/05/06  19:22:22  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.5  1993/05/03  17:45:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.14  1993/04/27  16:37:57  stefan
 * Fixed memory leaks which could occur on error returns in dvpsop_table() when
 * doing TBL_PROCINFO of a partition.
 *
 * Revision 1.13  1993/04/26  19:29:43  cfj
 * Fix a memory leak in dvpsop_table() in the case where it was doing a TBL_PROCINFO of a partition.
 *
 * Revision 1.12  1993/04/03  03:08:43  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.11  1993/03/29  18:07:02  stefan
 * Made this file compile by removing duplicate "case TBL_RTREE_NODE:" line.
 *
 * Revision 1.10  1993/03/26  18:43:40  cfj
 * T9 merge.
 *
 * Revision 1.6.4.5  1993/03/26  18:37:52  cfj
 * netstat fix from rkl.
 *
 * Revision 1.9  1993/03/25  23:26:14  cfj
 * T9 Merge.
 *
 * Revision 1.6.4.4  1993/03/24  23:42:16  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.8  1993/03/17  15:47:20  cfj
 * Merge with T9.
 *
 * Revision 1.6.4.3  1993/03/17  15:40:36  cfj
 * Check for debug_sethostname and if set, print the node number before sending the RPC.
 *
 * Revision 1.7  1993/02/23  16:47:23  cfj
 * Restrict TBL_PROCINFO to the root_fs_node if the system is not in multi-user
 * mode indicated by whether the vnode_pager_is_set variable is set.  This is
 * a bit of a kludge, but there is no other way for the server to know.
 *
 * Revision 1.6.4.1  1993/02/16  22:16:28  nandy
 * is_node_valid() is called for  TBL_PROCINFO to make sure that the node
 * got booted.
 *
 * Revision 1.1.2.4.2.3  1993/02/16  20:06:03  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.6  1993/01/21  19:24:23  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.1.2.4.2.2  1992/12/16  06:02:01  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.4.2.1  1992/12/14  23:21:23  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.5  1992/12/11  03:01:34  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.4  1992/11/30  22:47:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.6  1992/11/25  21:37:35  nandy
 * TBL_ARGUMENT permission checks changed.
 *
 * Revision 1.1.2.5  1992/11/19  17:14:37  nandy
 * TBL_PROCINFO now gets information from nodes in the partition of the caller
 * instead of all the nodes in the system. This was done to make ps report
 * processes on service partitin only
 *
 * Revision 1.1.2.4  1992/11/15  22:11:20  cfj
 * Fix it so that table() knows about TBL_IPD_MSG_INFO.
 *
 * Revision 1.1.2.3  1992/11/13  18:52:53  cfj
 * Added reset_boot_node_list() system call.
 *
 * Revision 1.1.2.2  1992/11/06  20:31:10  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:45:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.17  93/09/16  08:55:21  chrisp
 * [SPE 0030] Generic Spanning Trees: dvps_ops_gen.h now dvps_protos_gen.h.
 * 
 * Revision 3.16  93/08/26  10:46:19  mjl
 * Constants in obsolete <vsocket/ins_var.h> are now in <vsocket/vs_types.h>.
 * 
 * Revision 3.15  93/06/11  12:11:32  slively
 * Added bug number to comment for SVR398 checkin and rearranged a fix
 * that converts a negative number to a positive so that it may be used
 * as an index.
 * 
 * Revision 3.14  93/06/02  09:59:35  yazz
 * For Sys V IPC under TNC change the "root fs node only" category of
 * table() operations to "svipc server node only" operations.  All
 * these operations were already related to Sys V IPC operations.
 * 
 * Revision 3.13  93/06/01  13:00:23  nina
 * Fix two problems in TBL_IF case.  index wasn't being set
 * properly and wrong node number was being passed to
 * PVPSOP_TABLE_GET.  In addition, incorporate a fix from
 * INTEL for TBL_MAXUPRC table set operations.
 * 
 * Revision 3.12  93/05/14  12:13:50  slively
 * [Bug 208] Change the call to PVPSOP_TABLE_SET to PVPOP_TABLE_SET.
 * 
 * Revision 3.11  93/05/07  15:10:09  nina
 * Modified table code to support the new MI driver
 * and improve performance of route table queries.
 * 
 * Revision 3.10  93/05/03  13:53:43  yazz
 * Added new table() calls TBL_UNPCB and TBL_PGINFO (part of
 * ad1.0.3 merge).
 * 
 * Revision 3.9  93/02/22  17:19:18  mjl
 * Add support for tables that pack a subtable number into the index arg.
 * 
 * Revision 3.8  93/01/12  15:49:59  roman
 * Include TBL_ARPTAB for the table() system call for TNC.
 * 
 * Revision 3.7  92/12/01  12:32:31  chrisp
 * Correct previous log entry.
 * 
 * Revision 3.6  92/12/01  11:23:15  chrisp
 * Add virtual process system operation for VPSOP_REBOOT().
 * 
 * Revision 3.5  92/11/24  14:21:28  chrisp
 * Major changes to account for changes in table() system call.
 * 	[chrisp for roman]
 * 
 * Revision 3.4  92/11/02  11:39:20  roman
 * Get rid of type buffer_t. It's really a char_array.
 * Fix bug in looping though nodes and averaging loads; had "j = 2" rather
 * 	than "j <= 2".
 * Fix comparison of node numbers when seeing if current node is root file
 * 	system node.
 * 
 * Revision 3.3  92/10/28  15:15:03  roman
 * Add virtual process system ops to allow the correct TNC behavior of
 * 	the table(), sethostname(), sethostid(), and sethostname()
 * 	system calls.
 * Include procedure prototypes generated by makeTNCtables.sh for better
 * 	type-checking.
 * 
 * Revision 3.2  92/10/01  10:29:17  roman
 * Fix up types for clean compilation under gcc.
 * Fix bug where "nice" variable was referenced rather than "node_nice".
 * 
 * Revision 3.1  92/09/30  15:50:21  chrisp
 * In dvpsop_sigprocset(), obtain caller's pid from pproc_get_attr().
 * 
 * Revision 3.0  92/09/28  16:08:05  roman
 * Initial submission of file with TNC version of virtual process system
 * operations.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/vproc.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/user.h>
#include <sys/mbuf.h>
#define TABLE_NETWORK
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/un.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/in_systm.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/in_pcb.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <vsocket/vs_types.h>

#include <machine/machlimits.h>
#include <uxkern/import_mach.h>
#include <tnc/dpvproc.h>

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>

#if __STDC__ == 1
extern int nx_get_info(struct pvproc *pvp,
                       APPLINFO_T *applinfo,
                       LP_MAP_T *nodelist,
                       int      *nodelistcnt);
#else
extern int nx_get_info();
#endif /* __STDC__ */
#endif /* NX */

#ifdef SLL
#include <sll/sll_types.h>
#endif /* SLL */

#include <sys/table.h>
#include <sys/reboot.h>

/* Prototypes generated from vproc.h */
#include <tnc/dvps_protos_gen.h>

/*
 * Virtual process system operation for VPSOP_SIGPROCSET()
 */
int
dvpsop_sigprocset(
	struct procset	*ps,
	int		signo,
	int		arg,
	int		*nproc)
{
	int		proc_count = 0;
	int		error = ESUCCESS;
	node_t		node;
	int		i;
	pid_t		pid, sid;
	uid_t		uid, ruid;
	int		has_priv;
	extern node_t	node_array[][2];
	extern int	node_array_entries;

	/*
	 * Get current process attributes that are used for each PVPSOP.
	 */
	(void) pproc_get_attr(0,&pid,0,0,&sid,&has_priv,&uid,&ruid,0,0);

	/*
	 * Go through all the nodes doing a sigprocset on every node. If the
	 * return is anything other than ESRCH or ESUCCESS, then quit
	 * immediately.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
			int   node_nproc;
			error = PVPSOP_SIGPROCSET(node, 
					ps, 
					signo, 
					arg, 
					uid,
					ruid,
					pid,
					sid,
					&node_nproc,
					has_priv?VPROC_HAS_PRIV:0);
			if (error == ESUCCESS)
				proc_count += node_nproc;
			else if (error != ESRCH)
				return(error);

		}
	}

	/*
	 * If all the calls were successful or the only type of failure found 
	 * was ESRCH and at least one call was successful, then the routine
	 * as a whole is judged a success.
	 */
	if (proc_count > 0)
		error = ESUCCESS;
	if (nproc)
		*nproc = proc_count;

	return(error);
}


/*
 * Virtual process system operation for VPSOP_PROCSET_NICE()
 */
int
dvpsop_procset_nice(
	struct procset	*ps,
	int		*nice,
	int		flag)
{
	node_t		node;
	int		i;
	int		error = ESUCCESS;
	extern node_t	node_array[][2];
	extern int	node_array_entries;

	if (flag & VPROC_SET) {
		uid_t		euid, ruid;
		int		has_priv = TRUE;
		boolean_t	found_one = FALSE;

		/*
		 * Get current process attributes that are used for each PVPSOP
		 */
		(void) pproc_get_attr(0,0,0,0,0,&has_priv,&euid,&ruid,0,0);

		/*
		 * Go through all the nodes doing a procset_nice on every node.
		 * If the return is anything other than ESRCH or ESUCCESS, 
		 * then quit immediately.
		 */
		flag |= (has_priv ? VPROC_HAS_PRIV : 0);
		for (i = 0; i < node_array_entries; i++) {
			for (node = node_array[i][0]; 
					node <= node_array[i][1]; 
					node++){
				error = PVPSOP_PROCSET_NICE(node, 
							    ps, 
							    nice, 
							    euid, 
							    ruid, 
							    flag);
				if (error == ESUCCESS)
					found_one = TRUE;
				else if (error != ESRCH)
					return(error);
			}
		}

		/*
		 * If all the calls were successful or the only type of 
		 * failure found was ESRCH and at least one call was 
		 * successful, then the routine as a whole is judged a success.
		 */
		if (found_one) {
			error = ESUCCESS;
		}
	} else {
		int	low = PRIO_MAX + 1;
		int	node_nice;

		/*
		 * Go through all the nodes doing a procset_nice on every node.
		 * Save the lowest nice value found.
		 */
		for (i = 0; i < node_array_entries; i++) {
			for (node = node_array[i][0]; 
					node <= node_array[i][1]; 
					node++){
				error = PVPSOP_PROCSET_NICE(node, ps, 
							    &node_nice,
							    0, 0, 0);
				if (error != ESUCCESS)
					continue;
				if (node_nice < low)
					low = node_nice;
			}
		}

		/*
		 * If none was found, return ESRCH.
		 */
		if (node_nice == PRIO_MAX + 1)
			error = ESRCH;
		else
			*nice = low;
	}

	return(error);
}


/*
 * Virtual process system operation for VPSOP_PID_FROM_TASK()
 */
int 
dvpsop_pid_from_task(
	task_t		task,
	pid_t		*pid,
	char		*comm,
	unsigned int	*commlen)
{
	int		error;
	kern_return_t	kr;
	node_t		node;

	/*
	 * The task cannot move. Therefore norma_port_location_hint()
	 * always knows where the task is.
	 */
	kr = norma_port_location_hint(mach_task_self(), task, &node);
	if (kr != KERN_SUCCESS)
		return(ESRCH);

	error = PVPSOP_PID_FROM_TASK(node, task, pid, comm, commlen);
	return(error);
}


/*
 * Virtual process system operation for VPSOP_TABLE()
 */
int
dvpsop_table(
	int		id,
	int		index,
	caddr_t		addr,
	int		nel,
	u_int		lel,
	int		*retval)
{
	int		i, j;
	node_t		n;
	int		cnt;
	int		error;
	int		stid, next_index;
	struct vproc	*vp;
	extern node_t	svipc_node;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	extern mach_port_t clearinghouse_port;
	extern mach_port_t inetserver_port;
	int		first;
	unsigned int	bufferlen;

	int		numns;
	int		rel;
	node_t		*np;
	node_t		nodes[MAX_SERVERS];	
	mach_port_t	ports[MAX_SERVERS];

	/* Our preferred network server to get routing, etc. info */
	node_t	nserver = INVALID_NODE;
	mach_port_t	retport;

	union {
		struct tbl_loadavg t_u_tl;
		struct tbl_sysinfo t_u_ts;
		struct tbl_intr t_u_ti;	
		struct tbl_ttyinfo t_u_tt;
#ifdef OSF1_ADFS
		struct rtstat t_u_tr;
		struct mbstat t_u_tm;
		struct ipstat t_u_tip;
		struct icmpstat t_u_tic;
		struct tcpstat t_u_tcp;
		struct udpstat t_u_tu;
		struct nfsstats t_u_tn;
		struct tbl_arpparam t_u_ta;
#endif
	} tbl_data, total_tbl_data, *addr_tbl_data;
#define	tl	tbl_data.t_u_tl
#define ts	tbl_data.t_u_ts
#define ti	tbl_data.t_u_ti
#define tt	tbl_data.t_u_tt
#define tr      tbl_data.t_u_tr
#define tm      tbl_data.t_u_tm
#define tip     tbl_data.t_u_tip
#define tic     tbl_data.t_u_tic
#define tcp     tbl_data.t_u_tcp
#define tu      tbl_data.t_u_tu
#define tn      tbl_data.t_u_tn
#define ta      tbl_data.t_u_ta
#define	ttl	total_tbl_data.t_u_tl
#define tts	total_tbl_data.t_u_ts
#define tti	total_tbl_data.t_u_ti
#define ttt	total_tbl_data.t_u_tt
#define ttr     total_tbl_data.t_u_tr
#define ttm     total_tbl_data.t_u_tm
#define ttip    total_tbl_data.t_u_tip
#define ttic    total_tbl_data.t_u_tic
#define ttcp    total_tbl_data.t_u_tcp
#define ttu     total_tbl_data.t_u_tu
#define ttn     total_tbl_data.t_u_tn
#define tta     total_tbl_data.t_u_ta

#ifdef NX
        APPLINFO_T      applinfo;
        LP_MAP_T        nodelist = (LP_MAP_T)NULL;
        int             nodelistcnt;
        int             has_priv;
        uid_t           uid;
	struct	proc	*p;

#endif	/* NX */



	/*
	 * Perform the appropriate permissions checks. This should be
	 * done on the caller's node. Note that pps_table() also
	 * does permission checks, and the remote pvpsop code makes
	 * sure that the environment is set up in such a way that
	 * the permissions checks never fail.
	 *
	 * Note that the TBL_MAXUPRC permissions checks are ONLY done
	 * in the pps_table() routine.
	 */
	switch(id) {

	case TBL_PROCINFO:
		if (error = suser(u.u_cred, &u.u_acflag))
			return(error);
		break;

	case TBL_ARGUMENTS:
	case TBL_ENVIRONMENT:
	case TBL_UAREA:
#ifndef NX
		if (id != u.u_procp->p_pid && 
				(error = suser(u.u_cred, &u.u_acflag)))
			return(error);
#endif /* NX */
		break;
#ifdef SLL
	case TBL_FASTNODE: {
		/*
		 * Only set operation needs privilege.
		 */
		if ( nel < 0 ) {
#if SEC_BASE
			/*
			 * Must have the SEC_SYSATTR privilege.
			 */
			if ( !privileged(SEC_SYSATTR, EPERM) ) {
				return (EPERM);
			}
#else /* SEC_BASE */
			/*
			 * Must be super user
			 */
			if ( error = suser(u.u_cred, &u.u_acflag) ) {
				return (error);
			}
#endif /* SEC_BASE */
		}
		break;
	}
#endif /* SLL */
	}

	/*
	 * Perform any table set operation.
	 */
	if (nel < 0) {
		switch(id) {

		case TBL_MAXUPRC:
#ifdef NX
		case TBL_IPD_MSG_INFO:
#endif /* NX */
			/*
			 * Operations identified by a process id, and thus
			 * done on a particular vproc.
			 */
			if (nel != -1)		/* handle 1 proc at a time */
				return(EINVAL);
			nel = 1;		/* convert nel to positive */
			for (cnt = 0; cnt < nel; index++, cnt++) {
				vp = LOCATE_VPROC_PID(index);
				if (vp == NULL)
					return(ESRCH);
				bufferlen = lel;
				error = PVPOP_TABLE_SET(vp,
					        	id, 
					        	addr, bufferlen,
							lel);
				VPROC_RELEASE(vp,"vpsop_table");
				if (error != ESUCCESS)
					return(error);
				addr += lel;
			}
			*retval = cnt;
			return(ESUCCESS);

#ifdef SLL
		case TBL_FASTNODE: {
			/*
			 * Operations done only on the current node.
			 */
			bufferlen = -nel * lel;
			return(PVPSOP_TABLE_SET(this_node,
						id, index,
						addr, bufferlen,
						nel, lel,
						retval));
		}
#endif /* SLL */

		default:
			return(EINVAL);
		}
	}

#ifdef NX
	/*
	 *  Check if we are in a partition.
	 *  If so, use TBL_PROCINFO_PART to differentiate.
	 */
	if (id == TBL_PROCINFO || id == TBL_PROCINFO_PART) {
	    vp = LOCATE_VPROC_PID(u.uu_procp->p_pid);

	    if (nx_in_partition( vp )) {
		(void) pproc_get_attr(0,0,0,0,0,&has_priv,&uid,0,0,0);
		if (PVPOP_NX_GET_INFO(vp,
				      &applinfo,
				      &nodelist,
				      &nodelistcnt,
				      has_priv ? VPROC_HAS_PRIV : 0,
				      uid, &error)
		    != KERN_SUCCESS) {
		    nodelist = (LP_MAP_T)NULL;
		    VPROC_RELEASE(vp,"vpsop_table");
		    return (EINVAL);
		}
		id = TBL_PROCINFO_PART;
	    }
	    VPROC_RELEASE(vp,"vpsop_table");
	}
#endif /* NX */

	/*
	 * Table get operations fall into 5 categories:
	 *	1. Those done on a specific process identified by
	 *		pid == index.
	 *	2. Those done only on the current node.
	 *	3. Those done only on the root file system node. 
	 *	4. Those done on all nodes of the system, pretending that
	 *		table entries are really one big system-wide table.
	 *	5. Those done on all nodes of the system, with a single
	 *		"summed up" table entry returned.
	 */
	switch(id) {

	case TBL_UAREA:
	case TBL_ARGUMENTS:
	case TBL_ENVIRONMENT:
	case TBL_MAXUPRC:
	case TBL_U_TTYD:
		/*
		 * Operations identified by a process id, and thus
		 * done on a particular vproc.
		 */
		if (nel != 1)		/* handle 1 proc at a time */
			return(EINVAL);
		for (cnt = 0; cnt < nel; index++, cnt++) {
			vp = LOCATE_VPROC_PID(index);
			if (vp == NULL)
				return(ESRCH);
			bufferlen = lel;
			error = PVPOP_TABLE_GET(vp,
					        id, 
					        &addr, &bufferlen,
						lel);
			VPROC_RELEASE(vp,"vpsop_table");
			if (error != ESUCCESS)
				return(error);
			addr += lel;
		}
		*retval = cnt;
		break;
	
	case TBL_INCLUDE_VERSION:
	case TBL_VPROCINFO:
	case TBL_NODEINFO:
	case TBL_PHYSNODEINFO:
	case TBL_VERSION:
#ifdef NX
	case TBL_IPD_MSG_INFO:
#endif /* NX */
#ifdef SLL
	case TBL_FASTNODE:
#endif /* SLL */
		/*
		 * Operations done only on the current node.
		 */
		bufferlen = nel * lel;
		return(PVPSOP_TABLE_GET(this_node,
				        id, index, 
				        &addr, &bufferlen, 
					nel, lel,
				        retval));

	case TBL_MSGDS:
	case TBL_SEMDS:
	case TBL_SHMDS:
	case TBL_MSGINFO:
	case TBL_SEMINFO:
	case TBL_SHMINFO:
		/*
		 * Operations done only on the System V IPC server node.
		 */
		bufferlen = nel * lel;
		return(PVPSOP_TABLE_GET(svipc_node,
				        id, index, 
				        &addr, &bufferlen, 
					nel, lel,
				        retval));


	case TBL_PGINFO:
		/*
		 * Operations done only on the root_fs_node
		 */
		bufferlen = nel * lel;
		return(PVPSOP_TABLE_GET(root_fs_node,
				        id, index, 
				        &addr, &bufferlen, 
					nel, lel,
				        retval));

#ifdef	     TBL_RTHASH_HOST
	case TBL_RTHASH_HOST:
	case TBL_RTHASH_NET:
#endif
	case TBL_RTREE_HEAD:
	case TBL_RTREE_NODE:
	case TBL_RTREE_NODE_DEBUG:

	{
		caddr_t	next_addr;
		node_t	network_server_nodes[MAX_SERVERS];
		mach_port_t network_server_ports[MAX_SERVERS];
		int	node_count = MAX_SERVERS;
		int	total_count;
		int	next_nel,nel_so_far;

		/*
		 * Operations performed on a "preferred" network
		 * server node.  This gets us the information we
		 * need and distributes the processing needed to
		 * do it.
		 */
		if ( HAS_SUBTABLES(id) ) {
			stid  = SUBTBL_STID(index);
			index = SUBTBL_SIDX(index);
		} else
			stid = 0;

		*retval = 0;

		error = find_network_servers(node_count,
					     &total_count,
					     &node_count,
					     network_server_nodes,
					     network_server_ports);

		if (error != KERN_SUCCESS) {
			return(error);
		}
		if (node_count == 0) {
			return(ESUCCESS);
		}

		ASSERT(total_count >= node_count);

		total_count = 0;
		next_addr = addr;
		nel_so_far = 0;
		for( i = 0; i < node_count; i++) {
			error = PVPSOP_TABLE_SIZE(network_server_nodes[i],
						  id,
						  SUBTBL_IDX(stid,0),
						  &cnt);
			if (error != ESUCCESS) {
				return(error);
			}

			/* If only getting counts, shortcut the actual GET */
			if (lel == 0 && nel >= SHRT_MAX) {
				total_count += cnt;
				continue;
			}

			next_nel = nel - nel_so_far;
			next_addr = addr + (nel_so_far * lel);
			bufferlen = next_nel * lel;
			next_index = (HAS_SUBTABLES(id)
				      ? SUBTBL_IDX(stid,index)
				      : index);

			error = PVPSOP_TABLE_GET(network_server_nodes[i],
						 id,
						 next_index,
						 &next_addr,
						 &bufferlen,
						 next_nel,
						 lel,
						 &cnt);
			if (error != ESUCCESS) {
				return(error);
			}

			nel_so_far += cnt;
			total_count += cnt;

		}

		*retval = total_count;

		break;

	}

#ifdef NX
	case TBL_HIPPI_ART:
#endif /* NX */
		/*
		 * Operations performed on a "preferred" network
		 * server node.  This gets us the information we
		 * need and distributes the processing needed to
		 * do it.
		 */
		if ( HAS_SUBTABLES(id) ) {
			stid  = SUBTBL_STID(index);
			index = SUBTBL_SIDX(index);
		} else
			stid = 0;

		*retval = 0;

		/* Locate the clearinghouse before query */
		error = find_clearinghouse();
		if(error != ESUCCESS) {
			printf("dvpsop_table: find_clearinghouse: error %d\n",\
				error);
			return(error);
		}
		
		error = if_get_nearest_server(clearinghouse_port,
				AF_INET,
				this_node,
				&nserver,				      
				&retport);

		/* return if error */
		if (error != KERN_SUCCESS) 
			return(error);

		/* there are no network servers, so return */
		if (nserver == INVALID_NODE)
			return(ESUCCESS);			

		bufferlen = 0;
		error = PVPSOP_TABLE_SIZE(nserver,
					  id,
					  SUBTBL_IDX(stid,0),
					  &cnt);
		/*
		 * If we can't get the size, quit
		 */
		if (error != ESUCCESS)
			return(error);

		/* 
		 * If we just want the count, set it and 
		 * return
		 */
		if (lel == 0 && nel >= SHRT_MAX) {
			*retval = cnt;
			break;

		}

		/*
		 * If we get here then we have a non-zero lel
		 * and we have the correct starting node picked 
		 * out. So we actually start obtaining data 
		 * from the nodes.
		 */
		bufferlen = nel * lel;
		next_index = (HAS_SUBTABLES(id)
			      ? SUBTBL_IDX(stid,index)
			      : index);
		error = PVPSOP_TABLE_GET(nserver,
		    		         id,
					 next_index,
		    		         &addr, &bufferlen, 
					 nel, lel,
				         &cnt);
		if (error != ESUCCESS)
			return(error);
		*retval = cnt;

		break;

	case TBL_IF:
	case TBL_ARPTAB:
		/*
		 * Operations done on each network server of the
		 * system, emulating a single large system-wide
		 * table.
		 */
		if( HAS_SUBTABLES(id) ) {
			stid  = SUBTBL_STID(index);
			index = SUBTBL_SIDX(index);
		} else
			stid = 0;

		*retval = 0;

		/*
		 * First get a list of the network server nodes
		 */
		error = find_network_servers(MAX_SERVERS, &numns,
					     &rel, nodes, ports);

		/*
		 * If we didn't get a list, there's nothing that
		 * we can do except return.
		 */
		if(error != ESUCCESS)
			return(error);

		/*
		 * If numns == 0, there are no network servers
		 * configured.  If there are no network servers
		 * configured, there are no network interfaces,
		 * no routes, etc.
		 */
		if(numns == 0) {
			return(ESUCCESS);
		}

		/* repeat for each node */
		for(i = 0, np = nodes; i < numns; i++, np++) {

			bufferlen = 0;
			error = PVPSOP_TABLE_SIZE(*np, 
						  id,
						  SUBTBL_IDX(stid,0),
						  &cnt);
			/*
			 * If this last call failed, forget 
			 * about node *np and keep going.
			 */
			if (error != ESUCCESS)
				continue;
			if (lel == 0 && nel >= SHRT_MAX) {
				*retval += cnt;
				continue;
			}
			if (cnt <= index) {
				index -= cnt;
				continue;
			}

			/*
			 * If we get here then we have a non-zero lel
			 * and we have the correct starting node picked 
			 * out. So we actually start obtaining data 
			 * from the nodes.
			 */
			bufferlen = nel * lel;
			next_index = (HAS_SUBTABLES(id)
				      ? SUBTBL_IDX(stid,index)
				      : index);
			error = PVPSOP_TABLE_GET(*np,
			    		         id,
						 next_index,
			    		         &addr, &bufferlen, 
						 nel, lel,
					         &cnt);
			/*
			 * If this last call failed, forget 
			 * about node *np and keep going.
			 */
			if (error != ESUCCESS)
				continue;
			*retval += cnt;
			if (cnt == nel)
				break;
			addr += cnt * lel;
			nel -= cnt;
			index = 0;
		}

		break;

	case TBL_PROCINFO:
	case TBL_TCPCB:		/* XXX not sure about this one yet */
	case TBL_UDPCB:		/* XXX ditto */
	case TBL_UNPCB:
		/*
		 * Operations done on all nodes of the system, emulating
		 * a single large system-wide table.
		 */
		if ( HAS_SUBTABLES(id) ) {
			stid  = SUBTBL_STID(index);
			index = SUBTBL_SIDX(index);
		} else
			stid = 0;
		*retval = 0;
		for (i = 0; i < node_array_entries; i++) {
			for (n = node_array[i][0]; n <= node_array[i][1]; n++) {
				bufferlen = 0;
				error = PVPSOP_TABLE_SIZE(n, 
							  id,
							  SUBTBL_IDX(stid,0),
							  &cnt);
				if (error != ESUCCESS)
					return(error);
				if (lel == 0 && nel >= SHRT_MAX) {
					*retval += cnt;
					continue;
				}
				if (cnt <= index) {
					index -= cnt;
					continue;
				}

				/*
				 * If we get here then we have a non-zero lel
				 * and we have the correct starting node picked 
				 * out. So we actually start obtaining data 
				 * from the nodes.
				 */
				bufferlen = nel * lel;
				next_index = (HAS_SUBTABLES(id)
					      ? SUBTBL_IDX(stid,index)
					      : index);
				error = PVPSOP_TABLE_GET(n,
				    		         id,
							 next_index,
				    		         &addr, &bufferlen, 
							 nel, lel,
						         &cnt);
				if (error != ESUCCESS)
					return(error);
				*retval += cnt;
				if (cnt == nel)
					goto outloop;
				addr += cnt * lel;
				nel -= cnt;
				index = 0;
			}
		}
	outloop:
		break;

	case TBL_INTR:
	case TBL_LOADAVG:
	case TBL_SYSINFO:
	case TBL_TTYINFO:
	case TBL_RTSTAT:
	case TBL_MBSTAT:
	case TBL_IPSTAT:
	case TBL_ICMPSTAT:
	case TBL_TCPSTAT:
	case TBL_UDPSTAT:
	case TBL_NFSSTAT:
	case TBL_ARPPARAM:
		/*
		 * Operations done on all nodes of the system, with
		 * a single "summed up" entry returned.
		 */
		if (nel != 1)
			return(EINVAL);

		/* Calculate the "real" entry length */
		switch(id) {
		case TBL_INTR:
			bufferlen = sizeof(ti);
			break;
		case TBL_LOADAVG:
			bufferlen = sizeof(tl);
			break;
		case TBL_SYSINFO:
			bufferlen = sizeof(ts);
			break;
		case TBL_TTYINFO:
			bufferlen = sizeof(tt);
			break;
		case TBL_RTSTAT:
			bufferlen = sizeof(tr);
			break;
		case TBL_MBSTAT:
			bufferlen = sizeof(tm);
			break;
		case TBL_IPSTAT:
			bufferlen = sizeof(tip);
			break;
		case TBL_ICMPSTAT:
			bufferlen = sizeof(tic);
			break;
		case TBL_TCPSTAT:
			bufferlen = sizeof(tcp);
			break;
		case TBL_UDPSTAT:
			bufferlen = sizeof(tu);
			break;
		case TBL_NFSSTAT:
			bufferlen = sizeof(tn);
			break;
		case TBL_ARPPARAM:
			bufferlen = sizeof(ta);
			break;
		}

		/* Obtain the summed up total entry */
		addr_tbl_data = &tbl_data;
		first = TRUE;
		cnt = 0;
		for (i = 0; i < node_array_entries; i++) {
			for (n = node_array[i][0]; n <= node_array[i][1]; n++) {
				cnt ++;
				error = PVPSOP_TABLE_GET(n,
				    		         id, index, 
							 (char_array *)
								&addr_tbl_data, 
						         &bufferlen, 
							 1, bufferlen,
						         retval);
				if (error != ESUCCESS)
					return(error);

				/* for TBL_LOADAVG, translate to lscale == 0 */
				if (id == TBL_LOADAVG && tl.tl_lscale != 0) {
					/*
					 * backwards copy to avoid
					 * clobbering the .l array
					 */
					for (j = 2; j >= 0; j--) {
						tl.tl_avenrun.d[j] =
							(double) tl.tl_avenrun.l[j] /
							(double) tl.tl_lscale;
					}
					tl.tl_lscale = 0;
				}

				/* Only need to copy on first time round */
				if (first) {
					bcopy(&tbl_data, 
					      &total_tbl_data, 
					      bufferlen);
					first = FALSE;
					continue;
				}

				/* Do the appropriate totalling */
#undef				INCR
				switch (id) {

				case TBL_INTR:
#define					INCR(a) tti.a += ti.a
					INCR(in_devintr);
                        		INCR(in_context);
					INCR(in_syscalls);
                        		INCR(in_forks);
#undef					INCR
					break;

				case TBL_LOADAVG:
#define					INCR(a) ttl.a += tl.a
					for (j = 0; j <= 2; j++) {
						INCR(tl_avenrun.d[j]);
						INCR(tl_mach_factor[j]);
					}
#undef					INCR
					break; 

				case TBL_SYSINFO:
#define					INCR(a) tts.a += ts.a
                        		INCR(si_user);
                        		INCR(si_nice);
                        		INCR(si_sys);
                        		INCR(si_idle);
					if (n == root_fs_node) {
						/* one only; choose root node */
                        			tts.si_hz = ts.si_hz;
                        			tts.si_phz = ts.si_phz;
						tts.si_boottime = 
							ts.si_boottime;
					}
#undef					INCR
                        		break;

				case TBL_TTYINFO:
#define					INCR(a) ttt.a += tt.a
                        		INCR(ti_nin);
                        		INCR(ti_nout);
                        		INCR(ti_cancc);
                        		INCR(ti_rawcc);
#undef					INCR
					break;

				case TBL_RTSTAT:
#define					INCR(a) ttr.a += tr.a
					INCR(rts_badredirect);
					INCR(rts_dynamic);
					INCR(rts_newgateway);
					INCR(rts_unreach);
					INCR(rts_wildcard);
#undef					INCR
					break;

				case TBL_MBSTAT:
#define					INCR(a) ttm.a += tm.a
					INCR(m_mbufs);
					INCR(m_clusters);
					INCR(m_mfree);
					INCR(m_clfree);
					INCR(m_drops);
					INCR(m_wait);
					INCR(m_drain);
					for (j = 0; j < MT_MAX; j++)
						INCR(m_mtypes[j]);
#undef					INCR
					break;

				case TBL_IPSTAT:
#define					INCR(a) ttip.a += tip.a
					INCR(ips_total);
					INCR(ips_badsum);
					INCR(ips_tooshort);
					INCR(ips_toosmall);
					INCR(ips_badhlen);
					INCR(ips_badlen);
					INCR(ips_fragments);
					INCR(ips_fragdropped);
					INCR(ips_fragtimeout);
					INCR(ips_forward);
					INCR(ips_cantforward);
					INCR(ips_redirectsent);
					INCR(ips_noproto);
					INCR(ips_delivered);
					INCR(ips_localout);
					INCR(ips_odropped);
					INCR(ips_reassembled);
					INCR(ips_fragmented);
					INCR(ips_ofragments);
					INCR(ips_cantfrag);
#undef					INCR
					break;

				case TBL_ICMPSTAT:
#define					INCR(a) ttic.a += tic.a
					INCR(icps_error);
					INCR(icps_oldshort);
					INCR(icps_oldicmp);
					for (j = 0; j < ICMP_MAXTYPE+1; j++)
						INCR(icps_outhist[j]);
 					INCR(icps_badcode);
					INCR(icps_tooshort);
					INCR(icps_checksum);
					INCR(icps_badlen);
					INCR(icps_reflect);
					for (j = 0; j < ICMP_MAXTYPE+1; j++)
						INCR(icps_inhist[j]);
#undef					INCR
					break;

				case TBL_TCPSTAT:
#define					INCR(a) ttcp.a += tcp.a
					INCR(tcps_connattempt);
					INCR(tcps_accepts);
					INCR(tcps_connects);
					INCR(tcps_drops);
					INCR(tcps_conndrops);
					INCR(tcps_closed);
					INCR(tcps_segstimed);
					INCR(tcps_rttupdated);
					INCR(tcps_delack);
					INCR(tcps_timeoutdrop);
					INCR(tcps_rexmttimeo);
					INCR(tcps_persisttimeo);
					INCR(tcps_keeptimeo);
					INCR(tcps_keepprobe);
					INCR(tcps_keepdrops);
					INCR(tcps_sndtotal);
					INCR(tcps_sndpack);
					INCR(tcps_sndbyte);
					INCR(tcps_sndrexmitpack);
					INCR(tcps_sndrexmitbyte);
					INCR(tcps_sndacks);
					INCR(tcps_sndprobe);
					INCR(tcps_sndurg);
					INCR(tcps_sndwinup);
					INCR(tcps_sndctrl);
					INCR(tcps_rcvtotal);
					INCR(tcps_rcvpack);
					INCR(tcps_rcvbyte);
					INCR(tcps_rcvbadsum);
					INCR(tcps_rcvbadoff);
					INCR(tcps_rcvshort);
					INCR(tcps_rcvduppack);
					INCR(tcps_rcvdupbyte);
					INCR(tcps_rcvpartduppack);
					INCR(tcps_rcvpartdupbyte);
					INCR(tcps_rcvoopack);
					INCR(tcps_rcvoobyte);
					INCR(tcps_rcvpackafterwin);
					INCR(tcps_rcvbyteafterwin);
					INCR(tcps_rcvafterclose);
					INCR(tcps_rcvwinprobe);
					INCR(tcps_rcvdupack);
					INCR(tcps_rcvacktoomuch);
					INCR(tcps_rcvackpack);
					INCR(tcps_rcvackbyte);
					INCR(tcps_rcvwinupd);
#undef					INCR
					break;

				case TBL_UDPSTAT:
#define					INCR(a) ttu.a += tu.a
					INCR(udps_ipackets);
					INCR(udps_hdrops);
					INCR(udps_badsum);
					INCR(udps_badlen);
					INCR(udps_noport);
					INCR(udps_noportbcast);
					INCR(udps_fullsock);
					INCR(udpps_pcbcachemiss);
					INCR(udps_opackets);
#undef					INCR
					break;

				case TBL_NFSSTAT:
#define					INCR(a) ttn.a += tn.a
					INCR(attrcache_hits);
					INCR(attrcache_misses);
					INCR(lookupcache_hits);
					INCR(lookupcache_misses);
					INCR(direofcache_hits);
					INCR(direofcache_misses);
					INCR(biocache_reads);
					INCR(read_bios);
					INCR(read_physios);
					INCR(biocache_writes);
					INCR(write_bios);
					INCR(write_physios);
					INCR(biocache_readlinks);
					INCR(readlink_bios);
					INCR(biocache_readdirs);
					INCR(readdir_bios);
					for (j = 0; j < NFS_NPROCS; j++)
						INCR(rpccnt[j]);
					INCR(rpcretries);
					for (j = 0; j < NFS_NPROCS; j++)
						INCR(srvrpccnt[j]);
					INCR(srvrpc_errs);
					INCR(srv_errs);
					INCR(rpcrequests);
					INCR(rpctimeouts);
					INCR(rpcunexpected);
					INCR(rpcinvalid);
					INCR(srvcache_inproghits);
					INCR(srvcache_idemdonehits);
					INCR(srvcache_nonidemdonehits);
					INCR(srvcache_misses);
					INCR(srvcache_reqdrops);
#undef					INCR
					break;

				case TBL_ARPPARAM:
#define					INCR(a) tta.a += ta.a
					INCR(arptab_size);
					INCR(arptab_nb);
#undef					INCR
					break;
				} /* end switch(id) */
			} /* inner for() loop */
		} /* outer for() loop() */

		/* For TBL_LOADAVG, we actually must average the totals */
		if (id == TBL_LOADAVG) {
			for (j = 0; j <= 2; j++) {
				ttl.tl_avenrun.d[j] /= (double) cnt;
				ttl.tl_mach_factor[j] /= cnt;
			}
		}

		/* Copy the data back to the user's buffer */
		copyout(&total_tbl_data, addr, min(bufferlen, lel));
		break;

#ifdef NX
	case TBL_PROCINFO_PART:
		/*
		 * Operations run in the partition
		 */
                *retval = 0;

		/* 
		 * Set id to TBL_PROCINFO since we now know it
		 * is restricted to a partition.
		 */
		id = TBL_PROCINFO;

		if ( HAS_SUBTABLES(id) ) {
			stid  = SUBTBL_STID(index);
			index = SUBTBL_SIDX(index);
		} else
			stid = 0;

                for (i = 0; i < nodelistcnt; i++) {
		    extern int vnode_pager_is_set;

				if(!is_tnc_node_valid(nodelist[i]))
					continue;

		                /* 
				 * If this test fails than assume that
				 * we are in single user mode and have
				 * not run bootmesh yet.  Do not try to
				 * do an RPC to any other nodes.
				 */
				if (this_node == root_fs_node &&
				    nodelist[i] != root_fs_node && 
				    !vnode_pager_is_set)
				    continue;

                                bufferlen = 0;
                                error = PVPSOP_TABLE_SIZE(nodelist[i],
                                                          id,
							  SUBTBL_IDX(stid,0),
                                                          &cnt);
                                if (error != ESUCCESS) {
					vm_deallocate(mach_task_self(),
							(vm_address_t)nodelist,
							(vm_size_t)(nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                                        return(error);
				}
                                if (lel == 0 && nel >= SHRT_MAX) {
                                        *retval += cnt;
                                        continue;
                                }
                                if (cnt <= index) {
                                        index -= cnt;
                                        continue;
                                }

                                /*
                                 * If we get here then we have a non-zero lel
                                 * and we have the correct starting node picked
                                 * out. So we actually start obtaining data
                                 * from the nodes.
                                 */
                                bufferlen = nel * lel;
                                error = PVPSOP_TABLE_GET(nodelist[i],
                                                         id, index,
                                                         &addr, &bufferlen,
                                                         nel, lel,
                                                         &cnt);
                                if (error != ESUCCESS) {
					vm_deallocate(mach_task_self(),
							(vm_address_t)nodelist,
							(vm_size_t)(nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                                        return(error);
				}
                                *retval += cnt;
                                if (cnt == nel)
                                        goto outloop2;
                                addr += cnt * lel;
                                nel -= cnt;
                                index = 0;
               }

        outloop2:
	       if (nodelist != (LP_MAP_T)NULL)
		   vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
			    (vm_size_t)(nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                break;
#endif /* NX */

	default:
		/*
		 * Operations we know nothing about.
		 */
		return(EINVAL);
	}

	/*
	 * We succesfully performed the operation.
	 */
	return(ESUCCESS);
}

/*
 *	This function takes the node_array table, and expands it into
 *	an array of nodes in ascending order, with no holes in it. For
 *	instance, if node_array had two entries consisting of [1,3] and
 *	[7,10], this routine would produce a table 7 entries long with
 *	entries: [1,2,3,7,8,9,10]. This can then be used with the
 *	spanning_tree routine, which generates indexes into this sort
 *	of table.
 */
node_t *
build_expanded_node_array(total_entries)
register int *total_entries;
{
	register int 	i;
	register node_t	*expanded_node_array;
	register int	expanded_array_count;
	register int	expanded_array_check_count;
	register int	node;
	register node_t	*np;
	register node_t	*enp;

	extern node_t	node_array[][2];
	extern int	node_array_entries;

try_again:

	/*
	 * Since node_array is a compressed (run-length encoded) list, we
	 * need to calculate how long the expanded list will be so that we
	 * can allocate enough memory to hold it. Entries are of the form
	 * n,m where (n) is the number of the first node in the extent,
	 * and (m) is the number of the final node in the extent. Thus the
	 * number of nodes in any given extent is m-n+1...
	 */
	expanded_array_count = 0;
	for (i = 0, np = &node_array[0][0]; i<node_array_entries; i++,np+=2) {
		expanded_array_count += np[1] - np[0] + 1;
	}

	*total_entries = expanded_array_count;

	/*
	 * Allocate a buffer to hold the expanded array of nodes. If we
	 * can't allocate a big enough buffer, we end up returning a null,
	 * and the calling code probably ends up using the slow
	 * algorithm...
	 */
	expanded_node_array = (node_t *)
			kalloc(expanded_array_count * sizeof(node_t));

	if (expanded_node_array == NULL) {
		return(NULL);
	}

	/*
	 * Fill in the expanded array by walking the extent array, and for
	 * each one generating all the node numbers contained in the
	 * extent, entering them in successive array elements. The check
	 * count is to insure we don't run off the end of our allocated
	 * array if the size of the node_array changes while we're doing
	 * this...
	 */
	np = &node_array[0][0];
	enp = expanded_node_array;
	expanded_array_check_count = 0;
	for (i = 0; i < node_array_entries; i++, np+=2) {
		for (node = np[0]; node <= np[1]; node++) {
			if (expanded_array_check_count == expanded_array_count) {
				kfree(expanded_node_array, 
				      expanded_array_count * sizeof(node_t));
				goto try_again;
			}
			*enp++ = node;
			expanded_array_check_count += 1;
		}
	}

	*total_entries = expanded_array_check_count;
	return(expanded_node_array);
}

/*
 * Virtual process system operation for VPSOP_SETHOSTID()
 */
int
dvpsop_sethostid(
	long	new_hostid)
{
	int		error;
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	node_t		*expanded_node_array;
	int		expanded_array_count = 0;

	expanded_node_array = build_expanded_node_array(&expanded_array_count);

	/*
	 * If we have a very small system, it may be more efficient to
	 * just rpc each node directly, rather than go through the trouble
	 * of using the spanning tree. However, if it's more than a very
	 * few nodes, we go ahead and use the spanning tree here:
	 */
	if (expanded_node_array != NULL && 
	    expanded_array_count > SPANNING_TREE_BENEFITS) {

		/*
		 * Broadcast to every node the new host id. Note that this
		 * runs on the node in expanded_node_array[0]. This does
		 * two things. If all nodes in the system have this as the
		 * first node (which they should), then this serializes
		 * setting host id by always going through this node.
		 * Also, it takes care of the fact that the
		 * get_spanning_tree code never returns an offset (index)
		 * of zero, since when we first call it assumes we're
		 * already on that (root) node. That's a bad assumption,
		 * but we make it true right here by forcing
		 * sethostid_multi to run on that node...
		 */
		error = PVPSOP_SETHOSTID_MULTI(
					(node_t)expanded_node_array[0],
					new_hostid,
					(long_node_array_t)expanded_node_array,
					(int)expanded_array_count,
					(int)0,
					(msg_handle_t *)NULL);

		/*
		 * Free any memory we allocated.
		 */
		kfree(expanded_node_array, 
		      expanded_array_count * sizeof(node_t));

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if (error == EAGAIN) {
			goto slow_way;
		}
		return(error);
	}

	/*
	 * Go through the list signalling each member.
	 */
slow_way:

	/*
	 * Go through all the nodes setting hostid on every node,
	 * blithely ignoring errors.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
			(void) PVPSOP_SETHOSTID(node, new_hostid);
		}
	}

	return(ESUCCESS);
}

/*
 *	This routine uses a spanning tree to rpc each node in the system
 *	in order to transmit the new host id. This function is a
 *	distributed recursive function, meaning that it calls itself on
 *	other nodes. The "expanded_array_index" parameter keeps track of
 *	where we are, so that the spanning tree algorithm "knows" where
 *	it is in the recursion.
 *
 *	Note that VP_STACK_ARRAY_SIZE determines the largest number of
 *	entries the spanning tree algorithm can return on any given node,
 *	and that at the time of writing this is defined to be around 20,
 *	which means this breaks if we have more than approximately 2^20
 *	nodes, which is way beyond the current architectural limit for
 *	the system.
 */
int
dpvpsop_sethostid_multi(
	node_t node,
	long new_hostid,
	node_t *expanded_node_array,
	u_int expanded_array_count,
	int expanded_array_index,
	msg_handle_t *h)
{
	int error;
	int nsuccesses = 0;
	register int i;
	int target_node;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];

	/*
	 * Get a list of nodes we should RPC. Note that this is
	 * misleading. The current spanning tree algorithm does not know
	 * which node is which. It just generates index values. Given that
	 * we pass in the total number of entries in our table, the
	 * spanning tree algorithm guarantees that it will generate each
	 * index eventually (if not to us, to one of the nodes we RPC).
	 * When we take these indexes and use them to index into our table
	 * of nodes, we convert index to node number. Just remember that
	 * the spanning tree routine knows nothing about our expanded node
	 * table. The algorithm would produce exactly the same results if
	 * we were dealing with an array which represented any arbitrary
	 * data you can conceive of. The algorithm will currently generate
	 * exactly the same values whether we have an array of 16 pids all
	 * living on the same node, or an array of 16 node numbers. All it
	 * really cares about is the total # of entries, and our current
	 * position in the tree...
	 */
	error = get_spanning_tree(expanded_array_count,
				  expanded_array_index,
				  entry_array,
				  &entry_array_count);
	if (error != ESUCCESS) {
		return(error);
	}

	/*
	 * For each entry that get_spanning_tree returned, RPC the node.
	 * The spanning tree routine just returns an index, and we then
	 * use that index to address our node table, giving us a target
	 * node number. Note that we don't wait for the RPC to complete -
	 * that is done later. It's necessary to do this asynchronously in
	 * order to get the nodes to all RPC in parallel, rather than
	 * serial.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		error_array[i] = PVPSOP_SETHOSTID_MULTI_SEND(target_node,
		                                        new_hostid,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);
	}

	/*
	 * Here is where we set the host id on the local node.
	 */
	error = PVPSOP_SETHOSTID(this_node, new_hostid);
	if (error == 0) {
		nsuccesses += 1;
	}

	/*
	 * Wait for all the RPCs to complete. We check the error_array so
	 * that we don't try to wait for an RPC which failed at send time.
	 * We also keep track of the number of successes so that we know
	 * if there were any nodes which didn't get notified.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		if (error_array[i] == 0) {
			error = PVPSOP_SETHOSTID_MULTI_RECEIVE(target_node,
							new_hostid,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);

			if (error == 0) {
				nsuccesses += 1;
			}
		}

		else {

			error = error_array[i];

		}

	}

	return(nsuccesses == (entry_array_count + 1) ? ESUCCESS : error);

}

/*
 * Virtual process system operation for VPSOP_SETHOSTNAME()
 */
int
dvpsop_sethostname(
	hostname_t	new_hostname,
	unsigned int	new_hostnamelen)
{
	int		error;
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	node_t		*expanded_node_array;
	int		expanded_array_count = 0;
#ifdef NX
	extern int      debug_sethostname;
#endif /* NX */

	expanded_node_array = build_expanded_node_array(&expanded_array_count);

	/*
	 * If we have a very small system, it may be more efficient to
	 * just rpc each node directly, rather than go through the trouble
	 * of using the spanning tree. However, if it's more than a very
	 * few nodes, we go ahead and use the spanning tree here:
	 */
	if (expanded_node_array != NULL && 
	    expanded_array_count > SPANNING_TREE_BENEFITS) {

		/*
		 * Broadcast to every node the new host name. Note that
		 * this runs on the node in expanded_node_array[0]. This
		 * does two things. If all nodes in the system have this
		 * as the first node (which they should), then this
		 * serializes setting host name by always going through
		 * this node. Also, it takes care of the fact that the
		 * get_spanning_tree code never returns an offset (index)
		 * of zero, since when we first call it assumes we're
		 * already on that (root) node. That's a bad assumption,
		 * but we make it true right here by forcing
		 * sethostname_multi to run on that node...
		 */
		error = PVPSOP_SETHOSTNAME_MULTI(
					(node_t)expanded_node_array[0],
					new_hostname,
					new_hostnamelen,
					(long_node_array_t)expanded_node_array,
					(int)expanded_array_count,
					(int)0,
					(msg_handle_t *)NULL);

		/*
		 * Free any memory we allocated.
		 */
		kfree(expanded_node_array, 
		      expanded_array_count * sizeof(node_t));

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if (error == EAGAIN) {
			goto slow_way;
		}
		return(error);
	}

	/*
	 * Go through the list signalling each member.
	 */
slow_way:

	/*
	 * Go through all the nodes setting hostname on every node,
	 * blithely ignoring errors.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
#ifdef NX
		        if (debug_sethostname) printf("%d\n", node);
#endif /* NX */
			(void) PVPSOP_SETHOSTNAME(node,
						  new_hostname,
						  new_hostnamelen);
		}
	}

	return(ESUCCESS);
}

/*
 *	This routine uses a spanning tree to rpc each node in the system
 *	in order to transmit the new host name. This function is a
 *	distributed recursive function, meaning that it calls itself on
 *	other nodes. The "expanded_array_index" parameter keeps track of
 *	where we are, so that the spanning tree algorithm "knows" where
 *	it is in the recursion.
 *
 *	Note that VP_STACK_ARRAY_SIZE determines the largest number of
 *	entries the spanning tree algorithm can return on any given node,
 *	and that at the time of writing this is defined to be around 20,
 *	which means this breaks if we have more than approximately 2^20
 *	nodes, which is way beyond the current architectural limit for
 *	the system.
 */
int
dpvpsop_sethostname_multi(
	node_t node,
	hostname_t	new_hostname,
	u_int		new_hostnamelen,
	node_t *expanded_node_array,
	u_int expanded_array_count,
	int expanded_array_index,
	msg_handle_t *h)
{
	int error;
	int nsuccesses = 0;
	register int i;
	int target_node;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];

	/*
	 * Get a list of nodes we should RPC. Note that this is
	 * misleading. The current spanning tree algorithm does not know
	 * which node is which. It just generates index values. Given that
	 * we pass in the total number of entries in our table, the
	 * spanning tree algorithm guarantees that it will generate each
	 * index eventually (if not to us, to one of the nodes we RPC).
	 * When we take these indexes and use them to index into our table
	 * of nodes, we convert index to node number. Just remember that
	 * the spanning tree routine knows nothing about our expanded node
	 * table. The algorithm would produce exactly the same results if
	 * we were dealing with an array which represented any arbitrary
	 * data you can conceive of. The algorithm will currently generate
	 * exactly the same values whether we have an array of 16 pids all
	 * living on the same node, or an array of 16 node numbers. All it
	 * really cares about is the total # of entries, and our current
	 * position in the tree...
	 */
	error = get_spanning_tree(expanded_array_count,
				  expanded_array_index,
				  entry_array,
				  &entry_array_count);
	if (error != ESUCCESS) {
		return(error);
	}

	/*
	 * For each entry that get_spanning_tree returned, RPC the node.
	 * The spanning tree routine just returns an index, and we then
	 * use that index to address our node table, giving us a target
	 * node number. Note that we don't wait for the RPC to complete -
	 * that is done later. It's necessary to do this asynchronously in
	 * order to get the nodes to all RPC in parallel, rather than
	 * serial.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		error_array[i] = PVPSOP_SETHOSTNAME_MULTI_SEND(
							target_node,
							new_hostname,
							new_hostnamelen,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);
	}

	/*
	 * Here is where we set the hostname on the local node.
	 */
	error = PVPSOP_SETHOSTNAME(this_node,
				   new_hostname,
				   new_hostnamelen);
	if (error == 0) {
		nsuccesses += 1;
	}

	/*
	 * Wait for all the RPCs to complete. We check the error_array so
	 * that we don't try to wait for an RPC which failed at send time.
	 * We also keep track of the number of successes so that we know
	 * if there were any nodes which didn't get notified.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		if (error_array[i] == 0) {
			error = PVPSOP_SETHOSTNAME_MULTI_RECEIVE(
							target_node,
							new_hostname,
							new_hostnamelen,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);

			if (error == 0) {
				nsuccesses += 1;
			}
		}

		else {

			error = error_array[i];

		}

	}

	return(nsuccesses == (entry_array_count + 1) ? ESUCCESS : error);

}

/*
 * Virtual process system operation for VPSOP_SETDOMAINNAME()
 */
int
dvpsop_setdomainname(
	domainname_t	new_domainname,
	unsigned int	new_domainnamelen)
{
	int		error;
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	node_t		*expanded_node_array;
	int		expanded_array_count = 0;

	expanded_node_array = build_expanded_node_array(&expanded_array_count);

	/*
	 * If we have a very small system, it may be more efficient to
	 * just rpc each node directly, rather than go through the trouble
	 * of using the spanning tree. However, if it's more than a very
	 * few nodes, we go ahead and use the spanning tree here:
	 */
	if (expanded_node_array != NULL && 
	    expanded_array_count > SPANNING_TREE_BENEFITS) {

		/*
		 * Broadcast to every node the new domainname. Note that
		 * this runs on the node in expanded_node_array[0]. This
		 * does two things. If all nodes in the system have this
		 * as the first node (which they should), then this
		 * serializes setting domain name by always going through
		 * this node. Also, it takes care of the fact that the
		 * get_spanning_tree code never returns an offset (index)
		 * of zero, since when we first call it assumes we're
		 * already on that (root) node. That's a bad assumption,
		 * but we make it true right here by forcing
		 * setdomainname_multi to run on that node...
		 */
		error = PVPSOP_SETDOMAINNAME_MULTI(
					(node_t)expanded_node_array[0],
					new_domainname,
					new_domainnamelen,
					(long_node_array_t)expanded_node_array,
					(int)expanded_array_count,
					(int)0,
					(msg_handle_t *)NULL);

		/*
		 * Free any memory we allocated.
		 */
		kfree(expanded_node_array, 
		      expanded_array_count * sizeof(node_t));

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if (error == EAGAIN) {
			goto slow_way;
		}
		return(error);
	}

	/*
	 * Go through the list signalling each member.
	 */
slow_way:

	/*
	 * Go through all the nodes setting domain name on every node,
	 * blithely ignoring errors.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
			(void) PVPSOP_SETDOMAINNAME(node,
						    new_domainname,
						    new_domainnamelen);
		}
	}

	return(ESUCCESS);
}

/*
 *	This routine uses a spanning tree to rpc each node in the system
 *	in order to transmit the new domain name. This function is a
 *	distributed recursive function, meaning that it calls itself on
 *	other nodes. The "expanded_array_index" parameter keeps track of
 *	where we are, so that the spanning tree algorithm "knows" where
 *	it is in the recursion.
 *
 *	Note that VP_STACK_ARRAY_SIZE determines the largest number of
 *	entries the spanning tree algorithm can return on any given node,
 *	and that at the time of writing this is defined to be around 20,
 *	which means this breaks if we have more than approximately 2^20
 *	nodes, which is way beyond the current architectural limit for
 *	the system.
 */
int
dpvpsop_setdomainname_multi(
	node_t node,
	domainname_t	new_domainname,
	u_int		new_domainnamelen,
	node_t *expanded_node_array,
	u_int expanded_array_count,
	int expanded_array_index,
	msg_handle_t *h)
{
	int error;
	int nsuccesses = 0;
	register int i;
	int target_node;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];

	/*
	 * Get a list of nodes we should RPC. Note that this is
	 * misleading. The current spanning tree algorithm does not know
	 * which node is which. It just generates index values. Given that
	 * we pass in the total number of entries in our table, the
	 * spanning tree algorithm guarantees that it will generate each
	 * index eventually (if not to us, to one of the nodes we RPC).
	 * When we take these indexes and use them to index into our table
	 * of nodes, we convert index to node number. Just remember that
	 * the spanning tree routine knows nothing about our expanded node
	 * table. The algorithm would produce exactly the same results if
	 * we were dealing with an array which represented any arbitrary
	 * data you can conceive of. The algorithm will currently generate
	 * exactly the same values whether we have an array of 16 pids all
	 * living on the same node, or an array of 16 node numbers. All it
	 * really cares about is the total # of entries, and our current
	 * position in the tree...
	 */
	error = get_spanning_tree(expanded_array_count,
				  expanded_array_index,
				  entry_array,
				  &entry_array_count);
	if (error != ESUCCESS) {
		return(error);
	}

	/*
	 * For each entry that get_spanning_tree returned, RPC the node.
	 * The spanning tree routine just returns an index, and we then
	 * use that index to address our node table, giving us a target
	 * node number. Note that we don't wait for the RPC to complete -
	 * that is done later. It's necessary to do this asynchronously in
	 * order to get the nodes to all RPC in parallel, rather than
	 * serial.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		error_array[i] = PVPSOP_SETDOMAINNAME_MULTI_SEND(
							target_node,
							new_domainname,
							new_domainnamelen,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);
	}

	/*
	 * Here is where we set the domain name on the local node.
	 */
	error = PVPSOP_SETDOMAINNAME(this_node,
				     new_domainname,
				     new_domainnamelen);
	if (error == 0) {
		nsuccesses += 1;
	}

	/*
	 * Wait for all the RPCs to complete. We check the error_array so
	 * that we don't try to wait for an RPC which failed at send time.
	 * We also keep track of the number of successes so that we know
	 * if there were any nodes which didn't get notified.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		if (error_array[i] == 0) {
			error = PVPSOP_SETDOMAINNAME_MULTI_RECEIVE(
							target_node,
							new_domainname,
							new_domainnamelen,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);

			if (error == 0) {
				nsuccesses += 1;
			}
		}

		else {

			error = error_array[i];

		}

	}

	return(nsuccesses == (entry_array_count + 1) ? ESUCCESS : error);

}

#ifdef i860
/*
 * Virtual process system operation for VPSOP_RPMOFFSET(). The RPM is
 * hardware specific, thus the conditional for i860 only compilation
 */
int
dvpsop_rpmoffset(
	unsigned int	rpmlow,
	unsigned int	rpmhigh)
{
	int		error;
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	node_t		*expanded_node_array;
	int		expanded_array_count = 0;

	expanded_node_array = build_expanded_node_array(&expanded_array_count);

	/*
	 * If we have a very small system, it may be more efficient to
	 * just rpc each node directly, rather than go through the trouble
	 * of using the spanning tree. However, if it's more than a very
	 * few nodes, we go ahead and use the spanning tree here:
	 */
	if (expanded_node_array != NULL && 
	    expanded_array_count > SPANNING_TREE_BENEFITS) {

		/*
		 * Broadcast to every node the new rpm offset. Note that
		 * this runs on the node in expanded_node_array[0]. This
		 * does two things. If all nodes in the system have this
		 * as the first node (which they should), then this
		 * serializes setting TOD offset by always going through
		 * this node. Also, it takes care of the fact that the
		 * get_spanning_tree code never returns an offset (index)
		 * of zero, since when we first call it assumes we're
		 * already on that (root) node. That's a bad assumption,
		 * but we make it true right here by forcing
		 * rpmoffset_multi to run on that node...
		 */
		error = PVPSOP_RPMOFFSET_MULTI(
					(node_t)expanded_node_array[0],
					(u_int)rpmlow,
					(u_int)rpmhigh,
					(long_node_array_t)expanded_node_array,
					(int)expanded_array_count,
					(int)0,
					(msg_handle_t *)NULL);

		/*
		 * Free any memory we allocated.
		 */
		kfree(expanded_node_array, 
		      expanded_array_count * sizeof(node_t));

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if (error == EAGAIN) {
			goto slow_way;
		}
		return(error);
	}

	/*
	 * Go through the list signalling each member.
	 */
slow_way:

	/*
	 * Go through all the nodes doing a rpmoffset on every node,
	 * blithely ignoring errors.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
			(void) PVPSOP_RPMOFFSET(node,
						rpmlow,
						rpmhigh);
		}
	}

	return(ESUCCESS);
}

/*
 *	This routine uses a spanning tree to rpc each node in the system
 *	in order to transmit the new TOD offset. This function is a
 *	distributed recursive function, meaning that it calls itself on
 *	other nodes. The "expanded_array_index" parameter keeps track of
 *	where we are, so that the spanning tree algorithm "knows" where
 *	it is in the recursion.
 *
 *	Note that VP_STACK_ARRAY_SIZE determines the largest number of
 *	entries the spanning tree algorithm can return on any given node,
 *	and that at the time of writing this is defined to be around 20,
 *	which means this breaks if we have more than approximately 2^20
 *	nodes, which is way beyond the current architectural limit for
 *	the system.
 */
int
dpvpsop_rpmoffset_multi(
	node_t node,
	u_int rpmlow,
	u_int rpm_high,
	node_t *expanded_node_array,
	u_int expanded_array_count,
	int expanded_array_index,
	msg_handle_t *h)
{
	int error;
	int nsuccesses = 0;
	register int i;
	int target_node;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];

	/*
	 * Get a list of nodes we should RPC. Note that this is
	 * misleading. The current spanning tree algorithm does not know
	 * which node is which. It just generates index values. Given that
	 * we pass in the total number of entries in our table, the
	 * spanning tree algorithm guarantees that it will generate each
	 * index eventually (if not to us, to one of the nodes we RPC).
	 * When we take these indexes and use them to index into our table
	 * of nodes, we convert index to node number. Just remember that
	 * the spanning tree routine knows nothing about our expanded node
	 * table. The algorithm would produce exactly the same results if
	 * we were dealing with an array which represented any arbitrary
	 * data you can conceive of. The algorithm will currently generate
	 * exactly the same values whether we have an array of 16 pids all
	 * living on the same node, or an array of 16 node numbers. All it
	 * really cares about is the total # of entries, and our current
	 * position in the tree...
	 */
	error = get_spanning_tree(expanded_array_count,
				  expanded_array_index,
				  entry_array,
				  &entry_array_count);
	if (error != ESUCCESS) {
		return(error);
	}

	/*
	 * For each entry that get_spanning_tree returned, RPC the node.
	 * The spanning tree routine just returns an index, and we then
	 * use that index to address our node table, giving us a target
	 * node number. Note that we don't wait for the RPC to complete -
	 * that is done later. It's necessary to do this asynchronously in
	 * order to get the nodes to all RPC in parallel, rather than
	 * serial.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		error_array[i] = PVPSOP_RPMOFFSET_MULTI_SEND(
							target_node,
							rpmlow,
							rpm_high,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);
	}

	/*
	 * Here is where we set the offset on the local node.
	 */
	error = PVPSOP_RPMOFFSET(this_node,
				 rpmlow,
				 rpm_high);
	if (error == 0) {
		nsuccesses += 1;
	}

	/*
	 * Wait for all the RPCs to complete. We check the error_array so
	 * that we don't try to wait for an RPC which failed at send time.
	 * We also keep track of the number of successes so that we know
	 * if there were any nodes which didn't get notified.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		if (error_array[i] == 0) {
			error = PVPSOP_RPMOFFSET_MULTI_RECEIVE(
							target_node,
							rpmlow,
							rpm_high,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);

			if (error == 0) {
				nsuccesses += 1;
			}
		}

		else {

			error = error_array[i];

		}

	}

	return(nsuccesses == (entry_array_count + 1) ? ESUCCESS : error);

}

#endif

/*
 * Virtual process system operation for VPSOP_SETTIMEZONE()
 */
int
dvpsop_settimezone(
	struct timezone new_timezone)
{
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	node_t		*expanded_node_array;
	int		expanded_array_count = 0;
	int		error;

	expanded_node_array = build_expanded_node_array(&expanded_array_count);

	/*
	 * If we have a very small system, it may be more efficient to
	 * just rpc each node directly, rather than go through the trouble
	 * of using the spanning tree. However, if it's more than a very
	 * few nodes, we go ahead and use the spanning tree here:
	 */
	if (expanded_array_count > SPANNING_TREE_BENEFITS) {

		/*
		 * Broadcast to every node the new time zone. Note that
		 * this runs on the node in expanded_node_array[0]. This
		 * does two things. If all nodes in the system have this
		 * as the first node (which they should), then this
		 * serializes setting timezone by always going through
		 * this node. Also, it takes care of the fact that the
		 * get_spanning_tree code never returns an offset (index)
		 * of zero, since when we first call it assumes we're
		 * already on that (root) node. That's a bad assumption,
		 * but we make it true right here by forcing _MULTI to run
		 * on that node...
		 */
		error = PVPSOP_SETTIMEZONE_MULTI(
					(node_t)expanded_node_array[0],
					new_timezone,
					(long_node_array_t)expanded_node_array,
					(int)expanded_array_count,
					(int)0,
					(msg_handle_t *)NULL);

		/*
		 * Free any memory we allocated.
		 */
		kfree(expanded_node_array, 
		      expanded_array_count * sizeof(node_t));

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if (error == EAGAIN) {
			goto slow_way;
		}
		return(error);
	}

	/*
	 * Go through the list signalling each member.
	 */
slow_way:
	/*
	 * Go through all the nodes doing a settimezone on every node,
	 * blithely ignoring errors.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
			(void) PVPSOP_SETTIMEZONE(node, new_timezone);
		}
	}

	return(ESUCCESS);
}

/*
 *	This routine uses a spanning tree to rpc each node in the system
 *	in order to transmit the new timezone. This function is a
 *	distributed recursive function, meaning that it calls itself on
 *	other nodes. The "expanded_array_index" parameter keeps track of
 *	where we are, so that the spanning tree algorithm "knows" where
 *	it is in the recursion.
 *
 *	Note that VP_STACK_ARRAY_SIZE determines the largest number of
 *	entries the spanning tree algorithm can return on any given node,
 *	and that at the time of writing this is defined to be around 20,
 *	which means this breaks if we have more than approximately 2^20
 *	nodes, which is way beyond the current architectural limit for
 *	the system.
 */
int
dpvpsop_settimezone_multi(
	node_t node,
	struct timezone new_timezone,
	node_t *expanded_node_array,
	u_int expanded_array_count,
	int expanded_array_index,
	msg_handle_t *h)
{
	int error;
	int nsuccesses = 0;
	register int i;
	int target_node;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];

	/*
	 * Get a list of nodes we should RPC. Note that this is
	 * misleading. The current spanning tree algorithm does not know
	 * which node is which. It just generates index values. Given that
	 * we pass in the total number of entries in our table, the
	 * spanning tree algorithm guarantees that it will generate each
	 * index eventually (if not to us, to one of the nodes we RPC).
	 * When we take these indexes and use them to index into our table
	 * of nodes, we convert index to node number. Just remember that
	 * the spanning tree routine knows nothing about our expanded node
	 * table. The algorithm would produce exactly the same results if
	 * we were dealing with an array which represented any arbitrary
	 * data you can conceive of. The algorithm will currently generate
	 * exactly the same values whether we have an array of 16 pids all
	 * living on the same node, or an array of 16 node numbers. All it
	 * really cares about is the total # of entries, and our current
	 * position in the tree...
	 */
	error = get_spanning_tree(expanded_array_count,
				  expanded_array_index,
				  entry_array,
				  &entry_array_count);
	if (error != ESUCCESS) {
		return(error);
	}

	/*
	 * For each entry that get_spanning_tree returned, RPC the node.
	 * The spanning tree routine just returns an index, and we then
	 * use that index to address our node table, giving us a target
	 * node number. Note that we don't wait for the RPC to complete -
	 * that is done later. It's necessary to do this asynchronously in
	 * order to get the nodes to all RPC in parallel, rather than
	 * serial.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		error_array[i] = PVPSOP_SETTIMEZONE_MULTI_SEND(
							target_node,
							new_timezone,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);
	}

	/*
	 * Here is where we set the offset on the local node.
	 */
	error = PVPSOP_SETTIMEZONE(this_node, new_timezone);
	if (error == 0) {
		nsuccesses += 1;
	}

	/*
	 * Wait for all the RPCs to complete. We check the error_array so
	 * that we don't try to wait for an RPC which failed at send time.
	 * We also keep track of the number of successes so that we know
	 * if there were any nodes which didn't get notified.
	 */
	for (i = 0; i < entry_array_count; i++) {
		target_node = expanded_node_array[entry_array[i]];
		if (error_array[i] == 0) {
			error = PVPSOP_SETTIMEZONE_MULTI_RECEIVE(
							target_node,
							new_timezone,
							expanded_node_array,
							expanded_array_count,
							entry_array[i],
							&handle_array[i]);

			if (error == 0) {
				nsuccesses += 1;
			}
		}

		else {

			error = error_array[i];

		}

	}

	return(nsuccesses == (entry_array_count + 1) ? ESUCCESS : error);

}


/*
 * Virtual process system operation for VPSOP_REBOOT()
 */
int
dvpsop_reboot(
	int	panic_flag,
	int	options)
{
	/*
	 * Redirect to the root fs node.
	 */
	(void) PVPSOP_REBOOT(root_fs_node, panic_flag, options);
}

#ifdef NX
/*
 * Virtual process system operation for VPSOP_RESET_BOOT_NODE_LIST()
 */
int
dvpsop_reset_boot_node_list(
	char	       *nodelist,
	unsigned int	nodelistlen,
	boolean_t       local_node_only)
{
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
	char 		*nodelist_copy;



	/*
	 * If local_node_only, then just do it for this node.
	 */
	if (local_node_only && nodelistlen > 0) {
	    (void) PVPSOP_RESET_BOOT_NODE_LIST(this_node,
					       nodelist,
					       nodelistlen);
	    return(ESUCCESS);
        }


	if (nodelistlen > 0) {
		/*
		 * The !@#$%^&* boot magic parser trashes the input string,
		 * so we must make a copy before calling it on the local
		 * node.
		 */
		if ( vm_allocate(mach_task_self(), 
					(vm_address_t *)&nodelist_copy, 
					(vm_size_t)nodelistlen,
					TRUE) != KERN_SUCCESS ) {
			return(EINVAL);
		}
		bcopy(nodelist, nodelist_copy, nodelistlen);

		(void) PVPSOP_RESET_BOOT_NODE_LIST(this_node,
					       nodelist_copy,
					       nodelistlen);

		
		vm_deallocate(mach_task_self(), (vm_address_t)nodelist_copy, 
				(vm_size_t)nodelistlen);
	}

	/*
	 * Go through all the nodes 
	 * blithely ignoring errors.
	 */
	for (i = 0; i < node_array_entries; i++) {
		for (node = node_array[i][0]; node <= node_array[i][1]; node++){
		    if (node != this_node)
			(void) PVPSOP_RESET_BOOT_NODE_LIST(node,
						  nodelist,
						  nodelistlen);
		}
	}

	return(ESUCCESS);
}
#endif /* NX */

