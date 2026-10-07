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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This file was modified and extended by the Center for High Performance
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: vnode.h,v $
 * Revision 1.15  1995/02/01  21:39:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.14  1994/11/18  20:42:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/06/28  22:54:20  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.12  1993/11/30  23:15:13  cfj
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
 * Revision 1.11  1993/07/15  13:43:12  shala
 * Change "#if PFS" to "#ifdef PFS".
 *
 * Revision 1.10  1993/07/15  02:56:17  brad
 * Added VIO_ERROR flag.
 *
 * Revision 1.9  1993/07/14  18:31:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.8  1993/07/09  20:23:07  brad
 * Added #define PFS for compiling applications.
 *
 * Revision 1.1.1.4  1993/07/01  20:40:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/27  02:13:18  brad
 * Added Fast Path support for PFS files.
 *
 * Revision 1.6  1993/05/06  19:21:15  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:08:07  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1992/12/11  03:00:16  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.2.2.4  1993/02/12  22:38:39  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.2.2.3  1993/02/09  21:55:42  brad
 * Added VIO_NONE.
 *
 * Revision 1.1.2.2.2.2  1992/12/16  06:01:24  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.2.2.1  1992/11/25  23:13:31  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.3  1992/11/30  22:45:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:30:02  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:43:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.16  1992/10/22  15:35:10  dbm
 * Added changes for PFS.
 *
 * Revision 2.16  92/10/29  13:18:59  chrisp
 * Move declaration of struct vnode_proxy outside #ifdef _KERNEL.
 *
 * Revision 2.22  93/10/20  15:26:57  dnoveck
 *      DEV_BSIZE elimination: Added v_bufhash_shift.
 * 
 * Revision 2.18  93/06/16  13:51:28  klh
 * 	Revision 2.21  93/05/13  16:45:36  roy
 * 		Added VIO_PAGING iomode.  Removed paging arg from vop_pageout.
 * 		[93/05/05            roy]
 * 
 * Revision 2.17  93/04/24  18:45:07  klh
 * 	Revision 2.20  93/04/06  11:58:14  rabii
 * 		Add/change some flags
 *
 * 	Revision 2.19  93/03/30  16:09:53  roy
 * 		- For osf1_adfs, added vop_datain/vop_dataout (later to be combined
 * 		  with vop_pagein/vop_pageout).
 * 		- For vfs_vio, added write-in-progress queue head to vnode.
 * 		- Added a couple VIO macros.
 * 		[93/02/16            roy]
 *
 * 	Revision 2.18  92/12/08  10:45:37  durriya
 * 		 1.1 unmount sync changes - Replaced VMOUNTING and VMOUNTWAIT with
 * 		 VMOUNTPOINT, which is set on the covered file of a mount point (for
 * 		 the duration of the mount).  Renumber vnode flags.         (durriya)
 *
 * Revision 2.16  92/10/29  13:18:59  chrisp
 * Move declaration of struct vnode_proxy outside #ifdef _KERNEL.
 * 
 * Revision 2.15  92/10/05  14:00:09  klh
 * 	Revision 2.16  92/10/05  12:08:13  rabii
 * 		Add paging arg to vop_pageout.
 * 		[92/10/02            roy]
 * 
 * 	Revision 2.15  92/09/20  11:25:03  roy
 * 		Added VOP_GETSIZE for OSF1_ADFS.
 * 		[92/09/15            roy]
 * 
 * 	Revision 2.14  92/09/11  09:27:41  rabii
 * 		Cleaned up VIO macros.
 * 		[92/09/10            roy]
 * 
 * 	Revision 2.13  92/08/26  12:12:05  loverso
 * 		Added VIO_SETMODE, VIO_IS_BUF, VIO_IS_MAPPED macros.
 * 		Added VOP_UPDATE for OSF1_ADFS.
 * 		[92/08/14            roy]
 * 
 * Revision 2.14  92/08/17  18:09:50  mjl
 * Get rid of extraneous debugging macros.
 * 
 * Revision 2.13  92/08/17  12:58:59  mjl
 * Add state flags for FIFO vnode relocation.  Add new iomode for file ports
 * wishing to be notified after a process migrates.  Added some debugging
 * macros used by FIFO relocation code.
 * 
 * Revision 2.12  92/07/29  08:22:48  rabii
 * 	pagein, pageout, and alloc VOP's now under OSF1_ADFS conditional.
 * 	[92/07/20            roy]
 * 
 * Revision 2.11  92/06/08  18:22:18  pjg
 * 	Add v_iomode (OSF1_ADFS only), define its values and the macros
 * 	to handle it (pjg).
 * 
 * Revision 2.10  92/05/31  18:59:04  loverso
 * 	Removed VOP_SETSIZE and added cred arg to VOP_ALLOC for OSF1_ADFS.
 * 	[92/05/29            roy]
 * 
 * Revision 2.9  92/04/05  17:04:02  pjg
 * 	Modified vnode proxies. They now contain the vnode port, the port
 * 	or both.
 * 
 * Revision 2.8  92/03/15  14:38:18  roy
 * 	92/02/28  13:59:54  roy
 * 	Added new ops for OSF1_ADFS.
 * 
 * Revision 2.7  92/03/01  18:39:43  pjg
 * 	1992/03/01  pjg
 * 	Define vnode proxies.
 * 
 * Revision 2.6  92/01/05  20:10:58  roy
 * 	1991/11/12  17:53:18  noemi
 * 	Made magic number a long and added a va_node field to the vattr 
 * 	structure.  Undid VOP_MKNOD extension done in revision 2.4.
 * 
 * 	1991/10/14  19:36:22  noemi
 * 	Added v_magic, v_seqno, and v_mscount fields to the vnode. 
 * 	Also defined V_MAGIC, NNMSWAIT, VPORTLOCK and VPORTWAIT.
 * 
 * 	1991/09/21  20:21:02  noemi
 * 	Added VUNMOUNTING
 * 
 * Revision 2.5  91/12/13  10:14:13  roy
 * 	91/10/29  17:59:41  roy
 * 	Add cookie as first field of vnode structure.
 * 
 * Revision 2.4  91/11/26  13:34:16  rabii
 * 	Removed VOP_RMKNOD and extended VOP_MKNOD to take node
 * 
 * Revision 2.3  91/11/25  11:25:53  rabii
 * 	Added VOP_RMKNOD operation for remote devices
 * 
 * Revision 2.2  91/08/31  14:17:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:47:44  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16  90/10/31  14:07:25  devrcs
 * 	Cleaned up comments.
 * 	[90/10/13  21:41:27  gmf]
 * 
 * 	define VXENIX for a vnode that has xenix file locks on the file.
 * 	[90/10/08  13:44:59  swallace]
 * 
 * Revision 1.15  90/10/07  14:57:14  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:50:07  gm]
 * 
 * 	changed SEC_MAC to SEC_FSCHANGE to allow C2 kernels to build
 * 	[90/09/21  04:19:51  hosking]
 * 
 * Revision 1.14  90/09/23  16:00:29  devrcs
 * 	VOP_PGWR loses the init argument, gains pager and offset args.
 * 	[90/09/11  06:54:17  ers]
 * 
 * 	The following bits in v_flag are for file locking:
 * 	        VENF_LOCK    mandatory file locking enabled for vnode
 * 	        VLOCKS       fcntl locks exist on file
 * 	[90/09/06  14:39:14  swallace]
 * 
 * 	For vflush and quota use:  mark certain vnodes
 * 	as being in use by kernel.
 * 	[90/09/03  22:36:19  nags]
 * 
 * Revision 1.13  90/08/24  12:28:08  devrcs
 * 	All fields used for number of outstanding buffer writes
 * 	must be separated out so they can be appropriately protected
 * 	by lock and spl.
 * 	[90/08/19  01:21:27  nags]
 * 
 * Revision 1.12  90/07/27  09:08:40  devrcs
 * 	Changed arguments to VOP_PGRD and VOP_PGWR.
 * 	[90/07/23  14:40:33  ers]
 * 
 * 	VOP_OPEN changes for clone driver support.
 * 	[90/07/20  17:06:58  nags]
 * 
 * 	Funnelling must assume vnode may disappear.
 * 	[90/07/17  08:49:13  nags]
 * 
 * Revision 1.11  90/07/17  11:42:47  devrcs
 * 	Remove #include of inode.h, no longer necessary.
 * 	[90/07/10  22:03:42  seiden]
 * 
 * Revision 1.10  90/06/29  13:54:11  devrcs
 * 	Remove flag in vnode to mark locked files; it's done in struct file
 * 	now.
 * 	[90/06/26  11:28:12  nags]
 * 
 * Revision 1.9  90/06/22  20:54:49  devrcs
 * 	Put include of unix_defs.h under ifdef _KERNEL
 * 	[90/06/15  12:21:57  nags]
 * 
 * 	nags merge
 * 
 * 	Condensed history:
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Secureware changes.				seiden@osf.org
 * 	Added support for system V filesystem.		morris@osf.org
 * 	Changed VOP_SELECT for new select interface.	coren@osf.org
 * 	Move v_hashchain, v_specnext to vfs_subr.c.	mkm@berkeley.edu
 * 	Vnode read/write counts (no file table walks).	noemi@osf.org
 * 	Eof fix for readdir (add arg to VOP_READDIR).	gmf@osf.org
 * 	Little/big endian versions of va_size.		noemi@osf.org
 * 	Added new vtype: VFIFO.				ers@osf.org
 * 	Restored v_data field to end of vnode.		gmf@osf.org
 * 	Integrated 4.4BSD filesystem changes [1/5/90]	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Locking protocols derived from Encore Mach/0.6.	mach@encore.com
 * 	Source from 4.4 Berkeley Standard Distribution.	mkm@berkeley.edu
 * 	[90/06/12  21:40:10  nags]
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
 *	@(#)vnode.h	7.18 (Berkeley) 1/4/90
 */

#ifndef _SYS_VNODE_H_
#define	_SYS_VNODE_H_

#ifndef	_KERNEL
#ifndef	PFS
#define	PFS
#endif
#endif

#include <sys/secdefines.h>
#include <machine/endian.h>
#include <kern/queue.h>

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

#ifndef	_EMULATOR
#include <vfs_vio.h>
#endif

#if	SEC_FSCHANGE
#include <sys/security.h>
#endif


/*
 * The vnode is the focus of all file activity in UNIX.
 * There is a unique vnode allocated for each active file,
 * each current directory, each mounted-on file, text file, and the root.
 */

/*
 * vnode types. VNON means no type.
 */
enum vtype 	{ VNON, VREG, VDIR, VBLK, VCHR, VLNK, VSOCK, VFIFO, VBAD };

/*
 * Vnode tag types.
 * These are for the benefit of external programs only (e.g., pstat)
 * and should NEVER be inspected inside the kernel.
 */
enum vtagtype	{ VT_NON, VT_UFS, VT_NFS, VT_MFS, VT_S5FS };

/*
 * This defines the maximum size of the private data area
 * permitted for any file system type.  Vn_maxprivate is
 * set early in the boot sequence based on the largest
 * filesystem-specific representation.  The actual size
 * of a vnode will depend on vn_maxprivate at run-time.
 */
extern int	vn_maxprivate;

#ifdef	OSF1_ADFS
/*
 * Magic number for vnodes.
 */
#define V_MAGIC		0x091358
#endif

struct vnode {
#ifdef	OSF1_ADFS
	long		v_magic;		/* indicates port is attached*/
	mach_port_t	v_port;			/* the attached port */
	u_long		v_seqno;		/* sequence number */
	mach_port_mscount_t v_mscount;		/* # of send rights created */
	u_long		v_iomode;		/* I/O mode (see below) */
#endif
	u_long		v_flag;			/* vnode flags (see below) */
	long		v_usecount;		/* reference count of users */
	long		v_holdcnt;		/* page & buffer references */
	u_short		v_shlockc;		/* count of shared locks */
	u_short		v_exlockc;		/* count of exclusive locks */
	off_t		v_lastr;		/* last read (read-ahead) */
	u_long		v_id;			/* capability identifier */
	struct mount	*v_mount;		/* ptr to vfs we are in */
	struct vnodeops	*v_op;			/* vnode operations */
	struct vnode	*v_freef;		/* vnode freelist forward */
	struct vnode	**v_freeb;		/* vnode freelist back */
	struct vnode	*v_mountf;		/* vnode mountlist forward */
	struct vnode	**v_mountb;		/* vnode mountlist back */
	struct buf	*v_cleanblkhd;		/* clean blocklist head */
	struct buf	*v_dirtyblkhd;		/* dirty blocklist head */
	long		v_numoutput;		/* num of writes in progress */
	u_long		v_outflag;		/* output flags */
	char		v_bufhash_shift;	/* shift for block number in */
						/* buffer hash computation */
	enum vtype	v_type;			/* vnode type */
	union {
		struct mount	*vu_mountedhere;/* ptr to mounted vfs (VDIR) */
		struct socket	*vu_socket;	/* unix ipc (VSOCK) */
		struct specinfo	*vu_specinfo;	/* device specinfo structure */
	} v_un;
	enum vtagtype	v_tag;			/* type of underlying data */
	u_short         v_rdcnt;                /* count of readers */
	u_short         v_wrcnt;                /* count of writers */
	struct vm_info	*v_vm_info;		/* private data for for vm */
#if SEC_FSCHANGE
	struct vnsecops	*v_secop;		/* vnode security operations */
#endif
#ifdef	_KERNEL
	udecl_simple_lock_data(,v_buflists_lock)/* protect clean/dirty blkhd */
	udecl_simple_lock_data(,v_output_lock)	/* protect numoutput, outflag */
	udecl_simple_lock_data(,v_lock)		/* multiprocessor exclusion */
#endif
#if	VFS_VIO
        queue_head_t	v_wip_list;		/* write-in-progress list */
	                                        /* protected by buflists_lock */
#endif
#ifdef	TNC
	caddr_t		v_tncdata;
#endif
	char v_data[1];				/* placeholder, private data */
};
#define v_mountedhere v_un.vu_mountedhere
#define v_socket v_un.vu_socket
#define v_specinfo v_un.vu_specinfo

/*
 * vnode flags.
 */
#define	VROOT		0x0001	/* root of its file system */
#define	VTEXT		0x0002	/* vnode is a pure text prototype */
#define	VXLOCK		0x0004	/* vnode is locked to change underlying type */
#define	VXWANT		0x0008	/* process is waiting for vnode */
#define	VEXLOCK		0x0010	/* exclusive lock */
#define	VSHLOCK		0x0020	/* shared lock */
#define	VLWAIT		0x0040	/* proc is waiting on shared or excl. lock */
#ifndef	OSF1_ADFS
#define	VMOUNTPOINT	0x0080	/* file system mounted here(or will be soon) */
#endif
#define VFLOCK		0x0100	/* vnode is locked for first fifo open */
#define VFWAIT		0x0200	/* waiting for fifo open */
#define	VSYSTEM		0x0400	/* vnode being used by kernel */
#define VENF_LOCK       0x0800  /* mandatory file locking enabled */
#define VLOCKS          0x1000  /* fcntl file locks on file */

#ifdef	OSF1_ADFS
#define	VMOUNTPOINT	0x00010000 /* directory is being mounted on */
#define VUNMOUNTING	0x00020000 /* filesystem is being unmounted */
#define	VNMSWAIT	0x00040000 /* no-more-senders message is waiting */
#define VPORTLOCK	0x00080000 /* locked to create/check make send count */
#define	VPORTWAIT	0x00100000 /* waiting for port lock */
#ifdef	TNC
#define VREDIRECT	0x00200000 /* redirect namei() lookups */
#define VRELOCATING	0x00400000 /* relocation attempt in progress */
#define VXREFWAIT	0x00800000 /* waiting for extra vrefs to be released */
#endif	/* TNC */


/*
 * I/O modes for v_iomode.
 * The I/O mode describes how the data associated with a vnode 
 * should be accessed, whether it should be cached, etc.
 */
#define VIO_BUF		0x00	/* Use buffer cache. */
#define VIO_MAPPED	0x01	/* Use Mach memory objects for caching. */
#define VIO_FASTPATH	0x02	/* Move data using read2, write2 interfaces;
				   no caching */
#define	VIO_PAGING	0x04	/* Paging file. */
#ifdef	TNC
#define VIO_REQNOTIFY	0x08	/* Request emulator migrate notification */
#endif	/* TNC */

#ifdef	PFS
#define	VIO_PFS		0x10		/* PFS file header */
#define	VIO_STRIPED	0x11		/* PFS stripe file */
#define	VIO_ERROR	0xfffffffe	/* I/O mode error flag */
#define	VIO_NONE	0xffffffff	/* Null mode, so use default */
#endif

#ifndef LOCAL_DEVNULL
#define LOCAL_DEVNULL
#endif /* LOCAL_DEVNULL */

#ifdef LOCAL_DEVNULL
#define VIO_DEVNULL     0x20            /* null device, handle locally */
#endif /* LOCAL_DEVNULL */

#define VIO_GETMODE(vp)		((vp)->v_iomode)
#define	MF_MAPPABLE(vp)		(VIO_GETMODE(vp) == VIO_MAPPED) /* XXX remove */
#define	VIO_IS_BUF(vp)		(VIO_GETMODE(vp) == VIO_BUF)
#define	VIO_IS_MAPPED(vp)	(VIO_GETMODE(vp) == VIO_MAPPED)
#ifndef	PFS
#define VIO_IS_FASTPATH(vp)	(VIO_GETMODE(vp) == VIO_FASTPATH)
#else
#define VIO_IS_FASTPATH(vp)	((VIO_GETMODE(vp) == VIO_FASTPATH) || \
				 (VIO_GETMODE(vp) == VIO_STRIPED)  || \
				 (VIO_GETMODE(vp) == VIO_PFS))
