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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log: syscall_subr.h,v $
 * Revision 1.10  1994/11/18  20:52:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/07/12  19:23:38  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.8  1994/05/18  00:07:16  regnier
 *  Reviewer: None.
 *  Risk: Low
 *  Benefit: Hook for large packets.
 *  Testing: Message Passing Eats, developer tests.
 *  Module(s): syscall_subr.h, syscall_sw.c, i860_paragon/mcmsg/mcmsg_init.c
 *
 * Revision 1.7.4.3  1994/03/02  18:43:23  andyp
 * Added additional user-mode bindings for RDMA testing.
 *
 * Revision 1.7.4.2  1994/02/07  20:19:26  andyp
 * Rounded out RDMA interfaces and added user-mode bindings for testing.
 *
 * Revision 1.7.4.1  1994/02/04  07:53:25  andyp
 * Added machine-specific RPC and RDMA intialization calls to
 * startup.c (see i860paragon/model_dep.c).  Added temporary
 * user-mode test points for RPC.
 *
 * Revision 1.7  1993/09/29  23:08:04  prp
 * Merge in R1.2 branch message passing.
 *
 * Revision 1.2.2.3.2.1  1993/08/26  01:05:40  prp
 * Add VC message passing module
 *
 * Revision 1.6  1993/06/30  22:45:50  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/05/23  00:19:47  prp
 * NX Configurability. Clean NX system calls and move them up.
 *
 * Revision 1.4  1993/04/27  20:36:50  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:37:38  dleslie
 * First R1_0 release
 *
 * Revision 1.1  1993/04/02  23:16:07  regnier
 * Initial revision
 *
 * Revision 2.5.1.1  92/05/27  00:46:11  jeffreyh
 * 	Added decls for MCMSG system calls
 * 	[regnier@ssd.intel.com]
 * 
 * Revision 2.5  91/05/18  14:33:56  rpd
 * 	Added thread_depress_timeout.
 * 	[91/03/31            rpd]
 * 
 * Revision 2.4  91/05/14  16:47:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:29:40  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:18:24  mrt]
 * 
 * Revision 2.2  90/06/02  14:56:22  rpd
 * 	Created.
 * 	[90/03/26  23:52:40  rpd]
 * 
 * Revision 2.5  89/10/11  14:27:28  dlb
 * 	Add thread_switch, remove kern_timestamp.
 * 	[89/09/01  17:43:07  dlb]
 * 
 * Revision 2.4  89/10/10  10:54:34  mwyoung
 * 	Add a new call to create an RFS link, a special form of
 * 	symbolic link that may contain null characters in the target.
 * 	[89/10/01            mwyoung]
 * 
 * Revision 2.3  89/05/01  17:28:32  rpd
 * 	Removed the ctimes() declaration; it's history.
 * 
 * Revision 2.2  89/05/01  17:01:50  rpd
 * 	Created.
 * 	[89/05/01  14:00:51  rpd]
 * 
 */

#ifndef	_KERN_SYSCALL_SUBR_H_
#define _KERN_SYSCALL_SUBR_H_

extern int	swtch();
extern int	swtch_pri();
extern int	thread_switch();
extern void	thread_depress_timeout();

#ifdef MCMSG
extern int	syscall_mcmsg_wire();
extern int	syscall_mcmsg_interface();
extern int	syscall_mcmsg_mp_access();
extern int	syscall_mcmsg_flick();
extern int	syscall_mcmsg_nodeinfo();
extern int	syscall_mcmsg_boot_send();
extern int	syscall_mcmsg_console_open();
extern int	syscall_mcmsg_console_close();
extern int	syscall_mcmsg_console_read();
extern int	syscall_mcmsg_console_write();
extern int	syscall_mcmsg_myphysnode();
extern int	syscall_mcmsg_nxport_setup();
extern int	syscall_mcmsg_nx_local_ast();
extern int	syscall_mcmsg_nx_op();
#if	VC
extern int	syscall_mcmsg_vc();
#endif	VC

#if	RPC
extern int	syscall_rpc_alloc();	/* RPC test point */
extern int	syscall_rpc_free();	/* RPC test point */
extern int	syscall_rpc_recv();	/* RPC test point */
extern int	syscall_rpc_reply();	/* RPC test point */
extern int	syscall_rpc_send();	/* RPC test point */
#endif	RPC

#if	RDMA
extern int	syscall_rdma_alloc();	/* RDMA test point */
extern int	syscall_rdma_free();	/* RDMA test point */
extern int	syscall_rdma_token();	/* RDMA test point */
extern int	syscall_rdma_accept();	/* RDMA test point */
extern int	syscall_rdma_connect();	/* RDMA test point */
extern int	syscall_rdma_send();	/* RDMA test point */
extern int	syscall_rdma_recv();	/* RDMA test point */
extern int	syscall_rdma_disconnect();	/* RDMA test point */
extern int	syscall_rdma_send_busy();	/* RDMA test point */
extern int	syscall_rdma_send_ready();	/* RDMA test point */
extern int	syscall_rdma_send_done();	/* RDMA test point */
extern int	syscall_rdma_send_complete();	/* RDMA test point */
extern int	syscall_rdma_recv_busy();	/* RDMA test point */
extern int	syscall_rdma_recv_ready();	/* RDMA test point */
extern int	syscall_rdma_recv_done();	/* RDMA test point */
extern int	syscall_rdma_recv_complete();	/* RDMA test point */
#endif	RDMA


#endif MCMSG

#endif	_KERN_SYSCALL_SUBR_H_
