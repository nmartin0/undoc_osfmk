/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 */

/*
 * $Id: nx_port.c,v 1.38 1995/03/17 23:42:24 joel Exp $
 *
 * NX kernel ports' functions. 
 *
 *	The two NX kernel ports allow the kernel to communicate asynchronously
 *	to the user level application.
 *
 *	The first NX kernel port is used to implement:
 *		1)  hsend() and hrecv()
 *
 *	The second NX kernel port is used to implement:
 *		1)  Message passing in pageable VM
 *		2)  xmsg copy to user's buffer and receive continue on blocked receive
 *
 * HISTORY
 * $Log: nx_port.c,v $
 * Revision 1.38  1995/03/17  23:42:24  joel
 *  Reviewer: 	sean
 *  Risk: 		low
 *  Benefit or PTS #: 12754   Fix for PTS 12716 broke all -plk programs
 *  Testing:	PTS 12646 test case, message EATs, parallel and paranoia SATs
 *  Module(s):	nx_port.c
 *
 * Revision 1.37  1995/03/14  22:39:44  joel
 *  Reviewer: none
 *  Risk: very low
 *  Benefit or PTS #: 12716   Moved declarations for mcmsg_process_lock and
 * 			nxport_wire around to correctly resolve references
 * 			for apps links with libc_r.a but not with libnx.a
 * 			Bug introduced in last fix.
 *  Testing: paragnoia SAT and parallel SATs
 *  Module(s): include/nx/mcmsglib.s and libnx/nx_port.c
 *
 * Revision 1.36  1995/03/10  23:12:43  joel
 *  Reviewer:      sean, tnt
 *  Risk:          low
 *  Benefit or PTS #: 12646  message passing in hrecv() handler with -plk
 *                           fails with "Not enough space"
 *                 the library was trying to wire down all memory between
 *                 the main thread stack and handler thread stack due to
 *                 the local variables in the handler, fixed by adding locking
 *                 code for the handler stack. Also deleted return statement
 * 		from function that never returns and is type void *.
 *  Testing:       developer test case, handlerproc test, message EATs,
 *                 parallel SATs
 *  Module(s):     nxlib.c and nx_port.c
 *
 * Revision 1.35  1994/11/19  02:31:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.34  1994/11/15  21:49:49  tnt
 *  PTS #: 11515
 *  Mandatory?:  No (H1)
 *  Description: Fixed problem with allocating 511MB thread stacks
 *               in NX application.
 *  Reviewer(s): None
 *  Risk:        Low
 *  Testing:     Developer test; Control-C, Message Passing, Pthreads,
 *               Misc, and Rmcall EATs at 128MB stacksize.  Some testing
 *               at 511MB stacksize.
 *  Module(s):   libnx/nx_port.c
 *
 * Revision 1.33  1994/10/25  18:14:28  tnt
 *  PTS #: 10245
 *  Mandatory?:  Yes
 *  Description: Changed nx thread stack allocation to begin just below
 *               the emulator and walk down through memory.
 *  Reviewer(s): Shane Story
 *  Risk:        Medium
 *  Testing:     Developer test; Control-C, Message Passing, Pthreads,
 *               and Os_interfaces EATs.
 *  Module(s):   libnx/nx_port.c
 *
 * Revision 1.32  1994/08/31  20:22:38  bradf
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
 * Revision 1.31.2.1  1994/08/30  16:13:38  terry
 * This fixes the global send into hrecv().
 *
 *  Reviewer: Greg Regnier
 *  Risk: low
 *  Benefit or PTS #: 10712
 *  Testing: Message Passing Eats
 *  Module(s): nx_port.c
 *
 * Revision 1.31  1994/06/21  23:02:08  tnt
 *  Reviewers: Terry Prickett, Greg Regnier
 *  Risk:  Low
 *  Benefit or PTS #: 7605; Consolidated the pthread specific code in nx_port.c
 *                    so that only the nx_port.o object need be included in
 *                    libc_r.a for pthread nx threads.
 *
 *  Testing: Message Passing EATs, TAMU ipx code.
 *  Module(s): nxlib.c, nx_port.c, libc_r/Makefile, libpthreads/pthread.c
 *
 * Moved masktrap() into nx_port.c.
 *
 * Revision 1.30  1994/06/17  21:13:37  terry
 * This fixes a problem with global csend to hrecv()s.
 *
 *  Reviewer: tnt, regnier
 *  Risk: low
 *  Benefit or PTS #: 8791
 *  Testing: Message passing eats, and a couple of test cases
 *  Module(s): nx_port.c
 *
 * Revision 1.29  1994/06/16  17:59:48  tnt
 *  Reviewers: Greg Regnier, Terry Prickett
 *  Risk:  Low
 *  Benefit or PTS #: 7605; Create the hrecv/hsend and vm paging/receive-continue
 *                    threads as pthreads in a pthread application.  When linking
 *                    with libpthreads and libc_r, the nxlib.o and nx_port.o
 *                    objects will be retrieved from libc_r.  The libnx.a objects
 *                    are unchanged and still create mach threads.
 *  Testing: Message Passing EATs, TAMU ipx code, lat, lath.
 *  Module(s): nxlib.c, nx_port.c
 *
 * Revision 1.28  1994/02/02  19:32:32  tnt
 *  Reviewers: Greg Regnier, Terry Prickett
 *  Risk: Medium
 *  Benefit or PTS #: 7785,7241; Main thread no longer suspended by hrecv thread.
 *                    The two threads now run independently, except for
 *                    synchronization via the nx and masktrap locks.
 *  Testing: Message Passing, Pthreads, Controlc, Unix, and Fileio EATs,
 *           Parallel SATs (except pfs, slab, cg), irxnode, TAMU.
 *  Modules: nx_port.c, nxlib.c
 *
 * Removed the main thread suspend.  The hrecv thread grabs the masktrap
 * lock before proceeding to call the user's hrecv handler.  This prevents
 * the main thread from successfully completing masktrap() and proceeding
 * into a critical section while the hrecv thread is executing in the user's
 * handler.  If the main thread attempts masktrap(), it will block until
 * the hrecv thread completes and unlocks the masktrap lock.
 *
 * Revision 1.25.2.2  1993/12/17  23:23:12  tnt
 *  Reviewer: Terry Prickett, Greg Regnier
 *  Risk: Low
 *  Benefit or PTS #: 7519; Created unique spin lock for masktrap so that the
 *                    instrumentation tools may call masktrap within the NX library.
 *  Testing: Message Passing EATs
 *  Modules: nxlib.c, nx_lock.c
 *
 * Can't suspend the main thread if it has the masktrap lock.  If it does let
 * it resume.
 *
 * Revision 1.25.2.1  1993/12/08  18:21:31  tnt
 *  Reviewers: Greg Regnier, Paul Pierce
 *  Risk:      Low
 *  Benefit or PTS #: 7216; Fixes race between threads when updating the index
 *                    for the mcmsg post page.  Also contains a fix to bug
 *                    causing corruption of the xmsg buffer queues.
 *  Testing:       TAMU, Message Passing EATs, irxnode.
 *  Module(s): mcmsglib.s, nx_port.c
 *
 *  Merge main line revision 1.26 into the R1_2 branch.
 *
 * Revision 1.26  1993/12/03  00:07:24  tnt
 *  Reviewers: Greg Regnier, Paul Pierce
 *  Risk:      Low
 *  Benefit or PTS #: 7216; Fixes race between threads when updating the index
 *                    for the mcmsg post page.  Also contains a fix to bug
 *                    causing corruption of the xmsg buffer queues.
 *  Testing:	TAMU, Message Passing EATs, irxnode.
 *  Module(s): mcmsglib.s, nx_port.c
 *
 * Added nx spin lock around the _fulfill_nx_xmsg() call to prevent corruption
 * of the xmsg buffer queues.  Added check to see if the main thread is in the
 * mcmsg_post routine after it has been suspended.  If it is then resume the
 * thread until it is no longer in the mcmsg_post routine.  This is necessary
 * to support the new code in mcmsg_post to lock the index to the mcmsg post
 * page while updating it.  Otherwise, if we suspend the main thread around
 * an interlocked instruction sequence, the thread never resumes correctly
 * after the hrecv thread has executed the mcmsg_post routine.  This may be
 * a microkernel problem with thread continuation at the end of an
 * interlocked instruction sequence.
 *
 * Revision 1.25  1993/11/18  19:25:13  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.24  1993/11/02  22:18:45  tnt
 *   Reviewer: Greg Regnier
 *   Risk:     Low
 *   Benefit:  Run-time allocation of NX threads' stacks.  Enables detection
 *             of stack overflow in an NX thread stack.
 *   Testing:  Message EATs on 66 nodes with MCP on.
 *   Module:   nx_port.c
 *
 * Allocate the two NX threads' stacks at run-time.  Then write protect the bottom
 * of each stack, so that if stack overflow occurs the application will terminate
 * with a bus error, rather than silently overwriting adjacent data areas.
 *
 * Revision 1.23  1993/10/14  17:45:57  tnt
 * Added code to create and start second NX port thread.  The new thread handles
 * the new receive continue functions, and the existing vm paging function.  The
 * vm paging had to be moved out of the hrecv thread to prevent deadlock in the
 * user's hrecv handler on a vm paging request.
 *
 * Revision 1.22  1993/08/24  15:05:44  terry
 * This corrects bug 6251 which was that the PERFMON_INTERRUPT_START() and
 * PERFMON_INTERRUPT_END() where located in the wrong spot in the code.
 *
 * Revision 1.21  1993/08/19  15:17:53  terry
 * Fixed two hrecv problems.  The first is the masktrap now correctly works and
 * gives mutual exclusion and the second now allows the hrecv handler to exit
 * correctly.  This also fixes other resource problems that the suspend of the
 * main thread may have caused.
 *
 * Revision 1.20  1993/08/13  18:43:43  regnier
 * Fix Paragraph problem with hsends causing receive events. PTS #6110.
 *
 * Revision 1.19  1993/08/04  20:25:48  regnier
 * Added PERFMON hooks for handler time and hrecvs. Added lock parameter
 * to _probe_nxreq().
 *
 * Revision 1.18  1993/07/13  01:06:47  regnier
 * Fixed typo from previous rev. which broke hsend/recv.
 *
 * Revision 1.17  1993/07/01  18:00:21  regnier
 * Add infocopy parameter to _probe_nxreq() to fix multiple receive
 * events for PERFMON support. Also set info pointer to NULL for hrecv
 * and hrecvx hopefully to fix Texas A&M problem.
 *
 * Revision 1.16  1993/06/23  17:37:09  terry
 * made second thread`s stack 16-byte aligned.
 *
 * Revision 1.15  1993/06/07  22:48:55  terry
 * change spin_lock_init, spin_lock and spin_unlock to nx_spin_lock_init,
 * nx_spin_lock and nx_spin_unlock respectively.  Also change nx_spin_lock and
 * nx_spin_unlock to _nx_spin_lock and _nx_spin_unlock respectively to avoid
 * a conflict with names in cthreads.
 *
 * Revision 1.14  1993/05/24  18:23:10  terry
 * Get lock before suspending the main thread.
 *
 * Revision 1.13  1993/05/19  16:07:26  regnier
 * Merge back fixes that were lost from R1.0.
 *
 * Revision 1.12  1993/03/23  19:45:40  hays
 * Modified nx_port.c, nxlib.c, and rklib.c for performance monitoring.
 * the macros are in the kernel header file mcmsg_xmsg.h
 *
 * Revision 1.11  1993/02/23  01:51:44  ellend
 * Parallel global send support
 *
 * Revision 1.10  1993/01/25  22:57:37  andrews
 * Fixed global hsend() problem (user handler received node number of last
 * node sent to, rather than -1).
 *
 * Revision 1.9  1992/12/22  19:02:40  regnier
 * Fix random message passing test hang after 50K messages.
 * Also use vm_page_size rather than constant.
 *
 * Revision 1.8  1992/12/16  01:16:32  regnier
 * Minor changes for second round of VM support.
 *
 * Revision 1.7  1992/12/15  23:48:35  ellend
 * Add support for global hsend
 *
 * Revision 1.6  1992/12/11  00:16:41  ellend
 * Make sure and release hrecv mids
 *
 * Revision 1.5  1992/10/28  18:24:55  regnier
 * Fix include path to thread_status.h
 *
 * Initial VM message passing support.
 *
 * Revision 1.4  1992/08/07  10:14:17  regnier
 * Remove unix signal handlers.
 *
 * Revision 1.3  92/08/03  16:54:00  regnier
 * Add signal handler in nxport recv thread.
 * 
 * Revision 1.2  92/07/30  17:02:06  regnier
 * Fix error reporting in nxport thread.
 * 
 * Revision 1.1  92/07/27  11:49:12  regnier
 * Initial revision
 * 
 * 
 */
