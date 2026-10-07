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
#ifndef _DAU_ERR_H
#define _DAU_ERR_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dau_err.h
 Title:		Disk Array Utility Error Message Definitions
 Version:	   
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:56 $
 Programmer:	sjr
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991-92, NCR Corp.

 Description:	This module contains array utility error message definitions.

*/

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/

#define SCSI_NULL_ERROR		0
#define NO_MESSAGE		0
#define INVALID_SELECT		1
#define INVALID_RANGE    	2 
#define SET_BLKRECON_ERR 	3
#define SET_AAD_ERR      	4
#define READ_AAD_ERR     	5
#define SET_ADR_ERR       	6
#define READ_ADR_ERR     	7
#define SET_ARRE_ERR     	8
#define READ_ARRE_ERR    	9
#define SET_AWRE_ERR      	10
#define READ_AWRE_ERR    	11
#define SET_BLKSIZ_ERR   	12
#define READ_BLKSIZ_ERR  	13
#define SET_DMW_ERR      	14
#define READ_DMW_ERR     	15
#define SET_DRVMAP_ERR   	16
#define READ_DRVMAP_ERR 	17
#define SET_DRVSTS_ERR		18
#define READ_DRVSTS_ERR  	19
#define SET_LUNSTS_ERR   	20
#define READ_LUNSTS_ERR  	21
#define SET_NBLKS_ERR	  	22
#define READ_NBLKS_ERR   	23
#define SET_PER_ERR      	24
#define READ_PER_ERR     	25
#define SET_RAIDLV_ERR   	26
#define READ_RAIDLV_ERR  	27
#define SET_RCAMT_ERR    	28
#define READ_RCAMT_ERR        	29
#define SET_RCFREQ_ERR   	30
#define READ_RCFREQ_ERR  	31
#define SET_RRETRY_ERR  	32
#define READ_RRETRY_ERR  	33
#define SET_SECSIZ_ERR   	34
#define READ_SECSIZ_ERR  	35
#define SET_SEGSIZ_ERR  	36
#define READ_SEGSIZ_ERR  	37
#define SET_SG0SIZ_ERR   	38
#define READ_SG0SIZ_ERR  	39
#define SET_WRETRY_ERR   	40
#define READ_WRETRY_ERR  	41
#define SET_TIME_ERR     	42
#define READ_TIME_NI_ERR    	43
#define DISPLAY_TIME_NI_ERR 	44
#define READ_RCPROG_ERR  	45
#define SET_RCPROG_ERR   	46
#define NO_INQUIRY_DATA  	47
#define INVALID_HWADDR		48
#define CMDLINE_SYNTAX_ERROR	49
#define ARG_RANGE_ERROR		50
#define IOB_RANGE_ERROR		51
#define SLOT_RANGE_ERROR	52
#define SBUS_RANGE_ERROR	53
#define SPUN_RANGE_ERROR	54
#define SLUN_RANGE_ERROR	55
#define DAC_SBUS_RANGE_ERROR	56
#define DAC_SPUN_RANGE_ERROR	57
#define BAD_DEV_NAME		58
#define INVALID_PF_OPTS		59
#define NO_ERROR_MESSAGE	60
#define FILE_LOCK_ERROR		61
#define FILE_OPEN_ERROR		62
#define OUT_OF_MEMORY_MSG	63
#define GENERAL_FILE_OPEN_ERROR_MSG	64
#define GENERAL_FILE_NOT_FOUND_MSG	65
#define GENERAL_DEV_OPEN_ERROR_MSG	66
#define IOCTL_FAILURE_MSG		67
#define DOWNLOAD_TIMEOUT_MSG		68
#define CAN_FILE_OPEN_ERROR		69
#define SYSTEM_CALL_ERROR		70
#define FORMAT_ERROR			71
#define INAPPROPRIATE_RAIDLVL_MSG	72
#define NOT_SINGLE_USER_MSG		73
#define NOT_OFFLINE_MSG			74
#define NEW_SCSI_DEV_FAIL	80
#define SCSI_LOCK_FAIL		81
#define READ_CAPACITY_FAIL	82
#define MISCOMP_RANGE_ERROR	83
#define SCSI_VERIFY_ERROR	84
#define VERIFY_FAIL_NOSENSE	85
#define SCSI_READ_ERROR		86
#define SCSI_WRTVERIFY_ERROR	87
#define CONV_MAJ_MIN_ERROR	88
#define BAD_RELID_OPEN		89
#define BAD_OS_REV_LEVEL	90
#define MODE_SENSE_ERROR	91
#define MODE_SELECT_ERROR	92
#define EXCLUSIVE_ACCESS_ERROR	93
#define SET_RCMDE_ERR		94
#define READ_RCMDE_ERR		95
#define SET_ARCMDE_ERR		96
#define READ_ARCMDE_ERR		97
#define SET_MDEPGE_ERR       	98
#define READ_MDEPGE_ERR     	99
#define SCSI_REPPARITY_ERROR	100
#define RECONSTRUCT_RAID_ZERO	101

