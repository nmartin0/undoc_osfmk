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
 * $Log: dvp_pvpsops.c,v $
 * Revision 1.23  1995/02/01  21:42:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.22  1994/11/18  20:43:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.21  1994/08/31  22:47:07  mtm
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
 * Revision 1.19.2.1  1994/08/03  14:06:02  chrisp
 * Remove changes for PTS-9688, tncgen skip facility.
 * Modified Files: dvp_pvpsops.c gen_mig.sh pvps.ops
 *
 * Revision 1.19  1994/07/27  18:43:14  chrisp
 * Implement a skip facility in tncgen. Without SLL (for LITE server),
 * table_set is skipped.
 *
 *  Reviewer: yazz@locus.com
 *  Risk: M
 *  Benefit or PTS #: 9688
 *  Testing: Built and run for STD and LITE servers
 *  Module(s): dvp_pvpsops.c gen_mig.sh pvps.ops
 *
 * Revision 1.18  1994/05/13  21:37:42  cfj
 * Remove the #ifdef SLL around the table_get() RPC.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17  1994/04/28  19:14:02  chrisp
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
 * Revision 1.16  1994/04/04  17:35:33  paul
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
 * Revision 1.15  1994/03/14  02:04:58  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.14  1993/12/03  20:07:55  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Added the dpvpsop_settimezone function which calls pps_settimezone.
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #: :Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/tnc/dvp_pvpsops.c
 *
 * Revision 1.13  1993/10/29  11:55:03  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.12  1993/10/21  23:34:12  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.11  1993/07/14  18:32:41  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  20:44:31  cfj
 * Adding new code from vendor
 *
 * Revision 1.10  1993/06/22  11:35:47  stefan
 * Commented out #ifdef SLL to allow builing with -sll.
 * Should be reverted as soon as LOCUS has fixed their build scripts.
 *
 * Revision 1.9  1993/06/08  12:50:45  stefan
 * Reintroduced PVPSOP_TABLE_SET for static load leveling.
 *
 * Revision 1.8  1993/05/20  16:03:18  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.7  1993/05/06  19:22:04  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:44:58  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:08:31  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/25  23:26:06  cfj
 * T9 Merge.
 *
 * Revision 1.4.6.1  1993/03/24  23:42:09  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 * 
 * Revision 1.1.2.3.2.1  1992/12/16  06:01:56  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  03:01:31  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:47:10  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/13  18:52:46  cfj
 * Added reset_boot_node_list() system call.
 *
 * Revision 1.1.2.2  1992/11/06  20:31:02  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:45:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.8  93/09/16  08:47:13  chrisp
 * [SPE 0030] Generic Spanning Trees: dvps_ops_gen.h now dpvps_protos_gen.h.
 * 
 * Revision 3.7  93/05/14  12:11:13  slk
 * Remove dpvpsop_table_set.
 * 
 * Revision 3.6  93/02/22  17:17:08  mjl
 * Pass index arg to dpvpsop_table_size(), it may contain subtable number.
 *
 * Revision 3.5  92/12/01  11:20:40  chrisp
 * Add local private virtual process system operations for VPSOP_REBOOT()
 * 	and VSPOP_BOOT(). REBOOT() redirects the reboot() system call to
 * 	the root fs node and BOOT() asynchronously halts a node.
 * 
 * Revision 3.4  92/11/02  11:37:27  roman
 * Get rid of type buffer_t. It's really a char_array.
 * 
 * Revision 3.3  92/10/28  15:10:47  roman
 * Include procedure prototypes generated my makeTNCtables.h to ensure better
 * 	type-checking.
 * Add new pvpsops for table_get(), table_set(), table_size(),
 * 	setdomainname(), sethostname(), and sethostid().
 * 
 * Revision 3.2  92/10/01  10:28:17  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.1  92/09/30  15:48:49  chrisp
 * Correct parameter order in call to pps_sigprocset()
 * 	from dpvpsop_sigprocset().
 * 
 * Revision 3.0  92/09/28  16:07:04  roman
 * Initial submission of file with TNC version of private virtual
 * process system operations.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/tty.h>
#include <machine/machlimits.h>
#include <sys/reboot.h>

