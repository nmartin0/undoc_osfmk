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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: kern_mman.c,v $
 * Revision 1.10  1994/12/20  22:53:55  suri
 *  Reviewer jlitvin
 *  Risk: Low
 *  Benefit or PTS #: 11640
 *  Testing: Specific Testcase
 *  Module(s): Backing out the fix for PTS-11270/11317/10593 (vm_allocate/sbrk
 *  changes), as it was causing excessive wiring down of memory on the compute
 *  nodes under some specific cases (the testcase for PTS-11640 is one of them).
 *  The vm_allocate/sbrk changes have to be reimplemented, perhaps using
 *  vm_reserve().
 *
 * Revision 1.9  1994/11/30  22:52:40  jlitvin
 * The server prints confusing messages to the user if sbrk() fails.
 * These messages seem more appropriate for the debug server than the
 * default one.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11718
 *  Testing: developer
 *  Module(s): server/bsd/kern_mman.c
 *
 * Revision 1.8  1994/11/18  20:27:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/10/25  23:34:07  suri
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 11317
 *  Testing: Specific testcase, fileio, pthreads, xtrnl, NQS/MACs EATs
 *  Module(s): obreak() in server/bsd/kern_mman.c
 * 	    setrlimit() in server/bsd/kern_resource.c
 * 	    coff_getxfile() in server/bsd/kern_exec.c
 * 	    user struct in server/sys/user.h
 * 	    rf_data struct in server/tnc/rtask.h
 * 	    rfork_pproc_load_msg() in server/tnc/rtask_cli_pproc.c
 * 	    rfork_pproc_unload_msg() in server/tnc/rtask_svr_pproc.c
 *
 * Revision 1.6  1994/10/11  18:38:14  suri
 *  Reviewer: cfj
 *  Risk: H
 *  Benefit or PTS #: 10593
 *  Testing: Specific testcase; fileio, message_passing, xtrnl,
 *           PFS and pthreads EATs
 *  Module(s): e_obreak() in emulator/emul_mapped.c
 *             coff_getxfile() in server/bsd/kern_exec.c
 *             obreak() in server/bsd/kern_mman.c
 *             setrlimit() in server/bsd/kern_resource.c
 *
 * Revision 1.5  1994/01/13  17:53:46  jlitvin
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
 * Revision 1.4  1993/07/14  17:47:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:46:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:03:09  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:15:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:05:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:06:38  cfj
 * Bump major revision number.
 *
 * Revision 2.13  1992/09/11  09:26:35  rabii
 * 	Print meaningful message when sbrk() fails.
 * 
 * Revision 2.12  92/05/24  14:13:58  pjg
 * 	92/03/23  18:02:08  condict
 * 	Add ux_server_thread_blocking/unblocking calls around the vm_map call,
 * 	since it sends messages to the vnode_pager, which can be blocked.
 * 
 * 	92/03/20  15:08:08  jose
 * 	Added code to handle obreak (not used yet)
 * 
 * 	92/03/05  12:52:18  sp
 * 	Delete msync on munmap. Remember if regions are private for msync.
 * 
 * Revision 2.11  92/05/18  12:29:22  roy
 * 	OSF1_ADFS version of smmap() now uses node numbers instead of 
 * 	fileserver ports.  Undid change in previous revision.
 * 	[92/05/16            roy]
 * 
 * Revision 2.10  92/05/01  10:18:09  rabii
 * 	Free the pager port in mmap for OSF1_ADFS.
 * 	[92/04/29            roy]
 * 
 * Revision 2.9  92/03/20  11:40:18  pjg
 * 	creds_port no longer passed as arg in smmap call. Enable 
 * 	master_unlock/lock around vm_map() for osf1_adfs too (durriya)
 * 
 * Revision 2.8  92/03/15  14:40:14  roy
 * 	added support for mmap of anonymous memory to be backed by a 
 * 	(remote) vnode pager  (durriya)
 * 
 * 	92/02/27  21:08:12  jose
 * 	Changed the condition for setting copy true in vmap
 * 	to allow the vnode_pager pageout anonymous memory
 * 	[bug 98].
 * 
 * 	92/02/19  16:22:31  emcmanus
 * 	Included kern/parallel.h, for the case where we are compiling without
 * 	options such as afs that include it indirectly.
 * 
 * 	92/02/18  18:56:53  jose
 * 	Added master lock release in smmap to prevent deadlock with vnode pager
 * 	and shared libraries (bug 96), but this is not a general solution.
 * 
 * 	91/12/20  14:32:32  jose
 * 	Corrected the way VM_MMAP_ANONYMOUS is set
 * 
 * 	91/12/19  15:58:54  bernadat
 * 	Fixed the call to device_pager_create (arguments mismatch)
 * 	for mmap on character devices.
 * 
 * 	91/12/17  17:18:42  jose
 * 	Changed mmap to also use the vnode pager
 * 	when mapping anonymous memory.
 * 
 * Revision 2.7  92/03/09  14:07:03  durriya
 * 	deallocate file port on success in smmap. 
 *	Don't vm_msynch in munmap (roy)
 * 
 * Revision 2.6  92/03/01  19:28:13  pjg
 * 	Got mmap working for the AD server (durriya).
 * 
 * Revision 2.4  91/12/17  16:42:51  roy
 * 	91/12/02  16:58:56  sp
 * 	sync regions before deallocating them in mmap
 * 
 * 	91/10/30  09:16:51  sp
 * 	Call mach_port_deallocate() with correct arguments in smmap (Bug 23)
 * 
 * 	91/10/24  09:04:05  sp
 * 	register and deregister mapped region information (smmap/munmap)
 * 	When mapping a region, allow the kernel to cache
 * 
 * 	91/10/23  16:37:44  condict
 * 	Remove ifndefs that were commenting out the BSD semaphore sys calls.
 * 
 * 	91/10/15  12:15:38  sp
 * 	Add mmap keep on exec support
 * 	make msync work
 * 
 * 	91/10/11  16:45:57  emcmanus
 * 	Fixed and tested plock() system call.
 * 
 * Revision 2.3  91/10/15  11:25:42  roy
 * Fix revision history comments.
 * 
 * Revision 2.2  91/10/14  12:07:08  sjs
 *	Initial revision.
 * 
 * Revision 3.0  91/09/13  12:46:22  sp
 * Add support for mmap
 * 
 * Revision 1.16.4.3  91/02/26  14:13:16  jeffc
 * 	Fixed plock to only unlock the locked regions.
 * 	[91/02/14  13:07:51  jeffc]
 * 
 * Revision 1.16.4.2  91/02/06  17:16:02  jeffc
 * 	Fix obreak() data size calculation so that resource limits can't
 * 	be bypassed by causing an overflow.
 * 	[91/01/08  15:42:56  jeffc]
 * 
 * Revision 1.16  90/10/31  13:49:02  devrcs
 * 	Change the protection to VM_PROT_READ rather than read|execute in plock
 * 	for the text area. This is because the read only data now gets lumped
 * 	in with the text size but does not have execute protection in the vm
 * 	map entry
 * 	[90/10/25  17:10:36  sp]
 * 
 * 	Correct return value in the getaddressconf system call.
 * 	[90/10/18  15:59:11  jvs]
 * 
 * 	change madvise to check for invalid behav argument
 * 	[90/10/15  13:32:44  garyf]
 * 
 * 	Fix bug 1302 -- need to get max protections from file open modes when
 * 	mapping a file shared.
 * 	[90/10/12  14:12:42  lwa]
 * 
 * 	Refixed conformance bugs (bug reports 413, 531, 563), as the changes
 * 	somehow got lost.
 * 	[90/10/10  09:11:47  collins]
 * 
 * Revision 1.15  90/10/07  13:17:23  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:55:58  gm]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:33:05  jeffc]
 * 
 * Revision 1.14  90/09/23  15:43:00  devrcs
 * 	Round the arguments to vm_map_pageable to page boundaries.
 * 	If data lock fails and we were doing a PROC_LOCK then unlock the
 * 	text segment.
 * 	[90/09/10  13:40:16  sp]
 * 
 * 	Fixed some conformance problems, bug reports 413, 531, 563.
 * 	[90/09/10  10:45:22  collins]
 * 
 * 	ANSI casting fix
 * 	[90/09/05  13:27:35  rossi]
 * 
 * Revision 1.13  90/08/24  11:16:28  devrcs
 * 	Changes for new system call interface
 * 	Removed include of syscontext.h
 * 	Changes for u_file_state
 * 	[90/08/17  17:36:53  nags]
 * 
 * Revision 1.12  90/08/09  13:13:55  devrcs
 * 	Added the msync system call.
 * 	[90/07/24  15:17:07  havens]
 * 
 * Revision 1.11  90/07/27  08:43:56  devrcs
 * 	Remove ofile array from getvnode interface.
 * 	[90/07/20  13:49:37  nags]
 * 
 * Revision 1.10  90/07/17  11:18:39  devrcs
 * 	Added mwakeup and msleep calls for msem_lock and msem_unlock.
 * 	Real code is in vm/vm_unix.c
 * 	[90/07/04  14:44:30  havens]
 * 
 * Revision 1.9  90/06/22  20:05:59  devrcs
 * 	nags merge
 * 	[90/06/12  19:05:57  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:39:44  seiden]
 * 
 * 	Add new getaddrconf() system call.  Use address-configuration record
 * 	in mmap to determine system default mapping address.
 * 	[90/06/06  09:32:06  lwa]
 * 
 * 	Add support for MAP_INHERIT (keep-on-exec).
 * 	Add new mvalid() routine.
 * 	Be more careful about mapping UNIX protections to Mach protections.
 * 	[90/06/04  15:58:29  lwa]
 * 
 * 	Condensed history (reverse chronology):
 * 	Updated for OSF/1 parallelization.		nags@encore.com
 * 	Fix unitialized variable in mmap		lwa@osf.org
 * 	Added plock(); now use syscontext structure.	ers@osf.org
 * 	Make getpagesize() return Mach VM page size.  	lwa@osf.org
 * 	Add MAP_UNALIGNED option to mmap()		lwa@osf.org
 * 	Added set_program_attributes() system call.  	kwallace@osf.org
 * 	Can't FP_UNREF an anonymous memory request.	kwallace@osf.org
 * 	File layer parallelization changes.		noemi@osf.org
 * 	Removed include of <sys/dir.h>			lwa@osf.org
 * 	AES needs file write access only on shared map.	lwa@osf.org
 * 	Conflicting mappings cause MAP_FIXED to fail.	lwa@osf.org
 * 	Anonymous memory regions, MAP_FIXED, mprotect.	lwa@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Change to use 4.4BSD vnodes.			ers@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.			gm@osf.org
 * 	Merged Mach 2.5 with Encore parallelization	alan@encore.com
 * 	Changed grow() to do an unsigned comparison	jjc@cmu.edu
 * 	Removed inclusion of psl.h for mips too.	af@cmu.edu
 * 	Enclosed modifications to smmap for Suntools 	mikeg@cmu.edu
 * 	Mark memory map MAP_PRIVATE when calling winmap	mikeg@cmu.edu
 * 	MACH_XP: Get the right offset for objects 	mwyoung@cmu.edu
 * 	Correct mmap() to copy data for MAP_PRIVATE	rpd@cmu.edu
 * 	Corrected include file references.		mwyoung@cmu.edu
 * 	Eliminate old variables in smmap.		mwyoung@cmu.edu
 * 	Use memory_object_special when not MACH_XP	mwyoung@cmu.edu
 * 	Simplified mmap() through use of vm_map().	mwyoung@cmu.edu
 * 	Use new memory object types.			mwyoung@cmu.edu
 * 	Use new memory object types.			mwyoung@cmu.edu
 * 	Print the name of the process that can't sbrk.	mwyoung@cmu.edu
 * 	Correct fix to return proper U*X return code.	dbg@cmu.edu
 * 	smmap only works on local inodes.		sanzi@cmu.edu
 * 	Changed inode_pager_setup() to new interface	mwyoung@cmu.edu
 * 	MACH_XP: Turned off device file mmaping.	bolosky@cmu.edu
 * 	Made mmap handle both special devices and files	dbg@cmu.edu
 * 	[90/06/07  16:55:14  lwa]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_mman.c	7.1 (Berkeley) 6/5/86
 */

