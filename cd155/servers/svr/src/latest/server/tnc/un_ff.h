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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: un_ff.h,v $
 * Revision 1.11  1995/02/01  22:04:42  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:44:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/07/29  21:53:57  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.8  1993/07/14  18:35:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/17  19:07:09  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.6  1993/05/06  19:27:42  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:57  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:09:51  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:36  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:13  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:28  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.8  93/07/27  11:29:13  mjl
 * [Bug #0308] The seqno mutex shared by all file structs attached to a FIFO is
 * replaced by a r/w lock in accordance with the locking hierarchy (i.e. first
 * take r/w locks, then take mutexes, then take simple spin locks).
 * 
 * Revision 3.7  93/06/24  12:45:32  mjl
 * Remove stale code.
 * 
 * Revision 3.6  93/05/07  19:07:47  mjl
 * Make UN_FF_DEBUG conditional on MACH_ASSERT.
 * 
 * Revision 3.5  1992/12/10  17:16:45  mjl
 * Use new debug macros.
 *
 * Revision 3.4  92/11/18  12:46:41  mjl
 * Add shared mutex used when computing # of outstanding threads on all of
 * the FIFO's file ports.
 * 
 * Revision 3.3  92/09/28  13:29:42  klh
 * Added macros for conditional debugging.  Define port_info_t structure for
 * keeping port-related information.  General rework of TNC data stroed with
 * FIFO vnodes (orginal mjl changes were lost, wrote over mjbs changes --klh)
 * 
 * Revision 3.0  92/08/17  12:43:38  mjl
 * Definitions for TNC FIFO relocation code.
 * 
 */

#ifndef	_TNC_UN_FF_H
#define _TNC_UN_FF_H

/*
 *  TNC definitions for FIFO relocation code.
 */

#include <sys/file.h>
#include <sys/vnode.h>
#include <tnc/un.h>
#include <kern/macro_help.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <mach_assert.h>

#if	MACH_ASSERT
#define UN_FF_DEBUG	1
#endif
#ifdef	UN_FF_DEBUG

extern int	un_ff_disable;
extern int	unffdebug;

#define UNFFDEBUG(mask,args)	if ((undebug|unffdebug) & (mask)) printf args

#define UNFFPORTINFO(port,str)				\
MACRO_BEGIN						\
	if (unffdebug & U_PORT)				\
		print_port_info( (port), (str));	\
MACRO_END

struct {
	int	xrefwait;	/* had to wait for non-file-port vnode op */
	int	halfopen;	/* couldn't kill rss due to half opens */
	int	portcreate;	/* # of ports created by set_redirect */
	int	portdestroywait;/* # of waits on nms prior to portdestroy */
	int	portdestroy;	/* # of ports destroyed by clr_redirect */
	int	enable_redirect;	/* # of calls to set_redirect */
	int	disable_redirect;	/* # of calls to clr_redirect */
} un_ff_statistics;

#define UNFFSTAT(field)	un_ff_statistics.field += 1

#else	/* ! UN_FF_DEBUG */

#define UNFFDEBUG(mask,args)
#define UNFFPORTINFO(port,str)
#define UNFFSTAT(field)

#endif	/* ! UN_FF_DEBUG */

/*
 *  Soft ports -- an abstraction useful for keeping track of all
 *  the port-related software maintained data.
 */
typedef struct port_info {
	mach_port_t		pi_name;
	mach_port_seqno_t	pi_seqno;
	mach_port_mscount_t	pi_mscount;
	mach_port_urefs_t	pi_srefs;
} port_info_t;

/*
 *  TNC FIFO-related data pointed to by vp->v_tncdata.
 */
typedef struct un_ff_data {
	policy_state_t	uf_pstate;	/* migration policy data */
	queue_head_t	uf_fileq;	/* list of file structs */
	long		uf_xflag;	/* extended flags, see below */
	long		uf_fcount;	/* count of files on chain */
	long		uf_factive;	/* count of active file ops */
	mach_port_t	uf_nrport;	/* namei redirection port */
	mach_port_t	uf_fsport;	/* send-rt for fs vnode */
	node_t		uf_fsnode;	/* node where fs vnode is */
	mach_port_urefs_t uf_srefs;	/* fifo port srights count */
	port_info_t	uf_saveport;	/* save/restore fifo port here */
	lock_data_t	uf_seqno_lock;	/* seqno lock for files on chain */
} un_ff_t;


/*
 *  Access macros for the un_ff_t structure.
 *
 *  Please make sure that vp->v_tncdata != NULL before calling these!
 *
 *  Note that those access macros that don't explicitly return an
 *  address can be used as either an lvalue or an rvalue,
 *  e.g. ``V_FCOUNT(vp) += 1;'' or ``count = V_FCOUNT(vp);''
 */

#define V_PSTATE(vp)	(&((un_ff_t *)((vp)->v_tncdata))->uf_pstate)
#define V_FILEQ(vp)	(&((un_ff_t *)((vp)->v_tncdata))->uf_fileq)
#define V_XFLAG(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_xflag)
#define V_FCOUNT(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_fcount)
#define V_FACTIVE(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_factive)
#define V_NRPORT(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_nrport)
#define V_FSPORT(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_fsport)
#define V_FSNODE(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_fsnode)
#define V_NRMSCNT(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_nrmscnt)
#define V_SREFS(vp)	(((un_ff_t *)((vp)->v_tncdata))->uf_srefs)
#define V_SAVEPORT(vp)	(&((un_ff_t *)((vp)->v_tncdata))->uf_saveport)
#define V_SEQLCK(vp)	(&((un_ff_t *)((vp)->v_tncdata))->uf_seqno_lock)

/*
 *  Lock macros for mutual exclusion when updating or examining
 *  file port sequence numbers for file structures attached to a
 *  FIFO vnode.  Note such file structures have fp->f_seqno_lock
 *  set to V_SEQLCK(vp), so these macros all access the same lock.
 *
 *  Note also that we don't need to hold the FP_LOCK() during
 *  FP_SEQNO_LOCK() and ...UNLOCK() since the fp->f_seqno_lock
 *  pointer is set only during opens and cleared only on final
 *  close, so there is no atomicity problem with testing whether
 *  this pointer is set.
 */

#define	VN_SEQNO_LOCK_INIT(vp)	lock_init(V_SEQLCK(vp), 1/*sleepable*/)
#define VN_SEQNO_LOCK(vp)	lock_write(V_SEQLCK(vp))
#define VN_SEQNO_UNLOCK(vp)	lock_done(V_SEQLCK(vp))

#define FP_SEQNO_LOCK(fp)				\
MACRO_BEGIN						\
	if (fp->f_seqno_lock)				\
		lock_write(fp->f_seqno_lock);		\
MACRO_END

#define FP_SEQNO_UNLOCK(fp)				\
MACRO_BEGIN						\
	if (fp->f_seqno_lock)				\
		lock_done(fp->f_seqno_lock);		\
MACRO_END

/*
 *  Flag argument values for FIFO_SET_REDIRECT().
 */
#define KEEPNAME	0
#define CHNGNAME	1

#define FIFO_SET_REDIRECT(vp, flag)	un_ff_set_redirect((vp), (flag))

#define FIFO_CLEAR_REDIRECT(vp)						\
MACRO_BEGIN								\
	ASSERT((vp)->v_tncdata && ((vp)->v_flag & VREDIRECT));		\
	V_NRPORT(vp) = MACH_PORT_NULL;					\
	(vp)->v_flag &= ~VREDIRECT;					\
MACRO_END

/*
 *  V_XFLAG(vp) flag bits.
 */
#define VX_STRG		0x01	/* storage site */
#define VX_FSYS		0x02	/* filesystem site */
#define VX_NRWAIT	0x04	/* namei() is waiting for redirection port */ 
#define VX_PDWAIT	0x08	/* waiting for vnode port death */
#define VX_ZOMBIE	0x10	/* "live" version of vnode has relocated */
#define VX_HADPORT	0x20	/* FIFO already had a vnode port */

/*
 *  This defines the wakeup channel that threads waiting for port death
 *  use.  The actual un_ff_t data item whose address is used has no
 *  relevance to port death waiting; I just needed to pick a channel.
 */
#define V_PDCHAN(vp)	((int)&V_FSNODE(vp))

/*
 *  Useful predicates using VX_STRG and VX_FSYS bits.
 *
 *  These macros return correct values even when vp->v_tncdata is NULL.
 *
 *  In TNC, the actual FIFO data storage can follow a "primary reader"
 *  process as it migrates around the multicomputer.  When the process
 *  being followed moves away from the filesystem node where the
 *  on-disk FIFO file resides, the FIFO vnode is "split" into a vnode
 *  at the process execution site that stores the data and a vnode on
 *  the filesystem node that forwards vnode operations to the storage
 *  vnode.
 */
#define V_STRG(vp)	(!vp->v_tncdata || (V_XFLAG(vp) & VX_STRG))
#define V_FSYS(vp)	(!vp->v_tncdata || (V_XFLAG(vp) & VX_FSYS))
#define V_STRG_ONLY(vp)	(!vp->v_tncdata || \
			 (V_XFLAG(vp)&(VX_STRG|VX_FSYS)) == VX_STRG)