/* Prototypes generated from dpvproc.h */
#include <tnc/dpvps_protos_gen.h>

/*
 * Local private virtual process system operation for PVPSOP_SIGPROCSET()
 */
int
dpvpsop_sigprocset(
	node_t		node,
	struct procset	*ps,
	int		signo,
	int		arg,
	uid_t		uid,
	uid_t		ruid,
	pid_t		pid,
	pid_t		sid,
	int		*nproc,
	int		flags)
{
	return(pps_sigprocset(ps, signo, arg,
			      (flags & VPROC_HAS_PRIV) != 0,
			      uid, ruid, pid, sid, nproc));
}


/*
 * Local private virtual process system operation for PVPSOP_PROCSET_NICE()
 */
int
dpvpsop_procset_nice(
	node_t		node,
	struct procset	*ps,
	int		*nice,
	uid_t		euid,
	uid_t		ruid,
	int		flag)
{
	if (flag & VPROC_GET)
		return(pps_procset_get_nice(ps, nice));
	else
		return(pps_procset_set_nice(ps, 
					    euid, ruid, 
					    flag & VPROC_HAS_PRIV,
					    *nice));
}


/*
 * Local private virtual process system operation for PVPSOP_PID_FROM_TASK()
 */
int
dpvpsop_pid_from_task(
	node_t		node,
	task_t		task,
	pid_t		*pid,
	char		*comm,
	unsigned int	*commlen)
{
	return(pps_pid_from_task(task, pid, comm, commlen));
}


/*
 * Local private virtual process system operation for PVPSOP_TABLE()
 */
int
dpvpsop_table_size(
	node_t		node,
	int		id,
	int		index,
	int		*retval)
{
	struct args {
		int	id;
		int	index;
		caddr_t	addr;
		int	nel;
		u_int	lel;
	} ua;

	ua.id = id;
	ua.index = index;
	ua.addr = NULL;
	ua.nel = SHRT_MAX;
	ua.lel = 0;
	return(pps_table(&ua, retval));
}


/*
 * Local private virtual process system operation for PVPSOP_TABLE_GET()
 */
int
dpvpsop_table_get(
	node_t		node,
	int		id,
	int		index,
	char_array	*buffer,
	u_int		*bufferlen,
	int		nel,
	u_int		lel,
	int		*retval)
{
	struct args {
		int	id;
		int	index;
		caddr_t	addr;
		int	nel;
		u_int	lel;
	} ua;

	ua.id = id;
	ua.index = index;
	ua.addr = *buffer;
	ua.nel = nel;
	ua.lel = lel;
	return(pps_table(&ua, retval));
}


/*
 * Local private virtual process system operation for PVPSOP_SETHOSTID()
 */
int
dpvpsop_sethostid(
	node_t	node,
	long	new_hostid)
{
	return(pps_sethostid(new_hostid));
}


/*
 * Local private virtual process system operation for PVPSOP_SETHOSTNAME()
 */
dpvpsop_sethostname(
	node_t		node,
	hostname_t	new_hostname,
	unsigned int	new_hostnamelen)
{
	return(pps_sethostname(new_hostname, new_hostnamelen));
}


/*
 * Local private virtual process system operation for PVPSOP_SETDOMAINNAME()
 */
dpvpsop_setdomainname(
	node_t		node,
	hostname_t	new_domainname,
	unsigned int	new_domainnamelen)
{
	return(pps_setdomainname(new_domainname, new_domainnamelen));
}


#ifdef ipsc860
/*
 * Local private virtual process system operation for PVPSOP_RPMOFFSET()
 */
dpvpsop_rpmoffset(
	node_t		node,
	unsigned int	rpmlow,
	unsigned int	rpmhigh)
{
	void pps_rpmoffset();
	union {
		unsigned int longwise[2];
		double		value;
	} rpmval;


	rpmval.longwise[0] = rpmlow;
	rpmval.longwise[1] = rpmhigh;

	pps_rpmoffset(rpmval.value);

	return(ESUCCESS);
}
#endif /* ipsc860 */

/*
 * Local private virtual process system operation for PVPSOP_SETTIMEZONE()
 */
int
dpvpsop_settimezone(
	node_t	node,
	struct timezone new_timezone)
{
	return(pps_settimezone(new_timezone));
}


