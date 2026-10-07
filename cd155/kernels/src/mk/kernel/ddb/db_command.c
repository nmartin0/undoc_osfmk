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
 * $Log: db_command.c,v $
 * Revision 1.19  1994/11/18  20:29:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/08/31  21:23:51  mtm
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
 * Revision 1.16  1994/07/29  21:47:45  rkl
 *  Added 'show payload' command to support dumping out DIPC RPC payload buffers.
 *
 * Revision 1.15  1994/07/22  16:14:33  andyp
 * Added 3 new debugger commands "show pmap <pmap>" to display a pmap,
 * "show vmtask <task>" to display the size of the virtual address
 * space, resident, and wired page counts.
 *
 * Revision 1.14  1994/07/14  21:47:08  stans
 *  Support new debug command "show prights ipc-port-address". List all tasks
 *  which have a right for the specified port.
 *
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: debug
 *  Testing: developer
 *
 * Revision 1.13  1994/07/12  19:17:19  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.12  1994/06/02  22:44:16  sean
 *  Reviewer:  regnier
 *  Risk:  low
 *  Benefit or PTS #:  in the kernel debugger, hitting return no longer repeats
 * 		command.  A new debugger command is added "repeat" which
 * 		toggles this feature (ie turns if on if it's off and vice
 * 		versa).
 *  Testing:	developer testing
 *  Module(s):  ddb/db_command.c
 *
 * Revision 1.11  1994/05/06  21:16:41  lenb
 *         delete 'show cpu' -> db_show_cpu()
 *         delete 'show all cpus' -> db_show_all_cpus()
 *
 * Revision 1.10  1994/03/19  00:58:18  lenb
 *  Reviewer: sean
 *  Risk: low
 *  Benefit or PTS #: more debugging show commands
 *
 * show processor
 * show all processors
 * show timeout
 * show all timeouts
 * show all softclocks
 *
 * Revision 1.9  1994/03/06  22:15:32  sean
 *  Reviewer: steved
 *  Risk: low
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
 * Revision 1.8.4.9  1994/06/28  19:14:56  stans
 *         removed unused rtns:
 *                 db_show_help()
 *                 db_help_cmd()
 *
 *         Function 'db_cmd_list()' now utilizes db_will_wrap() to determine if
 *         newline is needed. Pretty print on '?' command option.
 *
 *         Changed two NORMA_VM conditionals to be (NORMA_IPC || NORMA2) as they
 *         were not really NORMA_VM issues; prep for NORMA_VM going away (ASVM).
 *
 *         Support new debug commands:
 *
 *         show all rpc_handles rpc_groups rpc_classes rdma_handles rdma_groups
 *
 *         show rpc_handle x
 *         show rpc_group x
 *         show rpc_class x
 *
 *         show rdma_handle x
 *         show rdma_group x
 *         show rdma_engine x
 *
 *         show ptrace port-ptr    # show port's creation call chain.
 *
 *         show prefs n            # show all ports with 'n' references
 *
 * Revision 1.8.4.8  1994/05/18  22:33:43  stans
 * NORMA_IPC --> NORMA_VM
 *
 * Revision 1.8.4.7  1994/05/05  02:52:04  stans
 *   Added "show kmq 0x" iterate over a kmsg queue.
 *
 * Revision 1.8.4.6  1994/04/26  16:05:42  stans
 * show uid & NORMA_IPC == 0 work
 *
 * Revision 1.8.4.5  1994/03/15  23:07:47  andyp
 * show_norma_log() is only compiled when MACH_ASSERT is 1.
 *
 * Revision 1.8.4.4  1994/03/15  00:12:31  rkl
 *  Added norma_log under show commands.
 *
 * Revision 1.8.4.3  1994/02/23  21:01:03  rkl
 *  Added `resize' command to set debugger notion of window size based on
 *  current window size.
 *
 * Revision 1.8.4.2  1994/02/11  23:07:17  stans
 * show args support
 *
 * Revision 1.8.4.1  1994/02/08  18:56:57  andyp
 * Force "indent" back to the left-most column when at the top of
 * the command loop (eg, a command is aborted by 'q' and the longjmp
 * returns).
 *
 * Revision 1.8  1993/10/20  16:41:51  stans
 *    MP3: kdb command 'cpu n' to move kdb to another cpu.
 *
 * Revision 1.7  1993/09/28  17:53:08  andyp
 * Update with extra debugging.  Recovered OSF's logs.
 *
 *
 *	Add "show xmm_stack".  [alanl@osf.org]
 *
 * Revision 1.6  1993/07/01  15:31:31  terry
 * source sync with nmk13.25
 *
 * Revision 1.2.4.2  1993/06/16  17:17:34  dwm
 * 	Add 'show all spaces' from sjs/nmk14.
 * 	[1993/06/16  17:16:28  dwm]
 *
 * Revision 1.5  1993/06/30  22:20:25  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/05/23  18:53:59  prp
 * Fix ctrap config problem.
 *
 * Revision 1.3  1993/04/27  20:18:28  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:04  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: db_command.c,v
 * Revision 1.2.4.2  1993/06/16  17:17:34  dwm
 * 	Add 'show all spaces' from sjs/nmk14.
 * 	[1993/06/16  17:16:28  dwm]
 *
 * Revision 1.2  1992/11/25  01:04:16  robert
 * 	integrate changes below for norma_14
 * 
 * 	Dave Mitchell (dwm) at osf.org 12-Nov-92
 * 	Add show (all) zone(s) commands.
 * 	[1992/11/18  23:29:28  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/13  19:21:19  robert]
 * 
 * Revision 1.1  1992/09/30  02:00:56  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.16.2.7  92/09/15  17:14:21  jeffreyh
 * 	Add call to netipc_packet_state printing code.  With Jeffreyh.
 * 	[92/08/20            alanl]
 * 
 * Revision 2.16.2.6  92/04/30  11:48:57  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.16.2.4  92/03/28  10:04:15  jeffreyh
 * 	Added  traphist and vaddrs for the i860
 * 	[92/03/20            andyp]
 * 
 * Revision 2.16.2.3  92/02/21  11:22:58  jsb
 * 	Added show pset, show all vuids.
 * 	[92/02/20  10:52:22  jsb]
 * 
 * Revision 2.16.2.2  92/02/18  18:38:34  jeffreyh
 * 	Print cpu number with the prompt
 * 	added a "cpu" command to switch between cpus on the Corollary
 * 	[91/06/25            bernadat]
 * 	Added new show callhere option to debugger for iPSC only.
 * 	[92/02/13  12:31:30  jeffreyh]
 * 
 * Revision 2.16.2.1  92/01/21  21:49:40  jsb
 * 	NORMA_VM: added show xmm_obj, show xmm_reply.
 * 	[92/01/21  18:13:32  jsb]
 * 
 * 	NORMA_IPC: added show all uids, proxies, principals.
 * 	[92/01/16  21:25:38  jsb]
 * 
 * 	Added show copy, show packet, show pcs.
 * 	[92/01/13  10:13:50  jsb]
 * 
 * Revision 2.16  91/10/09  15:58:27  af
 * 	Revision 2.15.1.1  91/10/05  13:05:19  jeffreyh
 * 	Added compound, macro, and conditional command support.
 * 	Added "xf" and "xb" command to examine forward and backward.
 * 	Added last command execution with "!!".
 * 	Added "show task" command.
 * 	Added "ipc_port" command to print all IPC ports in a task.
 * 	Added dot address and default thread indicator in prompt.
 * 	Changed error messages to output more information.
 * 	Moved "skip_to_eol()" to db_lex.c.
 * 	[91/08/29            tak]
 * 
 * Revision 2.15  91/08/24  11:55:24  af
 * 	Added optional funcall at ddb entry: similar to GDB's
 * 	"display"s in spirit.
 * 	[91/08/02  02:42:11  af]
 * 
 * Revision 2.14  91/08/03  18:17:16  jsb
 * 	Added `show kmsg' and `show msg'.
 * 	Replaced NORMA_BOOT conditionals with NORMA_IPC.
 * 	Use _node_self instead of node_self() for prompt,
 * 	since node_self() will panic if _node_self not intialized.
 * 	[91/07/24  22:48:48  jsb]
 * 
 * Revision 2.13  91/07/11  11:00:32  danner
 * 	Copyright Fixes
 * 
 * Revision 2.12  91/07/09  23:15:42  danner
 * 	Modified the command loop to include the cpu number in the
 * 	 command prompt on multicpu machines, and node number in norma
 * 	      systems.
 * 	[91/04/12            danner]
 *
 * Revision 2.11  91/07/01  08:24:04  jsb
 * 	Added support for 'show all slocks'.
 * 	[91/06/29  15:59:01  jsb]
 * 
 * Revision 2.10  91/06/17  15:43:46  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  09:57:51  jsb]
 * 
 * 	Moved struct command definition to db_command.h, added include of
 * 	 db_command.h, renamed struct command to struct db_command. All
 * 	 in support of machine dependpent command extensions to db. 
 * 	[91/03/12            danner]
 * 
 * Revision 2.9  91/06/06  17:03:47  jsb
 * 	NORMA support: prompt includes node number, e.g., db4>.
 * 	[91/05/25  10:47:35  jsb]
 * 
 * Revision 2.8  91/05/14  15:32:51  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/03/16  14:42:30  rpd
 * 	Added db_recover.
 * 	[91/01/13            rpd]
 * 
 * Revision 2.6  91/02/05  17:06:10  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:17:18  mrt]
 * 
 * Revision 2.5  91/01/08  17:31:54  rpd
 * 	Forward reference for db_fncall();
 * 	[91/01/04  12:35:17  rvb]
 * 
 * 	Add call as a synonym for ! and match for next
 * 	[91/01/04  12:14:48  rvb]
 * 
 * Revision 2.4  90/11/07  16:49:15  rpd
 * 	Added search.
 * 	[90/11/06            rpd]
 * 
 * Revision 2.3  90/10/25  14:43:45  rwd
 * 	Changed db_fncall to print the result unsigned.
 * 	[90/10/19            rpd]
 * 
 * 	Added CS_MORE to db_watchpoint_cmd.
 * 	[90/10/17            rpd]
 * 	Added watchpoint commands: watch, dwatch, show watches.
 * 	[90/10/16            rpd]
 * 
 * Revision 2.2  90/08/27  21:50:10  dbg
 * 	Remove 'listbreaks' - use 'show breaks' instead.  Change 'show
 * 	threads' to 'show all threads' to avoid clash with 'show thread'.
 * 	Set 'dot' here from db_prev or db_next, depending on 'db_ed_style'
 * 	flag and syntax table.
 * 	[90/08/22            dbg]
 * 	Reduce lint.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

