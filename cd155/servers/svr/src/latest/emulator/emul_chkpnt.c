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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: emul_chkpnt.c,v $
 * Revision 1.6  1995/02/18  01:17:07  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12240, including emul console logging cleanup
 *  Testing: EATs controlc, sched
 *  Module(s):
 * 	svr/emulator/bsd_user_side.c
 * 	svr/emulator/emul_chkpnt.c
 * 	svr/emulator/emul_init.c
 * 	svr/emulator/emul_mapped.c
 * 	svr/emulator/fsvr_user_side.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/bsd/mach_signal.c
 * 	svr/server/bsd/subr_prf.c
 * 	svr/server/conf/makesyscalls.sh
 * 	svr/server/tnc/dvp_vpops.c
 * 	svr/server/uxkern/boot_config.c
 * 	svr/server/uxkern/bsd_server_side.c
 * 	svr/server/uxkern/credentials.c
 * 	svr/server/uxkern/rpm_clock.c
 *
 * General cleanup of emulator console logging.  Added bootnode_printf()
 * routine to server.  Added server bootmagic variable ENABLE_RPM_TIMESTAMP
 * so printf() and bootnode_printf() messages are timestamped with the
 * 56-bit RPM global clock value.  This enables very fine timings to be
 * observable in console log output.
 *
 * Revision 1.5  1995/02/10  18:00:13  toman
 * Added fdt_port_modref() call to release references in e_rforkmulti_call()
 * and e_forkfamily_call() in the case when memory allocation fails.
 *
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 12392
 *  Testing: By inpsection.
 *  Module(s): emulator/bsd_user_side.c
 *             emulator/emul_chkpnt.c
 *
 * Revision 1.4  1995/02/01  21:21:55  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.3  1994/11/18  20:23:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/06/02  22:11:03  chrisp
 * e_rforkmulti_call() and e_forkfamily_call() call new routines
 * fdt_get_rights() and fdt_port_modrefs() to assemble a table of file
 * ports to be transferred to the server. Note that the first 2 entries
 * in this table are the parent's root and current directory ports.
 * Explicit installation of these ports into child tasks and the
 * release of child emulator threads has been eliminated.  Fileserver
 * RPC fsvr_file_ref() now takes an extra parameter giving the reference
 * adjustment required.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): bsd_user_side.c emul_chkpnt.c fsvr_user_side.c pfs2_user_side.c
 *
 * Revision 1.1  1994/03/14  01:43:48  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 *  Module(s):
 *
 * Revision 2.3  93/11/10  12:06:45  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.5  93/08/13  13:10:07  hao
 * 	Kludged here to reset seekpointer to 0 if it is -1.  This is done
 * 	to avoid e_lseek() returning EINVAL, thus cause restart to fail.
 * 
 * Revision 2.1.1.4  93/07/13  07:14:12  chrisp
 * 	Add special-case logic to re-open unlinked, zero-length files upon
 * 		restart. Such transient files exist for ksh (and possibly other
 * 		shells).
 * 	Eliminate the "fd" return parameter from fh_fdt_reserve_slot() [now renamed
 * 		restart_fdt_reserve_slot()] since this is precisely the fdt_slot
 * 		specified by the first parameter. Change fdt_slot -> fd generally.
 * 	Change re-open mode adjustment (+FOPEN) to work without mapped files.
 * 	Add emul_blocking()/emul_unblocking() calls around bsd_proc_exit() to
 * 		avoid deadlock with the callback thread.
 * 
 * Revision 2.1.1.3  93/06/25  08:03:24  chrisp
 * 	Revision 3.17  93/06/15  14:57:48  hao2
 * 	Added check so a traced program can not be chkpnted.
 * 
 * Revision 2.1.1.2  93/06/22  11:54:54  chrisp
 * 	AD1.03 merge.
 * 
 * Revision 2.1.1.1  93/06/10  11:43:13  chrisp
 * 	Revision 3.16  93/06/04  11:12:03  chrisp
 * 	Some general tidy-up, addition of forward references etc.
 * 	chkpnt_async() renamed chkpnt_self() and this now takes option flags.
 * 	e_chkpnt_async() renamed emul_chkpnt_async() since this is no longer a
 * 		system call. This routine now translates the checkpoint argument
 * 		into an option flag value before calling into the server.
 * 	Emulator signal trampoline state now saved in and restored from the
 * 		file state structure.
 * 	RESTART_STOP option value renamed RESTARTEXEC_STOP.
 * 	File re-opening warnings made conditional on TNC_CHKPNT_DEBUG.
 * 
 * 	Revision 3.15  93/05/25  09:24:00  chrisp
 * 	Check return value from user's on_checkpoint() routine and abandon
 * 		checkpoint if non-zero.
 * 
 * 	Revision 3.14  93/05/20  14:31:53  hao2
 * 	Changed macro coding style.
 * 
 * 	Revision 3.13  93/05/19  10:35:49  chrisp
 * 	Add block comments and address sundry code review points.
 * 	Remove machine-dependent i860 assignment of sigtramp.
 * 
 * 	Revision 3.12  93/05/17  09:32:25  chrisp
 * 	Reset on_checkpoint/on_restart routines on exec().
 * 	Restore emulator global variable sigtramp for the 860.
 * 
 * 	Revision 3.11  93/04/30  13:01:51  chrisp
 * 	Remove stop action for e_exec_restart_call and pass a flag to the
 * 		calling machine-dependent code so that the stop can be delayed
 * 		until the user stack is about to be returned to.
 * 
 * 	Revision 3.10  93/04/29  08:09:48  chrisp
 * 	Correct interface to on_checkpoint() and on_restart() and save user-callbacks
 * 		in the state file.
 * 	Implement a fully functional forkfamily().
 * 
 * 	Revision 3.9  93/04/27  14:07:01  hao2
 * 	Fixed code so the rootdir_port is not deallocated.
 * 
 * 	Revision 3.8  93/04/26  12:32:52  chrisp
 * 	Use RESTAR_STOP flag value rather than overloading RESTART_SIGALL.
 * 
 * 	Revision 3.7  93/04/26  10:37:21  hao2
 * 	When re-opening a file, make sure the mode is no longer FTRUNC.
 * 
 * 	Revision 3.6  93/04/19  16:19:05  hao2
 * 	Added file descriptor number to emulator messgaes on restarting the fdt.
 * 	Added checks for f_fsid when comparing statfs.  Restored saved 
 * 	exec'ed filename and root/current directory to the restarted process.
 * 
 * 	Revision 3.5  93/04/17  13:09:55  chrisp
 * 	Complete forkfamily() for single process case.
 * 	Complete exec_restart().
 * 	Integrate file re-opening code from emul_restart.c.
 * 
 * 	Revision 3.4  93/04/16  12:09:11  hao2
 * 	Changed emul_restart_fdt() to extern (in emul_restart.c).
 * 
 * 	Revision 3.3  93/04/12  15:10:57  chrisp
 * 	Re-organize in preparation for file restarts - especially ctty issues.
 * 
 * 	Revision 3.2  93/04/08  11:45:18  chrisp
 * 	Implement exec_restart(); add forkfamily() stub; add emul_restart_fdt() stub.
 * 	Checkpoint mods: save fmode when checkpinting and fsync() files open for
 * 		writing.
 * 
 * 	Revision 3.1  93/03/16  07:31:51  chrisp
 * 	Add emulator state checkpointing logic.
 * 
 * 	Revision 3.0  93/02/17  10:44:08  chrisp
 * 	First draft.
 * 
 * 	$EndLog$
 * 
 */
