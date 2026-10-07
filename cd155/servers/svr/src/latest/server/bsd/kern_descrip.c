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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: kern_descrip.c,v $
 * Revision 1.17  1995/02/01  21:26:08  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1994/11/18  20:26:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/02/06  00:17:11  jlitvin
 * Remove some dead code that lint found.
 *
 * Revision 1.14  1994/01/13  17:54:16  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.13  1993/10/21  17:17:17  cfj
 * Modify the "closef: f_count not 1" panic to print out the reference
 * count for the offending file structure.  Hopefully this will give a handle
 * on the reasons for PTS bug #6640.
 *
 * Revision 1.12  1993/10/18  18:44:58  jlitvin
 * Fix for PTS bug #6567 broke PTS bugs #6734, #6772 and #6817.  Back out
 * that fix and reconsider treating FNONBLOCK and FNDELAY as semantically
 * the same.
 *
 * Revision 1.11  1993/10/11  13:23:31  nandy
 * Call to VPOP_GET_ATTR has two new parameters: uid, ruid.
 *
 * Revision 1.10  1993/09/14  14:48:52  cfj
 * Merged R1.1 bug fix into main stem.
 *
 * Revision 1.9.2.1  1993/09/14  14:45:52  cfj
 * Fix for PTS bug #6567.  Treat FNONBLOCK and FNDELAY as semantically
 * equivalent for fcntl().
 *
 * Revision 1.9  1993/09/01  01:34:27  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.8  1993/07/29  21:51:26  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.7  1993/07/14  17:46:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:45:40  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:02:14  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:03:43  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.3  1993/02/16  20:02:33  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/22  01:22:56  nandy
 * 1/20/93 Code drop from LOCUS
 *
 * Revision 1.1.2.1.2.2  1992/12/16  23:00:43  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:58:19  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.2  1992/11/30  22:15:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:05:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.25  93/08/31  09:27:09  mjl
 * [LCC #0381] Fix bug in fset(), it intended to treat FNONBLOCK and FNDELAY
 * as semantically the same but didn't really do so.
 * 
 * Revision 2.24  93/07/27  11:23:38  mjl
 * Change f_seqno_mtx to f_seqno_lock.
 * 
 * Revision 2.23  93/06/29  15:33:51  bhk
 * Partial update to OSF/1 AD V1.05b1 to fix select problems
 * 
 * Revision 2.22  93/04/29  13:57:19  klh
 * 	Revision 2.17  93/03/10  10:47:11  mmp
 * 		In closef, check for FUNREFWAIT flag and do a thread_wakeup if
 * 		it is set.
 *
 * 	Revision 2.16  93/02/10  14:03:53  durriya
 * 		In fdealloc, make assertions about f_count and f_type while
 * 		holding FP_LOCK.  (mmp)
 *
 * 	Revision 2.15  93/02/02  15:22:11  rabii
 * 		In falloc() only hold on to FREE_FILE_LOCK when accessing free_file_head
 *
 * 		Put file structures on a linked list (rabii)
 *
 * Revision 2.21  93/03/29  16:29:02  yazz
 * Replaced some debug code + ASSERT with a panic that prints more
 * info than the assertion had printed.
 * 
 * Revision 2.20  93/03/27  17:03:45  yazz
 * Add if statement & printf ahead of an assertion, to provide more info
 * (but only in cases where assertion is about to fail)..
 * 
 * Revision 2.19  93/01/06  16:38:29  mjl
 * Calling soo_ioctl() directly is not allowed in TNC, you *must* go through
 * the file ops pointer!
 * 
 * Revision 2.18  92/12/29  12:47:40  chrisp
 * File table is now a dynamically allocated list of file structures. NFILE
 * 	file structures still being allocated and initialized at startup
 * 	(file_table_init()) but more will be zalloc()ed if this initial
 * 	allocation is exhausted. Note: file and fileNFILE now point to
 * 	the lowest and highest addresses of allocated file structures.
 * 
 * Revision 2.17  92/11/24  14:19:28  chrisp
 * Remove extraneous ASSERT() in falloc(). [chrisp for klh for mjl]
 * 
 * Revision 2.16  92/11/23  10:32:35  klh
 * Initialize fp-f_seqno under OSF1-ADFS, not just under TNC #ifdef.
 * For TNC, initialize f_seqno_mtx shared mutex pointer.  All this in falloc().
 * (klh for mjl)
 * 
 * Revision 2.15  92/11/06  12:47:18  mjl
 * Add fgetnode() routine to copyout node number of node where file port's
 * receive right is located.
 * 
 * Revision 2.14  92/08/17  12:49:31  mjl
 * Have falloc() initialize file struct chain used by TNC FIFO relocation.
 * Have closef() pass the closing file pointer to TNC code that needs it.
 * 
 * Revision 2.13  92/08/08  01:28:20  jdh
 * fp_seqno is kept in sync with mach seqno, instead of being off
 * by 1 as was previously done -- jdh
 * 
 * Revision 2.12  92/06/11  16:06:47  mjl
 * In fdealloc(), don't deallocate a file port that has relocated (migrated).
 * 
 * Revision 2.11  92/02/17  14:34:36  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.10  92/02/11  22:10:39  pjg
 * 	Get rid of bogus i860 ifdef and use correct ANSI C code for both
 * 	i860 and other architectures (roman@locus).
 * 
 * Revision 2.9  92/01/09  16:28:39  roy
 * 	Undo last change.
 * 
 * Revision 2.8  92/01/05  18:19:51  roy
 * 	92/01/05  14:43:38  roy
 * 	For OSF1_ADFS, falloc() doesn't reference u.u_cred.
 * 
 * Revision 2.7  92/01/02  18:50:45  roy
 * 	92/01/02  17:31:18  roy
 * 	f_seqno field no longer exists in file structure.
 * 
 * 	91/11/27  02:56:46  ses
 * 	For OSF1_ADFS, changed initialization of fseqno field of fp from 0
 * 	to -1, so it corresponds to message seq numbers.  Deleted an ASSERTion.
 * 
 * 	91/10/14  20:02:05  noemi
 * 	For OSF1_ADFS, initialize magic number and sequence number in file 
 * 	structure to 0.
 * 
 * Revision 2.6  91/11/22  14:54:13  rabii
 * 	Locus drop merge including the following:
 * 	VPOP_GET_ATTR now has additional parameter. (chrisp)
 * 	Changed call to VPOP_GET_ATTR() to make ANSI C happy. (roman)
 * 	VPOP_GET_ATTR replaces VPOP_GET_PGRP_SID (chrisp)
 * 
 *
 * Revision 2.5  91/11/13  12:48:30  rabii
 * 	Fixed compiler complaint of casting &(pid_t)val to (pid_t *)&val
 * 
 * Revision 2.4  91/10/04  14:43:38  chrisp
 * Get rid of extraneous RCS $Log.
 * 
 * Revision 2.3  91/09/16  15:31:08  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	File descriptors now handled in emulator. All mention of them removed
 * 	from file. This involved changes to the call interface to
 * 	getf(), ufalloc(), and ufdealloc(). Additionally, system calls
 * 	with file descriptors as the first uarg were changed to have
 * 	a file pointer as the first argument, and no file descriptor
 * 	in the uarg. Also, close(), dup(), and dup2() were removed
 * 	entirely, as they are now done in the emulator. Handling of
 * 	fcntl() commands F_DUPFD, F_SETFD, and F_GETFD also now
 * 	handled in emulator. Changed getting pgrp code to use
 * 	 proc ops.
 * 
 * Revision 2.2  91/08/31  13:21:21  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/12  15:34:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14  90/10/31  13:48:40  devrcs
 * 	Seperation of O_NONBLOCK and O_NDELAY caused some conformance
 * 	problems.  This corrects that while still keeping the two
 * 	definitions seperate.
 * 	[90/10/17  16:15:38  jvs]
 * 
 * 	Seperate NDELAY and NONBLOCK.
 * 	[90/10/12  10:16:03  jvs]
 * 
 * Revision 1.13  90/10/07  13:16:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:54:43  gm]
 * 
 * 	Use the FD_CLOEXEC definitions instead of the constant 1.
 * 	[90/09/27  17:07:27  collins]
 * 
 * Revision 1.12  90/09/23  15:42:49  devrcs
 * 	Now checks struct flock arguments passed
 * 	to fcntl.
 * 	[90/09/21  16:17:08  hermi]
 * 
 * 	fix typo in previous change by gmf
 * 	[90/09/14  17:08:15  garyf]
 * 
 * 	Put cleanlocks() call into closef for System V
 * 	file locking (bug 1035).
 * 	[90/09/14  13:43:26  gmf]
 * 
 * 	Deleted the flag FFLCK in the file structure.  The routines that manage
 * 	file locks set/clear VLOCKS in the v_flag of the vnode.
 * 	[90/09/06  14:33:45  swallace]
 * 
 * Revision 1.11  90/08/24  11:15:42  devrcs
 * 	New system call interface
 * 	Removed include of syscontext.h
 * 	Changed RETURN to return.
 * 	Turned getf into a real function, and changed GETF macro calls to getf
 * 		function calls, including u_file_state changes.
 * 	[90/08/17  00:07:41  nags]
 * 
 * Revision 1.10  90/06/29  13:34:57  devrcs
 * 	Add BM lock.  Add use of FFLCK to mark files that have been locked, so
 * 	they can be intelligently cleaned up by closef.
 * 	[90/06/26  11:04:03  gmf]
 * 
 * 	nags merge
 * 
 * 	Condensed history, reverse chronology:
 * 	Parallelized for OSF/1				nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing	seiden@osf.org
 * 	Go via soo_ioctl for socket locking		tmt@osf.org
 * 	|| File layer: locks, free file list		noemi@osf.org
 * 	Added fcntl() F_SETLK, F_SETLKW, and F_GETLK	ers@osf.org
 * 	Change to use 4.4BSD pgrp structure.		coren@osf.org
 * 	Merged with 4.4BSD				noemi@osf.org
 * 	Encore BSD parallelization changes		alan@encore.com
 * 	[90/06/12  19:05:25  gmf]
 * 
 * Revision 1.9  90/06/22  20:05:35  devrcs
 * 	Removed debug code.
 * 	[90/06/18  16:58:27  gmf]
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
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_descrip.c	7.8 (Berkeley) 12/19/89
 */

