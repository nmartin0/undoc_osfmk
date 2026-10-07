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
 * $Log: tnc_server_side.c,v $
 * Revision 1.17  1995/04/08  00:01:47  yazz
 *  Authors of fix: Ray Shapouri & Bob Yasi
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12845
 *  Testing: EATs controlc, sched, os_interfaces, MUNOPS SAT runs
 *  Module(s): server/tnc/tnc_server_side.c
 * Fixed a VM leak in table() code, where deallocated length was smaller
 * than allocated length, thereby leaking the difference.
 *
 * Revision 1.16  1995/03/27  15:24:33  johannes
 * bsd_table_node_get: use local copy address for going through the allocated
 * 		    buffer of table entries, return original address and
 * 		    use original address for deallocating the buffer
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 12801
 *  Testing: testing table_node_get(.. TBL_UAREA ..)
 *  Module(s): server/tnc/tnc_server_side.c
 *
 * Revision 1.15  1995/02/01  21:56:22  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.14  1994/11/18  20:44:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/06/02  22:30:12  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.12  1994/05/09  04:03:51  yazz
 * Merge R1.2 revision 1.10.4.1 into main stem.
 *
 * Revision 1.11  1994/03/14  02:07:05  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.10.4.1  1994/04/27  22:08:14  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/tnc/tnc_server_side.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.10  1993/07/14  18:35:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:48:47  cfj
 * Adding new code from vendor
 *
 * Revision 3.7  93/06/11  12:12:56  slively
 * Comment fix for SVR398 installation.
 * 
 * Revision 3.6  93/06/02  12:29:47  yazz
 * For Sys V IPC under TNC add an RPC so a process can obtain an svipc port.
 * 
 *
 * Revision 1.9  1993/06/08  12:56:03  stefan
 * Reintroduced PVPSOP_TABLE_SET for static load leveling.
 *
 * Revision 1.8  1993/05/20  16:03:37  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.7  1993/05/06  19:26:29  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:47:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:44  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/03  14:02:19  stefan
 * Now executing END_SERVER in bsd_table_node_[set|get] in case of error exit.
 * Also the node number translation in bsd_get_tnc-port() has been moved to
 * get_tnc-port() (file: tnc_ipc.c).
 *
 * Revision 1.2.2.1.2.2  1993/02/16  20:06:31  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/26  18:15:14  cfj
 * Force the interrupt to FALSE before calling end_vprocserver_op().
 *
 * Revision 1.2.2.1.2.1  1992/12/14  23:22:05  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.3  1992/11/30  22:48:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.2.2.3  1992/11/27  16:57:19  stefan
 * In bsd_get_tnc_port() the logical node number is now mapped to the
 * appropriate physical node number.
 *
 * Revision 1.2.2.2  1992/11/17  20:22:40  cfj
 * Made our sources match Roman's for the crash in rforkmulti().
 * In table_node() map the node number from logical to physical.
 *
 * Revision 1.2.2.1  1992/11/10  01:17:36  cfj
 * Put into NX tree.
 *
 * Revision 1.2  1992/11/06  23:44:52  cfj
 * Nov. 5, 1992 Locus bug fix merge.
 *
 * Revision 1.1  1992/11/05  22:47:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.5  93/05/14  12:17:47  slively
 * [Bug 208] Add appropriate permissions checking for table_node() and
 * table() system calls.
 * 
 * Revision 3.4  1993/04/30  16:38:18  yazz
 * Change tnc to use the new _NOSIG version of the END_VPROCSERVER
 * macro (part of ad1.0.3 merge).
 *
 * Revision 3.3  93/01/26  14:39:55  yazz
 * Added bug number to previous RCS comment.
 * 
 * Revision 3.2  93/01/26  12:22:52  yazz
 * For bug #0156:
 * Temporary strategy to prevent end_vprocserver_op()'s signal processing
 * after rforkmulti() and rfork() calls.  
 * 
 * Revision 3.1  92/11/02  11:24:09  roman
 * Renamed tnc_server.c to new file name tnc_server_side.c.
 * Added interface for version of rforkmulti() using out-of-line data.
 * 
 * Revision 3.0  92/10/28  14:39:38  roman
 * First checkin of file with TNC-specific system calls.
 * 
 */


