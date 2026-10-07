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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_mp.c,v 1.36 1994/11/18 20:47:26 mtm Exp $
 */

/*
 * msgp_mp.c
 *
 * Message processor specific code
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#if NX
#include <i860paragon/mcmsg/mcmsg_nx.h>
#endif NX
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_mp.h>
#include <kern/thread.h>
#include <kern/thread_swap.h>
#include <i860/psl.h>
#include <i860paragon/msgp/msgp_hw.h>

#include <kern/processor.h>

int	mcmsg_regs[64];

int	mcmsg_nic_err_stat;

/*
 * Post page for kernel requests.
 * This page is used for all requests from the kernel on the compute processor.
 * It accepts all calls, including some that user post pages will not accept.
 * The Mach IPC interface goes through here.
 * When user code uses the old system call library for message passing
 * calls, they go through here.
 */

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
 * Statistics. Counts the number of calls by method.
 */

unsigned long mcmsg_post_count[POST_MAX];

#if	NCPUS > 1
/*
 *	Macros to track MCP CPU state (for SPV).
 */
#define MCP_RUNNING	cpu_to_processor(1)->state = PROCESSOR_RUNNING
#define MCP_IDLE	cpu_to_processor(1)->state = PROCESSOR_IDLE

#else	/* NCPUS > 1 */
#define MCP_RUNNING
#define MCP_IDLE
#endif	/* NCPUS > 1 */

/*
 *	Track state of Dispatch loop 
 */
int dispatch_state = 0;
#if     MSG_TRACE
#define	DISPATCH_IDLE dispatch_state = 0; MCP_IDLE
#define	DISPATCH_INTR dispatch_state = 1; MCP_RUNNING
#define	DISPATCH_KERN dispatch_state = 2; MCP_RUNNING
#define	DISPATCH_USER dispatch_state = 3; MCP_RUNNING
#else   MSG_TRACE
#define	DISPATCH_IDLE
#define	DISPATCH_INTR
#define	DISPATCH_KERN
#define	DISPATCH_USER
#endif  MSG_TRACE

/*
 * Routine: (MACRO)
 *	mcmsg_dispatch_call(mt, pp, sw)
 *
 * Arguments:
 *	mt	mcmsg_task pointer
 *	pp	points to post page entry
 *	sw	dispatch table
 *
 * Purpose:
 *	Call the interface procedure indicated in the post page entry.
 *	If tracing is enabled, first make a trace entry.
 *
 * Returns:
 *	none
 */

#if	MSG_TRACE

#define mcmsg_dispatch_call(mt, pp, sw) \
	if (pp->method < POST_MAX) { \
		mcmsg_post_count[pp->method]++; \
		mcmsg_trace_call(pp->method, \
				 pp->arg0, pp->arg1, pp->arg2, pp->arg3, pp->arg4, \
				 pp->arg5, pp->arg6, pp->arg7, pp->arg8, pp->arg9); \
		pp->status = sw[pp->method]( mt, \
			 pp->arg0, pp->arg1, pp->arg2, pp->arg3, pp->arg4, \
			 pp->arg5, pp->arg6, pp->arg7, pp->arg8, pp->arg9); \
	} else { \
		mcmsg_trace_debug("dispatch invalid method", 1, pp->method, 0, 0, 0); \
		pp->status = -1; \
	} \
	pp->method = POST_EMPTY;

#else	MSG_TRACE

#define mcmsg_dispatch_call(mt, pp, sw) \
	if (pp->method < POST_MAX) { \
		pp->status = sw[pp->method]( mt, \
			 pp->arg0, pp->arg1, pp->arg2, pp->arg3, pp->arg4, \
			 pp->arg5, pp->arg6, pp->arg7, pp->arg8, pp->arg9); \
	} else { \
		pp->status = -1; \
	} \
	pp->method = POST_EMPTY

#endif	MSG_TRACE