/* TODO:
 *	msync, madvise, mincore
 *	add support for PROT_NONE
 *	some error status returns are still suspect
 */

#include <sys/secdefines.h>
#include <sys/security.h>

#include <mach_xp.h>
#include <machine/reg.h>
#if	!defined(ibmrt) && !defined(mips)
#include <machine/psl.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/map.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <ufs/inode.h>
#include <sys/acct.h>
#include <sys/wait.h>
#ifndef	OSF1_SERVER
#include <sys/vm.h>
#endif
#include <sys/file.h>
#include <sys/lock.h>
#include <sys/vadvise.h>
#include <sys/trace.h>
#include <sys/mman.h>
#include <sys/conf.h>
#include <sys/addrconf.h>
#include <machine/addrconf.h>
#include <mach/kern_return.h>
#include <kern/sched_prim.h>
#ifndef	OSF1_SERVER
#include <kern/task.h>
#include <mach/vm_param.h>
#include <vm/vm_map.h>
#else	/* OSF1_SERVER */
#include <uxkern/vm_param.h>
#include <kern/parallel.h>
#endif	/* OSF1_SERVER */
#include <mach/memory_object.h>
#ifndef	OSF1_SERVER
#include <builtin/inode_pager.h>
#if	MACH_XP
#include <builtin/device_pager.h>
#endif
#endif	/* OSF1_SERVER */