#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/bsd_types.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/table.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/syscalltrace.h>
#include <uxkern/bsd_msg.h>
#include <tnc/dpvproc.h>
#ifdef  CHKPNT
#include <tnc/chkpnt.h>
#endif  CHKPNT

#ifdef SLL
#include <sll/sll_types.h>
#endif /* SLL */


/*
 * System call handling for TNC system calls.
 */
bsd_rfork(
	mach_port_t		vproc_port, 
	int			*interrupt,
	node_t			ch_node,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name,
	task_t			*ch_taskp,		/* OUT */
	thread_t		*ch_threadp,		/* OUT */
	pid_t			*ch_pidp)		/* OUT */
{
	START_VPROCSERVER(SYS_rfork, 1)

	error = rfork(vproc_port, ch_node,
		      ch_state, ch_state_count,
		      vproc_port_name, cred_port_name,
		      ch_taskp, ch_threadp, ch_pidp);

	END_VPROCSERVER_NOSIG(SYS_rfork)
}

bsd_migrate(
	mach_port_t		cur_vproc_port, 
	int			*interrupt,
	node_t			new_node,
	thread_state_t		cur_state,
	unsigned int		cur_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name,
	mach_port_t		old_task_name)
{
	int			error;
	struct proc		*p;
	int			serial = !sysent[SYS_migrate].sy_parallel;

	error = start_vprocserver_op(cur_vproc_port, SYS_migrate);
	if (error)
		return(error);
	if (NCPUS == 1 || serial)
		unix_master();
	
	p = u.u_procp;

	error = migrate(cur_vproc_port, new_node,
			cur_state, cur_state_count,
			vproc_port_name, cred_port_name, old_task_name);

	(void) end_vprocserver_op(cur_vproc_port, error, interrupt);
	if (NCPUS == 1 || serial)
		unix_release();
	if (error)
		return(error);

	/*
	 * We don't return to the emulator.
	 */
	return(MIG_NO_REPLY);
}


bsd_rexecve(
	mach_port_t		cur_vproc_port, 
	int			*interrupt,
	char			*fname,
	unsigned int		fname_count,
	node_t			new_node,
	thread_state_t		cur_state,
	unsigned int		cur_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name,
	mach_port_t		old_task_name,
	vm_offset_t		arg_addr,
	vm_size_t		arg_size,
	int			arg_count,
	int			env_count,
	unsigned int		char_count)
{
	int			error;
	struct proc *		p;
	int			serial = !sysent[SYS_rexecve].sy_parallel;

	error = start_vprocserver_op(cur_vproc_port, SYS_rexecve);
	if (error)
		return(error);
	if (NCPUS == 1 || serial)
		unix_master();
	
	p = u.u_procp;

	error = rexecve(cur_vproc_port, 
			fname, fname_count, 
			new_node,
			cur_state, cur_state_count, 
			vproc_port_name, cred_port_name, old_task_name,
			arg_addr, arg_size,
			arg_count, env_count, char_count);

	(void) end_vprocserver_op(cur_vproc_port, error, interrupt);
	if (NCPUS == 1 || serial)
		unix_release();
	if (error)
		return(error);

	/*
	 * We don't return to the emulator.
	 */
	return(MIG_NO_REPLY);
}


bsd_rexecve_arrival(
	mach_port_t		proc_port, 
	int			*interrupt,
	char 			*fname,			/* OUT */
	int			*fname_count,		/* OUT */
	vm_address_t		*arg_addr,		/* OUT */
	vm_size_t		*arg_size,		/* OUT */
	int			*arg_count,		/* OUT */
	int			*env_count,		/* OUT */
	unsigned int		*char_count,		/* OUT */
	char			*cfname,		/* OUT */
	char			*cfarg,			/* OUT */
	int			*entry,			/* OUT */
	unsigned int		*entry_count,		/* OUT */
	auxv_type_t		*auxv_structs,		/* OUT */
	unsigned int		*auxv_structs_count,	/* OUT */
	auxv_strings_t		auxv_strings,		/* OUT */
	unsigned int		*auxv_strings_count,	/* OUT */
	boolean_t		*traced)		/* OUT */
{
	START_SERVER(SYS_rexecve, 1)
	
	error = rexecve_arrival(procp,
				fname, fname_count,
				arg_addr, arg_size,
				arg_count, env_count, char_count,
				cfname, cfarg,
				entry, entry_count,
				auxv_structs, auxv_structs_count,
				auxv_strings, auxv_strings_count);

	*traced = (procp->p_flag & STRC);

	END_SERVER(SYS_rexecve)
}