mcmsg_exp_dispatch_call(mt, pp, sw)
	register mcmsg_task_t	*mt;
	register post_page_t	*pp;
	register int		(**sw)();
{
	mcmsg_dispatch_call(mt, pp, sw);
}

/*
 * Routine:
 *	mcmsg_mp_dispatch()
 *
 * Arguments:
 *	none
 *
 * Purpose:
 *	Main dispatch loop.
 *
 * Returns:
 *	doesn't
 */

mcmsg_mp_dispatch()
{
	extern void nic_interrupt_enable_mcp();

	mcmsg_trace_debug("mp_dispatch started", 0, 0, 0, 0, 0);

	nic_interrupt_enable_mcp();

	/*
	 * Set cpu state initially to RUNNING.
	 */
	MCP_RUNNING;

	/*
	 * Main loop
	 *
	 * The main loop is divided into four cases:
	 *	- No user tasks
	 *	- One user task (send interrupt enabled)
	 *	- One user task (send interrupt disabled)
	 *	- More than one user task
	 *
	 * There is a different version of the loop for each form, optimized
	 * for that case. The one-user-task cases should be the most important.
	 *
	 */

	for (;;) {

		/*
		 * No user tasks
		 */

		if (mcmsg_user_post_tasks == 0) {

			mcmsg_dispatch_kernel_only();


		/*
		 * One user task
		 */

		} else if (mcmsg_user_post_tasks == 1) {

#if    VCF
			mcmsg_adispatch_user_vcf();
#else  VCF
			if ( mcmsg_hw.recv_enable.halfs.lo &&
				!mcmsg_hw.send_int_enable) {

				mcmsg_dispatch_user_recv();

			} else {
				mcmsg_dispatch_user_general();
			}
#endif VCF
		/*
		 * More than one user task
		 */

		} else { /* General case, mcmsg_user_post_tasks > 1 */

			mcmsg_dispatch_users();
		}
	}
}

/*
 *	Routine:
 *		mcmsg_dispatch_kernel_only
 *
 *	Purpose:
 *		Dispatch loop used when no user tasks present.
 *
 *		NIC Interrupts handled done by mcmsg_intr()
 *
 *		Exit this loop when a user task appears.
 */

mcmsg_dispatch_kernel_only()
{
	register long		outk;
	register post_page_t	*kpp;

	outk = mcmsg_kernel_post_page_out;
	kpp = &mcmsg_kernel_post_page[outk];
	for (;;) {

		/*
		 * Check for interrupts
		 */

		assert(sploff() == 0);
		if ((get_epsr() & EPSR_INT) != 0) {
			mcmsg_intr();
		}

		/*
		 * Check for kernel requests
		 */


		if (kpp->method != POST_EMPTY) {
			DISPATCH_KERN;
			mcmsg_dispatch_call(kpp->mcmsg_task,
					    kpp,
					    mcmsg_post_switch);
			DISPATCH_IDLE;
			outk = (mcmsg_kernel_post_page_out+1) &
			      (POST_PAGE_SLOTS-1);
			mcmsg_kernel_post_page_out = outk;
			kpp = &mcmsg_kernel_post_page[outk];
			if (mcmsg_user_post_tasks != 0) {
				return;
			}
		}
	}
}

/*
 *	Routine:
 *		mcmsg_dispatch_user_general
 *
 *	Purpose:
 *		Dispatch loop used when one user task present and
 *		send interrupt is enabled.
 *
 *		NIC interrupts handled by mcmsg_intr()
 *
 *		Exit this loop when user tasks not equal to 1 or
 *		when send interrupt DISABLED.
 */