/*
 * Emulator support for checkpoint/restart.
 */

#ifdef CHKPNT

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/syscall.h>
#include <sys/signal.h>
#include <sys/file.h>
#include <uxkern/bsd_1.h>
#include <uxkern/bsd_types.h>
#include <tnc/chkpnt.h>
#include "emul.h"
#include "fdt.h"

#include <kern/macro_help.h>
#define	TNC_CHKPNT_DEBUG 1
#ifdef	TNC_CHKPNT_DEBUG
#define	EDEBUG EPRINT
#else
#define	EDEBUG(x)	/* Null */
#endif

/*
 * Forward references:
 */
void make_fh(
	file_info_t	*fip,
	struct fhandle	*fhp);
void close_fdt(
	boolean_t	*interrupt);
int emul_chkpnt_fdt(
	int		*interrupt,
	int		*chkpnt_n_open_files,
	int		chkpnt_fdt[],
	file_info_t	chkpnt_fs[]);
int emul_chkpnt_getfstats(
	int		*interrupt,
	int		fdes,
	file_info_t	*fi);
int emul_chkpnt_getstats(
	int		*interrupt,
	char		*path,
	file_info_t	*fip);
int compare_stat(
	struct stat	*fstat1,
	struct stat	*fstat2,
	int		fd);
int compare_statfs(
	struct statfs	*fstatfs1,
	struct statfs	*fstatfs2,
	int		fd);
int emul_open_transient(
	int		*interrupt,
	int		fd,
	int		mode,
	int		crtmode);

/*
 * Emulator globals for checkpoint/restart:
 */
static chkpnt_notify_t chkpnt_user_call;	/* on_checkpoint/on_restart */
static struct {
	path_name_t	fname;
	int		fname_len;
	file_info_t	rootdir_fs;
	file_info_t	currentdir_fs;
} chkpnt_exec_state;				/* state saved on exec() */
#ifdef	TNC_CHKPNT_DEBUG
int	emul_chkpnt_debug = 0;
#endif	/* TNC_CHKPNT_DEBUG */

/*
 * Here to checkpoint ourselves; derive the file state and passed this
 * to the server to be checkpointed to disc along with the process state.
 */