bsd_rforkmulti(
	mach_port_t		vproc_port, 
	int			*interrupt,
	node_t			node_array[],
	unsigned int		node_count,
	int			rval_array[],		/* OUT */
	unsigned int		*rval_count,
	pid_t			ch_pid_array[],		/* OUT */
	unsigned int		*ch_pid_count,
	mach_port_t		fp_name_array[],
	unsigned int		nfp_names,
	mach_port_t		fp_right_array[],
	unsigned int		nfp_rights,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name)
{
	START_VPROCSERVER(SYS_rforkmulti, 4)

	error = rforkmulti(vproc_port,
			   FALSE,
			   node_array, node_count,
			   rval_array, rval_count,
			   ch_pid_array, ch_pid_count,
			   fp_name_array, nfp_names,
			   fp_right_array, nfp_rights,
			   ch_state, ch_state_count,
			   vproc_port_name, cred_port_name);

	END_VPROCSERVER_NOSIG(SYS_rforkmulti)
}

bsd_rforkmulti_long(
	mach_port_t		vproc_port, 
	int			*interrupt,
	node_t			node_array[],
	unsigned int		node_count,
	int			*rval_array[],		/* OUT */
	unsigned int		*rval_count,
	pid_t			*ch_pid_array[],	/* OUT */
	unsigned int		*ch_pid_count,
	mach_port_t		fp_name_array[],
	unsigned int		nfp_names,
	mach_port_t		fp_right_array[],
	unsigned int		nfp_rights,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name)
{
	kern_return_t		ret;

	START_VPROCSERVER(SYS_rforkmulti, 4)

	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) rval_array,
			  node_count * sizeof(int),
			  TRUE);
	if (ret != KERN_SUCCESS) {
		error = EAGAIN;
		goto out;
	}
	*rval_count = node_count;

	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) ch_pid_array,
			  node_count * sizeof(pid_t),
			  TRUE);
	if (ret != KERN_SUCCESS) {
		error = EAGAIN;
		goto out;
	}
	*ch_pid_count = node_count;

	error = rforkmulti(vproc_port,
			   FALSE,
			   node_array, node_count,
			   *rval_array, rval_count,
			   *ch_pid_array, ch_pid_count,
			   fp_name_array, nfp_names,
			   fp_right_array, nfp_rights,
			   ch_state, ch_state_count,
			   vproc_port_name, cred_port_name);

out:
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) fp_name_array,
			     nfp_names * sizeof(mach_port_t));
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) fp_right_array,
			     nfp_rights * sizeof(mach_port_t));
	if (error != ESUCCESS) {
		if (*rval_array != NULL) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) *rval_array,
					     node_count * sizeof(int));
			*rval_array = NULL;
			*rval_count = 0;
		}
		if (*ch_pid_array != NULL) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) *ch_pid_array,
					     node_count * sizeof(pid_t));
			*ch_pid_array = NULL;
			*ch_pid_count = 0;
		}
	}

	END_VPROCSERVER_NOSIG(SYS_rforkmulti)
}

bsd_set_tnc_port(
	mach_port_t		proc_port, 
	int			*interrupt,
	int			id,
	mach_port_t		port)
{
	START_SERVER(SYS_set_tnc_port, 1)
	
	error = set_tnc_port(id, port);

	END_SERVER(SYS_set_tnc_port)
}
	