mcmsg_dispatch_user_general()
{
	register long		outk;
	register long		outu;
	register post_page_t	*kpp;
	register post_page_t	*upp;

	outk = mcmsg_kernel_post_page_out;
	kpp = &mcmsg_kernel_post_page[outk];
	outu = mcmsg_user_post_page_out[0];
	upp = &mcmsg_user_post_page[0][outu];

	for (;;) {

		/*
		 * Check for interrupts
		 */

		assert(sploff() == 0);
		if ((get_epsr() & EPSR_INT) != 0) {
			DISPATCH_INTR;
			mcmsg_intr();
			DISPATCH_IDLE;
		}

		/*
		 * Check for kernel requests
		 */


		if (kpp->method != POST_EMPTY) {
			DISPATCH_KERN;
			mcmsg_dispatch_call(kpp->mcmsg_task,
					    kpp,
					    mcmsg_post_switch);
			DISPATCH_IDLE;
			assert(outk == mcmsg_kernel_post_page_out);
			outk = (outk+1) & (POST_PAGE_SLOTS-1);
			mcmsg_kernel_post_page_out = outk;
			kpp = &mcmsg_kernel_post_page[outk];

			/*
			 * Return if user tasks not equal 1.
			 */

			if (mcmsg_user_post_tasks != 1) {
				return;
			}
		}

		/*
		 * Scan the user task.
		 */

		if (upp->method != POST_EMPTY) {
			DISPATCH_USER;
			mcmsg_dispatch_call(mcmsg_user_mcmsg_task[0],
					    upp,
					    mcmsg_user_switch);
			DISPATCH_IDLE;
			assert(outu == mcmsg_user_post_page_out[0]);
			outu = (outu+1) & (POST_PAGE_SLOTS-1);
			mcmsg_user_post_page_out[0] = outu;
			upp = &mcmsg_user_post_page[0][outu];
		}

		/*
		 * Return if no send interrupt enabled.
		 */

		if (!mcmsg_hw.send_int_enable) {
			return;
		}
	}
}

#if	HANDCODE && !BIGPKTS

/* see msgp_dispatch.s */

#else	HANDCODE && !BIGPKTS

/*
 *	Routine:
 *		mcmsg_dispatch_user_recv
 *
 *	Purpose:
 *		Dispatch loop used when one user task present and
 *		send interrupt is NOT enabled.
 *
 *		NIC interrupts handled in-line.
 *
 *		Exit this loop when user tasks not equal to 1 or
 *		when send interrupt ENABLED.
 */


mcmsg_dispatch_user_recv()
{
	register long		outk;
	register long		outu;
	register post_page_t	*kpp;
	register post_page_t	*upp;
	register unsigned long	hdr1;
	register unsigned long	hdr2;

	outk = mcmsg_kernel_post_page_out;
	kpp = &mcmsg_kernel_post_page[outk];
	outu = mcmsg_user_post_page_out[0];
	upp = &mcmsg_user_post_page[0][outu];

	for (;;) {

		/*
		 * Check for interrupts
		 */

		assert(sploff() == 0);
		if ((get_epsr() & EPSR_INT) != 0) {
		
			/*
			 * Process receive interrupts.
			 * Read the first two header words from the NIC
			 */
		
			recv2_errchk(hdr1, hdr2);
		
			/*
			 * Switch on the packet type
			 * and dispatch receive procedure
			 */
		
			assert((hdr1 & 0xFFFF) < MCTRL_END);

#if	BUMPERS
			mcmsg_hw.recv_status = RECV_INTR_MODE_1;
#endif	BUMPERS

			/*
			 * Call the receive method.
			 */
			DISPATCH_INTR;
			mcmsg_recv_switch[hdr1 & (MCTRL_END-1)](hdr1, hdr2);
			DISPATCH_IDLE;
		
#if	BUMPERS
			/*
			 * switch receive interrupt mode.
			 */
			mcmsg_set_bumper();
#endif	BUMPERS

			assert(mcmsg_eod_last());

			/*
			 * If send interrupts enabled, back off to
			 * the more general dispatch loop.
			 */

			if (mcmsg_hw.send_int_enable) {
				break;
			}
			continue;
		}

		/*
		 * Check for kernel requests
		 */


		if (kpp->method != POST_EMPTY) {
			DISPATCH_KERN;
			mcmsg_dispatch_call(kpp->mcmsg_task,
					    kpp,
					    mcmsg_post_switch);
			DISPATCH_IDLE;
			assert(outk == mcmsg_kernel_post_page_out);
			outk = (outk+1) & (POST_PAGE_SLOTS-1);
			mcmsg_kernel_post_page_out = outk;
			kpp = &mcmsg_kernel_post_page[outk];
			if (mcmsg_user_post_tasks != 1) {
				return;
			}
		}

		/*
		 * Scan the user task.
		 */

		if (upp->method != POST_EMPTY) {
			DISPATCH_USER;
			mcmsg_dispatch_call(mcmsg_user_mcmsg_task[0],
					    upp,
					    mcmsg_user_switch);
			DISPATCH_IDLE;
			assert(outu == mcmsg_user_post_page_out[0]);
			outu = (outu+1) & (POST_PAGE_SLOTS-1);
			mcmsg_user_post_page_out[0] = outu;
			upp = &mcmsg_user_post_page[0][outu];
		}

		/*
		 * If send interrupts enabled, back off to
		 * the more general dispatch loop.
		 */

		if (mcmsg_hw.send_int_enable) {
			break;
		}
	}
}
#endif	HANDCODE && !BIGPKTS