/*
 * Command dispatcher.
 */
#include <cpus.h>
#include <norma_ipc.h>
#include <norma_vm.h>
#include <mach_assert.h>

#include <mach/boolean.h>
#include <machine/db_machdep.h>

#include <ddb/db_lex.h>
#include <ddb/db_output.h>
#include <ddb/db_command.h>
#include <ddb/db_task_thread.h>

#include <machine/setjmp.h>
#include <kern/thread.h>

#include <ctrap_history.h>

/*
 * Exported global variables
 */
boolean_t	db_cmd_loop_done;
jmp_buf_t	*db_recover = 0;
db_addr_t	db_dot;
db_addr_t	db_last_addr;
db_addr_t	db_prev;
db_addr_t	db_next;

/*
 * if 'ed' style: 'dot' is set at start of last item printed,
 * and '+' points to next line.
 * Otherwise: 'dot' points to next item, '..' points to last.
 */
boolean_t	db_ed_style = TRUE;

/*
 * Results of command search.
 */
#define	CMD_UNIQUE	0
#define	CMD_FOUND	1
#define	CMD_NONE	2
#define	CMD_AMBIGUOUS	3
#define	CMD_HELP	4

/*
 * Search for command prefix.
 */
int
db_cmd_search(name, table, cmdp)
	char *		name;
	struct db_command	*table;
	struct db_command	**cmdp;	/* out */
{
	struct db_command	*cmd;
	int		result = CMD_NONE;

