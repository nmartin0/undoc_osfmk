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
 * $Log: ux_exception.c,v $
 * Revision 1.14  1995/03/03  02:43:45  stans
 *  Lint picking
 *
 *  Reviewer:self
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW07 sats
 *
 * Revision 1.13  1994/12/06  20:58:36  yazz
 *  Reviewer: Johnb Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 11363
 *  Testing: controlc EAT
 *  Module(s): server/uxkern/ux_exception.c, server/uxkern/ux_server_loop.c
 * Make sure that the server's exception thread is wired.
 *
 * Revision 1.12  1994/11/18  20:49:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/09/13  18:31:16  jlitvin
 * Author of change: Bob Yasi (yazz)
 * Reviewer(s): Brent Olsen, Bob Yasi
 * Benefit or PTS #: #10851
 *       Exception routine's crucial panic info lost by server, harms PTS
 *       reports.
 * Testing: Cause an catch_exception_raise panic and view the messages.
 * Module(s):
 * 	server/bsd/subr_prf.c
 * 	server/uxkern/ux_exception.c
 *
 * Incorporate crucial information into the official panic message produced
 * when a catch_exception_raise panic occurs.  Added additional printf's.
 *
 * Revision 1.10  1994/05/16  17:49:33  cfj
 * Translate KERN_PROTECTION_FAILURE to SIGSEGV instead of SIGBUS.
 *
 *  Reviewer:suri
 *  Risk:L
 *  Benefit or PTS #:6156
 *  Testing:Test case.
 *  Module(s):server/uxkern/ux_exception.c
 *            server/i860/bsd_machdep.c
 *
 * Revision 1.9  1994/01/13  21:07:12  cfj
 * Merge R1_2 bug fix.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8.2.1  1994/01/13  21:03:44  cfj
 * Cause the svr_exception_thread to be created even in the
 * non-MAPPED_FILE case.
 *
 *  Reviewer:jlitvin
 *  Risk:L
 *  Benefit or PTS #:7782.  When a server gets an exception on a compute node,
 * 		  a dead body will be left to examine.
 *  Testing:
 *  Module(s):server/bsd/init_main.c
 * 	   server/uxkern/ux_exception.c
 *
 * Revision 1.8  1993/11/12  15:20:12  cfj
 * Change the catch_exception_raise() panic so that the program counter of
 * the thread which took the exception is printed out.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.7  1993/07/14  18:45:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/10  18:26:48  cfj
 * Remove call to ux_thread_wire().
 *
 * Revision 1.5  1993/05/06  19:34:09  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:12:41  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:54  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:06:53  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:56:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:45:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.6  92/11/17  19:53:47  loverso
 * 	Added svr_exception_thread(), to create a port and handle server
 * 	exceptions.  Modified catch_exception_raise() to handle the one
 * 	server exception we're interested in (ENOSPC) and otherwise send
 * 	the exception on to the original exception port.  (mmp)
 * 
 * Revision 2.5  92/08/26  14:19:46  loverso
 * 	Set ux_signal for EXC_UNIX_ENOSPC and EXC_UNIX_EFBIG to force 
 * Revision 1.1.1.1  1993/05/03  17:54:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.8  1993/04/08  11:43:33  loverso
 * 	ux server threads are wired by default. (loverso)
 *
 * Revision 2.7  92/12/11  13:56:08  mmp
 * 	Fix for previous change: if orig_svr_exception_port isn't a
 * 	valid port, panic instead.  (mmp)
 * 
 * Revision 2.6  92/11/17  19:53:47  loverso
 * 	Added svr_exception_thread(), to create a port and handle server
 * 	exceptions.  Modified catch_exception_raise() to handle the one
 * 	server exception we're interested in (ENOSPC) and otherwise send
 * 	the exception on to the original exception port.  (mmp)
 * 
 * Revision 2.5  92/08/26  14:19:46  loverso
 * 	Set ux_signal for EXC_UNIX_ENOSPC and EXC_UNIX_EFBIG to force 
 * 	thread_psignal to be called.
 * 	[92/08/26            roy]
 * 
 * Revision 2.4  92/07/28  20:00:26  rabii
 * 	Added handling of EXC_UNIX_EFBIG and EXC_UNIX_ENOSPC (rabii)
 * 
 * Revision 2.3  92/05/24  14:00:46  pjg
 * 	In ux_handler_init, don't set the task exception port for user
 * 	processes because we may be a file server. Just return the exception
 * 	port and let the caller do it.
 * 
 * 	Revision 3.4  92/03/31  15:41:35  emcmanus
 * 	Catch exceptions for Unix tasks, but not for the server itself.
 * 
 * 	Revision 3.3  92/03/24  21:04:35  barbou
 * 	Fixed panic messages...
 * 
 * 	Revision 3.2  92/03/23  18:04:55  condict
 * 	Changes to simplify.
 * 
 * 	Revision 3.1  92/03/13  15:19:15  condict
 * 	Change include dir for ux_exception.h
 * 
 * Revision 2.2  91/08/31  14:29:23  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:06:27  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  90/06/02  15:28:54  rpd
 * 	Converted to new IPC.
 * 	Deallocate task and thread ports.
 * 	Added EXC_UNIX_ABORT.
 * 	[90/03/26  20:27:02  rpd]
 * 
 * Revision 2.2  89/10/17  11:27:35  rwd
 * 	Remove separate ux_exception thread.  Handle exception messages
 * 	by normal server threads.
 * 	[89/09/21            dbg]
 * 
 * 	Out-of-kernel version (NO CONDITIONALS!).  Runs in same task as
 * 	rest of UX kernel.
 * 	[89/01/06            dbg]
 * 
 * Revision 2.1  89/08/04  14:50:25  rwd
 * Created.
 * 
 * Revision 2.6  88/09/25  22:16:55  rpd
 * 	Changed includes to the new style.
 * 	Changed to use object_copyin instead of port_copyin,
 * 	and eliminated use of PORT_INVALID.
 * 	[88/09/24  18:14:52  rpd]
 * 
 * Revision 2.5  88/08/06  19:22:38  rpd
 * Eliminated use of kern/mach_ipc_defs.h.
 * 
 * Revision 2.4  88/07/20  21:08:20  rpd
 * Modified to use a port set to eat notifications; this fixes a message leak.
 * Also will now send error messages in response to bogus requests.
 * 
 * Revision 2.3  88/07/17  19:30:39  mwyoung
 * Change use of kernel_only to new kernel_vm_space.
 *
 * 10-Mar-88  David Black (dlb) at Carnegie-Mellon University
 *	Check error returns from port_copyin().
 *
 * 29-Dec-87  David Golub (dbg) at Carnegie-Mellon University
 *	Delinted.
 *
 *  8-Dec-87  David Black (dlb) at Carnegie-Mellon University
 *	Rewrite to use local port names and internal kernel rpcs.
 *
 *  4-Dec-87  David Black (dlb) at Carnegie-Mellon University
 *	Update to exc interface.  Set ipc_kernel in handler thread.
 *	Deallocate thread/task references now returned from convert
 *	routines.
 *
 * 30-Nov-87  David Black (dlb) at Carnegie-Mellon University
 *	Split unix-dependent stuff into this separate file.
 *
 * 30-Oct-87  David Black (dlb) at Carnegie-Mellon University
 *	Get port references right.
 *
 * 19-Oct-87  David Black (dlb) at Carnegie-Mellon University
 *	Removed port_copyout to kernel_task.  mach_ipc.c has been fixed
 *	to allow kernel to send to any port.
 *
 *  1-Oct-87  David Black (dlb) at Carnegie-Mellon University
 *	Created
 *
 **********************************************************************
 */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>

