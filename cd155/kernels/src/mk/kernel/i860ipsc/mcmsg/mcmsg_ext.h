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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_ext.h,v 1.18 1994/11/18 20:41:18 mtm Exp $
 */

/*
 * mcmsg_ext.h
 *
 * External declarations
 */

#ifndef MCMSG_EXT_H
#define MCMSG_EXT_H

#if 1
#include <kern/assert.h>
#include <norma/ipc_net.h>
#else
#include <norma/ipc_net.h>
#undef	MACH_ASSERT
#undef	assert
#undef	assert_static
#define MACH_ASSERT 0
#define assert(ex)
#define assert_static(ex)
#endif

#include <kern/mach_param.h>
#include <kern/zalloc.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <ipsc/dcmcom.h>
#include <i860ipsc/mcmsg/mcmsg_trace.h>
#include <i860ipsc/mcmsg/mcmsg_control.h>
#include <i860ipsc/mcmsg/mcmsg_err.h>
#include <i860ipsc/mcmsg/mcmsg_appl.h>
#include <i860ipsc/mcmsg/mcmsg_send.h>
#include <i860ipsc/mcmsg/mcmsg_recv.h>
#include <i860ipsc/mcmsg/mcmsg_intf.h>
#include <i860ipsc/mcmsg/mcmsg_xmsg.h>
#include <i860ipsc/mcmsg/mcmsg_select.h>
#include <i860ipsc/mcmsg/mcmsg_task.h>
#include <i860ipsc/mcmsg/mcmsg_ipc.h>
#include <i860ipsc/mcmsg/mcmsg_malloc.h>

#ifndef MCMSG_EXT
#define MCMSG_EXT extern
#endif  MCMSG_EXT

void	mcmsg_disable_tx_interrupts();
void	mcmsg_enable_tx_interrupts();
void	mcmsg_disable_rx_interrupts();
void	mcmsg_enable_rx_interrupts();

/*
 * General
 */

MCMSG_EXT select_t	mcmsg_pid_sel;
MCMSG_EXT select_t	mcmsg_app_sel;
MCMSG_EXT mcmsg_task_t	*mcmsg_task;
MCMSG_EXT int		mcmsg_phys;
MCMSG_EXT int		mcmsg_flush;
MCMSG_EXT node_list_t *mcmsg_node_list;

MCMSG_EXT int		boot_hang_time;
extern int		hang_time;

select_item_t *mcmsg_install_remote();
select_item_t *mcmsg_lookup_remote();
select_item_t *mcmsg_lookup_sequence();

/*
 * Kernel Memory
 */

MCMSG_EXT unsigned long		mcmsg_memory[MMSIZE/sizeof(long)];
MCMSG_EXT unsigned long		*mcmsg_memlist[LOG2MMSIZE + 1];

/*
 * Counters
 */

MCMSG_EXT unsigned long		mcmsg_msg_drop;
MCMSG_EXT unsigned long		mcmsg_ipc_drop;


/*
 * Send
 */

MCMSG_EXT unsigned short mcmsg_send_sequence;

#define MAXSEND	256
MCMSG_EXT struct {
	int		method;
	void		*item;
	unsigned long	sequence;
	mcmsg_task_t	*mcmsg_task;
} mcmsg_send_store[MAXSEND];
MCMSG_EXT unsigned char mcmsg_send_store_in;
MCMSG_EXT unsigned char mcmsg_send_store_out;


/*
 * Buffer management
 */

xmsg_t * mcmsg_alloc_whole_xmsg();


/*
 * For Mach IPC
 */

#define TRACE_IPC	PARAGON860

MCMSG_EXT int		ipcreq_receiving;
MCMSG_EXT ipcreq_t	*ipcreq_send_head;
MCMSG_EXT ipcreq_t	*ipcreq_send_tail;
MCMSG_EXT ipcreq_t	*ipcreq_recv_head;
MCMSG_EXT ipcreq_t	*ipcreq_recv_tail;
MCMSG_EXT ipcreq_t	*ipcreq_free_list;

MCMSG_EXT ipcreq_t	ipcreq[MAXIPCREQ];

MCMSG_EXT int		ipcreq_recv_count;	/* Outstanding IPC req's */
MCMSG_EXT int		ipcreq_send_count;
MCMSG_EXT int		ipcreq_send_max;

/*
 * Physical node for Paragon and Hypercube
 */

MCMSG_EXT int		ipsc_physnode;

/*
 * Paragon
 */

#if	iPSC860
#else	iPSC860

#define spldcm		splmsg
MCMSG_EXT int		paragon_first_node;
MCMSG_EXT int		paragon_mesh_x;
MCMSG_EXT int		paragon_mesh_y;

#endif	iPSC860

/*
 * Debug
 */

#if	MACH_ASSERT

#define mcmsg_validate(x) \
	mcmsg_validate_real((x), mcmsg_task->dirbase,  __FILE__, __LINE__)
#define mcmsg_validate_long(x) \
	mcmsg_validate_long_real((x), mcmsg_task->dirbase,  __FILE__, __LINE__)
#define mcmsg_validate_line(x) \
	mcmsg_validate_line_real((x), mcmsg_task->dirbase,  __FILE__, __LINE__)

MCMSG_EXT int		mcmsg_reentry;

#else	MACH_ASSERT

#define mcmsg_validate(x) \
	mcmsg_validate_real((x), mcmsg_task->dirbase)
#define mcmsg_validate_long(x) \
 (((unsigned long)(x) & 0x3) != 0? 0 : mcmsg_validate_real((x), mcmsg_task->dirbase))
#define mcmsg_validate_line(x) \
 (((unsigned long)(x) & 0x1f) != 0? 0 : mcmsg_validate_real((x), mcmsg_task->dirbase))

#endif	MACH_ASSERT

#endif MCMSG_EXT_H