static vm_prot_t vm_prots[] = {
	VM_PROT_NONE,
	VM_PROT_READ,
	VM_PROT_READ|VM_PROT_WRITE,
	VM_PROT_READ|VM_PROT_WRITE,
	VM_PROT_READ|VM_PROT_EXECUTE,
	VM_PROT_READ|VM_PROT_EXECUTE,
	VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE,
	VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE
};

#define MAX_PROT (PROT_EXEC|PROT_WRITE|PROT_READ)

/* ARGSUSED */
sbrk(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int	incr;
	} *uap = (struct args *)args;

	/* Not yet implemented */
	return (EOPNOTSUPP);

}

/* ARGSUSED */
sstk(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int	incr;
	} *uap = (struct args *)args;

	/* Not yet implemented */
	return (EOPNOTSUPP);


}

/* ARGSUSED */
getpagesize(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	*retval = PAGE_SIZE;
	return (0);
}


#ifdef 	OSF1_ADFS

smmap(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	/*
	 *	Map in special device (must be SHARED) or file
	 */
	register struct args {
                mach_port_t fport;
		caddr_t	addr;
		size_t	len;
		int	prot;
		int	flags;
		off_t	pos;

	} *uap = (struct args *)args;
	/*
	 *	Map in special device (must be SHARED) or file
	 */

	mach_port_t	user_map = u.uu_procp->p_task;
	kern_return_t	result;
	vm_offset_t	user_addr = (vm_offset_t) uap->addr;
	vm_size_t	user_size = (vm_size_t) uap->len;
	off_t		file_pos = uap->pos;
	vm_offset_t	pager_offset;
	vm_prot_t	user_prot;
	vm_prot_t	max_prot;
	int		mapshare = (uap->flags & (MAP_SHARED|MAP_PRIVATE));
					/* For winmmap kluge */
	int		maptype = (uap->flags & MAP_TYPE);
	int		error = 0;
	boolean_t	anywhere;
	enum vtype	vtype;
	int		fd_flag;
	memory_object_t	pager = MEMORY_OBJECT_NULL;
        node_t		node;

	if ((uap->prot < 0) || (uap->prot > MAX_PROT))
		return (EINVAL);
	user_prot = vm_prots[uap->prot];

	/* Check the flags argument. */
	if (((uap->flags & MAP_SHARED) && (uap->flags & MAP_PRIVATE)) ||
	     (uap->flags & (MAP_SHARED|MAP_PRIVATE) == 0))
		return (EINVAL);

	/* Impossible to check both MAP_FIXED and MAP_VARIABLE being defined */
	/* Impossible to check both MAP_FILE and MAP_VARIABLE being defined. */

	/* Round the end addresses to the nearest page boundary. */
	anywhere = (uap->flags & MAP_FIXED) ? FALSE : TRUE;

	if (!anywhere && !page_aligned(user_addr))
		return (EINVAL);
	if (!(uap->flags & MAP_UNALIGNED) && !page_aligned(file_pos))
		return (EINVAL);
	user_size = round_page(user_size);

	/*
	 * See if default address (addr = 0).  Use the address
	 * configuration record: read-only executable file mappings to
	 * MMAP_TEXT, other file mappings to MMAP_DATA, anon mappings to
	 * MMAP_BSS.
	 */
	if (user_addr == (vm_offset_t)0) {
		int     kind;

		if (maptype == MAP_FILE) {

			if (user_prot == (VM_PROT_READ|VM_PROT_EXECUTE))
				kind = AC_MMAP_TEXT;
			else
				kind = AC_MMAP_DATA;
		} else 
			kind = AC_MMAP_BSS;

		user_addr = (vm_offset_t)(addressconf[kind].ac_base);
	}

	if (maptype == MAP_ANON) {
		/*
		 * anonymous memory -- fport must be MACH_PORT_NULL.  
                 * Maximum protection will be ALL.
		 */
		if (uap->fport != MACH_PORT_NULL)
			return (EBADF);

                remote_vnode_pager_get(FALSE, user_size,
				       &pager, &node);
                
		pager_offset = (vm_offset_t) file_pos;
		max_prot = VM_PROT_ALL;

	} else  {
                if (error = remote_inode_pager_setup_fport(uap->fport, 
                                                           FALSE, TRUE,
                                                           &vtype,
                                                           &fd_flag, &pager,
                                                           &node))
                    goto err_out;

		/*
		 * Maximum protections are based on file descriptor protections
		 * for shared mapping; for private mapping, all access allowed.
		 * However, file must be opened for reading even for private
		 * mapping.
		 */
		if ((uap->flags & (MAP_SHARED|MAP_PRIVATE)) == MAP_SHARED) {

			max_prot = VM_PROT_NONE;
			if (fd_flag & FREAD)
				max_prot |= VM_PROT_READ | VM_PROT_EXECUTE;
			if (fd_flag & FWRITE)
				max_prot |= VM_PROT_WRITE;
		} else {

			if (!(fd_flag & FREAD)) {
				error = EACCES;
				goto err_out;
			}
			max_prot = VM_PROT_ALL;
		}
				
		/* Requested protections must be within maximums */

		if ((user_prot & ~max_prot) != VM_PROT_NONE) {
			error = EACCES;
			goto err_out;
		}

		if (vtype == VCHR) {
                        error = EOPNOTSUPP;
                        goto err_out;
                        /* 
                         * for now, mmap of character special files is not
                         * supported
			 */
                } else if (vtype != VREG) {
                        /*
                         *	Map in a file.  May be PRIVATE (copy-on-write)
                         *	or SHARED (changes go back to file)
                         */
                        
                        /*
                         *	Only allow regular files for the moment.
                         */
                        error = ENODEV;
                        goto err_out;
                }
                   
                pager_offset = (vm_offset_t) file_pos;
        }
                    
	/* Map the object */

	ux_server_thread_blocking();   	/* may interact with vnode pager */
	result = vm_map(user_map, &user_addr, user_size, 0, anywhere,
			pager, pager_offset,
			(mapshare == MAP_PRIVATE && maptype != MAP_ANON),
			user_prot,
			max_prot,
			((mapshare == MAP_SHARED) ?
			 VM_INHERIT_SHARE : VM_INHERIT_COPY)
			);
	ux_server_thread_unblocking();

	if (result == KERN_SUCCESS)
          *retval = (int)user_addr;
	else {
                switch (result) {
                        
                      case KERN_NO_SPACE:
                      case KERN_INVALID_ADDRESS:
                        error = ENOMEM;
                        break;
                        
                      case KERN_MEMORY_FAILURE:
                      case KERN_MEMORY_ERROR:
                        error = EIO;
                        break;
                        
                      default:
                        error = EINVAL;
                        break;

                }
        }

	/*
	 * We must hold onto the pager port right because it's needed at
	 * vm_msync() time.  This is unfortunate because it requires that
	 * the right be duplicated at fork time and deallocated at exit time.
	 */

	if (result == KERN_SUCCESS) {
		int flags = 0;
		if (uap->flags & MAP_INHERIT)
			flags |= VM_MMAP_KEEP_ON_EXEC;

		if (uap->flags & MAP_PRIVATE)
			flags |= VM_MMAP_PRIVATE;

		if (pager && (uap->flags & MAP_ANON))
			flags |= VM_MMAP_ANONYMOUS;

		result = vm_mmap_add(u.uu_procp, user_addr, user_size, pager, 
				      node, (vm_offset_t)uap->pos, flags);
		if (result == KERN_NO_SPACE) 
			error = ENOMEM;
        }        

err_out:
	if (error) {
		if (pager != MEMORY_OBJECT_NULL)
			mach_port_deallocate(mach_task_self(), pager);
	} else
		/* consume on success */
		mach_port_deallocate(mach_task_self(), uap->fport);

	return (error);
}