#include <mach/exception.h>
#include <builtin/ux_exception.h>

#include <uxkern/import_mach.h>

#include <mapped_files.h>

/*
 *	Unix exception handler.
 */

any_t		ux_handler();	/* forward */
extern void	ux_create_thread();

/*
 * Sets the exception port to map exceptions to signals.
 */
mach_port_t ux_handler_init()
{
	register kern_return_t	r;
	static mach_port_t	ux_local_port;

	/*
	 *	Allocate the exception port.
	 */
	r = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			       &ux_local_port);
	if (r != KERN_SUCCESS)
		panic("ux_handler_init: can't allocate");

	r = mach_port_insert_right(mach_task_self(),
				   ux_local_port, ux_local_port,
				   MACH_MSG_TYPE_MAKE_SEND);
	if (r != KERN_SUCCESS)
		panic("ux_handler_init: can't acquire send right");

	/*
	 * Add it to the server port set.
	 */
	ux_server_add_port(ux_local_port);

	/*
	 * Return the exception port.
	 */

	return (ux_local_port);
}

void	ux_exception();	/* forward */

mach_port_t	svr_exception_port;
mach_port_t	orig_svr_exception_port;

kern_return_t
catch_exception_raise(exception_port, thread, task,
	exception, code, subcode)
	mach_port_t	exception_port;
	thread_t	thread;
	task_t		task;
	int		exception, code, subcode;
{
	int	signal = 0, ux_code = 0, pc = 0;
	int	ret = KERN_SUCCESS;

#ifdef	lint
	exception_port++;
#endif	lint

	/*
	 *	Catch bogus ports
	 */
	if (MACH_PORT_VALID(task) && MACH_PORT_VALID(thread)) {

		if (task == mach_task_self()) {
			/* catch server's own exceptions */
#if	MAPPED_FILES
			if ((exception == EXC_BAD_ACCESS) &&
			    (code == EXC_UNIX_ENOSPC)) {
				/* ENOSPC for nfs server */
				signal = SIGSEGV;
				ux_code = ENOSPC;
			} else
#endif
			{
				if (MACH_PORT_VALID(orig_svr_exception_port))
					/* pass to original exception port */
					exception_raise(orig_svr_exception_port,
						thread, task, exception, code,
						subcode);
				else {
					get_thread_pc(thread, &pc);
					panic("catch_exception_raise() caught server exception:\n\tthread=0x%x exception=%d code=%d subcode/badaddr=0x%x pc=0x%x\n\t(see thread(s) in exception_raise_continue)",
					thread, exception, code, subcode, pc);
				}
			}
		} else
			/*
			 * Convert exception to unix signal and code.
			 */
			ux_exception(exception, code, subcode, &signal,
				&ux_code);

		/*
		 * Send signal.
		 */
		if (signal != 0) {
			thread_psignal(task, thread, signal, ux_code);
		}
	} else {
	    printf("catch_exception_raise: task %x thread %x\n",
		   task, thread);
	    ret = KERN_INVALID_ARGUMENT;
	}

	if (MACH_PORT_VALID(task))
		(void) mach_port_deallocate(mach_task_self(), task);

	if (MACH_PORT_VALID(thread))
		(void) mach_port_deallocate(mach_task_self(), thread);

	return(ret);
}

