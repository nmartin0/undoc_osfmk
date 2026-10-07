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
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * Paragon multi-processor routines.
 *
 * utilized when NCPUS > 1
 *
 * $Id: msgp_tod.c,v 1.2 1994/11/18 20:47:58 mtm Exp $
 *
 * HISTORY:
 * $Log: msgp_tod.c,v $
 * Revision 1.2  1994/11/18  20:47:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/12/14  23:01:23  steved
 * New file to support Turbo-On-Demand
 *
 * 
 *
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#if NX
#include <i860paragon/mcmsg/mcmsg_nx.h>
#endif NX
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_mp.h>
#include <cpus.h>
#include <mach_kdb.h>
#include <mach_assert.h>

#include <mach/kern_return.h>
#include <mach/processor_info.h>

#include <kern/lock.h>
#include <kern/cpu_number.h>
#include <kern/processor.h>
#include <kern/sched_prim.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/thread_swap.h>

#include <i860/cpu_master.h>

#if	NCPUS > 1
extern boot_mk_verbose;

static struct TURBO_DATA {
	int current_mode;
	int default_mode;
	int turbo_count;
	int mcp_count;
	int can_switch;
	decl_simple_lock_data(,lock)
} tod;

/*
 * Forward function references.
 */
kern_return_t msgp_turbo_mode(task_t task);
kern_return_t msgp_mcp_mode(task_t task);
kern_return_t msgp_free_mode(task_t task);

/*
 * External symbol references
 */

/*
 * Initialize data structures and whatever else we need on startup. This
 * is only called once from the Master Processors.
 *
 */
msgp_tod_init()
{
	/*
	 * Initialize the turbo data structure.
	 */
	simple_lock_init(&tod.lock);	
	tod.turbo_count=0;		/* Count of tasks req TURBO mode */
	tod.mcp_count = 0;		/* Count of tasks req MCP mode */
	tod.can_switch=1;		/* Default 'yes' */
	tod.default_mode=MSGP_MCP_MODE;	/* Default 'mcp' */
	tod.current_mode=tod.default_mode;

	/*
	 * Allow the bootmagic strings to be able to change defaults.
	 */
	if (node_in_list(node_self(), getbootenv("BOOT_TURBO_MODE")))
		tod.default_mode=MSGP_TURBO_MODE;
	if (node_in_list(node_self(), getbootenv("BOOT_LOCK_MODE")))
		tod.can_switch=0;
	if (boot_mk_verbose) {
		printf("Turbo On Demand Initialized...\n");
		printf("\tDefault mode will be %s Mode\n",
			(tod.default_mode == MSGP_MCP_MODE) ? "MCP" : "TURBO");
		printf("\tNode %s permitted to switch modes.\n",
			(tod.can_switch) ? "is" : "is not");
	}
}


/*
 * These are the turbo_extension routines. It's called from xxx_cpu_control
 * and is meant to provide additional control over Turbo mode.
 *
 */
kern_return_t msgp_tod_extensions(task_t task, int cmd)
{
	kern_return_t kr = KERN_FAILURE;

	simple_lock(&tod.lock);
	switch(cmd) {
		case 2:		/* Switch into TURBO mode */
			if (!tod.can_switch) break;
			if (tod.mcp_count) break; 
			if (!task->mcmsg_task) break;
			msgp_turbo_mode(task);
			break;
		case 3:		/* Switch into MCP mode */
			if (!tod.can_switch) break;
			if (tod.turbo_count) break; 
			if (!task->mcmsg_task) break;
			msgp_mcp_mode(task);
			break;

		case 4:		/* Free whatever mode I was in */
			if (!tod.can_switch) break;
			if (!task->mcmsg_task) break;
			msgp_free_mode(task);
			break;
		default:
			break;

	}
	simple_unlock(&tod.lock);
	return(kr);
}


/*
 * Switch into TURBO mode. The turbo lock should be held and all checking
 * to make sure a switch into Turbo mode is permitted needs to have already
 * happened. This routine will return when the switch has been completed.
 */
kern_return_t msgp_turbo_mode(task_t task)
{
	/*
	 * If we're not already in TURBO mode, then switch into Turbo mode.
	 */
	if (tod.current_mode != MSGP_TURBO_MODE) {
		mcmsg_mp_enable=0;
	}

	/*
	 * We are now in Turbo mode. Adjust counts and set the mode of this
	 * task.
	 */
	tod.turbo_count++;
	tod.current_mode = MSGP_TURBO_MODE;
	task->mcmsg_task->mode = MSGP_TURBO_MODE;
	return(KERN_SUCCESS);
}

/*
 * Switch into MCP mode. The turbo lock should be held and all checking
 * to make sure a switch into MCP mode is permitted needs to have already
 * happened. This routine will return when the switch has been completed.
 */
kern_return_t msgp_mcp_mode(task_t task)
{
	/*
	 * If we're not in MCP mode, then switch into it. If we're already
	 * in MCP mode, no problem.
	 */
	if (tod.current_mode != MSGP_MCP_MODE) {
	}

	/*
	 * We are now in MCP mode. Adjust counts and set the mode of this
	 * task.
	 */
	tod.current_mode = MSGP_MCP_MODE;
	tod.turbo_count++;
	task->mcmsg_task->mode = MSGP_MCP_MODE;
	return(KERN_SUCCESS);
}

/*
 * Free anything having to do with Turbo On Demans for the task. This will
 * just mean dropping the count if the task had requested a mode. It does
 * not imply changing modes, but this is not out of the question for the
 * future. 
 * Locks are not taken prior to entering this routine.
 */
kern_return_t msgp_free_mode(task_t task)
{
	if (task->mcmsg_task->mode != MSGP_NO_PREFERENCE)
	{
		simple_lock(&tod.lock);
		if (task->mcmsg_task->mode == MSGP_TURBO_MODE)
			tod.turbo_count--;
		else
			tod.mcp_count--;
		simple_unlock(&tod.lock);
	}
	return(KERN_SUCCESS);
}

#endif /* NCPUS > 1 */
