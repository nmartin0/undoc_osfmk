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
 * Copyright 1994 , 1995 Intel Corporation
 * All rights reserved
 *
 * $Id: allocinfo.c,v 1.6 1994/11/18 20:37:56 mtm Exp $
 *
 * HISTORY
 * $Log: allocinfo.c,v $
 * Revision 1.6  1994/11/18  20:37:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/10/25  17:07:06  johannes
 * added warnings (done with uprintf) to the user in case of wrong core actions
 * or problems to create/write core/allocinfo files
 *
 *  Reviewer: Karla (user view)
 *  Risk: low
 *  Benefit or PTS #: 10667 and warning for problems with core files
 *  Testing: developer testing
 *  Module(s): server/paracore/allocinfo.c
 * 	    server/paracore/core.c
 * 	    server/paracore/dump.c
 * 	    server/paracore/dvp_pvpcore.c
 *
 * Revision 1.4  1994/09/27  07:56:34  johannes
 * write_allocinfo(): check coredumpsize before creating allocinfo file
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: correct check of coredumpsize (7509, 10344)
 *  Testing: developer testing
 *  Module(s): server/paracore/allocinfo.c
 * 	    server/paracore/core.c
 *
 * Revision 1.3  1994/07/27  15:25:19  johannes
 * changed header
 *
 */

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>

#ifdef OSF1_SERVER
#include <sys/fcntl.h>
#include <kern/parallel.h>
#endif

#include <paracore/core_types.h>
#include <sys/allocinfo.h>

extern int paracoreprint;

write_allocinfo(const struct vproc	*vp,
		const mach_port_t	core_dir,
	  	const mach_port_t	root_dir)
{
	mach_port_t		vn = MACH_PORT_NULL;
	struct vattr 		vattr;
	int 			error, unlink_error;
	register struct nameidata *ndp = &u.u_nd;
	mach_port_t		orig_root_port, orig_cwd_port, orig_fwd_port;
	enum vtype		type;
	struct proc		*p;
	struct allocinfo_hdr	header;
	PARTREQ_T		partinfo;

	ASSERT(syscall_on_master());
	p = u.u_procp;
	
	if (paracoreprint)	/* XXX Just for debugging */
		uprintf("(%s) pid %d uid %d writes allocinfo file\n",
		       u.u_comm, p->p_pid, p->p_ruid);

	PROC_LOCK(p);
	if (p->p_flag & SXONLY) {
		PROC_UNLOCK(p);
		return -1;
	}
	crfree(u.u_cred);
	u.u_cred = p->p_rcred;
	crhold(p->p_rcred);
	p->p_rcred->cr_uid = p->p_ruid;
	p->p_rcred->cr_gid = p->p_rgid;
	PROC_UNLOCK(p);

	/* check core file size limit */
	if (sizeof header > u.u_rlimit[RLIMIT_CORE].rlim_cur) {
		CORE_DEBUGF("write_allocinfo: core file size limit exceeded\n");
		return -1;
	}
	
	orig_root_port = ndp->ni_rdirport;
	orig_cwd_port = ndp->ni_cdirport;
	orig_fwd_port = ndp->ni_forwport;
	
	ndp->ni_rdirport = root_dir;
	ndp->ni_cdirport = core_dir;
	ndp->ni_forwport = core_dir;
	
	ndp->ni_dirp = (caddr_t) "allocinfo";
	ndp->ni_segflg = UIO_SYSSPACE;
	
#if     SEC_BASE
	error = remote_vnopen(ndp, FCREAT|FWRITE, SEC_CORE_MODE, &vn, &type);
#else
	error = remote_vnopen(ndp, FCREAT|FWRITE, 0644, &vn, &type);
#endif
	
	if (error) {
		printf("write_allocinfo: remote_vnopen failed(0x%x)\n", error);
		uprintf("Cannot create allocinfo file.\n");
		goto out;
	}
	
	if (type != VREG) {
		error = EFAULT;
		uprintf("Cannot create allocinfo file.\n");
		goto out;
	}
	error = remote_getattr(vn, &vattr);
	if (error || vattr.va_nlink != 1) {
		if (error == 0)
			error = EFAULT;
		uprintf("Cannot create allocinfo file.\n");
		goto out;
	}
	/*
	 * 4.4 has some code here ifdef MMAP to unmap devices.
	 * We have not included that code.
	 */
	vattr_null(&vattr);
	vattr.va_size = 0;
	error = remote_setattr(vn, &vattr);

	U_HANDY_LOCK();
	u.u_acflag.fi_flag |= ACORE;
	U_HANDY_UNLOCK();

	/* fill core header */
	header.a_magic = ALLOC_MAGIC;
	header.a_swap = ALLOC_SWAP;
	header.a_version = ALLOC_VERSION;
	
	/* get application and partition information */
	CORE_DEBUGF("write_allocinfo: call nx_app_part()\n");
	if (error = nx_app_part(vp, 
				p->p_pgid,
				&header.a_application, 
				&partinfo)) {
		printf("write_allocinfo: nx_app_part failed(0x%x)\n", error);
		uprintf("Cannot get data for allocinfo file.\n");
		goto error_out;
	}
	
	/* 
	 * copy partion information into header 
	 * this is a copy of nx_get_part_attributes() in libnx/allocsys.c 
	 */
	header.a_partition.uid = partinfo.uid;
	header.a_partition.gid = partinfo.gid;
	header.a_partition.access = partinfo.access;
	header.a_partition.sched = partinfo.sched;
	header.a_partition.rq  = partinfo.rq;
	header.a_partition.epl = partinfo.maxpri;
	header.a_partition.nodes = partinfo.slots;
	header.a_partition.flags_or_size = partinfo.flags;
	header.a_partition.part_id = partinfo.inode;

        header.a_partition.mesh_x = partinfo.rect.cols;
        header.a_partition.mesh_y = partinfo.rect.rows;
        
        header.a_partition.enclose_mesh_x = partinfo.flags;
        header.a_partition.enclose_mesh_y = partinfo.free;

	/* write header */
	CORE_DEBUGF("write_allocinfo: write header\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vn, (caddr_t)&header, 
			          sizeof header, (off_t)0,
			          IO_UNIT,  (int *)0)) {
		printf("write_allocinfo: remote_vnrdwr failed(0x%x)\n", error);
		uprintf("Cannot write allocinfo file.\n");
		goto error_out;
	}
	
	goto out;
		
error_out:
	/* delete not completely written allocinfo file */
	if (unlink_error = remote_unlink(ndp))
		printf("write_allocinfo: remote_unlink failed(0x%x)\n", 
		       unlink_error);
	
out:
	if (vn != MACH_PORT_NULL)
		mach_port_deallocate(mach_task_self(), vn);
	
	ndp->ni_rdirport = orig_root_port;
	ndp->ni_cdirport = orig_cwd_port;
	ndp->ni_forwport = orig_fwd_port;
	
	return error;
}
