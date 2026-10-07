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
 * This file was modified and extended by the Center for High Performance
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: spec_vnops.c,v $
 * Revision 1.8  1994/11/18  20:50:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/28  23:21:55  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.6  1994/01/11  18:26:19  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: less lint complaints
 *  Testing: compiled
 *  Module(s):
 * 	nfs/nfs_vnops.c
 * 	vfs/fifo_vnops.c
 * 	vfs/vfs_cache.c
 * 	vfs/vfs_flock.c
 * 	vfs/vfs_vnops.c
 * 	vfs/vfs_bio.c
 * 	vfs/vfs_subr.c
 * 	vfs/vfs_vio.c
 * 	vfs/spec_vnops.c
 * 	vfs/vfs_syscalls.c
 * 	vfs/vfs_lookup.c
 *
 * Revision 1.5  1993/11/30  23:15:48  cfj
 * Bit bucket reads and writes to /dev/null at the emulator instead of sending
 * RPCs to the file server node.
 *
 *  Reviewer:brad, dbm
 *  Risk:M
 *  Benefit or PTS #:7261
 *  Testing:
 *  Module(s):server/sys/vnode.h
 * 	   server/uxkern/device_misc.c
 * 	   server/vfs/spec_vnops.c
 * 	   emulator/fsvr_user_side.c
 *
 * Revision 1.4  1993/07/14  18:45:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:08:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:38  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.21  93/10/20  15:31:55  dnoveck
 *      DEV_BSIZE elimination: Change use of DEV_BSIZE-based defines
 *      to their DISK_GRANULE-based corelates.  Add new logic for
 *      new buffer hashing scheme.
 *
 * Revision 2.20  1993/01/25  22:56:56  durriya
 * 	In spec_open do the same processing if syscode is 2011 (open_with_token)
 * 	as done for SYS_open                                    (durriya)
 *
 * Revision 2.19  93/01/08  14:38:58  durriya
 * 	add node # as arg to BDEVSW_CLOSE, BDEVSW_IOCTL, CDEVSW_CLOSE, 
 * 	CDEVSW_READ, CDEVSW_WRITE, CDEVSW_IOCTL
 * 	set b_devnode in buf struct in pageio                 durriya
 *
 * Revision 1.2  1992/11/30  22:57:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:46:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:57:03  cfj
 * Bump major revision number.
 *
 * Revision 2.19  1992/10/22  16:06:19  dbm
 * Added PFS functionality.
 *
 * Revision 2.18  1992/09/24  16:51:51  rabii
 * 	In spec_close(), call vflushbuf() and vinvalbuf() for the real vp
 * 	(as opposed to the shadow vp) to flush out filesystem data. (mmp)
 * 
 * Revision 2.17  92/09/20  11:26:00  roy
 * 	Another op for OSF1_ADFS.
 * 	[92/09/15            roy]
 * 
 * Revision 2.16  92/08/26  12:14:31  loverso
 * 	Additional ops for OSF1_ADFS.
 * 	[92/08/19            roy]
 * 
 * Revision 2.15  92/05/24  14:05:11  pjg
 * 	Renamed si_node and v_node to si_devnode and v_devnode.
 * 
 * Revision 2.14  92/03/20  11:37:46  pjg
 * 	92/03/17  17:37:09  pjg
 * 	Moved the routines that translate a node or a port to the fileserver
 * 	port to fsvr_port.c
 * 
 * 	92/03/17  17:37:09  noemi
 * 	Changed get_file_server_port and spec_open to return errors if
 * 	the name server port doesn't exist.  Removed VREF from spec_close.
 * 
 * Revision 2.13  92/03/16  18:29:32  pjg
 * 	92/03/16  20:26:17  noemi
 * 	Removed bogus mach_port_deallocate call from spec_close when this
 * 	is not the last close of the device.  However, the vnode reference
 * 	count is still incorrect.
 * 
 * Revision 2.12  92/03/09  13:58:01  durriya
 * 	Revision 3.11  92/02/27  21:08:46  jose
 * 	Set b_optimize_me to FALSE in pageio.
 * 
 * 	Revision 3.10  91/12/27  17:26:25  jose
 * 	Added code missing on ASYNC_PAGEOUT option
 * 
 * 	Revision 3.9  91/12/18  17:18:42  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.11  92/03/03  13:55:38  pjg
 * 	Call mach_port_deallocate instead of remote_vrele in spec_close.
 * 
 * Revision 2.10  92/03/01  18:44:43  pjg
 * 	Don't VREF the vnode before calling get_vnode_port.
 * 
 * Revision 2.9  92/01/14  10:54:04  roy
 * 	91/01/11  14:40:49  noemi
 * 	Removed printfs.  Fixed OSF1_ADFS_DEBUG error.
 * 
 * Revision 2.8  92/01/05  19:54:18  roy
 * 	Put OSF1_ADFS debugging output under ifdef.
 * 
 * Revision 2.7  92/01/05  19:31:54  roy
 * 	91/12/18  22:00:38  noemi
 * 	Reset si_flag to SI_CLOSED if the open of an unopened special 
 * 	file fails.
 * 
 * 	1991/12/18  15:09:05  pjg
 * 	Put the hacks to use local console and local root under a conditional.
 * 
 * 	91/12/17  21:50:24  noemi
 * 	Spec_open and norma fixes.
 * 
 * 	1991/12/05  23:43:13  noemi
 * 	Added node number to DEVSW_OPEN macro calls (from rabii@osf.org).
 * 
 * 	1991/11/12  19:26:32  noemi
 * 	New special files code.
 * 
 * 	1991/09/22  22:03:24  noemi
 * 	OSF1/ADFS update
 * 
 * Revision 2.6  91/12/17  08:25:43  roy
 * 	91/11/26  15:34:25  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/09/13  12:51:42  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.5  91/11/26  13:37:59  rabii
 * 	Remove debugging macro
 * 
 * Revision 2.4  91/11/25  16:15:57  rabii
 * 	Added remote devices
 * 
 * Revision 2.3  91/10/14  13:26:31  sjs
 * 	91/09/13  12:51:42  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  14:30:14  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.6  91/08/27  15:40:28  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.5  91/08/01  17:02:38  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.4.4  91/03/15  18:27:26  tmt
 * 	Merge gmf's clone ops for 1.0.1 submit.
 * 	[91/03/15  18:13:09  tmt]
 * 
 * Revision 1.13.5.3  91/03/14  16:31:57  gmf
 * 	Put in missing end comment.
 * 
 * Revision 1.13.5.2  91/03/14  16:20:24  gmf
 * 	Add clone operations so calls like fstat() can
 * 	succeed.
 * 
 * Revision 1.13.4.3  90/12/20  15:02:26  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.13.2.3  90/11/21  14:33:09  jeffc
 * 	Anpther pageio fix: don't set B_RAW.
 * 	[90/11/20  13:18:02  jeffc]
 * 
 * Revision 1.13.2.2  90/11/13  11:59:22  gmf
 * 	2 fixes: 1) move check for VBLK and setmount in clearalias
 * 	before locking of alias hash chain.
 * 	2) don't return after vinvalbuf in spec_close.  Fall
 * 	through to decrement alias usecount.
 * 	[90/11/09  12:11:52  gmf]
 * 
 * Revision 1.13  90/10/31  14:08:28  devrcs
 * 	3 pageio fixes: don't set B_PHYS, init and lock buffer lock,
 * 	and init b_vp.  Also eliminate unused defs from pageout done.
 * 	[90/10/24  17:30:13  dlb]
 * 
 * 	Added spec_seek to handle file system specific seek operation.
 * 	[90/10/08  17:12:39  collins]
 * 
 * Revision 1.12  90/10/07  15:00:12  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:47  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:54:37  gm]
 * 
 * 	Move wakeup and buffer enqueue in MACH_XP_SC to inode_pager.c.
 * 	[90/09/15  14:24:54  tmt]
 * 
 * 	Protect short circuit code with MACH_XP_SC.
 * 	[90/09/13  07:28:07  ers]
 * 
 * 	Fixed clearalias problem in which it called a driver
 * 	close routine holding a spin lock.
 * 	[90/09/10  17:15:17  gmf]
 * 
 * 	Modified to allow pageouts to be asynchronous.
 * 	[90/09/11  06:54:35  ers]
 * 
 * Revision 1.11  90/09/23  16:01:20  devrcs
 * 	Fixed two problems:  1) fixed arguments to zinit.
 * 	2) fixed problem where sa_usecount went negative
 * 	in spec_close called from vclean.  If VXLOCK is
 * 	set, just return.
 * 	[90/09/21  15:47:24  gmf]
 * 
 * 	Fix = instead of == in makealias -- leading to assert in
 * 	[90/09/17  18:32:10  lwa]
 * 
 * 	vfree (bug 1056)
 * 	[90/09/17  18:31:21  lwa]
 * 
 * Revision 1.10  90/08/24  12:29:59  devrcs
 * 	remove setjmp dependencies
 * 	[90/08/20  00:46:55  gmf]
 * 
 * 	Changed spec_page_read and spec_page_write to call common routine
 * 	pageio(), which knows how to read/write to a block device, and
 * 	can page to/from physical memory.
 * 	[90/08/18  15:43:48  ers]
 * 
 * Revision 1.9  90/07/27  09:09:35  devrcs
 * 	Added spec_page_read/write for paging to/from raw partitions.
 * 	[90/07/23  14:41:05  ers]
 * 
 * 	Dead mounts, clone driver support, fix VBLK write case.
 * 	[90/07/20  17:08:51  nags]
 * 
 * 	Handle unopened special files.
 * 	[90/07/17  07:42:03  nags]
 * 
 * Revision 1.8  90/06/22  20:56:17  devrcs
 * 	Give shadowvp's a mount structure (XXX -- temporary hack only).
 * 	[90/06/18  17:14:10  nags]
 * 
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:58:09  seiden]
 * 
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	   Parallelized for OSF/1; new alias handling.	nags@encore.com
 * 	   Changed for new select interface.		coren@osf.org
 * 	   Added islocked function (null).		gmf@osf.org
 * 	   Added missing ops to spec_vnodeops.		noemi@osf.org
 * 	   Integrated 4.4BSD changes as of 1/5/90.	noemi@osf.org
 * 	[90/06/12  21:42:55  nags]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)spec_vnops.c	7.20 (Berkeley) 11/30/89
 */

#include <norma_ipc.h>
#if NORMA_IPC
#include <mach/norma_special_ports.h>
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/specdev.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/disklabel.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#if	MACH
#include <uxkern/vm_param.h>
#include <uxkern/device.h>
#include <kern/assert.h>
#include <kern/zalloc.h>
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>
#include <kern/mfs.h>
#else
#include <sys/malloc.h>
#endif
#ifdef	OSF1_ADFS
#include <sys/syscall.h>
#include <uxkern/device_utils.h>
#endif

#ifdef	MACH_XP_SC
#include <mach_xp_sc.h>
#define ASYNC_PAGEOUT MACH_XP_SC
#endif

int	spec_lookup(),
	spec_getattr(),
	spec_open(),
	spec_read(),
	spec_clread(),
	spec_write(),
	spec_clwrite(),
	spec_strategy(),
	spec_bmap(),
	spec_ioctl(),
	spec_select(),
	spec_seek(),
	spec_close(),
	spec_reclaim(),
	spec_print(),
	spec_ebadf(),
	spec_page_read(),
	spec_page_write(),
	spec_badop(),
	spec_nullop();

