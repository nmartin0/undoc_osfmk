/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_mp.c,v 1.45 1995/04/08 00:23:21 jerrie Exp $
 */

/*
 * mcmsg_mp.c
 *
 * Message processor specific code
 */

#include <i860paragon/dp.h>	/* DP_CONTROL0_CLEAR */
#include <i860paragon/spl.h>	/* DP_CONTROL0_CLEAR */
#include <i860paragon/mcmsg/mcmsg_boot.h>	/* BOOT_DELAY */
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_mp.h>
#include <kern/thread.h>
#include <kern/thread_swap.h>
#include <i860/psl.h>
#include <cpus.h>	/* NCPUS */

int	mcmsg_flick_enable;
int	experimental;

#if	KERNEL_POST_TRACE
int	mcmsg_kernel_post_trace[256];
unsigned char mcmsg_kpt_index;
#endif	KERNEL_POST_TRACE

int	*routing_table;
#if MACH_ASSERT
int	routing_table_nodes = 0;
#endif /* MACH_ASSERT */

mcmsg_basic_init()
{
	extern int	ipsc_physnode;
	extern int	_node_self;
	extern char	*getbootenv();
#if	BOOT_DELAY
	extern unsigned long	mcmsg_boot_delay_bt2;
	extern unsigned long	mcmsg_boot_delay_btx;
#endif	BOOT_DELAY

	char *s;

	mcmsg_trace_init();

	_node_self = ipsc_physnode;

	mcmsg_init_send_store();

	if ((s = getbootenv("BOOT_FIRST_NODE"))) {
		paragon_first_node = atoi(s);
	}
	if ((s = getbootenv("BOOT_MESH_X"))) {
		paragon_mesh_x = atoi(s);
	}
	assert(paragon_mesh_x != 0);
	if ((s = getbootenv("BOOT_MESH_Y"))) {
		paragon_mesh_y = atoi(s);
	}
	assert(paragon_mesh_y != 0);

	experimental = getbootint("BOOT_EXPERIMENTAL", 0);
#if   MACH_ASSERT
	mcmsg_flick_enable = getbootint("BOOT_FLICK", 0);
#else MACH_ASSERT
	mcmsg_flick_enable = getbootint("BOOT_FLICK", 1);
#endif  MACH_ASSERT

	mcmsg_send_tail_limit = getbootint("BOOT_SEND_TAIL_LIMIT", 10);

#if	BOOT_DELAY
	mcmsg_boot_delay_bt2 = getbootint("BOOT_DELAY_BT2", 1000000);
	mcmsg_boot_delay_btx = getbootint("BOOT_DELAY_BTX", 250);
#endif	BOOT_DELAY

	mcmsg_hw_init();
	mcmsg_console_init();

}

/*
 * Routine:
 *	mcmsg_nic_check()
 *
 * Purpose:
 *	Checks for A-step NIC jam.
 * 	Called from timer interrupt.
 *	Only fully enabled when NIC tracing is enabled.
 *
 * Returns:
 *	none
 */

extern unsigned long mcmsg_recv_count;
unsigned long	mcmsg_recv_count_check;
unsigned long	mcmsg_recv_count_last;
int		mcmsg_check_flag;

#if	MCMSG && NCPUS == 1
extern char	*mcmsg_mp_assert_file;
extern int	mcmsg_mp_assert_line;
#endif	/* MCMSG && NCPUS == 1 */

