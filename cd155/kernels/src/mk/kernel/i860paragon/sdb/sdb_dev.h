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
 * $Log: sdb_dev.h,v $
 * Revision 1.2.8.1  1995/06/11  18:33:31  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:43:42  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	sdb_dev.h
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	SCSI-16 daughter board driver definitions
 */


#ifndef	_SDB_DEV_H_
#define	_SDB_DEV_H_

#define SDB_DEV_SRAM	0	/* Static RAM		*/
#define SDB_DEV_DPRAM	1	/* Static dual-port RAM	*/
#define SDB_DEV_FLASH	2	/* Flash EEPROM		*/

#define	NSDB_DEVS	3

extern struct bus_device 	*sdb_dinfo[];

/*
 * Starting virtual addresses of memory devices on the SCSI-16 daughter board
 */
#define SDB_SRAM_ADDR	((unsigned char *)sdb_dinfo[SDB_DEV_SRAM]->address)
#define SDB_DPRAM_ADDR	((unsigned char *)sdb_dinfo[SDB_DEV_DPRAM]->address)
#define SDB_FLASH_ADDR	((unsigned char *)sdb_dinfo[SDB_DEV_FLASH]->address)

/*
 * Sizeof memory devices on the SCSI-16 daughter board
 */
#define SDB_SIZEOF_SRAM	 ((unsigned long)sdb_dinfo[SDB_DEV_SRAM]->sysdep)
#define SDB_SIZEOF_DPRAM ((unsigned long)sdb_dinfo[SDB_DEV_DPRAM]->sysdep)
#define SDB_SIZEOF_FLASH ((unsigned long)sdb_dinfo[SDB_DEV_FLASH]->sysdep)

/*----------------------------------------------------------------------------*/

/*
 * The virtual addresses for memory used by each controller are defined based
 * on the SCSI-16 daughter board addresses and the size allocated for each
 * controller
 */
#define SIOPS_START_SRAM	(SDB_SRAM_ADDR)
#define ETHER_START_SRAM	(SIOPS_START_SRAM  + SIOPS_SIZEOF_SRAM)

#define SIOPS_START_DPRAM	(SDB_DPRAM_ADDR)
#define ETHER_START_DPRAM	(SIOPS_START_DPRAM  + SIOPS_SIZEOF_DPRAM)

#endif	_SDB_DEV_H_