int
emul_chkpnt_self(serv_port, interrupt, arg, new_state, new_state_count, tramp)
	mach_port_t		serv_port;
	boolean_t		*interrupt;	/* OUT */
	int			arg;
	thread_state_t		new_state;
	unsigned int		new_state_count;
	int			(*tramp)();
{
	int			error;
	int			i;
	kern_return_t		ret;
	thread_array_t		thread_list;
	mach_msg_type_number_t	thread_count;
	chkpnt_file_state_t	*fs;

	/*
	 * If the program is being debugged, chkpnt has no choice
	 * but fail.
	 */
	if (chkpnt_exec_state.fname[0] == '\0') 
		return(ENOSYS);
	/*
	 * Currently, we are unable to checkpoint a multi-threaded process,
	 * due to limitations in the logic for thread_abort(). Hence we
	 * give up here if the process is multithreaded. Note that
	 * multithreaded means > 2 threads, since we must allow for
	 * the emulator callback thread, which always exists.
	 */
	ret = task_threads(mach_task_self(), &thread_list, &thread_count);
	if (ret != KERN_SUCCESS)
		emul_panic("emul_chkpnt_self: task_threads failure");
	for (i = 0; i < thread_count; i++)
		(void) mach_port_deallocate(mach_task_self(), thread_list[i]);
	ret = vm_deallocate(mach_task_self(), 
			   (vm_address_t) thread_list,
			   thread_count * sizeof(*thread_list));
	if (ret != KERN_SUCCESS)
		emul_panic("emul_chkpnt_self: vm_deallocate failure");
#ifdef NX
        {
        extern boolean_t nx_application;

        if (thread_count > (nx_application ? 3 : 2))
                return(ENOSYS);
        }
#else
        if (thread_count > 2)
                return(ENOSYS);
#endif /* NX */

	/*
	 * If necessary, let the user do their own thing but check the
	 * return code and skip the checkpoint if we get a non-zero value.
	 */
	if (chkpnt_user_call.on_checkpoint != NULL)
		if ((*chkpnt_user_call.on_checkpoint)() != 0)
			return(EAGAIN);

	/*
	 * Capture file state info for all open files.
	 */
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &fs,
			  sizeof(chkpnt_file_state_t),
			  TRUE);
	if (ret != KERN_SUCCESS)
		return(ENOMEM);
	error = emul_chkpnt_fdt(interrupt,
				&fs->n_open_files,
				fs->fdt,
				fs->file_state);
	if (error)
		goto out;

	/*
	 * Copy the saved exec'ed filename and root/current directory
	 * context into the file state. Also save the user-callbacks and
	 * signal trampoline routine address stored in the emulator.
	 */
	strcpy(&fs->exec_fname, &chkpnt_exec_state.fname);
	fs->exec_rootdir      = chkpnt_exec_state.rootdir_fs;
	fs->exec_currentdir   = chkpnt_exec_state.currentdir_fs;
	fs->on_routines       = chkpnt_user_call;
	fs->signal_trampoline = (caddr_t) tramp;

	/*
	 * Capture the root, working directory and controlling terminal context.
	 */
	error = emul_chkpnt_getstats(interrupt, "/", &fs->rootdir);
	if (error)
		goto out;
	error = emul_chkpnt_getstats(interrupt, ".", &fs->currentdir);
	if (error)
		goto out;

	/* 
	 * Now it's time for the server to capture process state.
	 * Hand it all the emulator state needed for restart.
	 * Note that we pass over only as much file state information
	 * as there are open files.
	 */
	{
		int	options;

		/*
		 * Translate signal argument into checkpoint options.
		 */
		switch (arg) {
		case SIGCHKPNT_PROC:
			options = CHKPNT_PROC | CHKPNT_ASYNC; break;
		case SIGCHKPNT_PGRP:
			options = CHKPNT_FAMILY | CHKPNT_ASYNC; break;
		case SIGCHKPNT_KILL_PROC:
			options = CHKPNT_PROC | CHKPNT_KILL; break;
		case SIGCHKPNT_KILL_PGRP:
			options = CHKPNT_FAMILY | CHKPNT_KILL; break;
		case SIGCHKPNT_SYNC_PROC:
			options = CHKPNT_PROC; break;
		case SIGCHKPNT_SYNC_PGRP:
			options = CHKPNT_FAMILY; break;
		}

		emul_blocking();
		error = bsd_chkpnt_self(vproc_port, interrupt,
					options,
					emul_tnc_mynode(),
					new_state, new_state_count,
					(char *) fs,
					CHKPNT_FILE_STATE_SIZE(fs));
		emul_unblocking();
	}

out:
	vm_deallocate(mach_task_self(),
		      (vm_address_t) fs,
		      (vm_size_t) sizeof(chkpnt_file_state_t));
	return(error);
}

/*
 * This routine calls the server to perform a forkfamily() system call.
 * forkfamily() is like an rforkmulti() except that the child pids are
 * know in advance.
 */
int
e_forkfamily_call(
	mach_port_t	serv_port,
	boolean_t	*interrupt,	/* OUT */
	int		nproc,
	pid_t		pid_array[],
	thread_state_t	new_state,
	unsigned int	new_state_count,
	int		*rvalp)
{
	unsigned int		out_rval_count;
	unsigned int		nfile_ports;
	mach_port_t		*file_port_array;
	register int		i;
	int			error;
	kern_return_t		ret;

	if (!user_rcheck(pid_array, nproc * sizeof(pid_t)))
		return(EFAULT);

	/*
	 * Duplication of the address space and insertion of file port
	 * rights must be done atomically.
	 */
	fdt_atomic_begin();

	/*
	 * Before giving away port rights, we must contact the fileservers
	 * to keep the bookkeeping straight.
	 */
	fdt_port_modref(nproc);

	/*
	 * Get a list of all file ports to be conferred to the children.
	 * This is a two-pass process: first count then get into an array
	 * of adequate size. Note that this list will appear in the
	 * subsequent RPC to the server twice: as names and as rights.
	 */
	nfile_ports = fdt_get_rights(NULL, 0);
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &file_port_array,
			  (nfile_ports + 2) * sizeof(mach_port_t), TRUE);
	if (ret != KERN_SUCCESS) {
		fdt_port_modref(-nproc);
		fdt_atomic_end();
		return(ENOMEM);
	}
	file_port_array[0] = rootdir_port;
	file_port_array[1] = currentdir_port;
	(void) fdt_get_rights(file_port_array + 2, nfile_ports);
	nfile_ports += 2;

	/*
	 * Create the children. Use the short in-line form of the call
	 * if possible.
	 */
	if (nproc <= MAX_MULTI_LIST_SIZE &&
	    nfile_ports <= MAX_MULTI_LIST_SIZE) {
		error = bsd_forkfamily(vproc_port, interrupt,
				       pid_array, nproc,
				       file_port_array, nfile_ports,
				       file_port_array, nfile_ports,
				       new_state, new_state_count,
				       vproc_port,
				       credentials_port);
	} else {
		error = bsd_forkfamily_long(vproc_port, interrupt,
				            pid_array, nproc,
				            file_port_array, nfile_ports,
				            file_port_array, nfile_ports,
				            new_state, new_state_count,
				            vproc_port,
				            credentials_port);
	}
	if (error != ESUCCESS) {
		/*
		 * Adjust server's file port count if children weren't forked
		 */
		fdt_port_modref(-nproc);
	}
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) file_port_array,
			     (nfile_ports+2)*sizeof(mach_port_t));
	fdt_atomic_end();

	rvalp[0] = 1;
	rvalp[1] = 0;
	return(error);
}

/*
 * User wants to be called before checkpointing.
 */
int
e_on_checkpoint(
	mach_port_t	serv_port,
	boolean_t	*interrupt,	/* OUT */
	caddr_t		user_routine)
{
	/*
	 * Check given address is in user space.
	 */
	if (user_routine && !user_rcheck(user_routine, sizeof(caddr_t)))
		return(EFAULT);

	/*
	 * Record address in emulator-global.
	 */
	chkpnt_user_call.on_checkpoint = (int (*)()) user_routine;

	return(ESUCCESS);
} 