mcmsg_nic_check()
{
	nic_reg	t;

	/*
	 * Get current NIC status
	 */

	t.full = NIC.status.full;

	/*
	 * Run the lights
	 */

	if ((t.halfs.hi & NIC_STAT_RMIP) != 0) {
		RED_ON(RED_RMIP);
	} else {
		RED_OFF(RED_RMIP);
	}
	if ((t.halfs.hi & NIC_STAT_XMIP) != 0) {
		RED_ON(RED_XMIP);
	} else {
		RED_OFF(RED_XMIP);
	}

#if	MCMSG && NCPUS == 1
	/*
	 * Check for a message processor assert
	 * Note this panic is independent of MACH_ASSERT.
	 */

	if (mcmsg_mp_assert_file != 0) {
		panic("cpu%d: Detected MCP assertion failure:\n"
			"file \"%s\", line %d\n",
			cpu_number(),
			mcmsg_mp_assert_file, mcmsg_mp_assert_line);
		mcmsg_mp_assert_file = 0;	/* release the MCP */
	}
#endif	/* MCMSG && NCPUS == 1 */

#if    NIC_TRACE && BUMPERS && BURST
	/*
	 * Check if there is a receive message in progress from the MRC
	 * to the NIC and no complete message in the NIC
	 */

	if ((t.halfs.hi & NIC_STAT_RMIP) != 0 &&
	    (t.halfs.lo & NIC_STAT_EOD_IN_NIC) == 0) {

		/*
		 * See if still on the same old incoming message
		 * (We could happen to see the above condition by chance in
		 *  the middle of receiving several distinct messages,
		 *  but if are successfully receiving messages we are
		 *  not really jammed)
		 */

		if (mcmsg_recv_count_check == mcmsg_recv_count) {

			/*
			 * Assert that this condition has not persisted
			 * for a long time
			 */

			assert(mcmsg_check_flag++ != 100);
		} else {

			/*
			 * At least one message has been received successfully
			 * since the last time we saw the error state.
			 * Reset the counter.
			 */

			if (mcmsg_check_flag < 100) {
				mcmsg_check_flag = 0;
			}
		}

		/*
		 * We don't see the error state, bring our check count up
		 * to date with the current receive message count
		 */

		mcmsg_recv_count_check = mcmsg_recv_count;

	}
	mcmsg_recv_count_last = mcmsg_recv_count;
#endif NIC_TRACE && BUMPERS && BURST
}

syscall_mcmsg_myphysnode()
{

	return ipsc_physnode;
}

/*
 * Post page for kernel requests.
 * This page is used for all requests from the kernel on the compute processor.
 * It accepts all calls, including some that user post pages will not accept.
 * The Mach IPC interface goes through here.
 * When user code uses the old system call library for message passing
 * calls, they go through here.
 */

char		mcmsg_kernel_post_page_space[
			(POST_PAGE_SLOTS+1)*sizeof(post_page_t)];
post_page_t	*mcmsg_kernel_post_page;
int		mcmsg_kernel_post_page_in;
int		mcmsg_kernel_post_page_out;

/*
 * Post pages for user requests.
 * There is one page for each user process using the fast library.
 * Only parallel applications can use one of these.
 * These pages only accept NX calls.
 */

post_page_t	*mcmsg_user_post_page[POST_TASK_MAX];
mcmsg_task_t	*mcmsg_user_mcmsg_task[POST_TASK_MAX];
int		mcmsg_user_post_page_out[POST_TASK_MAX];
int		mcmsg_user_post_tasks;

/*
 * mcmsg_init_null()
 *
 * Null initialization procedure for modules which require no initialization.
 */

mcmsg_init_null()
{

	return;
}

/*
 * mcmsg_mp_init()
 *
 * Complete the initialization of message passing code.
 * Called from netipc_network_init late in the kernel bringup sequence.
 */

#define	CPU_NUM_MCP	1	/* The MCP is cpu1 */