/*

/*
 * Local private virtual process system operation for PVPSOP_REMOTE_VPROC_FREE()
 */
int
dpvpsop_remote_vproc_free(
	node_t		node,
	struct vproc	*vp)
{
	VPROC_RELEASE(vp, "dpvpsop_remote_vproc_free");
	return(ESUCCESS);
}


/*
 * Local private virtual process system operation for PVPSOP_CTTY_GETATTR()
 */
int
dpvpsop_ctty_getattr(
	node_t		node,
	struct vproc	*vs,
	pid_t		*t_pgid,
	dev_t		*dev)
{
	struct tty *tp;

	ASSERT(PVP(vs)->pvp_flag & PV_CTTY_NODE);

	tp = PVP(vs)->pvp_cttyp;
	if (dev)
		*dev = tp ? tp->t_dev : 0;
	if (t_pgid)
		*t_pgid = tp ? tp->t_pgid : 0;

	return(ESUCCESS);
}


/*
 * Local private virtual process system operation for PVPSOP_RELEASE_CTTY()
 */
int
dpvpsop_release_ctty(
	node_t		node,
	struct vproc	*vs,
	pid_t		*t_pgid)
{
	struct pvproc	*pvs = PVP(vs);

	VPROC_LOCK_EXCL(vs, "dpvpsop_release_ctty");
	ASSERT(pvs->pvp_flag&PV_CTTY_NODE);
	*t_pgid = pvs->pvp_cttyp->t_pgid;
	pvs->pvp_cttyp->t_sid = 0;
	pproc_release_cttyv(pvs->pvp_cttyp);
	pvs->pvp_cttyp = NULL;
	VPROC_LOCK_FLAG(vs, "dpvpsop_release_ctty");
	pvs->pvp_flag &= ~PV_CTTY_NODE;
	VPROC_UNLOCK_FLAG(vs, "dpvpsop_release_ctty");
	VPROC_UNLOCK_EXCL(vs, "dpvpsop_release_ctty");
	VPROC_RELEASE(vs, "dpvpsop_release_ctty(controlling session)");
	return(ESUCCESS);
}


/*
 * Local private virtual process system operation for PVPSOP_GET_VPROC_PORT()
 */
int
dpvpsop_get_vproc_port(
	node_t		node,
	struct vproc	*vp,
	mach_port_t	*vproc_port)
{
	*vproc_port = tnc_bestow_vproc_port(vp, 0);
	return(ESUCCESS);
}

/*
 * Local private virtual process system operation for VPSOP_REBOOT()
 */
int
dpvpsop_reboot(
	node_t	local_node,
	int	panic_flag,
	int	options)
{
	node_t		node;
	int		i;
	extern node_t	node_array[][2];
	extern int	node_array_entries;

	ASSERT(local_node == root_fs_node);
	pps_bootsync(panic_flag, options);

	/*
	 * For a normal shutdown (when we're not panicking)
	 * go through all nodes other than the local node requesting boot.
	 */
	if (panic_flag != RB_PANIC)
		for (i = 0; i < node_array_entries; i++) {
			for (node = node_array[i][0];
			     node <= node_array[i][1];
			     node++){
				if (node == this_node)
					continue;
				(void) PVPSOP_BOOT(node, RB_BOOT, options);
			}
		}

	/*
	 * Finally, commit suicide.
	 */
	return(pps_boot(panic_flag, options));

}

/*
 * Local private virtual process system operation for VPSOP_BOOT()
 */
int
dpvpsop_boot(
	node_t	node,
	int	panic_flag,
	int	options)
{
	return(pps_boot(panic_flag, options));
}


/*
 * Local private virtual process system operation for
 *	PVPSOP_REMOTE_VPROC_ALLOCATE()
 */
