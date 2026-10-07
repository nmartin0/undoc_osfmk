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
 * @OSF_COPYRIGHT@
 */
/* 
 * HISTORY
 * $Log: svipc_shm.c,v $
 * Revision 1.9  1994/11/18  20:28:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/01/13  17:52:31  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.7  1993/10/08  00:56:29  jlitvin
 * Fixes PTS #4748.  Our SYSV shared memory table is initialized to all
 * zeroes, so we get a false match with an address of zero.  Kludge
 * around this.  (Non-zero addresses work fine).
 *
 * Revision 1.6  1993/09/29  21:00:48  jlitvin
 * If vm_deallocate() fails, we don't need to call
 * ux_server_thread_unblocking() since we never made the blocking call.
 *
 * Revision 1.5  1993/07/14  17:49:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  18:50:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:06:09  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1992/11/30  22:16:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  18:21:50  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:07:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.10  93/06/02  09:50:48  yazz
 * Correct braces and ifdef interaction so braces correctly match up.
 * 
 * Revision 2.9  92/10/28  14:55:24  roman
 * Fix up some typecasting for cleaner compilations.
 * 
 * Revision 2.8  92/10/13  16:07:34  mbarnett
 * Changed code in shmat server function to fix the address parameter passed
 * in to vm_region for the user task on whose behalf the server does an attach.
 * 
 * Revision 2.7  92/09/25  16:05:45  mbarnett
 * Changed server shmat code to pass in the user task(on whose behalf the server 
 * is doing an attach) as the first parameter in the call to vm_region.  
 * Previously, the server task was incorrectly being passed in as the first 
 * Revision 2.9  1992/10/28  14:55:24  roman
 * Fix up some typecasting for cleaner compilations.
 *
 * Revision 2.8  92/10/13  16:07:34  mbarnett
 * Changed code in shmat server function to fix the address parameter passed
 * in to vm_region for the user task on whose behalf the server does an attach.
 * 
 * Revision 2.7  92/09/25  16:05:45  mbarnett
 * Changed server shmat code to pass in the user task(on whose behalf the server 
 * is doing an attach) as the first parameter in the call to vm_region.  
 * Previously, the server task was incorrectly being passed in as the first 
 * parameter in the call to vm_region.  Also, the line of code in the server
 * shmdt function which called "ux_server_thread_unblocking" was removed as the
 * call to this routine caused the server to hang since there was not a 
 * previously corresponding call to "ux_server_thread_blocking".
 * 
 * Revision 2.6  92/05/26  12:10:19  pjg
 * 	Don't call ux_server_thread_[ub]blocking around calls to vm_deallocate.
 * 	Change the comments in these calls.
 * 
 * Revision 2.5  92/05/24  14:17:54  pjg
 * 	92/03/23  18:02:28  condict
 * 	Add ux_server_thread_blocking/unblocking calls around each vm call,
 * 	since they sends messages to the vnode_pager, which can be blocked.
 * 	[92/05/20            srl]
 * 
 * Revision 2.4  92/05/18  12:29:11  roy
 * 	remote_vnode_pager_get now returns a node number instead of a port.
 * 	Name change:  vnode_flush_object -> vnode_pager_flush
 * 	[92/05/11            roy]
 * 
 * Revision 2.3  92/03/20  11:41:04  pjg
 * 	Changes for osf1_adfs (durriya) 
 * 
 * 	[Revision 3.1  91/12/17  17:18:47  jose]
 * 	Changed shared memory to use the vnode pager for back up.
 * 	Corrected bugs on segment deallocation.
 * 
 * Revision 2.2  91/12/16  13:07:40  roy
 * 	Initial check-in.
 * 
 * Revision 3.0  91/10/31  12:27:12  david
 * ported from 1.0.2
 * 
 * Revision 1.8.6.2  91/04/24  12:08:04  rod
 * 	In shmat(), when translating error returns from vm_map(), map
 * 	KERN_INVALID_ADDRESS to EINVAL.
 * 	[91/04/12  15:04:06  rod]
 * 
 * Revision 1.8.1.4  91/03/05  13:39:46  lwa
 * 	Get default attach address from address configuration record (mmap
 * 	BSS area) instead of using VM_MIN_ADDRESS (avoid conflict with brk)
 * 	Also, use vm_map_lookup() instead of vm_map_lookup_entry() to get
 * 	backing object.
 * 	[91/02/28  11:09:56  lwa]
 * 
 * Revision 1.8.1.3  90/12/20  12:11:37  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.8.1.2  90/12/14  13:43:17  bet
 * 	Check in shmdt() that the address returned from vm_map_lookup_entry()
 * 	matches the map entry starting address.
 * 	[90/12/14  13:40:15  bet]
 * 
 * Revision 1.8  90/10/31  13:49:41  devrcs
 * 	Added check for a negative shared memory segment id in shmconv().
 * 	[90/10/11  10:34:28  bet]
 * 
 * Revision 1.7  90/10/07  13:19:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:00:01  gm]
 * 
 * 	Check for pager == MEMORY_OBJECT_NULL in shmdt().
 * 	Define addr (virtual address) as caddr_t.
 * 	Added assert(pager != MEMORY_OBJECT_NULL) to shmget().
 * 	[90/09/26  10:55:17  bet]
 * 
 * 	New system call interface
 * 	[90/08/17  17:39:23  nags]
 * 
 * 	use new sec_svipc_object_create() to localize the number
 * 	of places that need SIP locking and audit hooks
 * 	[90/08/14  15:18:03  hosking]
 * 
 * 	tmp checkin for merge
 * 	[90/08/22  14:32:34  bet]
 * 
 * Revision 1.5  90/08/09  13:15:02  devrcs
 * 	Back out of shm_mark change in previous revision.
 * 	[90/07/30  15:47:03  bet]
 * 
 * Revision 1.3  90/06/22  20:07:06  devrcs
 * 	nags merge
 * 	[90/06/12  19:07:54  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:40:49  seiden]
 * 
 * 	Cleaned up comments.
 * 	[90/06/04  14:39:10  bet]
 * 
 * Revision 1.2  90/04/27  18:52:51  devrcs
 * 	Initial version of SysV shared memory.
 * 	[90/04/20  18:22:51  bet]
 * 
 * $EndLog$
 */
