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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */

/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992, 1993 Intel Corporation.
 */

/*
 * HISTORY
 * $Log: fdt.h,v $
 * Revision 1.11  1995/02/01  21:22:31  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:23:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/12/21  18:59:17  rlg
 * Merged the R1.8.4.1 update into the main trunk
 *
 * Revision 1.8.4.1  1993/12/15  23:23:47  rlg
 * Added copyright comment to head of file
 *
 *  Reviewer:  none
 *  Risk:      low
 *  Benefit or PTS #:  none
 *  Testing:  tested with build that tested changes for PTS # 6486
 *  Module(s):
 *
 * Revision 1.8  1993/07/14  17:31:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:23:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/06/16  20:30:39  dbm
 * Added pfs_iomode_info to fdte structure to allow PFS I/O mode information
 * to be correct for single node applications.
 *
 * Revision 1.6  1993/05/06  20:14:57  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.15  1993/04/29  13:54:50  klh
 * 	Revision 2.15  93/03/30  16:07:56  roy
 * 		Remove fast_path_io conditional.
 * 		[93/03/29            roy]
 *
 * 	Revision 2.14  93/03/10  10:44:41  mmp
 * 		change declaration of fdt_unref_entry from void to int.
 *
 * 	Revision 2.13  93/01/12  17:05:39  roy
 * 		Removed mappable and fpio_mode fields from fdte in favor of a 
 *		new iomode field.
 * 		[93/01/11            roy]
 *
 * Revision 2.14  93/03/10  14:15:22  yazz
 * Synchronous close merge from Intel.
 * 
 * 	Revision 1.3.6.1  1993/02/16  20:39:20  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.12  93/01/11  14:34:23  mmp
 * 		Add cleanlocks bit to fdt (mmp)
 * 
 * 	Revision 1.3  1992/12/11  02:51:42  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.2  1992/11/30  22:08:39  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.1  1992/11/05  22:15:48  dleslie
 * 	cal modifications for NX through noon, November 5, 1992ZZ
 *
 * 	Revision 2.12  1992/10/22  14:56:45  dbm
 * 	Added PFS definitions.
 *
 * Revision 1.5  1993/04/03  03:17:48  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1993/02/16  21:08:17  cfj
 * Merge sync close into main stem.
 *
 * Revision 1.3.6.1  1993/02/16  20:39:20  cfj
 * Synchronous close from OSF.
 *
 * Revision 2.12  93/01/11  14:34:23  mmp
 * 	Add cleanlocks bit to fdt (mmp)
 * 
 * Revision 1.3  1992/12/11  02:51:42  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.7  1993/03/16  01:42:23  wunder
 * Added pointer to async queuing structure within fdt_entry structure
 *
 * Revision 1.1.2.1.2.6  1992/12/21  21:37:19  brad
 * Removed now redundant declaration of fdt_slot_t (all PFS fields
 * have been moved to the fdte).
 *
 * Revision 1.1.2.1.2.5  1992/12/16  23:11:01  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.4  1992/12/16  05:57:05  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.3  1992/12/11  21:00:40  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.1.2.1.2.2  1992/12/03  00:09:05  dbm
 * Updated for pfs i/o mode information in the fdte entry.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:00:52  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.2  1992/11/30  22:08:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:48  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.12  1992/10/22  14:56:45  dbm
 * Added PFS definitions.
 *
 * Revision 2.13  92/11/23  16:04:38  klh
 * 	Revision 2.10  92/11/05  17:19:26  roy
 * 		Add next ptr to fdt_entry_t.
 * 		[92/10/22            roy]
 * 
 * Revision 2.12  92/11/23  10:22:45  klh
 * Add notify_asap bit to fdt, used for TNC unix datagram socket relocation
 * (klh for mjl)
 * 
 * Revision 2.11  92/10/06  12:04:33  roman
 * Fix RCS comments.
 * 
 * Revision 2.10  92/10/05  13:42:07  klh
 * 	Revision 2.9  92/08/26  12:09:28  loverso
 * 		Add spin_try_lock() variants for FDT and fdte locks. (loverso)
 * 
 * 	Revision 2.8  92/07/29  08:32:27  rabii
 * 		Add more externs.  Fix RCS comments.
 * 		[92/07/27            roy]
 * 
 * Revision 2.9  92/07/30  15:57:58  chrisp
 * Add declarations of fdt_atomic_begin() and fdt_atomic_end().
 * 
 * Revision 2.8  92/06/10  10:09:34  klh
 * 	Revision 2.7  92/06/08  18:16:46  pjg
 * 		Added fast_path_io support under FAST_PATH_IO conditional.
 * 	  	Added 'accessed' flag to fdte for MAPPED_FILES.
 * 	[92/06/01            roy]
 * 
 * Revision 2.7  92/06/05  13:54:39  klh
 * 	Revision 2.6  92/05/18  12:25:14  roy
 * 		Revision 2.4.1.2  92/04/22  09:48:47  roy
 * 		New flags for mapped files.
 * 		[92/04/05            roy]
 * 
 * 		Revision 2.4.1.1  92/03/30  16:50:59  roy
 * 		Implemented fdte_io_lock and friends.  Changed lock macros
 * 		to take fdte pointers instead of lock pointers.        
 * 		[92/03/30            roy]
 * 
 * 	Revision 2.5  92/04/05  16:42:46  pjg
 * 		Added fdte_entry status bits for TNC migrate notification. 
 *		(chrisp)
 * 
 * Revision 2.6  92/03/28  10:44:20  chrisp
 * Added fdte_entry status bits for TNC migrate notification.
 * 
 * Revision 2.5  92/03/24  08:59:15  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.4  92/03/15  14:29:09  roy
 * 	92/03/03  11:44:57  roy
 * 	Change min_offset, max_offset to off_t's.
 * 
 * Revision 2.3  92/02/11  18:54:09  pjg
 * 	Change flag name "inserted_on_fork" to "referenced" for more general
 * 	applicability (roman@locus).
 * 
 * Revision 2.2  92/01/02  18:38:09  roy
 * 	91/12/20  10:15:38  roy
 * 	File descriptor table info moved here from emul.h.
 * 
 * $EndLog$
 */
