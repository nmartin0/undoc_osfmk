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
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * HISTORY
 * $Log: syscall_sw.c,v $
 * Revision 1.12  1994/11/18  20:52:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/07/18  21:12:29  regnier
 * Add VCF message passing protocol. No VCF code is compiled in unless
 * the '+vcf' option is used to build the kernel.
 *
 *  Reviewer: wms
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: EATS, developer
 *  Module(s): vm/vm_pageout.c  kern/syscall_sw.c   conf/MASTER.i860
 *             conf/files.i860  i860paragon/mcmsg/mcmsg_config.h
 *             i860paragon/mcmsg/mcmsg_config_vcf.h
 *             i860paragon/msgp/msgp_mp.c
 *             i860paragon/vcf/*
 *
 * Revision 1.10  1994/07/12  19:23:40  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/05/18  00:07:35  regnier
 *  Reviewer: None.
 *  Risk: Low
 *  Benefit: Hook for large packets.
 *  Testing: Message Passing Eats, developer tests.
 *  Module(s): syscall_subr.h, syscall_sw.c, i860_paragon/mcmsg/mcmsg_init.c
 *
 * Revision 1.8.4.3  1994/03/02  18:43:36  andyp
 * Added additional user-mode bindings for RDMA testing.
 *
 * Revision 1.8.4.2  1994/02/07  20:19:31  andyp
 * Rounded out RDMA interfaces and added user-mode bindings for testing.
 *
 * Revision 1.8.4.1  1994/02/04  07:53:29  andyp
 * Added machine-specific RPC and RDMA intialization calls to
 * startup.c (see i860paragon/model_dep.c).  Added temporary
 * user-mode test points for RPC.
 *
 * Revision 1.8  1993/09/29  23:08:08  prp
 * Merge in R1.2 branch message passing.
 *
 * Revision 1.2.2.5.2.1  1993/08/26  01:05:43  prp
 * Add VC message passing module
 *
 * Revision 1.7  1993/06/30  22:45:52  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.6  1993/06/09  01:13:51  terry
 * source sync with OSF
 *
 * Revision 1.5  1993/05/23  00:19:55  prp
 * NX Configurability. Clean NX system calls and move them up.
 *
 * Revision 1.4  1993/04/27  20:36:52  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:37:40  dleslie
 * First R1_0 release
 *
 * Revision 1.1  1993/04/02  23:15:47  regnier
 * Initial revision
 *
 * Revision 2.11.2.3  92/05/27  00:46:14  jeffreyh
 * 	Added system call decls within MCMSG ifdefs
 * 	[regnier@ssd.intel.com]
 * 
 * Revision 2.11.2.2  92/03/28  10:10:37  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:17:52  jeffreyh]
 * 
 * Revision 2.13  92/02/19  16:06:58  elf
 * 	Added syscall_thread_depress_abort.
 * 	[92/01/20            rwd]
 * 
 * Revision 2.12  92/01/03  20:40:19  dbg
 * 	Use continuations always in evc_wait.
 * 	[91/12/27            af]
 * 
 * Revision 2.11  91/12/13  14:54:49  jsb
 * 	Added evc_wait().
 * 	[91/12/12  17:42:00  af]
 * 
 * Revision 2.10  91/05/14  16:47:45  mrt
 * 	Correcting copyright
 * 
 * Revision 2.9  91/03/16  14:52:12  rpd
 * 	Changed swtch, swtch_pri, and thread_switch to MACH_TRAP_STACK.
 * 	[91/01/17            rpd]
 * 
 * Revision 2.8  91/02/05  17:29:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:18:37  mrt]
 * 
 * Revision 2.7  91/01/08  15:17:21  rpd
 * 	Changed to use MACH_TRAP_STACK appropriately.
 * 	[90/12/27  21:20:57  rpd]
 * 
 * Revision 2.6  90/09/09  14:32:58  rpd
 * 	Added mach_port_allocate_name
 * 	[90/09/05            rwd]
 * 
 * Revision 2.5  90/06/19  22:59:30  rpd
 * 	Added mach_port_allocate, mach_port_deallocate, mach_port_insert_right.
 * 	[90/06/02            rpd]
 * 
 * Revision 2.4  90/06/02  14:56:26  rpd
 * 	Updated for new IPC and scheduling technology.
 * 	[90/03/26  22:20:34  rpd]
 * 
 * Revision 2.3  90/05/29  18:36:48  rwd
 * 	New traps from rfr (vm_map, task_create etc.)
 * 	[90/04/20            rwd]
 * 
 * Revision 2.2  89/10/16  15:22:13  rwd
 * 	Added debug option for kern_invalid.
 * 	[89/09/29            rwd]
 * 
 * 	Made swtch_pri trap call swtch, ignoring priority argument (it
 * 	scrambles scheduling).
 * 	[89/02/02            dbg]
 * 
 * 	Removed all non-MACH calls and options.
 * 	[88/10/28            dbg]
 * 
 * Revision 2.1  89/08/03  15:51:36  rwd
 * Created.
 * 
 * Revision 2.8  89/03/05  16:48:24  rpd
 * 	Renamed the obsolete msg_{send,receive,rpc} traps to
 * 	msg_{send,receive,rpc}_old.
 * 	[89/02/15  13:50:10  rpd]
 * 
 * Revision 2.7  89/02/25  18:09:05  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.6  89/01/10  23:32:13  rpd
 * 	Added MACH_IPC_XXXHACK conditionals around the obsolete IPC traps.
 * 	[89/01/10  23:10:07  rpd]
 * 
 * Revision 2.5  88/12/19  02:47:31  mwyoung
 * 	Removed old MACH conditionals.
 * 	[88/12/13            mwyoung]
 * 
 * Revision 2.4  88/10/27  10:48:53  rpd
 * 	Changed msg_{send,receive,rpc}_trap to 20, 21, 22.
 * 	[88/10/26  14:45:13  rpd]
 * 
 * Revision 2.3  88/10/11  10:20:33  rpd
 * 	Changed includes to the new style.  Replaced msg_receive_,
 * 	msg_rpc_ with msg_send_trap, msg_receive_trap, msg_rpc_trap.
 * 	[88/10/06  12:22:30  rpd]
 * 
 *  7-Apr-88  David Black (dlb) at Carnegie-Mellon University
 *	removed thread_times().
 *
 * 18-Jan-88  David Golub (dbg) at Carnegie-Mellon University
 *	Replaced task_data with thread_reply; change is invisible to
 *	users.
 *
 * 03-Mar-88  Douglas Orr (dorr) at Carnegie-Mellon University
 *	Added htg_unix_syscall()
 *
 * 27-Apr-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added new forms of msg_receive, msg_rpc.
 *
 *  1-Apr-87  William Bolosky (bolosky) at Carnegie-Mellon University
 *	Added "WARNING:" comment.
 *
 * 30-Mar-87  David Black (dlb) at Carnegie-Mellon University
 *	Added kern_timestamp()
 *
 * 27-Mar-87  David Black (dlb) at Carnegie-Mellon University
 *	Added thread_times() for MACH_TIME_NEW.  Flushed MACH_TIME.
 *
 * 25-Mar-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Added map_fd.
 *
 * 27-Feb-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	MACH_IPC: Turn off Accent compatibility traps.
 *
 *  4-Feb-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added ctimes() routine, to make a workable restore program.
 *
 * 10-Nov-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Eliminated KPortToPID.
 *
 *  7-Nov-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added "init_process".
 *
 * 29-Oct-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added inode_swap_preference.
 *
 * 12-Oct-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Major reorganization: added msg_send, msg_receive, msg_rpc;
 *	moved real Mach traps up front; renamed table.
 */

