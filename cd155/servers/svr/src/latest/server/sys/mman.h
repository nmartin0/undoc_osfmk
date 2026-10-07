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
 * HISTORY
 * $Log: mman.h,v $
 * Revision 1.4  1994/11/18  20:40:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:25:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:49  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:45  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:14  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/15  11:43:15  roy
 * Fix revision history comments.
 *
 * Revision 2.2  91/10/14  13:08:12  sjs
 *	Initial revision for this tree.
 * 
 * Revision 3.0  91/09/13  12:47:08  sp
 * included for mmap
 * 
 * Revision 1.8  90/10/07  14:52:11  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:11  gm]
 * 
 * Revision 1.7  90/09/23  15:59:58  devrcs
 * 	Fixed conformance bug #376
 * 	[90/09/10  10:50:46  collins]
 * 
 * Revision 1.6  90/08/09  13:28:50  devrcs
 * 	Added the msync system call.
 * 	[90/07/24  15:17:55  havens]
 * 
 * Revision 1.5  90/07/17  11:42:11  devrcs
 * 	Added defines and function prototypes for msem_* calls such as msem_lock.
 * 	[90/07/04  14:51:38  havens]
 * 
 * Revision 1.4  90/04/27  19:26:26  devrcs
 * 	Add MAP_UNALIGNED (sigh)
 * 	[90/04/16  10:42:51  lwa]
 * 
 * 	Add many new flags, plus procedure declarations.
 * 	[90/01/24  09:30:41  lwa]
 * 
 * Revision 1.3  90/02/05  15:51:38  robert
 * 	Fix for system inclusion
 * 
 * Revision 1.2  90/01/02  20:15:58  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:20  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 1.1  89/10/16  19:48:20  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  22:05:44  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:54:47  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mman.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_MMAN_H_
#define _SYS_MMAN_H_

/* protections are chosen from these bits, or-ed together */
#define PROT_NONE	0		/* no access to these pages */
#define PROT_READ	0x1		/* pages can be read */
#define PROT_WRITE	0x2		/* pages can be written */
#define PROT_EXEC	0x4		/* pages can be executed */

/* flags contain sharing type, mapping type, and options */

/* mapping visibility: choose either SHARED or PRIVATE */
#define MAP_SHARED	0x1		/* share changes */
#define MAP_PRIVATE	0x2		/* changes are private */

/* mapping region: choose either FILE or ANONYMOUS */
#define	MAP_FILE	0x00		/* map from a file */
#define	MAP_ANONYMOUS	0x10		/* man an unnamed region */
#define	MAP_ANON	0x10		/* man an unnamed region */
#define	MAP_TYPE	0xf0		/* the type of the region */

/* mapping placement: choose either FIXED or VARIABLE */
#define	MAP_FIXED	0x100		/* map addr must be exactly as specified */
#define	MAP_VARIABLE	0x00		/* system can place new region */

/* other flags */

#define	MAP_HASSEMAPHORE 0x0200		/* region may contain semaphores */
#define	MAP_INHERIT	0x0400		/* region is retained after exec */
#define MAP_UNALIGNED	0x0800		/* allow non-page-aligned file offset */

/* advice to madvise */
#define MADV_NORMAL	0		/* no further special treatment */
#define MADV_RANDOM	1		/* expect random page references */
#define MADV_SEQUENTIAL	2		/* expect sequential page references */
#define MADV_WILLNEED	3		/* will need these pages */
#define MADV_DONTNEED	4		/* dont need these pages */
#define	MADV_SPACEAVAIL	5		/* ensure that resources are available */

/* msem conditions and structure */
typedef struct {
	int msem_state;		/* The semaphore is locked is non-zero. */
	int msem_wanted;	/* Threads are waiting on the semaphore. */
}msemaphore;

#define MSEM_UNLOCKED 	0	/* Initialize the semahore to unlocked */
#define MSEM_LOCKED 	1	/* Initialize the semahore to locked */
#define MSEM_IF_NOWAIT	2	/* Do not wait if semaphore is locked */
#define MSEM_IF_WAITERS	3	/* Unlock only if there are waiters */

/* msync flags */
#define MS_ASYNC 1		/* Asynchronous cache flush */
#define MS_SYNC  3		/* Synchronous cashe flush */
#define MS_INVALIDATE 4		/* Invalidate cached pages */

#if !defined(_NO_PROTO) && !defined(KERNEL)

extern int	madvise(caddr_t addr, size_t len, int behav);
extern caddr_t	mmap(caddr_t addr, size_t len, int prot, int flags, int filedes, off_t off);
extern int	mprotect(caddr_t addr, size_t len, int prot);
extern msemaphore 	*msem_init(msemaphore *sem, int initial_value);
extern int	msem_lock(msemaphore *sem, int condition);
extern int	msem_remove(msemaphore *sem);
extern int	msem_unlock(msemaphore *sem, int condition);
extern int	msync(caddr_t addr, size_t len, int flags);
extern int	munmap(caddr_t addr, size_t len);

#else /* _NO_PROTO || KERNEL */

extern int	madvise();
extern caddr_t	mmap();
extern int	mprotect();
extern msemaphore 	*msem_init();
extern int	msem_lock();
extern int	msem_remove();
extern int	msem_unlock();
extern int	msync();
extern int	munmap();

#endif /* _NO_PROTO || KERNEL */


#endif	/* _SYS_MMAN_H_ */
