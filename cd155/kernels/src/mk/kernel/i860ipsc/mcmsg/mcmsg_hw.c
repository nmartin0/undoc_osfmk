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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_hw.c,v 1.26 1994/11/18 20:41:23 mtm Exp $
 */

/*
 * mcmsg_hw.c
 *
 * Hardware support routines for multicomputer message passing
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#define MCMSG_HW_EXT
#include <i860ipsc/mcmsg/mcmsg_hw.h>

int mcmsg_int_enables = 0;

#define	BURST 		PARAGON860
#define ENABLE_LTU	PARAGON860

#define LTU_RECV	PARAGON860
#define LTU_SEND	PARAGON860
#define LTU_SEND_EOD	PARAGON860

unsigned long	mcmsg_data_flush;
unsigned long	mcmsg_data_in_fifo;
unsigned long	mcmsg_data_in_ltu;
unsigned long	mcmsg_data_out_fifo;
unsigned long	mcmsg_data_out_ltu;


/*
 * FIFO Alignment.
 */
#if    PARAGON860
#define FIFO_ALIGN	8
#else  PARAGON860
#define FIFO_ALIGN	4
#endif PARAGON860

/*
 *	FIFO Tracing.
 */
#define TRACE_FIFO 0
#if TRACE_FIFO
#define MCMSG_TRACE_FIFO(s,n,a,b,c,d) mcmsg_trace_debug(s,n,a,b,c,d)
#else TRACE_FIFO
#define MCMSG_TRACE_FIFO(s,n,a,b,c,d)
#endif TRACE_FIFO

#if	PARAGON860

#include <i860paragon/dp.h>
#include <i860paragon/ltu.h>

int		fab7_or_greater;
nic_reg		mcmsg_nic_control;
unsigned long	mcmsg_nic_config;

#if	NIC_TRACE
char *mcmsg_nic_trace_name[] = {
	"?              ",
	"send_ready     ",
	"recv_ready 1   ",
	"recv_ready 2   ",
	"eod_last       ",
	"flush_invalid  ",
	"end recv       ",
	"sendeod_       ",
	"init           ",
	"check          ",
	"?              ",
};
#endif	NIC_TRACE

unsigned long	mcmsg_send_count;
unsigned long	mcmsg_recv_count;

#if	BUMPERS

#define	SHORT_BUMPER	1

#if	SHORT_BUMPER

#define	BUMPER	4
#define NIC_STAT_RX_LOW	NIC_STAT_RX_FIFO_NOT_EMTPY		

#else	SHORT_BUMPER

#define	BUMPER	38
#define NIC_STAT_RX_LOW	NIC_STAT_RX_FIFO_NOT_ALMOST_EMPTY	

#endif	SHORT_BUMPER


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

#endif	PARAGON860

int	mcmsg_recv_loaded;
int	mcmsg_recv_enable;
int	mcmsg_send_enable;

int	mcmsg_ltu_node;

double
mcmsg_nic_status(place)
	unsigned long	place;
{
#if	1
	nic_reg	t;

	t.full = NIC.status.full;
#else
	nic_reg	t;
	nic_reg	u;
	nic_reg	v;

	t.full = NIC.status.full;
	u.full = NIC.status.full;
	v.full = NIC.status.full;
	while (t.full != u.full || u.full != v.full) {
		t = u;
		u = v;
		v.full = NIC.status.full;
	}
#endif
#if	NIC_TRACE
	mcmsg_nic_trace_count++;
	mcmsg_nic_trace_index++;
	if (mcmsg_nic_trace_index == NIC_TRACE_SIZE) {
		mcmsg_nic_trace_index = 0;
	}
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].status = t;
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].rcount = 0;
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].scount = 0;
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].place = place;
	assert((t.halfs.lo & NIC_STAT_ERRORS) == 0);
#endif	NIC_TRACE
	return t.full;
}

unsigned long	mcmsg_send_count_check;
unsigned long	mcmsg_recv_count_check;
unsigned long	mcmsg_recv_count_last;
int		mcmsg_check_flag;

mcmsg_nic_check()
{
	nic_reg	t;
	unsigned long	index_save;
	unsigned long	count_save;
	int		x;

#if	NIC_TRACE
	index_save = mcmsg_nic_trace_index;
	count_save = mcmsg_nic_trace_count;
#endif	NIC_TRACE
	t.full = mcmsg_nic_status(9);
	if ((t.halfs.hi & NIC_STAT_RMIP) != 0) {
		RED_ON(RED_RMIP);
	} else {
		RED_OFF(RED_RMIP);
	}
	if ((t.halfs.hi & NIC_STAT_XMIP) != 0) {
		RED_ON(RED_XMIP);
	} else {
		RED_OFF(RED_XMIP);
	}
#if	NIC_TRACE
	if ((t.halfs.hi & NIC_STAT_RMIP) != 0 &&
	    (t.halfs.lo & NIC_STAT_EOD_IN_NIC) == 0) {
		if (mcmsg_recv_count_check == mcmsg_recv_count) {
			assert(mcmsg_check_flag++ != 100);
		} else {
			if (mcmsg_check_flag < 100) {
				mcmsg_check_flag = 0;
			}
		}
		mcmsg_recv_count_check = mcmsg_recv_count;
	} else if (mcmsg_recv_count_last == mcmsg_recv_count) {
		mcmsg_nic_trace_index = index_save;
		mcmsg_nic_trace_count = count_save;
	}
	mcmsg_recv_count_last = mcmsg_recv_count;
#endif	NIC_TRACE
}

#endif	BUMPERS

mcmsg_hw_init()
{
	char	*s;
	extern char *getbootenv();

#if	PARAGON860

	if ((s = getbootenv("BOOT_HANG_TIME")) != 0) {
		boot_hang_time = atoi(s);
	} else {
		boot_hang_time = 5;
	}

	mcmsg_nic_control.halfs.lo = 0;
	mcmsg_nic_control.halfs.hi = NIC_CNTRL_EN_iMRC | NIC_CNTRL_EN_ALMOST;

	if ((s = getbootenv("BOOT_NIC_MODE")) != 0) {
		switch (*s) {

		case '0':
		case 'i':	/* interlocked */
			break;

		case '1':
		case 's':	/* slow */
			mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_STREAM;
			break;

		case '2':
		case 'f':	/* fast */
			mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_STREAM;
			mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_SPEED;
			break;

		default:
			assert(0);
		}

	} else {	/* Default slow streaming */
		mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_STREAM;
	}
	if ((s = getbootenv("BOOT_NIC_CONFIG")) != 0) {
		mcmsg_nic_config = atoi(s);
	} else {
		/*
		 * Default NIC flags:
		 *     16 doubles before XMIT fifo empty
		 *      8 doubles before RECV fifo almost full
		 */
		mcmsg_nic_config = 0x023C;
	}

	{
		nic_reg	t;

		t.halfs.lo = 0;
		t.halfs.hi = NIC_CNTRL_RESET;
		NIC.set.full = t.full;

		t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
		t.halfs.hi = 0;
		NIC.reset_test.full = t.full;

		t.halfs.lo = mcmsg_nic_config;
		t.halfs.hi = 0;
		NIC.fifo_offset.full = t.full;

		NIC.set.full = mcmsg_nic_control.full;
	}
#if	BUMPERS
	mcmsg_recv_loaded = 1;
	mcmsg_recv_enable = 0;
#if	BURST
	mcmsg_send_enable = NIC_STAT_TX_FIFO_EMPTY;
#else	BURST
	mcmsg_send_enable = NIC_STAT_TX_FIFO_ALMOST_EMPTY;
#endif	BURST
	mcmsg_recv_byte_count = 0;
	mcmsg_send_byte_count = 0;
	{
		nic_reg	t, u;

		t.halfs.lo = 0;
		t.halfs.hi = NIC_CNTRL_RESET;
		NIC.set.full = t.full;

		t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
		t.halfs.hi = 0;
		NIC.reset_test.full = t.full;

		t.halfs.lo = mcmsg_nic_config;
		t.halfs.hi = 0;
		NIC.fifo_offset.full = t.full;

		NIC.set.full = mcmsg_nic_control.full;

		mcmsg_nic_status(8);

		send2eod(0, 0);

		t.full = NIC.control.full;
		u.full = NIC.status.full;
		mcmsg_trace_debug("NIC init", 4,
			t.halfs.hi, t.halfs.lo, u.halfs.hi, u.halfs.lo);
		printf("NIC control %08X %08X  status %08X %08X\n",
			t.halfs.hi, t.halfs.lo, u.halfs.hi, u.halfs.lo);

	}
#endif	BUMPERS

	if ((s = getbootenv("BOOT_LTU_NODE")) != 0) {
		mcmsg_ltu_node = atoi(s);
	} else {
		mcmsg_ltu_node = ipsc_physnode;
	}

	fab7_or_greater = node_status_register_node_revision() >= 6;

#endif	PARAGON860
}

#if	PARAGON860
mcmsg_nic_reset()
{
	nic_reg	t;

	t.halfs.lo = 0;
	t.halfs.hi = NIC_CNTRL_RESET;
	NIC.set.full = t.full;
	mcmsg_hw_init();
}
#endif	PARAGON860

