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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_hw.h,v 1.24 1994/11/18 20:44:16 mtm Exp $
 */

/*
 * mcmsg_hw.h
 *
 * External declarations
 */

#ifndef MCMSG_HW_H
#define MCMSG_HW_H

#ifndef MCMSG_HW_EXT
#define MCMSG_HW_EXT extern
#endif

/*
 * Include files for hardware declarations
 */

#include <i860paragon/nic.h>
#include <i860paragon/lbus.h>
#include <i860paragon/led.h>
#include <i860/psl.h>

/*
 * Hardware constants
 */

#define	MSG_PAGE_SIZE		8192
#define MSG_PAGE_MASK		(~(MSG_PAGE_SIZE-1))

#define	DRAM_PAGE		(16 * 1024)
#define NEXT_DRAM_PAGE(a)	(((a) & ~(DRAM_PAGE - 1)) + DRAM_PAGE)

#define	CACHE_LINE		32			/* cache line count  */
#define	LTU_ALIGN		(CACHE_LINE)		/* LTU buffer start */
#define	LTU_MIN			(2 * CACHE_LINE)	/* smallest LTU xfer */
#define	BYTES_TO_LTUS(bytes)	(((bytes) >> 5) - 1)	/* xfers n + 1 lines */


/*
 * The Truth Table for combos of BUMPERS, BURST and BIGPKTS
 * and the rules for using send2 and recv2 macros when reading
 * and writing the NIC fifo using the CPU:
 *
 *  BUMPERS  BIGPKTS   BURST    Description    HEADER     DATA
 *  -------  -------   -----    -----------    ------     ----
 *  0        0         1        NICB, burst    now        now
 *  0        1         0        NICB stream    now        wait
 *  1        0         1        NICA, burst    now        now
 *  1        1         0        NICA stream    wait       wait
 */

#ifdef	BUMPERS
#define	BUMPERS	1
#else	BUMPERS
#define	BUMPERS	0
#endif	BUMPERS

#ifdef	BIGPKTS
#define	BURST	0
#else	BIGPKTS
#define	BURST	1
#endif	BIGPKTS

/*
 * Interrupt mode definitions:
 */
#if	BUMPERS
/*
 * NIC-A Bumpers Workaround requires two receive interrupt modes:
 *	Mode 1: Bumper is loaded, waiting for a real packet.
 *	Mode 2: Bumper not loaded, time to read real packet.
 *	Modes vary depending upon BURST.
 */
#if	BURST
#define	RECV_INTR_MODE_1 NIC_STAT_RX_FIFO_NOT_EMPTY
#define RECV_INTR_MODE_2 NIC_STAT_EOD_IN_NIC
#else	BURST
#define	RECV_INTR_MODE_1 NIC_STAT_RX_FIFO_NOT_EMPTY
#define RECV_INTR_MODE_2 (NIC_STAT_RX_FIFO_NOT_EMPTY|NIC_STAT_EOD_IN_NIC)
#endif BURST
#else	BUMPERS
/*
 * NIC-B: Only one interrupt mode needed.
 *	Mode varies depending upon BURST.
 */
#if	BURST
#define	RECV_INTR_MODE_1 NIC_STAT_EOD_IN_NIC
#else	BURST
#define RECV_INTR_MODE_1 (NIC_STAT_RX_FIFO_NOT_ALMOST_EMPTY|NIC_STAT_EOD_IN_NIC)
#endif	BURST
#endif	BUMPERS
/*
 * Send interrupt
 */
#define SEND_INTR_MODE   NIC_STAT_TX_FIFO_EMPTY

#if	BUMPERS

MCMSG_HW_EXT unsigned long mcmsg_send_byte_count;
MCMSG_HW_EXT unsigned long mcmsg_recv_byte_count;

/*
 * Be sure mcmsg_eod_last() is called even without MACH_ASSERT, because
 * it has part of the workaround in it
 */

#define	assert_mcmsg_eod_last() if (!mcmsg_eod_last()) assert(0)

#else	BUMPERS

#define	assert_mcmsg_eod_last() assert(mcmsg_eod_last())

#endif	BUMPERS

/*
 * Debugging control
 */