#include <sys/secdefines.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <kern/parallel.h>
#include <kern/assert.h>
#include <sys/lock_types.h>
#include <sys/vproc.h>
#include <kern/zalloc.h>

/* Eliminated dup(), dup2(), close(), and F_DUPFD and F_SETFD options */
/* in fcntl() from this file.  These are now done in the emulation    */
/* library.							      */

/*
 * Descriptor management.
 */

/*
 * The free file lock controls allocation and deallocation of elements
 * within the file structure list.
 */
udecl_simple_lock_data(,free_file_lock)
#define	FREE_FILE_LOCK()	usimple_lock(&free_file_lock)
#define	FREE_FILE_UNLOCK()	usimple_unlock(&free_file_lock)
#define	FREE_FILE_LOCK_INIT() 	usimple_lock_init(&free_file_lock)

/*
 * System calls on descriptors.
 */
/* ARGSUSED */
getdtablesize(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	*retval = NOFILE;
	return (0);
}

/* ARGSUSED */
getdopt(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	return (EOPNOTSUPP);
}

/* ARGSUSED */
setdopt(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	return (EOPNOTSUPP);
}

static int checkflock();

/*
 * The file control system call.
 */
/* ARGSUSED */
fcntl(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		int	cmd;
		int	arg;
	} *uap = (struct args *)args;
	int i, error = 0;
	struct flock bf;

	if (error = getf(fp))
		return (error);
