/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: x24c02.h,v $
 * Revision 1.2.8.1  1995/06/11  18:34:02  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:44:11  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	x24c02.h
 *	Author:	Jim Chorn (modified by Jerrie Coffman for the microkernel)
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	11/94
 *
 *	This header file contain the support code for
 *	accessing the Xicor X24C02 Serial E**2 Prom
 */


#ifndef	_X24C02_H_
#define	_X24C02_H_

typedef u_char x24c02_reg_t;

#define X24C02_VALID	0x0F	/* Valid bits for this implementation */
#define X24C02_SCL	0x01	/* SCL Clock bit */
#define X24C02_SDA	0x02	/* SDA Data bit */
#define X24C02_DIR	0x04	/* DIR Direction bit 1=used=out */
#define X24C02_DIR_IN	0x00	/* DIR Direction bit 0=notused=in */
#define X24C02_WEN	0x08	/* WEN Write Enable bit */
#define X24C02_ADDR	0x70	/* Slave address mask if connected in reg */
#define X24C02_SLV_MASK	0x0E	/* Slave address mask with-in serial protocol */
#define X24C02_RW_BIT	0x01	/* Slave address R/#W bit in serial protocol */
#define X24C02_ID	0xA0	/* X24C02 id 1010 xxxx bit pattern */
#define X24C02_DMASK	0x80	/* Most significant bit of byte mask */

/*
 * The software implementation was designed to accommodate
 * more than one slave Eprom assuming that the slave select address bits
 * A0-A2 appear in the register as bits 4-6 at X24C02_CHIP_ADDRESS.
 * Since the hardware forces these bits to zero the following defines
 * allow the optimizer to produce code which does the same
 * (force slave addr to zero). This code only affects the chip selects
 * (and slave address comparisons internal to the component).
 */
#ifdef NO_SLAVE_FORCES	
/*
 * Define to enable slave address outputs
 */
#define	X24C02_SLAVE_ADDRESS(slave) \
		((((slave)<<1) & X24C02_SLV_MASK)|X24C02_ID)
#define SET_A0TOA2(slave) \
		(((slave)<<4) & X24C02_ADDR)
#else
/*
 * Force Slave Address bits A0-A2 to zero, let compiler do clean up.
 */
#define	X24C02_SLAVE_ADDRESS(slave) \
		(X24C02_ID)		/* <--- Must always have X24C02_ID! */
#define	SET_A0TOA2(slave) \
		(x24c02_reg_t)(0)
#endif

/*
 * The timeout is 40 ms which depends on the number of loops
 * thru the acknowledge polling loop.  This timeout is eight times
 * the typical write cycle time and four times the t_WR chip max (10 ms).
 * The time is calculated in useconds.
 */
#define TIMEOUT	( 40000/		   /* 40 ms divided by quantity:    */ \
		((8*(t_LOW+t_HIGH)) +	   /* Time for data  bits to go out */ \
		 (t_SU_STA+t_HD_STA)+	   /* Time for start bits to go out */ \
		 (t_SU_STO+t_HIGH+t_BUF))) /* Time for stop  bits to go out */
/*
 * Write cycle definitions for maximum page writes.
 */
#define X24C02_MAX_PAGE_MASK	0x03	/* Starting word_address wraps at 0 */
#define X24C02_MAX_PAGE_WRITE	0x04	/* Maximum page write count */

/*
 * Timing: Minimum timings required by chip interface.
 * See X24C02 data spec sheet. Assumes times in microseconds.
 */
#define	t_SU_STA	5	/* 4.7 minimum Start Condition Setup Time */
#define	t_HD_STA	4	/* 4.0 minimum Start Condition Hold Time */
#define	t_LOW		5	/* 4.7 minimum Clock Low Period */
#define	t_HIGH		4	/* 4.0 minimum Clock High Period */
#define	t_SU_STO	5	/* 4.7 minimum Stop Condition Setup Time */
#define	t_BUF		5	/* 4.7 minimum Bus must be free before start */
#define t_DH		2	/* 0.300 minimum Data Out Hold Time */

#endif	_X24C02_H_
