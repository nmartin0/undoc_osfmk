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
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: chkpnt_pproc.c,v $
 * Revision 1.4  1995/02/01  21:39:31  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.3  1994/11/18  20:43:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/10/25  23:28:41  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: High -- many lines changed in many files
 *  Benefit or PTS #: 9853
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/bsd/init_main.c
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_fork.c
 * 	server/bsd/kern_prot.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/sys/user.h
 * 	server/tnc/chkpnt_pproc.c
 * 	server/tnc/rvp_subr.c
 * 	server/tnc/tnc_svipc.c
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/syscall_subr.c
 * Side-thread changes.  Renamed p_sigref to the more general
 * p_exit_hold_count.
 *
 * Revision 1.1  1994/03/14  02:04:17  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 2.3  93/11/10  12:07:14  slk
 * *** empty log message ***
 * 
 * 
 * Revision 2.1.1.9  93/08/13  13:05:49  hao
 * 	Fixed code so that if looking up a directory port, we go to the node 
 * 	where the filesystem is mounted instead of always going to the root
 * 	filesystem node.
 * 
 * Revision 2.1.1.8  93/08/06  07:20:53  chrisp
 * 	Round addresses down to page boundaries before checking regions.
 * 		This change came at rev 2.1.1.3 but went at 2.1.1.4.
 * 
 * Revision 2.1.1.7  93/08/05  10:09:20  chrisp
 * 	Create checkpoint files with read and write permissions for owner.
 * 
 * Revision 2.1.1.6  93/08/04  07:17:23  chrisp
 * 	Check that the caller has read access for the checkpoint .stat file
 * 	before proceeding. This requires a remote_exec_check_vnode() call.
 * 
 * Revision 2.1.1.5  93/07/13  10:40:18  chrisp
 * 	Add interlock (using p_exit_hold_count) with server thread performing
 *		async core file write to prevent process exit until this
 *		completes.
 * 
 * Revision 2.1.1.4  93/07/02  10:46:56  chrisp
 * 	Correct last revision history.
 * 
 * Revision 2.1.1.3  93/07/02  10:29:28  chrisp
 * 	Round addresses down to page boundaries before checking regions.
 * 
 * Revision 2.1.1.2  93/06/25  08:01:49  chrisp
 * 	Revision 3.13  93/06/15  14:59:06  hao2
 * 	Added checks so that a program with extra memory regions 
 * 	will not be chkpnted.
 * 
 * Revision 2.1.1.1  93/06/10  11:49:26  chrisp
 * 	Revision 3.12  93/06/04  11:23:24  chrisp
 * 	Major revamp of core file writing to support asynchronous option plus
 * 		file renaming only when writing complete.
 * 
 * 	Revision 3.11  93/05/25  09:29:53  chrisp
 * 	Eliminate chkpnt_pproc_load_msg() and restart_pproc_unload_msg() in
 * 		favor of enhanced, common versions of migrate_pproc_load_msg()
 * 		and migrate_pproc_unload_msg() + migrate_pproc_fix().
 * 
 * 	Revision 3.10  93/05/19  15:26:18  hao2
 * 	Changed coding style so after the "if" statements, a new line is
 * 	used.
 * 
 * 	Revision 3.9  93/05/19  10:41:02  chrisp
 * 	Add function-level comments.
 * 
 * 	Revision 3.8  93/05/17  09:42:07  chrisp
 * 	Exit process failing exec_restart() w/ exit code reflecting error code.
 * 
 * 	Revision 3.7  93/05/11  15:34:48  hao2
 * 	Added changes so that the root/current directories can be reset from
 * 	non file server node.
 * 
 * 	Revision 3.6  93/04/26  10:38:25  hao2
 * 	Set the root/current directories both before the restart call
 * 	and before the chkpnted process begins to run.
 * 
 * 	Revision 3.5  93/04/22  08:50:44  chrisp
 * 	Minor change to comment ".image" -> image since we're now naming 
 * 		the image file with a ".core" extension.
 * 
 * 	Revision 3.4  93/04/12  15:18:29  chrisp
 * 	Addition of new root and current directory ports to restart_pproc_exec().
 * 
 * 	Revision 3.3  93/04/08  11:52:08  chrisp
 * 	Add pproc support for exec_restart().
 * 
 * 	Revision 3.2  93/03/19  09:43:13  chrisp
 * 	When writing state info to disk, update the file offset.
 * 
 * 	Revision 3.1  93/03/16  07:35:30  chrisp
 * 	Add support for obtaining physical process state for checkpointing and
 * 		for writing this to disk.
 * 
 * 	Revision 3.0  93/02/17  10:44:33  chrisp
 * 	First draft.
 * 
 * 	$EndLog$
 * 
 */