#ifndef	_FDT_H_
#define	_FDT_H_

#ifdef	PFS
#include "pfs_fdt.h"
#include "emul_stack.h"
#include "async.h"
#endif

/*
 * Structures associated with the per-process open file table.
 * Each slot in the file descriptor table is of type fdt_slot_t.
 * One of its fields points to a structure of type fdt_entry_t.
 */
typedef struct fdt_entry {
	struct fdt_entry *next;			/* next ptr for free list */
	spin_lock_t	lock;			/* lock protecting entry */
	mach_port_t	fp;			/* port to open files */
	int		refcnt;			/* reference count */
	u_long		iomode;			/* io mode of vnode */
	                                        /* e.g., is it a mapped file? */
	int		referenced:1,     	/* used only during fork */
#if	MAPPED_FILES | PFS
			accessed:1,		/* file been accessed? */
                        modified:1,		/* has the file been modifed?*/
                        must_release:1,		/* must release the token? */
                        can_revoke:1,		/* can the token be revoked? */
#endif
#ifdef	TNC
 			notify_on_migrate:1,	/* tell file we migrated */
			notify_asap:1,		/* notify on _any_ syscall */
 			was_notified:1,		/* true => file was told */
#endif
			cleanlocks:1;		/* ever used file locks? */
#if	MAPPED_FILES | PFS
	spin_lock_t	io_lock;		/* atomicity lock */
	int		fmode;			/* access/status flags */
	mach_port_t	mem_obj;		/* memory object port */
	mach_port_t	token;			/* token port */
	int		flags;			/* indication of tokens held */
	off_t		offset;			/* file offset (seek ptr) */
	int		length;			/* file length */
	vm_offset_t	win_offset;		/* offset of window in file */
 	vm_address_t	win_addr;		/* start of mapped window */
	int		win_size;		/* size of mapped window */
	off_t		min_offset;		/* range for cleaning */
	off_t		max_offset;
	int		token_refcnt;		/* Number of users of token. */
#endif	MAPPED_FILES
	spin_lock_t	fpio_lock;		/* fpio: atomicity lock */
	off_t		fpio_offset;		/* fpio: file offset */
#ifdef	PFS
	pfs_fd_t	*pfs_fd;		/* pointer to PFS descriptor */
	int		pfs_iomode;		/* current PFS I/O mode value.*/
	pfs_iomode_t	*pfs_iomode_info;	/* Pointer to PFS I/O mode 
						 * information, this is used 
						 * when multiple nodes have 
						 * set a non M_UNIX I/O mode.
						 */
	async_fdte_queue *async_queue;		/* async scheduling struct */
#endif
} fdt_entry_t;