/*
 * User wants to be called before restarting.
 */
int
e_on_restart(
	mach_port_t	serv_port,
	boolean_t	*interrupt,	/* OUT */
	caddr_t		user_routine)
{
	/*
	 * Check given address is in user space.
	 */
	if (user_routine && !user_rcheck(user_routine, sizeof(caddr_t)))
		return(EFAULT);

	/*
	 * Record address in emulator-global.
	 */
	chkpnt_user_call.on_restart = (int (*)()) user_routine;

	return(ESUCCESS);
} 

/*
 * This routine assembles file state information for all open files in the fdt.
 */
int
emul_chkpnt_fdt(
	int		*interrupt,
	int		*chkpnt_n_open_files,
	int		chkpnt_fdt[],
	file_info_t	chkpnt_fs[])
{
	fdt_entry_t		*fdte;
	file_info_t		*fi;
	int			n_file = 0;
	int			fdes;
	int			error = ESUCCESS;

	/*
	 * Scan through the FDT gathering file state for (the first
	 * reference to) each open file.
 	 * To avoid holding the FDT locked while extracting all the
	 * file state info, we make several passes. First, determine
	 * the open fdt entries and reference them. Then release the FDT.
	 */
	FDT_LOCK();
	for (fdes=0; fdes<=fdt_lastfile; fdes++) {
		fdte = fdt[fdes].fdte;
		chkpnt_fdt[fdes] = FD_UNUSED;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;

		if (fdte->referenced) {
			int	i;
			/*
			 * Look back to determine which referenced
			 * file state entry this corresponds to.
			 */
			for (i = 0; fdt[i].fdte != fdte; i++)
				;
			chkpnt_fdt[fdes] = chkpnt_fdt[i];
			continue;
		}

		chkpnt_fdt[fdes] = n_file++;
		fdte_lock(fdte);
		fdte->referenced = 1;
		fdte->refcnt++;
		fdte_unlock(fdte);
	}
	*chkpnt_n_open_files = n_file;

	/*
	 * Clear the 'referenced' flags.
	 */
	for (fdes=0; fdes<=fdt_lastfile; fdes++) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;
		fdte->referenced = 0;
	}

	FDT_UNLOCK();

	/*
	 * Second scan to collect file state information and
	 * unlock the fdt entries.
	 */
	n_file = 0;
	fi = chkpnt_fs;
	for (fdes=0; fdes<=fdt_lastfile; fdes++) {
		/*
		 * Ignore unused slots
		 */
		if (chkpnt_fdt[fdes] == FD_UNUSED)
			continue;

		/*
		 * Ignore slots which refer to a previously
		 * referenced file.
		 */
		if (chkpnt_fdt[fdes] < n_file)
			continue;

		/*
		 * Get file state info for this file provided
		 * an error hasn't already been encountered.
		 */
		fdte = fdt[fdes].fdte;
		if (error == ESUCCESS) {
			error = emul_chkpnt_getfstats(interrupt, fdes, fi);
		}

		/*
		 * Unreference the fdt entry.
		 */
		fdte_lock(fdte);
		fdte->refcnt--;
		fdte_unlock(fdte);
		fi++;
		n_file++;
	}

	for (; fdes<=OPEN_MAX; fdes++)
		chkpnt_fdt[fdes] = FD_UNUSED;
	
	return(error);
}

/*
 * Assemble file state information for a given file descriptor.
 */
int
emul_chkpnt_getfstats(
	int		*interrupt,
	int		fdes,
	file_info_t	*fi)
{
	int			error;
	transaction_id_t	trans_id;
	fdt_entry_t		*fdte = fdt[fdes].fdte;

	E_CHKPNT_DEBUG(("emul_chkpnt_getfstats() fdes=%d", fdes));

	/*
	 * fstat()
	 */
	isc_register(fdte->fp, &trans_id);
	error = fsvr_fstat(fdte->fp,
			   credentials_port,
			   trans_id,
			   &fi->fstat);
	isc_deregister(interrupt);
	if (error) {
		E_CHKPNT_DEBUG(("fsvr_fstat() returns=%d", error));
		return(error);
	}

	/*
	 * Obtain filesystem or device information depending on the file
	 * type. Not that many file types currently cannot be checkpointed.
	 */
	switch (fi->fstat.st_mode & S_IFMT) {
	    case S_IFREG:
	    case S_IFDIR:
	    case S_IFBLK:
	    case S_IFCHR:
		break;
	    default:
		return(EBADF);
	}
	{
		/*
		 * Must go through the generic fileserver interface
		 * for fstatfs().
		 */
		int	args[2];
		int	retval;
		args[0] = fdes;
		args[1] = (int) &fi->fs_or_dev.fstatfs;
		error = emul_fs_generic(our_bsd_server_port, interrupt,
					SYS_fstatfs, &args, &retval);
	}
	if (error) {
		E_CHKPNT_DEBUG(("emul_chkpnt_getfstats() returns=%d", error));
		return(error);
	}

	/*
	 * Pick up file offset and file open mode from fdt entry.
	 */
	fi->offset = fdte->offset;
	fi->fmode = fdte->fmode;

	/*
	 * Sync to disk if open for writing so that we don't lose anything.
	 */
	if (fdte->fmode & FWRITE) {
		isc_register(fdte->fp, &trans_id);
		error = fsvr_fsync(fdte->fp,
				   credentials_port,
				   trans_id);
		isc_deregister(interrupt);
		if (error) {
			E_CHKPNT_DEBUG(("fsvr_fsync() returns=%d", error));
			return(error);
		}
	}

	return(error);
}

/*
 * Get file state information (stat and statfs) for a given file.
 */