#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/acct.h>
#include <sys/wait.h>
#include <machine/vmparam.h>
#include <tnc/chkpnt.h>
#include <tnc/dpvproc.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/syscall_subr.h>
#include <kern/parallel.h>
#include <ufs/inode.h>
#include <mach.h>

/*
 * Forward references:
 */
int chkpnt_core_write(
	mach_port_t	vp,
	caddr_t		base,
	int		len,
	off_t		offset);
boolean_t is_null(
	vm_address_t	base,
	int		len);
int restart_pproc_setports(
	mach_port_t	cwd_port,
	mach_port_t	root_port);
int restart_pproc_get_dir_port(
	file_info_t	fi,
	mach_port_t	*dir_port);
int restart_pproc_make_fh(
	file_info_t	*fip,
	struct fhandle	*fhp);
int chkpnt_core_file(
	struct proc	*p,
	char		*path,
	unsigned int	path_len,
	caddr_t		user_area,
	int		user_area_size,
	caddr_t		kernel_stack,
	int		kernel_stack_size,
	caddr_t		stack,
	int		stack_size,
	caddr_t		data,
	int		data_size);

/*
 * Check and make sure we can chkpnt all regions of the memory.
 */
int
chkpnt_check_regions(struct proc *p)
{
	vm_address_t	address;
	vm_size_t 	size;
	vm_prot_t 	protection;
	vm_prot_t 	max_protection;
	vm_inherit_t 	inheritance;
	boolean_t 	shared;
	mach_port_t 	object_name;
	vm_offset_t 	offset;
	vm_address_t	region_start, region_end;
	vm_address_t	data_start, data_end;
	vm_address_t	text_start, text_end;
	vm_address_t	stack_start, stack_end;
	kern_return_t	ret;
	task_t		target_task = p->p_task;

	address = 0;
	text_start = (vm_address_t) trunc_page(u.u_text_start);
	text_end = (vm_address_t) (round_page(u.u_text_start)+ctob(u.u_tsize));
	data_start = (vm_address_t) trunc_page(u.u_data_start);
	data_end = (vm_address_t) (round_page(u.u_data_start)+ctob(u.u_dsize));
	stack_start = (vm_address_t) trunc_page(u.u_stack_start);
	stack_end = (vm_address_t) u.u_stack_end;

	while ((ret = vm_region(target_task, &address, &size, &protection, 
				&max_protection, &inheritance, &shared, 
				&object_name, &offset)) != KERN_NO_SPACE) {
		/* 
		 * Find the address for next region.
		 * And set the start and end address for this region.
		 */
		region_start = address;
		address += size;
		region_end = address;

		/*
		 * We can chkpnt all regions that are within the emulator,
		 * data, stack, and text.  We also ignore page 0.
		 */
		if (EMULATOR_BASE <= region_start && region_end <= EMULATOR_END)
			continue;
		if (data_start <= region_start && region_end <= data_end)
			continue;
		if (text_start <= region_start && region_end <= text_end) 
			continue;
		if (stack_start <= region_start && region_end <= stack_end) 
			continue;
		if (region_start == 0) 
			continue;
		/*
		 * The check here is for the ksh because it has a region
		 * at EMULATOR_END due to map file windows.
		 */
		if (region_start == EMULATOR_END)
			continue;

		/* 
		 * If there are other regions around, chkpnt should fail.
		 */
		return(ENOSYS);
	} 
	return(ESUCCESS);
}

/*
 * Routine to dump a standard core file of a given process (the caller
 * in this case) to a specified file.
 */
