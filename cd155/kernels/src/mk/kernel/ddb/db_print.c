/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * SSD HISTORY
 * $Log: db_print.c,v $
 * Revision 1.17  1995/03/07  03:04:32  lenb
 * show thread/v should print processor pointer in hex
 *
 * Revision 1.16  1994/11/18  20:29:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/08/31  21:23:53  mtm
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
 * Revision 1.13  1994/07/29  21:48:44  rkl
 *  Added a `verbose' option to !db_sys.  Calling !db_sys(1) will print
 *  additional information
 *
 * Revision 1.12  1994/07/22  16:14:38  andyp
 * Added 3 new debugger commands "show pmap <pmap>" to display a pmap,
 * "show vmtask <task>" to display the size of the virtual address
 * space, resident, and wired page counts.
 *
 * Revision 1.11  1994/07/13  18:27:17  stans
 *  Function 'db_show_port_id()' was coded to accept a thread argument as was
 *  'db_port_iterate()'. Change both routines to deal only with a task so
 *  "show ipc_port $task30" works. After all, ports are associated with a task
 *   not a thread.
 *
 *  Reviewer: self
 *  Risk: low
 *  PTS #: 10209
 *  Testing: developer
 *
 * Revision 1.10  1994/07/12  19:17:24  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/05/06  21:17:18  lenb
 *         new home for debugging code from processor.c
 *
 *         replace 'show cpu' command with 'show processor'
 *         added verbose flag to show thread commands, eg.
 *         show thread/v *active_threads
 *
 * Revision 1.8  1994/03/19  00:59:23  lenb
 *  Reviewer: sean
 *  Risk: low
 *  Benefit or PTS #: add bound and last processor to "show thread/ul" output
 *
 * Revision 1.7  1994/03/06  22:17:23  sean
 *  Reviewer:  steved
 *  Risk:  low
 *
 * Checking in these files which represent Steved's fixes for problems
 * in the following following areas:
 *
 * -fscan
 * -vdp clock skew
 * -bootmagic for processor mode
 *
 * ./ddb/db_command.c
 * ./ddb/db_print.c
 * ./i860/hardclock.c
 * ./i860paragon/mp.h
 * ./i860paragon/fscan.c
 * ./i860paragon/fscan.h
 * ./i860paragon/model_dep.c
 * ./i860paragon/mp_start.s
 * ./i860paragon/rpm.c
 * ./i860paragon/mp.c
 * ./i860paragon/interrupt.c
 * ./intel/pmap.c
 * ./ipsc/bootenv.c
 * ./kern/sched_prim.c
 * ./kern/mach_clock.c
 * ./vm/vm_pageout.c
 *
 * Revision 1.6.4.2  1994/06/14  17:53:23  andyp
 * Add the '#' for the current thread in the output of "show all threads".
 *
 * Revision 1.6.4.1  1994/02/15  03:12:48  stans
 * show args work
 *
 * Revision 1.6  1993/09/28  17:53:17  andyp
 * Update with extra debugging.  Recovered OSF's logs.
 *
 *
 *	When printing out thread information, include stack
 *	and vm privileges and whether the thread has a
 *	fixed scheduling policy.  [andyp@ssd.intel.com]
 *
 * 	Added kmsg queue counting that works for psets as
 *	well as ports.  [alanl@osf.org]
 *
 *	Added db_bool_str to print out booleans as strings.  [alanl@osf.org]
 *
 *	Add db_sys to dump generally useful information about
 *	system state during hangs.  From stans@ssd.intel.com.  [alanl]
 *
 *	Add /s option to "show ipc_port" to pick out port sets.  (dwm)
 *
 * Revision 1.5  1993/07/01  15:31:33  terry
 * source sync with nmk13.25
 *
 * Revision 1.1.2.4  1993/06/16  17:17:36  dwm
 * 	Add 'show all spaces' from nmk14 [sjs].
 * 	Add !db_show_port_pigs() command to print out list of ports
 * 	with pending msgs stacked up, from all spaces.
 * 	[1993/06/16  17:16:33  dwm]
 *
 * Revision 1.4  1993/06/30  22:20:47  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:18:54  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:35  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: db_print.c,v
 * Revision 1.1.2.5  1993/07/27  12:31:59  mmp
 * 	print out msgcount for each port in db_port_iterate
 * 	[1993/07/27  12:31:37  mmp]
 *
 * Revision 1.1.2.4  1993/06/16  17:17:36  dwm
 * 	Add 'show all spaces' from nmk14 [sjs].
 * 	Add !db_show_port_pigs() command to print out list of ports
 * 	with pending msgs stacked up, from all spaces.
 * 	[1993/06/16  17:16:33  dwm]
 * 
 * Revision 1.1.2.3  1993/04/15  22:43:50  alanl
 * 	Add db_sys to dump generally useful information about
 * 	system state during hangs.  From stans@ssd.intel.com.  [alanl]
 * 	[1993/04/15  22:03:29  alanl]
 * 
 * Revision 1.1.2.2  1993/02/17  16:12:19  dwm
 * 	Add /s option to "show ipc_port" to pick out port sets.
 * 	[1993/02/17  16:10:24  dwm]
 * 
 * Revision 1.1  1992/09/30  02:01:18  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.11.3.2  92/04/08  15:43:10  jeffreyh
 * 	Added i option to show thread. This gives wait state information.
 * 	[92/04/08            sjs]
 * 
 * Revision 2.11.3.1  92/03/03  16:13:34  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:00:01  jeffreyh]
 * 
 * Revision 2.13  92/02/20  18:34:28  elf
 * 	Fixed typo.
 * 	[92/02/20            elf]
 * 
 * Revision 2.12  92/02/19  15:07:47  elf
 * 	Added db_thread_fp_used, to avoid machine-dependent conditionals.
 * 	[92/02/19            rpd]
 * 
 * 	Added 'F' flag to db_thread_stat showing if the thread has a valid
 * 	FPU context. Tested on i386 and pmax.
 * 	[92/02/17            kivinen]
 * 
 * Revision 2.11  91/11/12  11:50:32  rvb
 * 	Added OPTION_USER ("/u") to db_show_all_threads, db_show_one_thread,
 * 	db_show_one_task.  Without it, we display old-style information.
 * 	[91/10/31            rpd]
 * 
 * Revision 2.10  91/10/09  16:01:48  af
 * 	Supported "show registers" for non current thread.
 * 	Changed display format of thread and task information.
 * 	Changed "show thread" to print current thread information 
 * 	  if no thread is specified.
 * 	Added "show_one_task" for "show task" command.
 * 	Added IPC port print routines for "show ipc_port" command.
 * 	[91/08/29            tak]
 * 
 * Revision 2.9  91/08/03  18:17:19  jsb
 * 	In db_print_thread, if the thread is swapped and there is a
 * 	continuation function, print the function name in parentheses
 * 	instead of '(swapped)'.
 * 	[91/07/04  09:59:27  jsb]
 * 
 * Revision 2.8  91/07/31  17:30:43  dbg
 * 	Revise scheduling state machine.
 * 	[91/07/30  16:43:42  dbg]
 * 
 * Revision 2.7  91/07/09  23:15:57  danner
 * 	Fixed a few printf that should be db_printfs. 
 * 	[91/07/08            danner]
 * 
 * Revision 2.6  91/05/14  15:35:25  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:06:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:18:56  mrt]
 * 
 * Revision 2.4  90/10/25  14:43:54  rwd
 * 	Changed db_show_regs to print unsigned.
 * 	[90/10/19            rpd]
 * 	Generalized the watchpoint support.
 * 	[90/10/16            rwd]
 * 
 * Revision 2.3  90/09/09  23:19:52  rpd
 * 	Avoid totally incorrect guesses of symbol names for small values.
 * 	[90/08/30  17:39:08  af]
 * 
 * Revision 2.2  90/08/27  21:51:49  dbg
 * 	Insist that 'show thread' be called with an explicit address.
 * 	[90/08/22            dbg]
 * 
 * 	Fix type for db_maxoff.
 * 	[90/08/20            dbg]
 * 
 * 	Do not dereference the "valuep" field of a variable directly,
 * 	call the new db_read/write_variable functions instead.
 * 	Reflected changes in symbol lookup functions.
 * 	[90/08/20            af]
 * 	Reduce lint.
 * 	[90/08/10  14:33:44  dbg]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/* CMU_ENDHIST */
/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
#include <norma_ipc.h>

/*
 * Miscellaneous printing.
 */
#include <mach/port.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/queue.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <vm/vm_page.h>

#include <machine/db_machdep.h>
#include <machine/thread.h>

#include <ddb/db_lex.h>
#include <ddb/db_variables.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>

extern unsigned int	db_maxoff;

void db_show_queue_entry(char *str, struct queue_entry *qep);	/* forward */

/*
 * given a name of a machine register, return a variable pointer to it.
 */

db_variable_t
db_find_reg_name( s )
	register char	*s;
{
	register db_variable_t	regp;

	if ( s == (char *)0 )
		return DB_VAR_NULL;

	for (regp = db_regs; regp < db_eregs; regp++) {
		if ( strcmp( s, regp->name) == 0 )
			return regp;
	}
	return DB_VAR_NULL;
}

/* ARGSUSED */
void
db_show_regs(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	int	(*func)();
	register struct db_variable *regp;
	db_expr_t	value, offset;
	char *		name;
	register	i;
	struct db_var_aux_param aux_param;
	task_t		task = TASK_NULL;

	aux_param.modif = modif;
	aux_param.thread = THREAD_NULL;
	if (db_option(modif, 't')) {
	    if (have_addr) {
		if (!db_check_thread_address_valid((thread_t)addr))
		    return;
		aux_param.thread = (thread_t)addr;
	    } else
	        aux_param.thread = db_default_thread;
	    if (aux_param.thread != THREAD_NULL)
		task = aux_param.thread->task;
	}
	for (regp = db_regs; regp < db_eregs; regp++) {
	    if (regp->max_level > 1) {
		db_printf("bad multi-suffixed register %s\n", regp->name);
		continue;
	    }
	    aux_param.level = regp->max_level;
	    for (i = regp->low; i <= regp->high; i++) {
		aux_param.suffix[0] = i;
	        db_read_write_variable(regp, &value, DB_VAR_GET, &aux_param);
		if (regp->max_level > 0)
		    db_printf("%s%d%*s", regp->name, i, 
				12-strlen(regp->name)-((i<10)?1:2), "");
		else
		    db_printf("%-12s", regp->name);
		db_printf("%#10n", value);
		db_find_xtrn_task_sym_and_offset((db_addr_t)value, &name, 
							&offset, task);
		if (name != 0 && offset <= db_maxoff && offset != value) {
		    db_printf("\t%s", name);
		    if (offset != 0)
			db_printf("+%#r", offset);
	    	}
		db_printf("\n");
	    }
	}
}

#if	MACH_SLOCKS
void
db_show_slock(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	struct slock *slp;

	if (addr == (db_expr_t)0 || !have_addr) {
		db_printf("No lock\n");
		return;
	}

	slp = (struct slock *)addr;

	db_printf("lock_data %x ", slp->lock_data);
#if	MACH_LDEBUG
	db_printf("lock_pc %x ", slp->lock_pc);
	db_printf("unlock_pc %x ", slp->unlock_pc);
	db_printf("thread %x\n", slp->thread);
#else	/* MACH_LDEBUG */
	db_printf("\n");
#endif	/* MACH_LDEBUG */
}

void
show_slock(char *str, struct slock *sp)
{
	db_printf("%s: ", str);
	db_show_slock((db_expr_t)sp, 1, 1, (char *)0);
}
#endif	MACH_SLOCKS

#define OPTION_LONG		0x001		/* long print option */
#define OPTION_USER		0x002		/* print ps-like stuff */
#define OPTION_INDENT		0x100		/* print with indent */
#define OPTION_THREAD_TITLE	0x200		/* print thread title */
#define OPTION_TASK_TITLE	0x400		/* print thread title */
#define OPTION_VERBOSE		0x800		/* verbose thread output */

#ifndef	DB_TASK_NAME
#define DB_TASK_NAME(task)			/* no task name */
#define DB_TASK_NAME_TITLE	""		/* no task name */
#endif	DB_TASK_NAME

char *
db_thread_stat(thread, status)
	register thread_t thread;
	char	 *status;
{
	register char *p = status;
	
	*p++ = (thread->state & TH_RUN)  ? 'R' : '.';
	*p++ = (thread->state & TH_WAIT) ? 'W' : '.';
	*p++ = (thread->state & TH_SUSP) ? 'S' : '.';
	*p++ = (thread->state & TH_SWAPPED) ? 'O' : '.';
	*p++ = (thread->state & TH_UNINT) ? 'N' : '.';
	/* show if the FPU has been used */
#ifdef	db_thread_fp_used
	*p++ = db_thread_fp_used(thread) ? 'F' : '.';
#else
	*p++ = '?';
#endif
	*p++ = 0;
	return(status);
}

void
db_print_timer_save(timer_save_t t)
{
	db_printf("low 0x%x high 0x%x\n", t->low, t->high);
}

void
db_print_timer(timer_t t)
{
	db_printf("low 0x%x high 0x%x check 0x%x tstamp 0x%x\n",
		t->low_bits, t->high_bits, t->high_bits_check, t->tstamp);
}

void
db_print_thread_verbose(thread, thread_id, flag)
	thread_t thread;
	int	 thread_id;
	int	 flag;
{
	if (thread == THREAD_NULL)
	{
		db_printf("THREAD_NULL\n");
		return;
	}

	db_show_queue_entry("links", &thread->links);
	db_printf("runq %x\n", thread->runq);
	db_printf("task %x\n", thread->task);
	db_show_queue_entry("thread_list", &thread->thread_list);
	db_show_queue_entry("pset_threads", &thread->pset_threads);
#if	MACH_SLOCKS
	show_slock("lock", &thread->lock);
#endif	MACH_SLOCKS
	db_printf("ref_count %d\n", thread->ref_count);
	db_printf("pcb 0x%x\n", thread->pcb);
	db_printf("kernel_stack 0x%x", thread->kernel_stack);
	db_printf(" stack_privilege 0x%x\n", thread->stack_privilege);
	db_printf("wait_event 0x%x ", thread->wait_event);
	if ((thread->state & TH_WAIT) && (thread->wait_event))
		db_task_printsym((db_addr_t)thread->wait_event,
			DB_STGY_ANY, TASK_NULL);
	db_printf("\nsuspend_count 0x%x", thread->suspend_count);
	db_printf(" wait_result 0x%x", thread->wait_result);
	db_printf(" wake_active 0x%x\n", thread->wake_active);
	db_printf("state 0x%b\n", thread->state, "\20\12TH_SW_COMING_IN"
		"\11TH_SWAPPED\10TH_IDLE\5TH_HALTED\4TH_UNINT"
		"\3TH_RUN\2TH_SUSP\1TH_WAIT");
	db_printf("swap_func 0x%x ", thread->swap_func);
	if ((thread->state & TH_SWAPPED) && (thread->swap_func))
		db_printsym((db_addr_t)thread->swap_func, DB_STGY_ANY);
	db_printf("\npriority %d max_priority %d sched_pri %d\n",
		thread->priority, thread->max_priority, thread->sched_pri);
#if	MACH_FIXPRI
	db_printf("sched_data 0x%x policy 0x%x\n",
		thread->sched_data, thread->policy);
#endif	MAX_FIXPRI
	db_printf("depress_priority %d cpu_usage %d\n",
		thread->depress_priority, thread->cpu_usage);
	db_printf("sched_usage %d sched_stamp %d\n",
		thread->sched_usage, thread->sched_stamp);
	db_printf("recover %d vm_privledge %d\n",
		thread->recover, thread->vm_privilege);
#if	NORMA2
	db_printf("dipc_ool_tx_map 0x%x dipc_ool_rx_map 0x%x\n",
		thread->dipc_ool_tx_map, thread->dipc_ool_rx_map);
	db_printf("dipc_rdma_tx_group %d dipc_rdma_rx_group %d\n",
		thread->dipc_rdma_tx_group, thread->dipc_rdma_rx_group);
	db_printf("dipc_th_spare_1 0x%x dipc_th_spare_2 0x%x\n",
		thread->dipc_th_spare_1, thread->dipc_th_spare_2);
#endif	NORMA2
	db_printf("user_stop_count %d\n", thread->user_stop_count);
	db_printf("ith_next 0x%x ith_prev 0x%x\n",
		thread->ith_next, thread->ith_prev);
	db_printf("msize 0x%x ith_seqno 0x%x ith_messages 0x%x\n",
		thread->data.msize, thread->ith_seqno,
		thread->ith_messages.ikmq_base);
#if	MACH_SLOCKS
	show_slock("ith_lock_data 0x%x", &thread->ith_lock_data);
#endif	MACH_SLOCKS
	db_printf("ith_self 0x%x ith_sself 0x%x ith_exception 0x%x\n",
		thread->ith_self, thread->ith_sself, thread->ith_exception);
#if	MACH_IPC_COMPAT
	db_printf("ith_reply 0x%x\n", thread->ith_reply);
#endif	MACH_IPC_COMPAT
	db_printf("ith_mig_reply 0x%x ith_rpc_reply 0x%x\n",
		thread->ith_mig_reply, thread->ith_rpc_reply);
	db_printf("saved state 0x%x\n", &(thread->saved));
	db_printf("user_timer ");
	db_print_timer(&(thread->user_timer));
	db_printf("system_timer ");
	db_print_timer(&(thread->system_timer));
	db_printf("user_timer_save ");
	db_print_timer_save(&(thread->user_timer_save));
	db_printf("system_timer_save ");
	db_print_timer_save(&(thread->system_timer_save));
	db_printf("cpu_delta 0x%x sched_delta 0x%x\n",
		thread->cpu_delta, thread->sched_delta);
	db_printf("timer 0x%x:\n", &thread->timer);
	db_show_timeout(&(thread->timer), 1, 0, 0);
	db_printf("depress_timer 0x%x\n", &thread->depress_timer);
	db_show_timeout(&(thread->depress_timer), 1, 0, 0);
#if	MACH_PROF
	db_printf("thread_profiled 0x%x thread_profiled_own 0x%x",
		thread->thread_profiled, thread->thread_profiled_own);
	db_printf(" profil_buffer 0x%x\n", thread->profil_buffer);
#endif	MACH_PROF
	db_printf("active 0x%x ast 0x%x\n", thread->active, thread->ast);
	db_printf("processor_set 0x%x bound_processor 0x%x\n",
		thread->processor_set, thread->bound_processor);
#if	MACH_HOST
	db_printf("may_assign 0x%x assign_active 0x%x\n",
		thread->may_assign, thread->assign_active);
#endif	MACH_HOST
#if	NCPUS > 1
	db_printf("last_processor 0x%x\n", thread->last_processor);
#if	MACH_LOCK_MON
	db_printf("lock_stat %d\n", thread->lock_stat);
#endif	MACH_LOCK_MON
#if	ASMP
	db_printf("baton_level %d\n", thread->baton_level);
#endif	ASMP
#endif	NCPUS > 1
}

void
db_print_thread(thread, thread_id, flag)
	thread_t thread;
	int	 thread_id;
	int	 flag;
{
	if (flag & OPTION_USER) {
	    char status[8];
	    char *indent = "";

	    if (flag & OPTION_LONG) {
		if (flag & OPTION_INDENT)
		    indent = "    ";
		if (flag & OPTION_THREAD_TITLE) {
		    db_printf("%s ID: THREAD   STAT   STACK    PCB", indent);
		    db_printf("      SUS PRI SV ");
		    db_printf("CONTINUE,WAIT_FUNC\n");
		}
#if	MACH_FIXPRI
		db_printf("%s%3d%c %08x %s %08x %08x %3d %c%2d ",
		    indent, thread_id,
		    (thread == current_thread())? '#': ':',
		    thread, db_thread_stat(thread, status),
		    thread->kernel_stack, thread->pcb,
		    thread->suspend_count,
		    (thread->policy == POLICY_FIXEDPRI) ? '!' : ' ',
		    thread->sched_pri);
#else	/* MACH_FIXPRI */
		db_printf("%s%3d%c %08x %s %08x %08x %3d %3d ",
		    indent, thread_id,
		    (thread == current_thread())? '#': ':',
		    thread, db_thread_stat(thread, status),
		    thread->kernel_stack, thread->pcb,
		    thread->suspend_count, thread->sched_pri);
#endif	/* MACH_FIXPRI */
		db_printf("%c%c ",
			(thread->stack_privilege) ? 's' : '-',
			(thread->vm_privilege) ? 'v' : '-');
		if ((thread->state & TH_SWAPPED) && thread->swap_func) {
		    db_task_printsym((db_addr_t)thread->swap_func,
				     DB_STGY_ANY, TASK_NULL);
		    db_printf(", ");
		}
		if (thread->state & TH_WAIT)
		    db_task_printsym((db_addr_t)thread->wait_event,
				     DB_STGY_ANY, TASK_NULL);
		db_printf("\n");
	    } else {
		if (thread_id % 3 == 0) {
		    if (flag & OPTION_INDENT)
			db_printf("\n    ");
		} else
		    db_printf(" ");
		db_printf("%3d%c(%08x,%s)", thread_id, 
		    (thread == current_thread())? '#': ':',
		    thread, db_thread_stat(thread, status));
	    }
	} else {
	    if (flag & OPTION_INDENT)
		db_printf("           %3d%c ",
			thread_id,
			(thread == current_thread())? '#': ':');
	    db_printf("(%08x) ", thread);
	    db_printf("%c%c%c%c",
		    (thread->state & TH_RUN)  ? 'R' : ' ',
		    (thread->state & TH_WAIT) ? 'W' : ' ',
		    (thread->state & TH_SUSP) ? 'S' : ' ',
		    (thread->state & TH_UNINT)? 'N' : ' ');
	    if (thread->state & TH_SWAPPED) {
		if (thread->swap_func) {
		    db_printf("(");
		    db_printsym((db_addr_t)thread->swap_func, DB_STGY_ANY);
		    db_printf(")");
		} else {
		    db_printf("(swapped)");
		}
	    }
	    if (thread->state & TH_WAIT) {
		db_printf(" ");
		db_printsym((db_addr_t)thread->wait_event, DB_STGY_ANY);
	    }
	    db_printf("\n");
	}
	if (flag & OPTION_VERBOSE) {
		db_print_thread_verbose(thread, thread_id, flag);
	}

}

void
db_print_task(task, task_id, flag)
	task_t	task;
	int	task_id;
	int	flag;
{
	thread_t thread;
	int thread_id;

	if (flag & OPTION_USER) {
	    if (flag & OPTION_TASK_TITLE) {
		db_printf(" ID: TASK     MAP      THD SUS PR %s", 
			  DB_TASK_NAME_TITLE);
		if ((flag & OPTION_LONG) == 0)
		    db_printf("  THREADS");
		db_printf("\n");
	    }
	    db_printf("%3d: %08x %08x %3d %3d %2d ",
			    task_id, task, task->map, task->thread_count,
			    task->suspend_count, task->priority);
	    DB_TASK_NAME(task);
	    if (flag & OPTION_LONG) {
		if (flag & OPTION_TASK_TITLE)
		    flag |= OPTION_THREAD_TITLE;
		db_printf("\n");
	    } else if (task->thread_count <= 1)
		flag &= ~OPTION_INDENT;
	    thread_id = 0;
	    queue_iterate(&task->thread_list, thread, thread_t, thread_list) {
		db_print_thread(thread, thread_id, flag);
		flag &= ~OPTION_THREAD_TITLE;
		thread_id++;
	    }
	    if ((flag & OPTION_LONG) == 0)
		db_printf("\n");
	} else {
	    if (flag & OPTION_TASK_TITLE)
		db_printf("    TASK        THREADS\n");
	    db_printf("%3d (%08x): ", task_id, task);
	    if (task->thread_count == 0) {
		db_printf("no threads\n");
	    } else {
		if (task->thread_count > 1) {
		    db_printf("%d threads: \n", task->thread_count);
		    flag |= OPTION_INDENT;
		} else
		    flag &= ~OPTION_INDENT;
		thread_id = 0;
		queue_iterate(&task->thread_list, thread,
			      thread_t, thread_list)
		    db_print_thread(thread, thread_id++, flag);
	    }
	}
}

void
db_print_space(task, task_id, flag)
	task_t	task;
	int	task_id;
	int	flag;
{
	ipc_space_t space;

	space = task->itk_space;
	db_printf("%3d: %08x %08x %08x %sactive   %d\n",
		  task_id, task, space, task->map,
		  space->is_active? "":"!", space->is_table_size);
}


void
db_print_task_vm(task, task_id, title, modif)
	task_t		task;
	int		task_id;
	boolean_t	title;
	char		*modif;
{
	vm_map_t	map;
	pmap_t		pmap;
	vm_size_t	size;
	long		resident;
	long		wired;
	extern vm_size_t	db_vm_map_total_size();

	if (title) {
		db_printf("id     task      map     pmap  virtual  rss pg rss mem  wir pg wir mem\n");
	}

	map = task->map;
	pmap = vm_map_pmap(map);

	size = db_vm_map_total_size(map);
	resident = pmap->stats.resident_count;
	wired = pmap->stats.wired_count;

	db_printf("%2d %08x %08x %08x %7dK  %6d %6dK  %6d %6dK\n",
		task_id,
		task,
		map,
		pmap,
		size / 1024,
		resident, (resident * PAGE_SIZE) / 1024,
		wired, (wired * PAGE_SIZE) / 1024);
}


/*ARGSUSED*/
void
db_show_one_task_vm(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	thread_t	thread;
	task_t		task;
	int		task_id;

	if (have_addr == FALSE) {
		if ((thread = db_default_thread) == THREAD_NULL) {
			if ((thread = current_thread()) == THREAD_NULL) {
				db_printf("no thread.\n");
				return;
			}
		}
		task = thread->task;
	} else {
		task = (task_t) addr;
	}

	task_id = db_lookup_task(task);
	if (task_id < 0) {
		db_printf("0x%x is not a task_t\n", addr);
		return;
	}

	db_print_task_vm(task, task_id, TRUE, modif);
}


/*ARGSUSED*/
void
db_show_all_task_vm(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	task_t		task;
	int		task_id;
	boolean_t	title = TRUE;
	processor_set_t	pset;

	task_id = 0;
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
		queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
			db_print_task_vm(task, task_id, title, modif);
			title = FALSE;
			task_id++;
		}
	}
}


/*ARGSUSED*/
void
db_show_all_threads(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	task_t task;
	int task_id;
	int flag;
	processor_set_t pset;

	flag = OPTION_TASK_TITLE|OPTION_INDENT;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;
	if (db_option(modif, 'v'))
	    flag |= OPTION_VERBOSE;

	task_id = 0;
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		db_print_task(task, task_id, flag);
		flag &= ~OPTION_TASK_TITLE;
		task_id++;
	    }
	}
}

void
db_show_one_space(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	int		flag;
	int		task_id;
	task_t		task;

	flag = OPTION_TASK_TITLE;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	if (!have_addr) {
	    task = db_current_task();
	    if (task == TASK_NULL) {
		db_error("No task\n");
		/*NOTREACHED*/
	    }
	} else
	    task = (task_t) addr;

	if ((task_id = db_lookup_task(task)) < 0) {
	    db_printf("bad task address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}

	db_printf(" ID: TASK     SPACE    MAP               COUNT\n");
	db_print_space(task, task_id, flag);
}

/*ARGSUSED*/
void
db_show_all_spaces(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	task_t task;
	int task_id = 0;
	int flag;
	processor_set_t pset;

	flag = OPTION_TASK_TITLE|OPTION_INDENT;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	db_printf(" ID: TASK     SPACE    MAP               COUNT\n");
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		    db_print_space(task, task_id, flag);
		    task_id++;
	    }
	}
}

/*ARGSUSED*/
void
db_show_one_thread(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	int		flag;
	int		thread_id;
	thread_t	thread;

	flag = OPTION_THREAD_TITLE;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;
	if (db_option(modif, 'v'))
	    flag |= OPTION_VERBOSE;

	if (!have_addr) {
	    thread = current_thread();
	    if (thread == THREAD_NULL) {
		db_error("No thread\n");
		/*NOTREACHED*/
	    }
	} else
	    thread = (thread_t) addr;

	if ((thread_id = db_lookup_thread(thread)) < 0) {
	    db_printf("bad thread address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}

	if (flag & OPTION_USER) {
	    db_printf("TASK%d(%08x):\n",
		      db_lookup_task(thread->task), thread->task);
	    db_print_thread(thread, thread_id, flag);
	} else {
	    db_printf("task %d(%08x): thread %d",
		      db_lookup_task(thread->task), thread->task, thread_id);
	    db_print_thread(thread, thread_id, flag);
	}
	if (db_option(modif, 'i') &&  (thread->state & TH_WAIT) && 
	    thread->kernel_stack == 0) {
	    db_printf("Wait State: msg 0x%x option 0x%x rcv_size 0x%x timeout 0x%x\n",
		thread->ith_msg,
		thread->ith_option,
		thread->ith_rcv_size,
		thread->ith_timeout);
	    db_printf("notify 0x%x object 0x%x mqueue 0x%x\n",
		thread->ith_notify,
		thread->ith_object,
		thread->ith_mqueue);
	}
}

/*ARGSUSED*/
void
db_show_one_task(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	int		flag;
	int		task_id;
	task_t		task;

	flag = OPTION_TASK_TITLE;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;
	if (db_option(modif, 'v'))
	    flag |= OPTION_VERBOSE;

	if (!have_addr) {
	    task = db_current_task();
	    if (task == TASK_NULL) {
		db_error("No task\n");
		/*NOTREACHED*/
	    }
	} else
	    task = (task_t) addr;

	if ((task_id = db_lookup_task(task)) < 0) {
	    db_printf("bad task address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}

	db_print_task(task, task_id, flag);
}



#define	db_pset_kmsg_count(port) \
	(ipc_list_count((port)->ip_pset->ips_messages.imq_messages.ikmq_base))
int
db_port_kmsg_count(port)
ipc_port_t	port;
{
	return (port->ip_pset ? db_pset_kmsg_count(port) : port->ip_msgcount);
}


int
db_port_iterate(task, is_pset)
	task_t task;
	int is_pset;
{
	ipc_entry_t entry;
	int index;
	int n = 0;
	int size;
	ipc_space_t space;

	space = task->itk_space;
	entry = space->is_table;
	size = space->is_table_size;
	for (index = 0; index < size; index++, entry++) {
	    if (entry->ie_bits & MACH_PORT_TYPE_PORT_RIGHTS) {
		ipc_port_t aport = (ipc_port_t)entry->ie_object;
		unsigned bits = entry->ie_bits;

		if (!is_pset){
		    if (n && n % 3 == 0)
			db_printf("\n");
		    db_printf("\t%s%d(%s,%x,%d)",
			    aport->ip_pset ? "pset" : "port",
			    index,
			    (bits & MACH_PORT_TYPE_RECEIVE)? "r":
			    (bits & MACH_PORT_TYPE_SEND)? "s": "S", aport,
			    db_port_kmsg_count(aport));
		    n++;
		} else if (aport->ip_pset) {
		    if (n && (n & 1) == 0)
			db_printf("\n");
		    db_printf("%sport%d(%s,%x,set=%x,%d)",
			    (n & 1) ? "\t" : "    ",  index,
			    (bits & MACH_PORT_TYPE_RECEIVE)? "r":
			    (bits & MACH_PORT_TYPE_SEND)? "s": "S",
			    aport, aport->ip_pset,
			    db_pset_kmsg_count(aport));
		    n++;
		}
	    }
	}
	return(n);
}


ipc_port_t
db_lookup_port(thread, id)
	thread_t thread;
	int id;
{
	register ipc_space_t space;
	register ipc_entry_t entry;

	if (thread == THREAD_NULL)
	    return(0);
	space = thread->task->itk_space;
	if (id < 0 || id >= space->is_table_size)
	    return(0);
	entry = &space->is_table[id];
	if (entry->ie_bits & MACH_PORT_TYPE_PORT_RIGHTS)
	    return((ipc_port_t)entry->ie_object);
	return(0);
}

/* ARGSUSED */
void
db_show_port_id(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	task_t task;

	if (!have_addr) {
	    task = current_task();
	    if (task == TASK_NULL) {
		db_error("No current task?\n");
		/*NOTREACHED*/
	    }
	} else
	    task = (task_t) addr;

	if (db_lookup_task(task) < 0) {
	    db_printf("Bad task address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}
	if (db_port_iterate(task, db_option(modif,'s')))
	    db_printf("\n");
}

/*
 *	Useful system state when the world has hung.
 */
db_sys(verbose)
	int	verbose;
{
	db_vm();
#if	NORMA_IPC
	iprintf("\n");
	db_norma_ipc();
#endif
#if	NORMA2
	iprintf("\n");
	db_sys_dipc(verbose);
#endif
	db_printf("current_{thread/task} 0x%x 0x%x\n",
			current_thread(),current_task());
	return 0;
}


/*
 *	Return pointer to a string representation of a boolean.
 */
char *
db_bool_str(bool)
boolean_t	bool;
{
	return bool == FALSE ? "FALSE" : "TRUE";
}

void
db_show_run_queue(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	struct run_queue *rqp;
	int i;

	if (addr == (db_expr_t)0 || !have_addr) {
		db_printf("No runq\n");
		return;
	}
	rqp = ((struct run_queue *)addr);

	db_printf("run_queue 0x%x: count %d low %d\n",
		rqp, rqp->count, rqp->low);
#if	MACH_SLOCKS
	show_slock("\tlock", &rqp->lock);
#endif	MACH_SLOCKS
	for (i = 0; i < NRQS; ++i)
	{
		int header_printed = 0;
		queue_t	q = &rqp->runq[i];
		thread_t	thread;
		/*
		 * if runq is empty, don't print it
		 */
		queue_iterate(q, thread, thread_t, links)
		{
			if (!header_printed)
			{
				db_printf("runq[%d]:\n", i);
				header_printed = 0;
			}
			db_show_one_thread(thread, 1, 1, modif);
		}
	}
}

void
db_show_processor_state(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	char *str;

	switch((int)addr)
	{
		case PROCESSOR_OFF_LINE:
			str = "PROCESSOR_OFF_LINE";
			break;
		case PROCESSOR_RUNNING:
			str = "PROCESSOR_RUNNING";
			break;
		case PROCESSOR_IDLE:
			str = "PROCESSOR_IDLE";
			break;
		case PROCESSOR_DISPATCHING:
			str = "PROCESSOR_DISPATCHING";
			break;
		case PROCESSOR_ASSIGN:
			str = "PROCESSOR_ASSIGN";
			break;
		case PROCESSOR_SHUTDOWN:
			str = "PROCESSOR_SHUTDOWN";
			break;
		case PROCESSOR_VIDLE:
			str = "PROCESSOR_VIDLE";
			break;
		default:
			str = "unknown";
			break;
	}
	db_printf("processor_state=0x%x<%s>\n", addr, str);
}


/* call with queue_t, &queue_head_t, &queue_chain_t, queue_entry_t */
void
db_show_queue_entry(char *str, struct queue_entry *qep)
{
	if (qep == 0)
		return;

	if (queue_empty(qep))
		db_printf("%s: empty\n", str);
	else
		db_printf("%s: next 0x%x prev 0x%x\n",
			str, qep->next, qep->prev);
}

void
db_show_processor(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	struct processor *p;

	if (addr == (db_expr_t)0 || !have_addr) {
		db_error("No processor\n");
		/*NOTREACHED*/
	}

	p = ((struct processor *)addr);

	db_printf("Processor 0x%x:\nlocal run queue:\n", addr);
	db_show_run_queue(&(p->runq), 1, count, modif);
	db_show_queue_entry("processor_queue", &p->processor_queue);
	db_show_processor_state(p->state, 1, count, modif);
	db_printf("next_thread 0x%x\n", p->next_thread);
	db_printf("idle_thread 0x%x\n", p->idle_thread);
	db_printf("first_quantum %d last_quantum %d\n",
		p->first_quantum, p->last_quantum);
	db_printf("processor_set 0x%x", p->processor_set);
	db_printf(" processor_set_next 0x%x\n", p->processor_set_next);
	db_show_queue_entry("processors", &p->processors);
	db_printf("slot_num 0x%x\n", p->slot_num);
#if	MACH_SLOCKS
	show_slock("lock", &(p->lock));
#endif	MACH_SLOCKS
	db_printf("processor_self 0x%x\n", p->processor_self);
#if	NCPUS > 1
	db_printf("ast_check 0x%x\n", (int)(p->ast_check_data));
	db_show_queue_entry("softclock_queue", &p->softclock_queue);
#endif	/* NCPUS >1 */
}
void
db_show_all_processors(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	int i;

	for (i = 0; i < NCPUS; ++i)
	{
		if(processor_ptr[i])
			db_show_processor(processor_ptr[i], 1, count, modif);
			db_printf("\n");
	}
}

void
db_show_pset(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	struct processor_set *psp;

	if (!have_addr)
		addr = (db_expr_t)&default_pset;

	if (addr == (db_expr_t)0) {
		db_error("No pset\n");
		/*NOTREACHED*/
	}

	psp = (struct processor_set *)addr;

	db_printf("pset 0x%x\n", psp);
	db_show_run_queue(&psp->runq, 1, count, modif);
	db_show_queue_entry("idle_queue", &psp->idle_queue);
	db_printf("idle_count %d\n", psp->idle_count);
#if	MACH_SLOCKS
	show_slock("idle_lock", &psp->idle_lock);
#endif	MACH_SLOCKS
	db_show_queue_entry("processors", &psp->processors);
	db_printf("processor_count %d\n", psp->processor_count);
	db_printf("empty %d\n", psp->empty);
	db_show_queue_entry("tasks", &psp->tasks);
	db_printf("task_count %d\n", psp->task_count);
	db_show_queue_entry("threads", &psp->threads);
	db_printf("thread_count %d\n", psp->thread_count);
	db_printf("ref_count %d\n", psp->ref_count);
	db_show_queue_entry("all_psets", &psp->all_psets);
	db_printf("active %d\n", psp->active);
#if	MACH_SLOCKS
	show_slock("misc lock", &psp->lock);
#endif	MACH_SLOCKS

}
