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
 
#ifndef _CDB_STRUCT_H
#define _CDB_STRUCT_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		cdb_struct.h
 Title:		Command Discriptor Block (CDB) Structures 
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/10 23:58:24 $ 
 Programmer:	lsg
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the Command Descriptor Block templates
		for the SCSI commands that are used by the Array Host Utilities.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <sys/types.h>

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
/*Major number for all SCSI devices */
#define SCSI_MAJOR		35

/* SCSI Command Codes  - 6 byte commond codes are defined in SCSI.h */
#define SCSI_WriteBuffer	0x3b
#define SCSI_Read_10		0x28
#define SCSI_Write_10		0x2a
#define SCSI_LogSelect		0x4c
#define SCSI_LogSense		0x4d
#define SCSI_ModeSense_10	0x5a
#define SCSI_ModeSelect_10	0x55


/* SCSI 2 CDB Structures for the SCSI commands used by the Array Utilities */
typedef struct Inquiry_CDB {
	u_int 	Op_Code		: 8,
	      	EVPD		: 1,
		Resv1		: 4,
		LUN		: 3,
	      	Pg_Code		: 8,
	      	Reserved	: 8;
	u_char	Allocation_Length;
	u_char	Control_byte;
	} Inquiry_CDB_t;

typedef struct Read_Write_CDB {
	u_char	Op_Code;
	u_char	RelAdr		: 1,
		Resv1		: 2,
		FUA		: 1,
		DPO		: 1,
		LUN		: 3;
	u_char	Logical_Blk_Addr[4];
	u_char	Resv2;
	u_char  Transfer_Length[2];	
	u_char  Control_Byte;
	} Read_Write_CDB_t;

typedef struct Mode_Sense_CDB {
	u_char	Op_Code		: 8;
	u_char	Resv1		: 3,
		DBD		: 1,
		Resv2		: 1,
		LUN		: 3;
	u_char	Pg_Code		: 6,
		Pg_Ctrl		: 2;
	u_char	Resv3[4];
	u_char	Allocation_Length[2];
	u_char	Control_Byte;
	} Mode_Sense_CDB_t;

typedef struct Mode_Select_CDB {
	u_char	Op_Code		: 8;
	u_char  SP		: 1,
		Reserved	: 3,
		PF		: 1,
		LUN		: 3;
	u_char	Resv1[5];
	u_char  Param_List_Length[2];
	u_char  Control_Byte;
	} Mode_Select_CDB_t;

typedef struct Write_Buffer_CDB {
	u_char	Op_Code;
	u_char	Mode	: 3,
		Reserved: 2,
		LUN	: 3;
	union {
		struct {
			u_char	Buffer_ID;
			u_char	Buffer_Offset[3];
			u_char	Param_List_Length[3];
			u_char	Control_Byte;
		} Buffer_Info;
		struct {
			u_char	Ctlr_Addr[4];
			u_char	Transfer_Length[3];
			u_char	Control_Byte;
		} Array_Specific;
	} _8_bytes;
	} Write_Buffer_CDB_t;

typedef struct Send_Recv_Diagnostic_CDB {
	u_char	Op_Code;
	/* first 5 bits are reserved in Recv and
	  defined as follows for Send */
	u_char	UnitOfl		: 1,
		DevOfl		: 1,
		SelfTest	: 1,
		Resv1		: 1,
		Pg_Format	: 1,
		LUN		: 3;
	u_char	Reserved;
	u_char  Length[2];
	u_char  Control_Byte;
	} Send_Recv_Diagnostic_CDB_t;
	
typedef struct Request_Sense_CDB {
	u_char	Op_Code;
	u_char	Reserved 	: 3,
		AEN		: 1,
		Reserved2	: 1,
		LUN		: 3;
	u_char	Resv1;
	u_char	Resv2;
	u_char	Allocation_Length;
	u_char	Control_Byte;
	} Request_Sense_CDB_t;

typedef struct Log_Select_CDB {
	u_char	Op_Code;
	u_char	SP		: 1,
		PCR		: 1,
		Resv1		: 3,
		LUN		: 3;
	u_char	Resv2		: 6,
		Pg_Ctrl		: 2;
	u_char Resv3[4];
	u_char Param_List_Length[2];
	u_char Control_Byte;
	} Log_Select_CDB_t;

typedef struct Log_Sense_CDB {
	u_char	Op_Code;
	u_char	SP		: 1,
		PPC		: 1,
		Resv1		: 3,
		LUN		: 3;
	u_char	Pg_Code		: 6,
		Pg_Ctrl		: 2;
	u_char	Resv2[2];
	u_char	Param_Pointer[2];
	u_char	Allocation_Length[2];
	u_char	Control_Byte;
	} Log_Sense_CDB_t;
#endif