bsd_get_tnc_port(
	mach_port_t		proc_port, 
	int			*interrupt,
	int			id,
	int			node,
	mach_port_t		*portp)
{
	START_SERVER(SYS_get_tnc_port, 2)

	error = get_tnc_port(id, node, portp);

	END_SERVER(SYS_get_tnc_port)
}

int
bsd_table_node_set(
	mach_port_t	proc_port,
	boolean_t	*interrupt,
	node_t		node,
	int		id,
	int		index,
	int		lel,
	int		nel,
	char		*addr,
	unsigned int	count,
	int		*nel_done)
{
	int	cnt;
	struct	vproc *vp;

	START_SERVER(SYS_table_node, 6)

#ifdef NX
	node = nx_map_node_pid(procp->p_pid, node);
	if (node == -1) {
	    error = EINVAL;

	    END_SERVER(SYS_table_node)
	}
#endif /* NX */

	/*
	 * Perform the appropriate permissions checks. 
	 */
	if (TBL_MAXUPRC == id) {
		if (error = suser(u.u_cred, &u.u_acflag))
			goto out;
	}

#ifdef SLL
	/*
	 * Perform the appropriate permissions checks.
	 * (TBL_MAXUPRC should be in this switch too!)
	 */
	switch(id) {
	case TBL_FASTNODE: {
#if SEC_BASE
		/*
		 * Must have the SEC_SYSATTR privilege.
		 */
		if ( !privileged(SEC_SYSATTR, EPERM) ) {
			error =  EPERM;
			goto out;
		}
#else /* SEC_BASE */
		/*
		 * Must be super user
		 */
		if ( error = suser(u.u_cred, &u.u_acflag) ) {
			goto out;
		}
#endif /* SEC_BASE */
		break;
	}
	}
#endif /* SLL */

	if (nel < 0) {
		switch(id) {

		case TBL_MAXUPRC:
			/*
			 * Operations identified by a process id, and thus
			 * done on a particular vproc.
			 */
	        	nel = (nel * -1);  /* Convert back to a positive. */
			nel += index; /* nel unused; make it top of pid range */
			for (cnt = 0; index < nel; index++, cnt++) {
				vp = LOCATE_VPROC_PID(index);
				if (vp == NULL) {
					error = ESRCH;
					goto out;
				}
				count = lel;
				error = PVPOP_TABLE_SET(vp,
					        	id, 
					        	addr, count,
							lel);
				VPROC_RELEASE(vp,"bsd_table_node_set");
				if (error != ESUCCESS)
					goto out;
				addr += lel;
			}
			*nel_done = cnt;
			break;
#ifdef SLL
		case TBL_FASTNODE: {
			/*
			 * Operations done on a particular node.
			 */
			error = PVPSOP_TABLE_SET(node,
						id, index,
						addr, count,
						nel, lel,
						nel_done);
			break;
		}
#endif /* SLL */

		default:
			error = EINVAL;
			goto out;
		}
	}
out:
	END_SERVER(SYS_table_node)
}