#define V_FSYS_ONLY(vp)	(!vp->v_tncdata || \
			 (V_XFLAG(vp)&(VX_STRG|VX_FSYS)) == VX_FSYS)

/*
 *  Macros for maintaining the chain of file structs kept in the
 *  vnode.  This chain lets us keep track of all the file structs (and
 *  their file ports) that should be relocated along with the FIFO
 *  vnode.
 *
 *  The vnode lock must be held when calling these macros.
 */

#define VN_FILEQ_ADD(vp,fp)					\
MACRO_BEGIN							\
	queue_t q = V_FILEQ(vp);				\
	queue_enter(q, (fp), struct file *, f_chain);		\
	V_FCOUNT(vp)++;						\
	(fp)->f_seqno_lock = V_SEQLCK(vp);			\
MACRO_END

#define VN_FILEQ_REMOVE(vp,fp)					\
MACRO_BEGIN							\
	if (fp->f_chain.next != NULL) {				\
		queue_t q = V_FILEQ(vp);			\
		queue_remove(q, (fp), struct file *, f_chain);	\
		fp->f_chain.next = fp->f_chain.prev = NULL;	\
		V_FCOUNT(vp)--;					\
		fp->f_seqno_lock = NULL;			\
	}							\
MACRO_END


/*
 *  Temporary port names based on a vnode address.  These are used
 *  when manipulating vnode ports at the filesystem and storage nodes.
 */
#define SAVED_FSPORT_NAME(vp)	((mach_port_t)((int)vp + 1))
#define NRPORT_NAME(vp)		((mach_port_t)((int)vp + 2))
#define TEMP_PORT_NAME(vp)	((mach_port_t)((int)vp + 3))


/*
 *  Exported function prototypes
 */
extern void	tnc_vnode_port_destroyed(struct vnode *);

/*
 *  XXX See comment in un_ff_close().  A dreadful but necessary hack.
 */
#define REMOTE_FIFO_DEATH_DELAY_IN_TICKS	10

/*
 *  Miscellaneous
 */

#define INIT_SAVEPORT(vp)				\
MACRO_BEGIN						\
	ASSERT(vp->v_tncdata);				\
	bzero(V_SAVEPORT(vp), sizeof(port_info_t));	\
MACRO_END

#define FIFO_PORT_GONE(vp)				\
MACRO_BEGIN						\
	ASSERT((vp)->v_tncdata);			\
	(vp)->v_magic = 0;				\
	(vp)->v_seqno = 0;				\
	(vp)->v_mscount = 0;				\
	V_SREFS(vp) = 0;				\
MACRO_END

#endif	/* ! _TNC_UN_FF_H */

