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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_hw.h,v 1.13 1994/11/18 20:41:27 mtm Exp $
 */

/*
 * mcmsg_hw.h
 *
 * External declarations
 */

#ifndef MCMSG_HW_H
#define MCMSG_HW_H

#if	iPSC860
#include <i860ipsc/nodehw.h>
#include <i860ipsc/ctlreg.h>

#define send2(x,y) { SEND_FIFO = x; SEND_FIFO = y; }
#define send2eod(x,y) { SEND_FIFO = x; SEND_EOD = y; }
#define recv2(x,y) { x = RECV_FIFO; y = RECV_FIFO; }

#define mcmsg_send_ready()	((STATUS_REG & SEND_HALF) == 0)
extern int mcmsg_int_enables;
#define mcmsg_recv_ready()	((mcmsg_int_enables & 2) != 0 && \
				 (STATUS_REG & (RECV_HALF|EOD_IN)) != 0)
#define mcmsg_eod_last()	((STATUS_REG & EOD_LAST) != 0)
#define assert_mcmsg_eod_last() assert(mcmsg_eod_last())

#define RED_ON(x)
#define RED_OFF(x)

#else	iPSC860
#include <i860paragon/nic.h>
#include <i860paragon/lbus.h>
#include <i860paragon/led.h>

#define	BUMPERS	1
#define TRACE_MSG_DATA	0
#define NIC_TRACE	MACH_ASSERT
#define HALF_MSG_FIFO	1984

#ifndef MCMSG_HW_EXT
#define MCMSG_HW_EXT extern
#endif

#if	NIC_TRACE
#define NIC_TRACE_SIZE	1000

MCMSG_HW_EXT
struct {
	nic_reg		status;
	unsigned short	rcount;
	unsigned short	scount;
	unsigned long	place;
} mcmsg_nic_trace_buf[NIC_TRACE_SIZE];

MCMSG_HW_EXT unsigned long mcmsg_nic_trace_index;
MCMSG_HW_EXT unsigned long mcmsg_nic_trace_count;

#define NIC_TRACE_SEND mcmsg_nic_trace_buf[mcmsg_nic_trace_index].scount++
#define NIC_TRACE_RECV mcmsg_nic_trace_buf[mcmsg_nic_trace_index].rcount++

#else	NIC_TRACE

#define NIC_TRACE_SEND;
#define NIC_TRACE_RECV;

#endif	NIC_TRACE

#if	BUMPERS
MCMSG_HW_EXT unsigned long mcmsg_send_byte_count;
MCMSG_HW_EXT unsigned long mcmsg_recv_byte_count;

#define	assert_mcmsg_eod_last() if (!mcmsg_eod_last()) assert(0)

#else	BUMPERS

#define	assert_mcmsg_eod_last() assert(mcmsg_eod_last())

#endif	BUMPERS


#if	TRACE_MSG_DATA

#if	BUMPERS

#define send2(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y); \
mcmsg_trace_debug("send2   ", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
   NIC.io.full = z.full; \
   mcmsg_send_byte_count += 8; \
   NIC_TRACE_SEND; }

#define send2eod(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y);  \
mcmsg_trace_debug("send2eod", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
   mcmsg_send_byte_count += 8; \
   send2eod_(z.halfs.lo, z.halfs.hi); }

#define recv2(x,y) \
 { nic_reg z; z.full = NIC.io.full;  \
mcmsg_trace_debug("recv2   ", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
   x = z.halfs.lo; y = z.halfs.hi; \
   mcmsg_recv_byte_count += 8; \
   NIC_TRACE_RECV; }

#else	BUMPERS

#define send2(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y); \
mcmsg_trace_debug("send2   ", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
 NIC.io.full = z.full; \
   NIC_TRACE_SEND; }

#define send2eod(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y);  \
mcmsg_trace_debug("send2eod", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
   EOD.io.full = z.full; \
   NIC_TRACE_SEND; }

#define recv2(x,y) \
 { nic_reg z; z.full = NIC.io.full;  \
mcmsg_trace_debug("recv2   ", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
   x = z.halfs.lo; y = z.halfs.hi; \
   NIC_TRACE_RECV; }

#endif	BUMPERS

#else	TRACE_MSG_DATA

#if	BUMPERS

#define send2(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y); NIC.io.full = z.full; \
   mcmsg_send_byte_count += 8; \
   NIC_TRACE_SEND; }

#define send2eod(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y); \
   mcmsg_send_byte_count += 8; \
   send2eod_(z.halfs.lo, z.halfs.hi); }

#define recv2(x,y) \
 { nic_reg z; z.full = NIC.io.full; x = z.halfs.lo; y = z.halfs.hi; \
   mcmsg_recv_byte_count += 8; \
   NIC_TRACE_RECV; }

#else	BUMPERS

#define send2(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y); NIC.io.full = z.full; \
   NIC_TRACE_SEND; }

#define send2eod(x,y) \
 { nic_reg z; z.halfs.lo = (x); z.halfs.hi = (y); EOD.io.full = z.full; \
   NIC_TRACE_SEND; }

#define recv2(x,y) \
 { nic_reg z; z.full = NIC.io.full; x = z.halfs.lo; y = z.halfs.hi; \
   NIC_TRACE_RECV; }

#endif	BUMPERS

#endif	TRACE_MSG_DATA

#endif	iPSC860

#endif MCMSG_HW_H