int
emul_chkpnt_getstats(
	int		*interrupt,
	char		*path,
	file_info_t	*fip)
{
	int			error;
	int			path_len = strlen(path) + 1;
	mach_port_t		start_port;
	transaction_id_t	trans_id;

	start_port = (*path == '/') ? rootdir_port : currentdir_port;

	/*
	 * Use the standard emulator interface to do the stat
	 * (rather than a direct fsvr_stat()) to remove PFS dependencies.
	 */
        error = e_lstat(our_bsd_server_port, interrupt,
			path, &fip->fstat, NULL);
	if (error) {
		E_CHKPNT_DEBUG(("e_lstat() for \"%s\" returns %d", path, error));
		return(error);
	}

	isc_register(start_port, &trans_id);
	switch (fip->fstat.st_mode & S_IFMT) {
	    case S_IFBLK:
	    case S_IFCHR:
        	error = fsvr_devstat(start_port, credentials_port, trans_id,
				     rootdir_port,
				     path, path_len,
				     &fip->fs_or_dev.devstat);
		break;
	    default:
        	error = fsvr_statfs(start_port, credentials_port, trans_id,
				    rootdir_port,
				    path, path_len,
				    &fip->fs_or_dev.fstatfs);
	}
	isc_deregister(interrupt);
	if (error) {
		E_CHKPNT_DEBUG(("fsvr_statfs() for \"%s\" returns %d", path, error));
		return(error);
	}

	E_CHKPNT_DEBUG(("emul_chkpnt_getstats() returns %d", error));
	return(error);
}

/*
 * This routine is called on each successful exec() to save the name
 * of the exec'ed file together with current and root directory context
 * to enable the file to be exec'ed once again on restart.
 */
int
emul_chkpnt_exec(
	int		*interrupt,
	char		*fname,
	boolean_t       traced)
{
	int	error;

	/*
	 * "on" routines are not propagated over exec().
	 */
	chkpnt_user_call.on_checkpoint = NULL;
	chkpnt_user_call.on_restart    = NULL;

	/*
	 * If the program is being traced, we mark the fname.
	 */
	if (traced) 
		*chkpnt_exec_state.fname = '\0';
	else
		strcpy(&chkpnt_exec_state.fname, fname);

	error = emul_chkpnt_getstats(interrupt,
				     "/",
				     &chkpnt_exec_state.rootdir_fs);
	if (error != ESUCCESS)
		return(error);
	error = emul_chkpnt_getstats(interrupt,
				     ".",
				     &chkpnt_exec_state.currentdir_fs);

	return (error);
}


/*
 * This routine re-opens a file on a given fdt_slot using file state
 * info saved in a checkpoint. 
 */
int
e_openfh(interrupt, fhp, seekoffset, mode, fd, 
	 chkpnt_fstat, chkpnt_fstatfs)
	boolean_t	*interrupt;
	struct fhandle	*fhp;
	off_t		seekoffset;
	int		mode;
	int		fd;
	struct stat	*chkpnt_fstat;
	struct statfs	*chkpnt_fstatfs;
{
	mach_port_t		fp;
	fdt_entry_t		*fdte;
	boolean_t		mappable;
	int			error;
	mach_port_t		start_port;
	transaction_id_t 	trans_id;
	u_long			iomode;
	int			rval;
	struct stat		new_fstat;
	struct statfs		new_fstatfs;
	int			reopen_mode;

	/*
	 * Since we have a specific file descriptor that we want 
	 * to use, try to reserve it.
	 */
	if (error = restart_fdt_reserve_slot(fd, &fdte))
		emul_panic("e_openfh: can't reserve fdt slot %d", fd);

	/*
	 * Set the start_port to root_port.
	 */
	start_port = rootdir_port;

#ifdef	MAPPED_FILES
	/* 
	 * Undo the -FOPEN for the fmode in the fdte 
	 * before the fsvr_open().
	 */
	reopen_mode = (mode + FOPEN);
#endif
	/* 
	 * Disable the FTRUNC just in case the file
	 * was opened with FTRUNC.
	 */
	reopen_mode &= ~FTRUNC;

	isc_register(start_port, &trans_id);
	error = fsvr_openfh(start_port, credentials_port, trans_id,
			    rootdir_port, *fhp, reopen_mode,
			    &fp, &(fdte->iomode));
	isc_deregister(interrupt);
	if (error) {
		fdt_cancel(fd, fdte);
		return(error);
	}

	/*
	 * Set up fields as though we just did a token_release.
	 */
	fdte->flags = 0;
	fdte->min_offset = INT_MAX;
	fdte->max_offset = 0;
	fdte->accessed = 0;
	fdte->modified = 0;
	fdte->must_release = 0;

	/*
	 * Do some checking here.  Use the new file port and
	 * verify that the stat info are still consistant
	 * with our chkpnt images.
	 */
	isc_register(fp, &trans_id);
	error = fsvr_fstat(fp, credentials_port, trans_id, &new_fstat);
	if (error) {
		E_CHKPNT_DEBUG(("fsvr_fstat() returns=%d", error));
		return(error);
	}
	isc_deregister(interrupt);

	error = compare_stat(chkpnt_fstat, &new_fstat, fd);
	if (error) 
		return(error);
		
	/* 
	 * Set up the fdte entry as the open() system call.
	 */
	fdte->fp = fp;
#ifdef	TNC
	if (fdte->iomode == VIO_REQNOTIFY) {
		fdte->notify_on_migrate = 1;
		fdte->iomode = VIO_BUF;
	}
#endif
#ifdef	FAST_PATH_IO
	fdte->fpio_mode = fdte->iomode;
	fdte->fpio_offset = 0;
#endif
	/*
	 * Also reset the fmode here.
	 */
	fdte->fmode = mode;

	/*
	 * After all that, install the new fdte into the fdt.
	 */
	fdt_install(fd, fdte);	

	/* 
	 * After installing the fd, we can use the generic
	 * fileserver interface for checking statfs. 
	 */