int
chkpnt_pproc_core(
	struct proc	*p,
	path_name_t	*image_path,
	THREAD_STATE_T	*tsp,
	boolean_t	async)
{
	int		error;
	kern_return_t	kr;
	vm_size_t	stack_size;
	vm_address_t	stack_addr;
	vm_offset_t	stack_copy_addr;
	vm_size_t	data_size = (vm_size_t) ctob(u.u_dsize);
	vm_address_t	data_addr = (vm_address_t) u.u_data_start;
	vm_offset_t	data_copy_addr;
	struct user	*fake_uarea;
	caddr_t		fake_kernel_stack;
	int		fake_kernel_stack_size;

	if (p->p_flag & SXONLY)
		return EPERM;

	/*
	 * Generate a faked kernel stack (i.e. registers) and determine
	 * the user stack address and size - suitably page rounded.
	 */
	FAKE_KERNEL_REGS(tsp, fake_kernel_stack, fake_kernel_stack_size);
	stack_addr = trunc_page(USER_STACK_POINTER(tsp));
	stack_size = round_page((vm_offset_t)USRSTACK - stack_addr);
	u.u_ssize = btoc(stack_size);

	if (ctob(UPAGES+u.u_dsize+u.u_ssize) >=
	    u.u_rlimit[RLIMIT_CORE].rlim_cur)
		return EFBIG;

	/*
	 *	Allocate fake uarea and fake the required fields
	 *	to keep the debuggers happy.
	 */
	fake_uarea = (struct user *) kalloc(sizeof *fake_uarea);
	if (fake_uarea == 0)
		return(error);
	fake_u(fake_uarea, p, current_thread());

	/*
	 * Read data and stack from user space into server space.
	 */
	kr = vm_read(p->p_task,
		     data_addr, data_size,
		     &data_copy_addr, &data_size);
	if (kr != KERN_SUCCESS) {
		return(EFAULT);
	}
	kr = vm_read(p->p_task,
		     stack_addr, stack_size,
		     &stack_copy_addr, &stack_size);
	if (kr != KERN_SUCCESS) {
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) data_copy_addr,
				     (vm_size_t) data_size);
		return(EFAULT);
	}

	/*
	 * If the core file is to be produced synchronously, do it.
	 */
	if (!async)
		return(chkpnt_core_file(p,
					(char *) image_path,
				        strlen(image_path) + 1,
					(caddr_t) fake_uarea,
					(int) sizeof *fake_uarea,
					(caddr_t) fake_kernel_stack,
					(int) fake_kernel_stack_size,
					(caddr_t) stack_copy_addr,
					(int) stack_size,
					(caddr_t) data_copy_addr,
					(int) data_size));

	/*
	 * Call a non-returning RPC to defer the core file write to
	 * a separate server thread, letting the calling process proceed.
	 * But first, increment the signal reference count so that the
	 * process cannot exit until the core file has been written.
	 */
	PROC_LOCK(p);
	p->p_exit_hold_count++;
	PROC_UNLOCK(p);
	error = cli_chkpnt_core_file(proc_to_port_lookup(p),
				     image_path, strlen(image_path) + 1,
				     (caddr_t) fake_uarea,
				     (int) sizeof *fake_uarea,
				     (caddr_t) fake_kernel_stack,
				     (int) fake_kernel_stack_size,
				     (caddr_t) stack_copy_addr,
				     (int) stack_size,
				     (caddr_t) data_copy_addr,
				     (int) data_size);

	if (error != ESUCCESS) {
		PROC_LOCK(p);
		p->p_exit_hold_count--;
		PROC_UNLOCK(p);
		/*
		 * If the message didn't get through, we have to
		 * free resources here.
		 */
		kfree(fake_uarea, sizeof *fake_uarea);
		kfree(fake_kernel_stack, fake_kernel_stack_size);
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) stack_copy_addr,
				     (vm_size_t) stack_size);
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) data_copy_addr,
				     (vm_size_t) data_size);
	}

	return (error);
}

/*
 * Service asynchronous core write requests.
 */
kern_return_t
svr_chkpnt_core_file(
	mach_port_t	proc_port,
	char		*path,
	unsigned int	path_len,
	caddr_t		user_area,
	int		user_area_size,
	caddr_t		kernel_stack,
	int		kernel_stack_size,
	caddr_t		stack,
	int		stack_size,
	caddr_t		data,
	int		data_size)
{
	register struct proc *p = port_to_proc_lookup(proc_port);
	register struct uthread *uth = &u;

	/*
	 * Set up server thread context and write the core.
	 * Usually, we would call server_thread_register() but that would
	 * fail if the process is exiting. So, instead, we do the
	 * registration explicitly.
	 */
	uth->uu_procp = 0;
	simple_lock(&p->p_lock);
	p->p_ref_count++;
	queue_enter(&p->p_servers, uth, uthread_t, uu_server_list);
	simple_unlock(&p->p_lock);

	uarea_init(uth, p);

	(void) chkpnt_core_file(p, path, path_len,
				user_area, user_area_size,
				kernel_stack, kernel_stack_size,
				stack, stack_size,
				data, data_size);

	uarea_terminate(uth);
	server_thread_deregister(uth, p);

	/*
	 * Handle the interlock the main server thread: decrement the
	 * signal reference count and wake up that thread if count
	 * reaches 0 - this allows the process to exit if it's waiting.
	 */ 
	unix_master();
	simple_lock(&p->p_lock);
	if (--p->p_exit_hold_count == 0)
		wakeup((caddr_t) &p->p_exit_hold_count);
	simple_unlock(&p->p_lock);
	unix_release();

	vproc_end_port_op(p->p_vproc, "svr_chkpnt_core_file");
	return KERN_SUCCESS;
}