#define VIO_IS_PFS(vp)		(VIO_GETMODE(vp) == VIO_PFS)
#define VIO_IS_STRIPED(vp)	(VIO_GETMODE(vp) == VIO_STRIPED)
#endif
#define VIO_IS_PAGING(vp)	(VIO_GETMODE(vp) == VIO_PAGING)

/* Does the file engage in disk block reservation? */
#define VIO_BLK_RESERVE(vp)	((VIO_GETMODE(vp) == VIO_MAPPED) ||	\
				 (VIO_GETMODE(vp) == VIO_FASTPATH))
/*
 * When growing a file via truncate, is it possible to use VOP_SETATTR
 * directly rather than using VOP_WRITE?
 */
#define VIO_GROW_DIRECT(vp)	((VIO_GETMODE(vp) == VIO_MAPPED) ||	\
				 (VIO_GETMODE(vp) == VIO_FASTPATH))

#endif	/* OSF1_ADFS */

#if     SEC_FSCHANGE
#define VMLD	    0x10000000  /* vnode is a multilevel directory */
#endif 
#ifdef i386
#define VXENIX      0x20000000  /* xenix file locks on file */
#endif

/*
 * vnode output flags
 */
#define	VOUTWAIT	0x0001	/* waiting for output to complete */

/*
 * Operations on vnodes.
 */
