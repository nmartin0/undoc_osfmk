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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: emul_mapped.c,v $
 * Revision 1.12  1995/02/18  01:17:39  yazz
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
 * Revision 1.11  1994/12/20  22:00:09  suri
 *  Reviewer: jlitvin
 *  Risk: Low
 *  Benefit or PTS #: 11640
 *  Testing: Specific testcase
 *  Module(s): Backing out the fix for PTS-11270/11317/10593 (vm_allocate/sbrk
 *  changes), as it was causing excessive wiring down of memory on the compute
 *  nodes under some specific cases (the testcase for PTS-11640 is one of them).
 *  The vm_allocate/sbrk changes have to be reimplemented, perhaps using
 *  vm_reserve().
 *
 * Revision 1.10  1994/11/18  20:23:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/10/11  18:33:13  suri
 *  Reviewer: cfj
 *  Risk: H
 *  Benefit or PTS #: 10593
 *  Testing: Specific testcase; fileio, message_passing, xtrnl,
 * 	  PFS and pthreads EATs
 *  Module(s): e_obreak() in emulator/emul_mapped.c
 * 	    coff_getxfile() in server/bsd/kern_exec.c
 * 	    obreak() in server/bsd/kern_mman.c
 * 	    setrlimit() in server/bsd/kern_resource.c
 *
 * Revision 1.8  1993/07/14  17:31:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  18:22:50  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/06/01  15:41:52  cfj
 * Put #ifdef DEBUG around share_lock failure message.
 *
 * Revision 1.6  1993/05/06  20:14:51  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:17:32  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:17:36  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  05:57:00  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  02:51:35  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:08:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  18:20:48  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/05  22:15:41  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.16  1992/10/14  17:24:18  cfj
 * NX integration.
 *
 * Revision 2.16  93/06/16  13:48:25  klh
 * 	Revision 2.14  93/06/02  17:27:56  rabii
 * 		Put ifdef DEBUG around print in share_lock (rabii, cfj@ssd.intel.com)
 * 
 * 	Revision 2.13  93/05/16  20:57:48  loverso
 * 		Remove use of syscall_suspend_barrier() (loverso)
 * 
 * Revision 2.15  92/10/09  09:52:27  roman
 * Fix RCS comment.
 * 
 * Revision 2.14  92/10/08  11:12:20  roman
 * Get rid of extraneous '\n' in error message.
 * 
 * Revision 2.13  92/10/06  12:03:47  roman
 * Fix RCS comments.
 * 
 * Revision 2.12  92/10/05  13:41:22  klh
 * 	Revision 2.11  92/09/11  09:25:18  rabii
 * 		Check suspend barrier while trying to take share lock.
 * 
 * Revision 2.11  92/07/07  15:01:42  roman
 * Correct minor typo.
 * 
 * Revision 2.10  92/06/05  13:54:21  klh
 * 	Revision 2.9  92/05/24  14:06:53  pjg
 * 		92/03/20  15:07:57  jose
 * 		Added hook to send obreak to server (not used yet)
 * 		[92/05/19            srl]
 * 
 * Revision 2.9  92/05/20  13:39:51  chrisp
 * Remove emulation of umask() via the shared user area, since this fails
 * 	to inform the credentials service of the update and the new mask
 * 	is ignored.
 * 
 * Revision 2.8  91/12/18  10:16:04  roy
 * 	Remove e_getpid (again).
 * 
 * Revision 2.7  91/12/16  10:21:55  roy
 * 	91/12/13  13:02:02  sp
 * 	Remove old BSD code and VICE conditionals
 * 
 * 	91/11/13  14:55:10  barbou
 * 	Fix for bug #35: the interface to sigpending() was completely bogus.
 * 
 * 	91/10/29  16:10:53  barbou
 * 	Fix for bug #29: got rid of bogus e_getgroups().
 * 
 * 	91/10/28  11:09:49  condict
 * 	Remove OSF1_SERVER ifdef.
 * 
 * Revision 2.6  91/10/16  09:14:22  sjs
 * 	Fixed problem with merge - deleted shared_readwrite
 * 
 * Revision 2.5  91/10/14  13:09:05  srl
 * (Grenoble V2.2 merge)
 * 	91/09/20  19:14:46  barbou
 * 	Dynamic computation of the number of pages shared with the server.
 * 
 * Revision 2.4  91/10/04  14:42:30  chrisp
 * Put back Locus copyright message.
 * 
 * Revision 2.3  91/09/17  12:15:47  sjs
 * integrate Locus changes	roman
 * With file descriptor handling being done in emulator and
 * potential for distributed file servers, using the shared
 * uarea region for IO is no longer feasible.  All such
 * dependencies removed, along with all code under
 * "ifdef MAP_FILE".
 * 
 * Revision 2.2  91/08/30  16:40:13  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.6  91/08/27  15:25:25  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.5  91/08/09  11:42:27  barbou
 * Initialization of the in_emulator counter lock.
 * 
 * Revision 3.4  91/07/29  16:07:07  barbou
 * Bug fix in e_lstat(), from rwd.
 * 
 * Revision 3.3  91/05/30  17:24:02  jose
 * Added o prefix to compatibility signal syscalls.
 * 
 * Revision 3.2  91/05/15  17:20:48  barbou
 * New entry points: e_sigpending() and e_sigprocmask().
 * 
 * Revision 3.1  91/03/08  15:51:28  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.0  91/01/17  12:05:09  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.11  91/08/12  22:36:53  rvb
 * 	Fixed e_maprw to use EPRINT instead of e_emulator_error.
 * 	[91/08/06            rpd]
 * 
 * Revision 2.10  91/07/30  15:35:20  rvb
 * 	From rwd: change default of case share_unlock line to
 * 	rel_it(argp[0],interrupt)
 * 	[91/07/29            rvb]
 * 
 * 	From rfr: gets rid of close piggybacking.
 * 	[91/07/29            rvb]
 * 
 * Revision 2.9  91/03/20  15:01:34  dbg
 * 	Fix for ANSI C.
 * 	[91/02/22            dbg]
 * 
 * Revision 2.8  90/11/05  15:31:37  rpd
 * 	Added spin_lock_t.
 * 	[90/10/31            rwd]
 * 
 * Revision 2.7  90/11/05  15:28:15  rpd
 * 	Fixed vm_region call in emul_mapped_init.
 * 	[90/11/02            rpd]
 * 
 * Revision 2.6  90/08/06  15:30:21  rwd
 * 	Change share_lock to set who field to be proc pointer instead of
 * 	worthless local port name.  Up threshold on share_lock error
 * 	message.
 * 	[90/07/19            rwd]
 * 	Added share_lock and share_unlock routines.
 * 	[90/06/08            rwd]
 * 	Changed debugging e_emulator_error to use EPRINT
 * 	[90/06/05            rwd]
 * 
 * Revision 2.5  90/06/19  23:06:44  rpd
 * 	Minor changes for greater portability.
 * 	[90/06/10            rpd]
 * 
 * Revision 2.4  90/06/02  15:20:35  rpd
 * 	Converted new functions to new IPC.
 * 	[90/06/01            rpd]
 * 
 * 	Fixed e_checksignals to take STRC/SRMT into account.
 * 	[90/05/13            rpd]
 * 
 * 	Removed us_sigcheck, e_check_server_signals.
 * 	Fixed e_checksignals to look at us_sig.
 * 	[90/05/10            rpd]
 * 	Updated to new IPC.
 * 	[90/05/04  16:58:37  rpd]
 * 
 * Revision 2.3  90/05/29  20:22:29  rwd
 * 	Fix multi-threaded problem in get_it().
 * 	[90/05/21            rwd]
 * 	Fix potential multi-threaded program bug in e_readwrite.
 * 	[90/05/16            rwd]
 * 	Added flag to turn off mapped operations.
 * 	[90/05/02            rwd]
 * 	Change close to set flag for close and not send message when
 * 	possible.
 * 	[90/04/23            rwd]
 * 	Simplify locking a bit.
 * 	[90/04/19            rwd]
 * 	Allow for > CHUNK_SIZE read/writes.
 * 	[90/04/13            rwd]
 * 	Add new MAP_FILE code.
 * 	[90/03/26            rwd]
 * 
 * Revision 2.2  90/03/14  21:23:04  rwd
 * 	Sigblock should not allow cantmask to be set.
 * 	[90/03/02            rwd]
 * 	Changed shared locks to use share_lock macros.  Added
 * 	share_unlock_solid.
 * 	[90/02/16            rwd]
 * 	Created
 * 	[90/01/31            rwd]
 * 
 * $EndLog 
 */