#else /* OSF1_ADFS */
/* ARGSUSED */
smmap(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	/*
	 *	Map in special device (must be SHARED) or file
	 */
	register struct args {
		caddr_t	addr;
		size_t	len;
		int	prot;
		int	flags;
		int	fd;
		off_t	pos;

	} *uap = (struct args *)args;
	struct file *fp;
	register struct vnode *vp;
#ifndef	OSF1_SERVER
	vm_map_t	user_map = current_task()->map;
#else
	mach_port_t	user_map = u.uu_procp->p_task;
#endif
	kern_return_t	result;
	vm_offset_t	user_addr = (vm_offset_t) uap->addr;
	vm_size_t	user_size = (vm_size_t) uap->len;
	off_t		file_pos = uap->pos;
	memory_object_t	pager;
	vm_offset_t	pager_offset;
	vm_prot_t	user_prot;
	vm_prot_t	max_prot;
	int		mapshare = (uap->flags & (MAP_SHARED|MAP_PRIVATE));
					/* For winmmap kluge */
	int		maptype = (uap->flags & MAP_TYPE);
	int		error = 0;
	struct ufile_state *ufp = &u.u_file_state;
	boolean_t	anywhere;
	enum vtype	vtype;
	int		flag, ret;
#if	MACH_XP
#else
	extern memory_object_t	memory_object_special();
#endif
	struct file	*oldfp;

	if ((uap->prot < 0) || (uap->prot > MAX_PROT))
		return (EINVAL);
	user_prot = vm_prots[uap->prot];

	/* Check the flags argument. */
	if (((uap->flags & MAP_SHARED) && (uap->flags & MAP_PRIVATE)) ||
	     (uap->flags & (MAP_SHARED|MAP_PRIVATE) == 0))
		return (EINVAL);
	/* Impossible to check both MAP_FIXED and MAP_VARIABLE being defined */
	/* Impossible to check both MAP_FILE and MAP_VARIABLE being defined. */

	/* Round the end addresses to the nearest page boundary. */
	anywhere = (uap->flags & MAP_FIXED) ? FALSE : TRUE;

	if (!anywhere && !page_aligned(user_addr))
		return (EINVAL);
	if (!(uap->flags & MAP_UNALIGNED) && !page_aligned(file_pos))
		return (EINVAL);
	user_size = round_page(user_size);

	/*
	 * See if default address (addr = 0).  Use the address
	 * configuration record: read-only executable file mappings to
	 * MMAP_TEXT, other file mappings to MMAP_DATA, anon mappings to
	 * MMAP_BSS.
	 */
	if (user_addr == (vm_offset_t)0) {
		int     kind;

		if (maptype == MAP_FILE) {

			if (user_prot == (VM_PROT_READ|VM_PROT_EXECUTE))
				kind = AC_MMAP_TEXT;
			else
				kind = AC_MMAP_DATA;
		} else 
			kind = AC_MMAP_BSS;

		user_addr = (vm_offset_t)(addressconf[kind].ac_base);
	}

	if (maptype == MAP_ANON) {
	

		/* anonymous memory -- fd must be -1.  No inode; use
		 * MEMORY_OBJECT_NULL.  Maximum protection will
		 * be ALL.
		 */

		if (uap->fd != -1)
			return (EBADF);

		fp = NULL;
		vp = NULL;
#ifndef	OSF1_SERVER
		pager = MEMORY_OBJECT_NULL;
#else
		/* using vnode_pager to backup anonynous memory */
		pager = vnode_pager_get(FALSE, user_size);
#endif
		pager_offset = (vm_offset_t) file_pos;
		max_prot = VM_PROT_ALL;

	} else  {
		/*
		 * Only work with local inodes.  Do not use getinode - it will
		 * try to invoke remote filesystem code, which will panic on
		 * smmap.
		 */

		if (error = getvnode(uap->fd, &fp))
			return (error);

		vp = (struct vnode *)fp->f_data;
		
		/*
		 * Maximum protections are based on file descriptor protections
		 * for shared mapping; for private mapping, all access allowed.
		 * However, file must be opened for reading even for private
		 * mapping.
		 */
		BM(FP_LOCK(fp));
		flag = fp->f_flag;
		BM(FP_UNLOCK(fp));	
		if ((uap->flags & (MAP_SHARED|MAP_PRIVATE)) == MAP_SHARED) {

			max_prot = VM_PROT_NONE;
			if (flag & FREAD)
				max_prot |= VM_PROT_READ | VM_PROT_EXECUTE;
			if (flag & FWRITE)
				max_prot |= VM_PROT_WRITE;
		} else {
			if (!(flag & FREAD)) {
				error = EACCES;
				goto out;
			}
			max_prot = VM_PROT_ALL;
		}
				
		/* Requested protections must be within maximums */

		if ((user_prot & ~max_prot) != VM_PROT_NONE) {
			error = EACCES;
			goto out;
		}

		BM(VN_LOCK(vp));
		vtype = vp->v_type;
		BM(VN_UNLOCK(vp));
		if (vtype == VCHR) {
			/*
			 *	Map in special device memory.
			 *	Must be shared, since we can't assume that
			 *	this memory can be made copy-on-write.
			 */
			int		off;
			dev_t		dev;
			int		(*mapfun)();
			extern int	nodev();
			extern int	nulldev();
#ifdef	sun		/* Really, dependent on Suntools */
			extern int	winmmap(); /* Suntools kluge */
#endif
			
			dev = vp->v_rdev;	/* XXX */
			mapfun = cdevsw[major(dev)].d_mmap;	/* XXX */
loop:
			if (mapfun==nulldev || mapfun==nodev || mapfun==0) {
				error = ENODEV;
				goto out;
			}
			
#ifdef	sun
			
			/*
			 * The following modification is required for running
			 * Suntools.  If this is a call to winmmap, the
			 * Suntools task is trying to map in a data structure
			 * in the window driver.  That is, it is using the
			 * mmap call to map some kernel memory into its address
			 * space!! This klugey use of mmap breaks the Mach VM
			 * system, but the problem can be avoided by simply
			 * not having the memory marked as shared.
			 */
			if( mapfun == winmmap )
				mapshare = MAP_PRIVATE;
#endif
			for (off = 0; off < uap->len; off += NBPG) {
				int	(*oldmapfun)() = mapfun;

				mapfun = cdevsw[major(dev)].d_mmap; /* XXX */
				if (oldmapfun != mapfun)
					goto loop;
				CDEVSW_MMAP(major(dev), dev, file_pos+off, uap->prot, ret);
				if (ret == -1) {
					error = ENXIO;
					goto out;
				}
			}
			
			if ((uap->flags & (MAP_SHARED|MAP_PRIVATE)) != MAP_SHARED) {
				error = EINVAL;
				goto out;
			}
			    
#if	MACH_XP
			pager_offset = (vm_offset_t) file_pos;
			pager = device_pager_create(dev, pager_offset,
						    user_size, uap->prot);
#else
			pager = memory_object_special(dev, mapfun, uap->prot,
						      (vm_offset_t) file_pos, user_size);
			pager_offset = 0;
#endif
#ifndef	OSF1_SERVER
			if (pager == PORT_NULL)
#else
			if (pager == MACH_PORT_NULL)
#endif
			{
				error = EINVAL;
				goto out;
			}

		} else {
			/*
			 *	Map in a file.  May be PRIVATE (copy-on-write)
			 *	or SHARED (changes go back to file)
			 */
				
			/*
			 *	Only allow regular files for the moment.
			 */
			if (vtype != VREG) {
				error = ENODEV;
				goto out;
			}
				
			if ((pager = inode_pager_setup(vp, FALSE, TRUE))
			    == MEMORY_OBJECT_NULL) {
				error = EINVAL;
				goto out;
			}
			pager_offset = (vm_offset_t) file_pos;
		}
			
	}

	/* Map the object */

#ifdef	OSF1_SERVER
	ux_server_thread_blocking();	/* We can block in vm_* calls,
					 * if pager is accessing remote
					 * files over network
					 */
#endif	OSF1_SERVER
	result = vm_map(user_map, &user_addr, user_size, 0, anywhere,
			pager, pager_offset,
			(mapshare == MAP_PRIVATE && maptype != MAP_ANON),
			user_prot,
			max_prot,
			((mapshare == MAP_SHARED) ?
			 VM_INHERIT_SHARE : VM_INHERIT_COPY)
			);
#ifdef	OSF1_SERVER
	ux_server_thread_unblocking();
#endif
	if (result == KERN_SUCCESS)
		*retval = (int)user_addr;
	else switch (result) {

	      case KERN_NO_SPACE:
	      case KERN_INVALID_ADDRESS:
		error = ENOMEM;
		break;

	      case KERN_MEMORY_FAILURE:
	      case KERN_MEMORY_ERROR:
		error = EIO;
		break;

	      default:
		error = EINVAL;
		break;

	}

#ifndef	OSF1_SERVER
	/*
	 *	Handle MAP_INHERIT (keep-on-exec) regions here
	 */
	if ((result == KERN_SUCCESS) && (uap->flags & MAP_INHERIT))
		(void) vm_keep_on_exec(user_addr, user_size);
#else
	{
	int flags = 0;
	if (uap->flags & MAP_INHERIT)
			flags |= VM_MMAP_KEEP_ON_EXEC;

	if (uap->flags & MAP_PRIVATE)
			flags |= VM_MMAP_PRIVATE;

	if (pager && (uap->flags & MAP_ANON))
			flags |= VM_MMAP_ANONYMOUS;

	(void) vm_mmap_add(u.uu_procp, user_addr, user_size, pager,
			   (vm_offset_t)uap->pos, flags);
	}
#endif

	/*
	 *	Throw away references to the appropriate pager
	 */

	if (maptype != MAP_ANON) {

		if (vtype == VCHR) {
#if	MACH_XP
#ifndef	OSF1_SERVER
			port_release(pager);
#else
			mach_port_deallocate(mach_task_self(), pager);
#endif
#else
			vm_object_t	object = vm_object_lookup(pager);
			
			if (object != VM_OBJECT_NULL) {
				/* Once for the lookup, once for real */
				vm_object_deallocate(object);
				vm_object_deallocate(object);
			}
#endif
		} else {
			inode_pager_release(pager);
		}

		if (result != KERN_SUCCESS) {
			goto out;
		}
		
		/*
		 * HACK attack.  XXX
		 * If another thread in this task re-allocated the file
		 * descriptor slot, blow him away.  Probably should free
		 * up all resources and return an error to this thread,
		 * or better yet mark the descriptor slot as reserved at
		 * the beginning.
		 */
		oldfp = NULL;
		U_FDTABLE_LOCK(ufp);
		if (ufp->uf_ofile[uap->fd] != fp) {
			oldfp = ufp->uf_ofile[uap->fd];
			ufp->uf_ofile[uap->fd] = fp;
		}
		ufp->uf_pofile[uap->fd] |= UF_MAPPED;
		U_FDTABLE_UNLOCK(ufp);
		if (oldfp)
			FP_UNREF(oldfp);
	}
#ifdef	OSF1_SERVER		/* using vnode_pager to backup anonynous memory */
	else {
		inode_pager_release(pager);
	}
#endif

out:
	if (fp)
		FP_UNREF(fp);
	return (error);

}

