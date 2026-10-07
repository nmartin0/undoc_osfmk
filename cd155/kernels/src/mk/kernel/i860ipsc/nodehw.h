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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
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
 * $Log: nodehw.h,v $
 * Revision 1.3  1994/11/18  20:40:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:34:59  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  18:12:28  regnier
 * Initial revision
 *
 * Revision 2.3.2.1  92/02/18  19:04:17  jeffreyh
 * 	[intel] macros for reading mem-map registers & uncached memory.
 * 	[92/02/13  12:52:57  jeffreyh]
 * 
 * Revision 2.3  91/06/18  20:52:34  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:02:57  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:20  jsb
 * 	First checkin.
 * 	[90/12/03  21:50:45  jsb]
 * 
 */

/*
 * READNC and DREADNC are for 32 bit and 64 bit non-cached reads.
 * When paging is enabled, these can be normal reads as long as
 * the mapping uses the cache-disable bit for hardware registers.
 */

#define READNC(x)	(* (volatile unsigned long *) (x))
#define DREADNC(x)	(* (volatile double *) (x))

/*
 * FIFO and Transmit EOD Base Addresses
 *
 * _PH are physical address as decoded by hardware
 * others are virtual address with standardized mapping
 */

#define FIFO_ADDR_PH	0x30000000
#define XEOD_OFF_PH	0x08000000
#define CSR_ADDR_PH	0x40000000
#define PERFCNT_ADDR_PH	0x50000000
#define UART_ADDR_PH	0x60000000

#define IO_BASE		0xFFFF0000

#define FIFO_ADDR	(IO_BASE + 0x0000)
#define XEOD_OFF	0x00001000
#define CSR_ADDR	(IO_BASE + 0x2000)
#define PERFCNT_ADDR	(IO_BASE + 0x4000)
#define UART_ADDR	(IO_BASE + 0x6000)

#define SEND_FIFO	*(volatile unsigned long *)(FIFO_ADDR)
#define SEND_EOD	*(volatile unsigned long *)(FIFO_ADDR+XEOD_OFF)
#define RECV_FIFO	READNC(FIFO_ADDR)

#define HALF_MSG_FIFO	2048

/*
 * Control and Status Register Addresses - Paging enabled
 */

#define STATUS_REG	READNC(CSR_ADDR)
#define CONTROL_REG	*(unsigned long *)(CSR_ADDR + 4)

/*
 * Generic, use with "in" and "out"
 */

#define STATUS_PORT	(CSR_ADDR - IO_BASE)
#define CONTROL_PORT	(CSR_ADDR + 4 - IO_BASE)


/*
 * Status Register Bit Definition
 */

/* Bits 0-31 */
#define SEND_SOME	0x00000001	/* active hi */
#define SEND_HALF	0x00000002	/* active hi */
#define SEND_FULL	0x00000004	/* active hi */
#define RECV_SOME	0x00000008	/* active hi */
#define RECV_HALF	0x00000010	/* active hi */
#define RECV_FULL	0x00000020	/* active hi */
#define EOD_IN		0x00000040	/* active hi */
#define EOD_LAST	0x00000080	/* active hi */
/* DCM bits */
#define PROGRAM_DONE	0x00000100	/* active hi */
#define CONFIG_MASK	0x00000E00	/* active hi */
#define XFMT		0x00001000	/* dual function active lo */
#define STATUS0		0x00001000	/* dual function */
#define RFMT		0x00002000	/* dual function active lo */
#define STATUS1		0x00002000	/* dual function */
#define XFHF		0x00004000	/* active lo */
#define RFHF		0x00008000	/* active lo */

#define SLOT_ID_MASK	0x007F0000
#define BOOT_STATUS	0x01000000	/* active lo */
#define MHZ_40_33	0x02000000	/* hi = 40 MHz, lo = 33 MHz */
#define PARITY_INT	0x04000000	/* active hi */
#define BUSTOUT_INT	0x08000000	/* active hi */
#define RECV_INT	0x10000000	/* active hi */
#define SEND_INT	0x20000000	/* active hi */
#define EXPAN_INT	0x40000000	/* active lo */
#define SERIAL_INT	0x80000000	/* active hi */
/* Bits 32-63 */
#define EXPAN_BIT0	0x00000001	/* active lo */
#define EXPAN_BIT1	0x00000002	/* active lo */
#define EXPAN_BIT2	0x00000004	/* active lo */
#define EXPAN_BIT3	0x00000008	/* active lo */
#define UNUSED_36_63	0xFFFFFFF0

/*
 * Control Register Bit Definition
 */