/* %W% %U% %G% SecureWare */
/* @(#)svipc_shm.c	6.3 */
#ifdef	OSF1_SERVER
#include <uxkern/import_mach.h>
#endif	/* OSF1_SERVER */
#include <sys/secdefines.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/addrconf.h>
#ifndef	OSF1_SERVER
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#else	/* OSF1_SERVER */
#include <uxkern/vm_param.h>
#endif	/* OSF1_SERVER */
#include <mach/kern_return.h>
#include <mach/machine/vm_types.h>
#ifndef	OSF1_SERVER
#include <vm/vm_object.h>
#else	/* OSF1_SERVER */
#include <mach/mach_port.h>
#endif	/* OSF1_SERVER */
#
#if	SEC_BASE
#include <sys/security.h>
#endif	
#if	SEC_ARCH
#include <sys/secpolicy.h>
#endif

struct	shminfo	shminfo = {	/* shared memory info structure */
		SHMMAX,
		SHMMIN,
		SHMMNI,	
		SHMSEG
};		

struct	shmid_internal	shmem[SHMMNI];	/* shared memory headers */
#ifndef OSF1_SERVER
extern	time_t		time;		/* system idea of date */
#endif /* OSF1_SERVER */

#if	SEC_ARCH
/*
 * Allocate space for the shared memory tag pools.  On systems that allocate
 * the shared memory structures dynamically, the tag pools should also be
 * dynamically allocated at the same time.
 */
tag_t		shmtag[SHMMNI * SEC_TAG_COUNT];
#endif	