/*
 * Routines which use mapped area of uarea instead of sending message
 * to server
 */

#ifdef MAP_UAREA
#include <uxkern/import_mach.h>
#include <uxkern/bsd_msg.h>
#include <mach/machine/vm_param.h>
#include <machine/machparam.h>
#include <sys/types.h>
#include <sys/ushared.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <machine/vmparam.h>
#include "emul.h"

int enable_sharing = 1;
int shared_enabled = 0;
struct ushared_ro *shared_base_ro;
struct ushared_rw *shared_base_rw;
spin_lock_t in_emulator_lock = 0;

/* Number of pages of memory shared with the server in each of the three
 * categories:
 */
int	io_pages_nb = IO_PAGES_NB;
int	rw_pages_nb, ro_pages_nb;

/*
 * Same as bsd/mach_signal.c
 */

emul_mapped_init()
{
	kern_return_t	result;
	vm_address_t	address;
	vm_size_t	size;
	vm_prot_t	prot;
	vm_prot_t	max_prot;
	vm_inherit_t	inherit;
	boolean_t	shared;
	mach_port_t	object_name;
	vm_offset_t	offset;
	vm_statistics_data_t	vm_stat;

	if (!enable_sharing)
		return;

	rw_pages_nb = ((vm_offset_t)sizeof(struct ushared_rw) + 
		       (vm_page_size - 1)) / vm_page_size;
	ro_pages_nb = ((vm_offset_t)sizeof(struct ushared_ro) +
		       (vm_page_size - 1)) / vm_page_size;

	shared_base_ro = (struct ushared_ro *)
		(EMULATOR_END - (ro_pages_nb * vm_page_size));
	shared_base_rw = (struct ushared_rw *)
		(EMULATOR_END - ((ro_pages_nb + rw_pages_nb) * vm_page_size));

	current_pid = shared_base_ro->us_pid;	/* set as soon as we know */

	address = (vm_address_t) shared_base_ro;
	result = vm_region(mach_task_self(), &address, &size,
			   &prot, &max_prot, &inherit, &shared,
			   &object_name, &offset);
	if (result != KERN_SUCCESS) {
		e_emulator_error("vm_region ret = %x",result);
		return;
	}
	if (!(prot & VM_PROT_READ)) {
		e_emulator_error("shared region not readable");
		return;
	}
	if (shared_base_ro->us_version != USHARED_VERSION) {
		e_emulator_error("shared region mismatch %x/%x",
			shared_base_ro->us_version, USHARED_VERSION);
		return;
	}

	if (shared_base_ro->us_pid == 1) {
		/* first process: print some info on the shared memory */
		e_emulator_error("sharing with server: IO(%dp), RW(%db/%dp), RO(%db/%dp)", io_pages_nb, sizeof(struct ushared_rw), rw_pages_nb, sizeof(struct ushared_ro), ro_pages_nb);
	}
	shared_enabled = 1;
	shared_base_rw->us_inuse = 1;
	shared_base_rw->us_debug = 0;
	spin_lock_init(&in_emulator_lock);
}

