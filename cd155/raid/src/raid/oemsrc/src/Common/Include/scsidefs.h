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
#ifndef _SCSIDEFS_H
#define _SCSIDEFS_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		scsidefs.h
 Title:		SCSI Definitions
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:33 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains SCSI standard definitions
*/
/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "sysdep.h"
#include "cdb_struct.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define MAX_LUNS			8	/* maximum number of LUs */

#define LOGICAL_ARRAY_PG_CODE		((u_char) 0x2b)
#define PHYSICAL_ARRAY_PG_CODE		((u_char) 0x2a)
#define REDUNDANT_CONTROLLER_PG_CODE	((u_char) 0x2c)
#define DATE_TIME_PG_CODE		((u_char) 0x2f)
#define PAGE_CONTROL_CURRENT_VALUES	((u_char) 0x00)
#define PAGE_CONTROL_CHANGEABLE_VALUES	((u_char) 0x01)
#define RW_RECOVERY_PG_CODE  		((u_char) 0x01)
#define SAVE_PARAMETERS                 TRUE 
#define PAGE_FORMAT_SCSI_2		1
#define MAX_PAGE_SIZE			1024

/* Drive Status Codes */
#define CHANGE_STAT			0x80
#define MAIN_STAT_04			0x04
#define RECONSTRUCT			0x00

/* Logical Array Page Action Codes */
#define ADD_LUN				0x80
#define DELETE_LUN			0x81
#define MODIFY_LUN			0x82
#define REPLACE_LUN			0x83

/* Logical Array Page LUN Status Codes */
#define OPTIMAL_LUN			0x00
#define NON_FATAL_PARM_MISMATCH		0x10
#define UNDEFINED_LUN			0x01 /* (NOT non-existent) */
#define RECONSTRUCTING			0x11
#define FATAL_PARM_MISMATCH		0x02
#define MULTIPLE_DRIVES_DOWN		0x12
#define AWAITING_FULL_FORMAT		0x22
#define AWAITING_MODIFIED_FORMAT	0x32
#define FORMATTING			0x42

/* INQUIRY Peripheral Qualifier Field Value(s) */
#define NOT_CONNECTED			0x01 /* LUN currently not connected */
#define LUN_CONNECTED			0x00 /* LUN currently connected */
#define LUN_NOT_PRESENT			0x01 /* (same as NOT_CONNECTED) */

/* Mode Parameter Header for a 10-byte cdb */
typedef struct Mode10ParameterHeader {
	unsigned char ModeDataLength[2];
	unsigned char MediumType;
	unsigned char DeviceSpecificParameter;
	unsigned char Reserved1;
	unsigned char Reserved2;
	unsigned char BlockDescriptorLength[2];
} Mode10ParameterHeader_t;

typedef struct Mode10BlockDescriptor {
	u_int32	NumberOfBlocks	:24,
		DensityCode	: 8;
	u_int32	BlockLength	:24,
		reserved	:8;
	} Mode10BlockDescriptor_t;

typedef struct logical_array_page {
	Mode10ParameterHeader_t	Header;
	Mode10BlockDescriptor_t	BlockDescriptor;
	u_char	Page_Code:				7,
		Parameters_Savable:			1;
	u_char	Page_Length;
	u_char	Action_Status;
	u_char	RAID_Level;
	u_char	LUN_Type;
	u_char	LUN_Block_Size[4];
	u_char	Drive_Sector_Size[2];
	u_char	LUN_Number_of_Blocks[4];
	u_char	Reserved_1[4];
	u_char	LUN_Segment_Size[4];
	u_char	Segment_Zero_Size;
	/* LUN Flags */
	/* 1st byte */
	u_char	Type_of_Mode_Page_to_Sense:		2,
		Reserved_6:				6;
	/* 2nd byte */
	u_char	Reserved_2:				1,
		Auto_Detect_Replace_Disabled:		1,
		Degraded_Modes_Writes_Disabled:		1,
		Array_Assurance_Disabled:		1,
		AEN_Polling_Enabled:			1,
		Parity_Verification_Enabled:		1,
		Wr_Parity_Verification_Enabled:		1,
		Reserved_3:				1;
	u_char	Block_Reconstructed[4];
	u_char	Reconstruction_Frequency;
	u_char	Reconstruction_Amount[2];
	u_char	Lun_Number;
	u_char	Reserved_5[2];
	u_char	Disk_Bit_Map[32];
	u_char	Configuration_Table[64];
} logical_array_page_t;