/* Bits 0-31 */
#define UNUSED_0_31	0xFFFFFFFF
/* Bits 32-63 */
#define UNUSED_32_39	0x000000FF
#define CORRUPT		0x00000100	/* dual function active hi */
#define DATA_BIT_0	0x00000100	/* dual function active hi */
#define RELEASE_RCV	0x00000200	/* dual function active hi */
#define DATA_BIT_1	0x00000200	/* dual function active hi */
#define DATA_BIT_2	0x00000400	/* active hi */
#define DATA_BIT_3	0x00000800	/* active hi */
#define NXRESET		0x00001000	/* active lo */
#define NTRESET		0x00002000	/* active lo */
#define CCLOCK		0x00004000	/* active hi */
#define NPROGRAM	0x00008000	/* active lo */
#define GREEN		0x00010000	/* active hi */
#define RED		0x00020000	/* active hi */
#define YELLOW		0x00040000	/* active hi */
#define DEFEAT_REFRESH	0x00080000	/* active hi */
#define CLEAR_BUS	0x00100000	/* active lo */
#define CLEAR_PARITY	0x00200000	/* active lo */
#define FIFO_RESET	0x00400000	/* active lo */
#define FIFO_WRAP	0x00800000	/* active lo */
#define BOOT_CTRL	0x01000000	/* active lo */
#define PARITY_ENABLE	0x02000000	/* active hi */
#define HARD_INT_MASK	0x04000000	/* active lo */
#define EOD_IN_INT_MASK 0x08000000	/* active lo */
#define RECV_INT_MASK	0x10000000	/* active lo */
#define SEND_INT_MASK	0x20000000	/* active lo */
#define EXPAN_INT_MASK	0x40000000	/* active lo */ 
#define SERIAL_INT_MASK 0x80000000	/* active lo */

/*
 * Performance Port and Counter Addresses
 */

#define PERF_REG	*(unsigned long *)PERFCNT_ADDR
#define COUNTER_REG	DREADNC(PERFCNT_ADDR)
#define COUNTER_REG_LO	READNC(PERFCNT_ADDR)

#define PERF_PORT	(PERFCNT_ADDR - IO_BASE)
#define COUNTER_PORT	(PERFCNT_ADDR - IO_BASE)


/*
 * UART Register Offsets
 */

#define UART_REG0	(UART_ADDR - IO_BASE + 0x00)
#define UART_REG1	(UART_ADDR - IO_BASE + 0x08)
#define UART_REG2	(UART_ADDR - IO_BASE + 0x10)
#define UART_REG3	(UART_ADDR - IO_BASE + 0x18)
#define UART_REG4	(UART_ADDR - IO_BASE + 0x20)
#define UART_REG5	(UART_ADDR - IO_BASE + 0x28)
#define UART_REG6	(UART_ADDR - IO_BASE + 0x30)
#define UART_REG7	(UART_ADDR - IO_BASE + 0x38)

/*
 * UART Bank Selects
 */

#define UART_BANK	UART_REG2
#define UART_BANK0	0x00
#define UART_BANK1	0x20
#define UART_BANK2	0x40
#define UART_BANK3	0x60

/*
 * UART Registers
 */

/* Bank 0 */
#define UART_RXD	UART_REG0
#define UART_TXD	UART_REG0
#define UART_BAL	UART_REG0
#define UART_BAH	UART_REG1
#define UART_GER	UART_REG1
#define UART_GIR	UART_REG2
#define UART_LCR	UART_REG3
#define UART_MCR	UART_REG4
#define UART_LSR	UART_REG5
#define UART_MSR	UART_REG6
#define UART_ACR0	UART_REG7

/* Bank 1 */
#define UART_RXF	UART_REG1
#define UART_TXF	UART_REG1
#define UART_TMST	UART_REG3
#define UART_TMCR	UART_REG3
#define UART_FLR	UART_REG4
#define UART_RST	UART_REG5
#define UART_RCM	UART_REG5
#define UART_TCM	UART_REG6
#define UART_GSR	UART_REG7
#define UART_ICM	UART_REG7

/* Bank 2 */
#define UART_FMD	UART_REG1
#define UART_TMD	UART_REG3
#define UART_IMD	UART_REG4
#define UART_ACR1	UART_REG5
#define UART_RIE	UART_REG6
#define UART_RMD	UART_REG7

/* Bank 3 */
#define UART_CLCF	UART_REG0
#define UART_BACF	UART_REG1
#define UART_BBL	UART_REG0
#define UART_BBH	UART_REG1
#define UART_BBCF	UART_REG3
#define UART_PMD	UART_REG4
#define UART_MIE	UART_REG5
#define UART_TMIE	UART_REG6
 
#define RECV_READY	0x01
#define XMIT_READY	0x02

#define USM_INT		0x08	/* active lo output */
#define NMI		0x10	/* active lo input */
#define NET_INT		0x80	/* active hi input */
