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
 * $Log: param.h,v $
 * Revision 1.16  1994/11/18  20:40:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/07/24  18:19:55  dbm
 * Added new definition DFLT_BCBSIZE to define the default buffer cache size.
 *
 *  Reviewer:None
 *  Risk:Low
 *  Benefit or PTS #: 10269
 *  Testing: Specific test case.
 *  Module(s):
 * 	param.h
 * 	vfs/vfs_bio.c
 *
 * Revision 1.14  1994/07/13  18:28:58  chrisp
 * For i386 systems, define default disk granules to retain compatibility
 * with existing filesystems.
 *
 *  Reviewer: None
 *  Risk: L
 *  Benefit or PTS #: 10183
 *  Testing: 386 compatibility verified
 *  Module(s): server/ufs/ufs_vfsops.c, server/sys/param.h
 *
 * Revision 1.13  1994/06/28  22:53:18  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.12  1994/02/11  23:17:42  cfj
 * Merge version 1.11.4.1 from R1_1 to bump SHMSEG to 20.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.11.4.1  1994/02/11  23:10:54  cfj
 * Raise the number of shared memory segments to 20.
 *
 *  Reviewer:shala
 *  Risk:L
 *  Benefit or PTS #:8106
 *  Testing:This has been in use at NRAD for months.
 *  Module(s):server/sys/param.h
 *
 * Revision 1.11  1993/07/14  18:26:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.10  1993/06/04  17:52:43  cfj
 * Raised the values for some SysV IPC parameters.
 *
 * Revision 1.9  1993/05/22  18:28:01  brad
 * Added MINDEV_BSIZE as part of the RAID 3 block size support.
 *
 * Revision 1.8  1993/05/18  04:03:14  cfj
 * Set DEV_BSIZE to 2K and MAXBSIZE to 64K.
 *
 * Revision 1.7  1993/05/06  19:19:45  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/03  03:07:42  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/02/24  19:47:26  cfj
 * Define NONODE for DEV_TAB <0>0 bug fix from OSF.
 *
 * Revision 1.1.2.1.2.1  1993/01/09  00:04:57  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.4  1993/01/05  00:49:56  shala
 * Defined HZ.
 *
 * Revision 1.3  1992/12/18  23:20:52  cfj
 * Changed MAXBSIZE back to 8K.
 *
 * Revision 1.2  1992/11/30  22:43:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:34  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/14  17:18:27  cfj
 * NX integration.
 *
 * Revision 2.6  94/02/03  11:01:29  dnoveck
 *      Make MAXBSIZE 512K and define MIN_BCBSIZE as part of per-node
 *      buffer-cache block size code.
 *
 * Revision 2.5  93/10/20  15:26:54  dnoveck
 *      DEV_BSIZE elimination: Reorganize explanation of block sizes.
 *      Add stuff for disk granule.  Put remnants of DEV_BSIZE defines
 *      under #ifndef _KERNEL until AD rev2.  Get rid of MAXFRAG.
 *      Make MAXBSIZE 64k.
 *
 * Revision 2.4  1993/04/29  14:01:19  klh
 * 	Revision 2.4  93/04/08  11:32:29  loverso
 * 		Revision 3.8  92/04/22  16:49:33  barbou
 * 		Fix for bug #133: implemented thread_should_halt().
 *
 * 	Revision 2.3  93/01/08  14:31:24  durriya
 * 		define NONODE
 *
 * Revision 2.3  93/03/10  14:17:16  yazz
 * Synchronous close merge from Intel: define NONODE.
 * 
 * Revision 2.2  91/08/31  14:09:26  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.7  91/08/13  15:04:36  sp
 * Add argument to thread_should_halt() again to keep source more in line
 * with the monolithic source.
 * 
 * Revision 3.6  91/07/31  15:44:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14  90/10/31  14:06:49  devrcs
 * 	Replace use of NZERO with PRIZERO
 * 	[90/10/25  14:30:33  sp]
 * 
 * Revision 1.13  90/10/07  14:52:43  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:08:40  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:43:00  gm]
 * 
 * 	changes for gcc build
 * 	[90/03/28  14:00:20  duthie]
 * 
 * 	Changed definition of MAXURPC, NOFILE, NCARGS, NGROUPS as in
 * 	4.4BSD, to reduce number of explicit constants.
 * 	[90/01/25  18:51:36  coren]
 * 
 * 	Posix tty support.
 * 	[90/02/06  12:16:19  morris]
 * 
 * Revision 1.12  90/09/23  16:00:05  devrcs
 * 	Increased size of NCARGS to accommodate the one or possibly two
 * 	strings, used by the auxiliary vector, added to the "exec arglist"
 * 	area.  Both of them are pathnames, one for the file argument to
 * 	exec() and the other for the pathname of the loader, if a loader
 * 	is used to load the program.
 * 	[90/09/12  07:49:23  kwallace]
 * 
 * Revision 1.11  90/08/24  12:26:42  devrcs
 * 	Put ifdef _KERNEL around some kernel-specific information.
 * 	[90/08/21  14:55:32  gmf]
 * 
 * 	Changes for sleep, tsleep, and mpsleep interfaces.
 * 	[90/08/20  06:40:55  gmf]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:37:15  mcg]
 * 
 * Revision 1.10  90/08/09  13:28:57  devrcs
 * 	Define SHMMNI as 100.
 * 	[90/08/03  16:26:16  bet]
 * 
 * Revision 1.9  90/05/13  20:33:49  devrcs
 * 	Changes for gcc for osc.10
 * 	[90/05/02  19:43:17  brezak]
 * 
 * 	Changes for gcc
 * 	[90/05/01  21:49:48  brezak]
 * 
 * Revision 1.8  90/04/27  19:26:41  devrcs
 * 	Removed SYSV_IPC conditional and
 * 	added definitions for SysV shared memory.
 * 	[90/04/20  10:52:15  bet]
 * 
 * Revision 1.7  90/03/13  20:49:38  mbrown
 * 	Merged SYS V IPC stuff.
 * 	[90/02/22  09:31:17  tom]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:03:13  tom]
 * 
 * 	Removed temporary comments.
 * 	[90/01/26  16:03:00  coren]
 * 
 * Revision 1.6  90/02/23  00:32:59  devrcs
 * 	Added definitions for System V IPC messages and semaphores.
 * 	[90/02/15  19:38:48  bet]
 * 
 * 	Corrected a screwup in the history log entry for previous version.
 * 	[90/01/31  13:21:11  coren]
 * 
 * Revision 1.5  90/02/16  16:52:28  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:22:46  ers]
 * 
 * Revision 1.4  90/01/03  12:43:43  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:15  gm]
 * 
 * Revision 1.3  89/12/26  10:36:49  gm
 * 	Removed new definition of BSD for now. It causes a preprocessor
 * 	warning about a redefined symbol.
 * 	Merged 4.4BSD and Mach changes.
 * 	[89/12/25            ers]
 * 
 * Revision 1.2  89/10/26  08:07:28  gm
 * 	MACH X115 Update
 * 
 * Revision 6.1  89/07/26  14:20:59  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.12  89/10/11  14:53:22  dlb
 * 	Minor macro changes to pass thread to thread_should_halt.
 * 	[88/10/18            dlb]
 * 
 * Revision 2.11  89/04/22  15:32:15  gm0w
 * 	Removed MACH_VFS changes.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.10  89/03/09  22:06:20  rpd
 * 	More cleanup.
 * 
 * Revision 2.9  89/02/25  17:55:14  gm0w
 * 	Made CMUCS and MACH conditionals always true and got
 * 	rid of the non-CMUCS and non-MACH code. Changed
 * 	romp to ibmrt and EXL to exl.
 * 	[89/02/13            mrt]
 * 
 * Revision 2.8  89/01/23  22:27:51  af
 * 	Added definition of domainname string length, reduced some
 * 	extra #include statements.
 * 	[89/01/09            af]
 * 
 * 	Changes for I386 -- define _bit_<foo> like for a vax??
 * 		I guess we just don't use them anymore.
 * 	[89/01/09            rvb]
 * 
 * Revision 2.7  89/01/21  13:33:29  jsb
 * 	Bump MAXSYMLINKS up from 8 to 32.
 * 
 * Revision 2.6  89/01/18  01:18:11  jsb
 * 	Vnode support: define MAXLINK, PMASK and PCATCH.
 * 	[89/01/13            jsb]
 * 
 * Revision 2.5  88/12/21  14:19:01  mja
 * 	Allow the file to be included recursively.
 * 
 * Revision 2.4  88/08/24  02:38:21  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:19:37  mwyoung]
 * 
 * Revision 0.0  88/05/23            mja
 * 	CMUCS:  Bump maximum user process count to 64 (it is patched
 * 	down to the standard 40 on time-sharing systems).
 * 	[ V5.1(XF26) ]
 * 	[88/05/23            mja]
 * 
 * Revision 0.0  88/04/20            dlb
 * 	Change to signal macros to allow use of exception signals as
 * 	back door ipc.  [XXX]
 * 	[88/04/20            dlb]
 * 
 * Revision 0.0  87/12/21            dbg
 * 	MACH: ISSIG() must check for per-thread exit conditions.
 * 	[87/12/21            dbg]
 * 
 * Revision 0.0  87/09/17            rvb
 * 	Split up ISSIG() so I can tell if issig() would be called.  This
 * 	is neccessary for astcheck which is running at interrupt level,
 * 	and for trap() that does now want to acquire/release the
 * 	unix_lock twice.
 * 	[87/09/17            rvb]
 * 
 * Revision 0.0  87/03/17            dlb
 * 	MULTIMAX & BALANCE: MAXUPRC -> 100
 * 	[87/03/17            dlb]
 * 
 * Revision 0.0  87/01/31            avie
 * 	machparam.h was a 4.3 *feature*, so I have changed the include
 * 	of machine/param.h back to machine/machparam.h.
 * 	[87/01/31            avie]
 * 
 * Revision 0.0  87/01/29            avie
 * 	Make tsleep stuff conditional on CMUCS instead of CS_COMPAT.
 * 	[87/01/29            avie]
 * 
 * Revision 0.0  86/10/29            avie
 * 	Moved INCLUDE_VERSION to h/version.h
 * 	[86/10/29            avie]
 * 
 * Revision 0.0  86/10/01            avie
 * 	INCLUDE_VERSION changed to -5 to account for different proc
 * 	structure and elimination of task and thread tables.
 * 	[86/10/01            avie]
 * 
 * Revision 0.0  86/09/29            mwyoung
 * 	Conditionalized which version of <machine/param.h> to get on
 * 	CMUCS.  Added separate INCLUDE_VERSION for MACH case.
 * 	Rearranged PSEM and PIPC under appropriate conditionals.
 * 	[86/09/29            mwyoung]
 * 
 * Revision 0.0  86/09/18            mwyoung
 * 	Bumped include version to -4, to account for different
 * 	TBL_* constants after merge with Facilities system.
 * 	Shouldn't the INCLUDE_VERSION really be in another file?
 * 	[86/09/18            mwyoung]
 * 
 * Revision 0.0  86/09/02            mwyoung
 * 	Bumped INCLUDE_VERSION to -3, to reflect changes in
 * 	"vm/vm_map.h".
 * 	[86/09/02            mwyoung]
 * 
 * Revision 0.0  86/08/23            mwyoung
 * 	Changed "machparam.h" reference to "machine/param.h".
 * 	[86/08/23            mwyoung]
 * 
 * Revision 0.0  86/07/27            bolosky
 * 	Changed ROMP to romp.
 * 	[86/07/27            bolosky]
 * 
 * Revision 0.0  86/06/10            mja
 * 	CMUCS:  Added INCLUDE_VERSION definition.
 * 	[86/06/10            mja]
 * 
 * Revision 0.0  86/03/22            avie
 * 	Merged VM and Romp versions.
 * 	[86/03/22            avie]
 * 
 * Revision 0.0  86/03/19            bolosky
 * 	Fixed definitions of the bit manipulation macros (setbit, etc.)
 * 	so that their arguments may be of any type (as opposed to having
 * 	to be of a single byte length type).
 * 	[86/03/19            bolosky]
 * 
 * Revision 0.0  86/02/28            bolosky
 * 	Added defintions of _bit_{set,clear,tst} for romp.
 * 	[86/02/28            bolosky]
 * 
 * Revision 0.0  86/02/20            bolosky
 * 	Spiffed up and included IBM code for Sailboat which handles the
 * 	inclusion of signal.h when we are running on the romp
 * 	(specifically, not to include it when building locore, or all
 * 	hell will break loose).
 * 	[86/02/20            bolosky]
 * 
 * Revision 0.0  86/01/25            avie
 * 	Upgraded to 4.3 and increased MAXHOSTNAMELEN to 256 [CMUCS].
 * 	Also removed CS_NOFILE.
 * 	[86/01/25            avie]
 * 
 * Revision 0.0  85/05/23            gm0w
 * 	CS_COMPAT:  Added tsleep() return values from 4.1BSD [V1(1)].
 * 	[85/05/23            gm0w]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)param.h	7.1 (Berkeley) 8/26/89
 */