struct vnodeops spec_vnodeops = {
	spec_lookup,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	spec_open,		/* open */
	spec_close,		/* close */
	spec_ebadf,		/* access */
	spec_ebadf,		/* getattr */
	spec_ebadf,		/* setattr */
	spec_read,		/* read */
	spec_write,		/* write */
	spec_ioctl,		/* ioctl */
	spec_select,		/* select */
	spec_badop,		/* mmap */
	spec_nullop,		/* fsync */
	spec_seek,		/* seek */
	spec_badop,		/* remove */
	spec_badop,		/* link */
	spec_badop,		/* rename */
	spec_badop,		/* mkdir */
	spec_badop,		/* rmdir */
	spec_badop,		/* symlink */
	spec_badop,		/* readdir */
	spec_badop,		/* readlink */
	spec_badop,		/* abortop */
	spec_nullop,		/* inactive */
	spec_reclaim,		/* reclaim */
	spec_bmap,		/* bmap */
	spec_strategy,		/* strategy */
	spec_print,		/* print */
	spec_page_read,		/* page_read */
	spec_page_write,	/* page_write */
#ifdef	PFS
	spec_badop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
#endif
};

/*
 * Data structures required for clone devices.
 * 1.  A new operations array.
 * 2.  A new node type and associate macros.
 */
struct vnodeops spec_cloneops = {
	spec_badop,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	spec_badop,		/* open */
	spec_close,		/* close */
	spec_nullop,		/* access */
	spec_getattr,		/* getattr */
	spec_nullop,		/* setattr */
	spec_clread,		/* read */
	spec_clwrite,		/* write */
	spec_ioctl,		/* ioctl */
	spec_select,		/* select */
	spec_badop,		/* mmap */
	spec_nullop,		/* fsync */
	spec_seek,		/* seek */
	spec_badop,		/* remove */
	spec_badop,		/* link */
	spec_badop,		/* rename */
	spec_badop,		/* mkdir */
	spec_badop,		/* rmdir */
	spec_badop,		/* symlink */
	spec_badop,		/* readdir */
	spec_badop,		/* readlink */
	spec_badop,		/* abortop */
	spec_nullop,		/* inactive */
	spec_reclaim,		/* reclaim */
	spec_badop,		/* bmap */
	spec_badop,		/* strategy */
	spec_print,		/* print */
	spec_badop,		/* page_read */
	spec_badop,		/* page_write */
#ifdef	PFS
	spec_badop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
#endif
};

struct spec_node {
	struct vnode *sn_vnode;
	struct vattr sn_vattr;
};

#define VTOS(vp)	((struct spec_node *)(vp)->v_data)

/*
 * Local shorthand
 */
#define v_alias v_specinfo->si_alias
#define v_shadowvp v_specinfo->si_shadowvp
#define v_nextalias v_specinfo->si_nextalias

extern void vfree();

/*
 * The hash chain.
 * This MUST be a power of two for the SPECHASH macro to work.
 * The size is enforced where the list is allocated (machine-dependent).
 */
struct spechash *speclisth;		/* the spec hash table */
#define SPECHASH(rdev)  (((rdev>>5)+(rdev))&(spechsz-1))


/* debug XXX */
int checkspeclist = 1;

#define SPECHASH_LOCK(sh)	usimple_lock(&sh->sh_lock)
#define SPECHASH_UNLOCK(sh)	usimple_unlock(&sh->sh_lock)
#define SPECHASH_LOCK_INIT(sh)	usimple_lock_init(&sh->sh_lock)

/*
 * Memory allocation macros
 */
#if	MACH
zone_t specinfo_zone;
zone_t specalias_zone;
zone_t swapbuf_zone;

#define	SPEC_ALLOCATE(si)	ZALLOC(specinfo_zone, si, struct specinfo *)
#define	SPEC_DEALLOCATE(si)	ZFREE(specinfo_zone, (si))
#define	SA_ALLOCATE(sa)		ZALLOC(specalias_zone, sa, struct specalias *)
#define	SA_DEALLOCATE(sa)	ZFREE(specalias_zone, (sa))
#define	SB_ALLOCATE(bp)		ZALLOC(swapbuf_zone, bp, struct buf *)
#define	SB_DEALLOCATE(bp)	ZFREE(swapbuf_zone, (bp))
#else	/* MACH */
#define	SPEC_ALLOCATE(si)	MALLOC(si, (struct specinfo *),	\
				      (u_long)sizeof(struct specinfo),\
				       M_VNODE, M_WAITOK)
#define	SPEC_DEALLOCATE(si)	FREE((caddr_t)si, M_VNODE)
#define SA_ALLOCATE(sa)		MALLOC(sa, (struct specalias *), \
					(u_long)sizeof(struct specalias),\
					M_VNODE, M_WAITOK)
#define	SA_DEALLOCATE(sa)	FREE((caddr_t)sa, M_VNODE)
#endif	/* MACH */

#define SPECALIAS_MAX	nvnode
#define SPECINFO_MAX	nvnode
#define SWAPBUF_MAX	1024


/*
 * General rules for aliases.
 *
 *	-- Specinfo structures are allocated for all device vnodes during
 *	   pathname translation.  They hold the device number and fields
 *	   to link the vnode on an alias list, if necessary; they are not
 *	   linked onto an alias list until the device is opened.
 *	-- Specalias structures are allocated during the open of a device.
 *	   They are linked on a hashchain to associate multiple vnodes that
 *	   reference the same physical device.
 *	-- During makealias, called from spec_open, a vnode is attached
 *	   to an alias list, possibly allocating a specalias structure
 *	   to put on the hashchain.  If the device is a block device, 
 *	   there is a vnode allocated also to be the "shadow" vnode used
 *	   for operations involving the buffer cache (read, write, buffer
 *	   flushing).  This vnode provides cache consistency for all of
 *	   the open vnodes representing the same block device.
 *	-- Specinfo structures and aliases are unlinked
 *	   and deallocated in spec_reclaim, when vnodes are being recycled.
 *	-- It is legitimate to have vnodes with v_usecount of 0 on an alias
 *	   list (opened, then closed, but not yet reclaimed).
 *	-- It is legitimate to have an alias structure with a sa_usecount of
 *	   0 on a hashchain, as long as its alias list is non-null.
 *	-- It is possible to have an alias structure with a sa_usecount of
 *	   > 0, with a null alias list (clearalias in progress; in this case
 *	   multiple opens of a vnode will only have a single close, so the
 *	   count won't match).
 *	-- Specalias structures for mounted file systems have the
 *	   SA_MOUNTED flag set.
 *	-- all fields inside specinfo, specalias, and spechash structures
 *	   are under protection of the spechash lock for the 
 *	   hashchain (MP only). 
 *
 * 	Synchronization with flags:
 *	-- spec_close waits for SA_GOING (clearalias).
 *	-- makealias and clearalias wait for SA_GOING and SA_CLOSING.
 *	-- those who sleep with SA_WAIT set MUST NOT re-check the flags
 *	   after awakening, but must start over, since the alias
 *	   structure could have been deallocated.
 *	MP synchronization:
 *	-- Because of the possibility of clearalias being called virtually
 *	   any time, we must validate the vnode when called through the
 *	   VOP_* interface, and other times vnodes are used as well.  It is
 *	   sufficient to check for VXLOCK and type != VBAD, since our 
 *	   vnode reference will prevent recycling of the vnode to another
 *	   file system.
 *	-- It is necessary to take both the vnode and the spechash lock 
 *	   at the same time to ensure synchronization.  In this case, we
 *	   need to take the vnode lock FIRST.
 */

/*
 * specalloc
 *	Called from path translation routines to allocate a specinfo
 *	structure to be associated with a device vnode.
 * Assumptions:
 *	-- v_type is set correctly.
 *	-- caller will set v_rdev.
 *	-- vnode is not on ANY lists, and cannot be found, so no locking
 *	   is required.
 */
#ifdef	OSF1_ADFS
specalloc(vp, dev, node)
#else
specalloc(vp, dev)
#endif
	struct vnode 	*vp;
	dev_t 		dev;
#ifdef	OSF1_ADFS
	node_t		node;
#endif
{
	register struct specinfo *si;

	SPEC_ALLOCATE(si);
	if (si == (struct specinfo *)0) {
		return (ENOMEM);
	}
	bzero((caddr_t)si, sizeof(struct specinfo));
	si->si_rdev = dev;
#ifdef	OSF1_ADFS
	si->si_devnode = node;
	si->si_specport = MACH_PORT_NULL;
	si->si_flag = SI_CLOSED;
	SI_LOCK_INIT(si);
	SPEC_LOCK_INIT(si);
#endif
	vp->v_specinfo = si;
	return (0);
}

/*
 * Create a special file vnode.
 * Defaults to v_type of VBLK.
 * Used for:
 *	root filesystem, argdev, and swap areas.
 *	memory file system special devices.
 *	clone devices (VCHR).
 * MP:
 *	-- No locking needed.  We get a new vnode; no one can
 *	   find it, so there's no contention.
 */
#ifdef	OSF1_ADFS
bdevvp(dev, node, type, vpp)
#else
bdevvp(dev, vpp)
#endif
	dev_t 	dev;
#ifdef	OSF1_ADFS
	node_t	node;
	enum vtype type;
#endif
	struct vnode **vpp;
{
	register struct vnode *vp;
        struct specinfo *nsi;
	struct vnode *nvp;
	int error;

	SPEC_ALLOCATE(nsi);
	if (nsi == (struct specinfo *)0) {
		*vpp = 0;
		return (ENOMEM);
	}
	error = getnewvnode(VT_NON, &spec_vnodeops, &nvp);
	if (error) {
		SPEC_DEALLOCATE(nsi);
		*vpp = 0;
		return (error);
	}
	bzero((caddr_t)nsi, sizeof(struct specinfo));
	vp = nvp;
	vp->v_specinfo = nsi;
	vp->v_rdev = dev;
#ifdef	OSF1_ADFS
	vp->v_devnode = node;
	vp->v_type = type;
	nsi->si_specport = MACH_PORT_NULL;

	SI_LOCK_INIT(nsi);
	SPEC_LOCK_INIT(nsi);
#else
	vp->v_type = VBLK;
#endif
	insmntque(vp, DEADMOUNT);
	*vpp = vp;
	return (0);
}

/*
 * spec_set_bshift
 *      Set the buffer cache hash shift for a block special file.  This
 *      must be called before anything is entered into the cache for the
 *      specified device vnode and not changed thereafter.
 */
spec_set_bshift(vp)
	struct vnode *vp;
{
	struct partition dpart;
	struct vnode *tvp;
	dev_t  rdev = vp->v_rdev;
#ifdef	OSF1_ADFS
	node_t node = vp->v_devnode;
	struct devinfo *info = NULL;
#endif
	int error;
	int bsize = 0;
	int bscale;
	int shift;

#ifdef OSF1_ADFS
	BDEVSW_IOCTL(major(rdev), rdev, node, DIOCGPINFO,
		     (caddr_t)&dpart, FREAD, error);
#else
	BDEVSW_IOCTL(major(rdev), rdev, DIOCGPINFO,
		     (caddr_t)&dpart, FREAD, error);
#endif
	if (error == 0 &&
	    dpart.p_fstype == FS_BSDFFS &&
	    dpart.p_frag != 0 && 
	    dpart.p_fsize != 0) {
	        bsize = (MAX(dpart.p_frag * dpart.p_fsize, BLKDEV_IOSIZE));
	};
#ifdef OSF1_ADFS
	if (bsize == 0) {
		info = (devinfo_t *) dev_lookup(rdev, node, BLOCK_DEV);
		if (info != NULL) {
			bsize = MAX(info->mrecsize, BLKDEV_IOSIZE);
		}
	}
#endif
	if (bsize == 0)
		bsize = BLKDEV_IOSIZE;
	bscale = btodg(bsize);
	for (shift = 0; bscale > 1; shift++, bscale >>= 1);
	vp->v_bufhash_shift = shift;
}