#include <stdio.h>
#include <i860/psl.h>
#include <mach.h>
#include <mach/boolean.h>
#include <mach/thread_status.h>
#include <mach/i860/vm_types.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <mcmsg/mcmsg_nx.h>
#include <mach/thread_switch.h>
#include <nx/mcmsg_lib.h>
#include <sys/vmparam.h>
#ifdef _THREAD_SAFE
#include <pthread.h>
#else _THREAD_SAFE
#include <sys/resource.h>
#endif _THREAD_SAFE

#ifndef _THREAD_SAFE
#define PAGE_ROUND_DOWN(b)	((b) & ~(vm_page_size - 1))
#define PAGE_ROUND_UP(b)	PAGE_ROUND_DOWN((b) + vm_page_size - 1)
#endif _THREAD_SAFE

/*
 *	Global variables: 
 *
 *		_nxport:	Portname for the hrecv NX kernel port.
 *		_nxvmport:	Portname for the vm paging/receive continue NX kernel port.
 *		_nxport_count: The number of requests for the ports. (debug)
 *		_nxport_pagefaults: Number of pages have we faulted in (debug)
 *					(not accurate, we assume all the pages in the range)
 *		_nxport_thread_stack[]: Stack for the hrecv nxport thread.
 *		_nxvmport_thread_stack[]: Stack for the vm paging/receive continue
 *					nxvmport thread.
 *		nxreq:		The NX request array.
 *		vm_page_size:	The size of a VM page.
 *		mcmsg_process_lock: 	Does this application use "-plk"?
 */