/*
 * This routine receives slabs of virtual address space to be written
 * to disc in core file format.
 */
int
chkpnt_core_file(
	struct proc	*p,
	char		*path,
	unsigned int	path_len,
	caddr_t		user_area,
	int		user_area_size,
	caddr_t		kernel_stack,
	int		kernel_stack_size,
	caddr_t		stack,
	int		stack_size,
	caddr_t		data,
	int		data_size)
{
	mach_port_t	vp = MACH_PORT_NULL;
	struct vattr	vattr;
	int		error;
	struct nameidata *ndp = &u.u_nd;
	enum vtype	type;
	path_name_t	preliminary_path;
	char		*base;
	int		dir_len;

	PROC_LOCK(p);
	crfree(u.u_cred);
	u.u_cred = p->p_rcred;
	crhold(p->p_rcred);
	p->p_rcred->cr_uid = p->p_ruid;
	p->p_rcred->cr_gid = p->p_rgid;
	PROC_UNLOCK(p);

	/*
	 * Construct the preliminary path under which the core file
	 * will be opened and written; it will be renamed to the required
	 * pathname only when complete. This preliminary name adds a '.'
	 * to the start of the basename component of the path.
	 */
	for (base = path + path_len; base != path && *(base-1) != '/'; base--)
		;		/* ends with base being the basename */
	dir_len = base - path;
	if (dir_len != 0)
		bcopy(path, preliminary_path, dir_len);
	preliminary_path[dir_len] = '.';
	bcopy(base, preliminary_path + dir_len + 1, path_len - dir_len);
	
	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = (caddr_t) preliminary_path;
	ndp->ni_forwport = *path == '/' ? ndp->ni_rdirport : ndp->ni_cdirport;

	error = remote_vnopen(ndp, FCREAT|FWRITE, 0600, &vp, &type);
	if (error) {
		goto out;
	}
	if (type != VREG) {
		error = EFAULT;
		goto out;
	}
	error = remote_getattr(vp, &vattr);
	if (error || vattr.va_nlink != 1) {
		if (error == 0)
			error = EFAULT;
		goto out;
	}
	/*
	 * 4.4 has some code here ifdef MMAP to unmap devices.
	 * We have not included that code.
	 */
	vattr_null(&vattr);
	vattr.va_size = 0;
	error = remote_setattr(vp, &vattr);

	/*
	 *	MACH breaks conventional debuggers because the kernel
	 *	stack is no longer at the top of memory.  Dump the forged
	 *	uarea where it is expected, followed by the current
	 *	kernel stack at the end of the UPAGES where the u-area
	 *	used to be.  Since the thread uarea contains the thread pcb
	 *	which contains the kernel stack offset, there is enough
	 *	information to decode what's going on.  This assumes that a
	 *	struct user, and a kernel stack all fit in UPAGES.
	 */
 	error = remote_vnrdwr(UIO_WRITE, vp,
			      (caddr_t)user_area, user_area_size,
			      (off_t)0, IO_UNIT, (int *)0);
	if (error)
		goto out;

	ASSERT(kernel_stack_size < ctob(UPAGES));
	error = remote_vnrdwr(UIO_WRITE, vp, kernel_stack,
			      kernel_stack_size,
			      ctob(UPAGES) - kernel_stack_size,
			      IO_UNIT, (int *)0);
	if (error)
		goto out;

	error = chkpnt_core_write((mach_port_t) vp,
				  data, data_size, 
				  (off_t)ctob(UPAGES));
	if (error)
		goto out;

	error = chkpnt_core_write((mach_port_t) vp,
				  stack, stack_size, 
				  (off_t)ctob(UPAGES) + data_size);
	if (error)
		goto out;

	/*
	 * With all writing done, we can close the core file
	 * and rename it to its intended name.
	 */
	(void) mach_port_deallocate(mach_task_self(), vp);
	vp = MACH_PORT_NULL;
	ndp->ni_dirp = (caddr_t) preliminary_path;
	ndp->ni_forwport = *path == '/' ? ndp->ni_rdirport :
					  ndp->ni_cdirport;
	error = remote_rename(ndp, path, path_len);

out:
	kfree(user_area, user_area_size);
	kfree(kernel_stack, kernel_stack_size);
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) stack, (vm_size_t) stack_size);
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) data, (vm_size_t) data_size);
	if (vp != MACH_PORT_NULL)
		(void) mach_port_deallocate(mach_task_self(), vp);
	return (error);
}