	for (cmd = table; cmd->name != 0; cmd++) {
	    register char *lp;
	    register char *rp;
	    register int  c;

	    lp = name;
	    rp = cmd->name;
	    while ((c = *lp) == *rp) {
		if (c == 0) {
		    /* complete match */
		    *cmdp = cmd;
		    return (CMD_UNIQUE);
		}
		lp++;
		rp++;
	    }
	    if (c == 0) {
		/* end of name, not end of command -
		   partial match */
		if (result == CMD_FOUND) {
		    result = CMD_AMBIGUOUS;
		    /* but keep looking for a full match -
		       this lets us match single letters */
		}
		else {
		    *cmdp = cmd;
		    result = CMD_FOUND;
		}
	    }
	}
	if (result == CMD_NONE) {
	    /* check for 'help' */
	    if (!strncmp(name, "help", strlen(name)))
		result = CMD_HELP;
	}
	return (result);
}

void
db_cmd_list(table)
	struct db_command *table;
{
	register struct db_command *cmd;

	for (cmd = table; cmd->name != 0; cmd++) {
	    if ( db_will_wrap(13) ) /* 13 must match 13 in db_printf() below */
		db_printf("\n");
	    db_printf("%-13s", cmd->name);
	}
}

int	db_command_repeat = 0;

