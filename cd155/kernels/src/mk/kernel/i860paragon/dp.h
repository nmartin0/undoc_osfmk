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
 * HISTORY
 * $Log: dp.h,v $
 * Revision 0.14  1995/03/07  02:37:45  lenb
 *         define DP_EPOCH_ADDR (12502)
 *
 * Revision 0.13  1994/11/18  20:42:17  mtm
 * Copyright additions/changes
 *
 * Revision 0.12  1994/07/17  03:09:52  lenb
 * lint
 *
 * Revision 0.11  1994/05/06  21:21:09  lenb
 *         define DP_ISTAT_HW_ERROR0 for benefit of ECC code
 *
 * Revision 0.10  1994/03/15  19:36:41  tnt
 * Reviewers: Greg Regnier, Terry Pricket
 * Risk: Medium
 * Benefit or PTS #: 8454; Changes to LTU programming to fix bugs preventing
 *                         LTU transfers to and from expansion memory.
 * Testing: Parallel SATs, Message Passing and Controlc EATs, latency test.
 * Modules: msgp_hw.s, msgp_asm.h, kernel/i860paragon/dp.h
 *
 * Added bit masks for LTU transfers.
 *
 * Revision 0.9  1993/12/16  01:54:17  steved
 * Fixed some problems due to merging with the main trunk. The main problem was
 * a coding error in spl.c with respect to more than 1 CPU.
 *
 * Revision 0.8  1993/12/14  23:00:00  steved
 * Fixed a multitude of problems using the real MP3 nodeboard. Added
 * new led routines to blink the additional LED's on the MP3 board.
 * Cleaned up some of the constants and removed those which are
 * not used on the GP nor MP3 boards.
 *
 * Revision 0.7  1993/10/27  19:59:57  andyp
 * Initial memory expansion card support.
 *
 * Revision 0.6  1993/10/01  16:52:28  steved
 * Added new Virtual DP Asic Constants to support the MP3 board.
 *
 * Revision 0.5  1993/06/30  22:37:06  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.4  1992/12/17  00:23:21  regnier
 * Jerrie's LTU support.
 *
 * Revision 0.3  1992/10/12  18:39:26  stans
 * DP_IMSK0_CPU0_CPU changed to DP_INTR_CPU0
 *
 * Revision 0.2  92/07/29  11:57:55  andyp
 * Slightly better support for 40, 45, and 50 MHz cards.
 * 
 * Revision 0.1  92/07/09  16:43:47  andyp
 * Initial checkin of Paragon files.
 * 
 *
 */

/*
 *	DP ASIC space
 *
 *	DP ASIC space accesses read or write the registers in the DP ASIC.
 *
 *	This space is accessed when the most significant four bits of the
 *	address are equal to 7.
 *
 *	Address bits <11:9> and <6:4> specifiy the resources within the DP
 *	ASIC that are accessed.
 *
 *	This space does not recognize burst transfers, therefore all accesses
 *	in DP ASIC space must be non-cached or I/O.
 */

/*
 *	Define the number of DP's we may expect to see, then define some
 * 	constants used to refer to the DP's.
 */
#define NDPS	5
#define DP0	0
#define DP1	1
#define VDP	2
#define MXDP0	3	/* Memory Expansion DP0 */
#define MXDP1	4	/* Memory Expansion DP1 */


/*
 *	DP Control/Mask register
 *
 *	This register is readable for diagnostic purposes.
 *	This register can be written in two modes: write-to-clear and
 *	write-to-set.  In write-to-clear space, each control bit
 *	written as a one is cleared when written.  In write-to-set space,
 *	each control bit written as a one is set when written.
 *	All bits are initialized to 0 on reset.
 *
 *	This register is a 64-bit register; for simplicity the bit
 *	definitions below are split in two 32-bit pieces (lo and hi).
 */
#define DP_CONTROL0_CLEAR	(0x70000000)
#define DP_CONTROL0_SET		(0x70000210)
#define DP_CONTROL0_READ	(0x70000000)
#define DP_CONTROL1_CLEAR	(DP_CONTROL0_CLEAR + 4)
#define DP_CONTROL1_SET		(DP_CONTROL0_SET + 4)
#define DP_CONTROL1_READ	(DP_CONTROL0_READ + 4)
#define DP_CONTROL2_CLEAR       (0x60600000)
#define DP_CONTROL2_SET         (0x60600210)
#define DP_CONTROL2_READ        (0x60600000)

/*
 *	Memory expansion card DP addresses.
 */