/*
 * Fifo transfer code
 */

long mcmsg_pbuf[I860_PGBYTES/sizeof(long)];

mcmsg_recv_buf(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	t1;
	register unsigned long	t2;

	t1 = bp1 & INTEL_PTE_PFN;
	t2 = bp2 & INTEL_PTE_PFN;
	if (t1 == t2) {
		MCMSG_TRACE_FIFO("in      all", 2, bp1, pkt, 0, 0);
		mcmsg_fifo_in(bp1, pkt);
	} else {
		t1 = t1 + I860_PGBYTES - bp1;
		assert(t1 < pkt);
		if ((t1 & (FIFO_ALIGN-1)) == 0)
		{
			MCMSG_TRACE_FIFO("in    first", 2, bp1, t1, 0, 0);
			mcmsg_fifo_in(bp1, t1);
			MCMSG_TRACE_FIFO("in     last", 2, t2, pkt - t1, 0, 0);
			mcmsg_fifo_in(t2, pkt - t1);
		} else {
			MCMSG_TRACE_FIFO("in     copy", 2, mcmsg_pbuf, pkt, 0, 0);
			mcmsg_fifo_in(
				(unsigned long)mcmsg_validate_real(
					mcmsg_pbuf,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__), pkt);
			bcopy(mcmsg_pbuf, bp1, t1);
			bcopy(((unsigned long)mcmsg_pbuf) + t1, t2, pkt - t1);
		}
	}
}

mcmsg_send_buf(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	t1;
	register unsigned long	t2;

	t1 = bp1 & INTEL_PTE_PFN;
	t2 = bp2 & INTEL_PTE_PFN;
	if (t1 == t2) {
		MCMSG_TRACE_FIFO("outeod  all", 2, bp1, pkt, 0, 0);
		mcmsg_fifo_out_eod(bp1, pkt);
	} else {
		t1 = t1 + I860_PGBYTES - bp1;
		assert(t1 < pkt);

		if ((t1 & (FIFO_ALIGN-1)) == 0)
		{
			MCMSG_TRACE_FIFO("out   first", 2, bp1, t1, 0, 0);
			mcmsg_fifo_out(bp1, t1);
			MCMSG_TRACE_FIFO("outeod last", 2, t2, pkt - t1, 0, 0);
			mcmsg_fifo_out_eod(t2, pkt - t1);
		} else {
			bcopy(bp1, mcmsg_pbuf, t1);
			bcopy(t2, ((unsigned long)mcmsg_pbuf) + t1, pkt - t1);
			MCMSG_TRACE_FIFO("outeod copy", 2, mcmsg_pbuf, pkt, 0, 0);
			mcmsg_fifo_out_eod(
				(unsigned long)mcmsg_validate_real(
					mcmsg_pbuf,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__), pkt);
		}
	}

}

#if PARAGON860

#define	DRAM_PAGE		(16 * 1024)
#define NEXT_DRAM_PAGE(a)	(((a) & ~(DRAM_PAGE - 1)) + DRAM_PAGE)

#define	CACHE_LINE		32			/* cache line count  */
#define	LTU_ALIGN		(2 * CACHE_LINE)	/* smallest LTU xfer */
#define	BYTES_TO_LTUS(bytes)	(((bytes) >> 5) - 1)	/* xfers n + 1 lines */

#if	ENABLE_LTU
mcmsg_ltu_send(addr, n)
	register unsigned long	*addr;
	register unsigned long	n;
{

	assert(((unsigned long)addr & (LTU_ALIGN-1)) == 0);
	assert((n & (LTU_ALIGN-1)) == 0);
	assert(n >= LTU_ALIGN);
	assert(n <= DRAM_PAGE);
	assert((unsigned long)addr+n <= NEXT_DRAM_PAGE((unsigned long)addr));
	assert( ((unsigned long)addr) ==
		(unsigned long)mcmsg_validate_line_real(
			addr,
			current_task()->map->pmap->dirbase,
			__FILE__, __LINE__));
#if	LTU_SEND
	if ((ipsc_physnode == mcmsg_ltu_node) && fab7_or_greater) {
		assert((inl(DP_CONTROL1_READ) & (1 << DP_IMSK1_LTU1_CNT)) == 0);
		RED_ON(RED_LTU);
		ltu_start((((unsigned long)addr) + 4) & 0x3fffffff,
			BYTES_TO_LTUS(n) | 0xc0000000);
		ltu_send_wait();	/* poll for completion */
		RED_OFF(RED_LTU);
	} else
#endif	LTU_SEND
	soft_ltu_send(addr, n, &NIC.io.full, &NIC.io.full);
}

mcmsg_ltu_sendeod(addr, n)
	register unsigned long	*addr;
	register unsigned long	n;
{

	assert(((unsigned long)addr & (LTU_ALIGN-1)) == 0);
	assert((n & (LTU_ALIGN-1)) == 0);
	assert(n >= LTU_ALIGN);
	assert(n <= DRAM_PAGE);
	assert((unsigned long)addr+n <= NEXT_DRAM_PAGE((unsigned long)addr));
	assert( ((unsigned long)addr) ==
		(unsigned long)mcmsg_validate_line_real(
			addr,
			current_task()->map->pmap->dirbase,
			__FILE__, __LINE__));
#if	LTU_SEND_EOD
	if ((ipsc_physnode == mcmsg_ltu_node) && fab7_or_greater) {
		assert((inl(DP_CONTROL1_READ) & (1 << DP_IMSK1_LTU1_CNT)) == 0);
		RED_ON(RED_LTU);
		ltu_start((((unsigned long)addr) + 4) & 0x3fffffff,
#if	BUMPERS
			BYTES_TO_LTUS(n) | 0xc0000000);
#else	BUMPERS
			BYTES_TO_LTUS(n) | 0x40000000);
#endif	BUMPERS
		ltu_send_wait();	/* poll for completion */
		RED_OFF(RED_LTU);
#if	BUMPERS
#if	MACH_ASSERT
		{
			nic_reg	t;
			int	i;

			if (mcmsg_send_byte_count % 16 == 8) {
				t.halfs.lo = 0xeeeeeeee;
				t.halfs.hi = 0xbabeeeee;
				NIC.io.full = t.full;
				NIC_TRACE_SEND;
			}
			for (i = 0; i < BUMPER-1; i++) {
				t.halfs.lo = i;
				t.halfs.hi = 0xbabefe9d;
				NIC.io.full = t.full;
				NIC_TRACE_SEND;
			}
			t.halfs.lo = i;
			t.halfs.hi = 0xbabefe9d;
			EOD.io.full = t.full;
			mcmsg_send_byte_count = 0;
			NIC_TRACE_SEND;
			mcmsg_nic_status(7);
			mcmsg_send_count++;
#if	BURST
			t.halfs.hi = 0;
			t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
			NIC.reset_test.full = t.full;
#endif	BURST
		}
#else	MACH_ASSERT
		{
			nic_reg	t;
			register double	x;

			if (mcmsg_send_byte_count % 16 == 8) {
				t.halfs.hi = 0xbabeeeee;
				NIC.io.full = t.full;
			}
			NIC.io.full = x;
			NIC.io.full = x;
			NIC.io.full = x;
			EOD.io.full = x;
			mcmsg_send_byte_count = 0;
			mcmsg_send_count++;
#if	BURST
			t.halfs.hi = 0;
			t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
			NIC.reset_test.full = t.full;
#endif	BURST
		}
#endif	MACH_ASSERT
#endif	BUMPERS
	} else
#endif	LTU_SEND_EOD
	{
#if	BUMPERS
		soft_ltu_send(addr, n, &NIC.io.full, &NIC.io.full);
#if	MACH_ASSERT
		{
			nic_reg	t;
			int	i;
	
			if (mcmsg_send_byte_count % 16 == 8) {
				t.halfs.lo = 0xeeeeeeee;
				t.halfs.hi = 0xbabeeeee;
				NIC.io.full = t.full;
				NIC_TRACE_SEND;
			}
			for (i = 0; i < BUMPER-1; i++) {
				t.halfs.lo = i;
				t.halfs.hi = 0xbabefe9d;
				NIC.io.full = t.full;
				NIC_TRACE_SEND;
			}
			t.halfs.lo = i;
			t.halfs.hi = 0xbabefe9d;
			EOD.io.full = t.full;
			mcmsg_send_byte_count = 0;
			NIC_TRACE_SEND;
			mcmsg_nic_status(7);
			mcmsg_send_count++;
#if	BURST
			t.halfs.hi = 0;
			t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
			NIC.reset_test.full = t.full;
#endif	BURST
		}
#else	MACH_ASSERT
		{
			nic_reg	t;
			register double	x;
	
			if (mcmsg_send_byte_count % 16 == 8) {
				t.halfs.hi = 0xbabeeeee;
				NIC.io.full = t.full;
			}
			NIC.io.full = x;
			NIC.io.full = x;
			NIC.io.full = x;
			EOD.io.full = x;
			mcmsg_send_byte_count = 0;
			mcmsg_send_count++;
#if	BURST
			t.halfs.hi = 0;
			t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
			NIC.reset_test.full = t.full;
#endif	BURST
		}
#endif	MACH_ASSERT
#else	BUMPERS
		soft_ltu_send(addr, n, &NIC.io.full, &NIC.eod.full);
#endif	BUMPERS
	}
}