/* Write a section of the task's address space to the file in vp.  We
   should be able to use vn_rdwr with UIO_USERSPACE to do this, but that
   is broken under the OSF/1 server.
   When we read a page from the process, we call it into existence even
   if it was previously just demand-zero.  Since we don't want to need
   backing store for a possibly huge virtual address space, we examine a
   block at a time and deallocate each block from the process after using
   it.  We also optimise space in the core file by creating holes for
   sufficiently large patches of zero bytes.  */
int
chkpnt_core_write(vp, base, len, offset)
	mach_port_t vp;
	caddr_t base;
	int len;
	off_t offset;
{
	int start, error = 0;
#define COREBLOCKSIZE 32768
	ASSERT(round_page(COREBLOCKSIZE) == COREBLOCKSIZE);

	for (start = 0; start < len; start += COREBLOCKSIZE) {
		vm_address_t bottom, top;
		int size = len - start;
		if (size > COREBLOCKSIZE)
			size = COREBLOCKSIZE;
		bottom = (vm_address_t)(base + start);
		top = bottom + size;

		/*
		 * If this is not the last block and it is entirely zero, write
		 * nothing and allow a hole in the core file to take its place.
		 */
		if (start + COREBLOCKSIZE >= len || !is_null(bottom, size))
			error = remote_vnrdwr(UIO_WRITE, vp,
					      bottom, size, offset + start,
					      IO_UNIT, (int *)0);
	}
	return error;
}


/* Return true only if len bytes starting at base are all 0. */
boolean_t
is_null(
	vm_address_t	base,
	int		len)
{
	int *p = (int *) base;
	assert(len % sizeof(int) == 0);
	for (len /= sizeof(int); len > 0; len--)
	    if (*p++)
		return FALSE;
	return TRUE;
}


/*
 * Write given emulator and process state into a file of specified name.
 */ 
int
chkpnt_pproc_state(
	struct proc		*p,
	path_name_t		*state_path,
	chkpnt_proc_state_t	*psp,
	chkpnt_file_state_t	*fsp)
{
	mach_port_t	vp;
	struct vattr	vattr;
	int		error;
	struct nameidata *ndp = &u.u_nd;
	enum vtype	type;
	int		file_offset = 0;

	p = u.u_procp;

/* XXX - required ??? */
	PROC_LOCK(p);
	crfree(u.u_cred);
	u.u_cred = p->p_rcred;
	crhold(p->p_rcred);
	p->p_rcred->cr_uid = p->p_ruid;
	p->p_rcred->cr_gid = p->p_rgid;
	PROC_UNLOCK(p);

	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = (caddr_t) state_path;

	error = remote_vnopen(ndp, FCREAT|FWRITE, 0600, &vp, &type);
	if (error) {
		return error;
	}
	if (type != VREG) {
		error = EFAULT;
		goto out;
	}
	error = remote_getattr(vp, &vattr);
	if (error || vattr.va_nlink != 1) {
		if (error == 0)
			error = EFAULT;
		goto out;
	}
	vattr_null(&vattr);
	vattr.va_size = 0;
	error = remote_setattr(vp, &vattr);

	/*
	 * Firstly, dump the server state
	 */
 	error = remote_vnrdwr(UIO_WRITE, vp,
			      (caddr_t)psp, sizeof(chkpnt_proc_state_t),
			      (off_t)0, IO_UNIT, (int *)0);
	if (error)
		goto out;
	file_offset += sizeof(chkpnt_proc_state_t);

	/*
	 * Secondly, dump the file state - noting that the size of the
	 * file state is a dynamically determined by the number of open files.
	 */
 	error = remote_vnrdwr(UIO_WRITE, vp,
			      (caddr_t)fsp, CHKPNT_FILE_STATE_SIZE(fsp),
			      (off_t)file_offset, IO_UNIT, (int *)0);

out:
	mach_port_deallocate(mach_task_self(), (mach_port_t) vp);
	return (error);
}

/*
 * Read checkpointed file and process state into memory. Process state
 * being read into a supplied buffer, while file (emulator) state is put
 * into a vm_allocated buffer.
 */ 