int
dpvpsop_remote_vproc_allocate(
	node_t	node,
	pid_t	pid)
{
	struct vproc	*vp;
	struct vproc	*nvp = NULL, *w;
	int		hashidx;
	int		error = ESUCCESS;
	extern struct vproc *vproc_alloc();

	/*
	 * First determine if the vproc for this pid exists.
	 */
	ASSERT(pid != (pid_t)-1);
	VPROC_LIST_LOCK();
restart:
	for (vp = vproc_hash[VPROCPIDHASH(pid)]; vp != 0; vp = vp->vp_hashfwd)
		if (vp->vp_pid == pid) 
			break;

	/*
	 * If the vproc doesn't exist, we attempt to create it.
	 */
	if (vp == NULL) {
		if (nvp == NULL) {
			VPROC_LIST_UNLOCK();
			if ((nvp = vproc_alloc()) == NULL) {
				printf("remote_vproc_allocate(): "
				       "vproc_alloc() failure\n");
				return(ENOMEM);
			}
			VPROC_LIST_LOCK();
		}

		/*
		 * Make sure here that during the last vproc_alloc()
		 * call, nobody has created a new vproc for this pid.
		 */
		for (vp = vproc_hash[VPROCPIDHASH(pid)]; 
				vp != 0; 
				vp = vp->vp_hashfwd)
			if (vp->vp_pid == pid) 
				break;
		if (vp == NULL) {
			vp = nvp;
			nvp = NULL;
			dpvproc_struct_init(vp);

			/*
			 * Put the vproc on the vproc hash chain.
			 */
			vp->vp_pid = pid;
			w = vproc_hash[hashidx = VPROCPIDHASH(pid)];
			vp->vp_hashbwd = NULL;
			vp->vp_hashfwd = w;
			vproc_hash[hashidx] = vp;
			if (w)
				w->vp_hashbwd = vp;

			/*
			 * Since this vproc is in an intermediate state
			 * we'll set the operation table pointers to NULL.
			 */
			vp->vp_ops = NULL;
			PVP(vp)->pvp_ops = NULL;
		}
	}

	/*
	 * Make a final check that we haven't latched onto a vproc that
	 * is in the process of being freed (special pid -1). We cannot
	 * reliably spot the pid of -1 until the refcnt lock is locked.
	 * If we find a vproc in the process of being freed, then we restart
	 * the search from scratch. Otherwise, increment the reference
	 * count and we're done.
	 */
	VPROC_REFCNT_LOCK(vp);
	if (vp->vp_pid == (pid_t)-1) {
		ASSERT(vp->vp_ref_cnt == 0);
		VPROC_REFCNT_UNLOCK(vp);
		goto restart;
	}

	/*
	 * Check that this vproc neither already exists nor is reserved.
	 * If so, reserve it and bump its reference count.
	 */
	VPROC_LOCK_FLAG(vp, "dpvpsop_remote_vproc_allocate");
	if (PVP(vp)->pvp_flag & PV_IS_ORIGIN) {
		error = EEXIST;
	} else {
		PVP(vp)->pvp_flag |= PV_IS_ORIGIN;
		vp->vp_ref_cnt++;
	}
	VPROC_UNLOCK_FLAG(vp, "dpvpsop_remote_vproc_allocate");
	VPROC_REFCNT_UNLOCK(vp);
	VPROC_LIST_UNLOCK();

	/*
	 * If we ended up allocating a new vproc but not using it,
	 * it's now safe to de_allocate it (now that the lists are unlocked).
	 */
	if (nvp != NULL)
		vproc_dealloc(nvp);

	return(error);
}

#ifdef NX

/*
 * Local private virtual process system operation 
 * for PVPSOP_RESET_BOOT_NODE_LIST()
 */
dpvpsop_reset_boot_node_list(
	node_t		node,
        char_array      nodelist,
        uint_t          nodelistlen)
{
	return(pps_reset_boot_node_list(nodelist, nodelistlen));
}

/*
 * Local private virtual process system operation for PVPSOP_TABLE_SET()
 */
int
dpvpsop_table_set(
	node_t		node,
	int		id,
	int		index,
	char_array	buffer,
	u_int		bufferlen,
	int		nel,
	u_int		lel,
	int		*retval)
{
	struct args {
		int	id;
		int	index;
		caddr_t	addr;
		int	nel;
		u_int	lel;
	} ua;

	ua.id = id;
	ua.index = index;
	ua.addr = buffer;
	ua.nel = nel;
	ua.lel = lel;
	return(pps_table(&ua, retval));
}
#endif /* NX */