mcmsg_ltu_recv(addr, n)
	register unsigned long	*addr;
	register unsigned long	n;
{

	assert(((unsigned long)addr & (LTU_ALIGN-1)) == 0);
	assert((n & (LTU_ALIGN-1)) == 0);
	assert(n >= LTU_ALIGN);
	assert(n <= DRAM_PAGE);
	assert((unsigned long)addr+n <= NEXT_DRAM_PAGE((unsigned long)addr));
	assert( ((unsigned long)addr) ==
		(unsigned long)mcmsg_validate_line_real(
			addr,
			current_task()->map->pmap->dirbase,
			__FILE__, __LINE__));
#if	LTU_RECV
	if ((ipsc_physnode == mcmsg_ltu_node) && fab7_or_greater) {
		assert((inl(DP_CONTROL1_READ) & (1 << DP_IMSK1_LTU0_CNT)) == 0);
		RED_ON(RED_LTU);
		ltu_start(((unsigned long)addr) & 0x3fffffff,
			BYTES_TO_LTUS(n) | 0x40000000);
		ltu_recv_wait();	/* poll for completion */
		RED_OFF(RED_LTU);
	} else
#endif	LTU_RECV
	soft_ltu_recv(addr, n, &NIC.io.full);
}

ltu_start(a, n)
unsigned long a;
unsigned long n;
{
	asm("	stio.l	r17,r16");
}

ltu_send_wait()
{
	register int i = 0;

	while (!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT)) {
		/*
		 * In BURST mode, LTU transmits that do not fit in the NIC FIFO
		 * will not complete.  The assertion below should catch it.
		 */
		assert(i++ < 1000000);
	}
	inl(DP_LTU1_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT));
}

ltu_recv_wait()
{
	register int i = 0;
	nic_reg	t;

	while (!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT)) {
		/*
		 * Check for LTU receives that are terminated early by an EOD
		 * from the NIC.  This shouldn't be needed.  Be carefull!
		 */
		t.full = mcmsg_nic_status(10);
		if (t.halfs.lo & NIC_STAT_EOD_IN_WORD) break;
		assert(i++ < 1000000);
	}
	inl(DP_LTU0_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT));
}


soft_ltu_holder()
{
asm("//	soft_ltu_recv(address, bytecount, nicaddr)		");
asm("	.align 4						");
asm("_soft_ltu_recv::						");
asm("	pfld.d	0(r18), f0	//  1st stage			");
asm("	adds	-32, r0, r22	// r22 is bla decrement		");
asm("	pfld.d	0(r18), f0	//  2nd stage			");
asm("	adds	-32, r17, r20	// r20 is bla count		");
asm("	pfld.d	0(r18), f0	//  3rd stage			");
asm("	bla	r22, r20, 0f	// prime bla			");
asm("	 adds	-16, r16, r21	// r21 is store address		");
asm("0:	bla	r22, r20, 2f	// tick off 32			");
asm("	 pfld.d	0(r18), f16	//  pipe primed and loading	");
asm("	pfld.d	16(r21), f18	//  flush pipe			");
asm("1:	pfld.d	16(r21), f20	//				");
asm("	pfld.d	16(r21), f22	//				");
asm("	fst.q	f16, 16(r21)++	// store final 32 bytes		");
asm("	bri	r1		// return			");
asm("	 fst.q	f20, 16(r21)	//				");
asm("								");
asm("2:	pfld.d	0(r18), f18	//  load next 32 bytes		");
asm("	pfld.d	0(r18), f20	//				");
asm("	pfld.d	0(r18), f22	//				");
asm("	bla	r22, r20, 4f	// tick off 32			");
asm("	 pfld.d	0(r18), f24	//				");
asm("3:	pfld.d	16(r21), f26	//  flush pipe			");
asm("	pfld.d	16(r21), f28	//				");
asm("	pfld.d	16(r21), f30	//				");
asm("	fst.q	f16, 16(r21)++	// store final 64 bytes		");
asm("	fst.q	f20, 16(r21)++	//				");
asm("	fst.q	f24, 16(r21)++	//				");
asm("	bri	r1		// return			");
asm("	 fst.q	f28, 16(r21)	//				");
asm("								");
asm("4:	pfld.d	0(r18), f26	//  load next 32 bytes		");
asm("	pfld.d	0(r18), f28	//				");
asm("	pfld.d	0(r18), f30	//				");
asm("	fst.q	f16, 16(r21)++	// store next 64 bytes		");
asm("	fst.q	f20, 16(r21)++	//				");
asm("	fst.q	f24, 16(r21)++	//				");
asm("	fst.q	f28, 16(r21)++	//				");
asm("	bla	r22, r20, 2b	// tick off 32			");
asm("	 pfld.d	0(r18), f16	//				");
asm("	br	1b		//				");
asm("	 pfld.d	16(r21), f18	//  flush pipe			");
asm("								");
asm("//	soft_ltu_send(address, bytecount, nicaddr, eodaddr)	");
asm("	.align 4						");
asm("_soft_ltu_send::						");
asm("	pfld.d	0(r16), f0	//  1st stage			");
asm("	adds	-32, r0, r22	// r22 is bla decrement		");
asm("	pfld.d	8(r16)++, f0	//  2nd stage			");
asm("	adds	-32, r17, r20	// r20 is bla count		");
asm("	pfld.d	8(r16)++, f0	//  3rd stage			");
asm("	bla	r22, r20, 0f	// prime bla			");
asm("	 nop			//				");
asm("0:	bla	r22, r20, 2f	// tick off 32			");
asm("	 pfld.d	8(r16)++, f16	//  pipe primed and loading	");
asm("	pfld.d	0(r16), f18	//  flush pipe			");
asm("1:	pfld.d	0(r16), f20	//				");
asm("	pfld.d	0(r16), f22	//				");
asm("	fst.d	f16, 0(r18)	// store final 32 bytes		");
asm("	fst.d	f18, 0(r18)	//				");
asm("	fst.d	f20, 0(r18)	//				");
asm("	bri	r1		// return			");
asm("	 fst.d	f22, 0(r19)	// send eod			");
asm("								");
asm("2:	pfld.d	8(r16)++, f18	//  load next 32 bytes		");
asm("	pfld.d	8(r16)++, f20	//				");
asm("	pfld.d	8(r16)++, f22	//				");
asm("	bla	r22, r20, 4f	// tick off 32			");
asm("	 pfld.d	8(r16)++, f24	//				");
asm("3:	pfld.d	0(r16), f26	//  flush pipe			");
asm("	pfld.d	0(r16), f28	//				");
asm("	pfld.d	0(r16), f30	//				");
asm("	fst.d	f16, 0(r18)	// store final 64 bytes		");
asm("	fst.d	f18, 0(r18)	// store final 64 bytes		");
asm("	fst.d	f20, 0(r18)	//				");
asm("	fst.d	f22, 0(r18)	//				");
asm("	fst.d	f24, 0(r18)	//				");
asm("	fst.d	f26, 0(r18)	//				");
asm("	fst.d	f28, 0(r18)	//				");
asm("	bri	r1		// return			");
asm("	 fst.d	f30, 0(r19)	// send eod			");
asm("								");
asm("4:	pfld.d	8(r16)++, f26	//  load next 32 bytes		");
asm("	pfld.d	8(r16)++, f28	//				");
asm("	pfld.d	8(r16)++, f30	//				");
asm("	fst.d	f16, 0(r18)	// store next 64 bytes		");
asm("	fst.d	f18, 0(r18)	// store next 64 bytes		");
asm("	fst.d	f20, 0(r18)	//				");
asm("	fst.d	f22, 0(r18)	//				");
asm("	fst.d	f24, 0(r18)	//				");
asm("	fst.d	f26, 0(r18)	//				");
asm("	fst.d	f28, 0(r18)	//				");
asm("	fst.d	f30, 0(r18)	//				");
asm("	bla	r22, r20, 2b	// tick off 32			");
asm("	 pfld.d	8(r16)++, f16	//				");
asm("	br	1b		//				");
asm("	 pfld.d	0(r16), f18	//  flush pipe			");
}

#endif	ENABLE_LTU