#endif	/* OSF1_ADFS */


/* ARGSUSED */
mremap(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*addr;
		int	len;
	} *uap = (struct args *)args;

	/* Not yet implemented */
	return (EOPNOTSUPP);
}

/* ARGSUSED */
munmap(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		caddr_t	addr;
		size_t	len;
	} *uap = (struct args *)args;
	vm_offset_t	user_addr;
	vm_size_t	user_size;
	kern_return_t	result;

	user_addr = (vm_offset_t) uap->addr;
	user_size = (vm_size_t) uap->len;
	/*
	 *	We bend a little - round the end addresses
	 *	to the nearest page boundary.
	 */
	if (!page_aligned(user_addr))
		return (EINVAL);
	user_size = round_page(user_size);
#ifndef	OSF1_SERVER
	result = vm_deallocate(current_task()->map, user_addr, user_size);
#else	/* OSF1_SERVER */
	result = vm_deallocate(u.uu_procp->p_task, user_addr, user_size);
	vm_mmap_delete(u.uu_procp, user_addr, user_size);
#endif	/* OSF1_SERVER */
	if (result != KERN_SUCCESS)
		return (EINVAL);
	return(result);
}

#ifdef notdef
munmapfd(fd)
{
	u.u_pofile[fd] &= ~UF_MAPPED;
}
#endif

