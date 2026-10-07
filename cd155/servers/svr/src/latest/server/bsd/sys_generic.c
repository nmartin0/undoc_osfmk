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
/* @(#)sys_generic.c    2.3 16:53:54 5/25/90 SecureWare */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: sys_generic.c,v $
 * Revision 1.7  1994/11/18  20:28:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  17:49:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:06:15  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:59:04  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:54:58  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:17:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:07:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.7  92/11/06  12:47:46  mjl
 * Call fgetnode() for FIOGETNODE ioctl.
 * 
 * Revision 2.7  1992/11/06  12:47:46  mjl
 * Call fgetnode() for FIOGETNODE ioctl.
 *
 * Revision 2.6  92/03/09  14:31:46  durriya
 * 	Revision 3.7  91/12/18  17:15:34  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.5  92/01/02  18:54:32  roy
 * 	1991/10/14  20:07:50  noemi
 * 	Changed uio_segflg to UIO_SYSSPACE.
 * 
 * Revision 2.4  91/10/04  14:53:22  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:45:28  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	System calls with file descriptors as the first uarg were changed to 
 * 	have a file pointer as the first argument, and no file descriptor
 * 	in the uarg. Use new interface to getf(). select() and poll() system 
 * 	calls removed (along with select() and poll() subroutines) as they 
 * 	are now handled within emulator. ioctl() commands FIOCLEX and
 * 	FIONCLEX now handled in emulator. SIGPIPE signal on EPIPE error
 * 	now done in emulator.
 * 
 * Revision 2.2  91/08/31  13:23:23  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/05  13:54:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16.4.3  91/03/21  11:01:59  morris
 * 	Fixed poll_scan to ignore invalid file descriptors and return the
 * 	number of file descriptors for which poll set the revents bitmask.
 * 	[91/03/21  11:00:41  morris]
 * 
 * Revision 1.16.4.2  91/02/19  17:02:10  jeffc
 * 	Eliminate errno value checks in short I/O return case in rwuio.
 * 	[90/11/20  17:06:13  jeffc]
 * 
 * Revision 1.16  90/10/31  13:49:45  devrcs
 * 	Return short write on file system full errors.
 * 	[90/10/19  16:06:43  collins]
 * 
 * 	Seperation of O_NONBLOCK and O_NDELAY caused some conformance
 * 	problems.  This corrects that while still keeping the two
 * 	definitions seperate.
 * 	[90/10/17  16:16:05  jvs]
 * 
 * 	Seperate NDELAY and NONBLOCK.
 * 	[90/10/12  10:16:25  jvs]
 * 
 * Revision 1.15  90/10/07  13:19:29  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:00:13  gm]
 * 
 * Revision 1.14  90/09/13  11:42:20  devrcs
 * 	Fixed missing braces in poll_scan, which caused a zero results
 * 	whenever any of the revents fields was set to zero.
 * 
 * 	Made poll_scan zero the revents field for bogus file descriptors.
 * 	[90/08/29  15:56:37  coren]
 * 
 * 	Change poll() to use kalloc, not kmem_alloc_pageable. Markedly
 * 	more efficient with this, plus puts small poll_fds on stack.
 * 	[90/08/28  11:48:32  tmt]
 * 
 * 	Return to splx before event_wait().  Event_wait protects itself.
 * 	[90/08/22  14:34:46  gmf]
 * 
 * Revision 1.13  90/08/24  11:18:53  devrcs
 * 	Eliminate setjmp in rwuio.
 * 	Added timeout to event_wait for select and poll.
 * 	[90/08/20  01:05:01  gmf]
 * 
 * 	Changes for new system call interface.
 * 	Removed include of syscontext.h
 * 	Incorporate coren's fixes to poll.
 * 	Changes for u_file_state and getf.
 * 	Check for negative timeout in poll().
 * 	[90/08/17  17:39:30  nags]
 * 
 * Revision 1.12  90/07/27  08:44:40  devrcs
 * 	Readv/writev passed bogus address to copyin.
 * 	[90/07/17  08:43:17  nags]
 * 
 * Revision 1.11  90/07/17  11:19:55  devrcs
 * 	Change to audit hook
 * 	[90/07/10  21:52:45  seiden]
 * 
 * 	Condensed History:
 * 	Select event is now in the thread structure		nags@encore.com
 * 	Parallelized for OSF/1					nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing		seiden@osf.org
 * 	system V accounting, track of number of characters 	ers@osf.org
 * 	Fixed a couple of minor bugs in new poll call		nolting@osf.org
 * 	Added poll() system call				coren@osf.org
 * 	Changed select() to use new poll-compatible iface	coren@osf.org
 * 	Fix select problem (tmt)				tmt@osf.org
 * 	File layer parallelization changes			nags@encore.com
 * 	Posix tty support.					ers@osf.org
 * 	Removed include of <sys/dir.h>				gmf@osf.org
 * 	[90/06/10  01:56:30  seiden]
 * 
 * 	No longer set and increment fp->f_offset		noemi@osf.org
 * 	Moved resource pausing to CMUCS_RP conditional		noemi@osf.org
 * 	Read and write calls set uio_rw field of uio structure	noemi@osf.org
 * 	Changed rwuio not to call GETF()			ers@osf.org
 * 	Change to call new 4.4bsd read and write fileops.  	ers@osf.org
 * 
 * 	Condensed relevant ancient history, reverse chronology:
 * 	Merged Encore with Mach Release 2.5. (alan@encore)
 * 	MMAX_MP: make fileop ioctl take lock on structure (alan@encore)
 * 	MMAX_MP:  cleaned up select bugs (alan@encore)
 * 	MMAX_MP: Added lock around system time. (boykin@encore)
 * 	MMAX_MP: go to master in selscan based on parallel_fptype (boykin)
 * 	MMAX_MP:  merged in parallellized network code. (boykin)
 * 	MMAX_MP:  added file descriptor and structure locking (alan)
 * 	Added support for Sun's in the ioctl function (mikeg)
 * 	MIPS ioctl (af)
 * 	Corrected include file references. (mwyoung)
 * 	Syscall does read/write* in parallel; rwuio does unix_master (rvb)
 * 	New scheduling state machine. (dbg)
 * 	Support for multiple threads. (avie)
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
 *	@(#)sys_generic.c	7.1 (Berkeley) 6/5/86
 */

#include <sys/unix_defs.h>
#include <sys/secdefines.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/stat.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include <kern/parallel.h>
#include <kern/sched_prim.h>
#include <kern/kalloc.h>

/* 
 * Removed select() and poll() system calls from this file.
 *  Now they are done in the emulation library
 */

/*
 * Read system call.
 */
read(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		char	*cbuf;
		unsigned count;
	} *uap = (struct args *) args;
	struct uio auio;
	struct iovec aiov;

	aiov.iov_base = (caddr_t)uap->cbuf;
	aiov.iov_len = uap->count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_READ;
	return (rwuio(fp, &auio, UIO_READ, retval));
}