#define FREE_ENTRY(sp)				\
MACRO_BEGIN					\
	sp->shm_pager = MEMORY_OBJECT_NULL;	\
	sp->s.shm_perm.mode = 0;		\
	sp->s.shm_segsz = 0;			\
	if (((int)(++(sp->s.shm_perm.seq) * shminfo.shmmni + (sp - shmem))) < 0) \
	    sp->s.shm_perm.seq = 0;		\
MACRO_END

/*
 * convert user supplied shmid into a ptr to the associated
 * shared memory table entry.
 */
struct shmid_internal *
shmconv(s)
register int	s;	/* shmid */
{
	register struct shmid_internal	*sp;	/* shared memory table entry */

	if (s < 0)
		return (NULL);

	sp = &shmem[s % shminfo.shmmni];

	if (!(sp->s.shm_perm.mode & IPC_ALLOC)
	    || s / shminfo.shmmni != sp->s.shm_perm.seq) {
		return(NULL);
	}
#if	SEC_ARCH
	audstub_levels(SHMTAG(sp, 0));
#endif	

	return(sp);
}


/* 
 * shmat system call - attach a shared memory segment.
 */ 
/* ARGSUSED */
shmat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args { 		
		int	shmid;	/* shared mem id, returned from shmget call */
 		caddr_t	addr;	/* virtual address requested for attach */
 		int	flag;	/* valid flags: SHM_RND, SHM_RDONLY */
 	}	*uap = (struct args *)args;
 	register struct shmid_internal	*sp;	/* shared memory table entry */
 	register boolean_t		findspace;
	register vm_prot_t		prot;	/* protection */
 	register kern_return_t		result;	/* return value from vm_map()*/
	vm_offset_t			addr;	/* virtual address */

	if ((sp = shmconv(uap->shmid)) == NULL)
		return(EINVAL);

#if	SEC_ARCH
	if (sec_ipcaccess(&sp->s.shm_perm, SHMTAG(sp, 0), SP_READACC))
#else	
	if (ipcaccess(&sp->s.shm_perm, SHM_R))
#endif	
		return(EACCES);

	if ((uap->flag & SHM_RDONLY) == 0)
#if	SEC_ARCH
		if (sec_ipcaccess(&sp->s.shm_perm, SHMTAG(sp, 0), SP_WRITEACC))
#else	
		if (ipcaccess(&sp->s.shm_perm, SHM_W))
#endif
			return(EACCES);

	/*
	 * check we don't exceed per-process limit on attaches
	 */
	if (u.u_shmsegs >= shminfo.shmseg)
		return(EMFILE);

	/* 
	 * kernel chooses virtual address if 0 specified 
	 */
	if (uap->addr == 0) {
		addr = (vm_offset_t)addressconf[AC_MMAP_BSS].ac_base;
		findspace = TRUE;
	} else {
		/* 
		 * round user-supplied address 
		 */
		addr = uap->flag & SHM_RND? 
                  (vm_offset_t)uap->addr & ~(SHMLBA - 1):
                    (vm_offset_t)uap->addr;
		findspace = FALSE;
	}

	/*
	 * set protection
	 */
	prot =  uap->flag & SHM_RDONLY? VM_PROT_READ: 
		                        VM_PROT_READ|VM_PROT_WRITE;

#ifdef	OSF1_SERVER
	ux_server_thread_blocking();	/* May contact vnode pager */
#endif	OSF1_SERVER
	/*  
	 * allocate a range in the current_task map, referring to the
	 * memory defined by the memory object in sp->shm_pager 
         */
	result = vm_map(u.uu_procp->p_task, &addr, sp->s.shm_segsz, 
			     0, findspace, sp->shm_pager, 0, FALSE, prot,
			     prot, VM_INHERIT_SHARE);
#ifdef	OSF1_SERVER
	ux_server_thread_unblocking();