#define TRACE_MSG_DATA	0
#if TRACE_MSG_DATA && MACH_ASSERT
#define mcmsg_trace_send2(s,n,v1,v2,v3,v4) \
		mcmsg_trace_debug_(MCMSG_MODULE,s,n,v1,v2,v3,v4)
#define mcmsg_trace_recv2(s,n,v1,v2,v3,v4) \
		mcmsg_trace_debug_(MCMSG_MODULE,s,n,v1,v2,v3,v4)
#else   TRACE_MSG_DATA && MACH_ASSERT
#define mcmsg_trace_send2
#define mcmsg_trace_recv2
#endif  TRACE_MSG_DATA && MACH_ASSERT

/*
 *	SEND_WAIT and RECV_WAIT macros.
 *	In streaming mode, waits for the NIC fifo to be ready for
 *	input/output.
 *
 *	In burst mode, they are nops.
 *
 *	SEND_WAIT_ERRCHK, RECV_WAIT_ERRCHK macros.
 *	Same as above, but while we have the nic status in registers,
 *	we check for nic errors.  As NIC errors provoke an interrupt, this
 *	is intended for use in recv2_errchk() immediately after detecting intr.
 *
 *	In burst mode, they are nops, except the error check is retained.
 */
#if BIGPKTS
#define SEND_WAIT \
{	int t; \
	nic_reg	st; \
	assert(t=1000000); \
	do { \
		st.full = NIC.status.full; \
		assert(--t); \
	} while ((st.halfs.lo & NIC_STAT_TX_FIFO_NOT_FULL) == 0); }

#define SEND_WAIT_ERRCHK \
{	int t; \
	nic_reg	st; \
	assert(t=1000000); \
	do { \
		st.full = NIC.status.full; \
		if (st.halfs.lo & NIC_STAT_ERROR_BITS) \
			nic_error(st.halfs.hi, st.halfs.lo); \
		assert(--t); \
	} while ((st.halfs.lo & NIC_STAT_TX_FIFO_NOT_FULL) == 0); }

#define RECV_WAIT \
{	int t; \
	nic_reg	st; \
	assert(t=100000); \
	do { \
		st.full = NIC.status.full; \
		assert(--t); \
	} while ((st.halfs.lo & (NIC_STAT_CAN_READ_2|NIC_STAT_EOD_IN_NIC)) == 0); }

#define RECV_WAIT_ERRCHK \
{	int t; \
	nic_reg	st; \
	assert(t=100000); \
	do { \
		st.full = NIC.status.full; \
		if (st.halfs.lo & NIC_STAT_ERROR_BITS) \
			nic_error(st.halfs.hi, st.halfs.lo); \
		assert(--t); \
	} while ((st.halfs.lo & (NIC_STAT_CAN_READ_2|NIC_STAT_EOD_IN_NIC)) == 0); }

#else BIGPKTS
#define SEND_WAIT
#define SEND_WAIT_ERRCHK \
{	nic_reg	st; \
	st.full = NIC.status.full; \
	if (st.halfs.lo & NIC_STAT_ERROR_BITS) \
		nic_error(st.halfs.hi, st.halfs.lo); \
}
#define RECV_WAIT
#define RECV_WAIT_ERRCHK SEND_WAIT_ERRCHK
#endif BIGPKTS


/*
 *	RELEASE_TX_FIFO and DISABLE_TX_FIFO macros.
 *	In burst mode, they enable/disable the NIC onto the mesh.
 *	In streaming mode, they are nops.
 */
#if BURST
#define RELEASE_TX_FIFO \
{	nic_reg	x; \
    x.halfs.hi = 0; \
    x.halfs.lo = NIC_TEST_RCV_AFULL_SEL; \
    NIC.reset_test.full = x.full; }
	
#define DISABLE_TX_FIFO \
{	nic_reg x; \
	x.halfs.hi = 0; \
    x.halfs.lo = NIC_TEST_DISABLE_TX | NIC_TEST_RCV_AFULL_SEL; \
    NIC.reset_test.full = x.full; }

#else BURST
#define RELEASE_TX_FIFO
#define DISABLE_TX_FIFO
#endif

/*
 * NIC status trace buffer
 */
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

/*
 * If bumpers, we have to keep count of bytes for the EVENer.
 */
