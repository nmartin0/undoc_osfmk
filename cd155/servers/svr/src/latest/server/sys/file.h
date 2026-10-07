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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
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
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * HISTORY
 * $Log: file.h,v $
 * Revision 1.13  1995/02/01  21:36:09  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/11/18  20:40:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/04/18  18:32:02  shala
 *  Reviewer: Charlie Johnson, David Leslie
 *  Risk: Low
 *  Benefit or PTS #: Bug # 8757
 *  Testing: Built server as well cmds/libs, tools and tested test case.
 *  Module(s): svr/server/sys/file.h
 *
 * Needed to include mach/port.h and mach/memory_object.h to file.h to
 * resolved undefined types. Because we have made OSF1_ADFS to be defined by
 * default in R4.5 compilers, there are few types which now need to be defined
 * properly.
 *
 * Revision 1.10  1993/07/29  21:53:19  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.9  1993/07/14  18:24:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.8  1993/07/09  20:22:25  brad
 * Added #define PFS for compiling applications.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/07  19:15:55  nandy
 * Fixed the merge conflict.
 *
 * Revision 1.6  1993/05/06  19:19:06  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:07:32  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1993/02/16  21:06:11  cfj
 * Merge sync close into main stem.
 *
 * Revision 1.3.6.1  1993/02/16  20:38:23  cfj
 * Synchronous close from OSF.
 *
 * Revision 2.9  93/01/11  14:37:26  mmp
 * 	Synchronous close support: add f_makesend and f_relsend to keep track
 * 	of outstanding send rights on the file port; add FP_UNREF_NUM to
 * 	atomically release num references (used by file_port_no_senders).  (mmp)
 * 
 * Revision 1.1.2.1.2.2  1992/12/16  22:56:54  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:01:01  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:59:54  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:41:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.9  92/11/23  10:35:51  klh
 * Keep f_seqno for OSF1_ADFS w/o TNC as well---fixes file port nms race.
 * For TNC, keep f_seqno_mtx pointer to shared mutex used by FIFO relocation.
 * (klh for mjl)
 * 
 * Revision 2.14  93/07/27  11:25:24  mjl
 * [Bug #0308] Change file port seqno mutex to a r/w lock.
 * 
 * Revision 2.13  93/06/29  15:35:28  bhk
 * Partial update to OSF/1 AD V1.05b1 to fix select problems
 * 
 * Revision 2.12  93/06/16  13:51:06  klh
 * 	Revision 2.12  93/05/26  14:42:45  loverso
 * 		Fix macro definitions. (loverso)
 * 
 * Revision 2.11  93/04/29  14:01:08  klh
 * 	Revision 2.11  93/03/10  10:47:34  mmp
 * 		Define ADFS version of FP_UNREF and FP_UNREF_NUM that check for
 * 		FUNREFWAIT flag and do a thread_wakeup if it's set (for close).
 *
 * 	Revision 2.10  93/02/02  15:23:25  rabii
 * 		Added pointer for linked list of all allocated file 
 *		structures (rabii)
 * 
 * 	Revision 2.9  93/01/11  14:37:26  mmp
 *              Synchronous close support: add f_makesend and
 *		f_relsend to keep track of outstanding send rights
 *		on the file port; add FP_UNREF_NUM to atomically
 *		release num references (used by file_port_no_senders).
 *		(mmp)
 * 
 * Revision 2.10  93/03/10  14:16:53  yazz
 * Synchronous close merge from Intel.
 * 
 * Revision 2.9  92/11/23  10:35:51  klh
 * Keep f_seqno for OSF1_ADFS w/o TNC as well---fixes file port nms race.
 * For TNC, keep f_seqno_mtx pointer to shared mutex used by FIFO relocation.
 * (klh for mjl)
 *
 * 	Revision 1.3.6.1  1993/02/16  20:38:23  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.9  93/01/11  14:37:26  mmp
 * 	 Synchronous close support: add f_makesend and f_relsend to keep track
 * 	 of outstanding send rights on the file port; add FP_UNREF_NUM to
 * 	 atomically release num references (used by file_port_no_senders). (mmp)
 * 
 * 	Revision 1.3  1992/12/11  02:59:54  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.2  1992/11/30  22:41:59  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.1  1992/11/05  22:39:37  dleslie
 * 	Local changes for NX through noon, November 5, 1992.
 *
 * 	Revision 2.9  92/11/23  10:35:51  klh
 * 	Keep f_seqno for OSF1_ADFS w/o TNC as well---fixes file port nms race.
 * 	For TNC, keep f_seqno_mtx pointer to shared mutex used by FIFO relocation.
 * 	(klh for mjl)
 * 
 * Revision 2.8  92/08/17  18:09:07  mjl
 * Get rid of unused TNC flag field.
 * 
 * Revision 2.7  92/08/17  12:55:06  mjl
 * Added chain field for queue of file structs used by FIFO relocation.
 * XXX f_tncflag field and values are bogus and should be nuked.
 * 
 * Revision 2.6  92/08/08  01:39:59  jdh
 * 
 * file port sequence number is now in the file structure, not the socket
 * structure -- jdh
 * 
 * Revision 2.5  92/06/11  16:08:54  mjl
 * New magic number F_RELOC denotes file struct whose port has migrated away.
 * 
 * Revision 2.4  92/01/05  20:06:59  roy
 * 	92/01/02  17:26:14  roy
 * 	f_seqno not needed in file structure.
 * 
 * 	1991/10/14  19:27:19  noemi
 * 	Added f_magic and f_seqno fields to the file structure.  Also defined
 * 	F_MAGIC
 * 
 * Revision 2.3  91/12/13  10:13:53  roy
 * 	91/10/29  17:59:10  roy
 * 	Add cookie as first field of file structure.
 * 	Added mem_obj to file structure in support of mapped files.
 * 
 * Revision 2.2  91/08/31  14:05:03  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/07/31  15:42:47  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:50:17  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:39:04  gm]
 * 
 * Revision 1.8  90/08/24  12:26:12  devrcs
 * 	Changes for u_file_state.
 * 	Replace GETF macro with function getf().
 * 	[90/08/17  17:48:17  nags]
 * 
 * Revision 1.7  90/06/22  20:52:55  devrcs
 * 	nags merge
 * 
 * 	Ancient history (reverse chronology):
 * 	Recombine GETF macros.				nags@encore.com
 * 	Secureware changes.				seiden@osf.org
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Revamped ifdef KERNEL to ifdef _KERNEL.		pam@osf.org
 * 	     General clean-up of kernel conditionals.	tom@osf.org
 * 	KERNEL ==> KERNEL || KERNEL_FILE || _KERNEL.	mbrown@osf.org
 * 	     AIX merge first cut.				tom@osf.org
 * 	File layer parallelization changes.		nags@osf.org
 * 	Added defs for POSIX record/file locking.	ers@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Merged 4.4BSD and Mach changes.			noemi@osf.org
 * 	Fix macros.					alan&shashi@encore.com
 * 	Merged prelim 2.5, Encore parallelization.	alan@encore.com
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Defined file table, structure locks.		alan@encore.com
 * 	Make definitions visible to user programs.	mrt@cmu.edu
 * 	Added FOKDIR flag.				gm0w@cmu.edu
 * 	Defined f_rmt_fileid, f_frced in struct file.	mrt@cmu.edu
 * 	Added vnode support.				jsb@cmu.edu
 * 	Added FNOSPC.					mja@cmu.edu
 * 	Added VICE support.				jjk@cmu.edu
 * 	Turn on KERNEL_FILE to get all definitions.	mja@cmu.edu
 * 	Upgraded to 4.3.				avie@cmu.edu
 * 	KERNEL_FILE makes struct file visible to users.	mja@cmu.edu
 * 	CS_RFS:  Added DTYPE_RFSINO definition.		mja@cmu.edu
 * 	[90/06/12  21:38:21  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)file.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_FILE_H_