#if SEC_BASE
	audstub_fcntl1(uap->cmd);
#endif
	switch(uap->cmd) {
	case F_GETFL:
		BM(FP_LOCK(fp));
		*retval = fp->f_flag+FOPEN;
		BM(FP_UNLOCK(fp));
		break;

	case F_SETFL:
		/*
		 * Guarantee that flags will match the request to
		 * the ioctl routine by holding a lock across both
		 * operations.  We can't hold f_incore_lock for a
		 * long time so we cheat by using the f_io_lock.
		 * This is only a problem for a multiprocessor or a
		 * pre-emptible kernel.
		 */
		MP_ONLY(FP_IO_LOCK(fp));
		FP_LOCK(fp);
		fp->f_flag &= FCNTLCANT;
		fp->f_flag |= (uap->arg-FOPEN) &~ FCNTLCANT;
		FP_UNLOCK(fp);
		if (error = fset(fp, FNDELAY, fp->f_flag & FNDELAY)) {
			MP_ONLY(FP_IO_UNLOCK(fp));
			break;
		}
		if (error = fset(fp, FNONBLOCK, fp->f_flag & FNONBLOCK)) {
			MP_ONLY(FP_IO_UNLOCK(fp));
			break;
		}
		if (error = fset(fp, FASYNC, fp->f_flag & FASYNC)) {
			/* was fset(fp, FNDELAY, 0) */
			(void) fset(fp, FNDELAY|FNONBLOCK, 0);
		}
		MP_ONLY(FP_IO_UNLOCK(fp));
		break;

	case F_GETOWN:
		error = fgetown(fp, retval);
		break;

	case F_SETOWN:
		error = fsetown(fp, uap->arg);
		break;

	case F_GETLK:
		/* get record lock */
		if (copyin(uap->arg, &bf, sizeof bf))
			error = EFAULT;
		else if ((i=checkflock(&bf)) != 0)
			 error = i;
		else if ((i=getflck(fp, &bf)) != 0)
			error = i;
		else if (copyout(&bf, uap->arg, sizeof bf))
			error = EFAULT;
		break;

	case F_SETLK:
		/* set record lock and return if blocked */
		if (copyin(uap->arg, &bf, sizeof bf))
			error = EFAULT;
		else if ((i=checkflock(&bf)) != 0)
			 error = i;
		else if ((i=setflck(fp, &bf, 0)) != 0)
			error = i;
		break;

	case F_SETLKW:
		/* set record lock and wait if blocked */
		if (copyin(uap->arg, &bf, sizeof bf))
			error = EFAULT;
		else if ((i=checkflock(&bf)) != 0)
			 error = i;
		else if ((i=setflck(fp, &bf, 1)) != 0)
			error = i;
		break;

	default:
		error = EINVAL;
	}
