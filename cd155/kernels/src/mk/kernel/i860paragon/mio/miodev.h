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
 * $Log: miodev.h,v $
 * Revision 0.3  1994/11/18  20:45:17  mtm
 * Copyright additions/changes
 *
 * Revision 0.2  1993/06/30  22:39:50  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.1  1992/07/09  16:59:34  andyp
 * Initial checkin of Paragon files.
 *
 *
 */

#define MIO_DEV_SRAM	0	/* 512KB static RAM */
#define MIO_DEV_FLASH	1	/* 256KB flash EEPROM */

#define	NMIODEVS	2

extern struct bus_device 	*mio_dinfo[];

#define MIO_RAM_SIZE		((unsigned long)mio_dinfo[MIO_DEV_SRAM]->sysdep)
#define MIO_ETHER_RAM_SIZE	0x10000
#define MIO_SCSI_RAM_SIZE	(MIO_RAM_SIZE - MIO_ETHER_RAM_SIZE)