#ifdef _THREAD_SAFE
static pthread_t	thread_hrecv;	/* hrecv thread id - pthread */
#else _THREAD_SAFE
static mach_port_t thread_hrecv;	/* hrecv thread id - mach thread */
#endif _THREAD_SAFE

mach_port_t	_nxport;
mach_port_t	_nxvmport;
long	_nxport_count = 0;
long	_nxport_pagefaults = 0;
vm_address_t	_nxport_thread_stack;
vm_address_t	_nxvmport_thread_stack;
long	_nxport_thread_stack_size = -1;

extern	nxreq_t *nxreq;
extern	vm_size_t vm_page_size;
long mcmsg_process_lock;

unsigned long handler_stack_locked;
unsigned long handler_stack_top;
static vm_address_t get_stack_pointer();

/*
 *	Routine:
 *		nx_port_init
 *
 *	Purpose:
 *		Allocate the NX kernel ports and hand the names to the
 *		task inside the kernel.
 *
 */
nx_port_init()
{
	int result;
#ifdef _THREAD_SAFE
	pthread_t	thread_child;	/* receive-continue thread is a pthread */
#else _THREAD_SAFE
	mach_port_t thread_child;	/* receive-continue thread is a mach thread */
	struct rlimit	limits;
	vm_address_t	sp;
#endif _THREAD_SAFE
	extern mach_port_t mach_task_self();
	void _nx_port_recv_thread();

	/* Allocate the nx hrecv handler port */

	result = mach_port_allocate(mach_task_self(),
				    MACH_PORT_RIGHT_RECEIVE,
				    &_nxport);

	/* inform the kernel */
	if (result == 0) {
		result = mcmsg_nxport_setup(_nxport);
	}

	/* Allocate the nx vm paging and receive continue port */

	result = mach_port_allocate(mach_task_self(),
				    MACH_PORT_RIGHT_RECEIVE,
				    &_nxvmport);

	/* inform the kernel */
	if (result == 0) {
		result = mcmsg_nxport_setup(_nxvmport);
	}

#ifdef	_THREAD_SAFE

	/* Create the hrecv and receive-continue threads as pthreads for
	 * compatibility with user pthreads when making calls into the
	 * reentrant libraries.
	 */
	if (pthread_create(&thread_hrecv,pthread_attr_default,
					(void *)_nx_port_recv_thread,(void *)&_nxport) == -1) {
		return -1;
	}

	if (pthread_create(&thread_child,pthread_attr_default,
					(void *)_nx_port_recv_thread,(void *)&_nxvmport) == -1) {
		return -1;
	}

#else	_THREAD_SAFE

	/*
	 * Find the default stack size. Use the current limit size from
	 * getrlimit().
	 */
	if (getrlimit(RLIMIT_STACK, &limits) != 0) {
		return -1;
	}

	_nxport_thread_stack_size = PAGE_ROUND_UP(limits.rlim_cur);

	/*
	 * Find the base of the initial stack.  The allocation of the
	 * the NX threads' stacks will start from below there.
	 */
	sp = get_stack_pointer();
	sp = sp & ~(_nxport_thread_stack_size - 1);

	/* Allocate the hrecv thread's stack */
	_nxport_thread_stack = sp - _nxport_thread_stack_size;
	while (vm_allocate(mach_task_self(), &_nxport_thread_stack,
			(vm_size_t)_nxport_thread_stack_size,
			(boolean_t)FALSE) != KERN_SUCCESS) {
		_nxport_thread_stack -= vm_page_size;
		if ((unsigned long)_nxport_thread_stack > (unsigned long) VM_MAX_ADDRESS) {
			/* Wrapped around memory - no space found */
			return -1;
		}
	}

	if (vm_protect(mach_task_self(), _nxport_thread_stack,
			(vm_size_t)sizeof(long), (boolean_t)TRUE,
			(vm_prot_t)VM_PROT_READ) != KERN_SUCCESS) {
			return -1;
	}

	/* Allocate the vm paging/receive continue thread's stack */
	_nxvmport_thread_stack = _nxport_thread_stack - _nxport_thread_stack_size;
	while (vm_allocate(mach_task_self(), &_nxvmport_thread_stack,
			(vm_size_t)_nxport_thread_stack_size,
			(boolean_t)FALSE) != KERN_SUCCESS) {
		_nxvmport_thread_stack -= vm_page_size;
		if ((unsigned long)_nxvmport_thread_stack >
				(unsigned long) VM_MAX_ADDRESS) {
			/* Wrapped around memory - no space found */
			return -1;
		}
	}

	if (vm_protect(mach_task_self(), _nxvmport_thread_stack,
			(vm_size_t)sizeof(long), (boolean_t)TRUE,
			(vm_prot_t)VM_PROT_READ) != KERN_SUCCESS) {
		return -1;
	}

	if ((result = _nx_port_thread_start(&_nxport, &thread_hrecv,
			_nxport_thread_stack+_nxport_thread_stack_size)) != 0) {
		return result;
	} else {
		return _nx_port_thread_start(&_nxvmport, &thread_child,
			_nxvmport_thread_stack+_nxport_thread_stack_size);
	}

#endif	_THREAD_SAFE

}