	{
		int	args[2];
		int	retval;
		args[0] = fd;
		args[1] = (int) &new_fstatfs;
		error = emul_fs_generic(our_bsd_server_port, interrupt,
					SYS_fstatfs, &args, &retval);
	}
	error = compare_statfs(chkpnt_fstatfs, &new_fstatfs, fd);
	if (error) 
		return(error);

	/*
	 * This is a big kludge here.  Someone is setting the
	 * file seekpointer to -1.  Set it to 0 to avoid
	 * e_lseek return EINVAL.
	 */
	if (seekoffset == -1)
		seekoffset = 0;

	/*
	 * Make a call to the file server to set the seek
	 * pointer.  Since seekoffset is the absolute offset,
	 * just use it. 
	 */
	error = e_lseek(our_bsd_server_port, interrupt, fd, seekoffset,
			   L_SET, &rval);
	return (error);
}

/*
 * This routine to called to re-open all files in a checkpointed fdt
 * given file state information.
 */
int
emul_restart_fdt(
	int		*interrupt,
	ctty_info_t	*chkpnt_ctty,
	int		chkpnt_n_open_files,
	int		chkpnt_fdt[],
	file_info_t	*chkpnt_fs)
{
	fdt_entry_t		*fdte;
	file_info_t		*fi;
	int			error = ESUCCESS;
	int	 		fd, index;
	fhandle_t		fh;	
	mach_port_t		fp;
	fdt_entry_t		*tty_fdte;
	boolean_t		mappable;
	transaction_id_t 	trans_id;
	u_long			iomode;

	/*
	 * Since I am having trouble including <ufs/inode.h>
	 * without defining _KERNEL, just add the definition
	 * for struct ufid by hand.
	 */
	struct ufid {
		u_short ufid_len;       /* length of structure */
		u_short ufid_pad;       /* force long alignment */
		ino_t   ufid_ino;       /* file number (ino) */
		long    ufid_gen;       /* generation number */
	};
	struct ufid 		*ufidp;

	/* 
	 * Try to close all the files in the fdt.
	 */
	close_fdt(interrupt);

	/*
	 * For re-establishing the controlling tty, we don't call 
	 * fdt_reserve() or fdt_install() fdte here. So just obtain 
	 * fdte entry for the controlling tty.
	 */
	isc_register(rootdir_port, &trans_id);
	error = fsvr_open(rootdir_port, credentials_port, trans_id,
			  rootdir_port, "/dev/tty", 9, 2,
			  0, &fp, &mappable, &iomode);
	isc_deregister(interrupt);
	if (error)
		emul_panic("emul_restart_fdt: can't open /dev/tty");

	/*
	 * Obtain a fdte entry.
	 */
	FDT_LOCK();
	fdte_alloc(&tty_fdte);
	FDT_UNLOCK();
	fdte_init(tty_fdte);
	
	/*
	 * Setting up the fdte as though going through the
	 * open() system call.
	 */
	tty_fdte->fp = fp;
	if (iomode == VIO_REQNOTIFY) {
		tty_fdte->notify_on_migrate = 1;
		iomode = VIO_BUF;
	}
#ifdef	FAST_PATH_IO
	tty_fdte->fpio_mode = iomode;
	tty_fdte->fpio_offset = 0;
#endif

	/*
	 * Go through the chkpnt_file_state structure and try 
	 * to reopen all the open files through a file handle.
	 * For files that match the controlling tty, point 
	 * them to the current controlling tty.
	 */
	for (fd = 0; fd < OPEN_MAX; fd++) {
		if ((index=chkpnt_fdt[fd]) == FD_UNUSED) 
			continue;

		fi = &(chkpnt_fs[index]);
		
		/*
		 * Compare every fdt entry with the controlling tty.
		 * If it is, just point it to tty_fdte.
		 * If it's an unlink, transient file with no contents
		 * then open a comparable file.
		 * Otherwise, attempt to reopen the file by file handle.
		 */
		if (IS_CTTY(fi, chkpnt_ctty)) {
			tty_fdte->fmode = fi->fmode;
			fdt_install(fd, tty_fdte);
		} else if ((fi->offset == 0) &&
			   (fi->fstat.st_nlink == 0) &&
			   (fi->fstat.st_size == 0) &&
			   (fi->fmode & O_CREAT)) {
			error = emul_open_transient(interrupt,
						    fd,
						    fi->fmode,
						    fi->fstat.st_mode);
		} else {
			/*
			 * Use the info from file_state[] table to 
			 * construct a file handle, and reopen the
			 * file through the file handle.
			 */
			make_fh(fi, &fh);
			error = e_openfh(interrupt, &fh, fi->offset, fi->fmode, 
			 		 fd, &(fi->fstat), 
					 &(fi->fs_or_dev.fstatfs));
		}
		if (error) 
			break;
	}
	return(error);
}

/*
 * Reserve a particular slot in the file descriptor table, and allocate
 * a file descriptor table entry.
 */
int
restart_fdt_reserve_slot(fdes, fdtep)
	register int fdes;
	fdt_entry_t **fdtep;
{
	FDT_LOCK();
	if (fdt[fdes].fdte == FD_EMPTY) {
		fdt[fdes].fdte = FD_RESERVED;
		fdte_alloc(fdtep);	 /* must hold fdt lock */
		FDT_UNLOCK();
		fdte_init(*fdtep);
		return(ESUCCESS);
	}
	FDT_UNLOCK();
	return(EMFILE);
}