#ifndef	_SYS_PARAM_H_
#define _SYS_PARAM_H_

#define BSD	198911		/* system version (year & month) */
#define BSD4_3  1

#ifdef _KERNEL
#ifdef	OSF1_SERVER
#ifdef	KERNEL
#include <mach_assert.h>
#endif	KERNEL
#endif	OSF1_SERVER
#endif
#include <sys/types.h>
#include <sys/limits.h>
#include <machine/machparam.h>

#define HZ CLOCKS_PER_SEC       /* see machine/machtime.h */

/*
 * Machine-independent constants
 */
#define NMOUNT	20		/* number of mountable file systems */
/* NMOUNT must be <= 255 unless c_mdev (cmap.h) is expanded */
#define MSWAPX	NMOUNT		/* pseudo mount table index for swapdev */
#define	MAXUPRC	CHILD_MAX	/* max processes per user */
#define	NOFILE	OPEN_MAX	/* max open files per process */
#define MAXLINK LINK_MAX		/* ??? */
#define	CANBSIZ	MAX_CANON		/* max size of typewriter line */
#define	NCARGS	(ARG_MAX + (2 * PATH_MAX))
				/* # characters in exec arglist */
#define	MAXINTERP	32	/* maximum interpreter file name length */
#define NGROUPS	NGROUPS_MAX	/* max number groups */
#define MAXHOSTNAMELEN	64	/* maximum hostname size */

