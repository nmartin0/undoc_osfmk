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
 * $Log: sdb_flash.h,v $
 * Revision 1.2.8.1  1995/06/11  18:33:40  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:43:50  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	sdb_flash.h
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	SCSI-16 daughter board flash structure
 */


#ifndef	_SDB_FLASH_H_
#define	_SDB_FLASH_H_

/* 
 *	   SCSI-16 daughter board flash structure
 *       +----------------------------------------+
 * high  | Common Configuration Area		  |
 *       +----------------------------------------+
 *       | Xilinx Block                           |
 *       +----------------------------------------+
 *       | Boot Block                             |
 *       +----------------------------------------+
 *       | tftp Block                             |
 *       +----------------------------------------+
 *       | Script Block                           |
 *       +----------------------------------------+
 *       | NCT Block                              |
 *       +----------------------------------------+
 *  low  | Flash Header                           |
 *       +----------------------------------------+
 */

/*
 *  Common Configuration Area
 */
struct	config {
	unsigned char	id_rev;		/* ID and Rev number at 0xbfffffe0 */
#define			ID_MASK			0xf1
#define			REV_MASK		0x07

#define				MIO			0xc0
#define				SCSI			0xc8
#define				HIPPI			0xd0
#define				MEM_16MB		0xe0
#define				MEM_64MB		0xe8
#define				MEM_32MB		0xf0
#define				MEM_128MB		0xf8

	unsigned char   sw_rev;		/* firmware version */

	unsigned char   spare0;		/* spare slot */
	unsigned char   spare1;		/* spare slot */

	unsigned int	sig_1;		/* Signature test pattern 0x55's */
	unsigned int	sig_2;		/* Signature test pattern 0xaa's */

        unsigned char	pad[ 18 ];	/* spare slots */

        unsigned short  flash_checksum;
};
typedef struct config config_t;		/* total 32 bytes */

/* 
 * Number of sections in the flash 
 */
#define NUM_BLKS	6

/*
 * Header structure for each block 
 */
struct blk_header {
	unsigned int 	offset;		/* offset from beginning of the flash */
	unsigned int	size;		/* size of this block		      */
	unsigned int    type;		/* block type or format		      */
#define			SCRIPT_BLOCK	0x53c720

	unsigned char	name[100];	/* name of this block		      */
};
typedef struct blk_header blk_header_t;

/*
 *  Setup Area
 */
#define BOOT_STRS       4
#define BOOT_STR_SIZE   256
#define BOOT_STR_SPACE  (BOOT_STRS * BOOT_STR_SIZE)

struct setup {
	char            boot_str [BOOT_STRS] [BOOT_STR_SIZE]; /* Boot Strings */
        unsigned short  crc;            /* CRC of boot_str */
};
typedef struct setup setup_t;

/*
 * This is the header structure located at the begining of the flash
 */
struct fw_header {
	unsigned char	id_rev;		/* ID and Revison number	*/
	unsigned char	size;		/* Size of total flash EPROM	*/
#define			SIZE_128Kx8		1
#define			SIZE_256Kx8		2
#define			SIZE_128Kx16		10
#define			SIZE_256Kx16		20

        setup_t         setup;		/* setup area for boot strings	*/

	unsigned int 	nblk;		/* number of sections		*/
	blk_header_t	blk[NUM_BLKS];	/* offset to sections		*/
};
typedef struct fw_header fw_header_t;	

#endif	_SDB_FLASH_H_