/*
 *	Routine:
 *		mcmsg_dispatch_users
 *
 *	Purpose:
 *		Most general dispatch loop.
 *		Dispatch loop used when MORE than one user task present.
 *
 *		NIC interrupts handled by mcmsg_intr().
 *
 *		Exit this loop when user tasks become <= 1.
 */

mcmsg_dispatch_users()
{
	register long		outk;
	register long		outu;
	register post_page_t	*kpp;
	register post_page_t	*upp;
	int	i;

	outk = mcmsg_kernel_post_page_out;
	kpp = &mcmsg_kernel_post_page[outk];
	i = 0;
	for (;;) {

		/*
		 * Check for interrupts
		 */

		assert(sploff() == 0);
		if ((get_epsr() & EPSR_INT) != 0) {
			DISPATCH_INTR;
			mcmsg_intr();
			DISPATCH_IDLE;
		}

		/*
		 * Check for kernel requests
		 */


		if (kpp->method != POST_EMPTY) {
			DISPATCH_KERN;
			mcmsg_dispatch_call(kpp->mcmsg_task,
					    kpp,
					    mcmsg_post_switch);
			DISPATCH_IDLE;
			assert(outk == mcmsg_kernel_post_page_out);
			outk = (outk+1) & (POST_PAGE_SLOTS-1);
			mcmsg_kernel_post_page_out = outk;
			kpp = &mcmsg_kernel_post_page[outk];
			if (mcmsg_user_post_tasks <= 1) {
				return;
			}
		}

		/*
		 * Scan the next user task.
		 */

		if (i >= mcmsg_user_post_tasks) {
			i = 0;
		}
		outu = mcmsg_user_post_page_out[i];
		upp = &mcmsg_user_post_page[i][outu];
		if (upp->method != POST_EMPTY) {
			DISPATCH_USER;
			mcmsg_dispatch_call(mcmsg_user_mcmsg_task[i],
					    upp,
					    mcmsg_user_switch);
			DISPATCH_IDLE;
			outu = (outu+1) & (POST_PAGE_SLOTS-1);
			mcmsg_user_post_page_out[i] = outu;
		}
		i++;
	}
}

/*
 *	Routine:
 *		mcmsg_intr_recv()
 *
 *	Arguments:
 *		None.
 *
 *	Purpose:
 *		Process a receive interrupt.
 *		The receive interrupt is assumed to be set at entry.
 *
 *	Return:
 *		None.
 */