/* ARGSUSED */
mprotect(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		caddr_t	addr;
		size_t	len;
		int	prot;
	} *uap = (struct args *)args;
#ifndef	OSF1_SERVER
	vm_map_t map = current_task()->map;
#endif
	vm_offset_t	user_addr;
	vm_size_t	user_size;
	kern_return_t	result;
	vm_prot_t	user_prot;

	if ((uap->prot < 0) || (uap->prot > MAX_PROT))
		return (EINVAL);
	user_prot = vm_prots[uap->prot];
	user_addr = (vm_offset_t) uap->addr;
	user_size = (vm_size_t) uap->len;
	/*
	 *	We bend a little - round the end addresses
	 *	to the nearest page boundary.
	 */
	if (!page_aligned(user_addr))
		return (EINVAL);
	user_size = round_page(user_size);

	/* Note: it appears that vm_protect allows you to apply protections to
	 * non-existent regions of the address space, which is supposed to be
	 * an error according to the AES.  If so, mprotect will have to make
	 * some kind of validity check on the region being re-protected.
	 */

#ifndef	OSF1_SERVER
	result = vm_protect(map, user_addr, user_size, FALSE, user_prot);
#else
	result = vm_protect(u.uu_procp->p_task, user_addr, user_size, FALSE, user_prot);
#endif
	if (result != KERN_SUCCESS) switch (result) {

		case KERN_PROTECTION_FAILURE:
			return (EACCES);

		case KERN_NO_SPACE:
		case KERN_INVALID_ADDRESS:
			return (ENOMEM);

		default:
			return (EINVAL);
	}
	return (0);
}

/* ARGSUSED */
mvalid(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		caddr_t addr;
		size_t  len;
		int     prot;
	} *uap = (struct args *)args;
#ifndef	OSF1_SERVER
	vm_map_t map = current_task()->map;
#endif
	vm_offset_t     user_addr;
	vm_size_t       user_size;
	kern_return_t   result;
	vm_prot_t       user_prot;
#ifdef	OSF1_SERVER
	vm_prot_t       prot;
	vm_prot_t       max_prot;
	vm_inherit_t	inherit;
	boolean_t	shared;
	memory_object_name_t name;
	vm_offset_t	offset;
#endif

	if ((uap->prot < 0) || (uap->prot > MAX_PROT))
		return (EINVAL);
	user_prot = vm_prots[uap->prot];
	user_addr = (vm_offset_t) uap->addr;
	user_size = (vm_size_t) uap->len;
	/*
	 *      We bend a little - round the end addresses
	 *      to the nearest page boundary.
	 */
	if (!page_aligned(user_addr))
		return (EINVAL);
	user_size = round_page(user_size);
	
#ifndef	OSF1_SERVER
	result = vm_map_check_protection(map, user_addr, user_addr + user_size,
					user_prot);