void
db_repeat_toggle()
{
	db_command_repeat = !db_command_repeat;
	printf(
		"Empty line command repeat is %s.\n",
		db_command_repeat ? "on" : "off"
	);
	
}

void
db_command(last_cmdp, cmd_table)
	struct db_command	**last_cmdp;	/* IN_OUT */
	struct db_command	*cmd_table;
{
	struct db_command	*cmd;
	int		t;
	char		modif[TOK_STRING_SIZE];
	db_expr_t	addr, count;
	boolean_t	have_addr;
	int		result;

	t = db_read_token();
	if (db_command_repeat && (t == tEOL || t == tSEMI_COLON)) {
	    /* empty line repeats last command, at 'next' */
	    cmd = *last_cmdp;
	    addr = (db_expr_t)db_next;
	    have_addr = FALSE;
	    count = 1;
	    modif[0]  = '\0';
	    if (t == tSEMI_COLON)
		db_unread_token(t);
	}
	else if (t == tEXCL) {
	    void db_fncall();
	    db_fncall();
	    return;
	}
	else if (t != tIDENT) {
	    db_printf("?\n");
	    db_flush_lex();
	    return;
	}
	else {
	    /*
	     * Search for command
	     */
	    while (cmd_table) {
		result = db_cmd_search(db_tok_string,
				       cmd_table,
				       &cmd);
		switch (result) {
		    case CMD_NONE:
			if (db_exec_macro(db_tok_string) == 0)
			    return;
			db_printf("No such command \"%s\"\n", db_tok_string);
			db_flush_lex();
			return;
		    case CMD_AMBIGUOUS:
			db_printf("Ambiguous\n");
			db_flush_lex();
			return;
		    case CMD_HELP:
			db_cmd_list(cmd_table);
			db_flush_lex();
			return;
		    default:
			break;
		}
		if ((cmd_table = cmd->more) != 0) {
		    t = db_read_token();
		    if (t != tIDENT) {
			db_cmd_list(cmd_table);
			db_flush_lex();
			return;
		    }
		}
	    }

	    if ((cmd->flag & CS_OWN) == 0) {
		/*
		 * Standard syntax:
		 * command [/modifier] [addr] [,count]
		 */
		t = db_read_token();
		if (t == tSLASH) {
		    t = db_read_token();
		    if (t != tIDENT) {
			db_printf("Bad modifier \"/%s\"\n", db_tok_string);
			db_flush_lex();
			return;
		    }
		    db_strcpy(modif, db_tok_string);
		}
		else {
		    db_unread_token(t);
		    modif[0] = '\0';
		}

		if (db_expression(&addr)) {
		    db_dot = (db_addr_t) addr;
		    db_last_addr = db_dot;
		    have_addr = TRUE;
		}
		else {
		    addr = (db_expr_t) db_dot;
		    have_addr = FALSE;
		}
		t = db_read_token();
		if (t == tCOMMA) {
		    if (!db_expression(&count)) {
			db_printf("Count missing after ','\n");
			db_flush_lex();
			return;
		    }
		}
		else {
		    db_unread_token(t);
		    count = -1;
		}
	    }
	}
	*last_cmdp = cmd;
	if (cmd != 0) {
	    /*
	     * Execute the command.
	     */
	    (*cmd->fcn)(addr, have_addr, count, modif);

	    if (cmd->flag & CS_SET_DOT) {
		/*
		 * If command changes dot, set dot to
		 * previous address displayed (if 'ed' style).
		 */
		if (db_ed_style) {
		    db_dot = db_prev;
		}
		else {
		    db_dot = db_next;
		}
	    }
	    else {
		/*
		 * If command does not change dot,
		 * set 'next' location to be the same.
		 */
		db_next = db_dot;
	    }
	}
}