#define _SYS_FILE_H_

#ifndef	_KERNEL
#ifndef	PFS
#define	PFS
#endif
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/access.h>
#include <sys/fcntl.h>

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#include <kern/lock.h>
#include <kern/assert.h>
#endif
#ifdef PFS
#include <sys/estat.h>
#endif

#ifndef KERNEL
#include <mach/port.h>
#include <mach/memory_object.h>
#endif

#ifdef	OSF1_ADFS
/*
 * Magic number for file structures.
 */
#define	F_MAGIC	0x050287

#ifdef	TNC
#define F_RELOC 0x590823
#include <kern/queue.h>
#endif

#endif	/* OSF1_ADFS */

#if	defined(_KERNEL) || defined(KERNEL_FILE)
/*
 * One file structure is allocated for each open/creat/pipe call.
 * Main use is to hold the read/write pointer associated with
 * each open file.
 */
struct	file {
#ifdef	OSF1_ADFS
	long		    f_magic;	/* magic # indicates attached port */
	mach_port_seqno_t   f_seqno;	/* no-more-senders synchronization */
	int		    f_makesend;	/* number of send rights made */
	int		    f_relsend;	/* number of send rights released */
	int		    f_svrsend;	/* number of send rights in server */
	memory_object_t	    mem_obj;	/* used for mapping by server */ 
	                                /*   MAPPED_FILES */
#endif
	int	f_flag;			/* see below */
	short	f_type;			/* descriptor type */
	cnt_t	f_count;		/* reference count */
	short	f_msgcount;		/* references from message queue */
	struct	file *f_filelist;	/* linked list of all allocated 
					   file structures */
	struct	ucred *f_cred;		/* descriptor's credentials */
	struct	fileops {
		int	(*fo_read)();
		int	(*fo_write)();
		int	(*fo_ioctl)();
		int	(*fo_select)();
		int	(*fo_close)();
	} *f_ops;
	caddr_t	f_data;			/* vnode, socket, etc. */
	union {				/* offset or next free file struct */
		off_t		fu_offset;
		struct file	*fu_freef;
	} f_u;
#ifdef	TNC
	queue_chain_t	f_chain;	/* vnode migration file struct chain */
	lock_t		f_seqno_lock;	/* ptr to mutex for f_seqno update */
#endif
#ifdef	_KERNEL
	lock_data_t f_io_lock;		/* see below about locks */
	udecl_funnel_data(,f_funnel)	/* uniprocessor code compatibility */
	udecl_simple_lock_data(,f_incore_lock)
#endif
#ifdef	PFS
	esize_t	pfs_offset;
#endif
};
#define	f_offset	f_u.fu_offset
#define	f_freef		f_u.fu_freef