#if    BUMPERS
#define INC_SEND_BYTE_COUNT	mcmsg_send_byte_count += 8
#define INC_RECV_BYTE_COUNT	mcmsg_recv_byte_count += 8
#else  BUMPERS
#define INC_SEND_BYTE_COUNT
#define INC_RECV_BYTE_COUNT
#endif BUMPERS


/*
 * Send and receive macros
 *
 * Each macro sends or receives 64 bits
 * The arguments are two 32-bit longs
 * The first is the least significant half and is sent or received first
 */

#define send2(x,y) \
{	nic_reg z; \
	SEND_WAIT \
	mcmsg_trace_send2("send2   ", 2, x, y, 0, 0); \
	z.halfs.lo = (x); \
	z.halfs.hi = (y); \
	NIC.io.full = z.full; \
	INC_SEND_BYTE_COUNT; \
	NIC_TRACE_SEND; }

#define send2_errchk(x,y) \
{	nic_reg z; \
	SEND_WAIT_ERRCHK \
	mcmsg_trace_send2("send2   ", 2, x, y, 0, 0); \
	z.halfs.lo = (x); \
	z.halfs.hi = (y); \
	NIC.io.full = z.full; \
	INC_SEND_BYTE_COUNT; \
	NIC_TRACE_SEND; }

#define send2_now(x,y) \
{	nic_reg z; \
	mcmsg_trace_send2("send2 now  ", 2, x, y, 0, 0); \
	z.halfs.lo = (x); \
	z.halfs.hi = (y); \
	NIC.io.full = z.full; \
	INC_SEND_BYTE_COUNT; \
	NIC_TRACE_SEND; }

#if BUMPERS

#define send2eod(x,y) \
{	SEND_WAIT \
	mcmsg_trace_send2("send2eod    ", 2, x, y, 0, 0); \
	INC_SEND_BYTE_COUNT; \
	send2eod_(x, y); }

#define send2eod_now(x,y) \
{	mcmsg_trace_send2("send2eod now", 2, x, y, 0, 0); \
	INC_SEND_BYTE_COUNT; \
	send2eod_(x, y); }

#else  BUMPERS

#define send2eod(x,y) \
{	nic_reg z; \
	SEND_WAIT \
	mcmsg_trace_send2("send2eod    ", 2, x, y, 0, 0); \
	z.halfs.lo = (x); \
	z.halfs.hi = (y); \
	EOD.io.full = z.full; }

#define send2eod_now(x,y) \
{	nic_reg z; \
	mcmsg_trace_send2("send2eod now", 2, x, y, 0, 0); \
	z.halfs.lo = (x); \
	z.halfs.hi = (y); \
	EOD.io.full = z.full; }

#endif BUMPERS

#define recv2(x,y) \
{	nic_reg z; \
	RECV_WAIT \
	z.full = NIC.io.full;  \
	x = z.halfs.lo; \
	y = z.halfs.hi; \
	mcmsg_trace_recv2("recv2      ", 2, x, y, 0, 0); \
	INC_RECV_BYTE_COUNT; \
	NIC_TRACE_RECV; }

#define recv2_errchk(x,y) \
{	nic_reg z; \
	RECV_WAIT_ERRCHK \
	z.full = NIC.io.full;  \
	x = z.halfs.lo; \
	y = z.halfs.hi; \
	mcmsg_trace_recv2("recv2      ", 2, x, y, 0, 0); \
	INC_RECV_BYTE_COUNT; \
	NIC_TRACE_RECV; }

#define recv2_now(x,y) \
{	nic_reg z; \
	z.full = NIC.io.full;  \
	x = z.halfs.lo; \
	y = z.halfs.hi; \
	mcmsg_trace_recv2("recv2 now  ", 2, x, y, 0, 0); \
	INC_RECV_BYTE_COUNT; \
	NIC_TRACE_RECV; }

