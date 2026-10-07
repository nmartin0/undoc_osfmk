// 
// $Copyright
// Copyright 1993, 1994 , 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/*
 * (c) Copyright 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * HISTORY
 * $Log: second_traps.s,v $
// Revision 1.5  1994/11/18  20:49:30  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/07/14  18:44:01  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.1.1.3  1993/07/01  21:05:40  cfj
// Adding new code from vendor
 *
// Revision 1.3  1993/05/06  19:32:39  nandy
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.1  1993/05/03  17:53:27  cfj
// Initial 1.0.3 code drop
//
// Revision 1.1.2.1  1992/11/05  23:44:38  dleslie
// Local changes for NX through noon, November 5, 1992.
//
 * Revision 4.1  1992/11/04  00:55:18  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/03/09  13:43:48  durriya
 * Revision 2.5  1993/04/15  22:47:34  condict
 * 	To support server profiling, define versions of all Mach traps that
 * 	do not call mcount (_noprof suffix).
 *
 * Revision 2.4  93/01/07  11:15:56  condict
 * 	Remove undef GPROF, to allow profiling version to be compiled.
 * 	[1992/06/04  14:19:00  condict]
 * 
 * Revision 2.3  92/03/09  13:43:48  durriya
 * 	[Revision 1.3  92/02/12  10:58:44  bernadat]
 * 	Do not profile mach kernel calls generated in second_trap.s
 * 
 * Revision 2.2  91/08/31  14:27:49  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 1.2  91/05/24  11:37:41  jose
 * Prefixed mach syscalls with second_ .
 *
 * Revision 1.1  91/03/13  15:06:56  condict
 * Initial revision
 * 
 */

 /* This file defines functions for Mach traps that exist in the standard
  * Mach kernel, but are not defined in the standard version of libmach_sa.a
  * (see the file mach/syscall_sw.h).  It is a replacement for (and
  * satisfies all the references that are defined in) the module
  * mk/src/.../user/libmach/mach_traps.cs of libmach_sa.a.  We include this
  * in the server to avoid having to modify the source to libmach, which is
  * under the mk source tree and not under our control.
 */

/* Task_by_pid is needed by the second server to initialize the
 * privileged host port and the device server port (see bsd/mach_init.c).
 * Undefining STANDALONE causes task_by_pid and several other traps to be
 * defined by mach/syscall_sw.h.  We need to change the names of the
 * trap functions so that they won't interfere with the functions of the
 * same name that implement them inside the server:
 */
#undef STANDALONE

#define _task_by_pid	  _second_task_by_pid
#define _pid_by_task	  _second_pid_by_task
#define _init_process	  _second_init_process
#define _map_fd		  _second_map_fd
#define _rfs_make_symlink _second_rfs_make_symlink
#define _htg_syscall	  _second_htg_syscall

#include <mach/syscall_sw.h>

/*
 * When compiling with profiling, the subr_mcount module may need
 * non-profiling versions of any or all Mach traps, so:
 */
#if	GPROF || PROF

/* We already defined these above, so avoid warnings: */
#undef _task_by_pid
#undef _pid_by_task
#undef _init_process
#undef _map_fd
#undef _rfs_make_symlink
#undef _htg_syscall

#define _evc_wait	_evc_wait_noprof
#define _mach_msg_trap	_mach_msg_trap_noprof
#define _mach_reply_port	_mach_reply_port_noprof
#define _mach_thread_self	_mach_thread_self_noprof
#define _mach_task_self	_mach_task_self_noprof
#define _mach_host_self	_mach_host_self_noprof
#define _swtch_pri	_swtch_pri_noprof
#define _swtch	_swtch_noprof
#define _thread_switch	_thread_switch_noprof
#define _syscall_device_read	_syscall_device_read_noprof
#define _syscall_device_read_request	_syscall_device_read_request_noprof
#define _syscall_device_write	_syscall_device_write_noprof
#define _syscall_device_write_request	_syscall_device_write_request_noprof
#define _syscall_vm_map	_syscall_vm_map_noprof
#define _syscall_vm_allocate	_syscall_vm_allocate_noprof
#define _syscall_vm_deallocate	_syscall_vm_deallocate_noprof
#define _syscall_task_create	_syscall_task_create_noprof
#define _syscall_task_terminate	_syscall_task_terminate_noprof
#define _syscall_task_suspend	_syscall_task_suspend_noprof
#define _syscall_task_set_special_port	_syscall_task_set_special_port_noprof
#define _syscall_mach_port_allocate	_syscall_mach_port_allocate_noprof
#define _syscall_mach_port_deallocate	_syscall_mach_port_deallocate_noprof
#define _syscall_mach_port_insert_right	_syscall_mach_port_insert_right_noprof
#define _syscall_mach_port_allocate_name	_syscall_mach_port_allocate_name_noprof
#define _syscall_thread_depress_abort	_syscall_thread_depress_abort_noprof
#define _task_by_pid	_task_by_pid_noprof
#define _pid_by_task	_pid_by_task_noprof
#define _init_process	_init_process_noprof
#define _map_fd	_map_fd_noprof
#define _rfs_make_symlink	_rfs_make_symlink_noprof
#define _htg_syscall	_htg_syscall_noprof
#define _task_self	_task_self_noprof
#define _thread_reply	_thread_reply_noprof
#define _task_notify	_task_notify_noprof
#define _thread_self	_thread_self_noprof
#define _msg_send_trap	_msg_send_trap_noprof
#define _msg_receive_trap	_msg_receive_trap_noprof
#define _msg_rpc_trap	_msg_rpc_trap_noprof
#define _host_self	_host_self_noprof

/* Include syscall_sw.h again, this time with profiling turned off: */
#undef	PROF
#undef	GPROF
/* Need to undef these in order to force the file to be included again: */
#undef	_MACH_SYSCALL_SW_H_
					/* Hack: have to have one of these
					 * for each architecture:
					 */
#undef	_MACH_I386_SYSCALL_SW_H_	/* XXX */
#undef	_MACH_I860_SYSCALL_SW_H_	/* XXX */
					/* Hack: have to know all the symbols
					 * defined in asm.h:
					 */
#undef	MCOUNT				/* XXX */
#undef	ENTRY				/* XXX */
#include <mach/syscall_sw.h>

#endif	GPROF || PROF