/*
 * makealias
 *	Attach a vnode to an alias structure, allocating one if necessary.
 *	Synchronization:
 *		-- wait for in-progress spec_close and clearalias calls.
 *		-- lock order: vnode, THEN spechash lock.  Need this to
 *		   guarantee vnode consistency.
 */
makealias(vp)
	register struct vnode *vp;
{
        register struct specalias *sa;
        register struct specalias *nsa;
        struct spechash *spechashp;
        register struct vnode *nvp;
        register dev_t rdev;
#ifdef	OSF1_ADFS
	node_t node;
	struct specinfo *si = vp->v_specinfo;
#endif
        register enum vtype type;
	int allocated = 0;

#ifdef	OSF1_ADFS
	BM(SI_LOCK(si));
	ASSERT((si->si_flag & SI_RMTDEV) == 0);
	BM(SI_UNLOCK(si));
#endif
loop:
	VN_LOCK(vp);
	/*
	 * Check for a change of state.   Type could only be VBAD if
	 * we've been vgone'd because of reference count.
	 */
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		if (allocated) {
			if (type == VBLK) {
				ASSERT(nvp);
				nvp->v_alias = (struct specalias *) 0;
				vfree(nvp);
			}
			SA_DEALLOCATE(nsa);
		}
		return (ENXIO);
	}
	rdev = vp->v_rdev;
#ifdef	OSF1_ADFS
	node = vp->v_devnode;
#endif
	type = vp->v_type;
	spechashp = &speclisth[SPECHASH(rdev)];
	SPECHASH_LOCK(spechashp);
	sa = vp->v_alias;
	VN_UNLOCK(vp);

	if (sa == (struct specalias *) 0)
		for (sa = spechashp->sh_alias; 
#ifdef OSF1_ADFS
		     sa && ((sa->sa_rdev != rdev) || (sa->sa_type != type) ||
                            (sa->sa_devnode != node)); 
#else
		     sa && ((sa->sa_rdev != rdev) || (sa->sa_type != type));
#endif
		     sa = sa->sa_next);
	if (sa != (struct specalias *) 0) {
		if ((sa->sa_flag & (SA_CLOSING|SA_GOING)) != 0) {
			sa->sa_flag |= SA_WAIT;
			assert_wait((int)&sa->sa_flag, FALSE);
			SPECHASH_UNLOCK(spechashp);
			thread_block();
			goto loop;
		}
		if (allocated) {
			/*
			 * We get here in the case of racing makealias calls
			 */
			if (type == VBLK) {
				ASSERT(nvp);
				nvp->v_alias = (struct specalias *) 0;
				vfree(nvp);
			}
			SA_DEALLOCATE(nsa);
		}
	} else {
		/*
		 * Not on list, so we allocate new structures and
		 * goto loop in case we slept and someone got in and
		 * created this alias before us.  In that case, we
		 * deallocate.  If all works out, we'll get back here
		 * again, with the allocated flag set.
		 */
		if (!allocated) {
			SPECHASH_UNLOCK(spechashp);
			SA_ALLOCATE(nsa);
			if (nsa == (struct specalias *)0)
				return (ENOMEM);
			bzero((caddr_t)nsa, sizeof(struct specalias));
			nsa->sa_type = type;
			nsa->sa_rdev = rdev;
#ifdef OSF1_ADFS
                        nsa->sa_devnode = node;
#endif
			if (type == VBLK) {
				struct vnode *tvp;
				int error;
#ifdef	OSF1_ADFS
				if (error = bdevvp(rdev, node, VBLK, &tvp)) {
					SA_DEALLOCATE(nsa);
					return (error);
				}
#else
				if (error = bdevvp(rdev, &tvp)) {
					SA_DEALLOCATE(nsa);
					return (error);
				}
#endif
				nvp = tvp;
				nsa->sa_vnode = nvp;
				nvp->v_alias = nsa;
			}
			allocated = 1;
			goto loop;
		}
		sa = nsa;
		sa->sa_next = spechashp->sh_alias;
		spechashp->sh_alias = sa;
	}
	/*
	 * put our reference on the alias structure
	 */
	sa->sa_usecount++;
	/*
	 * The hashchain is still locked
	 */
	LASSERT(SLOCK_HOLDER(&spechashp->sh_lock));
	/*
	 * Determine whether the vnode is already on the list.
	 * If not, add it.
	 */
	for (nvp = sa->sa_vlist; (nvp && nvp != vp); nvp = nvp->v_nextalias);
	if (!nvp) {
		vp->v_nextalias = sa->sa_vlist;
		vp->v_alias = sa;
		sa->sa_vlist = vp;
		/*
		 * set up shadow vnode for block devices
		 */
		if (type == VBLK) {
			vp->v_shadowvp = sa->sa_vnode;
                        vp->v_bufhash_shift = sa->sa_vnode->v_bufhash_shift;
		}
	} else {
		/* 
		 * this is DEBUG code that should disappear.  Want
		 * to verify list for now.
		 */
		if (checkspeclist) {
			register struct vnode *tvp;

			for (tvp=sa->sa_vlist; tvp; tvp=tvp->v_nextalias) {
				if (tvp == vp)
					break;
			}
			if (tvp != vp)
				panic("makealias: not on vlist");
		}
	}
	SPECHASH_UNLOCK(spechashp);
	return (0);
}

/*
 * Eliminate all activity associated with  the requested vnode
 * and with all vnodes aliased to the requested vnode.
 * 
 * This routine must traverses the alias list and vgone all of the
 * vnodes it finds.
 *
 * Synchronization issues:
 *	-- Wait for other clearalias calls in progress.
 *	-- Wait for in-progress spec_close calls to complete.
 *	   If, while we're sleeping on SA_CLOSING, someone else
 *	   sneaks in and vgone's us, we should exit.  So we re-check
 *	   the vnode after sleeping.
 * 	-- If we encounter a vnode with VXLOCK set, we simply wait for
 *	   it to clear (in vgone), and continue.  Since we have SA_GOING set
 *	   by then, we can guarantee that the alias specinfo structure will
 *	   remain valid.
 */
#ifdef	OSF1_ADFS
void
clearalias(vp)
	struct vnode 	*vp;
#else
void
clearalias(vp, ops)
	register struct vnode *vp;
	struct vnodeops	*ops;
#endif
{
	struct spechash *spechashp;
	register struct specalias *sa;
	register struct specalias *tsa;
	register struct vnode *tvp;
	register enum vtype type;
	register dev_t	rdev;
	int closeit;

#ifdef	OSF1_ADFS
	struct specinfo *si = vp->v_specinfo;
	int error;
        node_t   node;
#endif

loop:
	VN_LOCK(vp);
	/*
	 * Check for a change of state.   Type could only be VBAD if
	 * we've been vgone'd because of reference count.
	 */
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		return; 
	}
	rdev = vp->v_rdev;
	type = vp->v_type;
#ifdef OSF1_ADFS
        node = vp->v_devnode;
#endif
	/*
	 * Don't do anything to mounted devices.
	 */
	if (type == VBLK) {
		VN_UNLOCK(vp);
		if (setmount(vp, SM_MOUNTED))
			return;
		VN_LOCK(vp);
	}
	spechashp = &speclisth[SPECHASH(rdev)];
	SPECHASH_LOCK(spechashp);
	sa = vp->v_alias;
	VN_UNLOCK(vp);
#ifdef	OSF1_ADFS
	/*
	 * If the device represented by the specinfo structure is remote,
	 * the node servicing the device contains the alias list and must,
	 * therefore, perform the clearalias operation.  In this case, wait
	 * for the operation to complete on the remote node and then call
	 * vgone on the local special file vnode.
	 */
	SPEC_READ_LOCK(si);
	BM(SI_LOCK(si));
	if (si->si_flag & SI_RMTDEV) {
		mach_port_t specport = si->si_specport;

		BM(SI_UNLOCK(si));
		SPEC_READ_UNLOCK(si);
		ASSERT(sa == (struct specalias *)0);
		error = remote_clearalias(specport);
		ASSERT(error == 0);
		VN_LOCK(vp);
		(void) vgone(vp, VX_SLEEP,0);
		VN_UNLOCK(vp);
		return;
	} else {
		BM(SI_UNLOCK(si));
		SPEC_READ_UNLOCK(si);
	}
#endif
	/*
p	 * If the device was never opened we don't have an alias 
	 * structure.  Therefore we must search for one, and if found, 
	 * vgone all of its aliases.
	 */
	if (sa == (struct specalias *) 0) {
		for (sa = spechashp->sh_alias;
#ifdef OSF1_ADFS
		     sa && ((sa->sa_rdev != rdev) || (sa->sa_type != type) ||
                            (sa->sa_devnode != node));
#else
		     sa && ((sa->sa_rdev != rdev) || (sa->sa_type != type));
#endif
		     sa = sa->sa_next);
		if (sa == (struct specalias *) 0) {
			SPECHASH_UNLOCK(spechashp);
			return;
		}
	}
	/*
	 * Remember if there had been an active open on the device
	 */
	closeit = sa->sa_usecount;
	/*
	 * The v_alias field MUST be valid.  We know that we have a referenced
	 * device vnode, and its VXLOCK flag was NOT set when we locked the
	 * hashchain.
	 */
	if ((sa->sa_flag & SA_GOING) != 0) {
		/* clearalias in progress */
		sa->sa_flag |= SA_WAIT;
		assert_wait((int)&sa->sa_flag, FALSE);
		SPECHASH_UNLOCK(spechashp);
		thread_block();
		/*
		 * Can return; close couldn't have awoken us, and
		 * interrupts shouldn't have.  If they could, then
		 * we might want to re-check the SA_GOING flag.
		 */
		return;
	}
	if ((sa->sa_flag & SA_CLOSING) != 0) {
		/* spec_close in progress */
		sa->sa_flag |= SA_WAIT;
		assert_wait((int)&sa->sa_flag, FALSE);
		SPECHASH_UNLOCK(spechashp);
		thread_block();
		goto loop;
	}
	/*
	 * We have the hashchain locked; we now lock out 
	 * open/close/clearalias calls by setting SA_GOING.  At that
	 * point it's safe to traverse the list, calling vgone.
	 *
	 * Synchronization NOTES:
	 * -- Vgone will call spec_reclaim, which will
	 *    deallocate the specinfo structure on the hash chain 
	 * -- spec_reclaim will not deallocate the specalias structure
	 *    if SA_GOING is set, so it's always valid here.
	 * -- While SA_GOING is set, NOBODY else can mess with the alias
	 *    list we're working on (outside of vgone/vclean, which
	 *    is synchronized with VXLOCK).
	 */
	sa->sa_flag |= SA_GOING;
	while (tvp = sa->sa_vlist) {
		SPECHASH_UNLOCK(spechashp);
		VN_LOCK(tvp);
#ifdef	OSF1_ADFS
		(void) vgone(tvp, VX_SLEEP, 0);
#else
		(void) vgone(tvp, VX_SLEEP, ops);
#endif
		VN_UNLOCK(tvp);
		SPECHASH_LOCK(spechashp);
	}

	/*
	 * Need to get rid of shadow vnode.  Any associated
	 * buffers were flushed in spec_close.  This vnode is on no
	 * other lists.
	 */
	if (type == VBLK) {
		ASSERT(sa->sa_vnode->v_holdcnt == 0);
		vfree(sa->sa_vnode);
	}
	/*
	 * call the device close if necessary
	 */
	if (closeit) {
		/*
	 	 * Need to unlock the hash chain for the close function.
	 	 */
		SPECHASH_UNLOCK(spechashp);
#ifdef OSF1_ADFS
		speclose(rdev, node, 0, (type == VBLK ? S_IFBLK : S_IFCHR));
#else
		speclose(rdev, 0, (type == VBLK ? S_IFBLK : S_IFCHR));
#endif
		SPECHASH_LOCK(spechashp);
	}

	/*
	 * The vnode list is clean.  Now we remove the alias structure
	 * from the hashchain and deallocate it.
	 */
	tsa = spechashp->sh_alias;
	if (tsa == sa) {
		spechashp->sh_alias = sa->sa_next;
	} else {
		while (tsa && (tsa->sa_next != sa))
			tsa = tsa->sa_next;
		ASSERT(tsa);
		tsa->sa_next = sa->sa_next;
	}

	/*
	 * There is now no way to access the alias structure.  Anyone
	 * who once did is sleeping on it, so we unlock it.
	 */

	SPECHASH_UNLOCK(spechashp);
	/*
	 * wake up sleepers
	 */
	if ((sa->sa_flag & SA_WAIT) != 0) {
		sa->sa_flag &= ~SA_WAIT;
		thread_wakeup(&sa->sa_flag);
	}
	SA_DEALLOCATE(sa);
}

