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
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: mach_core.c,v $
 * Revision 1.8  1994/11/18  20:27:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/18  00:01:27  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.6  1994/01/13  17:52:49  jlitvin
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
 * Revision 1.5  1993/07/14  17:48:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:48:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/07  19:02:19  nandy
 * Fixed merge conflict.
 *
 * Revision 1.3  1993/05/06  19:04:21  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:24:44  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1  1992/11/06  00:06:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:07:40  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/10/14  17:15:38  cfj
 * NX integration.
 *
 * Revision 2.3  1991/12/17  13:35:53  roy
 * 	In fake_u(), don't initialize fields of struct user.h that no
 * 	longer exist. (loverso)
 * 
 * 	91/12/13  13:02:51  sp
 * 	Remove old BSD code, CMUCS and VICE conditionals and old dead BSD
 * 	code.
 *
 * 	91/10/17  18:32:44  barbou
 * 	Deleted include of "cmucs.h" (defunct option).
 *
 * 	91/10/09  18:35:04  emcmanus
 * 	Implemented coredump for server.
 *
 * Revision 2.2  91/08/31  13:22:39  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/06/25  16:58:50  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.3  91/06/07  15:49:48  barbou
 * Implemented fake_u().
 * 
 * Revision 3.2  91/03/08  15:51:50  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:52:45  condict
 * Turn off VICE.
 * 
 * Revision 3.0  91/01/17  12:11:35  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:21:35  rpd
 * 	No longer necessary to undefine u_arg.
 * 	[90/05/13            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  19:35:31  rpd]
 * 
 * Revision 2.1  89/08/04  14:06:31  rwd
 * Created.
 * 
 * 20-Jul-88  David Golub (dbg) at Carnegie-Mellon University
 *	Moved machine-dependent routines to machine/bsd_machdep.c
 *
 * 27-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created to replace kern_sig/core().  Moved fake_u here from old
 *	file.
 *
 */

/*
 *	bsd/mach_core.c
 *
 *	Routines to fake the old BSD U-area structure.  Includes
 *	core dump.
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <ufs/inode.h>
#include <sys/file.h>
#include <sys/acct.h>
#include <sys/uio.h>

#include <kern/parallel.h>

#include <uxkern/import_mach.h>

/*
 * Create a core image for a process.  It does not have
 * to be the current process.
 *
 * Must be called from a U*X process, so that there is
 * a u-area to make point to the proc structure.
 */

int core_dump(p)
	register struct proc *p;
{
	struct proc *save_proc;
	int corestatus;

	save_proc = u.u_procp;
	u.u_procp = p;
	corestatus = core();
	u.u_procp = save_proc;
	return corestatus;
}

/*
 *	fake_u:
 *
 *	fake a u-area structure for the specified thread.  Only "interesting"
 *	fields are filled in.
 *
 *	Must be last in the file, since it undefines many u-area access
 *	macros.
 */

fake_u(up, p, thread)
	register struct user	*up;
	struct proc		*p;
	register thread_t	thread;
{
	register struct utask	*utask;
	register struct uthread *uthread;
	time_value_t	sys_time, user_time;
	register int	i;

	utask = &p->p_utask;
	uthread = &u;

#undef u_pcb
	bzero((caddr_t)&up->u_pcb, sizeof(struct pcb));
#undef u_comm
	bcopy(utask->uu_comm, up->u_comm, sizeof(up->u_comm));
#undef u_tsize
	up->u_tsize = utask->uu_tsize;
#undef u_dsize
	up->u_dsize = utask->uu_dsize;
#undef u_ssize
	up->u_ssize = utask->uu_ssize;
#undef u_signal
	for (i = 0; i < NSIG; i++) {
		if (thread_signal_disposition(i))
		  up->u_signal[i] = uthread->uu_tsignal[i];
		else
		  up->u_signal[i] = utask->uu_signal[i];
	}
#undef u_code
	up->u_code = uthread->uu_code;
#undef u_procp
	up->u_procp = utask->uu_procp;
/* do not #undef u_arg */
	up->u_arg[0] = utask->uu_procp->p_cursig;
#undef u_ru
	up->u_ru = utask->uu_ru;
#undef u_acflag
	up->u_acflag = utask->uu_acflag;
#undef u_cmask
	up->u_cmask = utask->uu_cmask;
#undef	u_start
        up->u_start = utask->uu_start;
#undef u_logname
	bcopy(utask->uu_logname, up->u_logname, MAXLOGNAME - 1);

	/*
	 *	Times aren't in uarea any more.
	 */
	thread_read_times(thread, &user_time, &sys_time);
	up->u_ru.ru_stime.tv_sec = sys_time.seconds;
	up->u_ru.ru_stime.tv_usec = sys_time.microseconds;
	up->u_ru.ru_utime.tv_sec = user_time.seconds;
	up->u_ru.ru_utime.tv_usec = user_time.microseconds;
#undef	u_cru
	up->u_cru = utask->uu_cru;
}