#ifndef	_THREAD_SAFE
/*
 *	Routine:
 *		_nx_port_thread_start
 *
 *	Purpose:
 *		Start a new thread to listen to the given nx port.
 *
 */
_nx_port_thread_start(port, child_thread, stack)
	mach_port_t *port;
	mach_port_t *child_thread;
	vm_address_t stack;
{
	long i, *p;
	int result;		/* S/B kern_return_t */

	void _nx_port_recv_thread();
	struct i860_thread_state state;
	int statecnt = i860_THREAD_STATE_COUNT;
	int flavor=i860_THREAD_STATE;

	/* Create the thread */

	result = thread_create (mach_task_self(), child_thread);

	if (result == 0) {

		/* Setup the state */
	
		for (p = (long *) &state, i=0; i<statecnt; i++) p[i] = 0;
		state.r16 = (unsigned long) port;
		state.psr = (unsigned long) PSR_U | PSR_PU | PSR_IM | PSR_PIM;
		state.pc = (unsigned long) _nx_port_recv_thread;
		state.sp = 0xfffffff0 & (unsigned long) stack;
	
		/* Set state of new thread */
	
		result = thread_set_state(*child_thread,
					  flavor,
					  &state,
					  statecnt);
		if (result == 0) {

			/* Start it */
		
			result = thread_resume(*child_thread);
		}
	}
	return result;
}
#endif	_THREAD_SAFE