typedef struct fdt_slot {
	fdt_entry_t	*fdte;			/* pointer to fdt entry */
	int		cloexec:1;		/* close on exec? */
} fdt_slot_t;

/*
 * Values for file descriptor table pointers.
 */
#define FD_EMPTY	((fdt_entry_t *) NULL)
#define FD_RESERVED	((fdt_entry_t *) -1)

#if	MAPPED_FILES | PFS
/*
 * Minimum window size of mapped file access.
 */
#define MIN_WIN_SIZE	65536		/* 64k */

/*
 * Bits indicating type of token(s).
 */
#define TOK_OFFSET      0x01            /* these values are also known */
#define TOK_DATA_READ   0x02            /*   by the server! */
#define TOK_DATA_WRITE  0x04

#endif  /* MAPPED_FILES */

/*
 * Macros for manipulating the locks on the file descriptor table.
 * Note that spin_lock() will voluntarily give up the processor
 * each time through its loop.
 */
#define FDT_LOCK()		spin_lock(&fdt_lock)
#define FDT_TRY_LOCK()		spin_try_lock(&fdt_lock)
#define FDT_UNLOCK()		spin_unlock(&fdt_lock)
#define FDT_LOCK_INIT()		spin_lock_init(&fdt_lock)

#define fdte_lock(fdte)         spin_lock(&(fdte->lock))
#define fdte_try_lock(fdte)	spin_try_lock(&(fdte->lock))
#define fdte_unlock(fdte)       spin_unlock(&(fdte->lock))
#define fdte_lock_init(fdte)    spin_lock_init(&(fdte->lock))

#if MAPPED_FILES | PFS
#define fdte_io_lock(fdte)      spin_lock(&(fdte->io_lock))
#define fdte_io_unlock(fdte)    spin_unlock(&(fdte->io_lock))
#define fdte_io_lock_init(fdte) spin_lock_init(&(fdte->io_lock))

extern 	mach_port_t		revoke_port;
#endif  /* MAPPED_FILES */

#define fdte_fpio_lock(fdte)      spin_lock(&(fdte->fpio_lock))
#define fdte_fpio_unlock(fdte)    spin_unlock(&(fdte->fpio_lock))
#define fdte_fpio_lock_init(fdte) spin_lock_init(&(fdte->fpio_lock))

extern fdt_slot_t	fdt[];			/* file descriptor table */
extern int 		fdt_lastfile;		/* HWM of fdt */
extern spin_lock_t 	fdt_lock;		/* lock protecting fd table */

extern void 		fdt_init();
extern void 		fdt_init_child();
extern int 		fdt_ref_entry();
extern int		fdt_unref_entry();
extern void		fdt_atomic_begin();
extern void		fdt_atomic_end();
#endif	_FDT_H_