void
db_command_list(last_cmdp, cmd_table)
	struct db_command	**last_cmdp;	/* IN_OUT */
	struct db_command	*cmd_table;
{
	void db_skip_to_eol();

	do {
	    db_command(last_cmdp, cmd_table);
	    db_skip_to_eol();
	} while (db_read_token() == tSEMI_COLON && db_cmd_loop_done == 0);
}

/*
 * 'show' commands
 */
extern void	db_listbreak_cmd();
extern void	db_listwatch_cmd();
extern void	db_show_regs(), db_show_one_thread(), db_show_one_task();
extern void	db_show_all_threads();
extern void	db_show_one_space(), db_show_all_spaces();
extern void	db_show_macro();
extern void	vm_map_print(), vm_object_print(), vm_page_print();
extern void	vm_map_copy_print();
extern void	ipc_port_print(), ipc_pset_print(), db_show_all_slocks();
extern void	ipc_kmsg_print(), ipc_msg_print(), ipc_kmsg_print_queue();
extern void	db_show_port_id();
void		db_show_machine_args();

#if	NORMA_IPC
extern void	netipc_packet_print(), netipc_pcs_print(), db_show_all_uids();
extern void	netipc_pacstate_print();
extern void	db_show_all_proxies(), db_show_all_principals();
extern void	db_show_all_uids_verbose();
#if	iPSC386 || iPSC860
extern void	db_show_netipc_called_here();
#endif	iPSC386	|| iPSC860
#endif	NORMA_IPC

#if	i860
#if	CTRAP_HISTORY
extern void	db_show_trap_hist();
#endif	CTRAP_HISTORY
extern void	db_show_vaddrs();
extern void	db_show_pmap();
#endif	i860

#if	NORMA2
#if	MACH_ASSERT
extern	void	show_norma_log();
extern  void	port_trace();
extern  void	db_ref();
extern  void	db_tasks_by_port();
#endif	MACH_ASSERT
#if	RDMA
extern  void	rdma_print_handle();
extern  void	rdma_print_group();
extern  void	rdma_print_engine();
extern  void	rdma_print_all_handles();
extern  void	rdma_print_all_groups();
#endif
#if	RPC
extern  void	rpc_print_handle();
extern  void	rpc_print_group();
extern  void	rpc_print_class();
extern  void	rpc_print_all_handles();
extern  void	rpc_print_all_groups();
extern  void	rpc_print_all_classes();
extern	void	db_show_payload();
#endif
extern	void	db_show_dipc_uid();
#endif	/* NORMA2 */

#if	NORMA_VM
extern void	xmm_obj_print(), xmm_reply_print();
extern void	xmm_stack_print();
#endif	NORMA_VM

extern void	db_show_one_task_vm(), db_show_all_task_vm();
extern void	db_show_one_zone(), db_show_all_zones();
extern void	db_show_all_processors(), db_show_processor();
extern void	db_show_pset();
extern void	db_show_timeout(), db_show_all_timeouts();
extern void	db_show_all_softclocks();