#define NOGROUP	65535		/* marker for empty group set member */

/*
 * Priorities
 */
#define PSWP	0
#define PINOD	10
#ifdef	OSF1_SERVER
#define PSPECL	11
#endif	OSF1_SERVER
#define PRIBIO	20
#define PVFS	22
#define PRIUBA	24
#define PZERO	25
#define PPIPE	26
#define PWAIT	30
#define PLOCK	35
#define PSLEP	40
#define PUSER	50
#define PMASK	0177
#define PCATCH	0400	/* implies interruptible sleep */

#define PRIZERO	0	/* The kernel version of NZERO */

/*
 * Signals
 */
#if	!(defined(LOCORE) && defined(ibmrt))
#include <sys/signal.h>
#endif	!(defined(LOCORE) && defined(ibmrt))

#ifdef	_KERNEL
/*
 * Constants passed to mpsleep, the MP-safe sleep function.
 */
#define MS_LOCK_SIMPLE		0x0001		/* simple lock */
#define MS_LOCK_READ		0x0002		/* read lock */
#define MS_LOCK_WRITE		0x0004		/* write lock */
#define MS_LOCK_ON_ERROR	0x1000		/* lock on error return */

/*
 * Non-assertion macros for sleep and tsleep.  MP-safe versions.
 */