readv(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		struct	iovec *iovp;
		unsigned iovcnt;
	} *uap = (struct args *) args;
	struct uio auio;
	register struct iovec *iov;
	int error;
	struct iovec aiov[UIO_SMALLIOV];

	if (uap->iovcnt > UIO_SMALLIOV) {
		if (uap->iovcnt > UIO_MAXIOV)
			return (EINVAL);	
		if ((iov = (struct iovec *)
			    kalloc(sizeof(struct iovec) * (uap->iovcnt))) == 0)
			return (ENOMEM);
	} else
		iov = aiov;
	auio.uio_iov = iov;
	auio.uio_iovcnt = uap->iovcnt;
	auio.uio_rw = UIO_READ;
	error = copyin((caddr_t)uap->iovp, (caddr_t)iov,
	    uap->iovcnt * sizeof (struct iovec));
	if (!error)
		error = rwuio(fp, &auio, UIO_READ, retval);
	if (uap->iovcnt > UIO_SMALLIOV)
		kfree(iov, sizeof(struct iovec)*uap->iovcnt);
	return (error);
}

/*
 * Write system call
 */
write(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		char	*cbuf;
		unsigned count;
	} *uap = (struct args *) args;
	struct uio auio;
	struct iovec aiov;


	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_WRITE;
	aiov.iov_base = uap->cbuf;
	aiov.iov_len = uap->count;
	return (rwuio(fp, &auio, UIO_WRITE, retval));
}

writev(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		struct	iovec *iovp;
		unsigned iovcnt;
	} *uap = (struct args *) args;
	struct uio auio;
	register struct iovec *iov;
	int error;
	struct iovec aiov[UIO_SMALLIOV];

	if (uap->iovcnt > UIO_SMALLIOV) {
		if (uap->iovcnt > UIO_MAXIOV)
			return (EINVAL);	
		if ((iov = (struct iovec *)
			    kalloc(sizeof(struct iovec) * (uap->iovcnt))) == 0)
			return (ENOMEM);
	} else
		iov = aiov;
	auio.uio_iov = iov;
	auio.uio_iovcnt = uap->iovcnt;
	auio.uio_rw = UIO_WRITE;
	error = copyin((caddr_t)uap->iovp, (caddr_t)iov,
	    uap->iovcnt * sizeof (struct iovec));
	if (!error)
		error = rwuio(fp, &auio, UIO_WRITE, retval);
	if (uap->iovcnt > UIO_SMALLIOV)
		kfree(iov, sizeof(struct iovec)*uap->iovcnt);
	return (error);
}