extern boolean_t	machine_exception();

/*
 *	ux_exception translates a mach exception, code and subcode to
 *	a signal and u.u_code.  Calls machine_exception (machine dependent)
 *	to attempt translation first.
 */

void ux_exception(exception, code, subcode, ux_signal, ux_code)
int	exception, code, subcode;
int	*ux_signal, *ux_code;
{
	/*
	 *	Try machine-dependent translation first.
	 */
	if (machine_exception(exception, code, subcode, ux_signal, 
	    ux_code))
		return;
	
	switch(exception) {

	    case EXC_BAD_ACCESS:
		switch (code) {
		    case EXC_UNIX_EFBIG:
		    	*ux_signal = SIGSEGV;
			*ux_code = EFBIG;
			break;
		    case EXC_UNIX_ENOSPC:
		    	*ux_signal = SIGSEGV;
			*ux_code = ENOSPC;
			break;
		    case KERN_INVALID_ADDRESS:
		    case KERN_PROTECTION_FAILURE:
			*ux_signal = SIGSEGV;
			*ux_code = EFAULT;
			break;
		    default:
			*ux_signal = SIGBUS;
			*ux_code = EFAULT;
		}
		break;

	    case EXC_BAD_INSTRUCTION:
	        *ux_signal = SIGILL;
		break;

	    case EXC_ARITHMETIC:
	        *ux_signal = SIGFPE;
		break;

	    case EXC_EMULATION:
		*ux_signal = SIGEMT;
		break;

	    case EXC_SOFTWARE:
		switch (code) {
		    case EXC_UNIX_BAD_SYSCALL:
			*ux_signal = SIGSYS;
			break;
		    case EXC_UNIX_BAD_PIPE:
		    	*ux_signal = SIGPIPE;
			break;
		    case EXC_UNIX_ABORT:
			*ux_signal = SIGABRT;
			break;
		}
		break;

	    case EXC_BREAKPOINT:
		*ux_signal = SIGTRAP;
		break;

	}
}

void
svr_exception_thread()
{
	register kern_return_t	r;
	union excep_msg {
		mach_msg_header_t	hdr;
		mig_reply_header_t	reply_hdr;
		char			space[8192];
	} excep_msg, reply_msg;

	cthread_set_name(cthread_self(), "server exception thread");

	/*
	 *	Allocate the exception port.
	 */
	r = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			       &svr_exception_port);
	if (r != KERN_SUCCESS)
		panic("svr_exception_thread: can't allocate port");

	r = mach_port_insert_right(mach_task_self(),
				   svr_exception_port, svr_exception_port,
				   MACH_MSG_TYPE_MAKE_SEND);
	if (r != KERN_SUCCESS)
		panic("svr_exception_thread: can't aquire send right");

	/*
	 * get and save the original exception port, so exceptions not
	 * handled by this thread can be passed on to the original port.
	 */
	r = task_get_exception_port(mach_task_self(), &orig_svr_exception_port);
	if (r != KERN_SUCCESS)
		panic("svr_exception_thread: can't get original exception port");

	/* set server task's exception port so this thread gets exceptions */
	r = task_set_exception_port(mach_task_self(), svr_exception_port);
	if (r != KERN_SUCCESS)
		panic("svr_exception_thread: can't set exception port");

	ux_thread_wire();		/* wire server's exception thread */

	while (1) {
		r = mach_msg(&excep_msg.hdr, MACH_RCV_MSG, 0,
			sizeof excep_msg, svr_exception_port,
			MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
		if (r != MACH_MSG_SUCCESS) {
			printf("svr_exception_thread: mach_msg failed %d\n", r);
			continue;
		}
		if (!exc_server(&excep_msg.hdr, &reply_msg.reply_hdr))
			printf("svr_exception_thread: received invalid message\n");
		mach_msg_send(&reply_msg.hdr);
	}
}