/*
 *	Routine:
		nx_port_wire(addr,size);
 *
 *	Purpose:
 *		wire down a range of memory.  This is identical to 
 *		mcmsg_wire() in mcmsglib.s but declared here to reduce
 *		conflicts in libc_r.a generated if you try to put
 *		mcmsglib.s in libc_r.a.  This allows an executable to
 *		be linked with libc_r.a without being linked to libnx.a
 *		
 *		
 *	Returns:
 *		Nothing	
 */
void
nx_port_wire(addr,size)
{
        asm ("or      -140,r0,r31");
        asm ("trap    r31,r31,r0");
}

/*
 *	Routine:
 *		_nx_port_recv_thread  [thread]
 *
 *	Purpose:
 *		Wait for messages on the given nx kernel port and
 *		perform actions based on contents.
 *		
 *	Returns:
 *		Never returns.
 */

int _masktrap_shadow = 0;	/* shadow of kernel version */

void
_nx_port_recv_thread(port)
mach_port_t	*port;
{
	int result;
	unsigned long mid, pmid, smid;
	struct {
		mach_msg_header_t	hdr;
		mach_msg_type_t		type;
		mcmsg_nxport_req_t	req;
	} msg;
	mcmsg_nxport_req_t *req;
	nxreq_t            *locreq;
	unsigned long      send_item;	/* send select item */
	long ptype;
	int ncount;
	int nlist[MAX_GLOBAL];
	struct i860_thread_state       	state;
	unsigned int 	count;
	int i,st;

	/* wire the top page of the hrecv stack for plk programs */
	if ((port == &_nxport) && mcmsg_process_lock) {
		handler_stack_top = 
			((unsigned long)&result | (vm_page_size - 1)) + 1;
		handler_stack_locked = handler_stack_top - vm_page_size;
		nx_port_wire(handler_stack_locked,vm_page_size);
	}

	/* setup receive hdr */

	msg.hdr.msgh_size = sizeof(msg);
	msg.hdr.msgh_local_port = *port;

	msg.type.msgt_name     = MACH_MSG_TYPE_INTEGER_32;
	msg.type.msgt_size     = 32;
	msg.type.msgt_number   = sizeof(mcmsg_nxport_req_t) / sizeof(long);
	msg.type.msgt_inline   = 1;
	msg.type.msgt_longform = 0;
	msg.type.msgt_deallocate = 0;
	msg.type.msgt_unused   = 0;
	
	/* receive */
	while(1) {

		result = mach_msg_receive ( &msg );

		if (result != MACH_MSG_SUCCESS) {
			fprintf (stderr, "nx thread port failure %d\n", result);
		} else {

			req       = &msg.req;
			locreq    = msg.req.nxreq;
			send_item = msg.req.fill1;

			switch (req->op) {
				case NXPORT_OP_INVOKE_HANDLER:

					PERFMON_INTERRUPT_START();

					for (;;) {
						masktrap_spin_lock();
						/*
						 * See if masktrap was set after the
						 * hrecv AST was scheduled, if so then let the
						 * main thread run a while.
						 */
						if (_masktrap_shadow == 0) {
							break;
						}
						masktrap_spin_unlock();
						thread_switch(MACH_PORT_NULL, SWITCH_OPTION_DEPRESS, 10);
					}

					/*
					 * Masktrap still locked - we don't want the user threads
					 * to be able to masktrap(1) and then run assuming that
					 * they've protected their critical sections.
					 */
					if (_probe_nxreq(locreq, 0, 1)) {
						if (locreq->req == NX_RECV_REQ) {
							PERFMON_RECV_INFO(_nx_port_recv_thread, 
						                      locreq->localinfo);
						}
						ptype = locreq->localinfo[3];
						mid = locreq - &nxreq[0];
						if ((locreq->req == NX_RECV_REQ) &&
						    (ptype & GLOBAL_BIT)) {

							/*
							 * Global send, send it on
							 */

							ncount = tree_recv(locreq->localinfo[4], nlist);
							if (ncount > 0) {
								smid = -1;
								for (i = 0; i < ncount; i++) {
									st = _isend_forward(locreq->type,
										locreq->buf,
                           				locreq->bsize,
										nlist[i],
                               			myptype(),
                               			locreq->localinfo[2],
                       					mcmsg_ptype | GLOBAL_BIT);
									if (st == -1) {
										if (smid != -1) {
											_msgignore(smid);
										}
									}
									smid = _msgmerge(smid, st);
								}
								/*
								 * Wait for sends to complete
								 */
								_msgwait(smid);
							} 
                        }
						if (nxreq[mid].link != -1) {
							/*
							 * Was a global operation, need to unlink
							 * the mid from the circular list, can't
							 * invoke handler until all mids satisfied.
							 */
							pmid = mid;
							while (nxreq[pmid].link != mid) {
								pmid = nxreq[pmid].link;
							}
							if (pmid != mid) {
								nxreq[pmid].link = nxreq[mid].link;
								_free_mid(mid);
								goto handler_done;
							}
						}

						/* call the user handler */

						PERFMON_USER_HANDLER_START();
						((void (*)()) (locreq->handler)) 
							(locreq->localinfo[0],
							 locreq->localinfo[1],
							/*
							 * If this is a broadcast handler, 
							 *   nxreq[mid].link will point to itself at this
							 *   point, and we need to set the node # to -1; 
							 * else, if nxreq[mid].link == -1,
							 *   we need to pass the real node number
							 *   to the user handler. 
							 */
							 (nxreq[mid].link==-1) ? locreq->localinfo[2] : -1,
							 locreq->localinfo[3] & (~GLOBAL_BIT),
							 locreq->hparam);
						PERFMON_USER_HANDLER_END();
						_free_mid(mid);
					}
				handler_done:
					/* unlock the masktrap lock */
					masktrap_spin_unlock();
					PERFMON_INTERRUPT_END();
					break;

				case NXPORT_OP_MISSING_PAGE:
					_nx_touch_pages(locreq, send_item);
					if (mcmsg_nx_send_continue(send_item) != 0) {
						fprintf(stderr, "nx thread: Send Continue failed\n");
					}
					break;

				case NXPORT_OP_RECV_CONTINUE:
					nx_spin_lock();
					_fulfill_nx_xmsg(locreq, locreq->xmsg, 0);
					nx_spin_unlock();
					break;

				default:
					fprintf (stderr, "nx thread: INVALID REQ %d\n", req->op);
					break;
			} /* end switch */
		}
		_nxport_count++;
	}
}