#endif

	if (result != KERN_SUCCESS)
		switch (result) {

		      case KERN_NO_SPACE:
		      case KERN_MEMORY_FAILURE:
		      case KERN_MEMORY_ERROR:
			return(ENOMEM);
		      case KERN_INVALID_ADDRESS:
		      default:
			return(EINVAL);
		}
	
	/* 
	 * bump the object reference count on the first attach
	 * so the object won't be removed on the last detach 
	 */	
	if (sp->shm_bumpref) {
#ifndef OSF1_SERVER
		vm_object_reference((vm_object_t)port_object_get(sp->shm_pager));
#else	/* OSF1_SERVER */
		vm_size_t		size;
		vm_prot_t		out_prot;
		vm_prot_t		max_prot;
		vm_inherit_t		inherit;
		boolean_t		single;
		vm_offset_t		offset;

		ux_server_thread_blocking();	/* May contact vnode pager */
		/*
		 * We don't really want to map this into the server but it
		 * is the only way to get a reference so we just map one
		 * page so we don't burn address space.
		 */
		sp->shm_serv_addr = 0;
		if (vm_map(current_task(), &sp->shm_serv_addr, PAGE_SIZE,
			   0, TRUE, sp->shm_pager, 0, FALSE, VM_PROT_READ,
			   VM_PROT_READ, VM_INHERIT_NONE) !=  KERN_SUCCESS) {
		  ux_server_thread_unblocking();
		  return(EINVAL);
		}
		ux_server_thread_unblocking();

		/*
		 * Get the internal name of the object so that we can
		 * look it up when detaching
		 */
		if (vm_region(u.uu_procp->p_task, &addr, &size, 
                              &out_prot, &max_prot, &inherit, &single, 
                              &sp->shm_obj_name, &offset)
		    != KERN_SUCCESS)
			return(EINVAL);
#endif	/* OSF1_SERVER */
		sp->shm_bumpref = FALSE;
	}

	u.u_shmsegs++;
	*retval = (int) addr;

#ifndef OSF1_SERVER
	sp->s.shm_atime = time;
#else
	TIME_READ_LOCK();
	sp->s.shm_atime = time.tv_sec;
	TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */
	sp->s.shm_lpid = u.u_procp->p_pid;
	sp->s.shm_nattch++;
	return(0);
}

/*
 * shmctl system call - perform shared memory control operation.
 */
/* ARGSUSED */
shmctl(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int		shmid,	/* shared mem id, returned by shmget */
				cmd;	/* IPC_RMID, IPC_STAT, or IPC_SET */
		struct shmid_ds	*arg;  

	}	*uap = (struct args *)args;
	register struct shmid_internal	*sp;	/* shared memory table entry */
	struct shmid_ds			ds;	/* hold area for IPC_SET */
	int				error;
#if	SEC_ARCH
	dac_t				dac;
	int				ret;
#endif	

	if ((sp = shmconv(uap->shmid)) == NULL)
		return(EINVAL);

#if	SEC_BASE
	/*
	 * Set audit event type based on function code
	 */
	audstub_ipc1(uap->cmd);