mcmsg_fifo_out(addr, n)
	register unsigned long addr;
	register unsigned long n;
{
	register unsigned long m;

	RED_ON(RED_XMIT);
	assert(n <= 4096);
	if ((addr & 7) == 0) {

#if	ENABLE_LTU
		m = (LTU_ALIGN-1) & -addr;
		if (n >= m && n - m >= LTU_ALIGN) {
			register unsigned long *saddr = (unsigned long *) addr;

			n -= m;
			if (m) {
				mcmsg_data_out_fifo += m;
				do {
					/* Don't try this at home */
					/*send2(*saddr++, *saddr++);*/
					send2(saddr[0], saddr[1]);
					saddr += 2;
					m -= 8;
				} while (m);
			}

			m = n & (LTU_ALIGN-1);
			n &= ~(LTU_ALIGN-1);
			mcmsg_data_out_ltu += n;
			mcmsg_ltu_send(saddr, n);

			if (m) {
				saddr = (unsigned long *)
				 ((unsigned long)saddr + n);
				mcmsg_data_out_fifo += m;
				while (m) {
					/* Don't try this at home */
					/*send2(*saddr++, *saddr++);*/
					send2(saddr[0], saddr[1]);
					saddr += 2;
					m -= 8;
				}
			}
		} else
#endif	ENABLE_LTU
		{
			register unsigned long *saddr = (unsigned long *) addr;
			register unsigned long *eaddr = (unsigned long *)
							(addr + n);

			mcmsg_data_out_fifo += n;
			while (saddr < eaddr) {
				/* Don't try this at home */
				/*send2(*saddr++, *saddr++);*/
				send2(saddr[0], saddr[1]);
				saddr += 2;
			}
			assert(saddr == eaddr);
		}

	} else {
		unsigned long tv[2];
		register unsigned char *p;
		register unsigned char *saddr = (unsigned char *) addr;
		register unsigned char *eaddr = (unsigned char *) (addr + n);

		mcmsg_data_out_fifo += n;
		while (saddr < eaddr) {
			p = (unsigned char *)tv;
			p[0] = saddr[0];
			p[1] = saddr[1];
			p[2] = saddr[2];
			p[3] = saddr[3];
			p[4] = saddr[4];
			p[5] = saddr[5];
			p[6] = saddr[6];
			p[7] = saddr[7];
			saddr += 8;
			send2(tv[0], tv[1]);
		}
		assert(saddr == eaddr);
	}
	RED_OFF(RED_XMIT);
}


mcmsg_fifo_out_eod(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long m;

	RED_ON(RED_XMIT);
	assert(n <= 4096);
	if ((addr & 7) == 0) {
#if	ENABLE_LTU
		m = (LTU_ALIGN-1) & -addr;
		if (n >= m && n - m >= LTU_ALIGN) {
			register unsigned long *saddr = (unsigned long *) addr;

			n -= m;
			if (m) {
				mcmsg_data_out_fifo += m;
				do {
					/* Don't try this at home */
					/*send2(*saddr++, *saddr++);*/
					send2(saddr[0], saddr[1]);
					saddr += 2;
					m -= 8;
				} while (m);
			}

			m = n & (LTU_ALIGN-1);
			n &= ~(LTU_ALIGN-1);
			mcmsg_data_out_ltu += n;
			if (m == 0) {
				mcmsg_ltu_sendeod(saddr, n);
			} else {
				mcmsg_ltu_send(saddr, n);

				mcmsg_data_out_fifo += m;
				m -= 8;
				saddr = (unsigned long *)
				 ((unsigned long)saddr + n);
				while (m) {
					/* Don't try this at home */
					/*send2(*saddr++, *saddr++);*/
					send2(saddr[0], saddr[1]);
					saddr += 2;
					m -= 8;
				}
				/* Don't try this at home */
				/*send2eod(*saddr++, *saddr++);*/
				send2eod(saddr[0], saddr[1]);
				saddr += 2;
			}

		} else
#endif	ENABLE_LTU
		{
			register unsigned long *saddr = (unsigned long *) addr;
			register unsigned long *eaddr = (unsigned long *)
							(addr + n - 8);

			mcmsg_data_out_fifo += n;
			while (saddr < eaddr) {
				/* Don't try this at home */
				/*send2(*saddr++, *saddr++);*/
				send2(saddr[0], saddr[1]);
				saddr += 2;
			}
			assert(saddr == eaddr);
			/* Don't try this at home */
			/*send2eod(*saddr++, *saddr++);*/
			send2eod(saddr[0], saddr[1]);
			saddr += 2;
		}

	} else {
		unsigned long tv[2];
		register unsigned char *saddr = (unsigned char *)addr;
		register unsigned char *eaddr = (unsigned char *)(addr + n - 8);
		register unsigned char *p;

		mcmsg_data_out_fifo += n;
		while (saddr < eaddr) {
			p = (unsigned char *)tv;
			p[0] = saddr[0];
			p[1] = saddr[1];
			p[2] = saddr[2];
			p[3] = saddr[3];
			p[4] = saddr[4];
			p[5] = saddr[5];
			p[6] = saddr[6];
			p[7] = saddr[7];
			saddr += 8;
			send2(tv[0], tv[1]);
		}
		assert(saddr == eaddr);
		p = (unsigned char *)tv;
		p[0] = saddr[0];
		p[1] = saddr[1];
		p[2] = saddr[2];
		p[3] = saddr[3];
		p[4] = saddr[4];
		p[5] = saddr[5];
		p[6] = saddr[6];
		p[7] = saddr[7];
		saddr += 8;
		send2eod(tv[0], tv[1]);
	}
	RED_OFF(RED_XMIT);
}

mcmsg_fifo_in(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned char	*p;
	register unsigned char	*q;
	register unsigned long m;

	RED_ON(RED_RECV);
	assert(n <= 4096);
	if ((addr & 7) == 0) {

#if	ENABLE_LTU
		m = (LTU_ALIGN-1) & -addr;
		if (n >= m && n - m >= LTU_ALIGN) {
			register unsigned long *saddr = (unsigned long *) addr;

			n -= m;
			if (m) {
				mcmsg_data_in_fifo += m;
				do {
					/* Don't try this at home */
					/*recv2(*saddr++, *saddr++);*/
					recv2(saddr[0], saddr[1]);
					saddr += 2;
					m -= 8;
				} while (m);
			}

			m = n & (LTU_ALIGN-1);
			n &= ~(LTU_ALIGN-1);
			mcmsg_data_in_ltu += n;
			mcmsg_ltu_recv(saddr, n);

			if (m) {
				saddr = (unsigned long *)
				 ((unsigned long)saddr + n);
				mcmsg_data_in_fifo += m;
				while (m >= 8) {
					/* Don't try this at home */
					/*recv2(*saddr++, *saddr++);*/
					recv2(saddr[0], saddr[1]);
					saddr += 2;
					m -= 8;
				}
				if (m > 0) {
					unsigned long last[2];

					recv2(last[0], last[1]);
					p = (unsigned char *)last;
					q = (unsigned char *)saddr;
					while (m--) {
						*q++ = *p++;
					}
				}
			}
		} else
#endif	ENABLE_LTU
		{
			register unsigned long	*dp = (unsigned long *) addr;

			mcmsg_data_in_fifo += n;
			while (n >= 8) {
				/* Don't try this at home */
				/*recv2(*dp++, *dp++);*/
				recv2(dp[0], dp[1]);
				dp += 2;
				n -= 8;
			}
			if (n > 0) {
				unsigned long last[2];

				recv2(last[0], last[1]);
				p = (unsigned char *)last;
				q = (unsigned char *)dp;
				while (n--) {
					*q++ = *p++;
				}
			}
		}

	} else {
		unsigned long tv[2];
		register unsigned char	*dp = (unsigned char *) addr;

		mcmsg_data_in_fifo += n;
		while (n >= 8) {
			recv2(tv[0], tv[1]);
			p = (unsigned char *)tv;
			dp[0] = p[0];
			dp[1] = p[1];
			dp[2] = p[2];
			dp[3] = p[3];
			dp[4] = p[4];
			dp[5] = p[5];
			dp[6] = p[6];
			dp[7] = p[7];
			dp += 8;
			n -= 8;
		}
		if (n > 0) {
			recv2(tv[0], tv[1]);
			p = (unsigned char *)tv;
			while (n--) {
				*dp++ = *p++;
			}
		}
	}
	RED_OFF(RED_RECV);
}

#else PARAGON860