int
compare_stat(
	struct stat	*fstat1,
	struct stat	*fstat2,
	int		fd)
{
#define FSTAT_WARNING_CHECK(fl1, fl2, field, field_string, fdes)	\
	MACRO_BEGIN							\
	if (fl1->field != fl2->field)					\
		EDEBUG(("Warning: fd %d stat field mismatch: %s",	\
			 fdes, field_string));				\
	MACRO_END

#define FSTAT_FAIL_CHECK(fl1, fl2, field, field_string, fdes)		\
	MACRO_BEGIN							\
	if (fl1->field != fl2->field) {					\
		EDEBUG(("Restart failure: fd %d stat field mismatch: %s", \
			 fdes, field_string));				\
		return(ESTALE);						\
	}								\
	MACRO_END

	/*	
	 * If the following fields are inconsistent,
	 * we have no choice but to fail.    
	 * All three fields should have been checked 
	 * during the file handle to vnode translation.
	 */
	FSTAT_FAIL_CHECK(fstat1, fstat2, st_dev, "st_dev", fd);
	FSTAT_FAIL_CHECK(fstat1, fstat2, st_ino, "st_ino", fd);
	FSTAT_FAIL_CHECK(fstat1, fstat2, st_gen, "st_gen", fd);

	/*
	 * Inconsistencies in the following fields only
	 * generate a warning.  Not serious enough to 
	 * warrant a panic.  
	 */
	FSTAT_WARNING_CHECK(fstat1, fstat2, st_uid, "st_uid", fd);
	FSTAT_WARNING_CHECK(fstat1, fstat2, st_gid, "st_gid", fd);
/*
	FSTAT_WARNING_CHECK(fstat1, fstat2, st_atime, "st_atime", fd);
	FSTAT_WARNING_CHECK(fstat1, fstat2, st_mtime, "st_mtime", fd);
	FSTAT_WARNING_CHECK(fstat1, fstat2, st_ctime, "st_ctime", fd);
*/

	/*
	 * File size is a special case, we allow file
	 * that has been recently appended to.  But
	 * we fail if file size is smaller than
	 * expected. 
	 */
	if (fstat1->st_size != fstat2->st_size){
		if (fstat1->st_size > fstat2->st_size) {
			EDEBUG(("fd %d: file size decreased", fd));
			return(-1);
		}
		else
			EDEBUG(("Warning: fd %d file size increased", fd));  
	}
	return(ESUCCESS);
}

int
compare_statfs(
	struct statfs	*fstatfs1,
	struct statfs	*fstatfs2,
	int		fd)
{
#define FSTATFS_FAIL_CHECK(fl1, fl2, field, field_string, fd)		\
	MACRO_BEGIN							\
	if (fl1->field != fl2->field) {					\
		EDEBUG(("Restart: fd %d statfs field mismatch: %s\n",	\
			 fd, field_string));				\
		return(ESTALE);						\
	}								\
	MACRO_END

#define FSTATFS_STRING_FAIL_CHECK(fl1, fl2, field, field_string, fd)	\
	MACRO_BEGIN							\
	if (strcmp(fl1->field, fl2->field) != 0) {			\
		EDEBUG(("Restart: fd %d statfs field mismatch: %s\n",	\
			 fd, field_string));				\
		return(ESTALE);						\
	} \
	MACRO_END

#define FSTATFS_WARNING_CHECK(fl1, fl2, field, field_string, fd)	\
	MACRO_BEGIN							\
	if (strcmp(fl1->field, fl2->field) != 0)			\
		EDEBUG(("Warning: fd %d statfs field mismatch: %s\n",	\
			 fd, field_string));				\
	MACRO_END

	FSTATFS_FAIL_CHECK(fstatfs1, fstatfs2, f_type, "f_type", fd);
	FSTATFS_WARNING_CHECK(fstatfs1, fstatfs2, f_mntonname, "f_mntonname", fd);
	FSTATFS_STRING_FAIL_CHECK(fstatfs1, fstatfs2, f_mntfromname, "f_mntfromname", fd);
	FSTATFS_FAIL_CHECK(fstatfs1, fstatfs2, f_fsid.val[0], "f_fsid", fd);
	FSTATFS_FAIL_CHECK(fstatfs1, fstatfs2, f_fsid.val[1], "f_fsid", fd);
	return(0);
}

void
make_fh(file_info_t *fip, struct fhandle *fhp)
{
	/*
	 * Since I am having trouble including <ufs/inode.h>
	 * without defining _KERNEL, just add the definition
	 * for struct ufid by hand.
	 */
	struct ufid {
		u_short ufid_len;       /* length of structure */
		u_short ufid_pad;       /* force long alignment */
		ino_t   ufid_ino;       /* file number (ino) */
		long    ufid_gen;       /* generation number */
	};
	struct ufid 	*ufidp;

	ufidp = (struct ufid *) &(fhp->fh_fid);
	fhp->fh_fsid = fip->fs_or_dev.fstatfs.f_fsid; 

	/*
	 * NOTE: ufid_len and ufid_pad is based on past
	 * observation, will need more accurate count.
	 */
	ufidp->ufid_len = 12;
	ufidp->ufid_pad = 0;

	ufidp->ufid_ino = fip->fstat.st_ino;
	ufidp->ufid_gen = fip->fstat.st_gen;
}

void
close_fdt( boolean_t	*interrupt)
{
	int 		fdes;
	int		rval;
	int		error;
	fdt_entry_t	*fdte;

	for (fdes=0; fdes<=fdt_lastfile; fdes++) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;
		error = e_close(our_bsd_server_port, interrupt, fdes, &rval);
		if (error)
			emul_panic("restart: can't close all current files");
	}
}
/*
 * Exec a previously checkpointed process state - restoring stack and data
 * regions before returning from the server and returning file state info
 * so that files can be reopened here in the emulator.
 */