mcmsg_intr_recv()
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	method;

	/*
	 * Process a receive interrupt.
	 * Read the first two header words from the NIC
	 */
	recv2_errchk(hdr1, hdr2);

	/*
	 * Switch on the packet type and dispatch receive procedure
	 */
	method = hdr1 & 0xFFFF;
	if (method < MCTRL_END) {

#if	BUMPERS
		mcmsg_hw.recv_status = RECV_INTR_MODE_1;
#endif	BUMPERS
	
		/*
		 * Call the receive method.
		 */
		mcmsg_recv_switch[hdr1 & (MCTRL_END-1)](hdr1, hdr2);
	
#if	BUMPERS
		mcmsg_set_bumper();
#endif	BUMPERS

		/*
		 * Check that the receive procedure took exactly
		 * the whole packet.
		 */
		assert(mcmsg_eod_last());
	} else {
		mcmsg_recv_sw_err(hdr1, hdr2);
	}

#if NIC_TRACE
	mcmsg_nic_status(10);
#endif NIC_TRACE
}

/*
 * Routine:
 *	mcmsg_validate_line_real(a, dirbase, file, line)
 *	mcmsg_validate_long_real(a, dirbase, file, line)
 *	mcmsg_validate_real(a, dirbase, file, line)
 *
 * Arguments:
 *	a	Address
 *	dirbase	Page table directory pointer
 *	file	File name (__FILE__, for debugging)
 *	line	Line number (__LINE__, for debugging)
 *
 * Purpose:
 *	Translate address a in user space to physical and validate it.
 *	There are three versions, one for a cache line (actually, xmsg),
 *	one for a long, and one for any alignment. In the debug kernel,
 *	these are separate procedures where the first two check for
 *	correct alignment then call the last. In the non-debug kernel,
 *	the first two are macros.
 *
 * Returns:
 *	Physical address corresponding to a, or
 *	zero if not valid (not mapped user r/w)
 */

#if	MACH_ASSERT

mcmsg_validate_line_real(a, dirbase, file, line)
	unsigned long a;
	unsigned long	*dirbase;
	char		*file;
	int		line;
{

	if (a & 0x0000001F) {
mcmsg_trace_debug("validate line failed", 3, file, line, a, 0);
		return 0;
	}
	return mcmsg_validate_real(a, dirbase, file, line);
}

mcmsg_validate_long_real(a, dirbase, file, line)
	unsigned long a;
	unsigned long	*dirbase;
	char		*file;
	int		line;
{

	if (a & 0x00000003) {
		mcmsg_trace_debug("validate long failed", 3,
				file, line, a, 0);
		return 0;
	}
	return mcmsg_validate_real(a, dirbase, file, line);

}

mcmsg_validate_real(a, dirbase, file, line)
	unsigned long	a;
	unsigned long	*dirbase;
	char		*file;
	int		line;
{
	pt_entry_t	pt;

	pt = dirbase[pdenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		mcmsg_trace_debug("validate no pde", 4,
				file, line, a, pt);
		return 0;
	}
	pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		mcmsg_trace_debug("validate no pte", 4,
				file, line, a, pt);
		return 0;
	}

	pt = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
	return pt;
}


#else	MACH_ASSERT

#if	HANDCODE
/* See msgp_dispatch.s */
#else	HANDCODE
mcmsg_validate_real(a, dirbase)
	unsigned long	a;
	unsigned long	*dirbase;
{
	pt_entry_t	pt;

	pt = dirbase[pdenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		return 0;
	}
	pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		return 0;
	}

	pt = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
	return pt;
}
#endif	HANDCODE
#endif	MACH_ASSERT


#if	HANDCODE
/* See msgp_dispatch.s */
#else	HANDCODE
unsigned long mcmsg_validate_bp2;