int
bsd_table_node_get(
	mach_port_t	proc_port,
	boolean_t	*interrupt,
	node_t		node,
	int		id,
	int		index,
	int		lel,
	int		nel,
	char		**addr,		/* out, dealloc (ool memory) */
	unsigned int	*count,		/* out, size of the above */
	int		*nel_done)
{
	char	*local_addr;
	int	cnt;
	struct	vproc	*vp;
	unsigned int	vm_count = 0;

	START_SERVER(SYS_table_node, 6)

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*addr = 0;
	*count = 0;

#ifdef NX
	node = nx_map_node_pid(procp->p_pid, node);
	if (node == -1) {
	    error = EINVAL;

	    END_SERVER(SYS_table_node)
	}
#endif /* NX */

	/*
	 * Perform the appropriate permissions checks. 
	 */
	switch(id) {

	case TBL_PROCINFO:
		if ((error = suser(u.u_cred, &u.u_acflag)) &&
		    (error = sameuser(u.u_cred, u.u_procp->p_rcred))) {
			*addr = 0;
			goto out;
		}
		break;

	case TBL_ARGUMENTS:
	case TBL_ENVIRONMENT:
	case TBL_MAXUPRC:
	case TBL_UAREA:
	case TBL_U_TTYD:
		if (id != u.u_procp->p_pid && 
		   (error = suser(u.u_cred, &u.u_acflag) &&
		   (error = sameuser(u.u_cred, u.u_procp->p_rcred)))) {
			*addr = 0;
			goto out;
		}
		break;
	}

	vm_count = *count = nel * lel;	/* later, use vm_count for dealloc */
	vm_allocate_strict(mach_task_self(), (vm_address_t *)addr, *count,
			TRUE);
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
		local_addr = *addr;
		nel += index;	/* nel unused; make top of pid range */
		for (cnt = 0; index < nel; index++, cnt++) {
			vp = LOCATE_VPROC_PID(index);
			if (vp == NULL) {
				error = ESRCH;
				goto out;
			}
			*count = lel;
			error = PVPOP_TABLE_GET(vp,
					        id, 
					        &local_addr, 
					        count,
						lel);
			VPROC_RELEASE(vp,"bsd_table_node_get");
			if (error != ESUCCESS) 
				goto out;
			local_addr += lel;
		}
		*nel_done = cnt;
		*count = cnt * lel;
		break;

	default:
		*count = nel * lel;
		error = PVPSOP_TABLE_GET(node,
				        id, index, 
				        addr, count, 
					nel, lel,
				        nel_done);
		break;
    	}
out:
	if (NCPUS == 1 || serial)
		unix_release();
	error = end_server_op(error, interrupt);
	if (error && *addr) {
		vm_deallocate_strict(mach_task_self(), (vm_address_t) *addr,
				     vm_count);
		*count = 0;
	}
	return(error);
}

/*
 * System V IPC calls.
 */
bsd_get_my_svipc_port(
	mach_port_t	proc_port,
	int		*interrupt,
	mach_port_t	*svipc_portp)
{
	START_SERVER(1007, 1)

	error = get_my_svipc_port(svipc_portp);

	END_SERVER(1007)
}

#ifdef	CHKPNT
int
bsd_chkpnt_self(
	mach_port_t		vproc_port, 
	int			*interrupt,
	int			flags,
	node_t			node,
	thread_state_t		cur_state,
	unsigned int		cur_state_count,
	chkpnt_file_state_t	*fs,
	int			fs_size)
{
	START_VPROCSERVER(SYS_migrate, 3)

	error = chkpnt_self(port_to_vproc_lookup(vproc_port),
			     flags, node, cur_state, cur_state_count, fs);

	(void) vm_deallocate(mach_task_self(), (vm_address_t) fs, fs_size);	

	END_VPROCSERVER(SYS_migrate)
}

int
bsd_exec_restart(
	mach_port_t		vproc_port, 
	int			*interrupt,
	char			*chkpnt_prefix,
	unsigned int		chkpnt_prefix_count,
	thread_state_t		*new_state,
	unsigned int		*new_state_count,
	mach_port_t		*new_rootdir_port,
	mach_port_t		*new_currentdir_port,
	chkpnt_file_state_t	**fs,
	int			*fs_size,
	boolean_t		*traced)
{
	START_VPROCSERVER(SYS_exec_restart, 4)

	error = exec_restart(port_to_vproc_lookup(vproc_port),
			     chkpnt_prefix,
			     new_state, new_state_count,
			     new_rootdir_port, new_currentdir_port,
			     fs, fs_size,
			     traced);

	*traced = (u.u_procp->p_flag & STRC);

	END_VPROCSERVER(SYS_exec_restart)
}