struct db_command db_show_all_cmds[] = {
	{ "threads",	db_show_all_threads,	0,	0 },
	{ "slocks",	db_show_all_slocks,	0,	0 },
#if	NORMA_IPC
	{ "uids",	db_show_all_uids,	0,	0 },
	{ "proxies",	db_show_all_proxies,	0,	0 },
	{ "principals",	db_show_all_principals,	0,	0 },
	{ "vuids",	db_show_all_uids_verbose, 0,	0 },
#endif	NORMA_IPC
	{ "spaces",	db_show_all_spaces,	0,	0 },
	{ "zones",	db_show_all_zones,	0,	0 },
	{ "processors",	db_show_all_processors,	0,	0 },
	{ "timeouts",	db_show_all_timeouts,	0,	0 },
	{ "softclocks",	db_show_all_softclocks,	0,	0 },
	{ "vmtask",	db_show_all_task_vm,	0,	0 },
#if	NORMA2 && RPC
	{ "rpc_handles",rpc_print_all_handles,	0,	0 },
	{ "rpc_groups",rpc_print_all_groups,	0,	0 },
	{ "rpc_classes",rpc_print_all_classes,	0,	0 },
#endif
#if	NORMA2 && RDMA
	{ "rdma_handles",rdma_print_all_handles,0,	0 },
	{ "rdma_groups",rdma_print_all_groups,	0,	0 },
#endif
	{ (char *)0 }
};

struct db_command db_show_cmds[] = {
	{ "all",	0,			0,	db_show_all_cmds },
	{ "args",	db_show_machine_args,	0,	0 },
	{ "registers",	db_show_regs,		0,	0 },
	{ "breaks",	db_listbreak_cmd, 	0,	0 },
	{ "watches",	db_listwatch_cmd, 	0,	0 },
	{ "thread",	db_show_one_thread,	0,	0 },
	{ "task",	db_show_one_task,	0,	0 },
	{ "vmtask",	db_show_one_task_vm,	0,	0 },
	{ "macro",	db_show_macro,		CS_OWN, 0 },
	{ "map",	vm_map_print,		0,	0 },
	{ "object",	vm_object_print,	0,	0 },
	{ "page",	vm_page_print,		0,	0 },
	{ "copy",	vm_map_copy_print,	0,	0 },
	{ "port",	ipc_port_print,		0,	0 },
	{ "pset",	ipc_pset_print,		0,	0 },
	{ "kmsg",	ipc_kmsg_print,		0,	0 },
	{ "kmq",	ipc_kmsg_print_queue,	0,	0 },
	{ "msg",	ipc_msg_print,		0,	0 },
	{ "ipc_port",	db_show_port_id,	0,	0 },

#if	NORMA_IPC
	{ "packet",	netipc_packet_print,	0,	0 },
	{ "pstate",	netipc_pacstate_print,	0,	0 },
	{ "pcs",	netipc_pcs_print,	0,	0 },
#if	iPSC386 || iPSC860
	{ "callhere",	db_show_netipc_called_here,	0,	0 },
#endif	iPSC386 || iPSC860
#endif	NORMA_IPC

#if	NORMA2
	{ "uid",	db_show_dipc_uid,	0,	0 },
#endif	/* NORMA2 */

#if	i860
#if	CTRAP_HISTORY
	{ "traphist",	db_show_trap_hist,	0,	0 },
#endif	CTRAP_HISTORY
	{ "vaddrs",	db_show_vaddrs,		0,	0 },
	{ "pmap",	db_show_pmap,		0,	0 },
#endif	i860

#if	NORMA2 && MACH_ASSERT
	{ "norma_log",	show_norma_log,		0,	0 },
	{ "prefs",	db_ref,			0,	0 },
	{ "prights",	db_tasks_by_port,	0,	0 },
	{ "ptrace",	port_trace,		0,	0 },
#endif	NORMA2 && MACH_ASSERT

#if	NORMA2 && RPC
	{ "rpc_handle",	rpc_print_handle,	0,	0 },
	{ "rpc_group",	rpc_print_group,	0,	0 },
	{ "rpc_class",	rpc_print_class,	0,	0 },
	{ "payload",	db_show_payload,	0,	0 },
#endif	NORMA2 && RPC

#if	NORMA2 && RDMA
	{ "rdma_handle",rdma_print_handle,	0,	0 },
	{ "rdma_group",	rdma_print_group,	0,	0 },
	{ "rdma_engine",rdma_print_engine,	0,	0 },
#endif	NORMA2 && RDMA

#if	NORMA_VM
	{ "xmm_obj",	xmm_obj_print,		0,	0 },
	{ "xmm_reply",	xmm_reply_print,	0,	0 },
	{ "xmm_stack",	xmm_stack_print,	0,	0 },
#endif	NORMA_VM