struct file *flisthead;			/* pointer to head of linked list
					   of file structures */
extern struct	file *file, *fileNFILE;
extern int	nfile;

#define	DTYPE_FREE	0	/* unused file structure */
#define DTYPE_VNODE	1	/* file */
#define DTYPE_SOCKET	2	/* communications endpoint */
#define	DTYPE_RESERVED	3	/* open/close in progress XXX */
#endif	/* defined(_KERNEL) || defined(KERNEL_FILE) */

/*
 * Lseek call.
 */
#define L_SET		0	/* absolute offset */
#define L_INCR		1	/* relative to current offset */
#define L_XTND		2	/* relative to end of file */

#ifdef	_KERNEL
#include <kern/macro_help.h>

/*
 * File structure locking constraints.
 *
 *	Field			Comment
 *	-----			-------
 *	f_flag			incore lock (but FREAD/FWRITE are read-only)
 *	f_type			read-only
 *	f_count			incore lock
 *	f_msgcount		incore lock
 *	f_cred			read-only
 *	f_ops			read-only
 *	f_data			read-only
 *	f_offset		i/o lock, incore lock
 *	f_funnel		guard uniprocessor subsystems
 *
 * A read-only field is one set during the allocation process; no other
 * processor or thread within the same task knows about the file structure
 * yet so the values in the field don't matter while the structure is
 * being allocated.  After allocation and initialization, the value in
 * such a field never changes until the structure is de-allocated.
 *
 * The f_io_lock serializes I/O through a shared file structure.  Programs
 * that naively expect to fork and read or write streams will receive non-
 * overlapping, unduplicated input and generate non-overlapping, unduplicated
 * output (although possibly interleaved between "atomic" i/o calls).  The
 * f_io_lock accomplishes this purpose by preventing f_offset from being
 * changed between the time the I/O request is initiated and the time
 * it completes.  Although the f_io_lock is provided at the file layer, the
 * lock is used at the discretion of the lower-level fileops.  In the vnode
 * layer, this lock is used to serialize access through plain files and
 * directories but not through character-special files, symbolic links
 * or sockets.  In the latter two cases, f_offset has no meaning.  We do
 * not guarantee I/O on character-special files because device drivers
 * may have additional constraints beyond the knowledge of the file layer.
 * For example, the terminal driver must be permitted to do the job of
 * sorting out competing claims on a terminal:  if we used the f_io_lock,
 * processes sharing file structures would not be able to do output while
 * one of their number has a read outstanding on the terminal.
 *
 * The f_io_lock has one other important use:  it serializes F_SETFL
 * operations on the same file structure.  The ordinary f_incore_lock
 * does not suffice because the flags word is modified in one routine
 * but its value must be guaranteed to remain constant until it is used
 * in another.
 *
 * Finally, the file structure credentials pointer never changes during
 * the working lifetime of a file structure.  Because routines that use
 * file structures must increase their reference counts, there is no way
 * for the credentials to be deallocated while a system call is in progress.
 * For this reason, it is possible to pass the file structure's credentials
 * pointer to lower-level routines without incrementing the credentials'
 * reference count.
 */

