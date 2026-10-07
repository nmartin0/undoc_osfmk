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
 * 03-Sep-92  David Bernard (bernard) at Open Software Foundation
 *	Added syscall_device_read[_request], syscall_device_write[_request].
 *
 * $Log: syscall_sw.h,v $
 * Revision 1.8  1994/11/18  20:54:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/07/12  19:24:17  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6.8.3  1994/03/02  18:43:43  andyp
 * Added additional user-mode bindings for RDMA testing.
 *
 * Revision 1.6.8.2  1994/02/07  20:19:48  andyp
 * Rounded out RDMA interfaces and added user-mode bindings for testing.
 *
 * Revision 1.6.8.1  1994/02/04  07:53:59  andyp
 * Added temporary user-mode test points for RPC.
 *
 * Revision 1.6  1993/06/30  22:47:47  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/06/09  01:38:59  terry
 * source sync with OSF
 *
 * Revision 1.4  1993/04/27  20:39:08  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:40:22  dleslie
 * First R1_0 release
 *
 * Revision 2.8.3.2  92/03/28  10:11:14  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:21:28  jeffreyh]
 * 
 * Revision 2.10  92/02/19  16:07:22  elf
 * 	Removed STANDALONE conditionals.
 * 	[92/02/19            elf]
 * 
 * 	Added syscall_thread_depress_abort.
 * 	[92/01/20            rwd]
 * 
 * Revision 2.9  92/01/15  13:44:35  rpd
 * 	Changed MACH_IPC_COMPAT conditionals to default to not present.
 * 
 * Revision 2.8  91/12/13  14:55:06  jsb
 * 	Added evc_wait.
 * 	[91/11/02  17:44:54  af]
 * 
 * Revision 2.7  91/05/14  17:00:32  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:36:20  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:21:07  mrt]
 * 
 * Revision 2.5  90/09/09  14:33:12  rpd
 * 	Added mach_port_allocate_name trap.
 * 	[90/09/05            rwd]
 * 
 * Revision 2.4  90/06/19  23:00:14  rpd
 * 	Added pid_by_task.
 * 	[90/06/14            rpd]
 * 
 * 	Added mach_port_allocate, mach_port_deallocate, mach_port_insert_right.
 * 	[90/06/02            rpd]
 * 
 * Revision 2.3  90/06/02  14:59:58  rpd
 * 	Removed syscall_vm_allocate_with_pager.
 * 	[90/05/31            rpd]
 * 
 * 	Added map_fd, rfs_make_symlink.
 * 	[90/04/04            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  22:39:28  rpd]
 * 
 * Revision 2.2  90/05/29  18:36:57  rwd
 * 	New vm/task/threads calls from rfr.
 * 	[90/04/20            rwd]
 * 
 * Revision 2.1  89/08/03  16:03:28  rwd
 * Created.
 * 
 */

#ifndef	_MACH_SYSCALL_SW_H_
#define _MACH_SYSCALL_SW_H_

/*
 *	The machine-dependent "syscall_sw.h" file should
 *	define a macro for
 *		kernel_trap(trap_name, trap_number, arg_count)
 *	which will expand into assembly code for the
 *	trap.
 *
 *	N.B.: When adding calls, do not put spaces in the macros.
 */

#include <mach/machine/syscall_sw.h>

/*
 *	These trap numbers should be taken from the
 *	table in <kern/syscall_sw.c>.
 */

kernel_trap(evc_wait,-17,1)
kernel_trap(mach_msg_trap,-25,7)
kernel_trap(mach_reply_port,-26,0)
kernel_trap(mach_thread_self,-27,0)
kernel_trap(mach_task_self,-28,0)
kernel_trap(mach_host_self,-29,0)

kernel_trap(swtch_pri,-59,1)
kernel_trap(swtch,-60,0)
kernel_trap(thread_switch,-61,3)

/*
 *	These are syscall versions of Mach kernel calls.
 *	They only work on local tasks.
 */

kernel_trap(syscall_vm_map,-64,11)
kernel_trap(syscall_vm_allocate,-65,4)
kernel_trap(syscall_vm_deallocate,-66,3)

kernel_trap(syscall_task_create,-68,3)
kernel_trap(syscall_task_terminate,-69,1)
kernel_trap(syscall_task_suspend,-70,1)
kernel_trap(syscall_task_set_special_port,-71,3)

kernel_trap(syscall_mach_port_allocate,-72,3)
kernel_trap(syscall_mach_port_deallocate,-73,2)
kernel_trap(syscall_mach_port_insert_right,-74,4)
kernel_trap(syscall_mach_port_allocate_name,-75,3)
kernel_trap(syscall_thread_depress_abort,-76,1)
kernel_trap(syscall_device_read,-77,6)
kernel_trap(syscall_device_read_request,-78,5)
kernel_trap(syscall_device_write,-79,6)
kernel_trap(syscall_device_write_request,-80,6)
kernel_trap(syscall_device_read_overwrite,-81,6)
kernel_trap(syscall_device_read_overwrite_request,-82,6)

/*
 *	These "Mach" traps are not implemented by the kernel;
 *	the emulation library and Unix server implement them.
 *	But they are traditionally part of libmach, and use
 *	the Mach trap calling conventions and numbering.
 */

kernel_trap(task_by_pid,-33,1)
kernel_trap(pid_by_task,-34,4)
kernel_trap(init_process,-41,0)
kernel_trap(map_fd,-43,5)
kernel_trap(rfs_make_symlink,-44,3)
kernel_trap(htg_syscall,-52,3)

/*
 *	dangerous RPC test traps (that *will* go away)
 */
kernel_trap(syscall_rpc_alloc,-150,3)
kernel_trap(syscall_rpc_free,-151,1)
kernel_trap(syscall_rpc_recv,-152,3)
kernel_trap(syscall_rpc_reply,-153,2)
kernel_trap(syscall_rpc_send,-154,5)

/*
 *	dangerous RDMA test traps (that *will* go away)
 */
kernel_trap(syscall_rdma_alloc,-155,3)
kernel_trap(syscall_rdma_free,-156,1)
kernel_trap(syscall_rdma_token,-157,1)
kernel_trap(syscall_rdma_accept,-158,1)
kernel_trap(syscall_rdma_connect,-159,2)
kernel_trap(syscall_rdma_send,-160,3)
kernel_trap(syscall_rdma_recv,-161,3)
kernel_trap(syscall_rdma_disconnect,-162,1)
kernel_trap(syscall_rdma_send_busy,-163, 1)
kernel_trap(syscall_rdma_send_ready,-164, 1)
kernel_trap(syscall_rdma_send_done,-165, 1)
kernel_trap(syscall_rdma_send_complete,-166, 1)
kernel_trap(syscall_rdma_recv_busy,-167, 1)
kernel_trap(syscall_rdma_recv_ready,-168, 1)
kernel_trap(syscall_rdma_recv_done,-169, 1)
kernel_trap(syscall_rdma_recv_complete,-170, 1)

/* Traps for the old IPC interface. */

#if	MACH_IPC_COMPAT

kernel_trap(task_self,-10,0)
kernel_trap(thread_reply,-11,0)
kernel_trap(task_notify,-12,0)
kernel_trap(thread_self,-13,0)
kernel_trap(msg_send_trap,-20,4)
kernel_trap(msg_receive_trap,-21,5)
kernel_trap(msg_rpc_trap,-22,6)
kernel_trap(host_self,-55,0)

#endif	MACH_IPC_COMPAT

#endif	_MACH_SYSCALL_SW_H_