#define MXDP_CONTROL0_CLEAR	(0x88000000)
#define MXDP_CONTROL0_SET	(0x88000210)
#define MXDP_CONTROL0_READ	(0x88000000)
#define MXDP_CONTROL1_CLEAR	(MXDP_CONTROL0_CLEAR + 4)
#define MXDP_CONTROL1_SET	(MXDP_CONTROL0_SET + 4)
#define MXDP_CONTROL1_READ	(MXDP_CONTROL0_READ + 4)


/*
 * interrupt mask/control bits (actual hardware):
 *
 *	< 7: 0>	interrupt mask0 for cpu0
 *	<15: 8>	interrupt mask0 for cpu1
 *	<23:16>	lo dp mask/control
 *	<31:24>	parity error generators + spares
 *	-------
 *	<39:32>	interrupt mask1 for cpu0
 *	<47:40> interrupt mask1 for cpu1
 *	<55:48>	hi dp mask/control
 *	<63:56> parity error generators + spares
 *
 *  rather than create a pile of #define's, we use the "symmetry"
 *  and shift bits into the proper position
 *
 */
#define DP_IMSK0_NIC_CPU	(0)	/* NIC */
#define DP_IMSK0_EXP_CPU	(1)	/* EXPansion */
#define DP_IMSK0_SCAN_CPU	(2)	/* SCAN's INT2NODE */
#define DP_INTR_CPU0		(4)	/* interrupt cpu 0 from cpuN */
#define DP_IMSK0_TMR0_CPU	(5)	/* interrupts from timer0 to cpuN */
#define DP_IMSK0_CPU_SHIFT	(8)
#define DP_INTR_CPU1		(12)	/* interrupt cpu 1 from cpuN */

/*
 * When used in the low 32-bits, the following 6 bits select LO DP
 * functions; when in the high 32-bits, they select HI DP functions.
 */
#define DP_IMSK_HWERR_CPU0	(16)	/* hardware error intr mask for cpu0 */
#define DP_IMSK_HWERR_CPU1	(17)	/* hardware error intr mask for cpu1 */
#define DP_CTRL_ECCENABLE	(18)	/* enable ECC generation */
#define DP_CTRL_CLEAR_HWERR	(19)	/* clear hardware error interrupt */
#define DP_CTRL_CLEAR_TIMER	(20)	/* clear timer interrupt */
#define DP_CTRL_ATTEN_OTHER	(21)	/* interrupt cpuN */

/*
 * The following 4 bits can be used in either mask/control word 0 or
 * mask/control word 1.
 */
#define DP_N11B0PERR		(24)	/* generate N11 byte 0 parity error */
#define DP_N11B1PERR		(25)	/* generate N11 byte 1 parity error */
#define DP_N11B2PERR		(26)	/* generate N11 byte 2 parity error */
#define DP_N11B3PERR		(27)	/* generate N11 byte 3 parity error */

/*
 * Bits in the high 32-bits of the mask/control word
 */
/*				(0)	   (spare) */
/*				(1)	   (spare) */
#define	DP_IMSK1_LTU0_CNT	(2)	/* interrupts from ltu0 to cpuN */
#define	DP_IMSK1_LTU1_CNT	(3)	/* interrupts from ltu1 to cpuN */
#define DP_IMSK1_CPU1_CPU	(4)	/* interrupts from cpu1 to cpuN */
#define DP_IMSK1_TMR1_CPU	(5)	/* interrupts from timer1 to cpuN */
#define DP_IMSK1_BUST_CPU	(6)	/* bus-timeout1 interrupt to cpuN */
#define DP_IMSK1_CPU_SHIFT	(8)

/*
 *	DP status/interrupt register (64-bits)
 */
#define DP_STATUS_LO	(0x70000210)		/* lo 32-bits */
#define DP_STATUS_HI	(DP_STATUS_LO + 4)	/* hi 32-bits */
#define VDP_STATUS      (0x60600210)
#define DP_ISTAT_NIC		(1 <<  0)	/* NIC interrupt */
#define DP_ISTAT_EXP		(1 <<  1)	/* expansion interrupt */
#define DP_ISTAT_NET		(1 <<  2)	/* net interrupt */
#define DP_ISTAT_SEC0		(1 <<  8)	/* lo single-error interrupt */
#define DP_ISTAT_ECC0		(1 <<  9)	/* lo ecc interrupt */
#define DP_ISTAT_PARITY0	(1 << 10)	/* lo n11 parity interrupt */
#define	DP_ISTAT_HW_ERROR0	(DP_ISTAT_SEC0 | DP_ISTAT_ECC0 | DP_ISTAT_PARITY0)