mcmsg_fifo_code()
{
asm("									");
asm("fifo =		0xFFFF0000					");
asm("eod =		0xFFFF1000					");
asm("//counter_reg =	0xFFFF4000					");
asm("									");
asm("//*****************************************************************");
asm("//									");
asm("//	Calling Sequence:						");
asm("//	      mcmsg_fifo_out(buf, count);				");
asm("//									");
asm("//	Description:							");
asm("//	      Copy to send FIFO.					");
asm("//									");
asm("//	Parameters:							");
asm("//	      buf:	Area to copy from				");
asm("//	      count:	Byte count					");
asm("//									");
asm("//	Returns:							");
asm("//	      none							");
asm("//									");
asm("//*****************************************************************");
asm("									");
asm("_mcmsg_fifo_out::							");
asm("	orh	ha%fifo, r0, r31	// Address of FIFO in r31	");
asm("	and	7, r16, r0		// Check alignment		");
asm("	bnc	_fifoout4		// Br if misaligned		");
asm("fo41:	adds	-8, r16, r16		// Adjust buffer pointer");
asm("	adds	-32, r17, r27		// Fix count for bla and test	");
asm("	shl	27, r17, r28		// 5 LSB of count in r28 MSB	");
asm("	bc	fo2			// Br if count < 32		");
asm("	adds	-32, r0, r26		// Constant decrement in r26	");
asm("	bla	r26, r27, fo1		// Initial bla to prime loop	");
asm("	 pfld.d	8(r16)++, f0		// Get 8 words from buffer	");
asm("fo1:									");
asm("									");
asm("//orh	ha%counter_reg, r0, r30					");
asm("//fld.d	l%counter_reg(r30), f16					");
asm("//orh	ha%_testinfo, r0, r30					");
asm("//fst.d	f16, l%_testinfo+8(r30)					");
asm("//st.l	r17, l%_testinfo(r30)					");
asm("									");
asm("	pfld.d	8(r16)++, f0		//				");
asm("	pfld.d	8(r16)++, f0		//				");
asm("	bla	r26, r27, fo1b		// branch if not end of loop	");
asm("	 pfld.d	8(r16)++, f20		//				");
asm("fo1a:	fst.l	f20, l%fifo(r31)	// Put 8 words in FIFO	");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("	pfld.l	0(sp), f22		//				");
asm("	pfld.l	0(sp), f24		//				");
asm("	pfld.l	0(sp), f26		//				");
asm("	fst.l	f22, l%fifo(r31)	//				");
asm("	fst.l	f23, l%fifo(r31)	//				");
asm("	fst.l	f24, l%fifo(r31)	//				");
asm("	fst.l	f25, l%fifo(r31)	//				");
asm("	fst.l	f26, l%fifo(r31)	//				");
asm("	fst.l	f27, l%fifo(r31)	//				");
asm("									");
asm("//orh	ha%counter_reg, r0, r30					");
asm("//fld.d	l%counter_reg(r30), f16					");
asm("//orh	ha%_testinfo, r0, r30					");
asm("//fld.d	l%_testinfo+8(r30), f18					");
asm("//fisub.dd	f16, f18, f16						");
asm("//fst.d	f16, l%_testinfo+8(r30)					");
asm("									");
asm("fo2:	addu	r28, r28, r28		// Shift MSB of r28 into CC");
asm("	bnc	fo3			// Br if bit 4 of count zero	");
asm("	fld.d	8(r16)++, f20		// Get 4 words from buffer	");
asm("	fld.d	8(r16)++, f22		//				");
asm("	fst.l	f20, l%fifo(r31)	// Put 4 words in FIFO		");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("	fst.l	f22, l%fifo(r31)	//				");
asm("	fst.l	f23, l%fifo(r31)	//				");
asm("									");
asm("fo3:	addu	r28, r28, r28		// Next MSB of r28 into CC");
asm("	bnc	fo4			// Br if bit 3 of count zero	");
asm("	fld.d	8(r16)++, f20		// Get 2 words from buffer	");
asm("	fst.l	f20, l%fifo(r31)	// Put 2 words in FIFO		");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("									");
asm("fo4:	addu	r28, r28, r28		// Next MSB of r28 into CC");
asm("	bnc	fo5			// Br if bit 2 of count zero	");
asm("	fld.l	8(r16), f20		// Get word from buffer		");
asm("	fst.l	f20, l%fifo(r31)	// Put word in FIFO		");
asm("	adds	4, r16, r16		// Adjust buffer pointer	");
asm("									");
asm("fo5:	btne	r0, r28, fo7		// Br if bits 1,0 nonzero");
asm("fo6:								");
asm("	bri	r1			// Return			");
asm("	 nop								");
asm("									");
asm("fo7:	fld.l	8(r16), f20		// Get word from buffer	");
asm("	bri	r1			// Return			");
asm("	 fst.l	f20, l%fifo(r31)	// Put word in FIFO		");
asm("									");
asm("fo1b:	fst.l	f20, l%fifo(r31)	// Put 2 words in FIFO	");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("	pfld.d	8(r16)++, f22		// Get 8 words from buffer	");
asm("	pfld.d	8(r16)++, f24		//				");
asm("	pfld.d	8(r16)++, f26		//				");
asm("	pfld.d	8(r16)++, f20		//				");
asm("	fst.l	f22, l%fifo(r31)	// Put 6 words in FIFO		");
asm("	fst.l	f23, l%fifo(r31)	//				");
asm("	fst.l	f24, l%fifo(r31)	//				");
asm("	fst.l	f25, l%fifo(r31)	//				");
asm("	fst.l	f26, l%fifo(r31)	//				");
asm("	bla	r26, r27, fo1b		// Loop				");
asm("	 fst.l	f27, l%fifo(r31)	//				");
asm("	br	fo1a			// Branch			");
asm("	 nop				//				");
asm("									");
asm("_fifoout4::							");
asm("	and	3, r16, r0		// Check alignment		");
asm("	bnc	_fifoout1		// Br if misaligned		");
asm("	fld.l	0(r16), f16		// Load one word to align	");
asm("	fst.l	f16, l%fifo(r31)	// Store in FIFO		");
asm("	adds	4, r16, r16		// Increment buffer pointer	");
asm("	br	fo41			// Go back to main line		");
asm("	 adds	-4, r17, r17		// Decrement count		");
asm("									");
asm("_fifoout1::							");
asm("	adds	-4, r17, r27		// Fix count for bla, test	");
asm("	bc	fo12			// Br if count < 4		");
asm("	adds	-4, r0, r26		// Constant decrement in r26	");
asm("	bla	r26, r27, fo11		// Initial bla to prime loop	");
asm("	 shr	8, r0, r0		// Set SC			");
asm("fo11:	ld.b	0(r16), r20		// Get 4 bytes from buffer");
asm("	shrd	r20, r0, r21		// shift			");
asm("	ld.b	1(r16), r20		// load next byte		");
asm("	shrd	r20, r21, r21		//				");
asm("	ld.b	2(r16), r20		//				");
asm("	shrd	r20, r21, r21		//				");
asm("	ld.b	3(r16), r20		//				");
asm("	shrd	r20, r21, r21		//				");
asm("	st.l	r21, l%fifo(r31)	// Put 1 word in FIFO		");
asm("	bla	r26, r27, fo11		// Loop				");
asm("	 subs	r16, r26, r16		// Adjust buffer pointer	");
asm("									");
asm("fo12:	and	3, r17, r0		// Check 2 LSB of count	");
asm("	bc	fo15			// Br if no more to transfer	");
asm("	and	2, r17, r0		// Check NLSB of count		");
asm("	bc	fo13			// Br if only 1 byte to go	");
asm("	ld.b	1(r16), r21		// Load a byte			");
asm("	shl	8, r21, r21		// Shift left one byte		");
asm("	ld.b	0(r16), r20		// Load a byte			");
asm("	and	0xFF, r20, r20		// Mask				");
asm("	or	r20, r21, r21		// and combine			");
asm("	and	1, r17, r0		// Check LSB of count		");
asm("	bc	fo14			// Br if just 2 bytes		");
asm("	ld.b	2(r16), r20		// Load a byte			");
asm("	shl	16, r20, r20		// Position it			");
asm("	and	0xFFFF, r21, r21	// Mask low 16 bits		");
asm("	br	fo14			// Jump to end			");
asm("	 or	r20, r21, r21		// Combine			");
asm("									");
asm("fo13:	ld.b	0(r16), r21		// Load a byte		");
asm("fo14:	bri	r1			// Return		");
asm("	 st.l	r21, l%fifo(r31)	// Put last word in fifo	");
asm("fo15:								");
asm("	bri	r1			// Return			");
asm("	 nop				//				");
asm("									");
asm("//*****************************************************************");
asm("//									");
asm("//	Calling Sequence:						");
asm("//	      mcmsg_fifo_out_eod(buf, count);				");
asm("//									");
asm("//	Description:							");
asm("//	      Copy to send FIFO with EOD.				");
asm("//									");
asm("//	Parameters:							");
asm("//	      buf:	Area to copy from				");
asm("//	      count:	Byte count					");
asm("//									");
asm("//	Returns:							");
asm("//	      none							");
asm("//									");
asm("//*****************************************************************");
asm("									");
asm("_mcmsg_fifo_out_eod::						");
asm("	orh	ha%fifo, r0, r31	// Address of FIFO in r31	");
asm("	and	7, r16, r0		// Check alignment		");
asm("	bnc	_fifoeod4		// Br if misaligned		");
asm("fe41:	adds	-1, r17, r17		// Take one word from count,");
asm(" 	andnot	3, r17, r17		// round to even number words	");
asm(" 	bc	fe6			// Br if only one word to go	");
asm("	adds	-8, r16, r16		// Adjust buffer pointer	");
asm("	adds	-32, r17, r27		// Fix count for bla and test	");
asm("	shl	27, r17, r28		// 5 LSB of count in MSB r28	");
asm("	bc	fe2			// Br if count < 32		");
asm("	adds	-32, r0, r26		// Constant decrement in r26	");
asm("	bla	r26, r27, fe1		// Initial bla to prime loop	");
asm("	 pfld.d	8(r16)++, f0		// Get 8 words from buffer	");
asm("fe1:								");
asm("									");
asm("//orh	ha%counter_reg, r0, r30					");
asm("//fld.d	l%counter_reg(r30), f16					");
asm("//orh	ha%_testinfo, r0, r30					");
asm("//fst.d	f16, l%_testinfo+8(r30)					");
asm("//st.l	r17, l%_testinfo(r30)					");
asm("									");
asm("	pfld.d	8(r16)++, f0		//				");
asm("	pfld.d	8(r16)++, f0		//				");
asm("	bla	r26, r27, fe1b		// branch if not end of loop	");
asm("	 pfld.d	8(r16)++, f20		//				");
asm("fe1a:	fst.l	f20, l%fifo(r31)	// Put 8 words in FIFO	");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("	pfld.l	0(sp), f22		//				");
asm("	pfld.l	0(sp), f24		//				");
asm("	pfld.l	0(sp), f26		//				");
asm("	fst.l	f22, l%fifo(r31)	//				");
asm("	fst.l	f23, l%fifo(r31)	//				");
asm("	fst.l	f24, l%fifo(r31)	//				");
asm("	fst.l	f25, l%fifo(r31)	//				");
asm("	fst.l	f26, l%fifo(r31)	//				");
asm("	fst.l	f27, l%fifo(r31)	//				");
asm("									");
asm("//orh	ha%counter_reg, r0, r30					");
asm("//fld.d	l%counter_reg(r30), f16					");
asm("//orh	ha%_testinfo, r0, r30					");
asm("//fld.d	l%_testinfo+8(r30), f18					");
asm("//fisub.dd	f16, f18, f16						");
asm("//fst.d	f16, l%_testinfo+8(r30)					");
asm("									");
asm("fe2:	addu	r28, r28, r28		// Shift MSB of r28 into CC");
asm("	bnc	fe3			// Br if bit 4 of count zero	");
asm("	fld.d	8(r16)++, f20		// Get 4 words from buffer	");
asm("	fld.d	8(r16)++, f22		//				");
asm("	fst.l	f20, l%fifo(r31)	// Put 4 words in FIFO		");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("	fst.l	f22, l%fifo(r31)	//				");
asm("	fst.l	f23, l%fifo(r31)	//				");
asm("									");
asm("fe3:	addu	r28, r28, r28		// Next MSB of r28 in CC");
asm("	bnc	fe4			// Br if bit 3 of count zero	");
asm("	fld.d	8(r16)++, f20		// Get 2 words from buffer	");
asm("	fst.l	f20, l%fifo(r31)	// Put 2 words in FIFO		");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("									");
asm("fe4:	addu	r28, r28, r28		// Next MSB of r28 into CC");
asm("	bnc	fe5			// Br if bit 2 of count zero	");
asm("	fld.l	8(r16), f20		// Get word from buffer		");
asm("	fst.l	f20, l%fifo(r31)	// Put word in FIFO		");
asm("	adds	4, r16, r16		// Adjust buffer pointer	");
asm("									");
asm("fe5:	ld.l	 8(r16), r20		// Get word from buffer	");
asm("	bri	r1			// Return			");
asm("	 st.l	r20, l%eod(r31)		// Put word in FIFO with EOD	");
asm("									");
asm("fe1b:	fst.l	f20, l%fifo(r31)	// Put 2 words in FIFO	");
asm("	fst.l	f21, l%fifo(r31)	//				");
asm("	pfld.d	8(r16)++, f22		// Get 8 words from buffer	");
asm("	pfld.d	8(r16)++, f24		//				");
asm("	pfld.d	8(r16)++, f26		//				");
asm("	pfld.d	8(r16)++, f20		//				");
asm("	fst.l	f22, l%fifo(r31)	// Put 6 words in FIFO		");
asm("	fst.l	f23, l%fifo(r31)	//				");
asm("	fst.l	f24, l%fifo(r31)	//				");
asm("	fst.l	f25, l%fifo(r31)	//				");
asm("	fst.l	f26, l%fifo(r31)	//				");
asm("	bla	r26, r27, fe1b		// Loop				");
asm("	 fst.l	f27, l%fifo(r31)	//				");
asm("	br	fe1a			// Branch			");
asm("	 nop				//				");
asm("									");
asm("_fifoeod4::							");
asm("	and	3, r16, r0		// Check alignment		");
asm("	bnc	_fifoeod1		// Br if misaligned		");
asm(" 	adds	-5, r17, r0		// Test count			");
asm("	bc	fe6			// Br if count <= 4		");
asm("	ld.l	0(r16), r20		// Load one word to align	");
asm("	st.l	r20, l%fifo(r31)	// Store in FIFO		");
asm("	adds	4, r16, r16		// Increment buffer pointer	");
asm("	br	fe41			// Go back to main line		");
asm("	 adds	-4, r17, r17		// Decrement count		");
asm("									");
asm("fe6:	ld.l	 0(r16), r20		// Get word from buffer	");
asm("	bri	r1			// Return			");
asm("	 st.l	r20, l%eod(r31)		// Put word in FIFO with EOD	");
asm("									");
asm("_fifoeod1::							");
asm(" 	adds	-5, r17, r27		// Fix count for bla and test	");
asm("	bc	fe12			// Br if count <= 4,		");
asm(" 	andnot	3, r27, r27		// round to even num words	");
asm("	adds	-4, r0, r26		// Constant decrement in r26	");
asm("	bla	r26, r27, fe11		// Initial bla to prime loop	");
asm("	 shr	8, r0, r0		// Set SC			");
asm("fe11:	ld.b	0(r16), r20		// Get 4 bytes from buffer");
asm("	shrd	r20, r0, r21		// shift			");
asm("	ld.b	1(r16), r20		// load next byte		");
asm("	shrd	r20, r21, r21		//				");
asm("	ld.b	2(r16), r20		//				");
asm("	shrd	r20, r21, r21		//				");
asm("	ld.b	3(r16), r20		//				");
asm("	shrd	r20, r21, r21		//				");
asm("	st.l	r21, l%fifo(r31)	// Put 1 word in FIFO		");
asm("	bla	r26, r27, fe11		// Loop				");
asm("	 subs	r16, r26, r16		// Adjust buffer pointer	");
asm("									");
asm("									");
asm("fe12:	and	3, r17, r0		// Check NLSB of count	");
asm("	bnc	fe13			// Br if < 4 bytes to go	");
asm("	shr	8, r0, r0		// Load SC			");
asm("	ld.b	0(r16), r20		// Get 4 bytes from buffer	");
asm("	shrd	r20, r0, r21		// shift			");
asm("	ld.b	1(r16), r20		// load next byte		");
asm("	shrd	r20, r21, r21		//				");
asm("	ld.b	2(r16), r20		//				");
asm("	shrd	r20, r21, r21		//				");
asm("	ld.b	3(r16), r20		//				");
asm("	shrd	r20, r21, r21		//				");
asm("	bri	r1			// Return			");
asm("	 st.l	r21, l%eod(r31)		// Put 1 word in FIFO with EOD	");
asm("									");
asm("fe13:	and	2, r17, r0		// Check NLSB of count	");
asm("	bc	fe14			// Br if only 1 byte to go	");
asm("	ld.b	1(r16), r21		// Load a byte			");
asm("	shl	8, r21, r21		// Shift left one byte		");
asm("	ld.b	0(r16), r20		// Load a byte			");
asm("	and	0xFF, r20, r20		// Mask				");
asm("	or	r20, r21, r21		// and combine			");
asm("	and	1, r17, r0		// Check LSB of count		");
asm("	bc	fe15			// Br if just 2 bytes		");
asm("	ld.b	2(r16), r20		// Load a byte			");
asm("	shl	16, r20, r20		// Position it			");
asm("	and	0xFFFF, r21, r21	// Mask low 16 bits		");
asm("	br	fe15			// Jump to end			");
asm("	 or	r20, r21, r21		// Combine			");
asm("									");
asm("fe14:	ld.b	0(r16), r21		// Load a byte		");
asm("fe15:								");
asm("	bri	r1			// Return			");
asm("	 st.l	r21, l%eod(r31)		// 1 word in FIFO with EOD	");
asm("									");
asm("//*****************************************************************");
asm("//									");
asm("//	Calling Sequence:						");
asm("//	      mcmsg_fifo_in(buf, count);				");
asm("//									");
asm("//	Description:							");
asm("//	      Copy from receive FIFO.					");
asm("//									");
asm("//	Parameters:							");
asm("//	      buf:	Area to copy into				");
asm("//	      count:	Byte count					");
asm("//									");
asm("//	Returns:							");
asm("//	      none							");
asm("//									");
asm("//*****************************************************************");
asm("									");
asm("_mcmsg_fifo_in::							");
asm("	orh	ha%fifo, r0, r31	// Address of FIFO in r31	");
asm("	and	7, r16, r0		// Check alignment		");
asm("	bnc	_fifoin4		// Br if misaligned		");
asm("fi41:								");
asm("	adds	-8, r16, r16		// Precondition buffer ptr	");
asm("	adds	-32, r17, r27		// Fix count for bla and test	");
asm("	shl	27, r17, r28		// 5 LSB of count in MSB r28	");
asm("	bc	fi2			// Br if count < 32		");
asm("	adds	-32, r0, r26		// Constant decrement in r26	");
asm("	bla	r26, r27, fi1		// Initial bla to prime loop	");
asm("	 pfld.l	l%fifo(r31), f16	// Get 8 words from FIFO	");
asm("fi1:								");
asm("									");
asm("//orh	ha%counter_reg, r0, r30					");
asm("//fld.d	l%counter_reg(r30), f16					");
asm("//orh	ha%_testinfo, r0, r30					");
asm("//fst.d	f16, l%_testinfo+8(r30)					");
asm("//st.l	r17, l%_testinfo(r30)					");
asm("									");
asm("	pfld.l	l%fifo(r31), f18	//				");
asm("	pfld.l	l%fifo(r31), f28	//				");
asm("	pfld.l	l%fifo(r31), f20	//				");
asm("	pfld.l	l%fifo(r31), f21	//				");
asm("	pfld.l	l%fifo(r31), f22	//				");
asm("	bla	r26, r27, fi1b		//				");
asm("	 pfld.l	l%fifo(r31), f23	//				");
asm("									");
asm("	pfld.l	l%fifo(r31), f24	//				");
asm("fi1a:								");
asm("	pfld.l	0(sp), f25		//				");
asm("	pfld.l	0(sp), f26		//				");
asm("	pfld.l	0(sp), f27		//				");
asm("	fst.d	f20, 8(r16)++		// Put 8 words in buffer	");
asm("	fst.d	f22, 8(r16)++		//				");
asm("	fst.d	f24, 8(r16)++		//				");
asm("	fst.d	f26, 8(r16)++		//				");
asm("									");
asm("//orh	ha%counter_reg, r0, r30					");
asm("//fld.d	l%counter_reg(r30), f16					");
asm("//orh	ha%_testinfo, r0, r30					");
asm("//fld.d	l%_testinfo+8(r30), f18					");
asm("//fisub.dd	f16, f18, f16						");
asm("//fst.d	f16, l%_testinfo+8(r30)					");
asm("									");
asm("									");
asm("fi2:	addu	r28, r28, r28		// Shift MSB of r28 into CC");
asm("	bnc	fi3			// Br if bit 4 of count zero	");
asm("	fld.l	l%fifo(r31), f20	// Get 4 words from FIFO	");
asm("	fld.l	l%fifo(r31), f21	//				");
asm("	fld.l	l%fifo(r31), f22	//				");
asm("	fld.l	l%fifo(r31), f23	//				");
asm("	fst.d	f20, 8(r16)++		// Put 4 words in buffer	");
asm("	fst.d	f22, 8(r16)++		//				");
asm("									");
asm("fi3:	addu	r28, r28, r28		// Next MSB of r28 into CC");
asm("	bnc	fi4			// Br if bit 3 of count zero	");
asm("	fld.l	l%fifo(r31), f20	// Get 2 words from FIFO	");
asm("	fld.l	l%fifo(r31), f21	//				");
asm("	fst.d	f20, 8(r16)++		// Put 2 words in buffer	");
asm("									");
asm("fi4:	addu	r28, r28, r28		// Next MSB of r28 into CC");
asm("	bnc	fi5			// Br if bit 2 of count zero	");
asm("	fld.l	l%fifo(r31), f20	// Get word from FIFO		");
asm("	fst.l	f20, 8(r16)		// Put word in buffer		");
asm("	adds	4, r16, r16		// Adjust buffer pointer	");
asm("									");
asm("fi5:	btne	r0, r28, fi7		// Br if bits 1,0 nonzero");
asm("fi6:	bri	r1			// Return		");
asm("	 nop				//				");
asm("									");
asm("fi7:	addu	r28, r28, r28		// Next MSB of r28 into CC");
asm("	ld.l	l%fifo(r31), r20	// Get one word from fifo	");
asm("	bnc	fi8			// Br if bit 1 of count zero	");
asm("	st.s	r20, 8(r16)		// Put 2 bytes in buffer	");
asm("	bte	r0, r28, fi6		// Br if count complete		");
asm("	shr	16, r20, r20		// Shift upper byte down	");
asm("	bri	r1			// Return			");
asm("	 st.b	r20, 10(r16)		// Put last byte in buffer	");
asm("									");
asm("fi8:	bri	r1			// Return		");
asm("	 st.b	r20, 8(r16)		// Put last byte in buffer	");
asm("									");
asm("fi1b:								");
asm("	pfld.l	l%fifo(r31), f24	//				");
asm("	pfld.l	l%fifo(r31), f25	//				");
asm("	pfld.l	l%fifo(r31), f26	//				");
asm("	pfld.l	l%fifo(r31), f27	//				");
asm("	fst.d	f20, 8(r16)++		// Put 8 words in buffer	");
asm("	fst.d	f22, 8(r16)++		//				");
asm("	fst.d	f24, 8(r16)++		//				");
asm("	fst.d	f26, 8(r16)++		//				");
asm("	pfld.l	l%fifo(r31), f20	// Get 8 words from fifo	");
asm("	pfld.l	l%fifo(r31), f21	//				");
asm("	pfld.l	l%fifo(r31), f22	//				");
asm("	bla	r26, r27, fi1b		// Loop				");
asm("	 pfld.l	l%fifo(r31), f23	//				");
asm("	br	fi1a			// Branch			");
asm("	 pfld.l	l%fifo(r31), f24	//				");
asm("									");
asm("_fifoin4::								");
asm("	and	3, r16, r0		// Check alignment		");
asm("	bnc	_fifoin1		// Br if misaligned		");
asm("	adds	-4, r17, r0		// Test count			");
asm("	bc	fi12			// Br if count < 4		");
asm("	ld.l	l%fifo(r31), r20	// Get word from fifo		");
asm("	st.l	r20, 0(r16)		// Put word in buffer		");
asm("	adds	4, r16, r16		// Realign buffer pointer	");
asm("	br	fi41			// Branch to main line		");
asm("	 adds	-4, r17, r17		// Fix count			");
asm("									");
asm("_fifoin1::								");
asm("	orh	ha%fifo, r0, r31	// Address of FIFO in r31	");
asm("	adds	-4, r17, r27		// Fix count for bla and test	");
asm("	bc	fi12			// Br if count < 4		");
asm("	adds	-4, r0, r26		// Constant decrement in r26	");
asm("	bla	r26, r27, fi11		// Initial bla to prime loop	");
asm("	 nop				//				");
asm("fi11:	ld.l	l%fifo(r31), r20	// Get 1 word from FIFO	");
asm("	st.b	r20,  0(r16)		// Put 4 bytes in buffer	");
asm("	shr	8, r20, r20		// shift			");
asm("	st.b	r20,  1(r16)		// store next byte		");
asm("	shr	8, r20, r20		//				");
asm("	st.b	r20,  2(r16)		//				");
asm("	shr	8, r20, r20		//				");
asm("	st.b	r20,  3(r16)		//				");
asm("	bla	r26, r27, fi11		// Loop				");
asm("	 subs	r16, r26, r16		// Adjust buffer pointer	");
asm("									");
asm("fi12:	and	3, r17, r27		// Expose 2 LSB of count");
asm("	bc	fi14			// Br if no more to go		");
asm("	adds	-1, r27, r27		// Adjust count			");
asm("	ld.l	l%fifo(r31), r20	// Get last word from FIFO	");
asm("fi13:	st.b	r20, 0(r16)		// Store a byte		");
asm("	adds	-1, r27, r27		// Count			");
asm("	shr	8, r20, r20		// Shift			");
asm("	bnc.t	fi13			// Br if not through		");
asm("	 adds	1, r16, r16		// Adjust buffer pointer	");
asm("fi14:	bri	r1			// Return		");
asm("	 nop				//				");
}
#endif PARAGON860