/*
 *	Routine:
 *		_nx_touch_pages(nxreq)
 *
 *	Purpose:
 *		Dissect nxreq and determine which and how many
 *		pages need to be resident, then touch them to cause
 *		them to be faulted in.
 *
 */
_nx_touch_pages(nxreq, send_item)
	nxreq_t       *nxreq;		/* kernel/lib use, nx request struct */
	unsigned long send_item;	/* kernel use only, send select item */
{
	char *p, t;
	unsigned long pagecount, pageoffset;
	

	/*
	 * Decide how many pages to touch, do a little look-ahead.
	 */
	pagecount = nxreq->bsize / vm_page_size;
	if (pagecount == 0) {
		pagecount++;
	}
	pageoffset = nxreq->buf % vm_page_size;
	if ((pageoffset + nxreq->bsize) >= vm_page_size) {
		pagecount++;
	}
	if (pagecount > 5) {
		pagecount = 5;
	}

	_nxport_pagefaults += pagecount;

	/*
	 * Touch the pages to fault them in.
	 */

	p = (char *)nxreq->buf;
	while (pagecount--) {
		t = *p;		
		p += vm_page_size;
	}
}

/*
 *	prevmask = _masktrap(mask)
 *
 *	set hrecv/hsend interrupt mask.
 *	returns previous value of mask.
 *		mask = 1 disables 
 *		     = 0 enables
 */

long
_masktrap(mask)
int mask;
{
	int st, tempmask;
#ifdef _THREAD_SAFE
	pthread_t	self = pthread_self();
#else _THREAD_SAFE
	mach_port_t	self = mach_thread_self();
#endif _THREAD_SAFE

	/* The hrecv thread already has the masktrap lock */
	if (self != thread_hrecv) { masktrap_spin_lock(); }

	if (mask == _masktrap_shadow) {
		if (self != thread_hrecv) { masktrap_spin_unlock(); }
		return mask;
	}

	if (mask & ~1) {
		if (self != thread_hrecv) { masktrap_spin_unlock(); }
		return -1;
	}

	st = mcmsg_masktrap(mask);
	if (st == -1) {
		if (self != thread_hrecv) { masktrap_spin_unlock(); }
		return -1;
	}

	tempmask = _masktrap_shadow;
	_masktrap_shadow = mask;
	if (self != thread_hrecv) { masktrap_spin_unlock(); }
	return tempmask;
}

static vm_address_t
get_stack_pointer()
{
    int x;
 
    return((vm_address_t)&x);
}
