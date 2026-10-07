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
 * $Log: esp_ctlr.h,v $
 * Revision 1.5  1994/11/18  20:37:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:52  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:23  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:53  dleslie
 * First R1_0 release
 *
 * Revision 2.1.2.1  92/03/28  10:07:27  jeffreyh
 * 	New file for generic scsi driver.
 * 	[92/03/17            jeffreyh]
 * 
 *
 */
/*
 * ESP controller physical address map
 *
 *	D0000000 - D000FFFF	FIFO access
 *	D4000000 - D40000FF	ESP access
 *	D8000000 - D80000FF	AUX access
 *	DC000000 - DC0000FF	LOOPBACK access (for diagnostics)
 */

/*
 * Physical address for the SCSI controller FIFO
 */

#define SCSI_FIFO	0xD0000000


/*
 * Physical offset for the SCSI controller registers
 */

#define SCSI_PHYS_OFF	0x04000000


/*
 * Length of SCSI controller registers
 */

#define SCSI_FIFO_LEN	0x10000
#define ESP_REG_LEN	0x100
#define SCSI_AUX_LEN	0x100


/*
 * Definition of SCSI controller auxilliary port
 */

#define	AUXPAD(n)	char n[60]

typedef struct {
	volatile unsigned long	scsi_clear_cnt;    /* r: clear counter */
#define		scsi_read_mode	scsi_clear_cnt     /* w: set read mode */
	AUXPAD(pad0);
	volatile unsigned long	scsi_reset_esp;    /* r: reset esp */
#define		scsi_write_mode	scsi_reset_esp     /* w: set write mode */
	AUXPAD(pad1);
	volatile unsigned long	scsi_reset_fifo;   /* r: reset scsi fifo */
#define		scsi_enable_hf	scsi_reset_fifo	   /* w: enable fifo hf int */
	AUXPAD(pad2);
	volatile unsigned long	scsi_disable_hf;   /* w: disable fifo hf int */
} scsi_auxmap_t;


/*
 * SCSI controller flags - read as upper byte of ESP status register
 */

#define SCSI_RFF		(1 << 15)    /* read fifo full */
#define SCSI_RHF		(1 << 14)    /* read fifo half full */
#define SCSI_REF		(1 << 13)    /* read fifo empty active low */
#define SCSI_MODE		(1 << 12)    /* mode bit: 1=write, 0=read */
#define SCSI_INT		(1 << 11)    /* esp interrupt active low */
#define SCSI_WFF		(1 << 10)    /* write fifo full */
#define SCSI_WHF		(1 <<  9)    /* write fifo half full */
#define SCSI_WEF		(1 <<  8)    /* write fifo empty active low */