#include <mach_ipc_compat.h>

#include <mach/port.h>
#include <mach/kern_return.h>
#include <kern/syscall_sw.h>

/*
 *	To add a new entry:
 *		Add an "MACH_TRAP(routine, arg count)" to the table below.
 *
 *		Add trap definition to mach/syscall_sw.h and
 *		recompile user library.
 *
 * WARNING:	If you add a trap which requires more than 7
 *		parameters, mach/ca/syscall_sw.h and ca/trap.c both need
 *		to be modified for it to work successfully on an
 *		RT.  Similarly, mach/mips/syscall_sw.h and mips/locore.s
 *		need to be modified before it will work on Pmaxen.
 *
 * WARNING:	Don't use numbers 0 through -9.  They (along with
 *		the positive numbers) are reserved for Unix.
 */

int kern_invalid_debug = 0;

mach_port_t	null_port()
{
	if (kern_invalid_debug) Debugger("null_port mach trap");
	return(MACH_PORT_NULL);
}

kern_return_t	kern_invalid()
{
	if (kern_invalid_debug) Debugger("kern_invalid mach trap");
	return(KERN_INVALID_ARGUMENT);
}


/* Include declarations of the trap functions. */

#include <mach/mach_traps.h>
#include <mach/message.h>
#include <kern/syscall_subr.h>