/*
 * Reclaim a special file vnode.  
 *
 * This involves removing it from any alias lists it's on, deallocate
 * its specinfo structure, and possibly deallocating the specalias 
 * structure for the alias.
 *
 * Synchronization NOTES:
 *	-- this funtion is only called from vclean, to remove vnodes from
 *	   alias lists even if they have active reference counts.
 *	-- If SA_GOING is set on the specalias structure, then we are 
 *	   being called as a result of clearalias.  In this case, we leave
 *	   the specalias structure alone, so clearalias can deallocate it.
 *	-- VXLOCK is set, so nothing else can be happening to the vnode.
 *
 * We must be prepared to find no alias list because this routine is
 * called even if there were no opens done on the vnode.
 */
spec_reclaim(vp)
	struct vnode *vp;
{
	struct spechash *spechashp;
	register struct specalias *sa;
#ifdef	OSF1_ADFS
	register struct specinfo *si;
	int error;
#endif

	ASSERT(vp->v_flag & VXLOCK);
#ifdef	OSF1_ADFS
	/*
	 * Acquire the write lock on the specinfo structure to prevent
	 * another thread from using it while it is being destroyed.
	 */
	si = vp->v_specinfo;
	SPEC_WRITE_LOCK(si);
	SI_LOCK(si);
	if (si->si_flag & SI_RMTDEV) {
		/*
		 * If the device is serviced remotely, ensure that the node
		 * managing the device reclaims its pseudo vnode first.
		 * If the reclaim was generated on the node servicing the
		 * special file, set SI_RECLAIM in the specinfo structure's
		 * flag field and forward the message to the remote node.
		 * If the SI_RECLAIM flag has already been set, the node
		 * servicing the device has already reclaimed its pseudo
		 * vnode.  (Note: spec_reclaim messages originating from
		 * the node servicing the device set the SI_RECLAIM flag
		 * in the specinfo structure).
		 */
		ASSERT(si->si_specport != MACH_PORT_NULL);
		ASSERT(vp->v_alias == (struct specalias *)0);
		if (si->si_flag & SI_RECLAIM) {
			SI_UNLOCK(si);
			/*
			 * Deallocate the remote vnode port and the specinfo
			 * structure.
			 */
			mach_port_deallocate(mach_task_self(), si->si_specport);
			SPEC_WRITE_UNLOCK(si);
			SPEC_DEALLOCATE(si);
			vp->v_specinfo = (struct specinfo *)0;
		} else {
			mach_port_t specport = si->si_specport;

			si->si_flag |= SI_RECLAIM;
			SI_UNLOCK(si);
			SPEC_WRITE_UNLOCK(si);
			error = remote_spec_reclaim(specport);
			ASSERT(error == 0);
			/*
			 * The remote node sends a spec_reclaim messaage
			 * to this node once it completes the reclaim
			 * operation and the remote vnode port and specinfo
			 * structure are deallocated above by another thread
			 * on this node .  So by the time the
			 * remote_spec_reclaim returns, both sides have
			 * reclaimed their special files vnodes.
			 */
		}
		return(0);
	} else
		SI_UNLOCK(si);
#endif

	spechashp = &speclisth[SPECHASH(vp->v_rdev)];

	SPECHASH_LOCK(spechashp);
	sa = vp->v_alias;
	/*
	 * Take the vnode off of the alias list
	 */
	if (sa != (struct specalias *) 0) {
		register struct vnode *tvp;
		tvp = sa->sa_vlist;
		if (tvp == vp)
			sa->sa_vlist = vp->v_nextalias;
		else {
			while (tvp && tvp->v_nextalias != vp)
				tvp = tvp->v_nextalias;
			if (tvp) 
				tvp->v_nextalias = vp->v_nextalias;
		}
	}
	/*
	 * Possibly deallocate the specalias structure on the alias list.
	 * It's usecount is changed in makealias and spec_close.
	 * It is valid to have a sa_usecount of 0, but still have a
	 * list attached.  If there are vnodes on the list, don't deallocate.
	 * If clearalias is being used, it is also valid to have an empty
	 * list with sa_usecount > 0.
	 */
	if (sa && (sa->sa_usecount == 0) && ((sa->sa_flag & SA_GOING) == 0) &&
	    (sa->sa_vlist == (struct vnode *) 0)) {
		register struct specalias *tsa;

		tsa = spechashp->sh_alias;
		if (tsa == sa) {
			spechashp->sh_alias = sa->sa_next;
		} else {
			while (tsa && (tsa->sa_next != sa))
				tsa = tsa->sa_next;
			ASSERT(tsa);
			tsa->sa_next = sa->sa_next;
		}
		SPECHASH_UNLOCK(spechashp);
		if (sa->sa_type == VBLK) {
			ASSERT(sa->sa_vnode);
			sa->sa_vnode->v_alias = (struct specalias *) 0;
			vfree(sa->sa_vnode);
		}
		SA_DEALLOCATE(sa);
	} else
		SPECHASH_UNLOCK(spechashp);
#ifdef	OSF1_ADFS
	/*
	 * If the special file is remote, send a spec_reclaim message
	 * to the node servicing it.  When the message returns, deallocate
	 * the specinfo strucutre's remote vnode port.
	 */
	BM(SI_LOCK(si));
	if (si->si_flag & SI_RMTSPEC) {
		BM(SI_UNLOCK(si));
		ASSERT(si->si_specport != MACH_PORT_NULL);
		remote_spec_reclaim(si->si_specport);
		mach_port_deallocate(mach_task_self(), si->si_specport);
	} else
		BM(SI_UNLOCK(si));
	SPEC_WRITE_UNLOCK(si);
#endif
	/*
	 * Deallocate the specinfo structure from the vnode
	 */
	SPEC_DEALLOCATE(vp->v_specinfo);
	vp->v_specinfo = (struct specinfo *) 0;
	return (0);
}


/*
 * Trivial lookup routine that always fails.
 */
spec_lookup(vp, ndp)
	struct vnode *vp;
	struct nameidata *ndp;
{

	ndp->ni_dvp = vp;
	ndp->ni_vp = NULL;
	return (ENOTDIR);
}

/*
 * Open called to allow handler
 * of special files to initialize and
 * validate before actual IO.
 */
/* ARGSUSED */
spec_open(vpp, mode, cred)
	struct vnode **vpp;
	int mode;
	struct ucred *cred;
{
	register struct vnode *vp = *vpp;
	dev_t dev;
	register int maj;
#ifdef	OSF1_ADFS
	mach_port_t	fsport;
	node_t node = vp->v_devnode;
	struct specinfo *si;
	int remote_open = 0;
	int open = 0, readlock = 0;
	int error;
	/*
	 * The assignments below are a bit gross - XXX
	 */
	struct uthread	*uth = &u;
	struct nameidata *ndp = &uth->uu_nd;
	extern mach_port_t vnode_to_fileserver_port();
#if	NORMA_IPC
	extern node_t this_node;
#endif
#endif
	register struct mount *mp;
	enum vtype type;
	int flag; 
	int ret_val = 0;

	VN_LOCK(vp);
	dev = (dev_t) vp->v_rdev;
	maj = major(dev);
	mp = vp->v_mount;
	type = vp->v_type;
	VN_UNLOCK(vp);
	if (mp != DEADMOUNT) {
		BM(MOUNT_LOCK(mp));
		flag = mp->m_flag;
		BM(MOUNT_UNLOCK(mp));
		if (flag & M_NODEV)
			return (ENXIO);
	}

#ifdef	OSF1_ADFS
	/*
	 * Acquire the specinfo structure's read lock.  This prevents a
	 * racing reclaim operation from destroying the special file to device
	 * mapping, if it exists.
	 */
	BM(VN_LOCK(vp));
	si = vp->v_specinfo;
	BM(VN_UNLOCK(vp));
again:
	SPEC_READ_LOCK(si);
	/*
	 * If this is the first open of this special file, acquire the write
	 * lock on the specinfo structure to prevent racing opens from creating
	 * duplicate special file to vnode mappings.
	 */
	BM(SI_LOCK(si));
	if (si->si_flag == SI_CLOSED && si->si_specport == MACH_PORT_NULL) {
		BM(SI_UNLOCK(si));
		SPEC_READ_UNLOCK(si);
		SPEC_WRITE_LOCK(si);
		BM(SI_LOCK(si));
		if (si->si_flag & SI_OPEN) {
			BM(SI_UNLOCK(si));
			SPEC_WRITE_UNLOCK(si);
			goto again;
		}
		BM(SI_UNLOCK(si));
	} else {
		BM(SI_UNLOCK(si));
		readlock++;
	}
	BM(SI_LOCK(si));
	open = (si->si_flag & SI_OPEN);
	BM(SI_UNLOCK(si));