rwuio(fp, uio, rw, retval)
	struct file *fp;
	register struct uio *uio;
	enum uio_rw rw;
	int *retval;
{
	register struct iovec *iov;
	int i, count, flag, error;

	/* We could inline getf for performance */
	if (error = getf(fp))
		return (error);
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	if ((flag&(rw==UIO_READ ? FREAD : FWRITE)) == 0) {
#if	SEC_BASE
		audstub_rdwr2();
#endif
		error = EBADF;
		goto out;
	}
	uio->uio_resid = 0;
#ifdef	OSF1_ADFS
	uio->uio_segflg = UIO_SYSSPACE;
#else
	uio->uio_segflg = UIO_USERSPACE;
#endif
	iov = uio->uio_iov;
	for (i = 0; i < uio->uio_iovcnt; i++) {
		if (iov->iov_len < 0) {
			error = EINVAL;
			goto out;
		}
		uio->uio_resid += iov->iov_len;
		if (uio->uio_resid < 0) {
			error = EINVAL;
			goto out;
		}
		iov++;
	}
	count = uio->uio_resid;
	/*
	 * We don't need to crhold fp->f_cred because we already
	 * hold a reference on the file structure.  This prevents
	 * fp->f_cred from being freed.
	 */
	if (rw == UIO_READ)
		FOP_READ(fp, uio, fp->f_cred, error);
	else
		FOP_WRITE(fp, uio, fp->f_cred, error);
	if (error) {
		/*
		 * If some data has been moved, then we should
		 * report the movement count rather than the error.
		 */
		if (uio->uio_resid != count)
			error = 0;
		/* RABII another psignal removed */
	}
	*retval = count - uio->uio_resid;
	u.u_ioch += (unsigned) *retval;
#if	SEC_BASE
	if (*retval) {
		u.u_error = error;	/* XXX */
		audstub_rdwr1(rw, fdes);
	}
#endif
out:
	FP_UNREF(fp);
	return (error);
}

/*
 * Ioctl system call
 */
ioctl(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		int	cmd;
		caddr_t	cmarg;
	} *uap = (struct args *) args;
	register int com, error, flag;
	register u_int size;
	caddr_t memp = 0;
#define STK_PARMS	128
	char stkbuf[STK_PARMS];
	caddr_t data = stkbuf;

	error = 0;
	if (error = getf(fp))
		return (error);
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	if ((flag & (FREAD|FWRITE)) == 0) {
		error = EBADF;
		goto out;
	}
	com = uap->cmd;

	/*
	 * Interpret high order word to find
	 * amount of data to be copied to/from the
	 * user's address space.
	 */
	size = IOCPARM_LEN(com);
	if (size > IOCPARM_MAX) {
		error = ENOTTY;
		goto out;
	}
	if (size > sizeof (stkbuf)) {
		if ((memp = (caddr_t)kalloc(size)) == 0) {
			error = ENOMEM;
			goto out;
		}
		data = memp;
	}
	if (com&IOC_IN) {
#if	mips || sun	/* Needed on Sun's to make Suntools happy */
		if (size == sizeof (int) && uap->cmarg == NULL)
			*(int *)data = 0;
		else
#endif
		if (size) {
			if (error = 
			    copyin(uap->cmarg, (caddr_t)data, (u_int)size))
				goto out;
		} else
			*(caddr_t *)data = uap->cmarg;
	} else if ((com&IOC_OUT) && size)
		/*
		 * Zero the buffer on the stack so the user
		 * always gets back something deterministic.
		 */
		bzero((caddr_t)data, size);
	else if (com&IOC_VOID)
		*(caddr_t *)data = uap->cmarg;

	switch (com) {

	case FIONBIO:
		/* was fset(fp, FNDELAY, *(int *)data); */
		error = fset(fp, FNONBLOCK, *(int *)data);
		break;

	case FIOASYNC:
		error = fset(fp, FASYNC, *(int *)data);
		break;

	case FIOSETOWN:
		error = fsetown(fp, *(int *)data);
		break;

	case FIOGETOWN:
		error = fgetown(fp, (int *)data);
		break;

#ifdef	OSF1_ADFS
	case FIOGETNODE:
		error = fgetnode(fp, uap->cmarg);
		break;
#endif
	default:
		/*
		 * We expect the lower-level routine to lock and unlock
		 * the file structure as necessary.  We guarantee that the
		 * file structure won't disappear because we hold a reference
		 * on the structure courtesy of getf.
		 */

		/* XXX hack for STREAMS -- temporary */
		u.u_spare[0] = 0;
		/* end hack */

		FOP_IOCTL(fp, com, data, error);

		/*
		 * Copy any data to user, size was
		 * already set and checked above.
		 */
		if (error == 0 && (com&IOC_OUT) && size)
			error = copyout(data, uap->cmarg, (u_int)size);

		/* XXX hack for STREAMS -- temporary */
		if (u.u_spare[0] != 0)
			*retval = u.u_spare[0];
		/* end hack */

		break;
	}
out:
	FP_UNREF(fp);
	if (memp)
		kfree(memp, size);
	return (error);
}


/*ARGSUSED*/
seltrue(dev, events, revents, scanning)
	int scanning;
	dev_t dev;
        short *events, *revents;
{
	if (scanning) {
		*revents = *events;
	}
	return (0);
}

