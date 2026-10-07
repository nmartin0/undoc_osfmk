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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This file was modified by the Center for High Performance Computing (CHPC)
 * on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: emul_init.c,v $
 * Revision 1.13  1995/02/18  01:17:30  yazz
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
 * Revision 1.12  1995/02/01  21:22:19  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:23:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/08/31  22:45:38  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.9.2.2  1994/08/23  18:28:04  dbm
 * Added missing ifdef PFS statements around the pfs_async_dflt logic.
 * This was causing the ramdisk build to break.
 * Reviewer: rlg
 * Risk:Low
 * Benefit or PTS #: 10701
 * Testing: Ramdisk build.
 * Module(s):
 * 	server/uxkern/boot_config.c
 * 	emulator/emul_init.c
 *
 * Revision 1.9.2.1  1994/08/19  22:29:28  dbm
 * Added support for a new bootmagic, PFS_ASYNC_DFLT, this allows setting
 * the default PFS I/O mode to M_ASYNC.
 *
 *  Reviewer:Bob Godley
 *  Risk:M
 *  Benefit or PTS #:10569
 *  Testing: Specific test cases. PFS EATS (With and without bootmagic set)
 *  Module(s):
 *
 *     (server)
 *         uxkern/boot_config.c
 *         uxkern/fsvr_server_side.c
 *         uxkern/fsvr.defs
 *     (emulator)
 *         emul_init.c
 *         fsvr_user_side.c
 *         pfs2_user_side.c
 *         pfs_iomode.c
 *         pfs_tokenmgt.c
 *         pfs_iomode.h
 *         pfs_fdt.h
 *     (libnx)
 *         _pfs_setio.c
 *         _setiomode.c
 *
 * Revision 1.9  1994/02/24  23:20:55  rlg
 * A new initialization subroutine, async_lock_init(), is called from main().
 * The asynchronous I/O code was testing a spin lock before it was initialized.
 * This change is the fix for PTS #8211.
 *
 * Revision 1.8  1993/07/14  17:30:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  18:22:40  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  20:14:49  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:17:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/05/06  00:21:02  brad
 * Added ifdef PFS ... they were missing in async I/O code.
 *
 * Revision 1.5  1993/04/03  03:17:34  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.2  1993/03/20  01:13:51  wunder
 * Added changes to reinitialize async support on fork, in child_init().
 *
 * Revision 2.36  93/06/09  17:11:46  yazz
 * [ Bug #0107 ] Establish our_svipc_port for Sys V IPCs.
 * 
 * Revision 2.35  93/06/02  09:44:28  yazz
 * For Sys V IPC under TNC clear the svipc port for child on fork.
 * 
 * Revision 2.34  93/03/05  11:03:05  yazz
 * Check in correct version of file to include fdt.h and accurately spell
 * comments.
 *
 * Revision 2.33  93/03/04  17:02:57  yazz
 * For no MAPPED_FILES, substitute MACH_PORT_NULL for revoke_port, a global
 * only present when MAPPED_FILES is true.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  05:56:57  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  02:51:32  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:08:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  18:20:46  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/05  22:15:39  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.32  92/11/23  15:59:07  klh
 * 	Revision 2.18  92/11/05  17:37:04  roy
 * 		Added rlimit_fsize global.
 * 		[92/10/26            roy]
 * 
 * 	Revision 2.17  92/10/29  11:29:09  loverso
 * 		Further clarify comments on the reply port. (loverso)
 * 
 * Revision 2.31  92/10/28  14:45:38  roman
 * Fix some types for a cleaner compilation.
 * 
 * Revision 2.30  92/10/08  11:11:35  roman
 * Chnage file fdt_moveproc_insert_righst() to moveproc_insert_rights()
 * 	(because fdt is not locked during call).
 * Change rexecve_init_call() to rexecve_init() (there is no longer
 * 	an intermediate machine-dependent routine).
 * No longer invoke e_rexecve_arrival() from rexecve_init - this
 * 	is now done from lower level machine dependent code in
 * 	a fashion that makes it look like a "true" system call.
 * 
 * Revision 2.29  92/10/06  12:03:37  roman
 * Fix RCS comments.
 * 
 * Revision 2.28  92/10/05  13:41:06  klh
 * 	Revision 2.16  92/08/26  12:09:17  loverso
 * 		Add warning comments. (loverso)
 * 
 * Revision 2.27  92/07/30  15:56:46  chrisp
 * Rename moveproc_insert_rights() to fdt_moveproc_insert_rights() to
 * 	signify that the FDT is locked when it is called. This is in line
 * 	with the renaming of fdt_fork_insert_rights().
 * 
 * Revision 2.26  92/07/07  15:00:17  roman
 * Change invocation of callback_init() to handle the revoke_port in addition to
 * 	the callback_port.
 * 
 * Revision 2.25  92/06/15  15:14:15  roman
 * Change callback_init() parameters to allow TNC migration and remote
 * 	execution to pass in the callback port from the remote task.
 * 
 * Revision 2.24  92/06/05  13:54:12  klh
 * 	Revision 2.13  92/05/31  18:55:24  loverso
 * 		Revision 3.2  92/02/28  00:11:08  condict
 * 		Notifiy server which addresses are involved in EFAULT recovery.
 * 
 * 	Revision 2.12  92/05/18  12:26:01  roy
 * 		Revision 2.9.1.1  92/04/22  09:47:39  roy
 * 			Rename isc_init() to callback_init().
 * 			[92/04/05            roy]
 * 
 * Revision 2.23  92/04/06  19:06:56  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.11  92/04/05  16:41:02  pjg
 * 	Change the algorithm for TNC calculation of the current node to 
 * 	not call norma_node_self() at the start of every process. (roman)
 * 
 * 	For TNC, cache the current node number as part of process 
 * 	initialization. (roman)
 * 
 * Revision 2.10  92/03/20  11:54:05  pjg
 * 	Print the error message in emul_panic.
 * 
 * Revision 2.9  92/02/11  18:53:15  pjg
 * 	Call isc_init() before mig_init() in all cases.
 * 
 * 	Restructure the "init" routines for migrate() and rexec(). 
 * 	This got rid of the routine rtask_init_common() completely, 
 * 	and moved major portions of the rexec code (that was similar 
 * 	to the exec code) into bsd_user_side.c.
 * 	Change type of auxv_strings to the correct one (auxv_strings_t).
 * 	Correct code for doing file descriptor manipulation for migrate 
 * 	and rexec. Reset emulator's SIGMIGRATE handler after rexec() 
 * 	[moved from emul_machdep] (TNC only).
 * 	(roman@locus.com, chrisp@locus.com, yazz@locus.com).
 * 
 * Revision 2.8  92/01/17  17:16:54  roy
 * 	Interruptible system call support (srl).
 * 
 * Revision 2.7  92/01/03  09:29:08  roy
 * 	Remove OSF1_ADFS_HACK.
 * 
 * Revision 2.6  92/01/02  18:41:38  roy
 * 	91/09/22  22:17:38  noemi
 * 	OSF1/ADFS update.  Added OSF1_ADFS_HACK.
 * 
 * Revision 2.5  91/12/13  10:05:24  roy
 * 	91/11/11  19:19:50  roy
 * 	Added revoke_port and call to fdt_init_child().
 * 
 * 	91/10/29  17:52:19  roy
 * 	Call fdt_init().
 * 
 * Revision 2.4  91/10/04  14:42:14  chrisp
 * Put back Locus copyright message.
 * 
 * Revision 2.3  91/09/17  12:13:40  sjs
 * integrate Locus changes	roman
 * Call the routine ufinit() to initialize the emulator file
 * descriptor table.  Also call routines to get the bootstrap
 * root and current directory vnode ports, along with the
 * bootstrap credentials port and bootstrap vproc port.
 * 
 * Revision 2.2  91/08/30  16:40:09  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.1  91/02/27  16:22:09  condict
 * Change panic to do a breakpoint trap (i386) or suspend, for the debugger.
 * 
 * Revision 3.0  91/01/17  12:05:08  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  90/08/06  15:30:13  rwd
 * 	Comment potential problem in main().
 * 	[90/07/19            rwd]
 * 
 * Revision 2.3  90/06/02  15:20:29  rpd
 * 	Converted to new IPC.
 * 	Added emul_panic.
 * 	[90/03/26  19:28:17  rpd]
 * 
 * Revision 2.2  90/03/14  21:22:55  rwd
 * 	Added conditional call to emul_mapped_init.
 * 	[90/01/23            rwd]
 * 
 * $EndLog$
 * 
 */
/*
 * Setup emulator entry points.
 */
#include <mach/mach.h>
#include <mach/message.h>
#include <uxkern/bsd_1.h>
#include <uxkern/bsd_msg.h>
#include "emul_stack.h"
#include "emul.h"
#include "fdt.h"

void	emul_panic();		/* forward */
  
#ifdef PFS
extern int pfs_async_dflt;
#endif
/*
 * For error recovery on user address exceptions.  See user_bcopy:
 */
extern int uacc_start(), uacc_end(), uacc_err();

#ifdef	PFS
/*
 * For setting uninitialized state of asynchronous support on fork
 */
extern boolean_t async_initialized;
#endif

/*
 * Ports that are global to the emulator.
 */
mach_port_t	our_bsd_server_port;	/* General server (proc port) */
mach_port_t	credentials_port;	/* Credentials port */
mach_port_t	vproc_port;		/* Server for vproc calls */
mach_port_t	rootdir_port;		/* Port for root directory */
mach_port_t	currentdir_port;	/* Port for current working directory */

#ifdef	TNC
/*
 * Ports that are global to the emulator under TNC.
 */
mach_port_t	our_svipc_port = MACH_PORT_NULL;	/* Port for Sys V IPC */
#endif	/* TNC */

/*
 * Other global variables.
 */
pid_t		current_pid = -1;	/* pid of current process */

#ifdef	MAPPED_FILES
unsigned int	rlimit_fsize;		/* max. file size allowed by rlimit */
#endif

#ifdef	TNC
node_t		tnc_mynode;
#endif	/* TNC */

/*
 * Initialize emulator.  Our bootstrap port is the BSD request port.
 */
emul_stack_t
emul_init(new_stack_flag)
	int	new_stack_flag;
{
	emul_stack_t stack;
	int dummy[2];

	/*
	 * Bootstrap port is the request port.
	 */
	(void) task_get_bootstrap_port(mach_task_self(), &our_bsd_server_port);

	/*
	 * Set up emulator stack support.
	 */
	if(new_stack_flag )
		stack = emul_stack_init();
	else
		stack = emul_stack_reinit();

#ifdef ECACHE
	/*
	 * Setup cache.
	 */
	emul_cache_init();
#endif ECACHE

#ifdef MAP_UAREA
	/*
	 * Setup mapped area.  Also sets current_pid from this area.
	 * Use e_getpid to set current_pid by side effect, if uarea
	 * not mapped.
	 */
	emul_mapped_init();
#else MAP_UAREA
	e_getpid(our_bsd_server_port, dummy, dummy);
#endif MAP_UAREA

#ifdef	TNC
	/*
	 * For TNC, current node is unknown so far.
	 */
	tnc_mynode = (node_t) -1;
#endif	/* TNC */

	return (stack);
}

main(argc, argv)
	int	argc;
	char	**argv;
{
	emul_stack_t stack;

	/*
	 * Initialize the emulator.
	 */
	stack = emul_init(TRUE);	/* say we DO need a new stack */

	/*
	 * Tell the server which addresses to use for error recovery when the
	 * emulator gets an exception during access of a user address in
	 * user_bcopy:
	 */
	(void) bsd_emul_uacc(our_bsd_server_port, (int)uacc_start,
						(int)uacc_end, (int)uacc_err);

	/*
	 * Set up the emulator vectors.
	 */
	emul_setup(mach_task_self());

	/*
	 * Initialize the per-process file descriptor table.
	 */
	fdt_init();

#ifdef	PFS
	/*
	 * Initialize the asyncronous I/O locks:
	 */
	async_lock_init();
#endif

	/*
	 * Get the credentials port and the vproc port.
	 */
	(void) bsd_get_bootstrap_process_ports(our_bsd_server_port,
					       &vproc_port,
					       &credentials_port);

	/*
	 * Get the root directory and the current working directory.
	 */
	(void) bsd_get_bootstrap_directories(our_bsd_server_port,
					     &rootdir_port,
					     &currentdir_port);

#ifdef PFS
	(void) fsvr_pfs_async_dflt(rootdir_port, &pfs_async_dflt);
#endif

	/*
	 * Initialize the callback thread, including support for interruptable 
	 * system calls.
	 *
	 * WARNING: Once this is enabled, we can immediately begin processing
	 *	    semul_state_release() messages.  Thus, have all other
	 *	    configuration completed before callback_init.
	 */
	callback_init(MACH_PORT_NULL, MACH_PORT_NULL, MACH_PORT_NULL); 

	mig_init(stack);
	/*
	 * Don't on your life try to use a mig stub after the call to
	 * mig_init(). The stack has not been switched yet, and it won't work.
	 * The emul_execve call results in the desired stack switch 
	 * occuring (though it itself never returns)
	 */

	/*
	 * Exec the first program.
	 */
	return (emul_execve(argv[0], argv, (char **)0));
}


/*
 * Initialize emulator on child branch of fork.
 */
void
child_init()
{
	emul_stack_t stack;

	/*
	 * Re-initialize ALL of the emulator - saved
	 * port numbers are no longer valid, and
	 * we have a new request port.
	 *
	 * mach_init resets mach_task_self(), and also calls mig_init(),
	 * which gives us a new global reply port (which is used until
	 * out explicit call to mig_init() below).
	 */
	mach_init();			/* reset mach_task_self_ ! */
	stack = emul_init(TRUE);	/* say we DO need a new stack */

#ifdef	TNC
	/*
	 * Clear the Sys V IPC port; a child (fork/rfork'd) task does not
	 * inherit its parent's svipc port.
	 */
	our_svipc_port = MACH_PORT_NULL;
#endif	/* TNC */

	/*
	 * Initialize state associated with the file descriptor table.
	 */
	fdt_init_child();

	/*
	 * Initialize the callback thread, including support for interruptable 
	 * system calls.
	 *
	 * WARNING: Once this is enabled, we can immediately begin processing
	 *	    semul_state_release() messages.  Thus, have all other
	 *	    configuration completed before callback_init.
	 */
	callback_init(MACH_PORT_NULL, MACH_PORT_NULL, MACH_PORT_NULL); 

	mig_init(stack);
	/*
	 * Don't on your life try to use a mig stub after the call to
	 * mig_init(). The stack has not been switched yet, and it won't work.
	 */

#ifdef	PFS
	/*
	 * Initialize asynchronous file I/O status
	 */
	async_initialized = FALSE;
#endif
}

#ifdef TNC

void
migrate_init()
{
	task_t			old_task;
	emul_stack_t		stack;
	int			err;
	mach_port_t		local_revoke_port;
	extern mach_port_t	callback_port;

#ifdef MAPPED_FILES
	local_revoke_port = revoke_port;	/* extern in fdt.h */
#else
	local_revoke_port = MACH_PORT_NULL;
#endif

	/*
	 * Save off the name previously used by mach_task_self(). Due to
	 * the efforts of the server, this is now a port right to the
	 * old task (the task on the originating node). Then use
	 * mach_init() to get a new value for mach_task_self().
	 */
	old_task = mach_task_self_;	/* same as mach_task_self() */
	/*
	 * mach_init resets mach_task_self(), and also calls mig_init(),
	 * which gives us a new global reply port (which is used until
	 * out explicit call to mig_init() below).
	 */
	mach_init();			/* reset mach_task_self_ ! */

	/*
	 * Reinitialize the emulator stack area and get a MiG reply
	 * port allocated. After this call, MiG can be used within
	 * this process until mig_init() is called.
	 */
	stack = emul_init(TRUE);

	/*
	 * This is probably wrong, as the thread should probably
	 * "naturally" get re-created by other migration logic. XXXXX
	 */
	callback_init(callback_port, local_revoke_port, old_task);

	/*
	 * Extract all the necessary port rights from the old task on
	 * the originating node.
	 */
	moveproc_insert_rights(old_task);

	/*
	 * Now that we have extracted all the old emulator's rights that
	 * we're interested in, destroy that task altogether.  Recall that
	 * tho we are the same unix process with the same unix pid, we are
	 * a different mach task with a different mach task port.
	 */
	err = task_terminate(old_task);
	if (err != KERN_SUCCESS) {
		emul_panic("task_terminate failure");
	}

	mig_init(stack);
}


void
rexecve_init()
{
	task_t			old_task;
	emul_stack_t		stack;
	int			err;
	mach_port_t		local_revoke_port;
	extern mach_port_t	callback_port;

#ifdef MAPPED_FILES
	local_revoke_port = revoke_port;	/* extern in fdt.h */
#else
	local_revoke_port = MACH_PORT_NULL;
#endif

	/*
	 * Save off the name previously used by mach_task_self(). Due to
	 * the efforts of the server, this is now a port right to the
	 * old task (the task on the originating node). Then use
	 * mach_init() to get a new value for mach_task_self().
	 */
	old_task = mach_task_self_;	/* same as mach_task_self() */
	/*
	 * mach_init resets mach_task_self(), and also calls mig_init(),
	 * which gives us a new global reply port (which is used until
	 * out explicit call to mig_init() below).
	 */
	mach_init();			/* reset mach_task_self_ ! */

	/*
	 * Reinitialize the emulator stack area and get a MiG reply
	 * port allocated. After this call, MiG can be used within
	 * this process.
	 */
	stack = emul_init(TRUE);

	/*
	 * Start up a new callback thread.
	 */
	callback_init(callback_port, local_revoke_port, old_task);

	/*
	 * Extract all the necessary port rights from the old task on
	 * the originating node.
	 */
	moveproc_insert_rights(old_task);

	/*
	 * Close all the files that were set "close-on-exec".
	 */
	(void) close_on_exec();

	/*
	 * Now that we have extracted all the old emulator's rights that
	 * we're interested in, destroy that task altogether.  Recall that
	 * tho we are the same unix process with the same unix pid, we are
	 * a different mach task with a different mach task port.
	 */
	err = task_terminate(old_task);
	if (err != KERN_SUCCESS) {
		emul_panic("task_terminate failure");
	}

	mig_init(stack);

	/*
	 * Don't on your life try to use a mig stub after the call to
	 * mig_init() without specifying the reply_port.
	 * The stack has not been switched yet, and it won't work.
	 */
}

#endif	/* TNC */

/*
 * Fail in an interesting, easy-to-debug way.
 */
void
emul_panic(msg)
	char *msg;
{
	e_emulator_error("OS Emulator Crashed!:");
	e_emulator_error(msg);

	for (;;) {
#ifdef	i386
		asm("int3");
#else	i386
		task_suspend(mach_task_self());
#endif	i386
		(void) mach_msg_trap((mach_msg_header_t *) msg,
				     MACH_MSG_OPTION_NONE, /* do nothing */
				     0, 0, MACH_PORT_NULL,
				     MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	}
}