mcmsg_validate_rw1(a, n, dirbase, rw_valid)
	unsigned long	a;
	unsigned long	n;
	unsigned long	*dirbase;
	register unsigned long rw_valid;
{
	unsigned long	b;
	unsigned long	na;
	unsigned long	nb;
	pt_entry_t	pd;
	pt_entry_t	pt;
	unsigned long	bp1;
	unsigned long	bp2;

	na = pdenum(a);
	pd = dirbase[na];
	if ((pd & INTEL_PTE_VALID) == 0) {
		mcmsg_trace_drop("validate buf pde", a);
		return 0;
	}
	pt = ((pt_entry_t *)(pd & INTEL_PTE_PFN))[ptenum(a)];
	if ((pt & rw_valid) != rw_valid) {
		mcmsg_trace_drop("validate buf pte", a);
		return 0;
	}
	bp2 = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
	nb = (a & ~(INTEL_PTE_PFN));
	bp1 = (pt & INTEL_PTE_PFN) | nb;

	n -= 1;
	if ( ((a & (MSG_PAGE_SIZE-1)) + n & ~(MSG_PAGE_SIZE-1)) == 0) {
		mcmsg_validate_bp2 = bp1 + n;
		return bp1;
	}
	b = a + n;
	nb = pdenum(b);
	if (na != nb) {
		pd = dirbase[nb];
		if ((pd & INTEL_PTE_VALID) == 0) {
			mcmsg_trace_drop("validate buf pde 2", a);
			return 0;
		}
	}
	pt = ((pt_entry_t *)(pd & INTEL_PTE_PFN))[ptenum(b)];
	if ((pt & rw_valid) != rw_valid) {
		mcmsg_trace_drop("validate buf pte 2", a);
		return 0;
	}
	mcmsg_validate_bp2 = (pt & INTEL_PTE_PFN) | (b & ~(INTEL_PTE_PFN));
	return bp1;
}

mcmsg_validate2()
{

	return mcmsg_validate_bp2;
}
#endif	HANDCODE

/*
 * Routine:
 *	mcmsg_mp_install(mt, post_page, dirbase)
 *
 * Arguments:
 *	mt		Pointer to mcmsg_task
 *	post_page	New user post page
 *	dirbase		Page table directory
 *
 * Purpose:
 *	Install a new user post page.
 *
 * Returns:
 *	Zero if successful, negative if not; return value is not significant
 */

mcmsg_mp_install(mt, post_page, dirbase)
	mcmsg_task_t	*mt;
	post_page_t	*post_page;
	unsigned long	dirbase;
{
	post_page_t	*pp;
	int		i;

	/*
	 * Error out if table full
	 */

	if (mcmsg_user_post_tasks == POST_TASK_MAX) {
		return -5;
	}

	/*
	 * Convert post page pointer to physical address
	 * Error out if not mapped or not on page boundary
	 */

	if ((pp = (post_page_t *)mcmsg_validate_real(
		post_page, dirbase, __FILE__, __LINE__)) == 0 ||
	    ((unsigned long)pp & (INTEL_PGBYTES-1)) != 0) {
		return -6;
	}
	i = mcmsg_user_post_tasks++;
	mcmsg_user_post_page[i] = pp;
	mcmsg_user_mcmsg_task[i] = mt;
	mcmsg_user_post_page_out[i] = 0;

	return 0;
}

#if	NX

/*
 * Routine:
 *	mcmsg_mp_clear_task()
 *
 * Arguments:
 *	none
 *
 * Purpose:
 *	Clear the post page (if any) for a task as it disappears.
 *	Then complete task destruction in another module.
 *
 * Returns:
 *	none
 */

mcmsg_mp_clear_task(mt)
	mcmsg_task_t	*mt;
{
	int	i, j;

	/*
	 * Look for a user post page
	 */

	for (i = 0; i < mcmsg_user_post_tasks; i++) {
		if (mcmsg_user_mcmsg_task[i] == mt) {

			/*
			 * Decrement the number of active tasks
			 *
			 * Fill in the hole in the array with the
			 * user post page that used to be last
			 */

			mcmsg_user_post_tasks--;
			j = mcmsg_user_post_tasks;
			mcmsg_user_mcmsg_task[i] = mcmsg_user_mcmsg_task[j];
			mcmsg_user_post_page[i] = mcmsg_user_post_page[j];
			mcmsg_user_post_page_out[i] = mcmsg_user_post_page_out[j];
			mcmsg_user_mcmsg_task[j] = 0;
			mcmsg_user_post_page[j] = 0;
			mcmsg_user_post_page_out[j] = 0;
			break;
		}
	}

	/*
	 * Complete the task's destruction
	 */

	return mcmsg_destroy_task(mt);
}

