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
 * HISTORY
 * $Log: flock.h,v $
 * Revision 1.4  1994/11/18  20:40:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:24:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:30  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:06:21  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:42:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/31  14:06:32  devrcs
 * 	added a status field in the struct flino for Xenix compatibility
 * 	[90/10/08  13:42:13  swallace]
 * 
 * Revision 1.4  90/10/07  14:50:26  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:39:21  gm]
 * 
 * Revision 1.3  90/06/22  20:52:59  devrcs
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Definitions for POSIX record/file locking.	ers@osf.org
 * 	[90/06/12  21:38:27  gmf]
 * 
 * $EndLog$
 */

#ifndef	_SYS_FLOCK_H
#define	_SYS_FLOCK_H

#ifdef	_KERNEL
#include <unix_locks.h>
#if	UNIX_LOCKS
#include <kern/lock.h>
#endif
#endif

/* file locking structure (connected to file table entry) */
#ifdef	_KERNEL
/*
 * Locking constraints on the filock structure.
 * 	Field			Comment
 *	-----			-------
 *	set			FICHAIN_LOCK (from flip field)
 *	stat			FICHAIN_LOCK
 *	prev			FICHAIN_LOCK
 *	next			FICHAIN_LOCK
 *	flip			read-only
 */
#endif
struct	filock	{
	struct	flock set;	/* contains type, start, and length */
	union	{
		int wakeflg;	/* for locks sleeping on this one */
		int blkpid;	/* pid of blocking lock
				 * (for sleeping locks only)
				 */
	}	stat;
	struct	filock *prev;
	struct	filock *next;
	struct	flino  *flip;	/* back pointer to chain we are on */
};

/* table to associate files with chain of locks */
#ifdef	_KERNEL
/*
 * Locking constraints for the flino structure.
 *	Field			Comment
 *	-----			-------
 *	vp			read-only
 *	fl_refcnt		flino_lock
 *	fl_flck			fi_chain_lock
 *	prev			FIDS_LOCK (global lock)
 *	next			FIDS_LOCK (global lock)
 *
 * The FIDS_LOCK cannot be taken while holding the flino_lock
 */
#endif
struct	flino {
	struct vnode *vp;	 /* vnode address for the file */
	int	fl_refcnt;	 /* # procs currently referencing this flino */
	struct	filock *fl_flck; /* pointer to chain of locks for this file */
	struct	flino  *prev;
	struct	flino  *next;
#ifdef i386
	unsigned short status;	/* for Xenix compatibility */
#endif
	udecl_simple_lock_data(, flino_lock)
#if	defined(_KERNEL) && UNIX_LOCKS
	lock_data_t	fichain_lock;	/* lock to protect fl_flck chain */
#endif
};

/* file and record locking configuration structure */
/* record and file use totals may overflow */
#ifdef	_KERNEL
/*
 * Locking constraints for the flockinfo structure.
 *	Field		Comment
 *	-----		-------
 *	recs		read-only
 *	fils		read-only
 *	reccnt		flckinfo_lock
 *	filcnt		flckinfo_lock
 *	rectot		flckinfo_lock
 *	filtot		flckinfo_lock
 */
#endif
struct flckinfo {
	long recs;	/* number of records configured on system */
	long fils;	/* number of file headers configured on system */
	long reccnt;	/* number of records currently in use */
	long filcnt;	/* number of file headers currently in use */
	long rectot;	/* number of records used since system boot */
	long filtot;	/* number of file headers used since system boot */
#ifdef	_KERNEL
	udecl_simple_lock_data(, flckinfo_lock)
#endif
};

extern struct flckinfo	flckinfo;

#ifdef	_KERNEL

#define	FLCKINFO_LOCK(f)	usimple_lock(&(f)->flckinfo_lock)
#define	FLCKINFO_UNLOCK(f)	usimple_unlock(&(f)->flckinfo_lock)
#define	FLCKINFO_LOCKINIT(f)	usimple_lock_init(&(f)->flckinfo_lock)

#define	FLINO_LOCK(f)		usimple_lock(&(f)->flino_lock)
#define	FLINO_UNLOCK(f)		usimple_unlock(&(f)->flino_lock)
#define	FLINO_LOCKINIT(f)	usimple_lock_init(&(f)->flino_lock)

#define	FICHAIN_LOCKINIT(l)	ulock_init(&(l)->fichain_lock,TRUE,LTYPE_FICHAIN)
#define	FICHAIN_LOCK(l)		ulock_write(&(l)->fichain_lock)
#define	FICHAIN_UNLOCK(l)	ulock_done(&(l)->fichain_lock)
#define	FICHAIN_LOCK_HOLDER(p)	ULOCK_HOLDER(&(p)->fichain_lock)

#endif	/* _KERNEL */
#endif	/* _SYS_FLOCK_H */