out:
	FP_UNREF(fp);
	return (error);
}

static int
checkflock(bf)
struct flock *bf;
{
	short type;
	short whence;

	/* check l_type is valid */
	type = bf->l_type;
	if ((type != F_RDLCK) && (type != F_WRLCK) &&
	    (type != F_UNLCK))
		return EINVAL;

	/* check l_whence is valid */
	whence = bf->l_whence;
	if ((whence != L_SET) && (whence != L_INCR) &&
	    (whence != L_XTND))
		return EINVAL;

	/* check l_start is not before file start */
	/* Argh! off_t is unsigned - need casts */
	if (((long)bf->l_start < 0) &&
	    (whence == L_SET))
		return EINVAL;

	/* check that for -ve l_len, l_end is not before file start */
	if (((long)bf->l_len < 0) &&
	    (whence == L_SET)) {
		if (((long)(bf->l_start) - 1) < 0)
			return EINVAL;
	}

	return 0;
}

fset(fp, bit, value)
	struct file *fp;
	int bit, value;
{

	FP_LOCK(fp);
	if (value)
		fp->f_flag |= bit;
	else
		fp->f_flag &= ~bit;
	FP_UNLOCK(fp);
	/* was (bit == FNDELAY) */
	return (fioctl(fp, 
		(int)((bit & (FNDELAY|FNONBLOCK)) ? FIONBIO : FIOASYNC),
		(caddr_t)&value));
}

fgetown(fp, valuep)
	struct file *fp;
	int *valuep;
{
	int error;

	switch (fp->f_type) {

	case DTYPE_SOCKET:
		/* For TNC, must go thru FOP_IOCTL() macro! */
		return fioctl(fp, (int)SIOCGPGRP, (caddr_t)valuep);

	default:
		error = fioctl(fp, (int)TIOCGPGRP, (caddr_t)valuep);
		*valuep = -*valuep;
		return (error);
	}
}

fsetown(fp, value)
	struct file *fp;
	int value;
{

	if (fp->f_type == DTYPE_SOCKET)
		/* For TNC, must go thru FOP_IOCTL() macro! */
		return fioctl(fp, (int)SIOCSPGRP, (caddr_t)&value);
	if (value > 0) {
		struct vproc *vp;
		vp = LOCATE_VPROC_PID(value);
		if (vp == 0) 
			return (ESRCH);
		if (VPOP_GET_ATTR(vp, 0, (pid_t *)&value, 0, 0, 0, 0, 0) == 0) {
			VPROC_RELEASE(vp,"fsetown");
			return(ESRCH);
		}
		VPROC_RELEASE(vp,"fsetown");
	} else
		value = -value;
	return (fioctl(fp, (int)TIOCSPGRP, (caddr_t)&value));
}

#ifdef	OSF1_ADFS
/* ARGSUSED */
fgetnode(fp, uva)
	struct file *fp;
	caddr_t uva;
{
	extern node_t this_node;

	/* XXX I think fgetown() above should call copyout as well! */
	return(copyout(&this_node, uva, sizeof(node_t)));
}
#endif	/* OSF1_ADFS */