#endif
	switch(uap->cmd) {

	/* 
	 * Remove the shared memory identifier
	 */
	case IPC_RMID:
#if	SEC_BASE
		if (!sec_owner(sp->s.shm_perm.uid, sp->s.shm_perm.cuid))
			return(EPERM);
#else	
		if ((u.u_uid != sp->s.shm_perm.uid) && (u.u_uid != 
                                                        sp->s.shm_perm.cuid)
		    && (error = suser(u.u_cred, &u.u_acflag)))
			return(error);
#endif	

#if	SEC_ARCH
		if (sec_ipcaccess(&sp->s.shm_perm, SHMTAG(sp, 0), 
                                  SP_DELETEACC))
			return(EACCES);
#endif
		/*
		 * decrement object ref_count so it will be removed
		 * now if there are no attached to it otherwise 
		 * after the last detach.
		 */
#ifndef	OSF1_SERVER
		vm_object_deallocate(port_object_get(sp->shm_pager));
		inode_pager_release(sp->shm_pager);
#else
		if (!sp->shm_bumpref) {

#ifdef OSF1_ADFS
			if ((error = remote_vnode_pager_flush(
                                                     sp->shm_node,
                                                     sp->shm_pager,
						     (vm_offset_t)0,
						     (vm_size_t)sp->s.shm_segsz,
						     FALSE, FALSE, TRUE, 
                                                     VM_PROT_NONE))
			    != KERN_SUCCESS) 
#else
			if ((error = vnode_pager_flush(sp->shm_pager,
						     (vm_offset_t)0,
						     (vm_size_t)sp->s.shm_segsz,
						     FALSE, FALSE, TRUE, 
                                                     VM_PROT_NONE))
			    != KERN_SUCCESS)
#endif /* OSF1_ADFS */
                          {


				printf("shmctl: vnode_pager_flush error %d\n", 
                                       error);
				return(EINVAL);
			}
			if ((error = vm_deallocate(current_task(), 
                                                   sp->shm_serv_addr,
						   PAGE_SIZE))
			    != KERN_SUCCESS) {
				printf("shmctl: vm_deallocate error %d\n", 
                                       error);
				return(EINVAL);
			}
                }
		
#ifdef OSF1_ADFS
                /* 
                 * for ad we cannot directly call inode_pager_release. We have
                 * to go remote. However, since inode_pager_release just
                 * calls mach_dellocate_port we will save ourselves the 
                 * expense of going remote.
                 */
                mach_port_deallocate(mach_task_self(), sp->shm_pager);
#else
		inode_pager_release(sp->shm_pager);
#endif /* OSF1_ADFS */
                
#endif /* OSF1_SERVER */
                
                FREE_ENTRY(sp);
                
		break;
                
	/* 
	 * Set ownership and permissions
	 */
	case IPC_SET:
#if	SEC_BASE
		if (!sec_owner(sp->s.shm_perm.uid, sp->s.shm_perm.cuid))
			return(EPERM);
#else
		if ((u.u_uid != sp->s.shm_perm.uid) && (u.u_uid != 
                                                        sp->s.shm_perm.cuid)
		    && (error = suser(u.u_cred, &u.u_acflag)))
			return(error);
#endif

#if	SEC_ARCH
		if (sec_ipcaccess(&sp->s.shm_perm, SHMTAG(sp, 0), 
                                  SP_SETATTRACC))
			return(EACCES);
#endif	

		if (copyin(uap->arg, &ds, sizeof(ds)))
			return(EFAULT);
#if	SEC_BASE
                if (!sec_owner_change_permitted(sp->s.shm_perm.uid,
                                sp->s.shm_perm.gid, ds.shm_perm.uid,
                                ds.shm_perm.gid))
                        return(EPERM);
		audstub_dac(0, sp->s.shm_perm.uid, sp->s.shm_perm.gid,
				sp->s.shm_perm.mode);
#endif	
		sp->s.shm_perm.uid = ds.shm_perm.uid;
		sp->s.shm_perm.gid = ds.shm_perm.gid;
		sp->s.shm_perm.mode = (ds.shm_perm.mode & 0777) |
			(sp->s.shm_perm.mode & ~0777);
#if	SEC_ARCH
		dac.uid = sp->s.shm_perm.uid;
		dac.gid = sp->s.shm_perm.gid;
		dac.mode = sp->s.shm_perm.mode;
		ret = SP_CHANGE_OBJECT(SHMTAG(sp, 0), &dac,
				SEC_NEW_UID|SEC_NEW_GID|SEC_NEW_MODE);
		if (ret) {
			if (ret & SEC_NEW_UID)
				sp->s.shm_perm.uid = dac.uid;
			if (ret & SEC_NEW_GID)
				sp->s.shm_perm.gid = dac.gid;
			if (ret & SEC_NEW_MODE)
				sp->s.shm_perm.mode =
					(sp->s.shm_perm.mode & ~0777)
						 | (dac.mode & 0777);
		}
#endif
#if	SEC_BASE
		audstub_dac(1, sp->s.shm_perm.uid, sp->s.shm_perm.gid,
				sp->s.shm_perm.mode);
#endif	
#ifndef OSF1_SERVER
		sp->s.shm_ctime = time;
#else
		TIME_READ_LOCK();
		sp->s.shm_ctime = time.tv_sec;
		TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */
		break;

	/*
	 * Get shared memory data structure
	 */
 	case IPC_STAT:

#if	SEC_ARCH
		if (sec_ipcaccess(&sp->s.shm_perm, SHMTAG(sp, 0), SP_READACC))
#else	
		if (ipcaccess(&sp->s.shm_perm, SHM_R))
#endif
			return(EACCES);

		if (copyout(&sp->s, uap->arg, sizeof(sp->s)))
			return(EFAULT);

		break;

	default:
		return(EINVAL);
	}
	return(0);
}