int
bsd_forkfamily(
	mach_port_t		vproc_port, 
	int			*interrupt,
	node_t			pid_list[],
	int			nproc,
	mach_port_t		fp_name_array[],
	unsigned int		nfp_names,
	mach_port_t		fp_right_array[],
	unsigned int		nfp_rights,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name)
{
	int			rval_array[MAX_MULTI_LIST_SIZE];
	pid_t			ch_pid_array[MAX_MULTI_LIST_SIZE];
	unsigned int		rval_count = nproc;
	struct vproc		*vp = port_to_vproc_lookup(vproc_port);

	START_VPROCSERVER(SYS_forkfamily, 5)

	error = rforkmulti(vp,
			   TRUE,
			   pid_list, nproc,
			   rval_array, &rval_count,
			   ch_pid_array, &rval_count,
			   fp_name_array, nfp_names,
			   fp_right_array, nfp_rights,
			   ch_state, ch_state_count,
			   vproc_port_name, cred_port_name);

	/*
	 * Scan through the returns ... any failure implies total failure.
	 * Total failure means that we have to build a list of children
	 * created and kill them all - using a SIGPROC_MULTI().
	 */
	if (error == ESUCCESS) {
		unsigned int	i;
		unsigned int	nproc_created = 0;
		for (i = 0; i < nproc; i++) {
			if (rval_array[i] == ESUCCESS)
				ch_pid_array[nproc_created++] = pid_list[i];
		}
		if (nproc_created != nproc && nproc_created > 0) {
			(void) PVPOP_SIGPROC_MULTI(VPROCPTR(ch_pid_array[0]),
						   SIGKILL, 0,
						   0, 0, 0,
						   vp->vp_pid,
						   0, VPROC_HAS_PRIV,
						   0,
						   ch_pid_array, 
						   nproc_created,
						   NULL);
		}
	}

	END_VPROCSERVER_NOSIG(SYS_forkfamily)
}

int
bsd_forkfamily_long(
	mach_port_t		vproc_port, 
	int			*interrupt,
	node_t			pid_list[],
	int			nproc,
	mach_port_t		fp_name_array[],
	unsigned int		nfp_names,
	mach_port_t		fp_right_array[],
	unsigned int		nfp_rights,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name)
{
	int			*rval_array;
	unsigned int		rval_count;
	pid_t			*ch_pid_array;
	unsigned int		ch_pid_count;
	kern_return_t		ret;
	struct vproc		*vp = port_to_vproc_lookup(vproc_port);

	START_VPROCSERVER(SYS_forkfamily, 5)

	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &rval_array,
			  nproc * sizeof(int),
			  TRUE);
	if (ret != KERN_SUCCESS) {
		error = EAGAIN;
		goto out;
	}
	rval_count = nproc;

	/*
	 * The child pid array is allocated to satisfy the rforkmulti()
	 * code and is deallocated on return and is not required to be
	 * passed back to the caller (who knows the pids!).
	 */
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &ch_pid_array,
			  nproc * sizeof(pid_t),
			  TRUE);
	if (ret != KERN_SUCCESS) {
		error = EAGAIN;
		goto out;
	}
	ch_pid_count = nproc;

	error = rforkmulti(vp,
			   TRUE,
			   pid_list, nproc,
			   rval_array, &rval_count,
			   ch_pid_array, &ch_pid_count,
			   fp_name_array, nfp_names,
			   fp_right_array, nfp_rights,
			   ch_state, ch_state_count,
			   vproc_port_name, cred_port_name);

	/*
	 * Scan through the returns ... any failure implies total failure.
	 * Total failure means that we have to build a list of children
	 * created and kill them all - using a SIGPROC_MULTI().
	 */
	if (error == ESUCCESS) {
		unsigned int	i;
		unsigned int	nproc_created = 0;
		for (i = 0; i < nproc; i++) {
			if (rval_array[i] == ESUCCESS)
				ch_pid_array[nproc_created++] = pid_list[i];
		}
		if (nproc_created != nproc && nproc_created > 0) {
			(void) PVPOP_SIGPROC_MULTI(VPROCPTR(ch_pid_array[0]),
						   SIGKILL, 0,
						   0, 0, 0,
						   vp->vp_pid, 0,
						   VPROC_HAS_PRIV,
						   0,
						   ch_pid_array, 
						   nproc_created,
						   NULL);
		}
	}
out:
	if (ch_pid_array != NULL)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) ch_pid_array,
				     nproc * sizeof(pid_t));
	if (rval_array != NULL)
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) rval_array,
				     nproc * sizeof(int));

	END_VPROCSERVER_NOSIG(SYS_forkfamily)
}

#endif	CHKPNT