#endif	NX

/*
 * Routine:
 *	mcmsg_nop()
 *
 * Arguments:
 *	none
 *
 * Purpose:
 *	Nop for testing.
 *
 * Returns:
 *	zero
 */

mcmsg_nop()
{

	return 0;
}


/*
 * Routine:
 *	mcmsg_post_sw_err(mt)
 *
 * Arguments:
 *	mt	mcmsg_task pointer
 *
 * Purpose:
 *	Error procedure for invalid calls
 *
 * Returns:
 *	none
 */

mcmsg_post_sw_err(mt)
	mcmsg_task_t	*mt;
{

	return -1;
}

#if	MCMSG && NCPUS == 1
/*
 * Routine:
 *	mcmsg_msgp_assert(file, line)
 *
 * Arguments:
 *	file:	File name (__FILE__)
 *	line:	Line number (__LINE__)
 *
 * Purpose:
 *	What to do when the message processor fails an assertion
 *
 * Returns:
 *	doesn't
 */

volatile char	*mcmsg_mp_assert_file;
int	mcmsg_mp_assert_line;

mcmsg_msgp_assert(file, line)
	char	*file;
	int	line;
{

	mcmsg_trace_drop(file, line);
	mcmsg_catch_regs();
	if (mcmsg_mp_enable) {
		mcmsg_mp_assert_file = file;
		mcmsg_mp_assert_line = line;
		while (mcmsg_mp_assert_file);
	} else {
		Assert(file, line);
	}
}
#endif	/* MCMSG && NCPUS == 1 */

#include <i860paragon/mp.h>

#define	MSGP_STACK_SIZE	16384
char	msgp_stack_space[MSGP_STACK_SIZE];
char	*msgp_stack = &msgp_stack_space[MSGP_STACK_SIZE-16];
char	*msgp_stack_limit = &msgp_stack_space[1024];
char	*msgp_stack_low = (char *)0xFFFFFFFF;
char	*msgp_stack_last;

/*
 * for ddb purposes, mcp_thread is set to cpu1's idle thread.
 * someday this may become for real.
 */
thread_t mcp_thread;

msgp_main()
{
	/*
	 * enable VM mapping.
	 */

	set_dirbase(kernel_pmap->dirbase);
#if	NCPUS > 1
	active_threads[1] = mcp_thread;
	/*
	 * xxx overwrite exiting stack pointer w/o freeing old stack
	 */
	mcp_thread->kernel_stack = (vm_offset_t) msgp_stack;
#endif	/* NCPUS > 1 */

#if    VCF
	setup_asm_upcalls();
#endif VCF

	/*
	 * Enter dispatch loop
	 */

	mcmsg_mp_dispatch();
}

msgp_stack_info(a)
	char	a;
{

	return (((unsigned long)msgp_stack) - ((unsigned long)&a))/64;
}

msgp_stack_check(a)
	char	a;
{

	if (!mcmsg_mp_enable) {
		return;
	}
	msgp_stack_last = &a;
	if (&a < msgp_stack_low) {
		msgp_stack_low = &a;
	}
	assert(&a > msgp_stack_limit);
	assert(&a < msgp_stack);
	return;
}

/*
 * Need to provide these if not compiled multiprocessor
 */

msgp_pmap_update_interrupt()
{

	flush_tlb();
}

/* 
 * PTS 9218 fix, add data cache flushing capability
 */
msgp_dflush_interrupt()
{
	flush();
}