mcmsg_fifo_flush(n)
	unsigned long	n;
{
	unsigned long	x;
	unsigned long	y;
	unsigned long	t;

#if	iPSC860
	n = (n+3) & ~3;
#else	iPSC860
	n = (n+7) & ~7;
#endif	iPSC860

	assert((t = 1000000) != 0);
	mcmsg_data_flush += n;
	while (n) {

#if	iPSC860
		while ((STATUS_REG & RECV_SOME) == 0) {
			assert(t-- != 0);
		}
		x += RECV_FIFO;
		n -= 4;
#else	iPSC860
#if	BUMPERS
#else	BUMPERS
		while (!mcmsg_recv_ready()) {
			assert(t-- != 0);
		}
#endif	BUMPERS
		recv2(x, y);
		n -= 8;
#endif	iPSC860
	}
	assert(n == 0);
	return t;
}

/*
 *  Disable the transmit interrupts.
 */

void
mcmsg_disable_tx_interrupts()
{
	int	s;

#if	iPSC860

	soft_pic_disable(SEND_INT_MASK);
	s = sploff();
	CLR_CONTROL(SEND_INT_MASK);
	splon(s);

#else	iPSC860

	{
		nic_reg	t;

		t.halfs.lo = mcmsg_send_enable;
		t.halfs.hi = 0;
		NIC.clear.full = t.full;
	}

#endif	iPSC860

	mcmsg_int_enables &= ~1;
}