struct vnodeops {
	int	(*vn_lookup)(		/* vp, ndp */ );
	int	(*vn_create)(		/* ndp, vap */ );
	int	(*vn_mknod)(		/* ndp, vap, cred */ );
	int	(*vn_open)(		/* vp, fflags, cred */ );
	int	(*vn_close)(		/* vp, fflags, cred */ );
	int	(*vn_access)(		/* vp, fflags, cred */ );
	int	(*vn_getattr)(		/* vp, vap, cred */ );
	int	(*vn_setattr)(		/* vp, vap, cred */ );

	int	(*vn_read)(		/* vp, uiop, ioflag, cred */ );
	int	(*vn_write)(		/* vp, uiop, ioflag, cred */ );
	int	(*vn_ioctl)(		/* vp, com, data, fflag, cred */ );
	int	(*vn_select)(	    /* vp, events, revents, scanning, cred */ );
	int	(*vn_mmap)(		/* vp, ..., cred */ );
	int	(*vn_fsync)(		/* vp, fflags, cred, waitfor */ );
	int	(*vn_seek)(		/* vp, (old)offp, off, whence */ );

	int	(*vn_remove)(		/* ndp */ );
	int	(*vn_link)(		/* vp, ndp */ );
	int	(*vn_rename)(		/* fndp, tndp */ );
	int	(*vn_mkdir)(		/* ndp, vap */ );
	int	(*vn_rmdir)(		/* ndp */ );
	int	(*vn_symlink)(		/* ndp, vap, name */ );
	int	(*vn_readdir)(		/* vp, uiop, cred, eofflagp */ );
	int	(*vn_readlink)(		/* vp, uiop, cred */ );

