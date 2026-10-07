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
 * $Log: buf.h,v $
 * Revision 1.9  1994/11/18  20:40:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/28  22:50:54  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 * 	    PTS 10033.
 *
 * Revision 1.7  1994/02/17  16:57:05  brad
 * Merged revision 1.6.2.1 from the R1.2 branch.
 *
 * Revision 1.6.2.1  1994/02/16  04:21:06  brad
 * Fixed flawed implementation of disk block preallocation.  Only preallocate
 * full file system blocks for simplicity.  Handle i_resfrags field in
 * the inode correctly.  Several errors in ufs_prealloc() fixed.
 *  Reviewer: Bob Godley
 *  Risk: Med
 *  Benefit or PTS #: 6318
 *  Testing: Ran PTS test.  Ran ORNL climate modelling code from bug #7266
 *     and verified lsize working now.  Ran PFS EATs and fileio EATs on
 *     64 nodes.  unmounted and force-ran fsck many times to ensure file
 *     systems clean.
 *  Module(s): server/ufs/{ufs_alloc,ufs_bmap,ufs_inode,ufs_vnops}.c
 *             server/sys/buf.h
 *
 * Revision 1.6  1993/09/25  20:18:29  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.5  1993/09/23  22:19:51  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.4.6.2  1993/09/25  20:14:24  cfj
 * Put #ifdef _KERNEL around the new buffer pool funnel structure and macros.
 *
 * Revision 1.4.6.1  1993/09/23  22:17:23  cfj
 * Define a new type of funnel to restrict the number of threads which
 * can concurrently have 2 cache blocks locked at the same time.
 *
 * Revision 1.4  1993/07/14  18:23:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:17  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:41:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:41  cfj
 * Bump major revision number.
 *
 *
 * Revision 2.11  94/02/03  11:01:23  dnoveck
 *      Add variables for per-node buffer-cache block size.
 *
 * Revision 2.9  93/10/20  15:26:47  dnoveck
 *      DEV_BSIZE elimination: fix buffer hashing not to depend on
 *      DEV_BSIZE or MAXBSIZE.
 *
 * Revision 2.8  1993/01/08  14:31:00  durriya
 * 	add b_devnode to buf struct for ADFS
 *
 * Revision 2.7  92/08/26  12:11:58  loverso
 * 	MAY_USE_BUFCACHE removed in favor of VIO_IS_BUF in vnode.h.
 * 	[92/08/14            roy]
 * 
 * Revision 2.6  92/07/29  08:22:37  rabii
 * 	Added B_RESERVE flag.
 * 	[92/07/20            roy]
 * 
 * Revision 2.5  92/06/08  18:21:59  pjg
 * 	Redefine MAY_USE_BUFCACHE to check the v_iomode field in the vnode
 * 	(OSF1_ADFS only) (pjg).
 * 
 * Revision 2.4  92/03/15  14:38:25  roy
 * 	92/02/28  13:59:54  roy
 * 	Added MAY_USE_BUFCACHE for OSF1_ADFS.
 * 
 * Revision 2.3  91/12/16  17:53:16  roy
 * 	91/10/21  18:47:44  emcmanus
 * 	Fixes to compile with asserts.
 * 
 * 	91/10/17  18:33:31  barbou
 * 	Added b_proc_dummy field for code reuse.
 * 	New field "b_optimize_mem" to provide some backward compatibility with 
 * 	some pieces of code that are not aware of the data copy optimization in 
 * 	bio_read_reply(). LVM needs this or a major code rewrite.
 * 
 * Revision 2.2  91/08/31  14:02:44  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.5  91/07/31  15:41:59  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16  90/10/31  14:06:14  devrcs
 * 	Add <sys/lock_types.h> to reduce the number of changes that
 * 	need to be made when porting a device driver.
 * 	[90/10/18  08:51:18  jeffc]
 * 
 * Revision 1.15  90/10/07  14:49:09  devrcs
 * 	Remove obsolete BUF_INHERIT macro, change comments describing how
 * 	the lock package debugging code is subverted. Removed obsolete
 * 	B_CALL definition.
 * 	[90/09/30  17:21:44  jeffc]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:37:13  gm]
 * 
 * 	Lock debugging.
 * 	[90/09/28  12:38:43  nags]
 * 
 * 	Include thread.h so that debug code internals aren't quite so visible
 * 	to users of these macros.  Provide null versions of debug macros so
 * 	that #if MACH_LDEBUG isn't necessary everyplace they get used.  Add
 * 	splbio()/splx() to those locking macros that require them. Add new
 * 	BUF_ACCEPT() macro for debugging assertions on buf locks; this replaces
 * 	the BUF_INHERIT() macro, and allows assertions that do not themselves
 * 	violate the lock ownership rules around calls to strategy.
 * 
 * 	Added b_cylin definition to ensure that there is no version
 * 	skew between users of disksort().
 * 	[90/09/25  19:03:48  jeffc]
 * 
 * 	Changed BUF_UNLOCK to reset B_BUSY before the call to lock_write_done
 * 	[90/09/17  16:20:39  noemi]
 * 
 * Revision 1.14  90/09/23  15:59:35  devrcs
 * 	Added b_pager as a synonym for b_bufsize.
 * 	[90/09/11  06:54:10  ers]
 * 
 * Revision 1.13  90/08/24  12:25:41  devrcs
 * 	BUF_GIVE_AWAY and BUF_INHERIT should always
 * 	be used under MACH_LDEBUG.
 * 	[90/08/18  03:49:23  nags]
 * 
 * Revision 1.12  90/08/09  13:28:33  devrcs
 * 	Added BUF_IS_LOCKED assertion.
 * 	[90/07/24  13:03:40  nags]
 * 
 * Revision 1.11  90/07/27  09:06:51  devrcs
 * 	Changes to b_iodone/B_CALL interface.
 * 	[90/07/19  15:38:31  jeffc]
 * 
 * 	Export asynchronous I/O lock debugging interface.
 * 	[90/07/20  17:05:42  nags]
 * 
 * 	Changed bremfree macro to always NULL out av_forw and av_back
 * 	fields. This is hopefully in preparation for allowing strategy
 * 	routines to receive a linked list of struct buf's. No impact on
 * 	system operation so far.  Added new Write Verify and Hardware
 * 	Relocate bits. (This is incomplete, but is innocuous in it's
 * 	present form).
 * 	[90/07/13  11:38:45  jeffc]
 * 
 * Revision 1.10  90/06/22  20:52:33  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Changed KERNEL to _KERNEL; cleaned #if labels.	gmf@osf.org
 * 	Change args to lock_init2()			gmf@osf.org
 * 	Buf's machine-specific fields become generic.	jeffc@osf.org
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Merged 4.4BSD and Mach changes			noemi@osf.org
 * 	Merged Mach 2.5 with Encore parallelization 	alan@encore.com
 * 	Remember the head of the buffer's hash chain.	alan@encore.com
 * 	Migrate to buffer I/O event scheme 		alan@encore.com
 * 	Added buffer, hash chain, and free list locks	alan@encore.com
 * 	Added B_USELESS - like B_AGE, but not ignored	rpd@cmu.edu
 * 	Added b_command field for Ultrix SCSI drivers.	af@cmu.edu
 * 	Added Mips specific B_NOCACHE flag		af@cmu.edu
 * 	Changes for I386: sysV fields and masks		rvb@cmu.edu
 * 	[90/06/12  21:37:46  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 *	@(#)buf.h	7.10 (Berkeley) 1/4/90
 */

#ifndef	_SYS_BUF_H_
#define _SYS_BUF_H_

#include <sys/types.h>
#ifdef	_KERNEL
#include <sys/unix_defs.h>
#include <kern/event.h>
#endif

/*
 * The header for buffers in the buffer pool and otherwise used
 * to describe a block i/o request is given here.  The routines
 * which manipulate these things are given in vfs/vfs_bio.c.
 *
 * Each buffer in the pool is usually doubly linked into 2 lists:
 * hashed into a chain by <vnode,blkno> so it can be located in the cache,
 * and (usually) on (one of several) queues.  These lists are circular and
 * doubly linked for easy removal.
 *
 * There are currently three queues for buffers:
 *	one for buffers which must be kept permanently (super blocks)
 * 	one for buffers containing ``useful'' information (the cache)
 *	one for buffers containing ``non-useful'' information
 *		(and empty buffers, pushed onto the front)
 * The latter two queues contain the buffers which are available for
 * reallocation, are kept in lru order.  When not on one of these queues,
 * the buffers are ``checked out'' to drivers which use the available list
 * pointers to keep track of them in their i/o active queues.
 */

/*
 * Bufhd structures used at the head of the hashed buffer queues.
 * We only need three words for these, so this abbreviated
 * definition saves some space.
 */
struct bufhd
{
	long	b_flags;		/* see defines below */
	struct	buf *b_forw, *b_back;	/* fwd/bkwd pointer in chain */
	u_long	bhd_stamp;		/* time stamp for hash chain */
#ifdef	_KERNEL
	udecl_simple_lock_data(,bhd_lock) /* hash chain spin lock */
#endif
};

struct buf
{
	long	b_flags;		/* too much goes here to describe */
	struct	buf *b_forw, *b_back;	/* hash chain (2 way street) */
	struct	buf *av_forw, *av_back;	/* position on free list if not BUSY */
	struct	buf *b_blockf, **b_blockb;/* associated vnode */
#define	b_actf	av_forw			/* alternate names for driver queue */
#define	b_actl	av_back			/*    head - isn't history wonderful */
	long	b_bcount;		/* transfer count */
#define	b_active b_bcount		/* driver queue head: drive active */
	long	b_bufsize;		/* size of allocated buffer */
#define	b_pager	b_bufsize
	short	b_error;		/* returned after I/O */
	dev_t	b_dev;			/* major+minor device name */
#ifdef OSF1_ADFS
        dev_t   b_devnode;              /* node # of where dev lives */
#endif
	union {
	    caddr_t b_addr;		/* low order core address */
	    int	*b_words;		/* words for clearing */
	    struct fs *b_fs;		/* superblocks */
	    struct filsys *b_s5fs;      /* System V fs suberblock */
	    struct csum *b_cs;		/* superblock summary information */
	    struct cg *b_cg;		/* cylinder group block */
	    struct dinode *b_dino;	/* ilist */
	    struct s5dinode *b_s5dino;	/* s5 ilist */
	    daddr_t *b_daddr;		/* indirect block */
	} b_un;
	daddr_t	b_lblkno;		/* logical block number */
	daddr_t	b_blkno;		/* block # on device */
	long	b_resid;		/* words not transferred after error */
#define b_errcnt b_resid		/* while i/o in progress: # retries */
#define	b_cylin	b_resid			/* while on disk q: cylinder #	*/
#ifndef	OSF1_SERVER
	struct  proc *b_proc;		/* proc doing physical or swap I/O */
#else	OSF1_SERVER
	struct	proc *b_proc_dummy;	/* for code reuse in LVM */
#define b_proc b_proc_dummy
	mach_port_t b_reply_port;	/* reply port for IO */
	int	b_optimize_mem;		/* optimize data copies by replacing
					 * b_un.b_addr by the address of the
					 * data already allocated by the Mach
					 * device interface. The buf user must
					 * be aware of this optimization, since
					 * the data won't be provided at the 
					 * requested address...
					 */
#endif	OSF1_SERVER	
#if	!MACH
	int	b_pfcent;		/* center page when swapping cluster */
#endif
	struct 	buf *b_hash_chain;	/* head of hash chain owning buffer */
	void	(*b_iodone)();		/* function called by iodone */
	struct	vnode *b_vp;		/* vnode for dev */
	struct	vnode *b_rvp;		/* vnode buffer is associated with */
	struct	ucred *b_rcred;		/* ref to read credentials */
	struct	ucred *b_wcred;		/* ref to write credendtials */
	int	b_dirtyoff;		/* offset in buffer of dirty region */
	int	b_dirtyend;		/* offset of end of dirty region */
	union	{	/* these fields reserved _solely_ for device driver */
		long	longvalue;
		void	*pointvalue;
		daddr_t	diskaddr;
		time_t	timevalue;
	} b_driver_un_1, b_driver_un_2;
#ifdef	_KERNEL
	lock_data_t	b_lock;		/* mutual exclusion buffer lock */
	event_t	b_iocomplete;		/* guard vnode while i/o in progress */
#endif
};

/*
 * Following #ifdefs/#defines are for compatibility with other naming schemes
 * long-term, they should be moved into the driver(s) that use them.
 */
#ifdef	mips
	/* SCSI command in progress */
	/* The fields below are just aliases */
#define b_command	b_driver_un_1.longvalue
#define b_gid		b_driver_un_1.longvalue
#endif

#ifdef exl
	/* added the missing fields for System V driver (SCSI) --- csy */
        /* physical sector of disk request */
#define b_sector	b_driver_un_1.diskaddr
	/* request start time */
#define b_start		b_driver_un_2.timevalue
#endif

#ifdef _KERNEL
/*
 * Funnel for threads executing critical sections of code that allow a thread
 * to hold more than one buffer at a time.  Without this synchronization, 
 * deadlock may potentially occur when many threads each hold one buffer, no
 * more buffers are in the free list, and then each thread attempts to get
 * a second buffer.  See balloc_nbc() in ufs_bmap.c (the problem also exists
 * in balloc()).
 */
struct bpool_funnel {
	int		b_count;	/* num threads in critical section */
	boolean_t	b_waiting;	/* are threads blocked on b_count? */
	udecl_simple_lock_data(,b_lock)
} b_funnel;

#define BFUNNEL_LOCK()		usimple_lock(&b_funnel.b_lock)
#define BFUNNEL_UNLOCK()	usimple_unlock(&b_funnel.b_lock)
#define BFUNNEL_LOCK_INIT()	usimple_lock_init(&b_funnel.b_lock)
#endif /* _KERNEL */


#define BQUEUES		4		/* number of free buffer queues */

#define BQ_LOCKED	0		/* super-blocks &c */
#define BQ_LRU		1		/* lru, useful buffers */
#define BQ_AGE		2		/* rubbish */
#define BQ_EMPTY	3		/* buffer headers with no memory */

#define	MINBUFHSZ	16		/* minimum size of bufhash */

#ifdef	_KERNEL
extern int	bufhsz;			/* size of buffer cache hash table */
#define	BBITS(vp, lbn)	((unsigned int)(lbn) >> (vp)->v_bufhash_shift)
#define	VBITS(vp)	((unsigned int)(vp) >> 11)	/* DFLT_512 */
#define	BHCHAIN(vp, lbn) ((VBITS(vp)+BBITS(vp, lbn)) & (bufhsz-1))
#define	BUFHASH(dvp, lbn)       \
	((struct buf *)&bufhash[BHCHAIN(vp, lbn)])

extern struct	buf *buf;	/* the buffer pool itself */
extern char	*buffers;
extern int	nbuf;		/* number of buffer headers */
extern int	bufpages;	/* number of memory pages in the buffer pool */

extern struct	bufhd *bufhash;		/* heads of hash lists */
extern struct	buf bfreelist[BQUEUES];	/* heads of available lists */

extern struct	buf *getblk();
extern struct	buf *geteblk();
extern struct	buf *getnewbuf();

extern unsigned minphys();
extern int     bcache_maxbsize;		/* Maximum block in buffer cache. */
extern int     bcache_maxbshift;	/* Associated shift. */
extern int     bcache_maxdgsize;	/* Maximum block in buffer cache */
					/* specified in disk granules. */
extern int	bcache_maxdgshift;	/* Associated shift. */

#endif	/* _KERNEL */

/*
 * These flags are kept in b_flags.
 *
 * NOTE:
 *	The following flags are provided purely for backward 
 *	compatibility with code (drivers, mostly) that
 *	expect them to be around.  We'd like them to go away:
 *		B_BUSY, B_DONE, B_WANTED
 */
#define B_WRITE		0x00000000	/* non-read pseudo-flag */
#define B_READ		0x00000001	/* read when I/O occurs */
#if	!MACH
#define B_DONE		0x00000002	/* transaction finished */
#endif
#define B_ERROR		0x00000004	/* transaction aborted */
#define B_BUSY		0x00000008	/* not on av_forw/back list */
#define B_PHYS		0x00000010	/* physical IO */
#if	!MACH
#define B_XXX		0x00000020	/* was B_MAP, alloc UNIBUS on pdp-11 */
#endif
#define B_WANTED	0x00000040	/* issue wakeup when BUSY goes off */
#define B_AGE		0x00000080	/* delayed write for correct aging */
#define B_ASYNC		0x00000100	/* don't wait for I/O completion */
#define B_DELWRI	0x00000200	/* write at exit of avail list */
#define B_TAPE		0x00000400	/* this is a magtape (no bdwrite) */
#if	!MACH
#define B_UAREA		0x00000800	/* add u-area to a swap operation */
#define B_PAGET		0x00001000	/* page in/out of page table space */
#define B_DIRTY		0x00002000	/* dirty page to be pushed out async */
#define B_PGIN		0x00004000	/* pagein op, so swap() can count it */
#endif
#define B_CACHE		0x00008000	/* did bread find us in the cache ? */
#define B_INVAL		0x00010000	/* does not contain valid info  */
#define B_LOCKED	0x00020000	/* locked in core (not reusable) */
#define B_HEAD		0x00040000	/* a buffer header, not a buffer */
#define B_USELESS	0x00080000	/* cache, but at low priority */
#define B_BAD		0x00100000	/* bad block revectoring in progress */
#define	B_RAW		0x00400000	/* set by physio for raw transfers */
#define	B_NOCACHE	0x00800000	/* do not cache block after use */
#define	B_PRIVATE	0x01000000	/* private data, not part of buffers */
#define	B_WRITEV	0x02000000	/* perform verification of writes */
#define	B_HWRELOC	0x04000000	/* relocate/rewrite block */
#define B_WANTFREE	0x08000000	/* want buffer from freelist */

#define BHASH_NULL	(struct buf *)0

#ifdef	_KERNEL
#include <kern/macro_help.h>

/*
 * Insq/Remq for the buffer hash lists.
 */
#define	bremhash(bp) \
MACRO_BEGIN \
	(bp)->b_back->b_forw = (bp)->b_forw; \
	(bp)->b_forw->b_back = (bp)->b_back; \
	(bp)->b_hash_chain = BHASH_NULL; \
MACRO_END
#define	binshash(bp, dp) \
MACRO_BEGIN \
	(bp)->b_forw = (dp)->b_forw; \
	(bp)->b_back = (dp); \
	(bp)->b_hash_chain = (dp); \
	(dp)->b_forw->b_back = (bp); \
	(dp)->b_forw = (bp); \
	(BHASH_STAMP(dp))++; \
MACRO_END

/*
 * Insq/Remq for the buffer free lists.
 */

#define bremfree(bp)						\
MACRO_BEGIN							\
	(bp)->av_back->av_forw = (bp)->av_forw;			\
	(bp)->av_forw->av_back = (bp)->av_back;			\
	(bp)->av_forw = NULL;					\
	(bp)->av_back = NULL;					\
MACRO_END

#define binsheadfree(bp, dp)					\
MACRO_BEGIN							\
	(dp)->av_forw->av_back = (bp);				\
	(bp)->av_forw = (dp)->av_forw;				\
	(dp)->av_forw = (bp);					\
	(bp)->av_back = (dp);					\
MACRO_END

#define binstailfree(bp, dp)					\
MACRO_BEGIN							\
	(dp)->av_back->av_forw = (bp);				\
	(bp)->av_back = (dp)->av_back;				\
	(dp)->av_back = (bp);					\
	(bp)->av_forw = (dp);					\
MACRO_END

#define iodone	biodone
#define iowait	biowait

/*
 * Zero out a buffer's data portion.
 */
#define clrbuf(bp)						\
MACRO_BEGIN							\
	blkclr((bp)->b_un.b_addr, (unsigned)(bp)->b_bcount);	\
	(bp)->b_resid = 0;					\
MACRO_END
#define B_CLRBUF	0x1	/* request allocated buffer be cleared */
#define B_SYNC		0x2	/* do all allocations synchronously */
#define B_RESERVE	0x4	/* allocate reserved blocks */
#define B_PREALLOC	0x8	/* preallocate file blocks */

typedef struct bufhd bufhd_t;

/*
 * There are a couple of places outside of the buffer cache code
 * (in ../vfs/vfs_bio.c) that use buffer locks, and many places
 * that make assertions about the states of buffer locks, so these
 * definitions must be public.
 */
#include <sys/lock_types.h>
#define	BUF_LOCKINIT(bp)	lock_init2(&(bp)->b_lock, TRUE, LTYPE_BUF)

#define	BUF_LOCK(bp)						\
MACRO_BEGIN							\
	int s = splbio();					\
	lock_write(&(bp)->b_lock);				\
	(bp)->b_flags |= B_BUSY;				\
	splx(s);						\
MACRO_END

#define	BUF_UNLOCK(bp)						\
MACRO_BEGIN							\
	int s = splbio();					\
	(bp)->b_flags &= ~B_BUSY;				\
	lock_write_done(&(bp)->b_lock);				\
	splx(s);						\
MACRO_END

#define BUF_LOCK_TRY(bp, ret)					\
MACRO_BEGIN							\
	int s = splbio();					\
	if (ret = lock_try_write(&(bp)->b_lock)) 		\
		(bp)->b_flags |= B_BUSY;			\
	splx(s);						\
MACRO_END

#define	BUF_LOCKED(bp)	(LOCK_LOCKED(&(bp)->b_lock))

/*
 * Asynchronous I/O presents problems for the lock checking package.
 * The sequence of events is:
 *	fetch the buffer, locking it
 *	start the I/O
 *	current thread goes about its business
 *	an innocent victim thread inherits the buffer in interrupt
 *		context and brelse's it during I/O completion,
 *		releasing the buffer's lock.
 *
 * Convincing the lock checking package not to complain during
 * this process is a bit tricky.  Here are some of the problems.
 *
 * Failing to give away ownership of the buffer lock when
 * issuing the I/O request causes problems should the current
 * thread at some future time issue another request for the
 * same buffer it originally asked to read-ahead.  The lock
 * checking package will complain about a deadlock. (waiting for self)
 *
 * The solution requires two steps.  The thread initiating the I/O
 * gives the ownership of the lock away to a fake thread, biodone_ldebug.
 * The 'thread' that receives the buffer accepts the ownership of the lock
 * to itself, using BUF_ACCEPT.
 *
 * This scheme eliminates races in the ownership handoff, but strategy
 * routines are no longer allowed to assert that they own asynchronous
 * I/O buffers. They can assert that the buffer is locked, that they
 * own synchronous buffers, or that the "fake thread" owns asynchronous
 * buffers.
 */

#if	MACH_LDEBUG
extern char biodone_ldebug;

#ifndef	OSF1_SERVER
#include <kern/thread.h>
#define buf_thread_t thread_t
#else
#include <sys/user.h>
#define buf_thread_t uthread_t
#endif

#define	BUF_LOCK_THREAD(bp)	(LOCK_THREAD(&(bp)->b_lock))
#define	BUF_LOCK_OWNER(bp)	(LOCK_OWNER(&(bp)->b_lock))
#define	BUF_LOCK_HOLDER(bp)	(LOCK_HOLDER(&(bp)->b_lock))
#define BUF_IS_LOCKED(bp)	(!LOCK_READERS(&(bp)->b_lock) && \
				 LOCK_LOCKED(&(bp)->b_lock))
/*
 * BUF_GIVE_AWAY(bp): give away ownership of an asynchronous
 * I/O request buffer.
 */
#define	BUF_GIVE_AWAY(bp)					\
MACRO_BEGIN							\
	int s = splbio();					\
	simple_lock(&(bp)->b_lock.interlock);			\
	assert((buf_thread_t)(bp)->b_lock.lthread == current_thread());	\
	dec_lock(&(bp)->b_lock, (bp)->b_lock.lthread);  	\
	(bp)->b_lock.lthread =  &biodone_ldebug; 		\
	simple_unlock(&(bp)->b_lock.interlock);			\
	splx(s);						\
MACRO_END
/*
 * BUF_ACCEPT(bp): accept an asynchronous I/O buffer that was
 * previously given away.
 */
#define	BUF_ACCEPT(bp)						\
MACRO_BEGIN							\
	int s = splbio();					\
	simple_lock(&(bp)->b_lock.interlock);			\
	assert((bp)->b_lock.lthread == &biodone_ldebug);	\
	inc_lock(&(bp)->b_lock, current_thread());		\
	(bp)->b_lock.lthread = (char *) current_thread();	\
	simple_unlock(&(bp)->b_lock.interlock);			\
	splx(s);						\
MACRO_END
#else	/* MACH_LDEBUG */
#define BUF_GIVE_AWAY(bp)
#define	BUF_ACCEPT(bp)
#endif	/* MACH_LDEBUG */
#endif	/* _KERNEL */
#endif	/* _SYS_BUF_H_ */