int
restart_pproc_getstate(
	struct proc		*p,
	path_name_t		*state_path,
	chkpnt_proc_state_t	*psp,
	chkpnt_file_state_t	**fsp)
{
	struct vnode_proxy	*svp;
	int			error;
	int			ret;
	struct nameidata	*ndp = &u.u_nd;
	int			file_offset = 0;
	int			residual;
	int			read_access = ~ESUCCESS;

	ndp->ni_dirp = (caddr_t) state_path;
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg =  UIO_SYSSPACE;
	error = remote_exec_lookup(ndp, &svp);
	if (error != ESUCCESS)
		return(error);
	(void) remote_exec_check_vnode(svp, &read_access);
	if (read_access)
		return(EACCES);

	/*
	 * Firstly, read the server state
	 */
 	error = remote_exec_read(svp, (caddr_t)psp, sizeof(chkpnt_proc_state_t),
				 (off_t)0, IO_UNIT, (int *)0);
	if (error)
		goto out;
	file_offset += sizeof(chkpnt_proc_state_t);

	/*
	 * Secondly, read the file state - noting that the size of the
	 * file state is a dynamically determined by the number of
	 * open files and we vm_allocate a buffer of the required size.
	 */
	ret = vm_allocate(mach_task_self(), (vm_address_t *) fsp,
			  (vm_size_t) psp->fs_size, TRUE);
	if (ret != KERN_SUCCESS)
		goto out;

 	error = remote_exec_read(svp, (caddr_t)*fsp, psp->fs_size,
				 (off_t)file_offset, IO_UNIT, (int *)0);

out:
	remote_vfree(svp);
	return (error);
}

/*
 * Exec a given program, reload statck and data from a core file and
 * reset root and current directory context.
 */
int
restart_pproc_exec(
	struct proc	*procp,
	mach_port_t	new_rootdir_port,
	mach_port_t	new_currentdir_port,
	path_name_t	*fname,
	path_name_t	*image)
{
	int			error;
	int			ret;
	struct vnode_proxy	*ivp;
	register struct nameidata *ndp = &u.u_nd;
	struct user		user_area;
	unsigned int		exec_data_size;
	unsigned int		data_size;
	unsigned int		stack_size;
	vm_offset_t		buffer_addr;
	unsigned int		buffer_size;
	
	/*
	 * Open the image file. We'll read it later but here we're
	 * checking existence.
	 */
	ndp->ni_dirp = (char *) image;
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg =  UIO_SYSSPACE;
	error = remote_exec_lookup(ndp, &ivp);
	if (error != ESUCCESS)
		return(error);

	/*
	 * Use the standard execve() to load the checkpointed image.
	 * Note that the no args or environment is set-up since these
	 * are restored frmo the checkpoint image file below.
	 */
	{
		int arg[3];			/* arguments to execve */
		struct execr {			/* exec return arguments */
			char	*cfname;	/* shell file name */
			char	*cfarg;		/* shell args */
			int	*entry;		/* pointer to pc entry points */
			unsigned int *entry_count; /* number of entries */
		} rtv;
		struct execve_out_parms {	/* values returned by execve */
			char		cfname[8*64];
			char		cfarg[8*64];
			int		entry[16];
			unsigned int	entry_count;
			auxv_mig_t	auxv_structs;
			char		auxv_strings[4096];
		} *dummy_out_parms;

		ret = vm_allocate(mach_task_self(),
				  (vm_address_t *) &dummy_out_parms,
				  sizeof(struct execve_out_parms),
				  TRUE);
		if (ret != KERN_SUCCESS)
			panic("restart_pproc_exec: vm_allocate failure");

		arg[0] = (int)fname;		/* file name for exec */
		arg[1] = 0;			/* handled in the emulator */
		arg[2] = 0;			/* handled in the emulator */

		rtv.cfname = dummy_out_parms->cfname;
		rtv.cfarg = dummy_out_parms->cfarg;
		rtv.entry = dummy_out_parms->entry;
		rtv.entry_count = &dummy_out_parms->entry_count;

		procp->p_auxv_structs = &dummy_out_parms->auxv_structs[0];
		procp->p_auxv_strings = dummy_out_parms->auxv_strings;

		unix_master();
		error = execve(procp, arg, &rtv);
		unix_release();

		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) dummy_out_parms,
				     (vm_size_t) sizeof(*dummy_out_parms));

	}
	if (error != ESUCCESS)
		goto out;

	/*
	 * Read user area from checkpoint file. This contains details
	 * of the data and stack images dump thereafter.
	 */
	error = remote_exec_read(ivp, (caddr_t) &user_area, 
			  	 sizeof(struct user),
				 0, IO_UNIT,
				 (int *)0);
	if (error != ESUCCESS)
		goto fatal;

	/*
	 * Take note of data/stack sizes both now following the exec
	 * and what is required by the restarted process.
	 */
	exec_data_size = ctob(u.u_dsize);