	int	(*vn_abortop)(		/* ndp */ );
	int	(*vn_inactive)(		/* vp */ );
	int	(*vn_reclaim)(		/* vp */ );

	int	(*vn_bmap)(		/* vp, bn, vpp, bnp */ );
	int	(*vn_strategy)(		/* bp */ );

	int	(*vn_print)(		/* vp */ );
	int	(*vn_pgrd)(		/* vp, uiop, cred */ );
	int	(*vn_pgwr)(		/* vp, uiop, cred, pager, offset */ );
#ifdef	PFS
        int	(*vn_prealloc)(		/* vp, oldsize, newsize, cred */ );
#endif
#ifdef	OSF1_ADFS
	int	(*vn_pagein)(		/* vp, uiop, cred */ );
	int	(*vn_pageout)(		/* vp, uiop, cred */ );
	int	(*vn_alloc)(		/* vp, offset, length, count, cred */ );
	int	(*vn_update)(		/* vp, acc, mod, size, flags, cred */ );
	int	(*vn_getsize)(		/* vp, sizep, flags, cred */ );
	int	(*vn_datain)(		/* vp, uiop, ioflag, cred */ );
	int	(*vn_dataout)(		/* vp, uiop, ioflag, cred */ );
#endif
};

/*
 * Invoking a vnode operation implies funnelling when SER_COMPAT
 * is turned on.
 */

