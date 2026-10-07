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
 * HISTORY
 * $Log: mio.h,v $
 * Revision 0.3  1994/11/18  20:45:46  mtm
 * Copyright additions/changes
 *
 * Revision 0.2  1993/06/30  22:40:21  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.1  1992/07/09  16:59:04  andyp
 * Initial checkin of Paragon files.
 *
 *	Revision 1.6  92/05/11  09:12:20  rkl
 *	Added MIO Flash structure.
 *	
 *	Revision 1.5  92/03/31  08:31:07  rkl
 *	Fixed many of the _END defines that were using _START instead of _SIZE.
 *	
 *	Revision 1.4  92/03/09  15:05:07  rkl
 *	Changed defines to match Rod's new spec and added defines for SCSI and ETHER
 *	RAM partition sizes.
 *	
 *	Revision 1.3  92/03/05  11:39:12  rkl
 *	Fixes after receive simulation.
 *	
 *	Revision 1.2  92/02/27  17:06:18  rkl
 *	Changes after personal code review.
 *	
 *	Revision 1.1  92/02/21  12:33:42  rkl
 *	Initial revision
 *	
 */

#define	MIO_ETHERNET	1
#define	MIO_SCSI	2
#define	MIO_SERIAL 	3

#define	MIO_MAP_SIZE		0x40000000
#define	MIO_MAP_START		0x80000000
#define	MIO_MAP_END		((MIO_MAP_START + MIO_MAP_SIZE) - 1)

#define	MIO_RAM_SIZE		0x80000
#define	MIO_RAM_START		0x80000000
#define	MIO_RAM_END		((MIO_RAM_START + MIO_RAM_SIZE) - 1)
#define	MIO_RAM_OFFSET		( MIO_RAM_START - MIO_MAP_START)

#define	MIO_SCSI_SIZE		0x10
#define	MIO_SCSI_START		0x91000000
#define	MIO_SCSI_END		((MIO_SCSI_START + MIO_SCSI_SIZE) - 1)
#define	MIO_SCSI_OFFSET		( MIO_SCSI_START - MIO_MAP_START)

#define	MIO_SERIAL_SIZE		0x10
#define	MIO_SERIAL_START	0x91400000
#define	MIO_SERIAL_END		((MIO_SERIAL_START + MIO_SERIAL_SIZE) - 1)
#define	MIO_SERIAL_OFFSET	( MIO_SERIAL_START - MIO_MAP_START)

#define	MIO_DMA_LOW_SIZE	0x10
#define	MIO_DMA_LOW_START	0xa0800000
#define	MIO_DMA_LOW_END		((MIO_DMA_LOW_START + MIO_DMA_LOW_SIZE) - 1)
#define	MIO_DMA_LOW_OFFSET	( MIO_DMA_LOW_START - MIO_MAP_START)

#define	MIO_DMA_HIGH_SIZE	0x10
#define	MIO_DMA_HIGH_START	0xa0880000
#define	MIO_DMA_HIGH_END	((MIO_DMA_HIGH_START + MIO_DMA_HIGH_SIZE) - 1)
#define	MIO_DMA_HIGH_OFFSET	( MIO_DMA_HIGH_START - MIO_MAP_START)
#define	MOI_PARITY_ERR_OFFSET	MIO_DMA_HIGH_OFFSET
#define	MOI_INTERRUPT_OFFSET	MIO_DMA_HIGH_OFFSET

#define	MIO_PERF_LOW_SIZE	0x10
#define	MIO_PERF_LOW_START	0xa0900000
#define	MIO_PERF_LOW_END	((MIO_PERF_LOW_START + MIO_PERF_LOW_SIZE) - 1)
#define	MIO_PERF_LOW_OFFSET	( MIO_PERF_LOW_START - MIO_MAP_START)

#define	MIO_PERF_HIGH_SIZE	0x10
#define	MIO_PERF_HIGH_START	0xa0980000
#define	MIO_PERF_HIGH_END	((MIO_PERF_HIGH_START + MIO_PERF_HIGH_SIZE) - 1)
#define	MIO_PERF_HIGH_OFFSET	( MIO_PERF_HIGH_START - MIO_MAP_START)
#define MIO_PERF_RESET_OFFSET	MIO_PERF_HIGH_OFFSET

#define	MIO_PERR_RST_SIZE	0x10
#define	MIO_PERR_RST_START	0xa0a00000
#define	MIO_PERR_RST_END	((MIO_PERR_RST_START + MIO_PERR_RST_SIZE) - 1)
#define	MIO_PERF_RST_OFFSET	( MIO_PERR_RST_START - MIO_MAP_START)

#define	MIO_DMA_RW_SIZE		0x10
#define	MIO_DMA_RW_START	0xa0a80000
#define	MIO_DMA_RW_END		((MIO_DMA_RW_START + MIO_DMA_RW_SIZE) - 1)
#define	MIO_DMA_RW_OFFSET	( MIO_DMA_RW_START - MIO_MAP_START)
#define	MIO_LED_OFFSET		MIO_DMA_RW_OFFSET