	if (readlock)
		LASSERT(SPEC_READ_HOLDER(si));
	else
		LASSERT(SPEC_WRITE_HOLDER(si));
	VN_LOCK(vp);
	/*
	 * Check for a change of state.   Type could only be VBAD if
	 * we've been vgone'd because of reference count.
	 */
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		if (readlock)
			SPEC_READ_UNLOCK(si);
		else
			SPEC_WRITE_UNLOCK(si);
		return (ENXIO);
	}
	VN_UNLOCK(vp);
	if (!open) {
		BM(SI_LOCK(si));
		ASSERT(si->si_flag == SI_CLOSED);
		BM(SI_UNLOCK(si));
		if (mode & O_RMTDEV) {
			if (si->si_specport == MACH_PORT_NULL)
				LASSERT(SPEC_WRITE_HOLDER(si));
			else
				LASSERT(SPEC_READ_HOLDER(si));
			mode &= ~O_RMTDEV;
			remote_open++;
		} else if (si->si_specport != MACH_PORT_NULL) {
			LASSERT(SPEC_READ_HOLDER(si));
			SI_LOCK(si);
			si->si_flag = SI_OPEN|SI_RMTDEV;
			SI_UNLOCK(si);
		} else {
			mach_port_t	serviceport;
			mach_port_t	vport;
			mach_port_t	devport;
			node_t		servicenode;

			LASSERT(SPEC_WRITE_HOLDER(si));

			/*
			 * Locate the fileservice port on the node servicing
			 * the device.  Then get the vnode port for the special
			 * file and pass it to remote_spec_open which sends a
			 * special files open message to the fileservice port
			 * and returns the device vnode port that maps to the
			 * special file vnode port passed.
			 */
			serviceport = vnode_to_fileserver_port(vp);
 			if (serviceport == MACH_PORT_NULL) {
 				SPEC_WRITE_UNLOCK(si);
 				return(ENXIO);
 			}

#if	!NORMA_IPC
			SI_LOCK(si);
			si->si_flag |= SI_LOCAL;
			SI_UNLOCK(si);
#else	!NORMA_IPC
			/*
			 * If the fileserver port is local, don't send any
			 * messages.
			 */
			if (norma_port_location_hint(mach_task_self(),
			    serviceport, &servicenode) != KERN_SUCCESS)
			    panic("spec_open: can't get serviceport's node");

			if (servicenode == this_node) {
				SI_LOCK(si);
				si->si_flag |= SI_LOCAL;
				SI_UNLOCK(si);
			} else {
				get_vnode_port(vp, &vport);

				if ((uth->uu_syscode == SYS_open) ||
                                    (uth->uu_syscode == 2011)) {
					/*
					 * If the server is handling an
					 * open system call, remote_spec_open
					 * must return a remote file structure
					 * port, in addition to a remote vnode
					 * port. Note 2011 is open-with-token
					 */
					error = remote_spec_open(serviceport,
						node, dev, type, mode|O_RMTDEV,
						vp->v_tag, vport, &fsport,
						&devport);
					if (!error) {
						ASSERT(fsport != MACH_PORT_NULL);
						ASSERT(devport != MACH_PORT_NULL);
						ndp->ni_forwport = fsport;
						/*
						 * The vnode reference will
						 * be released later.
						 */
						error = EREMOTEPORT;
					}
				} else {
					error = remote_spec_open(serviceport,
						node, dev, type, mode|O_RMTDEV,
						vp->v_tag, vport,
						(mach_port_t *)0, &devport);
				}
				if (!error || error == EREMOTEPORT) {
					ASSERT(devport != MACH_PORT_NULL);
					si->si_specport = devport;
					SI_LOCK(si);
					si->si_flag = SI_OPEN|SI_RMTDEV;
					SI_UNLOCK(si);
				}
				SPEC_WRITE_UNLOCK(si);
				return(error);
			}
#endif	/* !NORMA_IPC */

		}
	}

	BM(SI_LOCK(si));
	if (si->si_flag & SI_RMTDEV) {
		/*
		 * If the special file to device mapping has already been
		 * created, forward the open message to the device vnode port
		 * on the node servicing the device.  The device vnode port
		 * is a pseudo vnode for the special file vnode on the node
		 * servicing the special file.
		 */
		ASSERT(si->si_flag & SI_OPEN);
		BM(SI_UNLOCK(si));
		LASSERT(SPEC_READ_HOLDER(si));
		ASSERT(si->si_specport != MACH_PORT_NULL);
		if ((uth->uu_syscode == SYS_open) || (uth->uu_syscode == 2011)) {
			/*
			 * If the server is handling an open system call,
			 * remote_spec_open must return a remote file
			 * structure port, in addition to a remote vnode
			 * port. Note 2011 is open-with-token
			 */
			error = remote_spec_vnode_open(si->si_specport,
				mode|O_RMTDEV, &fsport);
			if (!error) {
				ASSERT(fsport != MACH_PORT_NULL);
				ndp->ni_forwport = fsport;
				/*
				 * The vnode reference will
				 * be released later.
				 */
				error = EREMOTEPORT;
			}
		} else { 
			error = remote_spec_vnode_open(si->si_specport,
				mode|O_RMTDEV, (mach_port_t *)0);
		}
		SPEC_READ_UNLOCK(si);
		return(error);
	} else
		BM(SI_UNLOCK(si));

	/*
	 * Three potential cases exist at this point:  1) both the special
	 * file and device are local (si_flag & SI_LOCAL) 2) this is the 
	 * first open of the device on the node servicing it (si_flag ==
	 * SI_CLOSED) 3) this is a later open of the device on the node
	 * servicing it (si_flag & SI_RMTSPEC).  Most of the open processing
	 * is the same for all three cases.  However, some state must be set up
	 * for the first device open.  Cases 1 and 3 hold the read lock while
	 * case 2 holds the write lock on the specinfo structure.
	 */ 
#endif	/* OSF1_ADFS */

local:
	/*
	 * if makealias returns a non-zero value, it means
	 * there was a problem creating an alias (we could have
	 * been vgone'd), so return an error.
	 */
#ifdef	OSF1_ADFS
	if ((ret_val = makealias(vp)) != 0) {
		if (readlock)
			SPEC_READ_UNLOCK(si);
		else
			SPEC_WRITE_UNLOCK(si);
		return (ret_val);
	}
#else
	if ((ret_val = makealias(vp)) != 0)
		return (ret_val);
#endif
	/*
	 * Can't open a mounted block device
	 */
	if ((type == VBLK) && setmount(vp, SM_MOUNTED))	
		ret_val = EBUSY;
	else
		switch (type) {
		case VCHR:
			if ((u_int)maj >= nchrdev)
				ret_val = ENXIO;
			else
#ifdef	OSF1_ADFS
				/* flag unused */
				CDEVSW_OPEN(maj, dev, mode, S_IFCHR, 
					    &flag, node, ret_val);
#else
				CDEVSW_OPEN(maj, dev, mode, S_IFCHR, 
					    &flag, ret_val); /* flag unused */
#endif
#ifdef LOCAL_DEVNULL
			if (ret_val == ELOCAL) {
			    vp->v_iomode = VIO_DEVNULL;
			    ret_val = ESUCCESS;
			}
#endif /* LOCAL_DEVNULL */
			break;
		case VBLK:
			if ((u_int)maj >= nblkdev)
				ret_val = ENXIO;
			else
#ifdef	OSF1_ADFS
				BDEVSW_OPEN(maj, dev, mode, S_IFBLK, node,
					ret_val);
#else
				BDEVSW_OPEN(maj, dev, mode, S_IFBLK, ret_val);
#endif
			break;
		default:
			panic("spec_open type");
		}
	/*
	 * If there was an error, we have a bogus reference in the 
	 * alias structure, since makealias was called, but
	 * there was a later failure.  We need to clean this up.
	 * Lock order is vnode, then spechash list.
	 */
	if (ret_val) {
		struct specalias *sa; 
		struct spechash *spechashp;
		VN_LOCK(vp);
		type = vp->v_type;
		if ((vp->v_flag & VXLOCK) == 0 && (type != VBAD)) {
			sa = vp->v_alias;
			ASSERT(sa);
			spechashp = &speclisth[SPECHASH(dev)];
			SPECHASH_LOCK(spechashp);
			ASSERT(sa->sa_usecount > 0);
			sa->sa_usecount--;
			SPECHASH_UNLOCK(spechashp);
		}
		VN_UNLOCK(vp);
		/*
		 * If the error is ECLONEME, then the driver wants to be
		 * cloned.  We call spec_clone() to do the work.
		 */
		if (ret_val == ECLONEME) {
			if (type != VCHR)
				ret_val = EINVAL;
			else {
				ret_val = spec_clone(vpp, mode);
			}
		}
	}

#ifdef	OSF1_ADFS
	if (ret_val == 0) {
		/*
		 * Set up the buffer cache block shift:
		 */
		if (type == VBLK) {
			register struct vnode *shadow_vp;

			spec_set_bshift(vp);
			shadow_vp = shadowvnode(vp);
			spec_set_bshift(shadow_vp);
		}
		SI_LOCK(si);
		if (remote_open)
			si->si_flag |= (SI_OPEN|SI_RMTSPEC);
		else
			si->si_flag |= SI_OPEN;
		SI_UNLOCK(si);
	} else if (!open)
		si->si_flag = SI_CLOSED;

	if (readlock)
		SPEC_READ_UNLOCK(si);
	else
		SPEC_WRITE_UNLOCK(si);
#endif
	return(ret_val);
}

/*
 * The next few functions deal with clone devices -- their
 * creation and use.
 */
/*
 * Initialize a newly cloned vnode.
 *
 * -- fill in spec_node data structure.
 * -- cannot fail.
 * -- nobody knows about the vnode yet.
 */

void
spec_clone_init(vp)
	struct vnode *vp;
{
	struct spec_node *sp;
	register struct vattr *vap;
	struct timeval tval;

	vp->v_op = &spec_cloneops;
	sp = VTOS(vp);
	sp->sn_vnode = vp;
	vap = &sp->sn_vattr;
	bzero(vap, sizeof(struct vattr));
	vap->va_type = VCHR;
	/*
	 * This is a bit ugly, but we want a unique dev, ino pair, so
	 * we use the rdev for the dev, and the address of the vnode for
	 * the inode.  Since it is a clone, the vp will guarantee it is 
	 * unique, regardless of the file system on which the name is 
	 * located.
	 */
	vap->va_fsid = (long) vp->v_rdev;
	vap->va_fileid = (long) vp;
	vap->va_blocksize = MAXBSIZE;
	vap->va_rdev = vp->v_rdev;
	/*
	 * Now do the time -- use current time
	 */
	microtime(&tval);
	vap->va_atime = vap->va_mtime = vap->va_ctime = tval;
	
}
/*
 * Clone a vnode.
 * Algorithm:
 *	1.  Allocating and initializing a new (anonymous) vnode.
 *	2.  Call the driver with the O_DOCLONE flag set in the mode field.
 *	3.  Clean up if there is an error.
 * NB:  Clone vnodes never get on any lists or hash chains, other than
 *	the alias list.  They are guaranteed unique, and filesystem
 *	independent.
 */
spec_clone(vpp, mode)
	struct vnode **vpp;
	int mode;
{
	register struct vnode *vp = *vpp;
	struct vnode *newvp;
	int error = 0;
	dev_t dev, newdev;
	int maj;
#ifdef	OSF1_ADFS
	node_t node;
#endif
	extern void vfree();

	BM(VN_LOCK(vp));
	dev = vp->v_rdev;
	BM(VN_UNLOCK(vp));
	maj = major(dev);
#ifdef	OSF1_ADFS
	node = vp->v_devnode;
#endif
	/*
	 * Allocate and initialize the new vnode
	 */
#ifdef	OSF1_ADFS
	if ((error = bdevvp(dev, node, VCHR, &newvp)) == 0)
#else
	if ((error = bdevvp(dev, &newvp)) == 0)
#endif
	{
		/*
	 	 * Nobody knows about new vnode yet; no locking required.
	 	 */
#ifndef	OSF1_ADFS
		newvp->v_type = VCHR;	/* bdevvp returns VBLK by default */

		CDEVSW_OPEN(maj, dev, mode|O_DOCLONE, S_IFCHR, &newdev, error);
#else
		CDEVSW_OPEN(maj, dev, mode|O_DOCLONE, S_IFCHR, &newdev, node,
			error);
#endif
		if (error == 0) {
			newvp->v_rdev = newdev;
			spec_clone_init(newvp);
			error = makealias(newvp);
		}
		if (error == 0) {
			*vpp = newvp;
			vrele(vp);
		} else
			vfree(newvp);
	}
	return (error);
}

