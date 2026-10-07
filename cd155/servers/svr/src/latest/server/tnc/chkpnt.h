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
 * HISTORY
 * $Log: chkpnt.h,v $
 * Revision 1.3  1995/02/01  21:39:17  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.2  1994/11/18  20:42:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  02:04:13  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 2.3  93/11/10  12:07:09  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.2  93/07/20  10:10:20  chrisp
 * 	Introduce another restart option, RESTART_NOSTOP, to prevent
 * 	the stop/continue interaction during exec_restart().
 * 
 * Revision 2.1.1.1  93/06/10  11:52:22  chrisp
 * 	Revision 3.13  93/06/04  11:21:18  chrisp
 * 	CHKPNT_ASYNC and RESTART_NOMIGRATE options added.
 * 	Further negative SIGMIGRATE argument values added for kill after
 * 		checkpointing and synchronous options.
 * 	Machine-dependent user state macros added to derive user stack address
 * 		and "fake kernel stack".
 * 	Signal trampoline added to emulator state saved in state file.
 * 
 * 	Revision 3.12  93/05/19  10:40:07  chrisp
 * 	Add MACRO_BEGIN/MACRO_END brackets to debugging macros.
 * 	Add restart() option RESTART_EXECROOT to exec_restart() the root process
 * 		in a checkpoint directory if such exists. [The root process is the
 * 		only process without a parent amongst the checkpointed processes.]
 * 	Remove function prototype declarations.
 * 
 * 	Revision 3.11  93/05/11  15:32:52  hao2
 * 	Add option so the restarted process group does not setpgrp() to the
 * 	chkpnted pgid.
 * 
 * 	Revision 3.10  93/04/29  08:22:07  chrisp
 * 	Add chkpnt_notify_t for saving user on_checkpoint/on_restart routines.
 * 	Introduce RESTART_DBG() macro for debugging restart library module.
 * 
 * 	Revision 3.9  93/04/26  12:33:23  chrisp
 * 	Add RESTART_STOP as option to exec_restart().
 * 
 * 	Revision 3.8  93/04/22  08:48:27  chrisp
 * 	Bump the version number - filenaming has changed.
 * 
 * 	Revision 3.7  93/04/17  13:15:47  chrisp
 * 	Change IS_CTTY() macro to take pointers and to use fstat.st_spare1 directly.
 * 	Add conditional code so that user-level #defines are available in isolation
 * 		if _KERNEL is undefined.
 * 
 * 	Revision 3.6  93/04/16  12:16:02  hao2
 * 	Changed IS_CTTY() to use pointers.
 * 
 * 	Revision 3.5  93/04/12  15:17:35  chrisp
 * 	Changes in preparation to restarting controlling terminal devices.
 * 
 * 	Revision 3.4  93/04/08  11:51:26  chrisp
 * 	Add RESTART_SIGALL: the first restart option - there'll be others.
 * 	Add THREAD_STATE_COUNT which is a machine-dependent definition.
 * 	Add verison and fs_size fields to proc_state structure.
 * 	Add ctty_dev in file_state (as well as proc_state).
 * 
 * 	Revision 3.3  93/03/29  13:28:32  chrisp
 * 	Add fmode to file state.
 * 
 * 	Revision 3.2  93/03/19  09:42:26  chrisp
 * 	Define THREAD_STATE_T to be the appropriate machine-dependent thread
 * 		state structure (since thread_state_t is a fake int *).
 * 
 * 	Revision 3.1  93/03/16  07:33:16  chrisp
 * 	Additions and amendments for emulator file state information and for
 * 		server state for checkpointing.
 * 
 * 	Revision 3.0  93/02/17  10:44:24  chrisp
 * 	First draft.
 * 
 * 	$EndLog$
 * 
 */

#ifndef	_H_TNC_CHKPNT
#define	_H_TNC_CHKPNT

/*
 * POSIX-maybe-defined constants:
 */
#define	CHKPNT_PROC		0x00	/* checkpoint single process */
#define	CHKPNT_FAMILY		0x01	/* checkpoint process group */
#define	CHKPNT_KILL		0x02	/* kill checkpointed processes */
#define	CHKPNT_ASYNC		0x04	/* checkpoint asynchronously */

#define	RESTART_SIGALL		0x01	/* continue restarted process */

#define	RESTART_NOSETPGID	0x02	/* don't reset process pgid */
#define	RESTART_EXECROOT	0x04	/* if there's a root process, exec */
#define	RESTART_NOMIGRATE	0x08	/* don't migrate from origin node */
#define	RESTART_NOSTOP		0x10	/* don't stop restarted process */

#define	RESTARTEXEC_STOP	0x01	/* stop restarted process */

#define	SIGCHKPNT_PROC		-1	/* checkpoint process by signal */
#define	SIGCHKPNT_PGRP		-2	/* checkpoint process grp by signal */
#define	SIGCHKPNT_KILL_PROC	-3	/* checkpoint and kill process */
#define	SIGCHKPNT_KILL_PGRP	-4	/* checkpoint and kill process grp */
#define	SIGCHKPNT_SYNC_PROC	-5	/* checkpoint process synchronously */
#define	SIGCHKPNT_SYNC_PGRP	-6	/* checkpoint pgrp asynchronously  */

#define	FD_UNUSED		-1	/* unused FDT slot */

#define	TNC_CHKPNT_DEBUG
#ifdef	TNC_CHKPNT_DEBUG
#include <kern/macro_help.h>
extern int emul_chkpnt_debug;
#define	RESTART_DEBUG	0x80000000
#define	E_CHKPNT_DEBUG(blurb)		\
	if (emul_chkpnt_debug)		\
		EPRINT(blurb)