#if	!MACH_ASSERT
/*
 * We make a special check for lbolt (the once-per-second timer)
 * to avoid keeping a separate lbolt thread or overloading the
 * timeout queue:
 */
extern time_t	lbolt;
extern int	hz;

#define sleep(chan, pri) \
	(void) mpsleep(chan, pri, "Zzzzzz", \
			(chan == (caddr_t)&lbolt) ? hz : 0, (void *)NULL, 0)

#define tsleep(chan, pri, wmesg, timo) \
	mpsleep(chan, pri, wmesg, timo, (void *) NULL, 0)
#endif

/*
 *	Check for per-process and per thread signals.
 *	Must be MP-safe.
 */
#define SHOULDissig(p,uthreadp) \
	(((p)->p_sig | (uthreadp)->uu_sig) && ((p)->p_flag&STRC || \
	 (((p)->p_sig | (uthreadp)->uu_sig) &~ \
	   ((p)->p_sigignore | (p)->p_sigmask))))

#define thread_should_halt(thread) \
	((thread)->uu_procp && ((thread)->uu_procp->p_flag & SWEXIT) && \
	 ((thread)->uu_proc_exit == FALSE))

/*
 *	Check for signals, handling possible stop signals.
 *	Ignores signals already 'taken' and per-thread signals.
 *	Use before and after thread_block() in sleep().
 *	(p) is always current process.
 */
#define ISSIG(p) (thread_should_halt(current_thread()) || \
	 (SHOULDissig(p,current_thread()) && issig()))