/*
 * spec_settime.
 *
 * Called by clone operations to update access and modification
 * times on the vnode.
 */

void
spec_settime(vp, tvalp)
	struct vnode *vp;
	struct timeval *tvalp;
{
	struct timeval tval;

	microtime(&tval);
	VN_LOCK(vp);
	*tvalp = tval;
	VN_UNLOCK(vp);
}

/*
 * spec_clread
 *
 * Intercept operation for clone reads -- updates access time.
 * Safe from changes to dead ops, since no dangerous data structures
 * are touched, and the spec_node is always present, if referenced.
 */
spec_clread(vp, uio, ioflag, cred)
	register struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{

	spec_settime(vp, &(VTOS(vp))->sn_vattr.va_atime);
	return (spec_read(vp, uio, ioflag, cred));
}

/*
 * spec_clwrite
 *
 * Intercept operation for clone writes -- updates modify and creation times.
 * Safe from changes to dead ops, since no dangerous data structures
 * are touched, and the spec_node is always present, if referenced.
 */
spec_clwrite(vp, uio, ioflag, cred)
	register struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{

	register struct spec_node *sp = VTOS(vp);

	spec_settime(vp, &sp->sn_vattr.va_mtime);
	/* use the same time */
	VN_LOCK(vp);
	sp->sn_vattr.va_ctime = sp->sn_vattr.va_mtime;
	VN_UNLOCK(vp);
	return (spec_write(vp, uio, ioflag, cred));
}

/*
 * spec_getattr
 *
 * Used by clone devices for fstat.  Returns as much of
 * a struct vattr as makes sense for clones.
 * NOTES:
 *	Synchronization controlled by vnode spin lock.
 */
spec_getattr(vp, vap, cred)
	struct vnode *vp;
	struct vattr *vap;
	struct ucred *cred;
{
	VN_LOCK(vp);
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		return (EBADF); 
	}
	ASSERT(vp->v_type == VCHR);
	bcopy(&(VTOS(vp)->sn_vattr), vap, sizeof(struct vattr));
	VN_UNLOCK(vp);
	return (0);
}

/*
 * Vnode op for read
 */
spec_read(vp, uio, ioflag, cred)
	register struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	struct buf *bp;
	daddr_t bn;
	long bsize, bscale;
	struct partinfo dpart;
	register int n, on;
	int error = 0;
	extern int mem_no;
	enum vtype type;
	dev_t rdev;
	struct vnode *tvp;
#ifdef	OSF1_ADFS
	struct specinfo *si;
        node_t       node;
#endif
	if (uio->uio_rw != UIO_READ)
		panic("spec_read mode");
	if (uio->uio_resid == 0)
		return (0);
	VN_LOCK(vp);
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		return (EIO); 
	}
	type = vp->v_type;
	tvp = vp->v_shadowvp;	/* in case we need it */
	rdev = vp->v_rdev;
#ifdef	OSF1_ADFS
        node = vp->v_devnode;
	si = vp->v_specinfo;
#endif
	VN_UNLOCK(vp);

#ifdef	OSF1_ADFS
	/*
	 * If the device is remote, send a spec_read message to its
	 * remote vnode agent.  This is a relatively remote occurrence.
	 * Most device reads are preceeded by open system calls. The open
	 * establishes a mapping between the special file and its remote
	 * device and returns a file structure port on the node servicing
	 * the device.  All reads/writes, etc. operating on the file
	 * structure port are directed at the node servicing the device.
	 */
	if (si->si_flag & SI_RMTDEV) {
		error = remote_spec_read(si->si_specport, uio, ioflag);
		return(error);
	}
#endif

	switch (type) {
	case VCHR:
#if 0	/* XXX barbou@gr.osf.org: uio_offset is unsigned, sigh ! */
		/*
		 * Negative offsets allowed only for /dev/kmem.
		 */
		if (uio->uio_offset < 0 && major(rdev) != mem_no)
			return (EINVAL);
#endif
#ifdef OSF1_ADFS
		CDEVSW_READ(major(rdev), rdev, node, uio, ioflag, error);
#else
		CDEVSW_READ(major(rdev), rdev, uio, ioflag, error);
#endif
		return(error);
	case VBLK:
		/*
		 * Switch to shadow vnode.
		 */
		ASSERT(tvp);
		vp = tvp;

		if (uio->uio_offset < 0)
			return (EINVAL);
		bscale = 1 << vp->v_bufhash_shift;
		bsize = dgtob(bscale);
		do {
			bn = btodg(uio->uio_offset) &~ (bscale - 1);
			on = uio->uio_offset % bsize;
			n = MIN((unsigned)(bsize - on), uio->uio_resid);
			VN_LOCK(vp);
			if (vp->v_lastr + bscale == bn) {
				VN_UNLOCK(vp);
				error = breada(vp, bn, (int)bsize, bn + bscale,
					(int)bsize, NOCRED, &bp);
			} else {
				VN_UNLOCK(vp);
				error = bread(vp, bn, (int)bsize, NOCRED, &bp);
			}
			VN_LOCK(vp);
			vp->v_lastr = bn;
			VN_UNLOCK(vp);
			n = MIN(n, bsize - bp->b_resid);
			if (error) {
				brelse(bp);
				return (error);
			}
			error = uiomove(bp->b_un.b_addr + on, n, uio);
			if (n + on == bsize)
				bp->b_flags |= B_AGE;
			brelse(bp);
		} while (error == 0 && uio->uio_resid > 0 && n != 0);
		return (error);
	default:
		panic("spec_read type");
	}
	/* NOTREACHED */
}

/*
 * Vnode op for write
 */
spec_write(vp, uio, ioflag, cred)
	register struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	struct buf *bp;
	daddr_t bn;
	int bsize, blkmask;
	struct partinfo dpart;
	register int n, on;
	int error = 0;
	struct vnode *tvp;
	dev_t rdev;
	enum vtype type;
#if	!MACH
	int count;
#endif
#ifdef	OSF1_ADFS
	struct specinfo *si;
        node_t     node;
#endif
	extern int mem_no;

	if (uio->uio_rw != UIO_WRITE)
		panic("spec_write mode");
	VN_LOCK(vp);
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		return (EIO); 
	}
	type = vp->v_type;
	tvp = vp->v_shadowvp;	/* in case we need it */
	rdev = vp->v_rdev;
#ifdef	OSF1_ADFS
        node = vp->v_devnode;
	si = vp->v_specinfo;
#endif
	VN_UNLOCK(vp);
#ifdef	OSF1_ADFS
	/*
	 * If the device is remote, send a spec_write message to its
	 * remote vnode agent.  This is a relatively remote occurrence.
	 * Most device writes are preceeded by open system calls. The open
	 * establishes a mapping between the special file and its remote
	 * device and returns a file structure port on the node servicing
	 * the device.  All reads/writes, etc. operating on the file
	 * structure port are directed at the node servicing the device.
	 */
	if (si->si_flag & SI_RMTDEV) {
		error = remote_spec_write(si->si_specport, uio, ioflag);
		return(error);
	}
#endif
	switch (type) {
	case VCHR:
#if 0	/* XXX barbou@gr.osf.org: uio_offset is unsigned, sigh ! */
		/*
		 * Negative offsets allowed only for /dev/kmem
		 */
		if (uio->uio_offset < 0 && major(rdev) != mem_no)
			return (EINVAL);
#endif
#ifdef OSF1_ADFS
		CDEVSW_WRITE(major(rdev), rdev, node, uio, ioflag, error);
#else
		CDEVSW_WRITE(major(rdev), rdev, uio, ioflag, error);
#endif
		return(error);
	case VBLK:
		/*
		 * Switch to shadow vnode.
		 */
		ASSERT(tvp);
		vp = tvp;

		if (uio->uio_resid == 0)
			return (0);
		if (uio->uio_offset < 0)
			return (EINVAL);
		blkmask = 1 << vp->v_bufhash_shift;
		bsize = dgtob(blkmask);
		blkmask--;
		error = 0;
		do {
			bn = btodg(uio->uio_offset) &~ blkmask;
			on = uio->uio_offset % bsize;
			n = MIN((unsigned)(bsize - on), uio->uio_resid);
#if	MACH
			/*
			 * XXX -- which vnode is mapped?  real or shadow?
			 */
			VN_LOCK(vp);
			if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
				VN_UNLOCK(vp);
				inode_uncache(vp);
			} else
				VN_UNLOCK(vp);
#else
			count = howmany(bsize, CLBYTES);
			for (i = 0; i < count; i++)
				munhash(vp, bn + i * btodg(CLBYTES));
#endif
			if (n == bsize)
				bp = getblk(vp, bn, bsize);
			else
				error = bread(vp, bn, bsize, NOCRED, &bp);
			n = MIN(n, bsize - bp->b_resid);
			if (error) {
				brelse(bp);
				return (error);
			}
			error = uiomove(bp->b_un.b_addr + on, n, uio);
			if (n + on == bsize) {
				bp->b_flags |= B_AGE;
				bawrite(bp);
			} else
				bdwrite(bp, bp->b_vp);
		} while (error == 0 && uio->uio_resid > 0 && n != 0);
		return (error);
	default:
		panic("spec_write type");
	}
	/* NOTREACHED */
}

/*
 * Device ioctl operation.
 */
/* ARGSUSED */
spec_ioctl(vp, com, data, fflag, cred)
	struct vnode *vp;
	int com;
	caddr_t data;
	int fflag;
	struct ucred *cred;
{
	dev_t dev;
	int error, flags;
	enum vtype type;
#ifdef	OSF1_ADFS
	struct specinfo *si;
        node_t          node;
#endif

	VN_LOCK(vp);
	/*
	 * Check for a change of state.   Type could only be VBAD if
	 * we've been vgone'd because of reference count.
	 */
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		return (EIO); 
	}
	type = vp->v_type;
	dev = vp->v_rdev;
#ifdef	OSF1_ADFS
        node = vp->v_devnode;
	si = vp->v_specinfo;
#endif
	VN_UNLOCK(vp);
#ifdef	OSF1_ADFS
	/*
	 * Don't think ioctls are ever forwarded in this fashion.  But
	 * if they are, lots of extra work is needed.  If we panic here,
	 * then we have work to do. - XXX
	 */
	if (si->si_flag & SI_RMTDEV) {
		ASSERT(si->si_specport != MACH_PORT_NULL);
		error = remote_spec_ioctl(si->si_specport, com, data, fflag,
			cred);
		return(error);
	}
#endif
	switch (type) {
	case VCHR:
#ifdef OSF1_ADFS
		CDEVSW_IOCTL(major(dev), dev, node, com, data, fflag, error);
#else
		CDEVSW_IOCTL(major(dev), dev, com, data, fflag, error);
#endif
		return (error);
	case VBLK:
		if (com == 0 && (int)data == B_TAPE) {
			BDEVSW_FLAGS(major(dev),flags);
			if (flags & B_TAPE)
				return (0);
			else
				return (1);
		}
#ifdef OSF1_ADFS
		BDEVSW_IOCTL(major(dev), dev, node, com, data, fflag, error);
#else
		BDEVSW_IOCTL(major(dev), dev, com, data, fflag, error);
#endif
		return(error);
	default:
		panic("spec_ioctl");
		/* NOTREACHED */
	}
}