#define	_VOP_(f,v,arg,r)						\
MACRO_BEGIN								\
	struct mount *_vop_mp = (v)->v_mount;				\
									\
	MOUNT_FUNNEL(_vop_mp);						\
	(r) = (*((v)->v_op->f))arg;					\
	MOUNT_UNFUNNEL(_vop_mp);					\
MACRO_END

/* Macros to call the vnode ops */
#define	VOP_LOOKUP(v,n,r)	_VOP_(vn_lookup,(v),((v),(n)),(r))
#define	VOP_CREATE(n,a,r)	_VOP_(vn_create,(n)->ni_dvp,((n),(a)),(r))
#define	VOP_MKNOD(n,a,c,r)	_VOP_(vn_mknod,(n)->ni_dvp,((n),(a),(c)),(r))
#define	VOP_OPEN(vpp,f,c,r)	_VOP_(vn_open,(*vpp),((vpp),(f),(c)),(r))
#define	VOP_CLOSE(v,f,c,r)	_VOP_(vn_close,(v),((v),(f),(c)),(r))
#define	VOP_ACCESS(v,f,c,r)	_VOP_(vn_access,(v),((v),(f),(c)),(r))
#define	VOP_GETATTR(v,a,c,r)	_VOP_(vn_getattr,(v),((v),(a),(c)),(r))
#define	VOP_SETATTR(v,a,c,r)	_VOP_(vn_setattr,(v),((v),(a),(c)),(r))
#define	VOP_READ(v,u,i,c,r)	_VOP_(vn_read,(v),((v),(u),(i),(c)),(r))
#define	VOP_WRITE(v,u,i,c,r)	_VOP_(vn_write,(v),((v),(u),(i),(c)),(r))
#define	VOP_IOCTL(v,o,d,f,c,r)	_VOP_(vn_ioctl,(v),((v),(o),(d),(f),(c)),(r))
#define	VOP_SELECT(v,e,r,s,c,x)	_VOP_(vn_select,(v),((v),(e),(r),(s),(c)),(x))
#define	VOP_MMAP(v,c,r)		_VOP_(vn_mmap,(v),((v),(c)),(r))
#define	VOP_FSYNC(v,f,c,w,r)	_VOP_(vn_fsync,(v),((v),(f),(c),(w)),(r))
#define	VOP_SEEK(v,p,o,w,r)	_VOP_(vn_seek,(v),((v),(p),(o),(w)),(r))
#define	VOP_REMOVE(n,r)		_VOP_(vn_remove,(n)->ni_dvp,((n)),(r))
#define	VOP_LINK(v,n,r)		_VOP_(vn_link,(n)->ni_dvp,((v),(n)),(r))
#define	VOP_RENAME(s,t,r)	_VOP_(vn_rename,(s)->ni_dvp,((s),(t)),(r))
#define	VOP_MKDIR(n,a,r)	_VOP_(vn_mkdir,(n)->ni_dvp,((n),(a)),(r))
#define	VOP_RMDIR(n,r)		_VOP_(vn_rmdir,(n)->ni_dvp,((n)),(r))
#define	VOP_SYMLINK(n,a,m,r)	_VOP_(vn_symlink,(n)->ni_dvp,((n),(a),(m)),(r))
#define	VOP_READDIR(v,u,c,e,r)	_VOP_(vn_readdir,(v),((v),(u),(c),(e)),(r))
#define	VOP_READLINK(v,u,c,r)	_VOP_(vn_readlink,(v),((v),(u),(c)),(r))
#define	VOP_ABORTOP(n,r)	_VOP_(vn_abortop,(n)->ni_dvp,((n)),(r))
#define	VOP_INACTIVE(v,r)	_VOP_(vn_inactive,(v),((v)),(r))
#define	VOP_RECLAIM(v,r)	_VOP_(vn_reclaim,(v),((v)),(r))
#define	VOP_BMAP(v,s,p,n,r)	_VOP_(vn_bmap,(v),((v),(s),(p),(n)),(r))
#define	VOP_STRATEGY(b,r)	_VOP_(vn_strategy,(b)->b_vp,((b)),(r))
#define	VOP_PRINT(v,r)		_VOP_(vn_print,(v),((v)),(r))
#define	VOP_PGRD(v,u,c,r)	_VOP_(vn_pgrd,(v),((v),(u),(c)),(r))
#define	VOP_PGWR(v,u,c,p,o,r)	_VOP_(vn_pgwr,(v),((v),(u),(c),(p),(o)),(r))