#define E_DECLARE(routine) \
int routine(serv_port, interrupt, syscode, argp, rvalp) \
	mach_port_t	serv_port; \
	boolean_t	*interrupt; \
	int	syscode; \
	int	* argp; \
	int	* rvalp; \
{ \

#define E_IF \
    if (shared_enabled) {

#define E_END(status) \
end:\
	e_checksignals(interrupt); \
	return (status); \
    } else { \
server: \
	return emul_generic(serv_port, interrupt, syscode, argp, rvalp); \
    } \
} \



E_DECLARE(e_obreak)
	vm_offset_t	old, new;
	kern_return_t	ret;
E_IF
	new = round_page(argp[0]);
	if ((int)(new - (vm_offset_t)shared_base_ro->us_data_start) >
	    shared_base_ro->us_rlimit[RLIMIT_DATA].rlim_cur) {
		return ENOMEM;
	}
#ifdef	SERVER_DOES_SBRK
	goto server;
#endif
	old = round_page(shared_base_ro->us_data_start + 
			 ctob(shared_base_rw->us_dsize));
	if (new > old) {
		ret = vm_allocate(mach_task_self(), &old, new - old, FALSE);
		if (ret != KERN_SUCCESS) {
			e_emulator_error("obreak: vm_allocate %x old %x size %d",
				ret, old, new-old);
			goto server;
		} else
			shared_base_rw->us_dsize += btoc(new - old);
	}