int
pageio(vp, uio, rwflag, pager, offset)
	struct vnode *vp;
	register struct uio *uio;
	int rwflag;
	memory_object_t pager;
	vm_offset_t offset;
{
	register struct buf *bp;
	register struct iovec *iov;
#ifndef	OSF1_SERVER
	vm_offset_t kvaddr;
#endif
	int dev, type;
	int error;
#if	ASYNC_PAGEOUT
	extern void pageout_done();
#endif
#ifdef OSF1_ADFS
        node_t node;
#endif

	error = 0;
	iov = uio->uio_iov;
	VN_LOCK(vp);
	type = vp->v_type;
	dev  = vp->v_rdev;
#ifdef OSF1_ADFS
        node = vp->v_devnode;
#endif
	VN_UNLOCK(vp);

	assert(type == VBLK);
	/*
	 * Get a buffer header, and set up to
	 * call the strategy routine.  If paging
	 * into a physical page, establish a
	 * temporary kernel VA mapping to the
	 * page.
	 */
	SB_ALLOCATE(bp);
	event_init(&bp->b_iocomplete);
	bp->b_error = 0;
	bp->b_dev = dev;
#ifdef OSF1_ADFS
        bp->b_devnode = node;
#endif
	bp->b_bcount = bp->b_bufsize = iov->iov_len;
	bp->b_blkno = btodg(uio->uio_offset);
	bp->b_lblkno = btodg(offset);
	bp->b_iodone = NULL;
	bp->b_pager = pager;
#ifndef	OSF1_SERVER
	bp->b_proc = NULL;
#else
	bp->b_optimize_mem = FALSE;
#endif
	bp->b_vp = NULLVP;

	/*
	 *	Even though this is a private buffer that no one else
	 *	can possibly know about, device drivers expect it to
	 *	be locked when passed to them.
	 */
	BUF_LOCKINIT(bp);
	BUF_LOCK(bp);

	bp->b_flags = rwflag;
#ifndef	OSF1_SERVER
	if (uio->uio_segflg == UIO_PHYSSPACE) {
		/* Only pageins done to physical memory */
		kvaddr = map_physical_page(iov->iov_base);
		bp->b_un.b_addr = (caddr_t)kvaddr;
	}
	else {
		extern task_t vnode_pager_task;

#endif	/* OSF1_SERVER */
		bp->b_un.b_addr = iov->iov_base;
		if (rwflag == B_WRITE) {
			/* pageout */
#ifndef	OSF1_SERVER
			if (vm_map_pageable(vnode_pager_task->map,
			    bp->b_un.b_addr,
			    bp->b_un.b_addr + bp->b_bcount,
			    VM_PROT_READ) != KERN_SUCCESS) {
				error = EFAULT;
				goto out;
			}
#endif	/* OSF1_SERVER */
			VN_OUTPUT_LOCK(vp);
			vp->v_numoutput++;
			VN_OUTPUT_UNLOCK(vp);
#if	ASYNC_PAGEOUT
			bp->b_iodone = pageout_done;
#endif
		}
#ifndef	OSF1_SERVER
	}
#endif	/* OSF1_SERVER */

	BDEVSW_STRATEGY(major(dev), bp, error);

#if	ASYNC_PAGEOUT
	if (rwflag == B_WRITE)
		return 0;
	else
#endif
		error = biowait(bp);

#ifndef	OSF1_SERVER
	if (uio->uio_segflg == UIO_PHYSSPACE)
		unmap_physical_page(kvaddr);
#endif	/* OSF1_SERVER */
	/*
	 * else no need to unwire, since the memory will
	 * be vm_deallocated shortly if this is a pageout.
	 */
out:
	BUF_UNLOCK(bp);
	SB_DEALLOCATE(bp);
	if (error)
		printf("error %d on page%s (spec_%s)\n", error,
			rwflag == B_READ ? "in" : "out",
			rwflag == B_READ ? "read" : "write");
	return error;
}

#if	ASYNC_PAGEOUT
/*
 * After a page write has completed, queue the buffer describing
 * the I/O, and wakeup the vnode_pager_cleanup thread.
 *
 * NOTE: Currently, only pageins are done directly to physical
 *	 memory.  If pageouts are ever done from physical memory,
 *	 then this routine will need to call unmap_physical_page().
 *	 A flag will need to be set in the buffer to indicate that
 *	 b_addr is a mapped physical page.
 */
void
pageout_done(bp)
	register struct buf *bp;
{
	if (bp->b_flags & B_ERROR)
		printf("error %d on pageout\n", bp->b_error);

	vnode_pager_cleanup_wakeup(bp);
}
#endif	/* ASYNC_PAGEOUT */

/* ARGSUSED */
spec_page_read(vp, uio, cred)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
{
	return pageio(vp, uio, B_READ, 0, 0);
}

/* ARGSUSED */
spec_page_write(vp, uio, cred, pager, offset)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
	memory_object_t pager;
	vm_offset_t offset;
{
	return pageio(vp, uio, B_WRITE, pager, offset);
}

/* ARGSUSED */
spec_select(vp, events, revents, scanning, cred)
	struct vnode *vp;
	short *events, *revents;
	int scanning;
	struct ucred *cred;
{
	register dev_t dev;
	enum vtype type;
	int error;
#ifdef	OSF1_ADFS
	struct specinfo *si;
        node_t          node;
#endif

	VN_LOCK(vp);
	/*
	 * Check for a change of state.  Type could only be VBAD if
	 * we've been vgone'd because of reference count.
	 */
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		printf("spec_select: bad vnode\n");
		return (0);
	}
	type = vp->v_type;
	dev = vp->v_rdev;
#ifdef	OSF1_ADFS
	si = vp->v_specinfo;
        node = vp->v_devnode;
#endif
	VN_UNLOCK(vp);
#ifdef	OSF1_ADFS
	/*
	 * Don't think selects are ever forwarded in this fashion.  But
	 * if they are, lots of extra work is needed.  If we panic here,
	 * then we have work to do. - XXX
	 */
	if (si->si_flag & SI_RMTDEV)
		panic("bad remote spec_select");
#endif
	switch (type) {

	default:
		return (0);		/* no error XXX */

	case VCHR:
#ifdef OSF1_ADFS
		CDEVSW_SELECT(major(dev),dev,node,events,revents,
                              scanning,error);
#else
		CDEVSW_SELECT(major(dev),dev,events,revents,scanning,error);
#endif
		return (error);
	}
}

/*
 * Seek on a special file.  This always succeeds.
 */
/* ARGSUSED */
spec_seek(vp, oldoff, newoff, cred)
	struct vnode *vp;
	off_t oldoff, newoff;
	struct ucred *cred;
{
	return(0);
}

/*
 * Just call the device strategy routine
 */
spec_strategy(bp)
	register struct buf *bp;
{
	int error;

	BDEVSW_STRATEGY(major(bp->b_dev), bp, error);
	return (0);
}

/*
 * This is a noop, simply returning what one has been given.
 */
spec_bmap(vp, bn, vpp, bnp)
	struct vnode *vp;
	daddr_t bn;
	struct vnode **vpp;
	daddr_t *bnp;
{

	if (vpp != NULL)
		*vpp = vp;
	if (bnp != NULL)
		*bnp = bn;
	return (0);
}

/*
 * The function that calls the actual devsw close.  It is separate
 * so the code does not need to be duplicated in spec_close.
 */
#ifdef OSF1_ADFS
speclose(dev, node, flag, mode)
#else
speclose(dev, flag, mode)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
	int flag;
	int mode;
{
	register int error;

	if (mode == S_IFCHR)
#ifdef OSF1_ADFS
		CDEVSW_CLOSE(major(dev), dev, node, flag, mode, error);
#else
		CDEVSW_CLOSE(major(dev), dev, flag, mode, error);
#endif
	else
#ifdef OSF1_ADFS
		BDEVSW_CLOSE(major(dev), dev, node, flag, mode, error);
#else
		BDEVSW_CLOSE(major(dev), dev, flag, mode, error);
#endif
	return (error);
}

/*
 * Device close routine
 * Synchronization:
 *	-- lock order: vnode, then spec hashchain lock.
 */
/* ARGSUSED */
spec_close(vp, flag, cred)
	register struct vnode *vp;
	int flag;
	struct ucred *cred;
{
	int mode;
	int error = 0;
	register struct specalias *sa;
	struct spechash *spechashp;
	enum vtype type;
	dev_t rdev;
	struct vnode *tvp;
#ifdef	OSF1_ADFS
	struct uthread *uth = &u;
	struct specinfo *si;
        node_t          node;
#endif

	/*
	 * Synchronization issue:
	 * If this vnode has been vgone'd, or is in the process of
	 * being vgone'd, then we have nothing to do.  The major issue
	 * is making sure that the driver close routine is called.
	 * If wer're in the process of vgone'ing the vnode,
	 * then it's due to one of the following: 
	 * clearalias, getnewvnode, or unlink.
	 * Clearalias will call the driver close routine, 
	 * if required.  If it's one of the latter two conditions,
	 * then it has already been called, via a close() call.
	 */
	VN_LOCK(vp);
	type = vp->v_type;
	if ((type == VBAD) || (vp->v_flag & VXLOCK)) {
		VN_UNLOCK(vp);
		return (0); 
	}
	tvp = vp->v_shadowvp;	/* in case we need it */
	rdev = vp->v_rdev;
	si = vp->v_specinfo;
#ifdef OSF1_ADFS
        node = vp->v_devnode;
#endif
	VN_UNLOCK(vp);

#ifdef	OSF1_ADFS
	/*
	 * Acquire the specinfo structure's read lock to prevent racing
	 * reclaim operations from destroying the special file to device
	 * mapping while the file is being closed.  Note: the mapping is
	 * kept intact even after the last close.  It is only destroyed
	 * when the either this vnode or its remote counterpart must be
	 * reclaimed.
	 */
	SPEC_READ_LOCK(si);
	BM(SI_LOCK(si));
	if (si->si_flag & SI_RMTDEV) {
		/*
		 * If the device represented by this special file vnode
		 * is remote, we shouldn't have gotten here by a close system
		 * call.  All system calls operating on file descriptors
		 * representing remote devices are targeted at the file
		 * structure port on the remote node.  Thus, if the device is
		 * remote, we got here via an internal close operation.
		 * Call remote_spec_close to send a close message to the
		 * remote node so it can close the device, if needed.
		 */
		BM(SI_UNLOCK(si));
		ASSERT(uth->uu_syscode != SYS_close);
		/*
		 * The remote node will send back another spec_close
		 * message that will mark this specinfo structure as
		 * SI_CLOSED.
		 */
		(void)remote_spec_close(si->si_specport, flag);
		SPEC_READ_UNLOCK(si);
		return(0);
	} else
		BM(SI_UNLOCK(si));
	/*
	 * Two cases are possible: 1) the special file and its device are
	 * both local. 2) the special file is remote, but the device is
	 * local.
	 */
#endif

	if (type == VBLK) {
		/*
		 * On last close of a block device (that isn't mounted)
		 * we must invalidate any in core blocks, so that
		 * we can, for instance, change floppy disks.
		 *
		 * Must use the shadowvp, since it's the one used for
		 * reads and writes.  Only do this, if the block device
		 * has been opened (i.e. we have allocated a shadow vnode).
		 *
		 * This code used to simply return 0 if vinvalbuf returned
		 * a non-zero value.  This indicates a busy vnode/alias,
		 * but we can't do this; the usecount on the alias struct.
		 * needs to be decremented, so we just drop through and
		 * let things take care of themselves.  If the alias is
		 * busy, the usecount will catch it.
		 */
		if (tvp) {
			(void) vflushbuf(tvp, 0);
			(void) vinvalbuf(tvp, 1);
		}
		/*
		 * Also flush the real vp since it's the one used for
		 * filesystem data (inodes, bitmap).
		 */
		(void) vflushbuf(vp, B_SYNC);
		(void) vinvalbuf(vp, 1);
		mode = S_IFBLK;
	} else if (type == VCHR)
		mode = S_IFCHR;
	else
		panic("spec_close: bad type");