/*
 * shmdt system call - detach a shared memory segment.
 */
/* ARGSUSED */
shmdt(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		caddr_t	addr;	/* virtual address of attached mem, from 
                                   shmat call */
	} *uap = (struct args *)args;
	register struct shmid_internal *sp;	/* shared memory table entry */
#ifndef	OSF1_SERVER
	vm_map_t		var_map = current_task()->map;
#endif	/* OSF1_SERVER */
	caddr_t			tmp_addr = uap->addr;
	vm_size_t		size;
	vm_prot_t		max_prot;
	vm_inherit_t		inherit;
#ifndef	OSF1_SERVER
	port_t			obj_name;
	vm_map_version_t	vers;
	vm_object_t		vm_obj;
	boolean_t		wired;
	memory_object_t		pager;		/* memory object */
#else	/* OSF1_SERVER */
	mach_port_t		obj_name;
#endif	/* OSF1_SERVER */
	vm_offset_t		offset;
	vm_prot_t		out_prot;
	boolean_t		single;

	/* first, validate that this address starts a region.  shmdt
	 * only allows detaching a complete region.
	 */

#ifndef	OSF1_SERVER
	if (vm_region(var_map, 
		      (vm_address_t *) &tmp_addr, &size, 
		      &out_prot, &max_prot, 
                      &inherit, &single, &obj_name, &offset) != KERN_SUCCESS)
		return(EINVAL);
	if (obj_name != PORT_NULL)
		port_release(obj_name);
#else	/* OSF1_SERVER */
	if (vm_region(u.uu_procp->p_task,
		      (vm_address_t *) &tmp_addr, &size, 
		      &out_prot, &max_prot, 
                      &inherit, &single, &obj_name, &offset) != KERN_SUCCESS)
		return(EINVAL);
#endif	/* OSF1_SERVER */

	if (uap->addr != tmp_addr)
		return(EINVAL);

#ifndef	OSF1_SERVER
	/*
	 * look up the starting address in the map, to find the backing
	 * memory object.
	 */

	if (vm_map_lookup(&var_map, uap->addr, VM_PROT_NONE, &vers, &vm_obj,
			  &offset, &out_prot, &wired, &single) != KERN_SUCCESS)
		return(EINVAL);
	pager = vm_obj->pager;
	vm_object_unlock(vm_obj);

	if (pager == MEMORY_OBJECT_NULL)
		return(EINVAL);
#endif	/* OSF1_SERVER */

	/* 
	 * find the shmem table entry whose pager
	 * matches the one from the found map.
	 */ 
	
	for (sp = shmem; sp < &shmem[shminfo.shmmni]; sp++)
#ifndef	OSF1_SERVER
		if (sp->shm_pager == pager)
#else	/* OSF1_SERVER */
		if (sp->shm_obj_name == obj_name)
#endif	/* OSF1_SERVER */
			break;

	/* there is no shmem table! */
	if (sp->shm_obj_name == 0)
		return(EINVAL);

	if (sp >= &shmem[shminfo.shmmni])
		/* the shmem table entry has already been removed */
		return(0);

	/* deallocate the address range in the current_task map */
#ifndef	OSF1_SERVER
	if ((vm_deallocate(var_map,
			   (vm_address_t) uap->addr,
			   sp->s.shm_segsz)) != KERN_SUCCES)