	{ "space",	db_show_one_space,	0,	0 },
	{ "zone",	db_show_one_zone,	0,	0 },
	{ "processor",	db_show_processor,	0,	0 },
	{ "processor_set",	db_show_pset,	0,	0 },
	{ "timeout",	db_show_timeout,	0,	0 },
	{ (char *)0, }
};

extern void	db_print_cmd(), db_examine_cmd(), db_set_cmd();
extern void	db_examine_forward(), db_examine_backward();
extern void	db_search_cmd();
extern void	db_write_cmd();
extern void	db_delete_cmd(), db_breakpoint_cmd();
extern void	db_deletewatch_cmd(), db_watchpoint_cmd();
extern void	db_single_step_cmd(), db_trace_until_call_cmd(),
		db_trace_until_matching_cmd(), db_continue_cmd();
extern void	db_stack_trace_cmd(), db_cond_cmd();
extern void	db_resize_cmd();
void		db_def_macro_cmd(), db_del_macro_cmd();
void		db_fncall();
#if	NCPUS > 1
#define	db_switch_cpu kdb_on
extern void	db_switch_cpu();
#endif	NCPUS > 1

#ifdef DB_MACHINE_COMMANDS
extern struct db_command *db_machine_cmds;
#endif

struct db_command db_command_table[] = {
#ifdef DB_MACHINE_COMMANDS
  /* this must be the first entry, if it exists */
	{ "machine",    0,                      0,     		0},
#endif
	{ "print",	db_print_cmd,		CS_OWN,		0 },
	{ "examine",	db_examine_cmd,		CS_MORE|CS_SET_DOT, 0 },
	{ "x",		db_examine_cmd,		CS_MORE|CS_SET_DOT, 0 },
	{ "xf",		db_examine_forward,	CS_SET_DOT,	0 },
	{ "xb",		db_examine_backward,	CS_SET_DOT,	0 },
	{ "search",	db_search_cmd,		CS_OWN|CS_SET_DOT, 0 },
	{ "set",	db_set_cmd,		CS_OWN,		0 },
	{ "write",	db_write_cmd,		CS_MORE|CS_SET_DOT, 0 },
	{ "w",		db_write_cmd,		CS_MORE|CS_SET_DOT, 0 },
	{ "delete",	db_delete_cmd,		CS_OWN,		0 },
	{ "d",		db_delete_cmd,		CS_OWN,		0 },
	{ "break",	db_breakpoint_cmd,	CS_MORE,	0 },
	{ "dwatch",	db_deletewatch_cmd,	CS_MORE,	0 },
	{ "watch",	db_watchpoint_cmd,	CS_MORE,	0 },
	{ "step",	db_single_step_cmd,	0,		0 },
	{ "s",		db_single_step_cmd,	0,		0 },
	{ "continue",	db_continue_cmd,	0,		0 },
	{ "c",		db_continue_cmd,	0,		0 },
	{ "until",	db_trace_until_call_cmd,0,		0 },
	{ "next",	db_trace_until_matching_cmd,0,		0 },
	{ "match",	db_trace_until_matching_cmd,0,		0 },
	{ "trace",	db_stack_trace_cmd,	0,		0 },
	{ "cond",	db_cond_cmd,		CS_OWN,	 	0 },
	{ "call",	db_fncall,		CS_OWN,		0 },
	{ "macro",	db_def_macro_cmd,	CS_OWN,	 	0 },
	{ "dmacro",	db_del_macro_cmd,	CS_OWN,		0 },
	{ "show",	0,			0,	db_show_cmds },
	{ "resize",	db_resize_cmd,		CS_OWN,		0 },
#if	NCPUS > 1
	{ "cpu",	db_switch_cpu,		0,		0 },
#endif	NCPUS > 1
	{ "repeat",	db_repeat_toggle,	0,		0 },
	{ (char *)0, }
};

