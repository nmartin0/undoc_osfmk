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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * ipc_internal.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/ipc_internal.h,v 1.2 1994/11/19 03:11:05 mtm Exp $
 *
 */

/*
 * Macros internal to the ipc module.
 */


#ifndef	_IPC_INTERNAL_
#define	_IPC_INTERNAL_

#include	"ipc.h"
#include	"mem.h"

/*
 * Macros to test ipc/rpc status.
 */
#define	awaiting_local_reply(pr)						\
	((pr->portrec_reply_ipcrec != 0) &&					\
		(((ipc_rec_ptr_t)pr->portrec_reply_ipcrec)->status ==		\
							IPC_REC_REPLY))

#define	awaiting_remote_reply(pr)						\
	((pr->portrec_reply_ipcrec != 0)					\
		&&								\
	(((ipc_rec_ptr_t)pr->portrec_reply_ipcrec)->type == IPC_REC_TYPE_CLIENT)\
		&&								\
	(((ipc_rec_ptr_t)pr->portrec_reply_ipcrec)->status == IPC_REC_ACTIVE))

/*
 * Macro to clean up the resources used for processing an incoming message.
 */
#define	ipc_in_gc(ipc_ptr) {							\
	switch ((ipc_ptr)->in.assem_type) {					\
		case IPC_REC_ASSEM_PKT:						\
			break;							\
		case IPC_REC_ASSEM_OBJ:						\
			MEM_DEALLOCOBJ(((ipc_ptr)->in.assem_buff),	\
				MEM_ASSEMBUFF);					\
			break;							\
		case IPC_REC_ASSEM_MEMALLOC:					\
			MEM_DEALLOC((pointer_t)((ipc_ptr)->in.assem_buff),	\
						(ipc_ptr)->in.assem_len);	\
			break;							\
		default:							\
			ERROR((msg,						\
				"ipc_in_gc: unknown type for assembly area: %d",\
				(ipc_ptr)->in.assem_type));			\
			break;							\
	}									\
}


/*
 * Extern declarations for this module only.
 */
extern void	ipc_inmsg();
extern void	ipc_outmsg();
extern void	ipc_client_abort();
extern void	ipc_server_abort();

/*
 * Memory management for MEM_IPCBUFF.
 */
extern mem_objrec_t		MEM_IPCBUFF;


#endif	_IPC_INTERNAL_
