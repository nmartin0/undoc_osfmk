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
 * $Log: mio.h,v $
 * Revision 0.5  1994/11/18  20:45:12  mtm
 * Copyright additions/changes
 *
 * Revision 0.4  1994/01/07  18:08:42  rkl
 *  Added #define's for physical addresses of MIO performance counters.
 *
 *  Reviewer: andyp
 *  Risk: low
 *  Benefit or PTS #: Added device_mmap() access to performance counters.
 *  Testing: Yes
 *  Module(s): if_mioe.c mio.h
 *
 * Revision 0.3  1993/06/30  22:39:45  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.2  1992/08/25  09:45:10  andyp
 * Added a #define for the address of the LED on the MIO card.
 * Some cards have LED's that work, most cards don't have them.
 *
 * Revision 0.1  92/07/09  16:57:38  andyp
 * Initial checkin of Paragon files.
 * 
 *
 */

#define MIO_BASE_SRAM		(0x80000000)	/* phys addr of sram  */
#define MIO_TOPOF_EEPROM	(0xbfffffff)	/* top of flash       */
#define	MIO_SCSI_START		(0x91000000)	/* ASC base addr      */
#define	MIO_SERIAL_START	(0x91400000)	/* i82510 addr        */
#define MIO_DMA_LO		(0xa0800000)	/* bits 0 - 15 of dma */
#define MIO_DMA_HI		(0xa0880000)    /* bits 16, 17 of dma */
#define	MIO_PERF_COUNTER_LOW	(0xa0900000)	/* low 16 bits of perf cntr */
#define	MIO_PERF_COUNTER_HIGH	(0xa0980000)	/* high 16 bits of perf cntr */
#define MIO_PERF_COUNTER_RESET	(0xa0980000)	/* reset for perf counter */
#define MIO_LED			(0xa0a80000)	/* bit 1 -- 0==on, 1==off */

/*
 *	Interrupts from any of the four interrupt sources on the
 *	MIO card are enabled by a bit vector.  When set to 1,
 *	the interrupt is enabled.  When set to 0, the interrupt
 *	is masked.
 */
#define MIO_INTERRUPT_CONTROL	(0xa0b00000)
#define MIO_INTENABLE_PARITY	(1 << 12)
#define MIO_INTENABLE_ETHER	(1 << 13)
#define MIO_INTENABLE_SERIAL	(1 << 14)
#define MIO_INTENABLE_SCSI	(1 << 15)

/*
 *	Interrupt status for any of the four interrupt sources
 *	on the MIO card can be found in the MIO interrupt status
 *	register, bits <7:4>.
 */
#define MIO_INTERRUPT_STATUS	(0xa0880000)
#define MIO_INTPENDING_PARITY	(1 << 4)
#define MIO_INTPENDING_ETHER	(1 << 5)
#define MIO_INTPENDING_SERIAL	(1 << 6)
#define MIO_INTPENDING_SCSI	(1 << 7)