E_END(ESUCCESS)


E_DECLARE(e_getdtablesize)
E_IF
	rvalp[0] = shared_base_ro->us_nofile;
E_END(ESUCCESS)


E_DECLARE(e_getuid)
E_IF
E_END(EINVAL)


E_DECLARE(e_getgid)
E_IF
E_END(EINVAL)


E_DECLARE(e_getrlimit)
	struct rlimit *rlp = (struct rlimit *)argp[1];
E_IF
	if (argp[0] >= RLIM_NLIMITS) return EINVAL;
	*rlp = shared_base_ro->us_rlimit[argp[0]];
E_END(ESUCCESS)


E_DECLARE(e_osigblock)
E_IF
	share_lock(&shared_base_rw->us_siglock);
	rvalp[0] = shared_base_rw->us_sigmask;
	shared_base_rw->us_sigmask |= argp[0] &~ cantmask;
	share_unlock(&shared_base_rw->us_siglock);
E_END(ESUCCESS)

E_DECLARE(e_sigpending)
E_IF
	share_lock(&shared_base_rw->us_siglock);
	rvalp[0] = shared_base_rw->us_sig;
	share_unlock(&shared_base_rw->us_siglock);
E_END(ESUCCESS)

E_DECLARE(e_sigprocmask)
E_IF
        int how = (int) argp[0];
        sigset_t mask = (sigset_t) argp[1];
        share_lock(&shared_base_rw->us_siglock);
        rvalp[0] = shared_base_rw->us_sigmask;
        switch (how) {
	      case SIG_BLOCK:
		shared_base_rw->us_sigmask |= mask &~ sigcantmask;
		break;
	      case SIG_UNBLOCK:
		shared_base_rw->us_sigmask &= ~mask;
		break;
	      case SIG_SETMASK:
		shared_base_rw->us_sigmask = mask &~ sigcantmask;
		break;
	      default:
		share_unlock(&shared_base_rw->us_siglock);
		return EINVAL;
	}
        share_unlock(&shared_base_rw->us_siglock);
E_END(ESUCCESS)

E_DECLARE(e_osigsetmask)

E_IF
	share_lock(&shared_base_rw->us_siglock);

	rvalp[0] = shared_base_rw->us_sigmask;
	shared_base_rw->us_sigmask = argp[0] &~ cantmask;

	share_unlock(&shared_base_rw->us_siglock);

E_END(ESUCCESS)

share_lock(x)
register struct shared_lock *x;
{
	int i=0;
	extern boolean_t must_suspend;

	while(!spin_try_lock(&(x)->lock)) {
	    /*
	     * task suspension barrier
	     */
	    if (must_suspend) {
		e_emulator_error("share_lock must_suspend");
		/* XXX? */
	    }

#ifdef DEBUG
	    if (++i % 1024 == 0)
		e_emulator_error("share_lock failure %d", i);
#endif /* DEBUG */
	    swtch_pri(0);
	}
	(x)->who = shared_base_ro->us_proc_pointer & ~KERNEL_USER;
}

share_unlock(x)
register struct shared_lock *x;
{
    (x)->who = 0;
    spin_unlock(&(x)->lock);
    if ((x)->need_wakeup)
	bsd_share_wakeup(our_bsd_server_port, (int)x - (int)shared_base_rw);
}

e_shared_sigreturn(proc_port, interrupt, old_on_stack, old_sigmask)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		old_on_stack;
	int		old_sigmask;
{
	share_lock(&shared_base_rw->us_lock);
	shared_base_rw->us_sigstack.ss_onstack = old_on_stack & 01;
	share_unlock(&shared_base_rw->us_lock);

	share_lock(&shared_base_rw->us_siglock);
	shared_base_rw->us_sigmask = old_sigmask & ~cantmask;
	share_unlock(&shared_base_rw->us_siglock);

	e_checksignals(interrupt);
}

e_checksignals(interrupt)
	boolean_t	*interrupt;
{
	if (shared_enabled) {
		/*
		 *	This is really just a hint; so the lack
		 *	of locking isn't important.
		 */

		if (shared_base_ro->us_cursig ||
		    (shared_base_rw->us_sig &~
		     (shared_base_rw->us_sigmask |
		      ((shared_base_ro->us_flag&STRC) ?
		       0 : shared_base_rw->us_sigignore))))
			*interrupt = TRUE;
	}
}

#endif	MAP_UAREA
