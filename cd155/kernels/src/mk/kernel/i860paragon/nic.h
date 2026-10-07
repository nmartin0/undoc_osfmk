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
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * $Id: nic.h,v 0.10 1994/11/18 20:43:07 mtm Exp $
 */

/*
 *	NIC Address Space
 *
 *	NIC ASIC space accesses read or write the registers in the NIC.
 *
 *	This space is accessed when the most significant four bits of
 *	the address is equal to 4.
 *
 *	Address bits <5:3> specify the resources within the NIC that
 *	are accessed.
 *
 *	This space does not recognize burst transfers, therefore all
 *	accesses in NIC space must be non-cached or I/O.
 */
/*	w/r	a2 a1 a0
 *	  1	 0  0  0	set control register
 *	  1	 0  0  1	clear control register
 *	  0	 0  1  0	read control register
 *	  0	 0  1  1	read status register
 *	  0	 1  0  0	read receive register
 *	  1	 1  0  0	write transmit register
 *	  1	 1  0  1	write FIFO offset register
 *	  0	 1  0  1	read FIFO offset register
 *	  0	 1  1  0	unused read
 *	  1	 1  1  0	unused write
 *	  0	 1  1  1	hardware reset
 *	  1	 1  1  1	test register write
 */

/*
 *	NIC EOD Address Space
 *
 *	NIC EOD space accesses set the EOD bit on NIC writes.  All other
 *	accesses in this space are identical to NIC address space.
 *
 *	This space is accessed when the most significant four bits of
 *	the address is equal to 5.
 *
 *	Address bits <5:3> specify the resources within the NIC that
 *	are accessed.
 *
 *	This space does not recognize burst transfers, therefore all
 *	accesses in NIC space must be non-cached or I/O.
 */

/* Physical addresses */

#define NIC_ADDR_PH	0x40000000
#define EOD_ADDR_PH	0x50000000

/* Virtual address base for mapping IO */

#define IO_BASE		0xFFFF0000

#define NIC_ADDR	(IO_BASE + 0x0000)
#define EOD_ADDR	(IO_BASE + 0x1000)
#define NIC		(*(volatile struct nic *)(NIC_ADDR))
#define NIC_PH		(*(volatile struct nic *)(NIC_ADDR_PH))
#define EOD		(*(volatile struct nic *)(EOD_ADDR))

/* Addresses for DP ASIC (shouldn't be here) */
#define DP_ADDR_PH	0x70000000
#define DP_ADDR		(IO_BASE + 0x4000)

/*
 *  All references to NIC registers and FIFOs must be done 64 bits at a time.
 *  Since there are no "in/out double" instructions, we must have a non-cached
 *  PTE setup.  The structure below defines the memory mapped structure of the
 *  NIC.
 */

typedef union {
	double	full;
	struct {
		unsigned long	lo;
		unsigned long	hi;
	} halfs;
} nic_reg;

typedef union {
	double	s_64;
	unsigned long	s_32[ 2 ];
	unsigned short s_16[ 4 ];
	unsigned char	s_8 [ 8 ];
} nic_convert;

struct	nic {
	nic_reg	set;
	nic_reg	clear;
	nic_reg	control;
	nic_reg	status;
	nic_reg	io;
	nic_reg	fifo_offset;
	nic_reg	unused;
	nic_reg	reset_test;
};

/*
 *  Probe defines
 */

#define	NIC_BROADCAST	0x4000
#define	NIC_NORTH	0x0000
#define NIC_SOUTH	0x8000
#define	NIC_EAST	0x0000
#define	NIC_WEST	0x8000

/*
 *  NIC Status bit defines.
 */
#define	BIT(x)		(1 << (x & 31))
#define BITS(x,y)	((2 << (y & 31)) - (1 << (x & 31)))

/* ------------------- Lower 32 bits of status --------------------/
 * FIFO FLAGS
 */
#define	NIC_STAT_TX_FIFO_NOT_FULL		BIT(0)
#define	NIC_STAT_TX_FIFO_NOT_ALMOST_FULL	BIT(1)
#define	NIC_STAT_TX_FIFO_ALMOST_EMPTY		BIT(2)
#define	NIC_STAT_TX_FIFO_EMPTY			BIT(3)
#define	NIC_STAT_RX_FIFO_FULL			BIT(4)
#define	NIC_STAT_RX_FIFO_ALMOST_FULL		BIT(5)
#define	NIC_STAT_RX_FIFO_NOT_ALMOST_EMPTY	BIT(6)
#define	NIC_STAT_CAN_READ_2			BIT(7)
#define	NIC_STAT_CAN_READ_1			BIT(8)
#define NIC_STAT_RX_FIFO_NOT_EMPTY		BIT(9)

/* Processor port status */
#define	NIC_STAT_EOD_IN_NIC			BIT(10)
#define	NIC_STAT_RX_CHAN_RDY			BIT(11)
#define	NIC_STAT_TX_CHAN_RDY			BIT(12)
#define	NIC_STAT_EOD_IN_WORD			BIT(13)