mcmsg_mp_init()
{
	char	*s;
	int	i;
	thread_t th;
	int	physnode, num_physnodes;

	extern	int boot_msg_proc;
	extern	void cpu_start(int);
	extern int do_calculate_route(int);

	/*
	 * Setup the routing table that contains the hardware routing
	 * information for sending messages from this node to other nodes.
	 * We have to construct the table this late in the kernel bringup
	 * sequence as we use kalloc() for allocating the table.
	 */
	num_physnodes = paragon_mesh_x * paragon_mesh_y;
	routing_table = (int *) kalloc(num_physnodes * sizeof(int));

	for(physnode=0; physnode < num_physnodes; physnode++) {
		
		routing_table[physnode] = do_calculate_route(physnode);
	}
	assert(routing_table_nodes = num_physnodes);

	/*
	 * Initialize message passing code
	 */

	for (i = 0; i < MCMSG_MODULE_END; i++) {
		mcmsg_init_switch[i]();
	}

	/*
	 * If message processor is to be enabled
	 */
	if (boot_msg_proc) {

		/*
		 * Create the kernel post page
		 */

		assert((POST_PAGE_SLOTS & (POST_PAGE_SLOTS-1)) == 0); /*pwr 2*/
		mcmsg_kernel_post_page = (post_page_t *)mcmsg_kernel_post_page_space;
		mcmsg_kernel_post_page = (post_page_t *)
		 (((unsigned long)mcmsg_kernel_post_page) + (sizeof(post_page_t)-1) &
						     ~(sizeof(post_page_t)-1));
		for (i = 0; i < POST_PAGE_SLOTS; i++) {
			mcmsg_kernel_post_page[i].method = POST_EMPTY;
		}
		mcmsg_kernel_post_page_in = 0;
		mcmsg_kernel_post_page_out = 0;

		/*
		 * Clear the user post page list
		 */

		mcmsg_user_post_tasks = 0;
		for (i = 0; i < POST_TASK_MAX; i++) {
			mcmsg_user_post_page[i] = 0;
			mcmsg_user_mcmsg_task[i] = 0;
			mcmsg_user_post_page_out[i] = 0;
		}
	}

	if (boot_msg_proc) {
		extern void remove_nic_interrupt();
		
		cpu_start(CPU_NUM_MCP);
		remove_nic_interrupt();
	}

	mcmsg_mp_enable = boot_msg_proc;
}

/*
 * syscall_mcmsg_mp_access(post_page)
 *
 * Kernel system call to establish a user post page.
 */

syscall_mcmsg_mp_access(post_page)
	post_page_t	*post_page;
{
	register task_t		task;

	task = current_task();

	/*
	 * Error out if message processor inactive
	 */

	if (!mcmsg_mp_enable) {
		/* debugging */
		mcmsg_user_mcmsg_task[mcmsg_user_post_tasks] = task->mcmsg_task;
		mcmsg_user_post_tasks++;
		return -1;
	}
	/*
	 * Get pointer to mcmsg_task block.
	 * Error out if process is not initialized.
	 */

#if	NX
	if (task->mcmsg_task == 0) {
		return -2;
	}

	/*
	 * Error out if caller is not part of a parallel application
	 */

	if (task->mcmsg_task->applinfo.app == -1) {
		return -3;
	}
#endif	NX

	/*
	 * Wire the page
	 */

	if (mcmsg_wire_buffer(task,
			      post_page,
			      ((long)post_page) + INTEL_PGBYTES-1) != 0) {
		return -4;
	}


	return mcmsg_task_call(	task,
				POST_INSTALL,
				post_page,
				task->map->pmap->dirbase);
}

mcmsg_task_destroy(task)
	task_t		task;
{
	int		st;

	st = 0;
#if	NX
#if	MCMSG_MODULE_VC
	mcmsg_vc_clear_task(task->mcmsg_task);
#endif	MCMSG_MODULE_VC
	st = mcmsg_task_call(task, POST_TASKDEST);
#endif	NX
	task->mcmsg_task = 0;
	return st;
}

/*
 * mcmsg_kernel_post_page_next()
 *
 * Return a pointer to the next "in" entry in the kernel's post page.
 * Waits for the entry to become free.
 * Updates "in" index.
 * Must only be called while at sploff().
 */

post_page_t *
mcmsg_kernel_post_page_next()
{
	register int	in;
	register post_page_t *pp;

	in = mcmsg_kernel_post_page_in;
	mcmsg_kernel_post_page_in = (in + 1) & (POST_PAGE_SLOTS-1);
	pp = &mcmsg_kernel_post_page[in];
	while (pp->method != POST_EMPTY) {
		assert(0);
#if 0
		thread_block(0);
#endif
	}
	return pp;
}

/*
 * mcmsg_post_method(pp, method)
 *
 * Set the method. Likely to fool the optimizer into doing the right thing.
 * pp points to the entry through which the call was made.
 * Must only be called while at sploff().
 */