/*
 * Enable the transmit interrupts.
 * (Actually enabled when SPL set appropriately)
 *
 */

void
mcmsg_enable_tx_interrupts()
{
	int	s;

#if	iPSC860

	soft_pic_enable(SEND_INT_MASK);

#else	iPSC860

	{
		nic_reg	t;

		t.halfs.lo = mcmsg_send_enable;
		t.halfs.hi = 0;
		NIC.set.full = t.full;
	}


#endif	iPSC860

	mcmsg_int_enables |= 1;
}

/*
 *  Disable the receive interrupts.
 */

void
mcmsg_disable_rx_interrupts()
{
	int	s;

#if	iPSC860

	soft_pic_disable(RECV_INT_MASK | EOD_IN_INT_MASK);
	s = sploff();
	CLR_CONTROL(RECV_INT_MASK | EOD_IN_INT_MASK);
	splon(s);

#else	iPSC860

	{
		nic_reg	t;

#if	BUMPERS
		t.halfs.lo = mcmsg_recv_enable;
		mcmsg_recv_enable = 0;
#else	BUMPERS
		t.halfs.lo = NIC_CNTRL_EN_EOD_IN_NIC;
#endif	BUMPERS
		t.halfs.hi = 0;
		NIC.clear.full = t.full;
	}

	RED_ON(RED_BLOCK);

#endif	iPSC860

	mcmsg_int_enables &= ~2;
}