#ifdef	PFS
#define VOP_PREALLOC(v,o,n,c,r) _VOP_(vn_prealloc,(v),((v),(o),(n),(c)),(r))
#endif

#ifdef	OSF1_ADFS
#define	VOP_PAGEIN(v,u,c,r)	_VOP_(vn_pagein,(v),((v),(u),(c)),(r))
#define	VOP_PAGEOUT(v,u,c,r)	_VOP_(vn_pageout,(v),((v),(u),(c)),(r))
#define	VOP_ALLOC(v,o,l,co,c,r)	_VOP_(vn_alloc,(v),((v),(o),(l),(co),(c)),(r))
#define	VOP_UPDATE(v,a,m,s,f,c,r) _VOP_(vn_update,(v),((v),(a),(m),(s),(f),(c)),(r))
#define	VOP_GETSIZE(v,s,f,c,r) _VOP_(vn_getsize,(v),((v),(s),(f),(c)),(r))
#define	VOP_DATAIN(v,u,i,c,r)	_VOP_(vn_datain,(v),((v),(u),(i),(c)),(r))
#define	VOP_DATAOUT(v,u,i,c,r)	_VOP_(vn_dataout,(v),((v),(u),(i),(c)),(r))
#endif

/*
 * flags for ioflag
 */
#define IO_UNIT		0x01		/* do I/O as atomic unit */
#define IO_APPEND	0x02		/* append write to end */
#define IO_SYNC		0x04		/* do I/O synchronously */
#define IO_NODELOCKED	0x08		/* underlying node already locked */
#define	IO_NDELAY	0x10		/* FNDELAY flag set in file table */

#ifdef 	OSF1_ADFS
#define	IO_TRUNC	0x20		/* truncate: used by VOP_UPDATE */
#define IO_CONSUME	0x40		/* buffer consumed when writing */
#endif

/*
 * Vnode attributes.  A field value of VNOVAL
 * represents a field whose value is unavailable
 * (getattr) or which is not to be changed (setattr).
 */
struct vattr {
	enum vtype	va_type;	/* vnode type (for create) */
	u_short		va_mode;	/* files access mode and type */
	short		va_nlink;	/* number of references to file */
	uid_t		va_uid;		/* owner user id */
	gid_t		va_gid;		/* owner group id */
	long		va_fsid;	/* file system id (dev for now) */
	long		va_fileid;	/* file id */
	quad		va_qsize;	/* file size in bytes */
	long		va_blocksize;	/* blocksize preferred for i/o */
	struct timeval	va_atime;	/* time of last access */
	struct timeval	va_mtime;	/* time of last modification */
	struct timeval	va_ctime;	/* time file changed */
	u_long		va_gen;		/* generation number of file */
	u_long		va_flags;	/* flags defined for file */
	dev_t		va_rdev;	/* device special file represents */
#ifdef	OSF1_ADFS
	node_t		va_node;	/* node special files resides on */
#endif
	quad		va_qbytes;	/* bytes of disk space held by file */
	union {
		char	*vau_symlink;	/* name of symlink to be created */
		char	*vau_socket;	/* address of socket (XX make void *) */
	} va_un;
};

#define va_symlink      va_un.vau_symlink
#define va_socket       va_un.vau_socket

#if	BYTE_ORDER == LITTLE_ENDIAN
#define	va_size		va_qsize.val[0]
#define	va_size_rsv	va_qsize.val[1]
#define	va_bytes	va_qbytes.val[0]
#define	va_bytes_rsv	va_qbytes.val[1]
#else
#define	va_size		va_qsize.val[1]
#define	va_size_rsv	va_qsize.val[0]
#define	va_bytes	va_qbytes.val[1]
#define	va_bytes_rsv	va_qbytes.val[0]
#endif

#if	SEC_FSCHANGE

#define	VHASSECOPS(vp)	((vp)->v_secop)
#define	VSECURE(vp)	((vp)->v_mount->m_flag & M_SECURE)

struct vnsecops {
	int	(*vn_getsecattr)	( /* vp, vsap, cred */ );
	int	(*vn_setsecattr)	( /* vp, vsap, cred */ );
	int	(*vn_dirempty)		( /* vp, dvp, cred */ );
};

#define	VOP_GETSECATTR(v,a,c)	(*((v)->v_secop->vn_getsecattr))((v),(a),(c))
#define	VOP_SETSECATTR(v,a,c)	(*((v)->v_secop->vn_setsecattr))((v),(a),(c))
#define	VOP_DIREMPTY(v,p,c)	(*((v)->v_secop->vn_dirempty))((v),(p),(c))

struct vsecattr {
	u_short		vsa_valid;	/* which fields are valid (see below) */
	u_char		vsa_policy;	/* policy index for vsa_tag */
	u_char		vsa_tagnum;	/* policy-relative tag index */
	struct vnode	*vsa_parent;	/* parent vnode for tag changes */
	tag_t		vsa_tags[SEC_TAG_COUNT];	/* tag pool */
	privvec_t	vsa_gpriv;	/* granted privileges */
	privvec_t	vsa_ppriv;	/* potential privileges */
	u_long		vsa_type_flags;	/* type flags (MLD, 2 person, etc.) */
};
#define	vsa_tag	vsa_tags[0]

