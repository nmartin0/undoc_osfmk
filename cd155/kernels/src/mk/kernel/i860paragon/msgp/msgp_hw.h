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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hw.h,v 1.8 1994/11/18 20:46:43 mtm Exp $
 */

/*
 * msgp_hw.c
 *
 * Hardware support routines for multicomputer message passing
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>


#if	MACH_ASSERT

#define	DATA_COUNT(x,n)	x += n

unsigned long	mcmsg_data_flush;
unsigned long	mcmsg_data_in_fifo;
unsigned long	mcmsg_data_in_ltu;
unsigned long	mcmsg_data_out_fifo;
unsigned long	mcmsg_data_out_ltu;

#else	MACH_ASSERT

#define	DATA_COUNT(x,n)

#endif	MACH_ASSERT

/*
 * FIFO Alignment.
 */

#define FIFO_SIZE	2048
#define FIFO_ALIGN	8
#define FIFO_MASK	(~(FIFO_ALIGN-1))

/*
 *	FIFO Tracing.
 */
#define TRACE_FIFO 0
#if TRACE_FIFO
#define MCMSG_TRACE_FIFO(s,n,a,b,c,d) mcmsg_trace_debug(s,n,a,b,c,d)
#else TRACE_FIFO
#define MCMSG_TRACE_FIFO(s,n,a,b,c,d)
#endif TRACE_FIFO

/*
 * Last resort buffer for badly aligned messages
 */

long mcmsg_pbuf_space[3*MSG_PAGE_SIZE/sizeof(long)];
long *mcmsg_pbuf_in;
long *mcmsg_pbuf_out;

#include <i860paragon/dp.h>
#include <i860paragon/ltu.h>

nic_reg		mcmsg_nic_control;
unsigned long	mcmsg_nic_config;

unsigned long	mcmsg_recv_count;

#if	BUMPERS

#define	BUMPER	4
#define NIC_STAT_RX_LOW	NIC_STAT_RX_FIFO_NOT_EMPTY		

#endif	BUMPERS


#define NIC_STAT_ERRORS	       (NIC_STAT_NET_CRC0_ERR    | \
				NIC_STAT_NET_CRC1_ERR    | \
				NIC_STAT_PORT_PAR0_ERR   | \
				NIC_STAT_PORT_PAR1_ERR   | \
				NIC_STAT_PORT_PAR2_ERR   | \
				NIC_STAT_PORT_PAR3_ERR   | \
				NIC_STAT_PORT_PAR4_ERR   | \
				NIC_STAT_PORT_PAR5_ERR   | \
				NIC_STAT_PORT_PAR6_ERR   | \
				NIC_STAT_PORT_PAR7_ERR   | \
				NIC_STAT_NET_PAR0_ERR    | \
				NIC_STAT_NET_PAR1_ERR    | \
				NIC_STAT_TX_FIFO_OVERRUN | \
				NIC_STAT_RX_FIFO_OVERRUN | \
				NIC_STAT_RX_FIFO_UNDERRUN)

/* Assume control bits in same position as status bits */
#define NIC_CNTRL_RX_LOW	NIC_STAT_RX_LOW
#define NIC_CNTRL_ERRORS	NIC_STAT_ERRORS

struct mcmsg_hw {
	unsigned long	recv_status;
	unsigned long	send_int_enable;
	nic_reg		recv_enable;
	nic_reg		send_enable;
	nic_reg		bump_hdr;
	nic_reg		tx_stop;
	nic_reg		tx_go;
	unsigned long	send_loop;
	unsigned long 	globtime;
} mcmsg_hw;

int	mcmsg_ltu_node;
int	mcmsg_ltu_enable;

double mcmsg_nic_status();

#if    BIGPKTS
/*
 *	Global variables for concurrent send/recv
 *	mcmsg_send_in_progress:	boolean
 *	mcmsg_send_addr_remain:	address of send in progress
 *	mcmsg_send_count_remain: count of send in progress
 *
 *	mcmsg_send_waiting: boolean
 *	mcmsg_recv_waiting: boolean
 *
 */
extern int           mcmsg_send_in_progress;
extern int           mcmsg_send_count_remain;
extern unsigned long mcmsg_send_addr_remain;

extern int           mcmsg_send_waiting;
extern int           mcmsg_recv_waiting;
#endif BIGPKTS
