/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 
/* Copyright 1990 NCR Corporation - Dayton, Ohio, USA */
#ident	"@(#)./usr/src/arch/mca/uts/i386/sys/scsi:b0.h	1.1"

#if !defined(_SYS_B0_H)
#define	_SYS_B0_H
/*
 *	b0.h
 *
 *	This include file contains the definitions for the boot block.
 */

/*
 * MC88000-based ISL information structure.
 */
typedef struct ISL {
	unsigned long	ISL_pointer;
	unsigned long	ISL_length;
	unsigned long	ISL_execution_address;
	unsigned long	ISL_load_address;
} ISL_t;

/*
 * Old (SVR3) Partition information.
 */
typedef struct Partition {
	int	PartitionSize;
	long	PartitionStart;
} Disk_Partition_t;

/*
 * Disk bootblock structure.
 */
#define DBB_SIZE		512
#define DBB_STRUCTURE_SIZE	(400 + DRIVE_TYPE_SIZE + (NUMBER_OF_ISLs * sizeof(ISL_t)) + 4)
#define NUMBER_OF_ISLs		4
#define DRIVE_TYPE_SIZE		32
#define DBB_INITIALIZED		0x55aa66cc
#define DBB_NO_AUTOBOOT		0
#define DBB_AUTOBOOT		1

typedef struct DBB_structure {
	/*
	 * Compatible fields.
	 */
	unsigned char		dbb_reserved_1[20];
	unsigned long		dbb_ISL_compatible_pointer;
	unsigned short		dbb_ISL_compatible_length;
	unsigned char		dbb_ISL_compatible_execution_address[4];
	unsigned char		dbb_ISL_compatible_load_address[4];
	unsigned char		dbb_reserved_2[30];
	unsigned long		dbb_diagnostic_area_pointer;
	unsigned short		dbb_diagnostic_area_length;
	unsigned char		dbb_reserved_3[10];
	unsigned long		dbb_initialized_flag;
	unsigned char		dbb_autoboot_flag;
	unsigned char		dbb_reserved_4[170];
	Disk_Partition_t	dbb_partitions[16];
	unsigned char		dbb_reserved_5[16];
	/*
	 * New fields -- offset is 400 from the beginning of the boot block.
	 */
	unsigned char		dbb_drive_type[DRIVE_TYPE_SIZE];
	ISL_t			dbb_ISL_information[NUMBER_OF_ISLs];
	unsigned long		dbb_DCB_pointer;
	unsigned char		dbb_reserved_6[DBB_SIZE - DBB_STRUCTURE_SIZE];
} DBB_structure_t;
#endif
