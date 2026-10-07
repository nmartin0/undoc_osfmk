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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_ext.h,v 1.34 1994/11/18 20:44:08 mtm Exp $
 */

/*
 * mcmsg_ext.h
 *
 * External declarations
 */

#ifndef MCMSG_EXT_H
#define MCMSG_EXT_H

typedef	unsigned long	user_pointer_t;


#include "cpus.h"
#include "mach_assert.h"
#include "mach_kdb.h"

#include <kern/assert.h>
#include <norma/ipc_net.h>
#include <kern/mach_param.h>
#include <kern/zalloc.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <ipsc/dcmcom.h>
#include <i860paragon/mcmsg/mcmsg_config.h>
#include <i860paragon/mcmsg/mcmsg_trace.h>
#include <i860paragon/mcmsg/mcmsg_err.h>

#if	NX
#include <i860paragon/mcmsg/mcmsg_appl.h>
#include <i860paragon/mcmsg/mcmsg_intf.h>
#include <i860paragon/mcmsg/mcmsg_xmsg.h>
#include <i860paragon/mcmsg/mcmsg_select.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_task.h>
#include <i860paragon/mcmsg/mcmsg_malloc.h>
#else	NX
typedef struct mcmsg_task { int	t; }	mcmsg_task_t;
#endif	NX

#ifndef MCMSG_EXT
#define MCMSG_EXT extern
#endif  MCMSG_EXT

void	mcmsg_disable_tx_interrupts();
void	mcmsg_enable_tx_interrupts();
void	mcmsg_disable_rx_interrupts();
void	mcmsg_enable_rx_interrupts();

/*
 * Configuration
 */

extern int (*mcmsg_init_switch[])();
extern int (*mcmsg_inq_switch[])();
extern int (*mcmsg_post_switch[])();
extern int (*mcmsg_user_switch[])();
extern int (*mcmsg_send_switch[])();
extern int (*mcmsg_recv_switch[])();

/*
 * General
 */

#if	BIGPKTS && BUMPERS
/* Big Packets + Bumpers Not supported in ASM yet */
#define HANDCODE	0
#else	BIGPKTS && BUMPERS
#define	HANDCODE	!MACH_ASSERT
#endif	BIGPKTS && BUMPERS

#if    BIGPKTS
#define MAX_PKT_SIZE	8192
#else  BIGPKTS
#define MAX_PKT_SIZE	1984
#endif BIGPKTS

#define MIN_PKT_SIZE	32
#define PKT_GRAN	8

MCMSG_EXT int		mcmsg_send_tail_limit;

#if NX
MCMSG_EXT select_t	mcmsg_local_sel;
MCMSG_EXT select_t	mcmsg_seq_sel;
MCMSG_EXT select_t	mcmsg_app_sel;
MCMSG_EXT node_list_t *mcmsg_node_list;

select_item_t *mcmsg_install_remote();
#if    MACH_ASSERT
select_item_t *mcmsg_lookup_remote();
#else  MACH_ASSERT
/* see mcmsg_select.h */
#endif MACH_ASSERT
select_item_t *mcmsg_lookup_sequence();

/*
 * Kernel Memory
 */

#ifdef old
MCMSG_EXT unsigned long		mcmsg_memory[MMSIZE/sizeof(long)];
MCMSG_EXT unsigned long		*mcmsg_memlist[LOG2MMSIZE + 1];
#else
#define MAXLOG2MMSIZE	32
MCMSG_EXT unsigned long		*mcmsg_memory;
MCMSG_EXT unsigned long		*mcmsg_memlist[MAXLOG2MMSIZE];
#endif
#endif NX

/*
 * Counters
 */

MCMSG_EXT unsigned long		mcmsg_msg_drop;
MCMSG_EXT unsigned long		mcmsg_ipc_drop;


/*
 * Send
 */

MCMSG_EXT unsigned short mcmsg_send_sequence;

MCMSG_EXT unsigned long	mcmsg_send_tail_count;


/*
 * Buffer management
 */

#if NX
user_pointer_t	mcmsg_alloc_whole_xmsg();
#endif NX


/*
 * Physical node for Paragon and Hypercube
 */

MCMSG_EXT int		ipsc_physnode;

/*
 * Paragon
 */

#define spldcm		splmsg
MCMSG_EXT int		paragon_first_node;
MCMSG_EXT int		paragon_mesh_x;
MCMSG_EXT int		paragon_mesh_y;
MCMSG_EXT int		mcmsg_mp_enable;	/* Message processor */

/*
 * Routing table
 */
MCMSG_EXT int		*routing_table;

#if MACH_ASSERT

MCMSG_EXT int		routing_table_nodes;

#define calculate_route(physnode) 				\
	(							\
	(physnode) < routing_table_nodes				\
		? routing_table[physnode]			\
		: panic("calculate_route")			\
	)
#else /* MACH_ASSERT */

#define calculate_route(physnode) routing_table[physnode]

#endif /* MACH_ASSERT */

/*
 * Debug
 */

#if	MACH_ASSERT
MCMSG_EXT int		mcmsg_reentry;
#endif	MACH_ASSERT
#define	CHECK_REENTRY()

#if	NX

#define mcmsg_validate_read1(a, n, dirbase) \
 mcmsg_validate_rw1((a), (n), (dirbase), INTEL_PTE_VALID)
#define mcmsg_validate_write1(a, n, dirbase) \
 mcmsg_validate_rw1((a), (n), (dirbase), (INTEL_PTE_VALID | INTEL_PTE_WRITE))

#if	MACH_ASSERT

#define mcmsg_validate_long(m,x) \
	mcmsg_validate_long_real((x), (m)->dirbase,  __FILE__, __LINE__)
#define mcmsg_validate_line(m,x) \
	mcmsg_validate_line_real((x), (m)->dirbase,  __FILE__, __LINE__)

#else	MACH_ASSERT

#define mcmsg_validate_long(m,x) \
 (((unsigned long)(x) & 0x3) != 0? 0 : mcmsg_validate_real((x), (m)->dirbase))
#define mcmsg_validate_line(m,x) \
 (((unsigned long)(x) & 0x1f) != 0? 0 : mcmsg_validate_real((x), (m)->dirbase))

#endif	MACH_ASSERT
#endif	NX

#endif MCMSG_EXT_H