/*
 *	Check for signals, including signals already taken and
 *	per-thread signals.  Use in trap() and syscall() before
 *	exiting kernel.
 *	Must be MP-safe.
 */
#define CHECK_SIGNALS(p, thread, uthreadp)	\
	(!thread_should_halt(thread)		\
	 && ((p)->p_cursig		\
	     || SHOULDissig(p,uthreadp)))

#endif	/* _KERNEL */

/*
 * Machine type dependent parameters.
 */

#define NBPW	sizeof(int)	/* number of bytes in an integer */

#ifndef NULL
#define	NULL	0
#endif	/* NULL */
#define CMASK	022		/* default mask for file creation */
#define NODEV	(dev_t)(-1)
#ifdef OSF1_ADFS
#define NONODE  (node_t)(-1)
#endif

/*
 * Clustering of hardware pages on machines with ridiculously small
 * page sizes is done here.  The paging subsystem deals with units of
 * CLSIZE pte's describing NBPG (from machine/machparam.h) pages each.
 *
 * NOTE: SSIZE, SINCR and UPAGES must be multiples of CLSIZE
 */
#define CLBYTES		(CLSIZE*NBPG)
#define CLOFSET		(CLSIZE*NBPG-1)	/* for clusters, like PGOFSET */
#define claligned(x)	((((int)(x))&CLOFSET)==0)
#define CLOFF		CLOFSET
#define CLSHIFT		(PGSHIFT+CLSIZELOG2)

#if	CLSIZE==1
#define clbase(i)	(i)
#define clrnd(i)	(i)
#else
/* give the base virtual address (first of CLSIZE) */
#define clbase(i)	((i) &~ (CLSIZE-1))
/* round a number of clicks up to a whole cluster */
#define clrnd(i)	(((i) + (CLSIZE-1)) &~ (CLSIZE-1))
#endif

/* CBLOCK is the size of a clist block, must be power of 2 */
#define CBLOCK	64
#define CBQSIZE	(CBLOCK/NBBY)	/* quote bytes/cblock - can do better */
#define	CBSIZE	(CBLOCK - sizeof(struct cblock *) - CBQSIZE) /* data chars/clist */
#define CROUND	(CBLOCK - 1)				/* clist rounding */

/* System V IPC definitions */

/* messages */
#define MSGMAX   8192
#define MSGMNB   16384
#define MSGMNI   100
#define MSGTQL   40

/* semaphores */
#define SEMMNI   100
#define SEMMNS   60
#define SEMMSL   100
#define SEMOPM   10
#define SEMUME   100
#define SEMVMX   32767
#define SEMAEM   16384

/*shared memory */
#define SHMMIN	 1
#define SHMMAX	 (1024*1024*1024)
#define SHMMNI	 1000
#define SHMSEG	 20

/*
 * In the past the buffer cache allowed buffers up to MAXBSIZE in
 * length.  The size of buffers is now a run-time constant.  MAXBSIZE,
 * the limit on file system block size is now really a UFS value.
 * MAXBSIZE remains here to avoid disruption in the AD1 code base.
 * MIN_BCBSIZE is the smallest valid value for the buffer cache block
 * size.
 */
#define	MAXBSIZE	524288
#define	MIN_BCBSIZE	8192	/* Minimum buffer cache size. */
#define DFLT_BCBSIZE	65536	/* Default buffer cache size. */

/* 
 * MIN_IPI3_BCBSIZE is a default value for nodes with ipi-3 devices
 * attatched.
 */
#define MIN_IPI3_BCBSIZE	65536

/*
 * BLKDEV_IOSIZE is a default value of the transfer size for block
 * devices.  When we can read the label, we use the file system
 * logical block size.  If not, the maximum of the mach-record
 * size for the device and BLKDEV_IOSIZE is used.
 */
#define	BLKDEV_IOSIZE	8192

/*
 * Disk granules ar a common unit of disk addressing within the Unix
 * server.  They are used to specify disk addresses to the server's
 * drivers and to represent physical disk address in the buffer cache.
 * In order for every disk sector to be specifiable in terms of disk
 * granules, the DISK_GRANULE size must be no larger than the smallest
 * sector size and smallest mach-record size of block device.  In the
 * past, there was a constant, DEV_BSIZE, defined here.  It no longer
 * exists.
 */
