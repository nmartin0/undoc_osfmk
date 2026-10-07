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
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: ushared.h,v $
 * Revision 1.4  1994/11/18  20:42:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:12  cfj
 * Bump major revision number.
 *
 * Revision 2.14  1992/10/06  12:13:54  roman
 * Fix file to be identical with OSF.
 *
 * Revision 2.13  92/07/17  10:06:18  chrisp
 * Add initialization for condition lock_is_free in macro share_lock_init().
 * 
 * Revision 2.12  92/05/20  13:47:46  chrisp
 * Remove cmask in the shared user area, since emulating umask() in this way
 * 	fails to inform the credentials service of the update.
 * 
 * Revision 2.11  92/04/02  11:25:37  roman
 * Get rid of us_ruid and us_rgid fields. They are always in the proc
 * structure.
 * 
 * Revision 2.10  92/03/27  11:23:12  roman
 * Change us_pid, us_ppid from short to pid_t, and us_cursig to char to match
 * 	all other defintions.
 * Remove us_groups entirely.
 * 
 * Revision 2.9  92/03/09  12:19:29  durriya
 * 	[ 92/01/07  23:35:07  condict ]
 * 	Remove unused OSF1_SERVER conditional code.  Also, add initialization of
 * 	timed_out field to share locks.
 * 
 * 	[ 91/12/20  17:57:08  barbou ]
 * 	Added new fields to check the server-emulator consistency.
 * 
 * Revision 2.8  92/01/16  17:11:23  roy
 * 	Wed Jan 15 12:21:13 cfj@ssd.intel.com
 * 	Added the i860 version the the emulator stack pointers.
 * 
 * Revision 2.7  91/12/16  18:27:03  roy
 * 	91/11/15  15:31:54  condict
 * 	Increase number of shared I/O pages from 1 to 2 for performance.
 * 
 * 	91/10/09  18:40:15  emcmanus
 * 	Added emulator's stack pointer and structures for saved registers to
 * 	allow unwinding the stack to user when coredumping.
 * 
 * Revision 2.6  91/11/13  14:44:43  rabii
 * 	[Mon Oct 21 10:03:12 PDT 1991] cfj@ssd.intel.com
 * 	     Added i860 ifdefs.
 * 
 * Revision 2.5  91/10/14  13:09:49  sjs
 * 	91/09/20  19:18:09  barbou
 * 	Define the number of pages used as an I/O buffer between server and 
 *	emulator.
 * 
 * Revision 2.4  91/10/04  15:06:20  chrisp
 * Add in the Locus copyright notice.
 * 
 * Revision 2.3  91/09/17  12:05:08  sjs
 * integrate Locus changes	roman
 * Remove file descriptor table for ushared structure.
 * File descriptors now handled in the emulator.
 * 
 * Revision 2.2  91/08/31  14:16:38  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/09  12:20:41  barbou
 * Added a counter to know how many threads of the user process are in
 * the emulator code (to avoid interrupting these threads badly).
 * 
 * Revision 3.3  91/05/24  11:35:09  jose
 * Moved the us_file_state structure to the end of ushared_ro.
 * 
 * Revision 3.2  91/05/15  17:54:42  barbou
 * Fixed the u_sigtramp/u_sigreturn confusion here too.
 * We need to define _KERNEL to make sure the shared structs are the same on the
 * server and the emulator sides.
 * 
 * Revision 3.1  91/03/08  16:05:38  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.0  91/01/17  12:08:46  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.7  90/11/05  16:56:50  rpd
 * 	Added spin_lock_init.
 * 	[90/10/31            rwd]
 * 
 * Revision 2.6  90/08/06  15:34:03  rwd
 * 	Remove share_lock and share_unlock macros.  These are now
 * 	routines.
 * 	[90/06/08            rwd]
 * 
 * Revision 2.5  90/06/02  15:26:15  rpd
 * 	Added pointer to proc structure as first field of readonly area
 * 	for ease in debugging.
 * 	[90/05/23            rpd]
 * 
 * 	Added us_flag to ushared_ro, to make p_flag shared.
 * 	[90/05/13            rpd]
 * 
 * 	Removed us_sigcheck, us_sigchecklock.
 * 	[90/05/11            rpd]
 * 	Updated for new IPC.
 * 	[90/05/03            rpd]
 * 
 * Revision 2.4  90/05/29  20:24:40  rwd
 * 	Added pointer to proc structure as first field of readonly area
 * 	for ease in debugging.
 * 	[90/05/16            rwd]
 * 	Moved file_info structure to its own file and include it here.
 * 	Moved shared_lock structure to its own file and include it here.
 * 	[90/04/30            rwd]
 * 	Added us_closefile field.
 * 	[90/04/23            rwd]
 * 	Added file info.
 * 	[90/03/16            rwd]
 * 
 * Revision 2.3  90/05/21  13:59:51  dbg
 * 	Always include sigtramp.
 * 	[90/04/23            dbg]
 * 
 * Revision 2.2  90/03/14  21:29:59  rwd
 * 	Added shared memory locks.  Added more sig fields.
 * 	[90/02/15            rwd]
 * 	Created.
 * 	[90/01/23            rwd]
 * 
 */