fioctl(fp, cmd, value)
	struct file *fp;
	int cmd;
	caddr_t value;
{
	int	ret_val;

	FOP_IOCTL(fp, cmd, value, ret_val);
	return (ret_val);
}

/* ARGSUSED */
fstat(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		struct	stat *sb;
       	} *uap = (struct args *)args;
	struct stat ub;
	int error = 0;

	if (error = getf(fp))
		return (error);
	switch (fp->f_type) {
	case DTYPE_VNODE:
		error = vn_stat((struct vnode *)fp->f_data, &ub);
		break;

	case DTYPE_SOCKET:
		error = soo_stat((struct socket *)fp->f_data, &ub);
		break;

	default:
		panic("fstat");
		/*NOTREACHED*/
	}
	if (error == 0)
		error = copyout((caddr_t)&ub, (caddr_t)uap->sb,
		    sizeof (ub));
	FP_UNREF(fp);
	return (error);
}

struct file	free_file_head;		/* Head of free file list */
zone_t		file_zone = NULL;	/* zone for file structures */

struct file *
file_new()
{
	register struct file *fp;
	/*
	 * Allocate a new file structure from the file zone.
	 * Keeping track of its limits.
	 */
	fp = (struct file *) zalloc(file_zone);
	/* Note: zalloc() panics if unable to obtain memory */
	nfile++;
	/* put it on the linked list of all file structures */
	if (flisthead == (struct file *) NULL) {
		fp->f_filelist = (struct file *)NULL;
		flisthead = fp;
	} else {
		fp->f_filelist = flisthead;
		flisthead = fp;
	}
	file      = (struct file *) MIN((u_int) fp, (u_int) file); 
	fileNFILE = (struct file *) MAX((u_int) fp, (u_int) fileNFILE); 

#ifdef	OSF1_ADFS
	fp->f_magic = 0;
#endif
	fp->f_count = 0;
	fp->f_type = DTYPE_FREE;
	FP_LOCK_INIT(fp);
	FP_IO_LOCK_INIT(fp);

	return(fp);
}

/*
 * Allocate a file structure for a file.  File descriptors and file
 * table slots are dealt with by the emulation library.
 */

falloc(resultfp)
	struct file **resultfp;
{
	register struct file *fp;

	FREE_FILE_LOCK();
	if (free_file_head.f_freef == &free_file_head) {
		FREE_FILE_UNLOCK();
		fp = file_new();
	} else {
		fp = free_file_head.f_freef;
		ASSERT(fp->f_count == 0 && fp->f_type == DTYPE_FREE);
		free_file_head.f_freef = fp->f_freef;
		FREE_FILE_UNLOCK();
	}
	fp->f_freef = NULL;
	fp->f_type = DTYPE_RESERVED;
	/* Initialize the file structure */
	fp->f_count = 1;
	fp->f_data = 0;
	fp->f_offset = 0;
#if	SER_COMPAT
	fp->f_funnel = 1;
#endif
	crhold(u.u_cred);
	fp->f_cred = u.u_cred;
#ifdef	OSF1_ADFS
	ASSERT(fp->f_magic == 0);
	file_port_allocate(fp);
	fp->f_seqno = 0;
#ifdef	TNC
	fp->f_seqno_lock = NULL;
	fp->f_chain.next = fp->f_chain.prev = NULL;
#endif
#endif	/* OSF1_ADFS */	
	ASSERT(fp->f_count == 1 && fp->f_type == DTYPE_RESERVED);
	*resultfp = fp;
#ifdef PFS
	fp->pfs_offset.shigh = 0;
	fp->pfs_offset.slow = 0;
#endif
	return(0);
}

/*
 * Deallocate a file structure and return it to the free file list
 */

void
fdealloc(fp)
struct file *fp;
{
#ifdef	OSF1_ADFS
#ifndef	TNC
	ASSERT(fp->f_magic == F_MAGIC);
	ASSERT(fp->f_svrsend == 0);
	file_port_deallocate(fp);   /* Get rid of the Mach port for the file */
#else	/* TNC */
	ASSERT(fp->f_magic == F_MAGIC || fp->f_magic == F_RELOC);
	if (fp->f_magic == F_RELOC) {
		FP_LOCK(fp);
		fp->f_magic = 0;
		FP_UNLOCK(fp);
	} else {
		ASSERT(fp->f_svrsend == 0);
		file_port_deallocate(fp);   /* Get rid of the Mach port for the file */
	}
#endif
#endif
	ASSERT(fp->f_cred);
	crfree(fp->f_cred);
	FP_LOCK(fp);
	ASSERT(fp->f_count == 1 && fp->f_type != DTYPE_FREE);
	fp->f_count = 0;
	fp->f_type = DTYPE_FREE;
	fp->f_cred = NULL;
	FP_UNLOCK(fp);
	FREE_FILE_LOCK();
	fp->f_freef = free_file_head.f_freef;
	free_file_head.f_freef = fp;
	FREE_FILE_UNLOCK();
}