#define recv2dummy() \
{	nic_reg z; \
	RECV_WAIT \
	z.full = NIC.io.full; \
	mcmsg_trace_recv2("recv2dum    ", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
	INC_RECV_BYTE_COUNT; \
	NIC_TRACE_RECV; }

#define recv2dummy_now() \
{	nic_reg z; \
	z.full = NIC.io.full; \
	mcmsg_trace_recv2("recv2dum now", 2, z.halfs.lo, z.halfs.hi, 0, 0); \
	INC_RECV_BYTE_COUNT; \
	NIC_TRACE_RECV; }

/*
 * recv_hdr macros - use these macros to read the headers of 
 *                   messages that have data associated with them
 *                   (such an NX1 and NXN) in order to hide the
 *                   hardware dependencies.
 */

#if	BIGPKTS && BUMPERS

#define	recv_hdr2() \
	recv2dummy() \
	recv2dummy() \
	recv2dummy() \
	recv2dummy()

#define	recv_hdr4(a,b) \
	recv2(a,b) \
	recv2dummy() \
	recv2dummy() \
	recv2dummy()

#define	recv_hdr6(a,b,c,d) \
	recv2(a,b) \
	recv2(c,d) \
	recv2dummy() \
	recv2dummy()

#define	recv_hdr8(a,b,c,d,e,f) \
	recv2(a,b) \
	recv2(c,d) \
	recv2(e,f) \
	recv2dummy()

#define	recv_hdr10(a,b,c,d,e,f,g,h) \
	recv2(a,b) \
	recv2(c,d) \
	recv2(e,f) \
	recv2(g,h)

#else	BIGPKTS && BUMPERS

#define	recv_hdr2()

#define	recv_hdr4(a,b) \
	recv2_now(a,b) \
	recv2dummy_now()

#define	recv_hdr6(a,b,c,d) \
	recv2_now(a,b) \
	recv2_now(c,d)

#define	recv_hdr8(a,b,c,d,e,f) \
	recv2_now(a,b) \
	recv2_now(c,d) \
	recv2_now(e,f) \
	recv2dummy_now()

#define	recv_hdr10(a,b,c,d,e,f,g,h) \
	recv2_now(a,b) \
	recv2_now(c,d) \
	recv2_now(e,f) \
	recv2_now(g,h)

#endif	BIGPKTS && BUMPERS

/*
 * MCMSG_BUMPER_OUT  macro
 *
 *	Write BUMPER and EVENer to the network.
 */
#if BUMPERS
#if MACH_ASSERT
#define MCMSG_BUMPER_OUT \
		{		\
			nic_reg t;		\
			if (mcmsg_send_byte_count % 16 == 8) { \
				t.halfs.hi = 0xbabeeeee;	\
				t.halfs.lo = MCTRL_EVEN;	\
				SEND_WAIT;	\
				NIC.io.full = t.full;	\
			}		\
			t.halfs.hi = 0xbabefe9d;		\
			t.halfs.lo = MCTRL_BUMP;		\
			SEND_WAIT;	\
			NIC.io.full = t.full;		\
			t.halfs.lo = MCTRL_BUMP | 1 << 16;		\
			SEND_WAIT;	\
			NIC.io.full = t.full;		\
			t.halfs.lo = MCTRL_BUMP | 2 << 16;		\
			SEND_WAIT;	\
			NIC.io.full = t.full;		\
			t.halfs.lo = MCTRL_BUMP | 3 << 16;		\
			SEND_WAIT;	\
			EOD.io.full = t.full;		\
			mcmsg_send_byte_count = 0;		\
			NIC_TRACE_SEND;		\
			mcmsg_nic_status(13);		\
		}
#else MACH_ASSERT
#define MCMSG_BUMPER_OUT \
		{		\
			nic_reg t;		\
			register double	x;		\
			if (mcmsg_send_byte_count % 16 == 8) { \
				t.halfs.lo = MCTRL_EVEN;		\
				t.halfs.hi = 0xbabeeeee;		\
				SEND_WAIT;	\
				NIC.io.full = t.full;		\
			}       \
			t.halfs.hi = 0xbabefe9d;		\
			t.halfs.lo = MCTRL_BUMP;		\
			x = t.full;		\
			SEND_WAIT;	\
			NIC.io.full = x;		\
			SEND_WAIT;	\
			NIC.io.full = x;		\
			SEND_WAIT;	\
			NIC.io.full = x;		\
			SEND_WAIT;	\
			EOD.io.full = x;		\
			mcmsg_send_byte_count = 0;		\
		}
#endif MACH_ASSERT
#else	BUMPERS
/*
 *	No bumpers.
 */
#define	MCMSG_BUMPER_OUT
#endif	BUMPERS

#endif MCMSG_HW_H