#ifndef	_SYS_USHARED_H_
#define	_SYS_USHARED_H_

#ifdef	KERNEL
#include <uxkern/import_mach.h>
#else	KERNEL
#include <cthreads.h>
#endif	KERNEL
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/shared_lock.h>
#ifdef i860
#include <../emulator/emul_stack.h>
#endif

#define USHARED_VERSION 2
#define IO_PAGES_NB 2	/* size (in pages) of the shared IO buffer */

#define KERNEL_USER 0x80000000

#define share_lock_init(x)\
    do {\
      spin_lock_init(&(x)->lock);\
      (x)->who = 0;\
      (x)->need_wakeup = 0;\
      (x)->timed_out = 0;\
      condition_init(&(x)->lock_is_free);\
    } while (0)

struct ushared_ro {
	int		us_version;
/* some fields to check if the emulator version matches the server version */
/* these fields are set by the server and checked by the emulator */
	char		us_server_build[81]; 	/* content of version.build */
	int		us_io_pages_nb;		/* number of shared IO pages */
	int		us_rw_size;		/* size of shared RW area */
	int		us_ro_size;		/* size of shared RO area */

	int		us_proc_pointer;
	int		us_mach_nbc;
	int		us_nofile;		/* NOFILE */
/* user fields */
	struct		rlimit us_rlimit[RLIM_NLIMITS];
						/* uu_rlimit */
	caddr_t		us_data_start;		/* uu_data_start */
/* proc fields */
	int		us_flag;		/* p_flag */
	pid_t		us_pid;			/* p_pid */
	pid_t		us_ppid;		/* p_ppid */
	char		us_cursig;		/* p_cursig */
};

struct ushared_rw {
	int		us_inuse;
	int		us_map_files;
	int		us_debug;
	shared_lock_t	us_lock;
/* user fields */
	size_t		us_dsize;		/* uu_dsize */
	struct		sigstack us_sigstack;	/* uu_sigstack */
	sig_t		us_signal[NSIG+1];	/* uu_signal */
	int		us_usigmask[NSIG+1];	/* uu_sigmask */
#if	defined(i386)
	int		(*us_sigreturn)();	/* uu_sigreturn */
#endif	/* defined(i386) */
	int		us_sigonstack;		/* uu_sigonstack */
	int		us_sigintr;		/* uu_sigintr */
#if	defined(balance) || defined(mips) || defined(i860)
	int		(*us_sigtramp)();	/* uu_sigtramp */
#endif	/* defined(balance) || defined(mips) || defined(i860) */
/* proc fields */
	shared_lock_t	us_siglock;		/* p_siglock */
	int		us_sigmask;		/* p_sigmask */
	int		us_sig;			/* p_sig */
	int		us_sigignore;		/* p_sigignore */
	int		us_sigcatch;		/* p_sigcatch */
	int 		us_in_emulator;		/* how many threads in emul */
#ifdef i386
	struct		emul_regs *us_emul_regs;
#endif
#ifdef i860
	struct		emul_stack *us_emul_regs;
#endif
					/* emul sp after saving user regs */
};

/* The emulator saves its initial sp here so that coredumps can dump the user's
   stack accurately, ignoring the emulator.  If there is more than one thread
   this value may be inaccurate, but the coredump format cannot accomodate
   more than one thread anyway. */

#ifdef i386
/*
 * User's registers are stored on partially on the user's stack
 * and partially on the emulator's stack.
 */
struct emul_regs_2 {
	int	edx;
	int	ecx;
	int	eax;
	int	eflags;
	int	eip;
};