/*
 * Enable the receive interrupts.
 * (Actually enabled when SPL set appropriately)
 */

void
mcmsg_enable_rx_interrupts()
{
	int	s;

#if	iPSC860

	soft_pic_enable(RECV_INT_MASK | EOD_IN_INT_MASK);

#else	iPSC860

	{
		nic_reg	t;

#if	BUMPERS
		if (mcmsg_recv_loaded) {
			t.halfs.lo = NIC_CNTRL_RX_LOW | NIC_CNTRL_ERRORS;
		} else {
			t.halfs.lo = NIC_CNTRL_EN_EOD_IN_NIC | NIC_CNTRL_ERRORS;
		}
		mcmsg_recv_enable = t.halfs.lo;
#else	BUMPERS
		t.halfs.lo = NIC_CNTRL_EN_EOD_IN_NIC;
#endif	BUMPERS
		t.halfs.hi = 0;
		NIC.set.full = t.full;
	}

	RED_ON(RED_BLOCK);

#endif	iPSC860

	mcmsg_int_enables |= 2;
}

#if	PARAGON860

mcmsg_send_ready()
{
	nic_reg	t;

	t.full = mcmsg_nic_status(1);
#if	BURST
	if ((t.halfs.lo & NIC_STAT_TX_FIFO_EMPTY) != 0) {
		t.halfs.hi = 0;
		t.halfs.lo = NIC_TEST_DISABLE_TX | NIC_TEST_RCV_AFULL_SEL;
		NIC.reset_test.full = t.full;
		return 1;
	}
	return 0;
#else	BURST
	return (t.halfs.lo & NIC_STAT_TX_FIFO_ALMOST_EMPTY) != 0;
#endif	BURST
}
#if	BUMPERS

send2eod_(a, b)
	long	a;
	long	b;
{
#if	MACH_ASSERT
	nic_reg	t;
	int	i;

	t.halfs.lo = a;
	t.halfs.hi = b;
	NIC.io.full = t.full;
	NIC_TRACE_SEND;
	if (mcmsg_send_byte_count % 16 == 8) {
		t.halfs.lo = 0xeeeeeeee;
		t.halfs.hi = 0xbabeeeee;
		NIC.io.full = t.full;
		NIC_TRACE_SEND;
	}
	for (i = 0; i < BUMPER-1; i++) {
		t.halfs.lo = i;
		t.halfs.hi = 0xbabefe9d;
		NIC.io.full = t.full;
		NIC_TRACE_SEND;
	}
	t.halfs.lo = i;
	t.halfs.hi = 0xbabefe9d;
	EOD.io.full = t.full;
	mcmsg_send_byte_count = 0;
	NIC_TRACE_SEND;
	mcmsg_nic_status(7);
	mcmsg_send_count++;
#if	BURST
	t.halfs.hi = 0;
	t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
	NIC.reset_test.full = t.full;
#endif	BURST

#else	MACH_ASSERT
	nic_reg	t;
	register double	x;

	t.halfs.lo = a;
	t.halfs.hi = b;
	NIC.io.full = t.full;
	if (mcmsg_send_byte_count % 16 == 8) {
		t.halfs.hi = 0xbabeeeee;
		NIC.io.full = t.full;
	}
	NIC.io.full = x;
	NIC.io.full = x;
	NIC.io.full = x;
	EOD.io.full = x;
	mcmsg_send_byte_count = 0;
	mcmsg_send_count++;
#if	BURST
	t.halfs.hi = 0;
	t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
	NIC.reset_test.full = t.full;
#endif	BURST
#endif	MACH_ASSERT
}

boot_send2eod(a, b)
	long	a;
	long	b;
{
	nic_reg	t;

	t.halfs.lo = a;
	t.halfs.hi = b;
	EOD.io.full = t.full;
	mcmsg_send_byte_count = 0;
	mcmsg_nic_status(7);
#if	BURST
	t.halfs.hi = 0;
	t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
	NIC.reset_test.full = t.full;
#endif	BURST
}

#if	MACH_ASSERT
unsigned long mcmsg_last_bumper_hi, mcmsg_last_bumper_lo;
#endif	MACH_ASSERT

mcmsg_recv_ready()
{
	nic_reg	t;
	nic_reg	e;
	register double	x;
	int	i;
#if	!MACH_ASSERT
	register unsigned long mcmsg_last_bumper_hi, mcmsg_last_bumper_lo;
#endif	MACH_ASSERT

	if ((mcmsg_int_enables & 2) == 0)
		return 0;
	t.full = mcmsg_nic_status(2);
	if (mcmsg_recv_loaded) {
		if ((t.halfs.lo & NIC_STAT_RX_LOW) == 0) {
			return 0;
		}
		recv2(mcmsg_last_bumper_lo, mcmsg_last_bumper_hi);
		if (mcmsg_last_bumper_hi == 0xbabeeeee) {
			return 0;
		}
		assert(mcmsg_last_bumper_lo == 0 &&
		       mcmsg_last_bumper_hi == 0xbabefe9d);
#if	MACH_ASSERT
		for (i = 1; i < BUMPER; i++) {
			recv2(mcmsg_last_bumper_lo, mcmsg_last_bumper_hi);
			assert(mcmsg_last_bumper_lo == i &&
			       mcmsg_last_bumper_hi == 0xbabefe9d);
		}
#else	MACH_ASSERT
		x = NIC.io.full;
		x = NIC.io.full;
		x = NIC.io.full;
#endif	MACH_ASSERT
		t.full = mcmsg_nic_status(3);
		assert((t.halfs.lo & NIC_STAT_EOD_IN_WORD) != 0);
		assert(mcmsg_recv_byte_count % 16 == 8);
		mcmsg_recv_loaded = 0;
		mcmsg_recv_byte_count = 0;
		if (mcmsg_recv_enable) {
			e.halfs.lo = mcmsg_recv_enable;
			e.halfs.hi = 0;
			NIC.clear.full = e.full;
			mcmsg_recv_enable = NIC_CNTRL_EN_EOD_IN_NIC |
					    NIC_CNTRL_ERRORS;
			e.halfs.lo = mcmsg_recv_enable;
			NIC.set.full = e.full;
		}
	}
	return (t.halfs.lo & NIC_STAT_EOD_IN_NIC) != 0;
}

mcmsg_eod_last()
{
	long	t;
	nic_reg	u;
	nic_reg	e;

	if (mcmsg_recv_loaded == 0) {
		mcmsg_recv_loaded = 1;
		if (mcmsg_recv_enable) {
			e.halfs.lo = mcmsg_recv_enable;
			e.halfs.hi = 0;
			NIC.clear.full = e.full;
			mcmsg_recv_enable = NIC_CNTRL_RX_LOW |
					    NIC_CNTRL_ERRORS;
			e.halfs.lo = mcmsg_recv_enable;
			NIC.set.full = e.full;
		}
	}
	mcmsg_recv_count++;

	u.full = mcmsg_nic_status(4);
	return ((u.halfs.lo & (NIC_STAT_EOD_IN_NIC | NIC_STAT_CAN_READ_1)) ==
	                      (NIC_STAT_EOD_IN_NIC | NIC_STAT_CAN_READ_1));
}

mcmsg_flush_invalid()
{
	unsigned long a, b;
	nic_reg	t;
	long	m;

	m = 1000000;
	for (;;) {
		t.full = mcmsg_nic_status(5);
		if ((t.halfs.lo & NIC_STAT_RX_LOW) == 0) {
			continue;
		}
		if ((t.halfs.lo & NIC_STAT_EOD_IN_WORD) != 0) {
			return;
		}
		recv2(a, b);
		mcmsg_trace_debug("invalid msg data", 2, a, b, 0, 0);
		if (--m == 0) {
			break;
		}
	}
	mcmsg_trace_debug("no EOD found", 0, 0, 0, 0, 0);
	assert_mcmsg_eod_last();
}


#else	BUMPERS

mcmsg_recv_ready()
{
	nic_reg	t;

	t.full = NIC.status.full;
	return (t.halfs.lo & NIC_STAT_EOD_IN_NIC) != 0;
}

mcmsg_eod_last()
{
	nic_reg t;

	t.full = NIC.status.full;
	return (t.halfs.lo & NIC_STAT_EOD_IN_WORD) != 0;
}

mcmsg_flush_invalid()
{
	unsigned long a, b;

	while (!mcmsg_eod_last()) {
		recv2(a, b);
		mcmsg_trace_debug("invalid msg data", 2, a, b, 0, 0);
	}
}

#endif	BUMPERS

#endif	PARAGON860