/*
 *	and in the high 32-bits...
 */
#define	DP_ISTAT_LTU0_CNT	(1 <<  2)	/* ltu0 count interrupt */
#define	DP_ISTAT_LTU1_CNT	(1 <<  3)	/* ltu1 count interrupt */

/*
 * These bits are the same for the LO and HI DP's as well as the Virtual
 * DP so we just declare them once here.
 */
#define DP_ISTAT_CPU		(1 <<  4)	/* cpu interrupt */
#define DP_ISTAT_TIMER		(1 <<  5)	/* timer interrupt */

/*
 *	Memory expansion card DP status/interrupt register addresses.
 */
#define MXDP_STATUS_LO	(0x88000210)		/* lo 32-bits */
#define MXDP_STATUS_HI	(MXDP_STATUS_LO + 4)	/* hi 32-bits */

/*
 *	DP up counters (2 x 32-bits) (trigger interrupt at carry to 0)
 */
#define	DP_TIMER0	(0x70000420)		/* lo dp 32-bit timer */
#define DP_TIMER1	(DP_TIMER0 + 4)		/* hi dp 32-bit timer */
#define DP_TIMER2       (0x60600420)
#define	MXDP_TIMER0	(0x88000420)		/* lo dp 32-bit timer */
#define	MXDP_TIMER1	(MXDP_TIMER0 + 4)	/* hi dp 32-bit timer */


/*
 *	DP epoch counter (64-bit)
 */
#define	DP_EPOCH_DADDR	((volatile double *)(0xFFFF4000 + 0x630)) /* virtual */

#define DP_EPOCH_LO	(0x70000630)		/* lo 32-bits */
#define DP_EPOCH_HI	(DP_EPOCH_LO + 4)	/* hi 32-bits */
#define DP_EPOCH	((volatile double *) DP_EPOCH_LO)	/* all 64 */
#define MXDP_EPOCH_LO	(0x88000630)		/* lo 32-bits */
#define MXDP_EPOCH_HI	(MXDP_EPOCH_LO + 4)	/* hi 32-bits */
#define MXDP_EPOCH	((volatile double *) MXDP_EPOCH_LO)	/* all 64 */

/*
 *	DP ECC latch (bytes & address in error)
 */
#define DP_DRAM_ECCERR0		(0x70000840)
#define DP_DRAM_ECCERR1		(DP_DRAM_ECCERR0 + 4)
#define MXDP_DRAM_ECCERR0	(0x88000840)
#define MXDP_DRAM_ECCERR1	(MXDP_DRAM_ECCERR0 + 4)

/*
 *	DP DRAM address latch (captures last valid DRAM address)
 */
#define	DP_DRAM_ADDR0	(0x70000a50)
#define DP_DRAM_ADDR1	(DP_DRAM_ADDR0 + 4)
#define	MXDP_DRAM_ADDR0	(0x88000a50)
#define MXDP_DRAM_ADDR1	(MXDP_DRAM_ADDR0 + 4)

/*
 *	DP LTU interrupt/status clear
 *
 *	A read from this register clears the LTU count interrupt
 */
#define DP_LTU0_CLEAR_CNT	(0x60400000)
#define DP_LTU1_CLEAR_CNT	(0x60500000)

/*
 *	DP LTU count
 */
#define DP_LTU0_COUNT	(0x70000c60)
#define DP_LTU1_COUNT	(DP_LTU0_COUNT + 4)
#define MXDP_LTU0_COUNT	(0x88000c60)
#define MXDP_LTU1_COUNT	(MXDP_LTU0_COUNT + 4)

/*
 *	DP LTU address
 */
#define DP_LTU0_ADDR	(0x70000e70)
#define DP_LTU1_ADDR	(DP_LTU0_ADDR + 4)
#define MXDP_LTU0_ADDR	(0x88000e70)
#define MXDP_LTU1_ADDR	(MXDP_LTU0_ADDR + 4)

#define DP_BITBUCKET	0x60400000

/*
 *	DP LTU data
 */
#define	DP_LTU_MX_MSK	(1 << 14)	/* bit 30, Base or expansion memory */
#define	DP_LTU_NOEOD	(1 << 15)	/* bit 31, No EOD */
#define	DP_LTU_ADDR_CLR	(0xC000)	/* Clear high bits of addr for LTU */
