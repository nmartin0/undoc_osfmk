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
 * $Log: specdev.h,v $
 * Revision 1.6  1994/11/18  20:41:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:27:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:16  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/14  23:19:38  cfj
 * Define PUT_NODE_IN_SPECINFO macro to check if the node number is
 * NONODE, and if so pass this_node as the node number.
 *
 * Revision 1.3  1993/05/06  19:20:04  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:44:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.4  1992/05/24  14:37:58  pjg
 * Revision 1.1.1.1  1993/05/03  17:42:45  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  93/05/18  14:41:27  loverso
 * 	Added macro PUT_NODE_IN_SPECINFO (cfj@ssd.intel.com)
 * 
 * Revision 2.6  93/03/23  11:58:13  condict
 * 	Eliminate references to read_count field of locks. Use LOCK_READERS
 * 	macro defined in kern/lock.h instead.
 *
 * Revision 2.5  93/01/08  14:31:38  durriya
 * 	add sa_devnode to the specalias struct                     durriya
 * 
 * Revision 2.4  92/05/24  14:37:58  pjg
 * 	Renamed si_node and v_node to si_devnode and v_devnode.
 * 
 * Revision 2.3  92/01/05  20:09:13  roy
 * 	1991/11/12  17:27:59  noemi
 * 	New special files implementation.
 * 
 * Revision 2.2  91/08/31  14:13:00  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:45:20  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:54:13  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:12  gm]
 * 
 * Revision 1.2  90/06/22  20:54:10  devrcs
 * 	nags merge
 * 
 * 	Condensed history:
 * 	Created for special file and alias handling. 	nags@encore.com
 * 	[90/06/12  21:39:14  gmf]
 * 
 * $EndLog$
 */

#ifndef _SYS_SPECDEV_H_
#define	_SYS_SPECDEV_H_

#ifdef	_KERNEL
#include <sys/unix_defs.h>

/*
 * This file contains data structures that deal with special files and
 * aliases for special files.  Whenever a special file is activated
 * (via pathname translation), a struct specinfo is allocated to hold
 * device file-specific information.  An alias (struct specalias) is a 
 * structure that associates all vnodes for a given open device.  They 
 * are required because it is possible for more than one special file to 
 * represent the same device. 
 */


/*
 * This structure is allocated when a vnode for a special file is created
 * during pathname translation. 
 * Locking:
 * 	All fields in this structure are protected by the spec hashchain
 *	lock for the device.
 * NOTE:
 *	See IMPORTANT NOTE below about alignment of si_rdev.
 * 
 * For OSF/1 AD systems, the si_lock protects the specinfo structure's fields
 * and the si_rdwrlock protects the mapping between special file vnodes on the
 * node servicing the special file and device vnodes on the node servicing the
 * device.  si_lock is a spin lock and si_rdwrlock is a read/write lock that is
 * acquired for writing when setting up or destroying the special file to
 * device mapping.  Otherwise, the lock is held for reading.  The si_rdwrlock
 * is held for writing during the first open of the special file and while
 * reclaiming special file vnodes representing remote devices (and their
 * associated pseudo vnodes on remote nodes). It is also held for writing
 * when changing the si_specport field.  The si_lock is held when changing
 * the si_flag field.  The si_node and si_rdev fields are protected by the
 * vnode's spein lock.  The other fields are protected by the spechash lock.
 */

struct specinfo {
	dev_t		 si_rdev;
#ifdef	OSF1_ADFS
	node_t		 si_devnode;
	u_long		 si_flag;
#endif
	struct vnode	 *si_nextalias;		/* next vnode on alias list */
	struct vnode	 *si_shadowvp;		/* shadow vnode for VBLK */
	struct specalias *si_alias;		/* our alias structure */
#ifdef	OSF1_ADFS
	mach_port_t	 si_specport;		/* spec / device vnode port */
	lock_data_t	 si_rdwrlock;		/* read/write lock */
	udecl_simple_lock_data(,si_lock)	/* spin lock */
#endif
};