#undef	u_dsize			/* want the real user struct members not */
#undef	u_ssize			/* the macros which reference who-knows-what */
	data_size = ctob(user_area.u_dsize);
	stack_size = ctob(user_area.u_ssize);

	/*
	 * Allocate temporary space for image in server's address space
	 */
	buffer_size = max(data_size, stack_size);
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &buffer_addr, buffer_size, TRUE);
	if (ret !=  KERN_SUCCESS) {
		error = ENOMEM;
		goto fatal;
	}

	/*
	 * Read data area from image file to overwrite user task area.
	 */
	error = remote_exec_read(ivp, (caddr_t) buffer_addr, 
			  	 (int) data_size,
				 ctob(UPAGES), IO_UNIT,
				 (int *)0);
	if (error != ESUCCESS)
		goto fatal;

	/*
	 * Ensure that the data region is large enough before writing
	 * the  checkpointed data into the user task.
	 */
	if (exec_data_size < data_size) {
		caddr_t	addr = u.u_data_start + exec_data_size;
		ret = vm_allocate(procp->p_task,
				  (vm_offset_t *) &addr,
				  (vm_size_t) data_size - exec_data_size,
				  FALSE);
		if (ret != KERN_SUCCESS)
			goto fatal;
	}
	ux_server_thread_blocking();
	ret = vm_write(procp->p_task, (vm_offset_t) u.u_data_start,
		       buffer_addr, data_size);
	ux_server_thread_unblocking();
	if (ret != KERN_SUCCESS)
		goto fatal;

	/*
	 * Read stack area from image file and overwrite user task area.
	 */
	error = remote_exec_read(ivp, (caddr_t) buffer_addr, 
			  	 (int) stack_size,
				 ctob(UPAGES) + data_size, IO_UNIT,
				 (int *)0);
	if (error != ESUCCESS)
		goto fatal;
	ux_server_thread_blocking();
	ret = vm_write(procp->p_task,
		       (vm_offset_t)u.u_stack_end - stack_size,
		       buffer_addr, stack_size);
	ux_server_thread_unblocking();
	if (ret != KERN_SUCCESS)
		goto fatal;

	/*
	 * Restore the current and root directory for this process.
	 */
	error = restart_pproc_setports(new_currentdir_port, new_rootdir_port);
	if (error != ESUCCESS)
		goto fatal;

	(void) vm_deallocate(mach_task_self(), buffer_addr, buffer_size);

out:
	remote_vfree(ivp);
	return(error);

fatal:
	/*
	 * Here if an error occurred after loading the checkpointed image
	 * and while restoring data/stack areas. There's nothing we can
	 * do but blow away the process.
	 */
	remote_vfree(ivp);
	(void) vm_deallocate(mach_task_self(), buffer_addr, buffer_size);
	(void) VPOP_EXIT(procp->p_vproc, W_EXITCODE(error, SIGKILL));
	return(error);
}

/*
 * Borrowed from bsd_setports() to set the root/current directory
 * for the restart process.
 */
int
restart_pproc_setports(
	mach_port_t	cwd_port,
	mach_port_t	root_port)
{
	register int 		error = ESUCCESS;
	uthread_t		uth = &u;
	struct vnode_proxy	*vp;

	/*
	 * The PM keeps exactly one reference to the cwd and root
	 * ports in the u-area.
	 */
	U_HANDY_LOCK();

	VN_LOCK_PROXY(&uth->u_cdirproxy);
	ASSERT(uth->u_cdirproxy.vpx_usecount == 1);
	if (uth->u_cdirport != cwd_port) {
		mach_port_t temp = uth->u_cdirport;
		uth->u_cdirport = cwd_port;
		if (temp) {
			int error = mach_port_deallocate(mach_task_self(), 
							 temp);
			if (error) 
				printf ("bsd_setports: cwd port=0x%x, ret=0x%x\n", temp, error);
		}
		if (uth->u_cdir)
			VNODE_LOOKUP_DONE(uth->u_cdir);

		PORT_TO_VNODE_LOOKUP(cwd_port, uth->u_cdir);
		uth->u_cdirproxy.vpx_usecount = 1;
	} else
		mach_port_deallocate(mach_task_self(), cwd_port);
	VN_UNLOCK_PROXY(&uth->u_cdirproxy);

	VN_LOCK_PROXY(&uth->u_rdirproxy);
	ASSERT(uth->u_rdirproxy.vpx_usecount == 1);
	if (uth->u_rdirport != root_port) {
		mach_port_t temp = uth->u_rdirport;
		uth->u_rdirport = root_port;
		if (temp) {
			int error = mach_port_deallocate(mach_task_self(), 
							 temp);
			if (error) 
				printf ("bsd_setports: root port=0x%x, ret=0x%x\n", temp, error);
		}
		if (uth->u_rdir)
			VNODE_LOOKUP_DONE(uth->u_rdir);

		PORT_TO_VNODE_LOOKUP(cwd_port, uth->u_rdir);
		uth->u_rdirproxy.vpx_usecount = 1;
	} else
		mach_port_deallocate(mach_task_self(), root_port);
	VN_UNLOCK_PROXY(&uth->u_rdirproxy);

	U_HANDY_UNLOCK();

	return(error);
}

