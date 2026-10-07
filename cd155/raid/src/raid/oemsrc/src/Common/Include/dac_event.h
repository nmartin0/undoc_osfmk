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
#ifndef _DAC_EVENT_H
#define _DAC_EVENT_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_event.h
 Title:		DAC_EVENT Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:50 $ 
 Programmer:	sjr
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DAC_EVENT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DAC_EVENT_CLASS	OBJECT_CLASS					\
	int full_description;						\
	hw_zipcode hw_addr;						\
	u_long time_stamp;						\
	SCSI_Sense_Data_t sense_data;					\
	void (*put_identifier)( void *this );				\
	void (*display_ASC_ASCQ) ( void *this );			\
	void (*describe_sense_specific_part)( void *this );		\
	void (*describe_detail)( void *this );				\
	int (*sense_key) ( void *this );				\
	int (*info_bytes_valid) ( void *this );				\
	u_long (*info_bytes) ( void *this );
/* end DAC_EVENT_CLASS */

typedef struct dac_event {
	DAC_EVENT_CLASS
} DAC_EVENT;

/* Definitions used with status decode table */
typedef struct status_group {
	u_int	Sense_Key	:4,
		AS_Code		:8,
		AS_Qualifier	:8;
} status_group_t;

typedef struct stat_tbl_entry {
	status_group_t status_group;
	char *description;
	void (*print_ASC_specific)( DAC_EVENT *this );
} stat_tbl_entry_t;

/* definitions for FRU analysis */
/* group 0x01 */
typedef struct host_channel_group {
	u_short	SCSI_cable	: 1,
		SCSI_chip	: 1,
		reserved2	: 6,
		reserved	: 8;
} host_channel_grp_t;
		
/* group 0x02 */
typedef struct ctlr_drive_if_group {
	u_short ch1_SCSI_chip	: 1,
		ch2_SCSI_chip	: 1,
		ch3_SCSI_chip	: 1,
		ch4_SCSI_chip	: 1,
		ch5_SCSI_chip	: 1,
		ch6_SCSI_chip	: 1,
		ch7_SCSI_chip	: 1,
		ch8_SCSI_chip	: 1,
		ch9_SCSI_chip	: 1,
		ch10_SCSI_chip	: 1,
		ch11_SCSI_chip	: 1,
		ch12_SCSI_chip	: 1,
		ch13_SCSI_chip	: 1,
		ch14_SCSI_chip	: 1,
		ch15_SCSI_chip	: 1,
		reserved	: 1;
} ctlr_drive_if_grp_t;
		
/* group 0x03 */
typedef struct ctlr_buffer_group {
	u_short	mem_device	: 8,
		reserved	: 8;
} ctlr_buf_grp_t;
		
/* group 0x04 */
typedef struct ctlr_array_asic_group {
	u_short	SDP_chip	: 1,
		reserved2	: 7,
		reserved	: 8;
} ctlr_array_asic_grp_t;
		
/* group 0x05 */
typedef struct ctlr_other_group {
	u_short	micro_proc	: 1,
		array_ps	: 1,
		non_vol_mem	: 1,
		reserved2	: 5,
		reserved	: 8;
} ctlr_other_grp_t;
		
/* group 0x06 */
typedef struct subsystem_group {
	u_char  msb_value;
	u_char  lsb_value;
} subsystem_group_t;

/* group 0x10 */
typedef struct drive_group {
	u_short	HDA		: 1,
		ctl_electronics	: 1,
		power_supply	: 1,
		SCSI_cable	: 1,
		reserved2	: 4,
		reserved	: 8;
} drive_grp_t;
		
/* OK, now a union for all the above structures */
typedef union FRU_group_qual {
	host_channel_grp_t	Host_Chan_Grp;
	ctlr_drive_if_grp_t	Ctlr_Drive_IF_Grp;
	ctlr_buf_grp_t		Ctlr_Buf_Grp;
	ctlr_array_asic_grp_t	Ctlr_Array_ASIC_Grp;
	ctlr_other_grp_t	Ctlr_Other_Grp;
	subsystem_group_t 	Subsystem_Group;
	drive_grp_t		Drive_Grp;
} FRU_Group_Qual_t;
	 
/* structure for recovery action bytes of the sense data block */
typedef struct Recovery_Actions {
	u_short
	  /* byte 19 */
		transfer_data_async	: 1,
		renegot_sync_params	: 1,
		array_assur_tests	: 1,
		drive_assur_tests	: 1,
		drive_cmd_retried	: 1,
		bus_isolation_action	: 1,
		drive_isolation_action	: 1,
		info_logged		: 1,
	  /* byte 18 */
		parity_used		: 1,
		drive_cmd_broken_down	: 1,
		rezero_unit_cmd		: 1,
		reassign_block_cmd	: 1,
		start_stop_unit_cmd	: 1,
		abort_BDR_msg		: 1,
		reset_sig_drive_chan	: 1,
		SCSI_op_wo_chip_seq	: 1;
} Recovery_Action_t;
		
/* structure for host descriptor info bytes of the sense data block */
typedef struct Host_Descriptor {
	u_short
	  /* byte 53 */
		msg_using_host		: 1,
		reselectable_host	: 1,
		sync_host		: 1,
		sync_trans_negot	: 1,
		data_async		: 1,
		AEN_support		: 1,
		polled_AEN_support	: 1,
		reserved2		: 1,
	  /* byte 52 */
		wide_16			: 1,
		wide_32			: 1,
		reserved1		: 6;
} Host_Descriptor_t;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DAC_EVENT *
new_DAC_EVENT( hw_zipcode hw_addr, SCSI_Sense_Data_t *sense_data,
	       int full_description );

void
destroy_DAC_EVENT( void *this );

/* The following two functions are defined in mk_dacev.c */
DAC_EVENT *
make_dacevent( hw_zipcode hw_addr, u_long SCSI_status,
	       int full_description );

int
chk4_dacevent( int RtnStat, char *msg );

#endif