/*
 * This structure is allocated when a device is opened (after pathname
 * translation.  It serves as an alias for multiple vnodes referencing the
 * same device.  If it is a block device, it has an associated vnode that
 * is used for operation involving the buffer cache (read, write, close).
 *
 * IMPORTANT NOTE:
 *	The sa_rdev field of this structure MUST be first to make this 
 *	look like a struct specinfo.  Buffer cache functions need to
 *	access the v_rdev field of the vnode, and the shadow vnodes use
 *	one of these rather than a struct specinfo.
 */

struct specalias {
	dev_t		sa_rdev;
#ifdef	OSF1_ADFS
	node_t  	sa_devnode;     /* node where device lives */
#endif
	u_long		sa_flag;
	enum vtype 	sa_type;	/* type of alias (VCHR, VBLK) */
	long		sa_usecount;	/* count of vnodes aliased */
	struct vnode	*sa_vlist;	/* next vnode on alias list */
	struct vnode 	*sa_vnode;	/* our vnode, if there is one */
	struct specalias *sa_next;	/* next struct on hash chain */
};

/*
 * Data structures for aliasing special vnodes
 */

struct spechash {
	struct specalias *sh_alias;		/* the hash chain */
	udecl_simple_lock_data(,sh_lock)	/* a spin lock */
};

#define v_rdev v_specinfo->si_rdev
#ifdef	OSF1_ADFS
#define v_devnode v_specinfo->si_devnode
#endif

/*
 * flags for specalias struct
 */
#define SA_MOUNTED	0x0001	/* file system mounted here */
#define SA_CLOSING	0x0002	/* close in progress */
#define SA_GOING	0x0004	/* clearalias in progress */
#define SA_WAIT		0x0008	/* someone waiting on one of above */

/*
 * Flags passed to setmount.
 */
#define SM_MOUNTED	0x0001		/* check to see if device is mounted */
#define SM_OPEN		0x0002		/* check to see if device is open */
#define SM_SETMOUNT	0x0004		/* mark the device as mounted */
#define SM_CLEARMOUNT	0x0008		/* clear the mounted flag on device */

#ifdef	OSF1_ADFS
/*
 * Macros for specinfo spin lock
 */
#define SI_LOCK(si)             usimple_lock(&(si)->si_lock)
#define SI_UNLOCK(si)           usimple_unlock(&(si)->si_lock)
#define SI_LOCK_TRY(si)         usimple_lock_try(&(si)->si_lock)
#define SI_LOCK_INIT(si)        usimple_lock_init(&(si)->si_lock)
#define SI_LOCK_HOLDER(si)      SLOCK_HOLDER(&(si)->v_lock)

/*
 * Macros for specinfo read/write lock.
 */
#define SPEC_READ_LOCK(si)	lock_read(&(si)->si_rdwrlock)
#define SPEC_WRITE_LOCK(si)	lock_write(&(si)->si_rdwrlock)
#define SPEC_READ_UNLOCK(si)	lock_read_done(&(si)->si_rdwrlock)
#define SPEC_WRITE_UNLOCK(si)	lock_write_done(&(si)->si_rdwrlock)
#define SPEC_LOCK_INIT(si)	lock_init2(&(si)->si_rdwrlock, TRUE, LTYPE_SPEC)
#define SPEC_WRITE_HOLDER(si)	LOCK_HOLDER(&(si)->si_rdwrlock)
#define SPEC_READ_HOLDER(si)	LOCK_READERS(&(si)->si_rdwrlock)

/*
 * Flags for specinfo structure.
 */
#define	SI_RMTDEV	0x0001		/* device is remote */
#define	SI_RMTSPEC	0x0002		/* special file is remote */
#define	SI_LOCAL	0x0004		/* special file and device are local */
#define SI_RECLAIM	0x4000		/* spec file vnode is being reclaimed */
#define	SI_OPEN		0x8000		/* device is open */
#define	SI_CLOSED	0		/* device is closed */

/*
 *  Macro to determine whether node is valid.
 */
#define PUT_NODE_IN_SPECINFO(node) ((node == NONODE) ? (this_node) : (node))

#endif	/* OSF1_ADFS */

#define MINSPECHSZ	16
extern int spechsz;
extern struct spechash *speclisth;
extern int	setmount();		/* manipulate mount-related aliases */

#endif	/* _KERNEL */
#endif	/* _SYS_SPECDEV_H_ */
