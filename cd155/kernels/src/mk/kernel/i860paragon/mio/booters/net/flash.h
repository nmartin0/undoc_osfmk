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
#define	SETUP_FREE  (0x800 - (				\
			512	/* Boot str */	+	\
			2	/* CRC */	))

typedef	struct setup {
	u_char	boot_string[ 512 ];	/* Boot string			*/
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
	u_char	free[ DIAG_1_FREE ];	/* Free space			*/
} diag_1_t;

/*
 *  Composit Flash ROM data type;
 */
typedef struct mio_flash {
	/*
	 *  Main block
	 */
	struct {
		diag_1_t	diag_1;	/* Diagnostics area 1		*/
	} main;

	/*
	 *  First parameter area
	 */
	struct {
		diag_2_t	diag_2;	/* Diagnostics area 2		*/
	} param_1;

	/*
	 *  Second parameter area
	 */
	struct {
		diag_3_t	diag_3;	/* Diagnostics area 3		*/
		setup_t		setup;	/* Setup area			*/
	} param_2;

 	/*
	 *  Boot block
	 */
	struct {
		boot_t		boot;	/* Boot area			*/
		bd_specific_t	bd_specific; /* Board specific area	*/
		config_t	config;	/* Common configuration area	*/
	} boot;
} mio_flash_t;