#ifdef IN_ERROR_C
static char *error_table[] =
{
		/* Error Messages  0 thru  4 */
	"",
	"Invalid selection",
	"Error - entry out of proper range",
	"Error in setting highest block number reconstructed",
	"Error in setting Disable Array Assurance Flag",

		/* Error Messages  5 thru  9 */
	"Error in reading Disable Array Assurance Flag",
	"Error in setting Auto Detect Replacement Flag",
	"Error in reading Auto Detect Replacement Flag",
	"Error in setting ARRE Flag",
	"Error in reading ARRE Flag",
		
		/* Error Messages 10 thru 14 */
	"Error in setting AWRE Flag",
	"Error in reading AWRE Flag",
	"Error in setting Block Size",
	"Error in reading Block Size",
	"Error in setting Degrade Mode Writes Flag",

		/* Error Messages 15 thru 19 */
	"Error in reading Degrade Mode Writes Flag",
	"Error in setting Drive Map",
	"Error in reading Drive Map",
	"Error in setting Drive Status",
	"Error in reading Drive Status",

		/* Error Messages 20 thru 24 */
	"Error in setting LUN Status",
	"Error in reading LUN Status",
	"Error in setting LUN Capacity",
	"Error in reading LUN Capacity",
	"Error in setting PER Flag",

		/* Error Messages 25 thru 29 */
	"Error in reading PER Flag",
	"Error in setting Raid Level",
	"Error in reading Raid Level",
	"Error in setting Reconstruction Amount",
	"Error in reading Reconstruction Amount",

		/* Error Messages 30 thru 34 */
	"Error in setting Reconstruction Frequency",
	"Error in reading Reconstruction Frequency",
	"Error in setting Read Retry Count",
	"Error in reading Read Retry Count",
	"Error in setting Sector Size",

		/* Error Messages 35 thru 39 */
	"Error in reading Sector Size",
	"Error in setting Segment Size",
	"Error in reading Segment Size",
	"Error in setting Segment 0 Size",
	"Error in reading Segment 0 Size",

		/* Error Messages 40 thru 44 */
	"Error in setting Write Retry Count",
	"Error in reading Write Retry Count",
	"Error in setting DAC Time",
	"dac_time.read not implemented", 
	"dac_time.display not implemented",

		/* Error Messages 45 thru 49 */
	"Error in reading reconstruction progress", 
	"Error - illegal to set reconstruction progress",
	"SCSI-related error - could not obtain inquiry data",
	"Invalid hardware address",
	"Command line syntax error",

		/* Error Messages 50 thru 54 */
	"Argument range error",
	"Argument range error\nSystem I/O bus (m delimiter) should be in the range 0-7",
	"Argument range error\nSystem SCSI slot (c delimiter) should be in the range 0-15",
	"Argument range error\nSystem SCSI bus (b delimiter) should be in the range 0-7",
	"Argument range error\nSystem SCSI device ID (p delimiter) should be in the range 0-15",

		/* Error Messages 55 thru 59 */
	"Argument range error\nSystem SCSI LUN (l delimiter) should be in the range 0-7",
	"Argument range error\nDisk array controller SCSI bus (b delimiter) should be in the range 1-15",
	"Argument range error\nDisk array controller SCSI device ID (p delimiter) should be in the range 0-15",
	"Invalid device name on command line",
	"Invalid combination of PUN CUTOFF and FIRST PUN options",

		/* Error Messages 60 thru 64 */
	"Error - amd message does not exist",
	"Error - could not lock the amd message file",
	"Error - could not open the amd message file",
	"Error - could not allocate memory",
	"Error - could not open file",

		/* Error Messages 65 thru 69 */
	"Error - file does not exist",
	"Error - could not open device",
	"Error - I/O control function failed",
	"Error - timed out waiting for controller to come ready after download",
	"Error - cannot open the file of pre-defined configurations",

		/* Error Messages 70 thru 74 */
	"Error - cannot execute pre-defined configuration script",
	"Error - cannot perform format operation",
	"Device RAID level is inappropriate for this function",
	"Must be in single-user mode to repair parity on root",
	"Cannot repair parity on an open volume",

		/* Error Messages 75 thru 79 */
	"",
	"",
	"",
	"",
	"",

		/* Error Messages 80 thru 84 */
	"Error - could not open SCSI device",
	"Error - could not lock SCSI device",
	"Error - READ CAPACITY failure",
	"Error - Miscompared block not in expected range",
	"SCSI-related error during Block_Verify",

		/* Error Messages 85 thru 89 */
	"Error - SCSI verify failed, but no sense data is available",
	"Error - SCSI error encountered during READ operation",
	"Error - SCSI error encountered during WRITE_VERIFY operation",
	"Error - Could not convert hw-addr to device major/minor number",
	"Error - Could not determine the operating system revision level",

		/* Error Messages 90 thru 94 */
	"Error - Incorrect operating system revision level",
	"Error - MODE SENSE failure",
	"Error - MODE SELECT failure",
	"Error - Cannot obtain exclusive access to the device",
	"Error in setting redundant controller mode",

		/* Error Messages 95 thru 99 */
	"Error in reading redundant controller mode",
	"Error in setting alternate redundant controller mode",
	"Error in reading alternate redundant controller mode",
	"Error in setting Type of Mode Page to Sense Flag",
	"Error in reading Type of Mode Page to Sense Flag",

		/* Error Messages 100 thru 104 */
	"Error - SCSI error encountered during REPAIR_PARITY operation",
	"Reconstruction of Raid 0 occurred - Restore data from backup media",
	"",
	"",
	""
};
#endif

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
error( int errcode, int msgcode);

#endif