#define	MIO_INTR_ENA_SIZE	0x10
#define	MIO_INTR_ENA_START	0xa0b00000
#define	MIO_INTR_ENA_END	((MIO_INTR_ENA_START + MIO_INTR_ENA_SIZE) - 1)
#define	MIO_INTR_ENA_OFFSET	( MIO_INTR_ENA_START - MIO_MAP_START)

#define	MIO_LOOPBACK_SIZE	0x10
#define	MIO_LOOPBACK_START	0xa0b80000
#define	MIO_LOOPBACK_END	((MIO_LOOPBACK_START + MIO_LOOPBACK_SIZE) - 1)
#define	MIO_LOOPBACK_OFFSET	( MIO_LOOPBACK_START - MIO_MAP_START)

#define	MIO_CA_SIZE		0x10
#define	MIO_CA_START		0xa0c00000
#define	MIO_CA_END		((MIO_CA_START + MIO_CA_SIZE) - 1)
#define	MIO_CA_OFFSET		( MIO_CA_START - MIO_MAP_START)

#define	MIO_EEPROM_SIZE		0x40000
#define	MIO_EEPROM_START	0xbffc0000
#define	MIO_EEPROM_END		((MIO_EEPROM_START + MIO_EEPROM_SIZE) - 1)
#define	MIO_EEPROM_OFFSET	( MIO_EEPROM_START - MIO_MAP_START)

#define	MIO_ETHER_RAM_SIZE	0x10000
#define	MIO_SCSI_RAM_SIZE	(MIO_RAM_SIZE - MIO_ETHER_RAM_SIZE)

/******************************
 *
 *  MIO Flash structure.
 *
 *****************************/
/*
 *  Common Configuration Area
 */
typedef	struct	config {
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
} config_t;

/*
 *  Board Specific Area.
 */
#define	BD_SPEC_FREE  (0x100 - (			\
			6	/* ether_addr */      + \
			2	/* auto boot lck */   + \
			sizeof(config_t)	      + \
			2	/* CRC */	 ))

typedef struct bd_specific {
	u_char	ether_addr[ 6 ];	/* Ethernet address		*/
	u_short	auto_boot_lck;		/* Auto boot lock flag		*/
	u_char	free[ BD_SPEC_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
} bd_specific_t;

/*
 *  Boot Area
 */
#define	BOOT_FREE  (0x4000 - (				\
			sizeof(config_t)	+	\
			sizeof(bd_specific_t) 	+	\
			2	/* CRC */	))

typedef	struct boot {
	u_char	free[ BOOT_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
} boot_t;
	
/*
 *  Setup Area
 */
#define BOOT_STRS	4
#define BOOT_STR_SIZE	256
#define BOOT_STR_SPACE	(BOOT_STRS * BOOT_STR_SIZE)
#define	SETUP_FREE  (0x800 - (				\
			BOOT_STR_SPACE		+	\
			2	/* CRC */	))

typedef	struct setup {
	char	boot_string[ BOOT_STRS ][ BOOT_STR_SIZE ];/* Boot string array*/
	u_char	free[ SETUP_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
} setup_t;

/*
 *  Diag Area 3
 */
#define	DIAG_3_FREE  (0x2000 - (			\
			sizeof(setup_t)		+	\
			2	/* CRC */	))

typedef struct diag_3 {
	u_char	free[ DIAG_3_FREE ];	/* Free space			*/
	u_short	crc;			/* CRC				*/
} diag_3_t;

/*
 *  Diag Area 2
 */
#define	DIAG_2_FREE	(0x2000 - ( 0 ))

typedef	struct diag_2 {
	u_char	free[ DIAG_2_FREE ];	/* Free space			*/
} diag_2_t;

/*
 *  Diag Area 1
 */
#define	DIAG_1_FREE	(0x38000 - ( 0 ))
typedef struct diag_1 {
	u_char	tftp[ DIAG_1_FREE / 2 ];	/* Free space		*/
	u_char	mio_diag[ DIAG_1_FREE / 2 ];	/* Free space		*/
} diag_1_t;

/*
 *  Composit Flash ROM data type;
 */
typedef struct mio_flash {
	/*
	 *  Main block
	 */
	struct main {
		diag_1_t	diag_1;	/* Diagnostics area 1		*/
	} main;

	/*
	 *  First parameter area
	 */
	struct param_1 {
		diag_2_t	diag_2;	/* Diagnostics area 2		*/
	} param_1;

	/*
	 *  Second parameter area
	 */
	struct param_2 {
		diag_3_t	diag_3;	/* Diagnostics area 3		*/
		setup_t		setup;	/* Setup area			*/
	} param_2;

 	/*
	 *  Boot block
	 */
	struct boot_blk {
		boot_t		boot;	     /* Boot area		*/
		bd_specific_t	bd_specific; /* Board specific area	*/
		config_t	config;	/* Common configuration area	*/
	} boot_blk;
} mio_flash_t;

#include "andy_mio.h"