#else	/* OSF1_SERVER */
	result = vm_region(u.u_procp->p_task, &user_addr, &user_size, &prot,
				&max_prot, &inherit, &shared, &name, &offset);
	if (result == KERN_SUCCESS) {
		if (((vm_offset_t) uap->addr < user_addr) ||
		    ((vm_offset_t) uap->addr > (user_addr + user_size)) ||
		     ((user_prot&prot) != user_prot))
			result = FALSE;
		else
			result = TRUE;
	} else
		result = FALSE;

#endif	/* OSF1_SERVER */
	if (!result)
		return (EACCES);
	else
		return (0);
}

/* ARGSUSED */
madvise(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
 	struct args {
		char    *addr;
		int     len;
		int     behav;
	} *uap = (struct args *)args;
        int error;

	/* just check for invalid behav argument */

	*retval = 0;

        switch (uap->behav) {

        case MADV_NORMAL:
        case MADV_RANDOM:
        case MADV_SEQUENTIAL:
        case MADV_WILLNEED:
        case MADV_DONTNEED:
        case MADV_SPACEAVAIL:
	  error = ESUCCESS;
          break;
        default:
          error = EINVAL;
        }

	return (error);

}

/* ARGSUSED */
mincore(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
 	struct args {
		char    *addr;
		int     len;
		char	*vec;
	} *uap = (struct args *)args;

	/* Not yet implemented */
	return (EOPNOTSUPP);

}

/* msleep - If semaphore is still locked when checked by checked put */
/* 	requestor to sleep, waiting for an mwakeup on the semaphore. */
/* ARGSUSED */
msleep(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		msemaphore *sem;
	} *uap = (struct args *)args;
	kern_return_t result;
	
	result = vm_msleep(uap->sem);

	if (result == THREAD_INTERRUPTED)
		return (EINTR);
	else if (result != 0)
		return (EFAULT);
	return (0);
}

/* mwakeup - Wakeup any threads waiting on the semaphore.  Note the  */
/* 	semaphore may still be locked. */
/* ARGSUSED */
mwakeup(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		msemaphore *sem;
	} *uap = (struct args *)args;
	kern_return_t result;
	
	result = vm_mwakeup(uap->sem);

	if (result)
		return (EFAULT);
	return (0);
}

/* msync - Synchronize a mapped region with its backing object */
/* ARGSUSED */
msync(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		caddr_t addr;
		size_t len;
		int flags;
	} *uap = (struct args *)args;
	vm_size_t size;
	kern_return_t result;

	if (!page_aligned(uap->addr))
		return (EINVAL);

	/* Flags can only be one of the following values. */
	switch (uap->flags) {
	case MS_SYNC:
	case MS_ASYNC:
	case MS_INVALIDATE:
		break;
	default:
		return (EINVAL);
	}
	
	size = round_page(uap->len);
	
	result = vm_msync(uap->addr, size, uap->flags);

	if (result) switch(result) {
	case KERN_INVALID_ADDRESS:
		return (ENOMEM);
	case KERN_MEMORY_ERROR:
		return (EIO);
	default:
		return (EINVAL);
	}
	return (0);
}

/* ARGSUSED */
obreak(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*nsiz;
	} *uap = (struct args *)args;
	vm_offset_t	old, new;
	kern_return_t	ret;

	new = round_page(uap->nsiz);
	/* The calculation "new - u.u_data_start" is only valid
	 * if new > u.u_data_start. */
	if ((new > (vm_offset_t)u.u_data_start)
	    && (new - (vm_offset_t)u.u_data_start) > u.u_rlimit[RLIMIT_DATA].rlim_cur)
		return (ENOMEM);
	old = round_page(u.u_data_start + ctob(u.u_dsize));
	if (new > old) {
#ifdef	SERVER_DOES_SBRK
		memory_object_t	pager;
		vm_size_t size;
		/*
		 * First try to use a vnode pager temp. object
		 */
		size = new - old;
		pager = vnode_pager_get(FALSE, size);
		ret = vm_map(p->p_task, &old, size, 0, FALSE, pager, (vm_offset_t)0, FALSE,
			     VM_PROT_READ|VM_PROT_WRITE, VM_PROT_ALL, VM_INHERIT_COPY); 
		if (ret == KERN_SUCCESS)
			ret = vm_mmap_add(p, old, size, pager, (vm_offset_t)0,
					  VM_MMAP_ANONYMOUS);
		inode_pager_release(pager);
		if (ret != KERN_SUCCESS) {
#ifdef DEBUG
			uprintf("%s: using default pager for sbreak\n", u.u_comm);
#endif
			ret = vm_allocate(p->p_task, &old, size, FALSE);
		}
#else
		ret = vm_allocate(p->p_task, &old, new - old, FALSE);
#endif	/* SERVER_DOES_SBRK */
		if (ret != KERN_SUCCESS) {
#ifdef DEBUG
			uprintf("%s: sbrk: vm_allocate %x: old %x size %d\n",
				u.u_comm, ret, old, new-old);
#endif
			return ENOMEM;
		} else {
			/*
			 * If data area has been wired, then
			 * we must wire the new memory as well.
			 */
			if (u.u_lflags & UL_DATLOCK) {
#ifndef	OSF1_SERVER
				vm_map_t mymap = current_task()->map;

				vm_map_reference(mymap);
				vm_map_pageable(mymap, old, new, VM_PROT_READ|VM_PROT_WRITE);
				vm_map_deallocate(mymap);
#else	/* OSF1_SERVER */
				vm_pageable(u.uu_procp->p_task, old, new,
						VM_PROT_READ|VM_PROT_WRITE);
#endif	/* OSF1_SERVER */
			}
			u.u_dsize += btoc(new - old);
		}
	}
	return (0);
}

int	both;

/* BEGIN DEFUNCT */
/* ARGSUSED */
ovadvise(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
#ifdef	lint
	both = 0;
#endif
	return (EOPNOTSUPP);
}
/* END DEFUNCT */

/*
 * grow the stack to include the SP
 * true return if successful.
 */