typedef struct physical_array_page {
	Mode10ParameterHeader_t	Header;
	Mode10BlockDescriptor_t	BlockDescriptor;
	u_char	Page_Code:				7,
		Parameters_Savable:			1;
	u_char	Page_Length;
	u_char	Drive_Status[240];
} physical_array_page_t;

typedef struct redundant_controller_page {
	Mode10ParameterHeader_t	Header;
	Mode10BlockDescriptor_t	BlockDescriptor;
	u_char	Page_Code:				6,
		Reserved_1:				1,
		Parameters_Savable:			1;
	u_char	Page_Length;
	u_char	Controller_Serial_No[16];
	u_char	Alt_Controller_Serial_No[16];
	u_char	RDAC_Mode[2];
	u_char	Alt_RDAC_Mode[2];
	u_char	Reserved_2;
	u_char	Reserved_3;
	u_char	Controller_LUN_Table[64];
	u_char	Reserved_4;
	u_char	Reserved_5;
} redundant_controller_page_t;

typedef struct rw_recovery_page {
	Mode10ParameterHeader_t	Header;
	Mode10BlockDescriptor_t	BlockDescriptor;
	u_char	Page_Code:		7,
		Parameters_Savable:	1;
	u_char	Page_Length;
	u_char	DCR:			1,
		DTE:			1,
		PER:			1,
		EER:			1,
		RC:			1,
		TB:			1,
		ARRE:			1,
		AWRE:			1;
	u_char	Read_Retry_Count;
	u_char	Correction_Span;
	u_char	Head_Offset_Count;
	u_char	Data_Strobe_Offset_Count;
	u_char	Reserved1;
	u_char	Write_Retry_Count;
	u_char	Reserved2;
	u_char	Recovery_Time_Limit[2];
} rw_recovery_page_t;

typedef struct date_time_page {
	Mode10ParameterHeader_t	Header;
	Mode10BlockDescriptor_t	BlockDescriptor;
	u_char	Page_Code:				6,
		Reserved_1:				1,
		Parameters_Savable:			1;
	u_char	Reserved_2;
	u_char	Reserved_3;
	u_char	Number_of_Seconds[4];
} date_time_page_t;

typedef struct recovery_actions {	/* embedded in sense data */
	u_short	Reserved			:1,
		Information_Logged		:1,
		Drive_Isolation			:1,
		Bus_Isolation			:1,
		SCSI_Chip_Level_Zeros		:1,
		Drive_Assurance			:1,
		Array_Assurance			:1,
		Synchronous_Re_Negotiation	:1,
		Asynchronous_Transfer		:1,
		SCSI_Sequences_Overriden	:1,
		Bus_Or_Device_Reset		:1,
		Start_Stop_Command		:1,
		Block_Reassigned		:1,
		RTZ_Command			:1,
		Drive_Command_Broken_Down	:1,
		Parity_Used			:1;
} recovery_actions_t;

typedef struct fru_group_qualifier {	/* embedded in sense data */
	u_char	SDP_Chip			:1,
		SCSI_Chip_0			:1,
		SCSI_Chip_1			:1,
		SCSI_Chip_2			:1,
		SCSI_Chip_3			:1,
		SCSI_Chip_4			:1,
		SCSI_Chip_5			:1,
		Reserved_1			:1;
	u_char	Reserved_2			:4,
		SCSI_Cable			:1,
		Drive_Power_Supply		:1,
		Drive_Electronics		:1,
		Drive_HDA			:1;
} fru_group_qualifier_t;

typedef struct additional_sense {
/* request sense data bytes - Error Recovery Information 18-40 */
	u_char	Recovery_Actions[2];
	u_char	Total_Errors;
	u_char	Total_Retry_Count;
	u_char	Error_Stack[4];
	u_char	Additional_FRU_Info[8];
	u_char	Error_Specific_Information[3];
	u_char	Error_Detection_Point[4];
/* request sense data bytes - Environment Information 41-108 */
	u_char	Original_CDB[10];
	u_char	Host_ID;
	u_char	Host_Descriptor[2];
	u_char	Serial_Number[16];
	u_char	Revision_Level[4];
	u_char	Data_Transfer_Operation;
	u_char	LUN;
	u_char	LUN_Status;
	u_char	Drive_Identifier;
	u_char	Transfer_Start_Id;
	u_char	Transfer_Start_LBA[4];
	u_char	Drive_Product_ID[16];
	u_char	Power_Up_Status[2];
	u_char	RAID_Level;
/* request sense data bytes - Drive Sense Data 102-135 */
	u_char	Drive_Sense_Id[2];
	u_char	Drive_Sense_Data[32];
/* request sense data bytes - Expansion Area 136-159 */
	u_char	Reserved_Area[24];
} additional_sense_t;

#endif
