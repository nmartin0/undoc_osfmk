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
 * $Log: ufs_physio.c,v $
 * Revision 1.5  1994/11/18  20:46:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:38:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:53:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:56  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:40  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.5  1993/04/08  11:33:32  loverso
 * 	Revision 3.5  92/04/22  16:49:39  barbou
 * 	Preparation for OSF/1 build environment: added missing type casts.
 *
 * Revision 1.2  1992/11/30  22:51:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:39:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:48:29  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/01/09  16:31:51  roy
 * 	Add missing arg to vm_allocate().
 * 
 * Revision 2.3  91/12/17  08:56:15  roy
 * 	91/10/17  18:33:38  barbou
 * 	Almost complete but not very efficient or clean implementation of 
 * 	physio for LVM.
 * 
 * Revision 2.2  91/08/31  14:19:40  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  17:01:05  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9.4.2  91/02/19  17:06:56  jeffc
 * 	Remove unnecessary machine dependencies from this file,
 * 	change usage of minphys slightly: short returns from the
 * 	driver strategy routine will cause it to be re-entered.
 * 	Zero return or error causes loop to terminate. This pretty
 * 	much eliminates the need for a device-specific minphys;
 * 	however, we should eventually optimize the vm_map_pageable
 * 	calls to remove unnecessary paging acivity.
 * 	[90/11/27  09:32:39  jeffc]
 * 
 * Revision 1.9  90/10/07  14:59:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:53:18  gm]
 * 
 * Revision 1.8  90/09/13  11:51:39  devrcs
 * 	Fixed assertion failure caused when using the raw device.  (bug 812)
 * 	[90/08/30  15:41:10  noemi]
 * 
 * Revision 1.7  90/08/24  12:29:07  devrcs
 * 	fix parameters to event_wait
 * 	[90/08/19  23:21:49  gmf]
 * 
 * Revision 1.6  90/07/27  09:09:07  devrcs
 * 	Don't do access checks and don't try to wire memory in physio() if
 * 	segflg is SYSSPACE.
 * 	[90/07/23  14:40:41  ers]
 * 
 * 	Changed use or 'rw' argument to physio to allow passing the
 * 	new "B_WRITEV" flag through to the strategy routine.  This
 * 	functionality is incomplete, but innocuous at this stage.
 * 	[90/04/16  15:32:22  jeffc]
 * 
 * Revision 1.5  90/06/22  20:55:50  devrcs
 * 	nags merge
 * 
 * 	Condensed History:
 * 	BSD4.4 Changes.				gm@osf.org
 * 	Mach Release 2.5 plus mmax parallel	alan@encore.com
 * 	Fixed a bug in physio 			alan@encore.com
 * 	Simpler version of physio()		alan@encore.com
 * 	Buffer locking and unlocking 		alan@encore.com
 * 	Replace vslock/vsunlock w/ Mach equivs	dlb@cs.cmu.edu
 * 	Relocated from bsd/vm_swp.c		mwyoung@cs.cmu.edu
 * 	Corrected include file references.	af@cs.cmu.edu
 * 	Changes for I386			rvb@cs.cmu.edu
 * 	[90/06/12  21:42:01  gmf]
 * 
 * $EndLog$
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
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)vm_swp.c	7.10 (Berkeley) 9/14/89
 */

/*
 * Major changes from Berkeley:
 *	Mach VM is very different... swapping code has been removed.
 */
 
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/vnode.h>

#ifdef	OSF1_SERVER
#include <uxkern/import_mach.h>
#else	/* OSF1_SERVER */
#include <kern/task.h>
#include <mach/vm_param.h>
#include <vm/vm_map.h>
#endif	/* OSF1_SERVER */

/*
 * Raw I/O. The arguments are
 *	The strategy routine for the device
 *	A buffer, which will always be a special buffer
 *	  header owned exclusively by the device for this purpose
 *
 *	The device number
 *	Read/write flags (B_READ/B_WRITE, B_WRITEV)
 * Essentially all the work is computing physical addresses and
 * validating them.
 * If the user has the proper access privilidges, the process is
 * marked 'delayed unlock' and the pages involved in the I/O are
 * faulted and locked. After the completion of the I/O, the above pages
 * are unlocked.
 */
physio(strat, bp, dev, rw, mincnt, uio)
	int (*strat)(); 
	register struct buf *bp;
	dev_t dev;
	int rw;
	u_int (*mincnt)();
	struct uio *uio;
{
	register struct iovec *iov;
	register int requested, done;
	char *a;
	int error = 0;
	vm_offset_t	start, end;
#ifdef	OSF1_SERVER
	caddr_t iov_addr;	/* iov->iov_base + what's been done */
#endif	/* OSF1_SERVER */

	LASSERT(BUF_LOCK_HOLDER(bp));

#ifdef	OSF1_SERVER
	if (bp->b_optimize_mem == FALSE) {
		bp->b_bufsize = 0;
	} else if (uio->uio_segflg == UIO_USERSPACE) {
		panic("physio: need code to turn off memory optimization of buf for user space IO");
	}
		
#endif	/* OSF1_SERVER */