grow(sp)
	unsigned sp;
{
	register int si;

	if (sp >= USRSTACK-ctob(u.u_ssize))
		return (0);
	si = round_page((USRSTACK - sp) - ctob(u.u_ssize) + ctob(SINCR));
	if ((unsigned)si > u.u_rlimit[RLIMIT_STACK].rlim_cur)
		return (0);
	return (1);
}

/* ARGSUSED */
set_program_attributes(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		caddr_t	text_start;
		size_t	tsize;
		caddr_t	data_start;
		size_t	dsize;
	} *uap = (struct args *)args;
	size_t ts, ds;

	ts = round_page(uap->tsize);
	ds = round_page(uap->dsize);

	if ((unsigned int)ds > u.u_rlimit[RLIMIT_DATA].rlim_cur)
		return (ENOMEM);
	u.u_text_start = uap->text_start;
	u.u_tsize = btoc(ts);
	u.u_data_start = uap->data_start;
	u.u_dsize = btoc(ds);
	return (0);
}


/* Macros to isolate different methods of wiring down pages. */
#ifdef OSF1_SERVER

/* We don't need trunc_page or round_page since that is done by vm_wire. */
#define WIRE_TEXT(access) \
	vm_wire(privileged_host_port, mytask, up->u_text_start, \
		ctob(up->u_tsize), access)
#define WIRE_DATA(access) \
	vm_wire(privileged_host_port, mytask, up->u_data_start, \
		ctob(up->u_dsize), access)
#define WIRE_STACK(access) \
	vm_wire(privileged_host_port, mytask, up->u_stack_start, \
		up->u_stack_end - up->u_stack_start, access)

#else

#define WIRE_TEXT(access) \
	vm_map_pageable(mymap, trunc_page(u.u_text_start), \
			round_page(u.u_text_start + ctob(u.u_tsize)), access)
#define WIRE_DATA(access) \
	vm_map_pageable(mymap, trunc_page(u.u_data_start), \
			round_page(u.u_data_start + ctob(u.u_dsize)), access)
#define WIRE_STACK(access) \
	vm_map_pageable(mymap, trunc_page(u.u_stack_start), \
			round_page(u.u_stack_end), access)

#endif	/* OSF1_SERVER */


/* plock system call -- lock text and/or data in memory */
/* ARGSUSED */
plock(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		long cmd;
	} *uap = (struct args *)args;
	int error;
#ifndef	OSF1_SERVER
	vm_map_t mymap = current_task()->map;
#else
	task_t mytask = u.uu_procp->p_task;
	struct uthread *up = &u;
#endif	/* OSF1_SERVER */

#if SEC_BASE
	/*
	 * Must have privilege
	 */
	if (!privileged(SEC_LOCK, EPERM))       /* XXX which priv needed? */
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif

#ifndef	OSF1_SERVER
	/*
	 * Need to hold a reference to the task map
	 * through the call(s) to vm_map_pageable.
	 */
	vm_map_reference(mymap);
#endif	/* OSF1_SERVER */
	/*
	 * Note that the protections specified in the calls to
	 * vm_map_pageable must be a (possibly improper) subset
	 * of the protections specified in the vm_map calls made
	 * by exec.
	 */
	if (uap->cmd == TXTLOCK || uap->cmd == PROCLOCK) {

		if (u.u_lflags & UL_TXTLOCK) {
			error = EINVAL;
			goto out;
		}

		if (WIRE_TEXT(VM_PROT_READ) != KERN_SUCCESS) {
			error = EINVAL;	/* SVID III says EAGAIN? */
			goto out;
		}

		u.u_lflags |= UL_TXTLOCK;
	}

	if (uap->cmd == DATLOCK || uap->cmd == PROCLOCK) {

		if (u.u_lflags & UL_DATLOCK) {
			error = EINVAL;
			goto out;
		}

		if (WIRE_DATA(VM_PROT_READ|VM_PROT_WRITE) != KERN_SUCCESS) {
			if (uap->cmd == PROCLOCK) {
				(void) WIRE_TEXT(VM_PROT_NONE);
				u.u_lflags &= ~UL_TXTLOCK;
			}
			error = EINVAL;	/* SVID III says EAGAIN? */
			goto out;
		}

		/* EMcM: VM_PROT_ALL -> READ|WRITE. */
		if (WIRE_STACK(VM_PROT_READ|VM_PROT_WRITE) != KERN_SUCCESS) {
			(void) WIRE_DATA(VM_PROT_NONE);
			if (uap->cmd == PROCLOCK) {
				WIRE_TEXT(VM_PROT_NONE);
				u.u_lflags &= ~UL_TXTLOCK;
			}
			error = EINVAL;	/* SVID III says EAGAIN? */
			goto out;
		}

		u.u_lflags |= UL_DATLOCK;
	}

	else if (uap->cmd == UNLOCK) {

		/*
		 * We return an error only if neither of the regions
		 * is locked.
		 */
		if ((u.u_lflags & (UL_TXTLOCK|UL_DATLOCK)) == 0) {
			error = EINVAL;
			goto out;
		}

		if (u.u_lflags & UL_TXTLOCK) {
			(void) WIRE_TEXT(VM_PROT_NONE);
			u.u_lflags &= ~UL_TXTLOCK;
		}

		if (u.u_lflags & UL_DATLOCK) {
			(void) WIRE_DATA(VM_PROT_NONE);
			(void) WIRE_STACK(VM_PROT_NONE);
			u.u_lflags &= ~UL_DATLOCK;
		}
	}
out:
#ifndef	OSF1_SERVER
	vm_map_deallocate(mymap);
#endif	/* OSF1_SERVER */
	return (error);
}
#undef WIRE_TEXT
#undef WIRE_DATA
#undef WIRE_STACK

/* ARGSUSED */
getaddressconf(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		struct addressconf *buf;
		size_t  size;
	} *uap = (struct args *)args;
	size_t          copysize;
	int             error;

	copysize = (uap->size / sizeof(struct addressconf)) * sizeof(struct addressconf);
	if (copysize > sizeof(addressconf))
		copysize = sizeof(addressconf);
	error = copyout((caddr_t)&addressconf, (caddr_t)(uap->buf), copysize);
	if (error)
		return (error);
	*retval = (int)copysize;
	return (error);
}