/* Flag values for vsa_valid field */
#define	VSA_TAG		0x01
#define	VSA_GPRIV	0x02
#define	VSA_PPRIV	0x04
#define	VSA_TYPE_FLAGS	0x08
#define	VSA_ALLTAGS	0x10

#endif	/* SEC_FSCHANGE */

/*
 *  Modes. Some values same as Ixxx entries from inode.h for now
 */
#define	VSUID	04000		/* set user id on execution */
#define	VSGID	02000		/* set group id on execution */
#define	VSVTX	01000		/* save swapped text even after use */
#define	VREAD	0400		/* read, write, execute permissions */
#define	VWRITE	0200
#define	VEXEC	0100

/*
 * Token indicating no attribute value yet assigned
 */
#define VNOVAL	((unsigned)0xffffffff)

#ifdef	_KERNEL
/*
 * Vnode locking constraints.
 *	Field			Comment
 *	-----			-------
 *	v_flag			v_lock
 *	v_usecount		v_lock
 *	v_holdcnt		v_lock
 *	v_shlockc		v_lock
 *	v_exlockc		v_lock
 *	v_lastr			v_lock
 *	v_id			v_lock
 *	v_mount			read-only?	XXX
 *	v_op			v_lock
 *	v_freef			v_free_lock
 *	v_freeb			v_free_lock
 *	v_mountf		m_vlist_lock
 *	v_mountb		m_vlist_lock
 *	v_cleanblkhd		v_buflists_lock
 *	v_dirtyblkhd		v_buflists_lock
 *	v_numoutput		v_output_lock
 *	v_outflag		v_output_lock
 *	v_type			v_lock
 *	v_mountedhere		v_lock + VMOUNTPOINT (r/o except for funmnt)
 *	vu_socket		v_lock
 *	vu_nextalias		speclist hashchain lock
 *	v_rdev			v_lock
 *	v_tag			v_lock
 *	v_rdcnt			v_lock
 *	v_wrcnt			v_lock
 *	v_vm_info		read-only
 *
 * Special privileges are accorded unparallelized filesystem types,
 * as follows.  Manipulation of globally used fields (e.g. lists) is
 * not allowed by the filesystem, and is done under lock in the proper
 * places.  The private fields are not subject to locking, since when
 * the filesystem is running, it is guaranteed to be funnelled onto the
 * master processor.  It is, however, still subject to normal races, for
 * example, any operation that can sleep may need to provide its own
 * synchronization.
 *
 * The following additional synchronization requirements apply.
 * [expand on VXLOCK, VMOUNTING, etc.].
 *
 * Note about v_vm_info.  Currently, v_vm_info is set at boot-time to
 * point to the vnode's associated v_vm_info structure.  This field is
 * never re-set, not even by vgone, so no lock is needed to use the field.
 *
 * However, it must also be noted that OSF/1 does not currently support
 * the MACH_NBC option.  There are races with the filesystem's use of the
 * vm_info's pager field that must first be resolved before MACH_NBC will
 * work.  The current code does use the pager field but all of these uses
 * are not guaranteed at this time.  There could be problems with the
 * use of the v_vm_info->pager field during mapping and unmapping of
 * files. [XXX]
 *
 * The vm_info structure also has credentials, currently used only by 
 * the paging file code.  These credentials do not change during the
 * life of the paging file.
 */

/*
 * The vnode lock protects the contents of the vnode structure;
 * its only purpose is multiprocessor exclusion.  Long-term locking
 * is a privilege reserved for the filesystem-specific layers.
 */
#define	VN_LOCK(vp)		usimple_lock(&(vp)->v_lock)
#define	VN_UNLOCK(vp)		usimple_unlock(&(vp)->v_lock)
#define	VN_LOCK_TRY(vp)		usimple_lock_try(&(vp)->v_lock)
#define	VN_LOCK_INIT(vp)	usimple_lock_init(&(vp)->v_lock)
#define	VN_LOCK_HOLDER(vp)	SLOCK_HOLDER(&(vp)->v_lock)

/*
 * The vnode buffer lists lock protects both the clean and dirty
 * buffer lists in the vnode.
 */
#define	VN_BUFLISTS_LOCK(vp)	usimple_lock(&(vp)->v_buflists_lock)
#define	VN_BUFLISTS_UNLOCK(vp)	usimple_unlock(&(vp)->v_buflists_lock)
#define	VN_BUFLISTS_LOCK_INIT(vp) usimple_lock_init(&(vp)->v_buflists_lock)

/*
 * The vnode output lock protects the number of writes in progress and
 * the output flags.
 */
#define	VN_OUTPUT_LOCK(vp)	usimple_lock(&(vp)->v_output_lock)
#define	VN_OUTPUT_UNLOCK(vp)	usimple_unlock(&(vp)->v_output_lock)
#define	VN_OUTPUT_LOCK_INIT(vp) usimple_lock_init(&(vp)->v_output_lock)

#if	VFS_VIO
#define	VN_WIP_QUEUE_INIT(vp) 	queue_init(&(vp)->v_wip_list)
#endif

/*
 * public vnode manipulation functions
 */