mp_ast()
{
	CPU_INT_REASON_SET(MP_AST_MCMSG, master_cpu);
	cpu_interrupt(master_cpu);
}

mp_netipc_send_intr()
{

	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_NETSEND, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  netipc send");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		netipc_send_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry netipc send");
		RED_ON(RED_MSG);
	}
}

mp_netipc_recv_intr()
{

mcmsg_trace_debug("ipc recv deliver", 0, 0, 0, 0, 0);
	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_NETRECV, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  netipc recv");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		netipc_recv_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry netipc recv");
		RED_ON(RED_MSG);
	}
}

#if	MCMSG_ENG
mp_rpc_request_intr()
{

	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RPCREQ, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rpc request");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rpc_engine_request_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rpc request");
		RED_ON(RED_MSG);
	}
}

mp_rpc_reply_intr()
{

	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RPCREPLY, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rpc reply  ");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rpc_engine_reply_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rpc reply  ");
		RED_ON(RED_MSG);
	}
}

mp_rpc_depart_intr()
{
	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RPCDEPART, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rpc depart ");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rpc_engine_depart_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rpc depart ");
		RED_ON(RED_MSG);
	}
}
#endif	MCMSG_ENG

#if	MCMSG_ENG
mp_rdma_panic()
{
	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_PANIC, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rdma panic");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		panic("mp_rdma_panic");
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rdma panic");
		RED_ON(RED_MSG);
	}
}


mp_rdma_engine_send_intr()
{

	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RDMASEND, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rdma send");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rdma_engine_send_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rdma send");
		RED_ON(RED_MSG);
	}
}

mp_rdma_engine_recv_intr()
{

	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RDMARECV, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rdma recv  ");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rdma_engine_recv_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rdma recv  ");
		RED_ON(RED_MSG);
	}
}

mp_rdma_engine_send_fault_intr()
{
	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RDMATXF, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rdma txf   ");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rdma_engine_send_fault_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rdma txf   ");
		RED_ON(RED_MSG);
	}
}

mp_rdma_engine_recv_fault_intr()
{
	if (mcmsg_mp_enable) {
		CPU_INT_REASON_SET(MP_RDMARXF, master_cpu);
		cpu_interrupt(master_cpu);
	} else {
		CHECK_REENTRY("exit  rdma rxf   ");
		assert(--mcmsg_reentry == 0);
		RED_OFF(RED_MSG);
		rdma_engine_recv_fault_intr();
		assert(mcmsg_reentry++ == 0);
		CHECK_REENTRY("entry rdma rxf   ");
		RED_ON(RED_MSG);
	}
}
#endif	MCMSG_ENG

#if    VCF
/* Assembly upcall pointers. */
unsigned long msgp_asm_recv_switch[POST_MAX];
unsigned long msgp_asm_upp_switch[POST_MAX];

setup_asm_upcalls()  {
	extern int msgp_dis_pp_upcall(), msgp_dis_recv_upcall(),
	  msgp_dis_nx_send_upcall();
	extern int  vcf_asend(), vcf_arecv(),
	  vcf_ahandle_rd(), vcf_ahandle_rda(), vcf_ahandle_rdac();
	extern void  vcf_transmit_rd();
	int  i;

	for (i = 0;  i < POST_MAX;  ++i)  {
		msgp_asm_recv_switch[i] = (unsigned long) msgp_dis_recv_upcall;
		msgp_asm_upp_switch[i] = (unsigned long) msgp_dis_pp_upcall;
	}
	msgp_asm_recv_switch[VCF_HWORD_RD] = vcf_ahandle_rd;
	msgp_asm_recv_switch[VCF_HWORD_RDA] = vcf_ahandle_rda;
	msgp_asm_recv_switch[VCF_HWORD_RDAC] = vcf_ahandle_rdac;
	msgp_asm_upp_switch[POST_VCFSEND] = vcf_asend;
	msgp_asm_upp_switch[POST_VCFRECV] = vcf_arecv;
}
#endif VCF