#ifndef	i386
#define DISK_GRANULE	2048	/* DFLT_512 */
#define DISK_GSHIFT	11	/* DFLT_512 */
#else
#define DISK_GRANULE	512	/* default for 386 (test) systems */
#define DISK_GSHIFT	9	/* default for 386 (test) systems */
#endif
#define btodg(bytes)	((unsigned)(bytes) >> DISK_GSHIFT)
#define dgtob(gran)	((unsigned)(gran) << DISK_GSHIFT)

/*
 * TEMPORARY KLUDGE: Until AD rev-2, when we can build commands more
 * easily, we will leave the old DEV_BSIZE stuff in user-level code.
 */
#ifndef _KERNEL
#define DEV_BSIZE	DISK_GRANULE
#define DEV_BSHIFT	DISK_GSHIFT
#define btodb(x)	btodg(x)
#define dbtob(x)	dgtob(x)
#endif /* _KERNEL */

/*******
*
*  UBSIZE is the value that commands and libs will use to present 
*  file/blocks to the user. It is now set to 1k blocksize.
*
*******/

#define UBSIZE	1024
#define UBSHIFT 10	/* LOG2(UBSIZE) */

/*
 * MAXPATHLEN defines the longest permissable path length
 * after expanding symbolic links. It is used to allocate
 * a temporary buffer from the buffer pool in which to do the
 * name expansion, hence should be a power of two, and must
 * be less than or equal to MAXBSIZE.
 * MAXSYMLINKS defines the maximum number of symbolic links
 * that may be expanded in a path name. It should be set high
 * enough to allow all legitimate uses, but halt infinite loops
 * reasonably quickly.
 */
#define	MAXPATHLEN	PATH_MAX
#define MAXSYMLINKS	32

/*
 * Constants for setting the parameters of the kernel memory allocator.
 *
 * 2 ** MINBUCKET is the smallest unit of memory that will be
 * allocated. It must be at least large enough to hold a pointer.
 *
 * Units of memory less or equal to MAXALLOCSAVE will permanently
 * allocate physical memory; requests for these size pieces of
 * memory are quite fast. Allocations greater than MAXALLOCSAVE must
 * always allocate and free physical memory; requests for these
 * size allocations should be done infrequently as they will be slow.
 * Constraints: CLBYTES <= MAXALLOCSAVE <= 2 ** (MINBUCKET + 14)
 * and MAXALLOCSIZE must be a power of two.
 */
#define MINBUCKET	4		/* 4 => min allocation of 16 bytes */
#define MAXALLOCSAVE	(2 * CLBYTES)

/*
 * bit map related macros
 */

#define setbit(a,i)	(*(((char *)(a)) + ((i)/NBBY)) |= 1<<((i)%NBBY))
#define clrbit(a,i)	(*(((char *)(a)) + ((i)/NBBY)) &= ~(1<<((i)%NBBY)))
#define isset(a,i)	(*(((char *)(a)) + ((i)/NBBY)) & (1<<((i)%NBBY)))
#define isclr(a,i)      ((*(((char *)(a)) + ((i)/NBBY)) & (1<<((i)%NBBY))) == 0)

#if	!defined(vax) && !defined(i386)
#define _bit_set(i,a)   setbit(a,i)
#define _bit_clear(i,a)	clrbit(a,i)
#define _bit_tst(i,a)	isset(a,i)
#endif	/* !defined(vax) && !defined(i386) */


/*
 * Macros for fast min/max.
 */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/*
 * Macros for counting and rounding.
 */
#ifndef	howmany
#define howmany(x, y)	(((x)+((y)-1))/(y))
#endif
#define roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#define powerof2(x)	((((x)-1)&(x))==0)

/*
 * Scale factor for scaled integers used to count %cpu time and load avgs.
 *
 * The number of CPU `tick's that map to a unique `%age' can be expressed
 * by the formula (1 / (2 ^ (FSHIFT - 11))).  The maximum load average that
 * can be calculated (assuming 32 bits) can be closely approximated using
 * the formula (2 ^ (2 * (16 - FSHIFT))) for (FSHIFT < 15).
 *
 * For the scheduler to maintain a 1:1 mapping of CPU `tick' to `%age',
 * FSHIFT must be at least 11; this gives us a maximum load avg of ~1024.
 */
#define	FSHIFT	11		/* bits to right of fixed binary point */
#define FSCALE	(1<<FSHIFT)

#define MAXDOMNAMELEN	256		/* maximum domain name length */

#endif	/* _SYS_PARAM_H_ */