	for (; uio->uio_iovcnt; uio->uio_iov++, uio->uio_iovcnt--) {
		iov = uio->uio_iov;
		if (uio->uio_segflg == UIO_USERSPACE &&
		    !useracc(iov->iov_base, (u_int)iov->iov_len,
		    (rw & B_READ) ? B_WRITE : B_READ)) {
			error = EFAULT;
			break;
		}
		bp->b_proc = u.u_procp;
		bp->b_error = 0;
#ifdef	OSF1_SERVER
		if (uio->uio_segflg == UIO_SYSSPACE &&
		    bp->b_optimize_mem == FALSE) {
			bp->b_un.b_addr = iov->iov_base;
		}
		iov_addr = iov->iov_base;
#else	/* OSF1_SERVER */
		bp->b_un.b_addr = iov->iov_base;
#endif	/* OSF1_SERVER */
		while (iov->iov_len > 0) {
			LASSERT(BUF_LOCK_HOLDER(bp));
			bp->b_flags = B_PHYS | B_RAW | rw;
			event_clear(&bp->b_iocomplete);
			bp->b_dev = dev;
			bp->b_blkno = btodb(uio->uio_offset);
			bp->b_bcount = iov->iov_len;
			(*mincnt)(bp);
			requested = bp->b_bcount;
#ifdef	OSF1_SERVER
			a = iov_addr;
#else	/* OSF1_SERVER */
			a = bp->b_un.b_addr;
#endif	/* OSF1_SERVER */
			start = trunc_page((vm_address_t) a);
			end = round_page((vm_address_t) (a + requested - 1));
#ifdef	OSF1_SERVER
			if (uio->uio_segflg == UIO_USERSPACE &&
			    bp->b_bufsize < bp->b_bcount) {
				if (bp->b_bufsize > 0) {
					(void) vm_deallocate(mach_task_self(),
							     bp->b_un.b_addr,
							     bp->b_bufsize);
				}
				bp->b_bufsize = end - start;
				if (vm_allocate(mach_task_self(), 
						&bp->b_un.b_addr,
						bp->b_bufsize, TRUE)
				    != KERN_SUCCESS) {
					error = EFAULT;
					break;
				}
			}
			if (uio->uio_segflg == UIO_USERSPACE &&
			    (rw & B_READ) == 0) {
				if (copyin(iov_addr, bp->b_un.b_addr, 
					   bp->b_bcount) != 0) {
					error = EFAULT;
					break;
				}
			}
#else	/* OSF1_SERVER */
			/* Assume already wired if sys space */
			if (uio->uio_segflg != UIO_SYSSPACE &&
			    vm_map_pageable(current_task()->map, start, end,
				(rw & B_READ) ? VM_PROT_WRITE : VM_PROT_READ)
			    != KERN_SUCCESS) {
				error = EFAULT;
				break;
			}
#endif	/* OSF1_SERVER */
			(*strat)(bp);
			error = biowait(bp);
			LASSERT(BUF_LOCK_HOLDER(bp));

#ifdef	OSF1_SERVER
			done = bp->b_bcount - bp->b_resid;
			if (uio->uio_segflg == UIO_USERSPACE &&
			    (rw & B_READ)) {
				if (copyout(bp->b_un.b_addr, 
					    iov_addr,
					    done) != 0) {
					error = EFAULT;
					break;
				}
			}
#else	/* OSF1_SERVER */
			if (uio->uio_segflg == UIO_USERSPACE)
				(void)vm_map_pageable(current_task()->map,
						start, end, VM_PROT_NONE);
#endif	/* OSF1_SERVER */

#ifdef	OSF1_SERVER
			if (uio->uio_segflg == UIO_SYSSPACE &&
			    bp->b_optimize_mem == FALSE) {
				bp->b_un.b_addr += done;
			}
			iov_addr += done;
#else	/* OSF1_SERVER */
			done = bp->b_bcount - bp->b_resid;
			bp->b_un.b_addr += done;
#endif	/* OSF1_SERVER */
			iov->iov_len -= done;
			uio->uio_resid -= done;
			uio->uio_offset += done;

			if (done == 0 || error)
				break;
		}
		bp->b_flags &= ~(B_PHYS | B_RAW);

		if (done == 0 || error)
			break;
	}

#ifdef	OSF1_SERVER
	if (uio->uio_segflg == UIO_USERSPACE &&
	    bp->b_bufsize > 0) {
		/* deallocate last buffer */
		(void) vm_deallocate(mach_task_self(), 
				     bp->b_un.b_addr, bp->b_bufsize);
		bp->b_bufsize = 0;
	}
#endif	/* OSF1_SERVER */

	/* Note: rwuio will cause partial transfers to 
	 * be reported as "success", even though we return an error.
	 */
	return (error);
}

#ifdef	ibmrt
#include <sc.h>
#else
#define NSCC	0
#endif

#if	NSCC > 0
#define MAXPHYS	(64 * 512)	/* don't increase beyond NDMAXIO */
#else
#define MAXPHYS	(63 * 1024)
#endif

#if	EXL
#undef  MAXPHYS
#define MAXPHYS (32 * 4096)     /* EXL can take 32 pages for dma */
#endif

u_int
minphys(bp)
	struct buf *bp;
{
	LASSERT(BUF_LOCK_HOLDER(bp));
	if (bp->b_bcount > MAXPHYS)
		bp->b_bcount = MAXPHYS;
}