mcmsg_post_method(pp, method)
	register method;
	register volatile post_page_t *pp;
{

	assert(method != 0);
#if	KERNEL_POST_TRACE
	mcmsg_kernel_post_trace[mcmsg_kpt_index++] = method;
#endif	KERNEL_POST_TRACE
	pp->method = method;
}

/*
 * mcmsg_post_status_wait(pp, method)
 *
 * Set the method. Likely to fool the optimizer into doing the right thing.
 * Wait for a call to return and return status.
 * pp points to the entry through which the call was made.
 * Must only be called while at sploff().
 */

mcmsg_post_status_wait(pp, method)
	register method;
	register volatile post_page_t *pp;
{

	assert(method != 0);
#if	KERNEL_POST_TRACE
	mcmsg_kernel_post_trace[mcmsg_kpt_index++] = method;
#endif	KERNEL_POST_TRACE
	pp->method = method;
	while (pp->method != POST_EMPTY) {
#if 0
		thread_block(0);
#endif
	}
	return pp->status;
}


/*
 * mcmsg_post(method, arg0, ...)
 *
 * Post a request to the message processor if active,
 * otherwise make the call directly.
 * method is a POST_* constant from mcmsg_post.h to specify which call.
 * Calls mcmsg_entry() if necessary.
 * Does not wait for completion.
 */