extern	kern_return_t	syscall_vm_map();
extern	kern_return_t	syscall_vm_allocate();
extern	kern_return_t	syscall_vm_deallocate();

extern  kern_return_t	syscall_task_create();
extern  kern_return_t	syscall_task_terminate();
extern  kern_return_t	syscall_task_suspend();
extern  kern_return_t	syscall_task_set_special_port();

extern	kern_return_t	syscall_mach_port_allocate();
extern	kern_return_t	syscall_mach_port_deallocate();
extern	kern_return_t	syscall_mach_port_insert_right();
extern	kern_return_t	syscall_mach_port_allocate_name();

extern	kern_return_t	syscall_thread_depress_abort();
extern	kern_return_t	evc_wait();

extern	kern_return_t	syscall_device_read();
extern	kern_return_t	syscall_device_read_request();
extern  kern_return_t   syscall_device_read_overwrite();
extern  kern_return_t   syscall_device_read_overwrite_request();
extern	kern_return_t	syscall_device_write();
extern	kern_return_t	syscall_device_write_request();

#if    VCF
extern	kern_return_t	syscall_vcf_init();
#endif VCF


mach_trap_t	mach_trap_table[] = {
	MACH_TRAP(kern_invalid, 0),		/* 0 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 1 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 2 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 3 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 4 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 5 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 6 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 7 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 8 */		/* Unix */
	MACH_TRAP(kern_invalid, 0),		/* 9 */		/* Unix */

#if	MACH_IPC_COMPAT
	MACH_TRAP(task_self, 0),		/* 10 */	/* obsolete */
	MACH_TRAP(thread_reply, 0),		/* 11 */	/* obsolete */
	MACH_TRAP(task_notify, 0),		/* 12 */	/* obsolete */
	MACH_TRAP(thread_self, 0),		/* 13 */	/* obsolete */
#else	/* MACH_IPC_COMPAT */
	MACH_TRAP(null_port, 0),		/* 10 */
	MACH_TRAP(null_port, 0),		/* 11 */
	MACH_TRAP(null_port, 0),		/* 12 */
	MACH_TRAP(null_port, 0),		/* 13 */
#endif	/* MACH_IPC_COMPAT */
	MACH_TRAP(kern_invalid, 0),		/* 14 */
	MACH_TRAP(kern_invalid, 0),		/* 15 */
	MACH_TRAP(kern_invalid, 0),		/* 16 */
	MACH_TRAP_STACK(evc_wait, 1),		/* 17 */
	MACH_TRAP(kern_invalid, 0),		/* 18 */
	MACH_TRAP(kern_invalid, 0),		/* 19 */

#if	MACH_IPC_COMPAT
	MACH_TRAP(msg_send_trap, 4),		/* 20 */	/* obsolete */
	MACH_TRAP_STACK(msg_receive_trap, 5),	/* 21 */	/* obsolete */
	MACH_TRAP_STACK(msg_rpc_trap, 6),	/* 22 */	/* obsolete */
#else	/* MACH_IPC_COMPAT */
	MACH_TRAP(kern_invalid, 0),		/* 20 */
	MACH_TRAP(kern_invalid, 0),		/* 21 */
	MACH_TRAP(kern_invalid, 0),		/* 22 */
#endif	/* MACH_IPC_COMPAT */
	MACH_TRAP(kern_invalid, 0),		/* 23 */
	MACH_TRAP(kern_invalid, 0),		/* 24 */
	MACH_TRAP_STACK(mach_msg_trap, 7),	/* 25 */
	MACH_TRAP(mach_reply_port, 0),		/* 26 */
	MACH_TRAP(mach_thread_self, 0),		/* 27 */
	MACH_TRAP(mach_task_self, 0),		/* 28 */
	MACH_TRAP(mach_host_self, 0),		/* 29 */

	MACH_TRAP(kern_invalid, 0),		/* 30 */
	MACH_TRAP(kern_invalid, 0),		/* 31 */
	MACH_TRAP(kern_invalid, 0),		/* 32 */
	MACH_TRAP(kern_invalid, 0),		/* 33 */
	MACH_TRAP(kern_invalid, 0),		/* 34 */
	MACH_TRAP(kern_invalid, 0),		/* 35 */
	MACH_TRAP(kern_invalid, 0),		/* 36 */
	MACH_TRAP(kern_invalid, 0),		/* 37 */
	MACH_TRAP(kern_invalid, 0),		/* 38 */
	MACH_TRAP(kern_invalid, 0),		/* 39 */

	MACH_TRAP(kern_invalid, 0),		/* 40 */
	MACH_TRAP(kern_invalid, 0),		/* 41 */
	MACH_TRAP(kern_invalid, 0),		/* 42 */
	MACH_TRAP(kern_invalid, 0),		/* 43 */
	MACH_TRAP(kern_invalid, 0),		/* 44 */
	MACH_TRAP(kern_invalid, 0),		/* 45 */
	MACH_TRAP(kern_invalid, 0),		/* 46 */
	MACH_TRAP(kern_invalid, 0),		/* 47 */
	MACH_TRAP(kern_invalid, 0),		/* 48 */
	MACH_TRAP(kern_invalid, 0),		/* 49 */

#if    VCF
	MACH_TRAP(syscall_vcf_init, 2),	/* 50 */
#else  VCF
	MACH_TRAP(kern_invalid, 0),		/* 50 */
#endif VCF
	MACH_TRAP(kern_invalid, 0),		/* 51 */
	MACH_TRAP(kern_invalid, 0),		/* 52 */
	MACH_TRAP(kern_invalid, 0),		/* 53 */
	MACH_TRAP(kern_invalid, 0),		/* 54 */
#if	MACH_IPC_COMPAT
	MACH_TRAP(host_self, 0),		/* 55 */
#else	/* MACH_IPC_COMPAT */
	MACH_TRAP(null_port, 0),		/* 55 */
#endif	/* MACH_IPC_COMPAT */
	MACH_TRAP(null_port, 0),		/* 56 */
	MACH_TRAP(kern_invalid, 0),		/* 57 */
	MACH_TRAP(kern_invalid, 0),		/* 58 */
 	MACH_TRAP_STACK(swtch_pri, 1),		/* 59 */

	MACH_TRAP_STACK(swtch, 0),		/* 60 */
	MACH_TRAP_STACK(thread_switch, 3),	/* 61 */
	MACH_TRAP(kern_invalid, 0),		/* 62 */
	MACH_TRAP(kern_invalid, 0),		/* 63 */
	MACH_TRAP(syscall_vm_map, 11),			/* 64 */
	MACH_TRAP(syscall_vm_allocate, 4),		/* 65 */
	MACH_TRAP(syscall_vm_deallocate, 3),		/* 66 */
	MACH_TRAP(kern_invalid, 0),			/* 67 */
	MACH_TRAP(syscall_task_create, 3),		/* 68 */
	MACH_TRAP(syscall_task_terminate, 1),		/* 69 */

	MACH_TRAP(syscall_task_suspend, 1),		/* 70 */
	MACH_TRAP(syscall_task_set_special_port, 3),	/* 71 */
	MACH_TRAP(syscall_mach_port_allocate, 3),	/* 72 */
	MACH_TRAP(syscall_mach_port_deallocate, 2),	/* 73 */
	MACH_TRAP(syscall_mach_port_insert_right, 4),	/* 74 */
	MACH_TRAP(syscall_mach_port_allocate_name, 3),	/* 75 */
	MACH_TRAP(syscall_thread_depress_abort, 1),	/* 76 */
	MACH_TRAP(syscall_device_read, 6),		/* 77 */
	MACH_TRAP(syscall_device_read_request, 5),	/* 78 */
	MACH_TRAP(syscall_device_write, 6),		/* 79 */

	MACH_TRAP(syscall_device_write_request, 6),	/* 80 */
	MACH_TRAP(syscall_device_read_overwrite, 6),            /* 81 */
	MACH_TRAP(syscall_device_read_overwrite_request, 6),    /* 82 */

	MACH_TRAP(kern_invalid, 0),		/* 83 */
	MACH_TRAP(kern_invalid, 0),		/* 84 */
	MACH_TRAP(kern_invalid, 0),		/* 85 */
	MACH_TRAP(kern_invalid, 0),		/* 86 */
	MACH_TRAP(kern_invalid, 0),		/* 87 */
	MACH_TRAP(kern_invalid, 0),		/* 88 */
	MACH_TRAP(kern_invalid, 0),		/* 89 */

	MACH_TRAP(kern_invalid, 0),		/* 90 */
	MACH_TRAP(kern_invalid, 0),		/* 91 */
	MACH_TRAP(kern_invalid, 0),		/* 92 */
	MACH_TRAP(kern_invalid, 0),		/* 93 */
	MACH_TRAP(kern_invalid, 0),		/* 94 */
	MACH_TRAP(kern_invalid, 0),		/* 95 */
	MACH_TRAP(kern_invalid, 0),		/* 96 */
	MACH_TRAP(kern_invalid, 0),		/* 97 */
	MACH_TRAP(kern_invalid, 0),		/* 98 */
	MACH_TRAP(kern_invalid, 0),		/* 99 */

	MACH_TRAP(kern_invalid, 0),		/* 100 */
	MACH_TRAP(kern_invalid, 0),		/* 101 */
	MACH_TRAP(kern_invalid, 0),		/* 102 */
	MACH_TRAP(kern_invalid, 0),		/* 103 */
	MACH_TRAP(kern_invalid, 0),		/* 104 */
	MACH_TRAP(kern_invalid, 0),		/* 105 */
	MACH_TRAP(kern_invalid, 0),		/* 106 */
	MACH_TRAP(kern_invalid, 0),		/* 107 */
	MACH_TRAP(kern_invalid, 0),		/* 108 */
	MACH_TRAP(kern_invalid, 0),		/* 109 */

/* XXX temporary for limited backward compatibility */
#if	MCMSG && (iPSC860 || PARAGON860)
	MACH_TRAP(kern_invalid, 0),		/* 110 */
	MACH_TRAP(syscall_mcmsg_flick, 0),	/* 111 */
	MACH_TRAP(kern_invalid, 0),		/* 112 */
	MACH_TRAP(kern_invalid, 0),		/* 113 */
	MACH_TRAP(kern_invalid, 0),		/* 114 */
	MACH_TRAP(kern_invalid, 0),		/* 115 */
	MACH_TRAP(kern_invalid, 0),		/* 116 */
	MACH_TRAP(kern_invalid, 0),		/* 117 */
	MACH_TRAP(kern_invalid, 0),		/* 118 */
	MACH_TRAP(kern_invalid, 0),		/* 119 */
	MACH_TRAP(kern_invalid, 0),		/* 120 */
	MACH_TRAP(syscall_mcmsg_boot_send, 7),	/* 121 */
	MACH_TRAP(syscall_mcmsg_console_open, 2),	/* 122 */
	MACH_TRAP(syscall_mcmsg_console_close, 1),	/* 123 */
	MACH_TRAP(syscall_mcmsg_console_read, 3),	/* 124 */
	MACH_TRAP(syscall_mcmsg_console_write, 3),	/* 125 */
	MACH_TRAP(syscall_mcmsg_myphysnode, 0),		/* 126 */
	MACH_TRAP(kern_invalid, 0),		/* 127 */
#else   MCMSG && (iPSC860 || PARAGON860)
	MACH_TRAP(kern_invalid, 0),		/* 110 */
	MACH_TRAP(kern_invalid, 0),		/* 111 */
	MACH_TRAP(kern_invalid, 0),		/* 112 */
	MACH_TRAP(kern_invalid, 0),		/* 113 */
	MACH_TRAP(kern_invalid, 0),		/* 114 */
	MACH_TRAP(kern_invalid, 0),		/* 115 */
	MACH_TRAP(kern_invalid, 0),		/* 116 */
	MACH_TRAP(kern_invalid, 0),		/* 117 */
	MACH_TRAP(kern_invalid, 0),		/* 118 */
	MACH_TRAP(kern_invalid, 0),		/* 119 */
	MACH_TRAP(kern_invalid, 0),		/* 120 */
	MACH_TRAP(kern_invalid, 0),		/* 121 */
	MACH_TRAP(kern_invalid, 0),		/* 122 */
	MACH_TRAP(kern_invalid, 0),		/* 123 */
	MACH_TRAP(kern_invalid, 0),		/* 124 */
	MACH_TRAP(kern_invalid, 0),		/* 125 */
	MACH_TRAP(kern_invalid, 0),		/* 126 */
	MACH_TRAP(kern_invalid, 0),		/* 127 */
#endif	MCMSG && (iPSC860 || PARAGON860)
	MACH_TRAP(kern_invalid, 0),		/* 128 */
	MACH_TRAP(kern_invalid, 0),		/* 129 */

#if	MCMSG && (iPSC860 || PARAGON860)
	MACH_TRAP(syscall_mcmsg_interface, 12),	/* 130 */
	MACH_TRAP(syscall_mcmsg_myphysnode, 0),	/* 131 */
	MACH_TRAP(syscall_mcmsg_mp_access, 1),	/* 132 */
	MACH_TRAP(syscall_mcmsg_flick, 0),	/* 133 */
	MACH_TRAP(syscall_mcmsg_boot_send, 7),	/* 134 */
	MACH_TRAP(syscall_mcmsg_console_open, 2),	/* 135 */
	MACH_TRAP(syscall_mcmsg_console_close, 1),	/* 136 */
	MACH_TRAP(syscall_mcmsg_console_read, 3),	/* 137 */
	MACH_TRAP(syscall_mcmsg_console_write, 3),	/* 138 */
	MACH_TRAP(kern_invalid, 0),		/* 139 */
#if	NX
	MACH_TRAP(syscall_mcmsg_wire, 2),	/* 140 */
	MACH_TRAP(syscall_mcmsg_nodeinfo, 1),	/* 141 */
	MACH_TRAP(syscall_mcmsg_nxport_setup, 1),	/* 142 */
	MACH_TRAP(syscall_mcmsg_nx_local_ast, 1),	/* 143 */
	MACH_TRAP(syscall_mcmsg_nx_op, 9),		/* 144 */
	MACH_TRAP(kern_invalid, 0),		/* 145 */
	MACH_TRAP(kern_invalid, 0),		/* 146 */
	MACH_TRAP(kern_invalid, 0),		/* 147 */
	MACH_TRAP(kern_invalid, 0),		/* 148 */
	MACH_TRAP(kern_invalid, 0),		/* 149 */
#else	NX
	MACH_TRAP(kern_invalid, 0),		/* 140 */
	MACH_TRAP(kern_invalid, 0),		/* 141 */
	MACH_TRAP(kern_invalid, 0),		/* 142 */
	MACH_TRAP(kern_invalid, 0),		/* 143 */
	MACH_TRAP(kern_invalid, 0),		/* 144 */
	MACH_TRAP(kern_invalid, 0),		/* 145 */
	MACH_TRAP(kern_invalid, 0),		/* 146 */
	MACH_TRAP(kern_invalid, 0),		/* 147 */
	MACH_TRAP(kern_invalid, 0),		/* 148 */
#if	VC
	MACH_TRAP(syscall_mcmsg_vc, 9),		/* 149 */
#else	VC
	MACH_TRAP(kern_invalid, 0),		/* 149 */
#endif	VC
#endif	NX
#if	RPC
	/*
	 *	Dangerous user-visible bindings for the
	 *	RPC facility (these calls don't export all
	 *	of the RPC interfaces).  These *will*
	 *	go away.
	 */
	MACH_TRAP(syscall_rpc_alloc, 3),	/* 150 */
	MACH_TRAP(syscall_rpc_free, 1),		/* 151 */
	MACH_TRAP(syscall_rpc_recv, 3),		/* 152 */
	MACH_TRAP(syscall_rpc_reply, 2),	/* 153 */
	MACH_TRAP(syscall_rpc_send, 5),		/* 154 */
#else	RPC
	MACH_TRAP(kern_invalid, 0),		/* 150 */
	MACH_TRAP(kern_invalid, 0),		/* 151 */
	MACH_TRAP(kern_invalid, 0),		/* 152 */
	MACH_TRAP(kern_invalid, 0),		/* 153 */
	MACH_TRAP(kern_invalid, 0),		/* 154 */
#endif	RPC
#if	RDMA
	/*
	 *	Dangerous user-visible bindings for the
	 *	RDMA facility (these calls don't export all
	 *	of the RDMA interfaces).  These *will*
	 *	go away.
	 */
	MACH_TRAP(syscall_rdma_alloc, 3),	/* 155 */
	MACH_TRAP(syscall_rdma_free, 1),	/* 156 */
	MACH_TRAP(syscall_rdma_token, 1),	/* 157 */
	MACH_TRAP(syscall_rdma_accept, 1),	/* 158 */
	MACH_TRAP(syscall_rdma_connect, 2),	/* 159 */
	MACH_TRAP(syscall_rdma_send, 3),	/* 160 */
	MACH_TRAP(syscall_rdma_recv, 3),	/* 161 */
	MACH_TRAP(syscall_rdma_disconnect, 1),	/* 162 */
	MACH_TRAP(syscall_rdma_send_busy, 1),	/* 163 */
	MACH_TRAP(syscall_rdma_send_ready, 1),	/* 164 */
	MACH_TRAP(syscall_rdma_send_done, 1),	/* 165 */
	MACH_TRAP(syscall_rdma_send_complete, 1),	/* 166 */
	MACH_TRAP(syscall_rdma_recv_busy, 1),	/* 167 */
	MACH_TRAP(syscall_rdma_recv_ready, 1),	/* 168 */
	MACH_TRAP(syscall_rdma_recv_done, 1),	/* 169 */
	MACH_TRAP(syscall_rdma_recv_complete, 1),	/* 170 */
#else	RDMA
	MACH_TRAP(kern_invalid, 0),		/* 155 */
	MACH_TRAP(kern_invalid, 0),		/* 156 */
	MACH_TRAP(kern_invalid, 0),		/* 157 */
	MACH_TRAP(kern_invalid, 0),		/* 158 */
	MACH_TRAP(kern_invalid, 0),		/* 159 */
	MACH_TRAP(kern_invalid, 0),		/* 160 */
	MACH_TRAP(kern_invalid, 0),		/* 161 */
	MACH_TRAP(kern_invalid, 0),		/* 162 */
	MACH_TRAP(kern_invalid, 0),		/* 163 */
	MACH_TRAP(kern_invalid, 0),		/* 164 */
	MACH_TRAP(kern_invalid, 0),		/* 165 */
	MACH_TRAP(kern_invalid, 0),		/* 166 */
	MACH_TRAP(kern_invalid, 0),		/* 167 */
	MACH_TRAP(kern_invalid, 0),		/* 168 */
	MACH_TRAP(kern_invalid, 0),		/* 169 */
	MACH_TRAP(kern_invalid, 0),		/* 170 */
#endif	RDMA
#endif	MCMSG && (iPSC860 || PARAGON860)

};

int	mach_trap_count = (sizeof(mach_trap_table) / sizeof(mach_trap_table[0]));
