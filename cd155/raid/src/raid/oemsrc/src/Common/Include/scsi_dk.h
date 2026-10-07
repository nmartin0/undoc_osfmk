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
#ifndef _SCSI_DK_H
#define _SCSI_DK_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		scsi_dk.h
 Title:		SCSI_DK Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:30 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the SCSI_DK class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "scsi_dev.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/

/* New command */
#define SCSI_RepairParity	0x06

#define	SCSI_DK_CLASS	SCSI_DEV_CLASS					\
	int (*read_capacity) ( void *this, u_char *data,		\
				u_long block_addr, u_int pmi );		\
	int (*verify) ( void *this, u_char *data, 			\
			u_long byte_count, u_long block_addr,		\
			u_short block_count, u_int pcheck );		\
	int (*write_verify) ( void *this, u_char *data,			\
			u_long byte_count, u_long block_addr,		\
			u_short block_count, u_int pcheck );		\
	int (*format_unit) ( void *this, u_char *data,			\
			u_int byte_count, u_int fmtdata, u_int cmplist,	\
			     u_int listfmt, u_int interleave );		\
	int (*is_root) ( void *this );					\
	int (*repair_parity) ( void *this, u_char *data,		\
			u_int byte_count);
/* end SCSI_DK_CLASS */

typedef struct scsi_dk {
	SCSI_DK_CLASS
} SCSI_DK;

/* SCSI 2 CDB Structures */
typedef struct Read_Capacity_CDB {
	u_char	Op_Code;
	u_char	RelAdr		: 1,
		Resv1		: 4,
		LUN		: 3;
	u_char	Logical_Blk_Addr[4];
	u_char	Resv2[2];
	u_char	PMI		: 1,
		Resv3		: 7;
	u_char	Control_Byte;
	} Read_Capacity_CDB_t;

typedef struct Verify_CDB {
	u_char	Op_Code;
	u_char	RelAdr		: 1,
		BytChk		: 1,
		Resv1		: 1,
		ParityChk	: 1,
		DPO		: 1,
		LUN		: 3;
	u_char	Logical_Blk_Addr[4];
	u_char	Resv2;
	u_char	Verification_Length[2];
	u_char	Control_Byte;
	} Verify_CDB_t;

typedef struct Write_Verify_CDB {
	u_char	Op_Code;
	u_char	RelAdr		: 1,
		BytChk		: 1,
		Resv1		: 1,
		ParityChk	: 1,
		DPO		: 1,
		LUN		: 3;
	u_char	Logical_Blk_Addr[4];
	u_char	Resv2;
	u_char	Transfer_Length[2];
	u_char	Control_Byte;
	} Write_Verify_CDB_t;
		
typedef struct Format_Unit_CDB {
	u_char	Op_Code;
	u_char	Defect_List_Fmt	: 3,
		CmpLst		: 1,
		FmtData		: 1,
		LUN		: 3;
	u_char	Vendor_Specific;
	u_char	Interleave[2];
	u_char	Control_Byte;
	} Format_Unit_CDB_t;	

typedef struct Repair_Parity_CDB {
	u_char	Op_Code;
	u_char	Resv1		: 5,
		LUN		: 3;
	u_char	Resv2;
	u_char	Resv3;
	u_char	Resv4;
	u_char	Control_Byte;
	} Repair_Parity_CDB_t;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
SCSI_DK *
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
new_SCSI_DK( hw_zipcode hw_addr, int exclusive );
#else
new_SCSI_DK( u_long hw_addr, int exclusive );
#endif
void
destroy_SCSI_DK( void *this );

#endif