#ifdef	__STDC__
#define	RESTART_DBG(flags,blurb)			\
	MACRO_BEGIN					\
	if (flags & RESTART_DEBUG) printf##blurb;	\
	MACRO_END
#else
#define	RESTART_DBG(flags,blurb)			\
	MACRO_BEGIN					\
	if (flags & RESTART_DEBUG) printf/**/blurb/**/;	\
	MACRO_END
#endif
#else	/* TNC_CHKPNT_DEBUG */
#define	E_CHKPNT_DEBUG(blurb)
#define	RESTART_DBG(flags,blurb)
#endif	/* TNC_CHKPNT_DEBUG */

#ifdef	_KERNEL

#include <sys/stat.h>
#include <sys/mount.h>
#include <tnc/tnc_types.h>
#include <tnc/rtask.h>

typedef	struct mi_data MI_DATA_T;

/*
 * The following are machine-dependent definitions concerning user process
 * thread state and the derivation of fake kernel stack information.
 */
#ifdef	i386
#include <i386/reg.h>
typedef	struct i386_thread_state THREAD_STATE_T;
#define	THREAD_STATE_COUNT	i386_THREAD_STATE_COUNT
#define	NFAKEREGS		19
#define	FAKE_KERNEL_REGS(tsp,regs,size)				\
	size = NFAKEREGS * sizeof(int);				\
	if ((regs = (caddr_t) kalloc(size)) != NULL)	 {	\
		bzero((caddr_t)regs, size);			\
		((int *) regs)[SS]   = tsp->ss;			\
		((int *) regs)[UESP] = tsp->uesp;		\
		((int *) regs)[EFL]  = tsp->efl;		\
		((int *) regs)[CS]   = tsp->cs;			\
		((int *) regs)[EIP]  = tsp->eip;		\
		((int *) regs)[EAX]  = tsp->eax;		\
		((int *) regs)[ECX]  = tsp->ecx;		\
		((int *) regs)[EDX]  = tsp->edx;		\
		((int *) regs)[EBX]  = tsp->ebx;		\
		((int *) regs)[ESP]  = tsp->esp;		\
		((int *) regs)[EBP]  = tsp->ebp;		\
		((int *) regs)[ESI]  = tsp->esi;		\
		((int *) regs)[EDI]  = tsp->edi;		\
		((int *) regs)[DS]   = tsp->ds;			\
		((int *) regs)[ES]   = tsp->es;			\
		((int *) regs)[FS]   = tsp->fs;			\
		((int *) regs)[GS]   = tsp->gs;			\
	}
#define	USER_STACK_POINTER(tsp)	((vm_address_t) tsp->uesp)
#elif	i860
#include <i860/reg.h>
typedef	struct i860_thread_state THREAD_STATE_T;
#define	THREAD_STATE_COUNT	i860_THREAD_STATE_COUNT
#define	NFAKEREGS		(sizeof(struct i860_thread_state)/sizeof(int))
#define	FAKE_KERNEL_REGS(ts,regs,size)				\
	size = NFAKEREGS * sizeof(int);				\
	if ((regs = (caddr_t) kalloc(size)) != NULL) {		\
        	bcopy(&(ts->r0), &((int *)regs)[R0], size);	\
	}
#define	USER_STACK_POINTER(tsp)	((vm_address_t) tsp->sp)
#endif

typedef struct file_info {
	off_t		offset;			/* file offset (seek ptr) */
	int		fmode;			/* access/status flags */
	struct stat	fstat;			/* stat of file */
	union {
		struct statfs	fstatfs;	/* fstatfs of filesystem */
		struct devstat	devstat;	/* device info for char files */
	} fs_or_dev;
} file_info_t;

typedef struct ctty_info {			/* controlling tty info */
	dev_t	device;				/* device (major,minor) */
	node_t	node;				/* node hosting device */
} ctty_info_t;

typedef struct chkpnt_proc_state {
	/*
	 * Derived from migrate state data
	 */ 
	pid_t		pid;
	pid_t		ppid;
	pid_t		pgid;
	pid_t		sid;
	ctty_info_t	ctty;
	THREAD_STATE_T	thread_state;
	MI_DATA_T	mi_data;	/* includes ppid, pgid etc */
	command_name_t	command_name;
	logname_t	logname;
	unsigned int	fs_size;
	int		version;
#define	CHKPNT_FILE_VERSION	2	/* identifies chkpnt file format */
} chkpnt_proc_state_t;

typedef struct on_routines {
	int (*on_checkpoint)();
	int (*on_restart)();
} chkpnt_notify_t;

typedef struct chkpnt_file_state {
	path_name_t	exec_fname;
	file_info_t	exec_rootdir;
	file_info_t	exec_currentdir;
	file_info_t	rootdir;
	file_info_t	currentdir;
	ctty_info_t	ctty;		/* controlling tty at time of chkpnt */
	chkpnt_notify_t	on_routines;
	caddr_t		signal_trampoline;
	int		n_open_files;
	int		fdt[OPEN_MAX];
	file_info_t	file_state[OPEN_MAX];
} chkpnt_file_state_t;

#define	CHKPNT_FILE_STATE_SIZE(fsp)					\
	sizeof(chkpnt_file_state_t)					\
		- (OPEN_MAX - (fsp)->n_open_files) * sizeof(file_info_t)

#define	IS_CTTY(fip,cip)						\
	((((fip)->fstat.st_mode & S_IFMT) == S_IFCHR) &&		\
	 ((fip)->fstat.st_rdev == (cip)->device) &&			\
	 ((fip)->fstat.st_spare1 == (cip)->node))

struct chkpnt_state {
	chkpnt_proc_state_t	cps;
	chkpnt_file_state_t	cfs;
};

#endif	/* _KERNEL */
#endif	/* _H_TNC_CHKPNT */