/*
 * Compatibility with uniprocessor subsystems.
 */
#define	FILE_FUNNEL(f)		FUNNEL(f)
#define	FILE_UNFUNNEL(f)	UNFUNNEL(f)
#if	UNI_COMPAT
#define	file_default_lock	default_uni_lock
#endif

/*
 * The FP_LOCK macros are used to prevent multiple processors from
 * accessing fields in a file structure simultaneously.  The FP_IO_LOCK
 * serializes I/O through shared file structures.
 */
#define	FP_LOCK(fp)		usimple_lock(&(fp)->f_incore_lock)
#define	FP_UNLOCK(fp)		usimple_unlock(&(fp)->f_incore_lock)
#define	FP_LOCK_INIT(fp)	usimple_lock_init(&(fp)->f_incore_lock)
#define	FP_LOCK_HOLDER(fp)	SLOCK_HOLDER(&(fp)->f_incore_lock)

#define	FP_IO_LOCK(fp)		lock_write(&(fp)->f_io_lock)
#define	FP_IO_UNLOCK(fp)	lock_write_done(&(fp)->f_io_lock)
#define	FP_IO_LOCK_INIT(fp)	lock_init2(&(fp)->f_io_lock,TRUE,LTYPE_FILE_IO)
#define	FP_IO_LOCK_HOLDER(fp)	LOCK_HOLDER(&(fp)->f_io_lock)

/*
 * Count uses of a file structure by the kernel itself as well as
 * by file descriptors.  FP_REF never sleeps.  FP_UNREF may sleep
 * when the count falls to zero and closef is called.
 */
#define	FP_REF(fp)							\
MACRO_BEGIN								\
	FP_LOCK(fp);							\
	ASSERT((fp)->f_count > 0);					\
	(fp)->f_count++;						\
	FP_UNLOCK(fp);							\
MACRO_END

#ifdef	OSF1_ADFS

#define	FP_UNREF(fp)							\
MACRO_BEGIN								\
	FP_LOCK(fp);							\
	ASSERT((fp)->f_count > 0);					\
	if ((fp)->f_count > 1) {					\
		int needwakeup = 0;					\
		if ((fp)->f_flag & FUNREFWAIT) {			\
			(fp)->f_flag &= ~FUNREFWAIT;			\
			needwakeup++;					\
		}							\
		(fp)->f_count--;					\
		FP_UNLOCK(fp);						\
		if (needwakeup)						\
			thread_wakeup((int)&(fp)->f_count);		\
	} else {							\
		FP_UNLOCK(fp);						\
		(void) closef(fp);					\
	}								\
MACRO_END

#define	FP_UNREF_NUM(fp, num)						\
MACRO_BEGIN								\
	FP_LOCK(fp);							\
	ASSERT((fp)->f_count >= (num));					\
	if ((fp)->f_count > (num)) {					\
		int needwakeup = 0;					\
		if ((fp)->f_flag & FUNREFWAIT) {			\
			(fp)->f_flag &= ~FUNREFWAIT;			\
			needwakeup++;					\
		}							\
		(fp)->f_count -= (num);					\
		FP_UNLOCK(fp);						\
		if (needwakeup)						\
			thread_wakeup((int)&(fp)->f_count);		\
	} else {							\
		(fp)->f_count = 1;					\
		FP_UNLOCK(fp);						\
		(void) closef(fp);					\
	}								\
MACRO_END