/* Error status */
#define	NIC_STAT_NET_CRC0_ERR			BIT(14)
#define	NIC_STAT_NET_CRC1_ERR			BIT(15)
#define	NIC_STAT_PORT_PAR0_ERR			BIT(16)
#define	NIC_STAT_PORT_PAR1_ERR			BIT(17)
#define	NIC_STAT_PORT_PAR2_ERR			BIT(18)
#define	NIC_STAT_PORT_PAR3_ERR			BIT(19)
#define	NIC_STAT_PORT_PAR4_ERR			BIT(20)
#define	NIC_STAT_PORT_PAR5_ERR			BIT(21)
#define	NIC_STAT_PORT_PAR6_ERR			BIT(22)
#define	NIC_STAT_PORT_PAR7_ERR			BIT(23)
#define	NIC_STAT_NET_PAR0_ERR			BIT(24)
#define	NIC_STAT_NET_PAR1_ERR			BIT(25)
#define NIC_STAT_TX_FIFO_OVERRUN		BIT(26)
#define NIC_STAT_RX_FIFO_OVERRUN		BIT(27)
#define NIC_STAT_RX_FIFO_UNDERRUN		BIT(28)

/* Miscellaneous status */
#define	NIC_STAT_XREQ				BIT(29)
#define	NIC_STAT_XACK				BIT(30)
#define	NIC_STAT_RREQ				BIT(31)

/* ------------------- Upper 32 bits of status --------------------*/
#define	NIC_STAT_RACK				BIT(32)
#define	NIC_STAT_XMIP				BIT(33)
#define	NIC_STAT_RMIP				BIT(34)
#define	NIC_STAT_REV				BITS(35,37)
#define	NIC_STAT_TYPE				BITS(38,42)


/*
 *  NIC control bits.
 */

#define	NIC_CNTRL_EN_TX_NOT_FULL		BIT(0)
#define	NIC_CNTRL_EN_TX_NOT_ALMOST_FULL		BIT(1)
#define NIC_CNTRL_EN_TX_ALMOST_EMPTY		BIT(2)
#define	NIC_CNTRL_EN_TX_EMPTY			BIT(3)
#define	NIC_CNTRL_EN_RX_FULL			BIT(4)
#define NIC_CNTRL_EN_RX_ALMOST_FULL		BIT(5)
#define	NIC_CNTRL_EN_RX_NOT_ALMOST_EMPTY	BIT(6)
#define	NIC_CNTRL_EN_CAN_READ_2			BIT(7)
#define	NIC_CNTRL_EN_CAN_READ_1			BIT(8)
#define NIC_CNTRL_EN_EOD_IN_NIC			BIT(10)
#define NIC_CNTRL_EN_STREAM			BIT(47)
#define NIC_CNTRL_EN_SPEED			BIT(48)
#define	NIC_CNTRL_CLR_HW_ERR			BIT(50)
#define	NIC_CNTRL_EN_iMRC			BIT(52)
#define NIC_CNTRL_EN_ALMOST			BIT(53)
#define NIC_CNTRL_RESET				BIT(56)

/*
 *  NIC test bits.
 */

#define NIC_TEST_DISABLE_TX			BIT(0)
#define NIC_TEST_DISABLE_RX			BIT(1)
#define NIC_TEST_INHIBIT_CLEAR			BIT(2)
#define NIC_TEST_FORCE_RFIFO_RD			BIT(3)
#define NIC_TEST_RCV_AFULL_SEL			BIT(4)
#define NIC_TEST_ADVANCE_ALMOST			BIT(5)


#undef BITS

/*
 *  MACROS
 */
#define	rd_nic_cntrl(p)		((p)->full = nic_p->control.full)
#define	rd_nic_status(p)	((p)->full = nic_p->status.full)
#define	rd_nic_fifo_off(p)	((p)->full = nic_p->fifo_offset.full)
#define	set_nic_cntrl(p)	(nic_p->set.full = (p)->full)
#define	clr_nic_cntrl(p)	(nic_p->clear.full = (p)->full)
#define	wt_nic_fifo_off(p)	(nic_p->fifo_offset.full = (p)->full)

#define	NIC_RX_READY(p)	((p)->halfs.lo & (NIC_STAT_CAN_READ_1 | \
					  NIC_STAT_CAN_READ_2))

#define	NIC_TX_READY(p)	(((p)->halfs.lo & (NIC_STAT_TX_CHAN_RDY | \
					   NIC_STAT_TX_FIFO_NOT_FULL)) == \
						(NIC_STAT_TX_CHAN_RDY | \
					  	 NIC_STAT_TX_FIFO_NOT_FULL))

#define	NIC_RX_ERRORS(p)	((p)->halfs.lo & \
				 (NIC_STAT_NET_CRC0_ERR    | \
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
				  NIC_STAT_RX_FIFO_OVERRUN | \
				  NIC_STAT_RX_FIFO_UNDERRUN))

#define	NIC_TX_ERRORS(p)	((p)->halfs.lo & NIC_STAT_TX_FIFO_OVERRUN)
#define	NIC_ANY_ERRORS(p)	(NIC_RX_ERRORS(p) | NIC_TX_ERRORS(p))

#define	NIC_STAT_ERROR_BITS	( NIC_STAT_NET_CRC0_ERR    | \
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
				  NIC_STAT_RX_FIFO_UNDERRUN )

#define	NIC_RD_EOD(p)	(((p)->halfs.lo & NIC_STAT_EOD_IN_WORD) ? NIC_EOD : 0)
#define NIC_RX_STATUS(p)	NIC_RX_ERRORS(p) ? NIC_ERROR : NIC_RD_EOD(p)
#define	NIC_TX_STATUS(p)	NIC_TX_ERRORS(p) ? NIC_ERROR : 0

/*
 * external routines
 */
extern void nic_error(unsigned long lo, unsigned long hi);