#else	/* OSF1_SERVER */
	if ((vm_deallocate(u.uu_procp->p_task, 
			   (vm_address_t) uap->addr, 
			   sp->s.shm_segsz)) != KERN_SUCCESS)
#endif	/* OSF1_SERVER */
	{
		return(EINVAL);
	}

#ifndef OSF1_SERVER
	sp->s.shm_dtime = time;
#else
	TIME_READ_LOCK();
	sp->s.shm_dtime = time.tv_sec;
	TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */
	sp->s.shm_lpid = u.u_procp->p_pid;
	sp->s.shm_nattch--;

	assert(u.u_shmsegs > 0);
	u.u_shmsegs--;

#if	SEC_ARCH
	audstub_levels(SHMTAG(sp, 0));
#endif	
	return(0);
}


/*
 * shmget system call - get a shared memory segment.
 */
/* ARGSUSED */
shmget(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		key_t	key;	/* key to identify the shared memory segment */
		u_int	size,	/* minimum bytes to allocate */
			shmflg;	/* valid flags: IPC_CREAT, 
                                 * IPC_EXCL, access modes 
                                 */
	}	*uap = (struct args *)args;
	struct shmid_internal		*sp;	/* shared memory table entry */
	memory_object_t        		pager;	/* memory object */
	int				s;	/* ipcget status */
	int				error;
#ifdef 	OSF1_ADFS
        node_t				node;
#endif

	if (error = ipcget(uap->key, uap->shmflg, shmem, shminfo.shmmni, 
			 sizeof(*sp), &s, (struct ipc_perm **)&sp))
		return(error);
#if	SEC_BASE
	/*
	 * Set the audit event type based on whether or not the
	 * segment already existed or is being created. The second
	 * argument is non-zero to indicate that we are dealing with
	 * a shared memory segment.  Unlike other SysV IPC objects,
	 * a shmget of an existing segment does not make it available
	 * to the calling process--that must be done with shmat.
	 */
	audstub_ipc2(s, 1);
#endif	

	if (s) {

		/*
		 * This is a new shared memory segment - init shmem table entry
		 */

		/*
		 * verify size is between system min and max 
		 */
		if (uap->size < shminfo.shmmin || uap->size > shminfo.shmmax) {
			sp->s.shm_perm.mode = 0;
			return(EINVAL);
		}

		/* 
		 * allocate a paging file and init a memory object for it 
		 */
#ifdef OSF1_ADFS
                error = remote_vnode_pager_get(FALSE, uap->size, &pager, &node);
		if (pager == MEMORY_OBJECT_NULL)
			error = ENOMEM;
		if (error) {
			FREE_ENTRY(sp);
                        return(error);
                }
#else
		pager = vnode_pager_get(FALSE, uap->size);
		if (pager == KERN_RESOURCE_SHORTAGE)  {
			FREE_ENTRY(sp);
			return(ENOMEM);
		}
#endif

		assert(pager != MEMORY_OBJECT_NULL);
		sp->shm_pager = pager;
#ifdef OSF1_ADFS
                sp->shm_node = node;
#endif
		sp->shm_bumpref = TRUE;
		sp->s.shm_segsz = uap->size;
#ifndef OSF1_SERVER
		sp->s.shm_ctime = time;
#else
		TIME_READ_LOCK();
		sp->s.shm_ctime = time.tv_sec;
		TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */
		sp->s.shm_cpid = u.u_procp->p_pid;
		sp->s.shm_atime = sp->s.shm_dtime = 0;
		sp->s.shm_lpid = 0;
		sp->s.shm_nattch = 0;
#if	SEC_ARCH
		sec_svipc_object_create(SHMTAG(sp, 0));
#endif	
	} else {
		/*
		 * found an existing segment - verify the size
		 */
		if (uap->size && (uap->size > sp->s.shm_segsz))
			return(EINVAL);
	}

	*retval = sp->s.shm_perm.seq * shminfo.shmmni + (sp - shmem);
	return(0);
}