	VN_LOCK(vp);
	if (vp->v_type == VBAD) {
		VN_UNLOCK(vp);
#ifdef	OSF1_ADFS
		SPEC_READ_UNLOCK(si);
#endif
		return (0); 
	}
	spechashp = &speclisth[SPECHASH(rdev)];
	SPECHASH_LOCK(spechashp);
	sa = vp->v_alias;
	VN_UNLOCK(vp);
	if (sa == (struct specalias *) 0) {
		SPECHASH_UNLOCK(spechashp);
#ifdef	OSF1_ADFS
		SPEC_READ_UNLOCK(si);
#endif
		return (0);
	}
	/*
 	 * If we're in the middle of clearalias (SA_GOING set), don't do
	 * the close, clearalias will do it (once and only once).  No need
	 * to change the sa_usecount -- it'll be bogus anyway.
	 */
	if ((sa->sa_flag & SA_GOING) != 0) {
		SPECHASH_UNLOCK(spechashp);
#ifdef	OSF1_ADFS
		SPEC_READ_UNLOCK(si);
#endif
		return (0);
	}
	/*
	 * Synchronization:
	 *
	 * We don't really want to close the device if it is still in
	 * use, unless we are trying to close it forcibly (done above).
	 * The sa_usecount field of the specalias structure holds the
	 * count of active users of the device.  When it goes to zero,
	 * we call the device close.
	 *
	 * The vnode is unlinked from the alias list and the specinfo
	 * and specalias structures associated are deallocated in spec_reclaim.
	 */
	ASSERT(sa->sa_usecount > 0);
	if (--sa->sa_usecount == 0) {
		/*
		 * This is the last close on this device.  We want
		 * to call the devsw.d_close, and then deallocate
		 * this guy.
		 */
		ASSERT((sa->sa_flag & SA_CLOSING) == 0);
		sa->sa_flag |= SA_CLOSING;
		SPECHASH_UNLOCK(spechashp);
#ifdef	OSF1_ADFS
		BM(SI_LOCK(si));
		if (si->si_flag & SI_RMTSPEC) {
			/*
			 * Let the remote node mark its specinfo structure
			 * as closed and release the reference on its special
			 * file vnode.  Then mark this specinfo structure as
			 * closed.
			 */
			BM(SI_UNLOCK(si));
			ASSERT(si->si_specport != MACH_PORT_NULL);
			error = remote_spec_close(si->si_specport, flag);
			SI_LOCK(si);
			si->si_flag = SI_CLOSED;
			SI_UNLOCK(si);
		} else
			BM(SI_UNLOCK(si));

		error = speclose(rdev, node, flag, mode);
#else
		error = speclose(rdev, flag, mode);
#endif
		SPECHASH_LOCK(spechashp);
		sa->sa_flag &= ~SA_CLOSING;

		ASSERT(sa->sa_usecount == 0);	/* better be unused */
		/*
		 * Wake up sleepers
		 */
		if ((sa->sa_flag & SA_WAIT) != 0) {
			sa->sa_flag &= ~SA_WAIT;
			SPECHASH_UNLOCK(spechashp);
			thread_wakeup(&sa->sa_flag);
		} else
			SPECHASH_UNLOCK(spechashp);
	} else {
		SPECHASH_UNLOCK(spechashp);
#ifdef	OSF1_ADFS
		BM(SI_LOCK(si));
		if (si->si_flag & SI_RMTSPEC) {
			/*
			 * The remote node must release the vnode reference
			 * acquired by the open.
			 */
			BM(SI_UNLOCK(si));
			ASSERT(si->si_specport != MACH_PORT_NULL);
		} else
			BM(SI_UNLOCK(si));
#endif
	}
#ifdef	OSF1_ADFS
	SPEC_READ_UNLOCK(si);
#endif
	return (error);
}


/*
 * Print out the contents of a special device vnode.
 */
spec_print(vp)
	struct vnode *vp;
{
	dev_t rdev;
	VN_LOCK(vp);
	/*
	 * Check for a change of state.   Type could only be VBAD if
	 * we've been vgone'd because of reference count.
	 */
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		printf("spec_print: type == VBAD\n");
		return; 
	}
	rdev = vp->v_rdev;
	VN_UNLOCK(vp);
	printf("tag VT_NON, dev %d, %d\n", major(rdev),
		minor(rdev));
}

/*
 * Special device failed operation
 */
spec_ebadf()
{

	return (EBADF);
}

/*
 * Special device bad operation
 */
spec_badop()
{

	panic("spec_badop called");
	/* NOTREACHED */
}

/*
 * Special device null operation
 */
spec_nullop()
{

	return (0);
}


/*
 * Lookup a vnode by device number.
 */
vfinddev(dev, type, vpp)
	dev_t dev;
	enum vtype type;
	struct vnode **vpp;
{
	int ret = 1;
	struct spechash *spechashp = &speclisth[SPECHASH(dev)];
	register struct specalias *sa;

	SPECHASH_LOCK(spechashp);
	for (sa = spechashp->sh_alias; 
	     sa && ((sa->sa_rdev != dev) || (sa->sa_type != type)); 
	     sa = sa->sa_next);
	/*
	 * If we got a hit, just return the first one on the list.
	 * There better be one.  The assertions are debug; this
	 * could be condensed into one if statement upon their removal.
	 * NOTE:  this is only used by mfs_strategy now.  It expects
	 *	  a referenced vnode.
	 */
	if (sa != (struct specalias *) 0) {
		ASSERT(sa->sa_vlist != (struct vnode *) 0);
		if (sa->sa_vlist != (struct vnode *) 0) {
			*vpp = sa->sa_vlist;
			ASSERT((*vpp)->v_usecount > 0);
			ret = 0;
		}
	}
	SPECHASH_UNLOCK(spechashp);
	return (ret);
}

/*
 * Change from block vnode to its shadow vnode for the purposes
 * of the buffer cache.
 *
 * Called from mntflushbuf and mntinvalbuf.
 * Assumptions:
 *	-- caller verified that v_type is VBLK.
 * Synchronization:
 *	-- takes a locked vnode, and returns it locked.
 */
struct vnode *
shadowvnode(vp)
	struct vnode *vp;
{
	ASSERT(vp->v_type == VBLK);
	if (vp->v_flag & VXLOCK) 
		return ((struct vnode *) 0);
	/*
	 * Although the v_shadowvp field is under protection of the
	 * spec hashchain lock, we know that it's safe here, since it
	 * will only disappear under VXLOCK, and that can only be set
	 * under vnode lock.
	 */
	return (vp->v_shadowvp);
}

/*
 * spec_setopen
 *
 * This is a special purpose function for use by routines that bypass
 * spec_open(), but do the same thing.  That is, reference a vnode and
 * use it as an open file.
 *
 * Currently, the only client of this function is ioctl (TIOCSCTTY) for
 * setting the controlling tty.
 *
 * NOTES:  the type of this vnode shouldn't change.  It won't change on
 *	   forcible unmount, and we're the session leader (for the ioctl)
 *	   so no one else can be doing clearalias on us.  If these 
 * 	   assumptions aren't valid, we should do some sanity checking on
 *	   v_type and v_flag (see assertions).
 */
void
spec_setopen(vp)
	struct vnode *vp;
{
	struct spechash *spechashp;
	register struct specalias *sa;

	VN_LOCK(vp);
	ASSERT(vp->v_type != VBAD);
	ASSERT((vp->v_flag & VXLOCK) == 0);
	spechashp = &speclisth[SPECHASH(vp->v_rdev)];
	sa = vp->v_alias;
	++vp->v_usecount;
	VN_UNLOCK(vp);

	ASSERT(sa);
	SPECHASH_LOCK(spechashp);
	++sa->sa_usecount;
	SPECHASH_UNLOCK(spechashp);
}


/*
 * Several functions in one:
 *
 * 1.  Check to see if a device vnode is open (mount)
 * 2.  Check to see if a device vnode is mounted (spec_open)
 * 3.  Mark a device vnode as mounted (mount)
 * 4.  Mark a device vnode as NOT mounted (unmount)
 *
 * It depends on the flag argument passed, mode.
 * 	SM_OPEN		-- ret. EBUSY if device is open.
 * 	SM_MOUNTED	-- ret. EBUSY if device is mounted.
 * 	SM_SETMOUNT	-- set mounted flag.
 * 	SM_CLEARMOUNT	-- clear mounted flag.
 *
 * NOTES:
 * The check for open devices assumes that open has been called, which
 * means that it will return EBUSY if the usecount is != 1, not 0.
 *
 * For this function, it is possible that, if forcible
 * unmounts are taking place, for the vp to be in a state of transition.
 * The file system upon which the device file was located could be on its
 * way out.  If this happens, the v_type and v_rdev should remain 
 * consistent, in any case, since devices are only dissociated from their
 * file systems, not totally destroyed.
 */
int
setmount(vp, mode)
	register struct vnode *vp;
	register int mode;
{
	register struct specalias *sa;
	struct spechash *spechashp;
	int ret = 0;
	
	VN_LOCK(vp);
	if ((vp->v_flag & VXLOCK) || (vp->v_type == VBAD)) {
		VN_UNLOCK(vp);
		return (EBUSY); 
	}
	sa = vp->v_alias;
	spechashp = &speclisth[SPECHASH(vp->v_rdev)];
	SPECHASH_LOCK(spechashp);
	VN_UNLOCK(vp);
	if (sa) {
		if (sa->sa_flag & SA_MOUNTED) {
			if (mode & SM_MOUNTED)
				ret = EBUSY;
			else if (mode & SM_CLEARMOUNT)
				sa->sa_flag &= ~SA_MOUNTED;
			else
				panic("setmount: already mounted");
		} else {
			/* usecount of one is not busy */
			if (mode & SM_OPEN && sa->sa_usecount != 1)
				ret = EBUSY;
			else if (mode & SM_SETMOUNT)
				sa->sa_flag |= SA_MOUNTED;
			else if (mode & SM_CLEARMOUNT)
				panic("setmount: not mounted");
		}
	} else if ((mode & (SM_OPEN|SM_MOUNTED)) == 0)
		panic("setmount: no alias");
	SPECHASH_UNLOCK(spechashp);
	return (ret);
}

spec_init()
{
	register int i;

	for (i = 0; i < spechsz; i++) {
		SPECHASH_LOCK_INIT((&speclisth[i]));
		speclisth[i].sh_alias = 0;
	}
	ASSERT((spechsz & spechsz-1) == 0);
#if	MACH
	specalias_zone = zinit(sizeof(struct specalias),
				SPECALIAS_MAX*(sizeof(struct specalias)),
				PAGE_SIZE, "specalias");
	if (specalias_zone == (zone_t) NULL)
		panic("spec_init: no zones1");
	specinfo_zone = zinit(sizeof(struct specinfo),
				SPECINFO_MAX*(sizeof(struct specinfo)),
				PAGE_SIZE, "specinfo");
	if (specinfo_zone == (zone_t) NULL)
		panic("spec_init: no zones2");
	swapbuf_zone = zinit(sizeof(struct buf),
				SWAPBUF_MAX*(sizeof(struct buf)),
				PAGE_SIZE, "swapbuf");
	if (swapbuf_zone == (zone_t) NULL)
		panic("spec_init: no zones3");
#endif
}