extern int vn_open();			/* open vnode */
extern int vn_rdwr();			/* read or write vnode */
extern int vn_close();			/* close vnode */
extern int getnewvnode();		/* allocate a new vnode */
extern struct vnode *shadowvnode();	/* get shadow vnode for block device */
extern int bdevvp();			/* allocate a new special dev vnode */
extern int makealias();			/* make special device aliases */
extern int specalloc();			/* allocate specinfo structure */
extern int vgetm();			/* get reference to a vnode */
extern void vref();			/* increase reference to a vnode */
extern void vrele();			/* release vnode */
extern int vgone();			/* completely recycle vnode */
extern void clearalias();		/* recycle vnode and all its aliases */

/*
 * Synchronization notes:
 * Vgone and vgetm will return a 1 if the vnode had VXLOCK set, 0 if it
 * did not.  In the case of a vnode with VXLOCK set, neither routine will
 * successfully complete, and will wait for the lock to clear, depending
 * upon the arguments passed.
 *
 * Flags passed to vgone
 */
#define VX_NOSLEEP      0x0000          /* don't sleep on VXLOCK */
#define VX_SLEEP        0x0001          /* sleep on VXLOCK */
#define VX_INACTIVE     0x0002          /* vnode is not active (e.g. ref'd) */
#define VX_DOCLOSE      0x0004          /* vclean: close active files */


/*
 * Two flavors of vget (both return 0 if you get the reference):
 *	vget(vp) -- sleep on VXLOCK; always gets the reference.
 *	vget_nowait(vp) -- do not sleep on VXLOCK; return 1 immediately if set.
 */
#define vget(vp)	vgetm(vp, 1)	/* get reference, sleep on VXLOCK */
#define vget_nowait(vp)	vgetm(vp, 0)	/* if VXLOCK set, return 1, no ref. */

/*
 * Flags to various vnode functions.
 */
#define	SKIPSYSTEM	0x0001		/* vflush: skip VSYSTEM vnodes */
#define	FORCECLOSE	0x0002		/* vflush: force file closure */
#define	DOCLOSE		0x0004		/* vclean: close active files */

/*
 * Inline references for non-debug kernels
 */
#if	MACH_ASSERT
extern void vattr_null();		/* set attributes to null */
#define	VREF(vp)	vref(vp)	/* increase reference to a vnode */
#define VHOLD(vp)	vhold(vp)	/* increase buf or page ref to vnode */
#define HOLDRELE(vp)	holdrele(vp)	/* decrease buf or page ref to vnode */
#define VATTR_NULL(vap) vattr_null(vap) /* initialize a vattr structure */

#else	/* MACH_ASSERT */

#define VREF(vp)						\
MACRO_BEGIN							\
	VN_LOCK(vp);						\
	vp->v_usecount++;					\
	VN_UNLOCK(vp);						\
MACRO_END

#define VHOLD(vp)						\
MACRO_BEGIN							\
	VN_LOCK(vp);						\
	vp->v_holdcnt++;					\
	VN_UNLOCK(vp);						\
MACRO_END

#define HOLDRELE(vp)						\
MACRO_BEGIN							\
	VN_LOCK(vp);						\
	vp->v_holdcnt--;					\
	VN_UNLOCK(vp);						\
MACRO_END

#define VATTR_NULL(vap) *(vap) = va_null /* initialize a vattr structure */
#define vattr_null(vap) VATTR_NULL(vap);

#endif	/* MACH_ASSERT */

#define	VUNREF(vp)	vrele(vp)	 /* decrease reference to a vnode */
#define NULLVP	((struct vnode *)0)


/*
 * Global vnode data.
 */
extern	struct vnode *rootdir;		/* root (i.e. "/") vnode */

extern	struct vnode *vnode;		/* The vnode table itself */
extern	struct vnode *vnodeNVNODE;	/* The end of the vnode table */
extern	int nvnode;			/* number of slots in the table */
extern	long desiredvnodes;		/* number of vnodes desired */
extern	struct vattr va_null;		/* predefined null vattr structure */

#endif	/* _KERNEL */

#ifdef	OSF1_ADFS

/*
 * A vnode proxy represents a vnode that may be local or remote.
 * If the vnode is local, vpx_port contains the pointer to the vnode.
 * If the vnode is remote, vpx_vnode contains the port to the vnode.
 * vpx_node may also contain the port to a local vnode, to avoid
 * allocating and deallocating the port every time it is used.
 * vpx_usecount contains the number of local references to the vnode.
 * If the vnode is local, each vpx_usecount has a corresponding vref on
 * the real vnode. If the vnode is remote, vpx_usecount only contains
 * the local references to the vnode. When vpx_usecount goes to zero
 * the port is deallocated. The real vnode has exactly one vref that
 * accounts for all outstanding send rights to the vnode. That vref is
 * decremented when the NO_MORE_SENDERS message is received.
 */

struct vnode_proxy {
	long		vpx_usecount;	/* reference count of users */
	mach_port_t	vpx_port;	/* the attached port */
	struct vnode	*vpx_vnode;	/* the real vnode    */
#ifdef	_KERNEL
	udecl_simple_lock_data(,vpx_lock)	/* multiprocessor exclusion */
#endif
};

#define	VN_LOCK_PROXY(vpx)	usimple_lock(&(vpx)->vpx_lock)
#define	VN_UNLOCK_PROXY(vpx)	usimple_unlock(&(vpx)->vpx_lock)

#endif	/* OSF1_ADFS */

#endif	/* _SYS_VNODE_H_ */