int
e_exec_restart_call(
	mach_port_t	serv_port,
	boolean_t	*interrupt,
	char		*chkpnt_prefix,
	int		options,
	thread_state_t	new_state,
	unsigned int	*new_state_count,
	boolean_t	*stop,	 	/* OUT */
	boolean_t	*traced,	/* OUT */
	int		(**tramp)())
{
	int			pathname_len;
	chkpnt_file_state_t	*fsp = NULL;
	unsigned int		fs_size = 0;
	mach_port_t		new_rootdir_port = MACH_PORT_NULL;
	mach_port_t		new_currentdir_port = MACH_PORT_NULL;
	int			error = ESUCCESS;

	if (!user_strlen(chkpnt_prefix, &pathname_len))
		return(EFAULT);

	/*
	 * Call the server to do the work. It performs the "exec",
	 * re-establishes the checkpointed process context and
	 * returns thread and file state for the emulator to restore.
	 * Note: when re-exec'ing to a remote node, this will not
	 * return here but will restart through exec_chkpnt_arrival().
	 */
	emul_blocking();
	error = bsd_exec_restart(vproc_port, interrupt,
				 chkpnt_prefix, pathname_len + 1,
				 new_state, new_state_count,
				 &new_rootdir_port, &new_currentdir_port,
				 (char_array *) &fsp, &fs_size,
				 traced);
	emul_unblocking();
	if (error != ESUCCESS)
		return(error);

	/*
	 * Switch to new root/current dirs if required. Note that the
	 * server will have already switched in the exec_restart().
	 */
	if (new_rootdir_port != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(), rootdir_port);
		rootdir_port = new_rootdir_port;
	}
	if (new_currentdir_port != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(), currentdir_port);
		currentdir_port = new_currentdir_port;
	}

	/*
	 * Re-open all files and restore file offsets.
	 * Terminate the process if this fails, we're comitted.
	 */
	error = emul_restart_fdt(interrupt, 
				 &fsp->ctty,
				 fsp->n_open_files, fsp->fdt, fsp->file_state);
	if (error != ESUCCESS) {
		emul_blocking();
		(void) bsd_proc_exit(serv_port, interrupt, SIGKILL, FALSE);
		emul_unblocking();
	}

	/*
	 * Restore saved exec'ed filename and root/current directory
	 * from the saved image. Also restore user-callbacks.
	 */
	strcpy(&chkpnt_exec_state.fname, &fsp->exec_fname);
	chkpnt_exec_state.rootdir_fs    = fsp->exec_rootdir;
	chkpnt_exec_state.currentdir_fs = fsp->exec_currentdir;
	chkpnt_user_call                = fsp->on_routines;
	*tramp				= (int (*)()) fsp->signal_trampoline;

	/*
	 * Free the memory containing the file_state passed out-of-line.
	 */
	vm_deallocate(mach_task_self(),
		      (vm_address_t) fsp, (vm_size_t) fs_size);

	/*
	 * If necessary, now let the user do their own thing.
	 */
	if (chkpnt_user_call.on_restart != NULL)
		(void) (*chkpnt_user_call.on_restart)();

	/*
	 * Request machine dependent caller to stop, if necessary.
	 */
	*stop = options & RESTARTEXEC_STOP;

	return(ESUCCESS);
}

/*
 * This routine opens and unlinks a file on a given file descriptor.
 * Since unlinked, the original file will have been deleted when the
 * checkpointed process terminated. However, if nothing had been written
 * to it when the checkpoint occurred, there's no harm in whipping up a
 * substitute.
 */ 
int
emul_open_transient(
	int	*interrupt,
	int	fd,
	int	mode,
	int	crtmode)
{
	fdt_entry_t		*fdte;
	boolean_t		mappable;
	int			error;
	mach_port_t		start_port;
	transaction_id_t 	trans_id;
	u_long			iomode;
	int			reopen_mode;
	int			rval;
	path_name_t		fname;
	int			len_fname;

	/*
	 * Generate a unique (enough) temporary filename with which to
	 * open this transient file.
	 */
	EASSERT(current_pid > 0);
	sprintf(fname, "/tmp/restart.%d", current_pid);
	len_fname = strlen(fname);
	
	/*
	 * Since we have a specific file descriptor that we want 
	 * to use, try to reserve it.
	 */
	if (error = restart_fdt_reserve_slot(fd, &fdte))
		emul_panic("emul_open_transient: can't reserve fdt slot %d",
			   fd);

	/*
	 * Set the start_port to root_port.
	 */
	start_port = rootdir_port;

#ifdef	MAPPED_FILES
	/* 
	 * Undo the -FOPEN for the fmode in the fdte 
	 * before the fsvr_open().
	 */
	reopen_mode = mode + FOPEN;
#endif
	/* 
	 * Also disable the FTRUNC just in case the file
	 * was opened with FTRUNC.
	 */
	reopen_mode &= ~FTRUNC;

	isc_register(start_port, &trans_id);
	error = fsvr_open(start_port, credentials_port, trans_id,
			  rootdir_port,  fname, len_fname + 1, reopen_mode,
                          crtmode, &fdte->fp, &fdte->iomode);
	isc_deregister(interrupt);
	if (error) {
		fdt_cancel(fd, fdte);
		return(error);
	}

	/*
	 * Set up fields as though we just did a token_release.
	 */
	fdte->flags = 0;
	fdte->min_offset = INT_MAX;
	fdte->max_offset = 0;
	fdte->accessed = 0;
	fdte->modified = 0;
	fdte->must_release = 0;

	/* 
	 * Set up the fdte entry as the open() system call.
	 */
	if (fdte->iomode == VIO_REQNOTIFY) {
		fdte->notify_on_migrate = 1;
		fdte->iomode = VIO_BUF;
	}
#ifdef	FAST_PATH_IO
	fdte->fpio_mode = fdte->iomode;
	fdte->fpio_offset = 0;
#endif
	/*
	 * Also reset the fmode here.
	 */
	fdte->fmode = mode;

	/*
	 * After all that, install the new fdte into the fdt.
	 */
	fdt_install(fd, fdte);	

	/*
	 * Having done, we undo some - unlink the file so that it remains
	 * invisible.
	 * Use e_unlink() rather than fsvr_unlink() to avoid PFS dependency.
	 */
	error = e_unlink(our_bsd_server_port, interrupt, fname);

	return(error);

}

#endif /* CHKPNT */
