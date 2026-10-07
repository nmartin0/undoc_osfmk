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
 * $Log: i8250.h,v $
 * Revision 1.2.8.1  1995/06/11  18:32:33  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:43:24  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	i8250.h
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	Defines for the Intel 8250 UART
 */


#ifndef	_I8250_H_
#define	_I8250_H_

#if	PARAGON860
#define	portplus(n)	((n) << 3)
#else	PARAGON860
#define	portplus(n)	(n)
#endif	PARAGON860

#define TXRX(addr)	(addr + portplus(0))
#define BAUD_LSB(addr)	(addr + portplus(0))
#define BAUD_MSB(addr)	(addr + portplus(1))
#define INTR_ENAB(addr)	(addr + portplus(1))
#define INTR_ID(addr)	(addr + portplus(2))
#define LINE_CTL(addr)	(addr + portplus(3))
#define MODEM_CTL(addr)	(addr + portplus(4))
#define LINE_STAT(addr)	(addr + portplus(5))
#define MODEM_STAT(addr)(addr + portplus(6))

#define MODi 0
#define TRAi 2
#define RECi 4
#define LINi 6

/* line control register */
#define		iWLS0		0x01		/* word length select bit 0  */
#define		iWLS1		0x02		/* word length select bit 2  */
#define		iSTB		0x04		/* number of stop bits	     */
#define		iPEN		0x08		/* parity enable	     */
#define		iEPS		0x10		/* even parity select	     */
#define		iSP		0x20		/* stick parity		     */
#define		iSETBREAK	0x40		/* break key		     */
#define		iDLAB		0x80		/* divisor latch access bit  */
#define		i5BITS		0x00		/* 5 bits per char	     */
#define		i6BITS		0x01		/* 6 bits per char	     */
#define		i7BITS		0x02		/* 7 bits per char	     */
#define		i8BITS		0x03		/* 8 bits per char	     */

/* line status register */
#define		iDR		0x01		/* data ready		     */
#define		iOR		0x02		/* overrun error	     */
#define		iPE		0x04		/* parity error		     */
#define		iFE		0x08		/* framing error	     */
#define		iBRKINTR	0x10		/* a break has arrived	     */
#define		iTHRE		0x20		/* tx hold reg is now empty  */
#define		iTSRE		0x40		/* tx shift reg is now empty */

/* interrupt id regisger */
#define		iMODEM_INTR	0x01
#define		iTX_INTR	0x02
#define		iRX_INTR	0x04
#define		iERROR_INTR	0x08

/* interrupt enable register */
#define		iRX_ENAB	0x01
#define		iTX_ENAB	0x02
#define		iERROR_ENAB	0x04
#define		iMODEM_ENAB	0x08

/* modem control register */
#define		iDTR		0x01		/* data terminal ready	     */
#define		iRTS		0x02		/* request to send	     */
#define		iOUT1		0x04		/* COM aux line -not used    */
#define		iOUT2		0x08		/* turns intr to 386 on/off  */	
#define		iLOOP		0x10		/* loopback for diagnostics  */

/* modem status register */
#define		iDCTS		0x01		/* delta clear to send	     */
#define		iDDSR		0x02		/* delta data set ready	     */
#define		iTERI		0x04		/* trail edge ring indicator */
#define		iDRLSD		0x08		/* delta rx line sig detect  */
#define		iCTS		0x10		/* clear to send	     */
#define		iDSR		0x20		/* data set ready	     */
#define		iRI		0x40		/* ring indicator	     */
#define		iRLSD		0x80		/* rx line sig detect	     */

#endif	_I8250_H_