/*
 * Do all that is necessary for getting a file structure marked in use.
 * Because of MP locking considerations, this used to have to a function
 * rather than a macro.  It probably can be made a macro again.
 */
getf(fp)
	register struct file *fp;
{
	FP_REF(fp);
#if	SEC_MAC || SEC_NCAV
	audstub_getf(fp);
#endif
	return (0);
}

/*
 * Internal form of close.
 * Decrement reference count on file structure.
 */
closef(fp)
	register struct file *fp;
{
	int error;
#ifdef	TNC
	uthread_t uth = current_thread();

	uth->uu_opn_filep = (mach_port_t)fp;
#endif

	if (fp == NULL)
		return (0);
	/*
	 * SV file locking: clean locks, if present
	 */
	if (fp->f_type == DTYPE_VNODE) {
		int flag;
		BM(VN_LOCK((struct vnode *)fp->f_data));
		flag = ((struct vnode*) (fp->f_data))->v_flag;
		BM(VN_UNLOCK((struct vnode *)fp->f_data));
		if (flag & VLOCKS)
			cleanlocks(fp);
	}
	FP_LOCK(fp);
	if (fp->f_count > 1) {
#ifdef	OSF1_ADFS
		int needwakeup = 0;

		if (fp->f_flag & FUNREFWAIT) {
			fp->f_flag &= ~FUNREFWAIT;
			needwakeup++;
		}
#endif
		fp->f_count--;
		FP_UNLOCK(fp);
#ifdef	OSF1_ADFS
		if (needwakeup)
			thread_wakeup((int)&fp->f_count);
#endif
		return (0);
	}
	FP_UNLOCK(fp);
	if (fp->f_count != 1)
		panic("closef: f_count not 1 fp->f_count=%d", 
		      fp->f_count);
	FOP_CLOSE(fp, error);
	fdealloc(fp);
	return (error);
}


/*
 * Apply an advisory lock on a file descriptor.
 */
/* ARGSUSED */
flock(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		int	how;
	} *uap = (struct args *)args;
	int error = 0;

	if (error = getf(fp))
		return (error);
	if (fp->f_type != DTYPE_VNODE) {
		error = EOPNOTSUPP;
		goto out;
	}
	if (uap->how & LOCK_UN) {
		vn_funlock(fp, FSHLOCK|FEXLOCK);
		goto out;
	}
	if ((uap->how & (LOCK_SH | LOCK_EX)) == 0) {
		goto out;			/* error? */
	}
	if (uap->how & LOCK_EX)
		uap->how &= ~LOCK_SH;
	/* avoid work... */
	FP_LOCK(fp);
	if ((fp->f_flag & FEXLOCK) && (uap->how & LOCK_EX) ||
	    (fp->f_flag & FSHLOCK) && (uap->how & LOCK_SH)) {
		FP_UNLOCK(fp);
		goto out;
	}
	FP_UNLOCK(fp);
	error = vn_flock(fp, uap->how);
out:
	FP_UNREF(fp);
	return (error);
}

void
file_table_init()
{
	struct file	*fp;
	u_int		nfile_conf;

	flisthead = (struct file *) NULL;
	/*
	 * Initialize zone for the file table and record that
	 * the table is empty until zalloc() is called (in file_new()).
	 */
	file_zone = zinit(sizeof(struct file), 0,
			  nfile*sizeof(struct file), "file_table");
	if (file_zone == NULL)
		panic("file_table_init: unable to zinit file zone");
	nfile_conf = nfile;
	nfile = 0;
	file = (struct file *) -1;
	fileNFILE = (struct file *) 0;

	free_file_head.f_freef = &free_file_head;
	for (; nfile_conf > 0; nfile_conf--) {
		fp = file_new();
		fp->f_freef = free_file_head.f_freef;
		free_file_head.f_freef = fp;
	}
	FREE_FILE_LOCK_INIT();
}