#else	/* !OSF1_ADFS */

#define	FP_UNREF(fp)							\
MACRO_BEGIN								\
	FP_LOCK(fp);							\
	ASSERT((fp)->f_count > 0);					\
	if ((fp)->f_count > 1) {					\
		(fp)->f_count--;					\
		FP_UNLOCK(fp);						\
	} else {							\
		FP_UNLOCK(fp);						\
		(void) closef(fp);					\
	}								\
MACRO_END

#endif	/* OSF1_ADFS */


/*
 * File descriptor constraints.
 *
 * In Mach, multiple threads within the same task share a file
 * descriptor table, permitting races not found in the traditional
 * BSD model of one process + one file descriptor table.
 *
 * File descriptor allocation is broken into two steps:  (1) reserving
 * a file descriptor and (2) after the open succeeds, resetting the
 * reserved file descriptor to point to the new file structure.  The
 * U_FD_SET macro performs the reset.  If the open fails, U_FD_SET(fd,NULL)
 * frees up the reserved file descriptor.
 *
 * getf translates a file descriptor to a file structure and increments
 * the file structure's reference count.  The reference count is used
 * to prevent the file structure from being deallocated while the kernel
 * uses it.
 *
 * The file descriptor table lock may be held while acquiring a file
 * structure's f_incore_lock.
 */

#define	U_FDTABLE_LOCK(ufp)	usimple_lock(&(ufp)->uf_ofile_lock)
#define	U_FDTABLE_UNLOCK(ufp)	usimple_unlock(&(ufp)->uf_ofile_lock)
#define	U_FDTABLE_LOCK_INIT(ufp)	usimple_lock_init(&(ufp)->uf_ofile_lock)

#define	U_FD_RESERVED		((struct file *) -1)

#define	FILE_FLAGS_NULL		((char *) 0)

extern int getf();

#define	U_FD_SET(fd, fp, ufp)						\
MACRO_BEGIN								\
	U_FDTABLE_LOCK(ufp);						\
	ASSERT((ufp)->uf_ofile[(fd)] == U_FD_RESERVED);			\
	(ufp)->uf_ofile[(fd)] = (fp);					\
	U_FDTABLE_UNLOCK(ufp);						\
MACRO_END

/*
 * The FOP macros simplify calling through the file structure operations
 * table by taking into account a number of important details, such as
 * possible backwards compatibility with unparallelized subsystems.
 *
 * It is not legal to hold any simple locks across an FOP call.
 */

#define	FOP_READ(fp,uio,cred,ret_val)					\
MACRO_BEGIN								\
	FILE_FUNNEL((fp)->f_funnel);					\
	(ret_val) = (*(fp)->f_ops->fo_read)((fp),(uio),(cred));		\
	FILE_UNFUNNEL((fp)->f_funnel);					\
MACRO_END

#define	FOP_WRITE(fp,uio,cred,ret_val)					\
MACRO_BEGIN								\
	FILE_FUNNEL((fp)->f_funnel);					\
	(ret_val) = (*(fp)->f_ops->fo_write)((fp),(uio),(cred));	\
	FILE_UNFUNNEL((fp)->f_funnel);					\
MACRO_END

#define	FOP_IOCTL(fp,cmd,value,ret_val)					\
MACRO_BEGIN								\
	FILE_FUNNEL((fp)->f_funnel);					\
	(ret_val) = (*(fp)->f_ops->fo_ioctl)((fp),(cmd),(value));	\
	FILE_UNFUNNEL((fp)->f_funnel);					\
MACRO_END

#define	FOP_SELECT(fp,ev,rev,scanning,ret)				\
MACRO_BEGIN								\
	FILE_FUNNEL((fp)->f_funnel);					\
	(ret) = (*(fp)->f_ops->fo_select)((fp),(ev),(rev),(scanning));	\
	FILE_UNFUNNEL((fp)->f_funnel);					\
MACRO_END

#define	FOP_CLOSE(fp,ret_val)						\
MACRO_BEGIN								\
	FILE_FUNNEL((fp)->f_funnel);					\
	(ret_val) = (*(fp)->f_ops->fo_close)(fp);			\
	FILE_UNFUNNEL((fp)->f_funnel);					\
MACRO_END

#endif	/* _KERNEL */

#endif	/* _SYS_FILE_H_ */
