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
 * $Log: flash.h,v $
 * Revision 0.3  1994/11/18  20:45:03  mtm
 * Copyright additions/changes
 *
 * Revision 0.2  1993/06/30  22:39:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.1  1992/07/09  16:59:41  andyp
 * Initial checkin of Paragon files.
 *
 *
 */

/******************************
 *
 *  MIO Flash structure.
 *
 *****************************/
/*
 *  Common Configuration Area
 */
struct	config {
	u_short	spare[ 8 ];		/* spare slots			*/
	u_short	serial_num;		/* serial number		*/
	u_char	id_rev;			/* ID and Revison number	*/
#define			ID_MASK			0xf1
#define				MIO			0xc0
#define				SCSI			0xc8
#define				HIPPI			0xd0
#define				MEM_16MB		0xf0
#define				MEM_64MB		0xf1
#define			REV_MASK		0x07
	u_char	size;			/* Size of total flash EPROM	*/
#define			SIZE_128Kx8		1
#define			SIZE_256Kx8		2
#define			SIZE_128Kx16		10
#define			SIZE_256Kx16		20
	u_char	sig_1;			/* Signature test pattern 0x55	*/
	u_char	sig_2;			/* Signature test pattern 0xaa	*/
	u_short	crc;			/* CRC				*/
};
typedef struct config *config_t;

/*
 *  Board Specific Area.
 */
#define	BD_SPEC_FREE  (0x100 - (			\
			6	/* ether_addr */ +	\
			sizeof(struct config)	 +	\
			2	/* CRC */	 ))

struct bd_specific {
	u_char	ether_addr[ 6 ];	/* Ethernet address		*/
	u_char	free[ BD_SPEC_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
};
typedef struct bd_specific *bd_specific_t;

/*
 *  Boot Area
 */
#define	BOOT_FREE  (0x4000 - (				\
			sizeof(struct config)	+	\
			sizeof(struct bd_specific) 	+	\
			2	/* CRC */	))

struct boot {
	u_char	free[ BOOT_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
};
typedef struct boot *boot_t;

/*
 *  Setup Area
 */
#define BOOT_STRS	4
#define BOOT_STR_SIZE	256
#define BOOT_STR_SPACE	(BOOT_STRS * BOOT_STR_SIZE)
#define	SETUP_FREE  (0x800 - (				\
			BOOT_STR_SPACE		+	\
			2	/* CRC */	))

struct setup {
	char	boot_string[ BOOT_STRS ][ BOOT_STR_SIZE ];/* Boot string array*/
	u_char	free[ SETUP_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
};
typedef struct setup *setup_t;

/*
 *  Diag Area 3
 */
#define	DIAG_3_FREE  (0x2000 - (			\
			sizeof(struct setup)		+	\
			2	/* CRC */	))

struct diag_3 {
	u_char	free[ DIAG_3_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
};
typedef struct diag_3 *diag_3_t;

/*
 *  Diag Area 2
 */
#define	DIAG_2_FREE	(0x2000 - ( 0 ))

struct diag_2 {
	u_char	free[ DIAG_2_FREE ];	/* Free space			*/
};
typedef struct diag_2 *diag_2_t;

/*
 *  Diag Area 1
 */
#define	DIAG_1_FREE	(0x38000 - ( 0 ))
struct diag_1 {
	u_char	free[ DIAG_1_FREE ];	/* Free space			*/
};
typedef struct diag_1 *diag_1_t;

/*
 *  Composit Flash ROM data type;
 */
struct mio_flash {
	/*
	 *  Main block
	 */
	struct main {
		struct diag_1	diag_1;	/* Diagnostics area 1		*/
	} main;

	/*
	 *  First parameter area
	 */
	struct param_1 {
		struct diag_2	diag_2;	/* Diagnostics area 2		*/
	} param_1;

	/*
	 *  Second parameter area
	 */
	struct param_2 {
		struct diag_3	diag_3;	/* Diagnostics area 3		*/
		struct setup	setup;	/* Setup area			*/
	} param_2;

 	/*
	 *  Boot block
	 */
	struct boot_blk {
		struct boot		boot;		/* boot area */
		struct bd_specific	bd_specific;	/* brd specific area */
		struct config		config;		/* configuration area */
	} boot_blk;
};

typedef struct mio_flash *mio_flash_t;