struct emul_regs {
	int	ebp;
	int	edi;
	int	esi;
	int	ebx;
	struct emul_regs_2 *uesp;
				/* pointer to rest of user registers */
};

#endif

#ifdef i860

/*
 * User's registers are all stored on the user's stack.
 * The non-volatiles are saved (obviously), as well as
 * the parameters to the syscall (ie, r16-r27).
 *
 * The psr is saved so we can manipulate the CC bit and
 * clear any of the trap bits.
 *
 * Ordinarily, we wouldn't need to preserve r16-r27, but they contain
 * the parameters for the system call and the machine indep. code
 * would really like them spread out in memory.
 *
 * r28 follows i860 PRM convention for structure passing.
 *
 * r29 is magic in that it contains the address of the instruction
 * following the "trap" instruction -- we need to know where to return to.
 * The trap handler (i860/trap.c) saves the value of (fir + 4) in r29.
 * The original contents of the user's r29 are lost way back in the
 * trap handler, but it's defined to be volatile anyway.  See i860/trap.c
 * in the kernel.  (all because the i860 doesn't have a user-visible pc).
 *
 * r31 contains the syscall number.
 *
 * The non-volatile FP regs are saved as a precaution and might be
 * removed in the future during tuning.
 *
 * DON'T EVEN THINK ABOUT REARRANGING THIS WITHOUT CONSULTING
 * emul_vector.s and sys/signal.h !!
 */
struct emul_exception_frame {
	unsigned	onstack;	/* see sigcontext */
	unsigned	mask;
	unsigned	u_r0;		/* always 0 */
	unsigned	u_r1;		/* user's return address */
	unsigned	u_sp;		/* stack pointer */
	unsigned	u_fp;		/* frame pointer */
	unsigned	u_r4;		/* non-volatile */
	unsigned	u_r5;		/* non-volatile */
	unsigned	u_r6;		/* non-volatile */
	unsigned	u_r7;		/* non-volatile */
	unsigned	u_r8;		/* non-volatile */
	unsigned	u_r9;		/* non-volatile */
	unsigned	u_r10;		/* non-volatile */
	unsigned	u_r11;		/* non-volatile */
	unsigned	u_r12;		/* non-volatile */
	unsigned	u_r13;		/* non-volatile */
	unsigned	u_r14;		/* non-volatile */
	unsigned	u_r15;		/* non-volatile */
	unsigned	u_r16;		/* syscall param  1 */
	unsigned	u_r17;		/* syscall param  2 */
	unsigned	u_r18;		/* syscall param  3 */
	unsigned	u_r19;		/* syscall param  4 */
	unsigned	u_r20;		/* syscall param  5 */
	unsigned	u_r21;		/* syscall param  6 */
	unsigned	u_r22;		/* syscall param  7 */
	unsigned	u_r23;		/* syscall param  8 */
	unsigned	u_r24;		/* syscall param  9 */
	unsigned	u_r25;		/* syscall param 10 */
	unsigned	u_r26;		/* syscall param 11 */
	unsigned	u_r27;		/* syscall param 12 */
	unsigned	u_r28;		/* for structure passing or spills */
	unsigned	u_r29;		/* emulator will return thru here */
	unsigned	u_r30;		/* volatile */
	unsigned	u_r31;		/* system call number */
	unsigned	u_f0;		/* always 0 */
	unsigned	u_f1;		/* always 0 */
	unsigned	u_f2;		/* non-volatile */
	unsigned	u_f3;		/* non-volatile */
	unsigned	u_f4;		/* non-volatile */
	unsigned	u_f5;		/* non-volatile */
	unsigned	u_f6;		/* non-volatile */
	unsigned	u_f7;		/* non-volatile */
	unsigned	u_psr;		/* processor status reg */
	unsigned	u_fsr;		/* floating-point status reg */
	unsigned	u_epsr;		/* extended processor status reg */
	unsigned	u_fir;		/* pc, sometimes... */
	unsigned	__pad[2];	/* unused -- 16-byte alignment */
};


/*
 * following structure is KNOWN by assembly language preceeding the call
 * to emul_syscall() in emul_vector.s. 'unaligned_emul_stack_ptr' is what
 * 'emul_stack_alloc()' returned before we i860 (16-byte) stack-aligned it.
 */
struct emul_stack {
	struct emul_exception_frame	*usp;
	int				unaligned_emul_stack_ptr;
	struct emul_stack_top		unused;
};
#endif

#endif	_SYS_USHARED_H_