/*
 * First set the root/current directories to restart the chkpnted image.
 * Also set the root/current for the chkpnted image.
 */
int
restart_pproc_context(
	struct proc		*procp,
	chkpnt_file_state_t	**fs,
	mach_port_t		*new_rootdir_port,
	mach_port_t		*new_currentdir_port)
{
	int		error;
	mach_port_t	exec_rootdir;
	mach_port_t	exec_currentdir;

	/*
	 * Get root and current dir ports for the exec().
	 */
	error = restart_pproc_get_dir_port((*fs)->exec_rootdir,
					   &exec_rootdir);
	if (error) 
		return(error);
	error = restart_pproc_get_dir_port((*fs)->exec_currentdir,
					   &exec_currentdir);
	if (error) 
		return(error);

	error = restart_pproc_setports(exec_currentdir, exec_rootdir);
	if (error) 
		return(error);

	/* 
	 * Also set the chkpnted process' current and root directory here.
	 */
	error = restart_pproc_get_dir_port((*fs)->rootdir,
					   new_rootdir_port);
	if (error) 
		return(error);
	error = restart_pproc_get_dir_port((*fs)->currentdir,					  			   new_currentdir_port);
	if (error) 
		return(error);
}
		
/*
 * Get the vnodes for a particular directory from
 * a file handle.
 */
int
restart_pproc_get_dir_port(
	file_info_t	fi,
	mach_port_t	*dir_port)
{
	struct fhandle		fh;
	struct vnode		*vp;
	int 			error;
	extern node_t		this_node;
	node_t                  mnt_node;
	mach_port_t		mnt_port;

	/* 
	 * Construct a file handle from chkpnted info and
	 * obtain a vnode pointer for the file.
	 */
	restart_pproc_make_fh(&fi, &fh);
	mnt_node =  FSID1_2_NODE(fh.fh_fsid);

	/*
	 * First check to see if this is the file server node.
	 * If this is, just get the directory port here.  Else
	 * do it on the file server node.
	 */
	if (this_node == mnt_node) {
		error = vn_fhtovp(&fh, 0, (struct vnode **) &vp);
		/*
	         * Make sure the file has not been unlinked or unmounted.
	 	 * When error is ESTALE, vp should be NULL.  But return.
	 	 */
		if (error == ESTALE) 
			return(error);

		/* 
	 	 * From vp, try to obtain the file port.
	  	 */
		get_vnode_port(vp, dir_port);

		return(ESUCCESS);
	}

	/*
	 * If the file system does not reside on this node,
	 * find the dir_port on the file system node.
	 */
	mnt_port = node_to_fileserver_port(mnt_node);
	if (mnt_port != MACH_PORT_NULL)
		return(cli_tnc_get_dir_port(mnt_port, fh, dir_port));
	else
		return(EINVAL);
}

int
restart_pproc_make_fh(
	file_info_t	*fip,
	struct fhandle	*fhp)
{
	struct ufid 	*ufidp;

	ufidp = (struct ufid *) &(fhp->fh_fid);
	fhp->fh_fsid = fip->fs_or_dev.fstatfs.f_fsid; 

	/*
	 * NOTE: ufid_len and ufid_pad is based on past
	 * observation, will need more accurate count.
	 */
	ufidp->ufid_len = 12;
	ufidp->ufid_pad = 0;

	ufidp->ufid_ino = fip->fstat.st_ino;
	ufidp->ufid_gen = fip->fstat.st_gen;
}

int
svr_tnc_get_dir_port(mach_port_t root_fs_port,
		  struct fhandle fh, mach_port_t *dir_port)
{
	int error = 0;
	struct vnode		*vp;

	error = vn_fhtovp(&fh, 0, (struct vnode **) &vp);
	/*
	 * Make sure the file has not been unlinked or unmounted.
	 *  When error is ESTALE, vp should be NULL.  But return.
	 */
	if (error == ESTALE) return(error);

	/* 
	 * From vp, try to obtain the file port.
	 */
	get_vnode_port(vp, dir_port);
	return(ESUCCESS);
}