#ifdef DB_MACHINE_COMMANDS

/* this function should be called to install the machine dependent
   commands. It should be called before the debugger is enabled  */
void db_machine_commands_install(ptr)
struct db_command *ptr;
{
  db_command_table[0].more = ptr;
  return;
}

#endif


struct db_command	*db_last_command = 0;

int	(*ddb_display)();

void
db_command_loop()
{
	jmp_buf_t db_jmpbuf;
	jmp_buf_t *prev = db_recover;
	extern int db_output_line;
	extern int db_macro_level;
	extern int indent;
#if	NORMA_IPC || NORMA2
	extern int _node_self;	/* node_self() may not be callable yet */
#endif	NORMA_IPC || NORMA2

	/*
	 * Initialize 'prev' and 'next' to dot.
	 */
	db_prev = db_dot;
	db_next = db_dot;

	if (ddb_display)
		(*ddb_display)();

	db_cmd_loop_done = 0;
	while (!db_cmd_loop_done) {
	    (void) _setjmp(db_recover = &db_jmpbuf);
	    db_macro_level = 0;
	    if (db_print_position() != 0)
		db_printf("\n");
	    db_output_line = 0;
	    indent = 0;
	    db_printf("db%s", (db_default_thread)? "t": "");
#if	NORMA_IPC || NORMA2
	    db_printf("%d", _node_self);
#endif
#if	NCPUS > 1
	    db_printf("{%d}", cpu_number());
#endif
	    db_printf("> ");

	    (void) db_read_line("!!");
	    db_command_list(&db_last_command, db_command_table);
	}

	db_recover = prev;
}

boolean_t
db_exec_cmd_nest(cmd, size)
	char *cmd;
	int  size;
{
	struct db_lex_context lex_context;

	db_cmd_loop_done = 0;
	if (cmd) {
	    db_save_lex_context(&lex_context);
	    db_switch_input(cmd, size, &lex_context);
	}
	db_command_list(&db_last_command, db_command_table);
	if (cmd)
	    db_restore_lex_context(&lex_context);
	return(db_cmd_loop_done == 0);
}

void
db_error(s)
	char *s;
{
	extern int db_macro_level;

	db_macro_level = 0;
	if (db_recover) {
	    if (s)
		db_printf(s);
	    db_flush_lex();
	    _longjmp(db_recover, 1);
	    /*NOTREACHED*/
	}
}


/*
 * Call random function:
 * !expr(arg,arg,arg)
 */
void
db_fncall()
{
	db_expr_t	fn_addr;
#define	MAXARGS		11
	db_expr_t	args[MAXARGS];
	int		nargs = 0;
	db_expr_t	retval;
	db_expr_t	(*func)();
	int		t;

	if (!db_expression(&fn_addr)) {
	    db_printf("Bad function \"%s\"\n", db_tok_string);
	    db_flush_lex();
	    return;
	}
	func = (db_expr_t (*) ()) fn_addr;

	t = db_read_token();
	if (t == tLPAREN) {
	    if (db_expression(&args[0])) {
		nargs++;
		while ((t = db_read_token()) == tCOMMA) {
		    if (nargs == MAXARGS) {
			db_printf("Too many arguments\n");
			db_flush_lex();
			return;
		    }
		    if (!db_expression(&args[nargs])) {
			db_printf("Argument missing\n");
			db_flush_lex();
			return;
		    }
		    nargs++;
		}
		db_unread_token(t);
	    }
	    if (db_read_token() != tRPAREN) {
		db_printf("?\n");
		db_flush_lex();
		return;
	    }
	}
	while (nargs < MAXARGS) {
	    args[nargs++] = 0;
	}

	retval = (*func)(args[0], args[1], args[2], args[3], args[4],
			 args[5], args[6], args[7], args[8], args[9] );
	db_printf(" %#n\n", retval);
}

boolean_t
db_option(modif, option)
	char	*modif;
	int	option;
{
	register char *p;

	for (p = modif; *p; p++)
	    if (*p == option)
		return(TRUE);
	return(FALSE);
}