mcmsg_post(method,
		 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
	int	method;
{
	register volatile post_page_t	*pp;
	register int			x;

	/*
	 * Posting a request *must* be an atomic operation
	 */
	x = sploff();

	if (mcmsg_mp_enable) {
		pp = mcmsg_kernel_post_page_next();
		pp->arg0 = arg0;
		pp->arg1 = arg1;
		pp->arg2 = arg2;
		pp->arg3 = arg3;
		pp->arg4 = arg4;
		pp->arg5 = arg5;
		pp->arg6 = arg6;
		pp->arg7 = arg7;
		pp->arg8 = arg8;
		pp->arg9 = arg9;
		pp->mcmsg_task = 0;
		mcmsg_post_method(pp, method);
	} else {
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry post");
		mcmsg_send_tail_count = 0;
		RED_ON(RED_MSG);
#if	MSG_TRACE
		mcmsg_trace_call(method, arg0, arg1, arg2, arg3, arg4, arg5,
				 arg6, arg7, arg8, arg9);
#endif	MSG_TRACE
		if (method < POST_MAX) {
			mcmsg_post_switch[method](0,
		 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		}
		CHECK_REENTRY("exit  post");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
	}

	splon(x);
	return 0;
}

/*
 * mcmsg_call(method, arg0, ...)
 *
 * Post a request to the message processor if active,
 * otherwise make the call directly.
 * method is a POST_* constant from mcmsg_post.h to specify which call.
 * Calls mcmsg_entry() if necessary.
 * Waits for completion and returns status of the call.
 */

mcmsg_call(method, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
	int	method;
{
	register volatile post_page_t	*pp;
	int				x;
	int				st;

	/*
	 * Posting a request *must* be an atomic operation
	 */
	x = sploff();

	if (mcmsg_mp_enable) {
		pp = mcmsg_kernel_post_page_next();
		pp->arg0 = arg0;
		pp->arg1 = arg1;
		pp->arg2 = arg2;
		pp->arg3 = arg3;
		pp->arg4 = arg4;
		pp->arg5 = arg5;
		pp->arg6 = arg6;
		pp->arg7 = arg7;
		pp->arg8 = arg8;
		pp->arg9 = arg9;
		pp->mcmsg_task = 0;
		st = mcmsg_post_status_wait(pp, method);
	} else {
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry call");
		mcmsg_send_tail_count = 0;
		RED_ON(RED_MSG);
#if	MSG_TRACE
		mcmsg_trace_call(method, arg0, arg1, arg2, arg3, arg4, arg5,
				 arg6, arg7, arg8, arg9);
#endif	MSG_TRACE
		if (method < POST_MAX) {
			st = mcmsg_post_switch[method](0,
		 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		} else {
			st = -1;
		}
		CHECK_REENTRY("exit  call");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
	}

	splon(x);
	return st;
}

/*
 * mcmsg_task_call(method, arg0, ...)
 *
 * Post a request to the message processor if active,
 * otherwise make the call directly.
 * method is a POST_* constant from mcmsg_post.h to specify which call.
 * Called directly from a kernel system call, looks up mcmsg_task.
 * Calls mcmsg_entry() if necessary.
 * Waits for completion and returns status of the call.
 */

mcmsg_task_call(task, method,
		 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
	task_t	task;
	int	method;
{
	register volatile post_page_t	*pp;
	register int			x;
	int				st;

	/*
	 * verify that task is parallel
	 */

	if (task->mcmsg_task == 0) {
		return -1;
	}

	/*
	 * Posting a request *must* be an atomic operation
	 */
	x = sploff();

	if (mcmsg_mp_enable) {
		pp = mcmsg_kernel_post_page_next();
		pp->arg0 = arg0;
		pp->arg1 = arg1;
		pp->arg2 = arg2;
		pp->arg3 = arg3;
		pp->arg4 = arg4;
		pp->arg5 = arg5;
		pp->arg6 = arg6;
		pp->arg7 = arg7;
		pp->arg8 = arg8;
		pp->arg9 = arg9;
		pp->mcmsg_task = task->mcmsg_task;
		st = mcmsg_post_status_wait(pp, method);
	} else {
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry task call");
		mcmsg_send_tail_count = 0;
		RED_ON(RED_MSG);
#if	MSG_TRACE
		mcmsg_trace_call(method, arg0, arg1, arg2, arg3, arg4, arg5,
				 arg6, arg7, arg8, arg9);
#endif	MSG_TRACE
		if (method < POST_MAX) {
			st = mcmsg_post_switch[method](task->mcmsg_task,
		 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		} else {
			st = -1;
		}
		CHECK_REENTRY("exit  task call");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
	}

	splon(x);
	return st;
}

/*
 * syscall_mcmsg_interface(method, arg0, ...)
 *
 * Kernel call for user access to message passing interface
 */

syscall_mcmsg_interface(method,
		arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
	int	method;
{
	extern	mcmsg_post_sw_err();

	if (method < POST_MAX &&
	    mcmsg_user_switch[method] != mcmsg_post_sw_err) {
		return mcmsg_task_call(current_task(), method,
		  arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	} else {
		return -1;
	}
}

/*
 * call_pmap_update_interrupt()
 *
 * Called from pmap.c to request cooperation while updating the memory map
 */

call_pmap_update_interrupt()
{
	register volatile post_page_t	*pp;
	register int			x;

	if (mcmsg_mp_enable) {
		/*
		 * Posting a request *must* be an atomic operation
		 */
		x = sploff();

		pp = mcmsg_kernel_post_page_next();
		mcmsg_post_status_wait(pp, POST_PMAPUPDATE);

		splon(x);
	}
}

/*
 * call_mcmsg_dflush_interrupt()
 *
 * Called to request MCP data cache flush.
 * necessary for expansion bus masters, such
 * as the HiPPI controllers, that are not
 * cache-coherent. Called from driver (hctlr.c).
 */

call_mcmsg_dflush_interrupt()
{
	register volatile post_page_t	*pp;
	register int			x;

	if (mcmsg_mp_enable) {
		/*
		 * Posting a request *must* be an atomic operation
		 */
		x = sploff();

		pp = mcmsg_kernel_post_page_next();
		mcmsg_post_status_wait(pp, POST_DFLUSH);

		splon(x);
	}
}

mcmsg_wire_buffer(task, lo, hi)
	task_t		task;
	unsigned long	lo;
	unsigned long	hi;
{
	int		i;

	i = vm_map_pageable_user(task->map,
			trunc_page(lo),
			round_page(hi),
			VM_PROT_READ | VM_PROT_WRITE);

	return ( i != 0 ? -1 : 0 );
}

#if	NX
#else	NX

mcmsg_ast()
{

	assert(0);
}

int nx_put;
nx_task_put_info()
{

	assert(0);
}

nx_task_get_info()
{

	assert(0);
}

#endif	NX
