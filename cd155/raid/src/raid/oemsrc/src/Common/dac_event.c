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
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_event.c
 Title:		DAC_EVENT Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:04 $ 
 Programmer:	sjr
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_EVENT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "stddefs.h"
#include "dac_event.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
/* size of standard sense data in bytes */
#define STD_SENSE_SIZE		(int) 18
/* offset of Command_Specific_Information bytes in sense data block */
#define SENSE_CSI_OFFSET	(u_int) 8
/* offset of FRU byte in sense data block */
#define SENSE_FRU_OFFSET	(u_int) 14
/* offset of SKS field pointer bytes in sense data block */
#define SENSE_SKSFP_OFFSET	(u_int) 16
/* offset of RECOVERY ACTION bytes in sense data block */
#define SENSE_RECOVERY_OFFSET	(u_int) 18
/* offset of total number of errors byte in sense data block */
#define SENSE_TOTERR_OFFSET	(u_int) 20
/* offset of total retry count byte in sense data block */
#define SENSE_TRC_OFFSET	(u_int) 21
/* offset of 1st most recent ASC/ASCQ bytes in sense data block */
#define SENSE_ASCQ1_OFFSET	(u_int) 22
/* offset of 2nd most recent ASC/ASCQ bytes in sense data block */
#define SENSE_ASCQ2_OFFSET	(u_int) 24
/* offset of additional FRU byte in sense data block */
#define SENSE_FRU2_OFFSET	(u_int) 28
/* offset of 1st byte of original CDB in sense data block */
#define SENSE_ORIG_CDB_OFFSET	(u_int) 41
/* offset of Host ID byte in sense data block */
#define SENSE_HOSTID_OFFSET	(u_int) 51
/* offset of Host descriptor bytes in sense data block */
#define SENSE_HDESCRIP_OFFSET	(u_int) 52
/* offset of Array Board Serial Number bytes in sense data block */
#define SENSE_ABSN_OFFSET	(u_int) 54
/* offset of Array Application S/W Revision Level bytes in sense data block */
#define SENSE_AASWRL_OFFSET	(u_int) 70
/* offset of Raid Operation byte in sense data block */
#define SENSE_RAIDOP_OFFSET	(u_int) 74
/* offset of LUN Number byte in sense data block */
#define SENSE_LUNNUM_OFFSET	(u_int) 75
/* offset of LUN Status byte in sense data block */
#define SENSE_LUNSTAT_OFFSET	(u_int) 76
/* offset of Drive Identifier byte in sense data block */
#define SENSE_DRVID_OFFSET	(u_int) 77
/* offset of Transfer Start Drive ID byte in sense data block */
#define SENSE_TSDID_OFFSET	(u_int) 78
/* offset of Disk Transfer Start LBA bytes in sense data block */
#define SENSE_DTSLBA_OFFSET	(u_int) 79
/* offset of Drive Product ID bytes in sense data block */
#define SENSE_DRVPID_OFFSET	(u_int) 83
/* offset of Array Power Up Status bytes in sense data block */
#define SENSE_ARRAYPOS_OFFSET	(u_int) 99
/* offset of Drive Sense ID byte in sense data block */
#define SENSE_DRVSID_OFFSET	(u_int) 102
/* offset of Drive Sense Data bytes in sense data block */
#define SENSE_DRVSDATA_OFFSET	(u_int) 103

/******************************************************************************
 ***			     INTERNAL PROCEDURES			    ***
 *****************************************************************************/

void
describe_SENSE_KEY( fp, sk )
FILE *fp;
int sk;
{
	/* Print description of the sense key */
	fprintf( fp, "  SCSI Sense key: " );
	switch ( sk ) {
		case Key_No_Sense:
			fprintf( fp, "NO SENSE (0x00)\n" ); break;
		case Key_Recovered_Error:
			fprintf( fp, "RECOVERED ERROR (0x01)\n" ); break;
		case Key_Not_Ready:
			fprintf( fp, "NOT READY (0x02)\n" ); break;
		case Key_Medium_Error:
			fprintf( fp, "MEDIUM ERROR (0x03)\n" ); break;
		case Key_Hardware_Error:
			fprintf( fp, "HARDWARE ERROR (0x04)\n" ); break;
		case Key_Illegal_Request:
			fprintf( fp, "ILLEGAL REQUEST (0x05)\n" ); break;
		case Key_Unit_Attention:
			fprintf( fp, "UNIT ATTENTION (0x06)\n" ); break;
		case Key_Data_Protect:
			fprintf( fp, "DATA PROTECT (0x07)\n" ); break;
		case Key_Blank_Check:
			fprintf( fp, "BLANK CHECK (0x08)\n" ); break;
		case Key_Vendor_Specific:
			fprintf( fp, "VENDOR SPECIFIC (0x09)\n" ); break;
		case Key_Copy_Aborted:
			fprintf( fp, "COPY ABORTED (0x0A)\n" ); break;
		case Key_Aborted_Command:
			fprintf( fp, "ABORTED COMMAND (0x0B)\n" ); break;
		case Key_Equal:
			fprintf( fp, "EQUAL (0x0C)\n" ); break;
		case Key_Volume_Overflow:
			fprintf( fp, "VOLUME OVERFLOW (0x0D)\n" ); break;
		case Key_Miscompare:
			fprintf( fp, "MISCOMPARE (0x0E)\n" ); break;
		case Key_Reserved:
			fprintf( fp, "RESERVED (0x0F)\n" ); break;
		default:
			fprintf( fp,
			   "(No match for sense key 0x%02X was found)\n", sk );
			break;
	}
}

void
describe_ASC( THIS, fp, ascode, ascq, sk, text )
DAC_EVENT *THIS;
FILE *fp;
u_char ascode;
u_char ascq;
u_char sk;
char *text;
{
	extern int stat_tbl_size;
	extern stat_tbl_entry_t stat_tbl[];
	int idx;
	int Special = FALSE;
	int Found = FALSE;
	u_int ASQ_save;
	u_int SK_save;
	u_int *plong;
	u_int search_key = 0;
	status_group_t *sgp = (status_group_t *) &search_key;
	void (*pfunction)( DAC_EVENT *this );

	sgp->AS_Code = ascode;
	sgp->AS_Qualifier = ascq;
	/* Note: treat incoming sk==0xFF as a "don't care" */
	if ( sk == 0xFF )
		sgp->Sense_Key = 0;
	else
		sgp->Sense_Key = sk;

	/* Need to check AS_Code here to see if this is one of the "special" */
	/* one's in the ADP-92 F.Spec. Appendix A; one that has a "variable" */
	/* (actually, unspecified) AS_Qualifier.  Fortunately for us, all    */
	/* the "special" entries have a sense_key value of		     */
	/* 4 (hardware error) so we don't have to munge with multiple sense  */
	/* keys here.							     */

	if ( sgp->AS_Code == 0x40 || sgp->AS_Code == 0xA2
	    || sgp->AS_Code == 0xA3 || sgp->AS_Code == 0xA4
	    || sgp->AS_Code == 0xA5 || sgp->AS_Code == 0xA9) {
		Special = TRUE;
		ASQ_save = sgp->AS_Qualifier;
		/* Mask the AS_Qualifier while searching stat_tbl */
		sgp->AS_Qualifier = 0xFF;
	}

	/* OK, now search stat_tbl for a matching entry */
	for ( idx = 0; idx < stat_tbl_size; idx++ ) {
		/* Note: treat incoming sk==0xFF as a "don't care" */
		if ( sk == 0xFF )
			sgp->Sense_Key = stat_tbl[idx].status_group.Sense_Key;
		plong = (u_int *) &stat_tbl[idx].status_group;
		if ( search_key == *plong ) {
			Found = TRUE;
			break;
		}
	}

	/* Restore AS_Qualifier in case the AS_Code was "special" */
	if ( Special )
		/* Restore AS_Qualifier */
		sgp->AS_Qualifier = ASQ_save;

	if ( Found ) {
		/* Conditionally print out pre-description text */
		if ( text != NULL )
			fprintf( fp, "%s\n", text );

		/* Print out actual error text */
		fprintf( fp, "%s", stat_tbl[idx].description );

		/* For diagnostic failure on component, tack the hex code    */
		/* for the component at the end of the error text line along */
		/* with a newline, else just print the newline.		     */
		if ( sgp->AS_Code == 0x40 )
			fprintf( fp, " 0x%02X\n", sgp->AS_Qualifier );
		else
			fprintf( fp, "\n" );

		/* For illegal register values, display the value found */
		if ( sgp->AS_Code == 0xA2 || sgp->AS_Code == 0xA3
		    || sgp->AS_Code == 0xA4 || sgp->AS_Code == 0xA5
		    || sgp->AS_Code == 0xA9)
			fprintf( fp, "\t(Register value = 0x%02X)\n",
			    sgp->AS_Qualifier );

		/* The following function call is for future enhancements.   */
		/* Currently (3/3/91), pfunction always points to a null     */
		/* procedure, print_FRU(), defined in stat_tbl.c	     */
		if ( pfunction = stat_tbl[idx].print_ASC_specific )
			pfunction( THIS );
	}
	else
		fprintf( fp,
		    "\t(Could not find additional sense code/qualifier description for 0x%02X/0x%02X)\n",
		    sgp->AS_Code, sgp->AS_Qualifier );
}

void
describe_FRU( fp, FRU_code, FRU_gq, text )
FILE *fp;
u_char FRU_code;
FRU_Group_Qual_t *FRU_gq;
char *text;
{
	u_char chan_ID, scsi_ID;
	u_short *usp = (u_short *) &(FRU_gq);

	if ( text != NULL )
		fprintf( fp, "  %s Field Replaceable Unit (FRU) belongs to: ",
		    text );
	else
		fprintf( fp, "  Field Replaceable Unit (FRU) belongs to: " );
	switch ( FRU_code ) {
		case 0x01:
			fprintf( fp, "HOST CHANNEL GROUP\n" );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed component(s) to report]\n" );
				break;
			}
			else
				fprintf( fp,
				    "     List of failed components:\n" );
			if ( FRU_gq->Host_Chan_Grp.SCSI_cable )
				fprintf( fp, "\tSCSI CABLE\n" );
			if ( FRU_gq->Host_Chan_Grp.SCSI_chip )
				fprintf( fp, "\tSCSI CHIP\n" );
			break;

		case 0x02:
			fprintf( fp, "CONTROLLER DRIVE INTERFACE GROUP\n" );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed component(s) to report]\n" );
				break;
			}
			else
				fprintf( fp,
				    "     List of failed components:\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch1_SCSI_chip )
				fprintf( fp, "\tCHANNEL 1 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch2_SCSI_chip )
				fprintf( fp, "\tCHANNEL 2 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch3_SCSI_chip )
				fprintf( fp, "\tCHANNEL 3 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch4_SCSI_chip )
				fprintf( fp, "\tCHANNEL 4 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch5_SCSI_chip )
				fprintf( fp, "\tCHANNEL 5 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch6_SCSI_chip )
				fprintf( fp, "\tCHANNEL 6 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch7_SCSI_chip )
				fprintf( fp, "\tCHANNEL 7 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch8_SCSI_chip )
				fprintf( fp, "\tCHANNEL 8 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch9_SCSI_chip )
				fprintf( fp, "\tCHANNEL 9 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch10_SCSI_chip )
				fprintf( fp, "\tCHANNEL 10 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch11_SCSI_chip )
				fprintf( fp, "\tCHANNEL 11 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch12_SCSI_chip )
				fprintf( fp, "\tCHANNEL 12 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch13_SCSI_chip )
				fprintf( fp, "\tCHANNEL 13 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch14_SCSI_chip )
				fprintf( fp, "\tCHANNEL 14 SCSI CHIP\n" );
			if ( FRU_gq->Ctlr_Drive_IF_Grp.ch15_SCSI_chip )
				fprintf( fp, "\tCHANNEL 15 SCSI CHIP\n" );
			break;

		case 0x03:
			fprintf( fp, "CONTROLLER BUFFER GROUP\n" );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed component(s) to report]\n" );
				break;
			}
			if ( FRU_gq->Ctlr_Buf_Grp.mem_device )
				fprintf( fp, "\tFailed MEMORY DEVICE, identifier is: %u\n", FRU_gq->Ctlr_Buf_Grp.mem_device );
			break;

		case 0x04:
			fprintf( fp, "CONTROLLER ARRAY ASIC GROUP\n" );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed component(s) to report]\n" );
				break;
			}
			if ( FRU_gq->Ctlr_Array_ASIC_Grp.SDP_chip )
				fprintf( fp, "\tFailure of SDP CHIP\n" );
			break;

		case 0x05:
			fprintf( fp, "CONTROLLER OTHER GROUP\n" );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed component(s) to report]\n" );
				break;
			}
			else
				fprintf( fp,
				    "     List of failed components:\n" );
			if ( FRU_gq->Ctlr_Other_Grp.micro_proc )
				fprintf( fp, "\tMICROPROCESSOR\n" );
			if ( FRU_gq->Ctlr_Other_Grp.array_ps )
				fprintf( fp, "\tARRAY POWER SUPPLY\n" );
			if ( FRU_gq->Ctlr_Other_Grp.non_vol_mem )
				fprintf( fp, "\tNON-VOLATILE MEMORY\n" );
			break;

		case 0x06:
			fprintf( fp, "SUBSYSTEM GROUP\n" );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed component(s) to report]\n" );
				break;
			}
			else
				fprintf( fp,
				    "     List of failed components:\n" );
			if ( FRU_gq->Subsystem_Group.msb_value == 0x00 )
				fprintf( fp, "\tSubsystem component failure - MSB value 0x00\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x01 )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x01\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x02 )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x02\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x03 )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x03\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x04 )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x04\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x08 )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x08\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x09 )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x09\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x0A )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x0A\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x0B )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x0B\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x0C )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x0C\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x0D )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x0D\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x0E )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x0E\n" );
			if ( FRU_gq->Subsystem_Group.lsb_value == 0x0F )
				fprintf( fp, "\tSubsystem component failure - LSB value 0x0F\n" );
			break;

		case 0x07: case 0x08: case 0x09: case 0x0A:
		case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
			fprintf( fp, "(Reserved group: 0x%02X)\n", FRU_code );
			fprintf( fp, "\t[No individual failed component(s) are reportable]\n" );
			break;

		default:
			fprintf( fp, "DRIVE GROUP\n" );
			chan_ID = FRU_code >> 4;
			scsi_ID = FRU_code & 0x0F;
			fprintf( fp, "     Failure report for drive at controller SCSI channel %u, SCSI ID %u:\n", chan_ID, scsi_ID );
			if ( *usp == 0 ) {
				fprintf( fp, "\t[No individual failed drive component(s) to report]\n" );
				break;
			}
			else
				fprintf( fp,
				    "     List of failed drive components:\n" );
			if ( FRU_gq->Drive_Grp.HDA )
				fprintf( fp, "\tDRIVE HEAD/DISK ASSEMBLY (HDA)\n" );
			if ( FRU_gq->Drive_Grp.ctl_electronics )
				fprintf( fp, "\tDRIVE CONTROLLER ELECTRONICS\n" );
			if ( FRU_gq->Drive_Grp.power_supply )
				fprintf( fp, "\tDRIVE POWER SUPPLY\n" );
			if ( FRU_gq->Drive_Grp.SCSI_cable )
				fprintf( fp, "\tDRIVE SCSI CABLE\n" );
			break;
	}
}

#ifdef SENSE_EXTENDED_DESCRIPTION
void
describe_recovery_action( fp, ra )
FILE *fp;
Recovery_Action_t *ra;
{
	fprintf( fp,
	    "  List of recovery actions performed by the array controller:\n" );
	if ( ra->parity_used )
		fprintf( fp, "\tPARITY USED\n" );
	if ( ra->drive_cmd_broken_down )
		fprintf( fp, "\tDRIVE COMMAND BROKEN DOWN\n" );
	if ( ra->rezero_unit_cmd )
		fprintf( fp, "\tISSUED REZERO UNIT COMMAND\n" );
	if ( ra->reassign_block_cmd )
		fprintf( fp, "\tISSUED REASSIGN BLOCK COMMAND\n" );
	if ( ra->start_stop_unit_cmd )
		fprintf( fp, "\tISSUED START STOP UNIT COMMAND\n" );
	if ( ra->abort_BDR_msg )
		fprintf( fp, "\tISSUED ABORT OR BUS DEVICE RESET MESSAGE\n" );
	if ( ra->reset_sig_drive_chan )
		fprintf( fp, "\tASSERTED RESET SIGNAL ON DRIVE CHANNEL\n" );
	if ( ra->SCSI_op_wo_chip_seq )
		fprintf( fp,
		    "\tRETRIED SCSI OPERATION WITHOUT CHIP SEQUENCES\n" );
	if ( ra->transfer_data_async )
		fprintf( fp, "\tTRANSFERRED DATA ASYNCHRONOUSLY\n" );
	if ( ra->renegot_sync_params )
		fprintf( fp, "\tRE-NEGOTIATED SYNCHRONOUS PARAMETERS\n" );
	if ( ra->array_assur_tests )
		fprintf( fp, "\tPERFORMED ARRAY ASSURANCE TESTS\n" );
	if ( ra->drive_assur_tests )
		fprintf( fp, "\tPERFORMED DRIVE ASSURANCE TESTS\n" );
	if ( ra->drive_cmd_retried )
		fprintf( fp, "\tDRIVE COMMAND RETRIED\n" );
	if ( ra->bus_isolation_action )
		fprintf( fp, "\tPERFORMED BUS ISOLATION ACTIONS\n" );
	if ( ra->drive_isolation_action )
		fprintf( fp, "\tPERFORMED DRIVE ISOLATION ACTIONS\n" );
	if ( ra->info_logged )
		fprintf( fp, "\tINFORMATION LOGGED\n" );
}

void
describe_host( fp, hd )
FILE *fp;
Host_Descriptor_t *hd;
{
	fprintf( fp, "  Host information:\n" );
	if ( hd->wide_16 )
		fprintf( fp, "\tDATA IS BEING TRANSFERRED 16 BITS WIDE\n" );
	if ( hd->wide_32 )
		fprintf( fp, "\tDATA IS BEING TRANSFERRED 32 BITS WIDE\n" );
	if ( hd->msg_using_host )
		fprintf( fp, "\tMESSAGE-USING HOST\n" );
	if ( hd->reselectable_host )
		fprintf( fp, "\tRESELECTABLE HOST\n" );
	if ( hd->sync_host )
		fprintf( fp, "\tSYNCHRONOUS HOST\n" );
	if ( hd->sync_trans_negot )
		fprintf( fp,
		    "\tSYNCHRONOUS TRANSFER NEGOTIATION HAS TAKEN PLACE\n" );
	if ( hd->data_async )
		fprintf( fp, "\tDATA IS BEING TRANSFERRED ASYNCHRONOUSLY\n" );
	if ( hd->AEN_support )
		fprintf( fp, "\tAEN SENSE IS SUPPORTED\n" );
	if ( hd->polled_AEN_support )
		fprintf( fp, "\tPOLLED AEN SENSE IS SUPPORTED\n" );
}

void
describe_raid_op( fp, ucval )
FILE *fp;
u_char ucval;
{
	fprintf( fp,
	    "  RAID function being executed at the time of the error:\n" );
	switch ( ucval ) {
		case 0:
			fprintf( fp, "\t[Could not determine or not significant, (code = %u)]\n", ucval );
			break;
		case 1:
			fprintf( fp, "\tREAD, (code = %u)\n", ucval );
			break;
		case 2:
			fprintf( fp, "\tREAD PORTION OF A READ, MODIFY, WRITE OPERATION, (code = %u)\n", ucval );
			break;
		case 3:
			fprintf( fp, "\tWRITE PORTION OF A READ, MODIFY, WRITE OPERATION, (code = %u)\n", ucval );
			break;
		case 4:
			fprintf( fp, "\tRECOVERY READ, (code = %u)\n", ucval );
			break;
		case 5:
			fprintf( fp, "\tRECOVERY WRITE, (code = %u)\n", ucval );
			break;
		default:
			fprintf( fp, "\t[Reserved, (code = %u)\n", ucval );
			break;
	}
}
#endif	/* SENSE_EXTENDED_DESCRIPTION */

void
dump_sense_data( p )
SCSI_Sense_Data_t *p;
{
	int i, count;
	u_char *c;

	c = (u_char *) p;
	fprintf( stderr,
	    "Standard Sense Data:\n    " );
	for ( i=0; i<=17; i++ )
		fprintf( stderr, "%02X ", c[i] );
	fprintf( stderr, "\nInterpretation:\n" );
	fprintf( stderr,
	    "    0: Valid_Bit = %X, Error_Class = %X, Error_Code = %X\n",
	    p->Valid, p->Error_Class, p->Error_Code );
	fprintf( stderr,
	    "    1: Segment_Number = %02X\n", p->Segment_Number );
	fprintf( stderr,
	    "    2: Filemark = %X, EOM = %X, ILI = %X, Sense_Key = %X\n",
	    p->Filemark, p->End_Of_Media,
	    p->Incorrect_Length_Indicator, p->Sense_Key );
	fprintf( stderr,
	    "  3-6: Information = %02X %02X %02X %02X\n",
	    p->Information_MSB, p->Information_Bytes2,
	    p->Information_Bytes1, p->Information_Bytes0 );
	fprintf( stderr,
	    "    7: Additional_Sense_Length = %02X\n",
	    p->Additional_Length );
	fprintf( stderr,
	    " 8-11: Command_Specific_Information = %02X %02X %02X %02X\n",
	    c[8], c[9], c[10], c[11] );
	fprintf( stderr,
	    "   12: Additional_Sense_Code = %02X\n",
	    p->Additional_Sense_Code );
	fprintf( stderr,
	    "   13: Additional_Sense_Code_Qualifier = %02X\n",
	    p->Additional_Sense_Code_Qualifier );
	fprintf( stderr,
	    "   14: FRU_Code = %02X\n",
	    p->Field_Replaceable_Unit_Code );
	fprintf( stderr,
	    "   15: SKSV = %X, C/D = %X, BPV = %X, Bit_Pointer = %X\n",
	    p->Sense_Key_Specific_Valid,
	    p->Illegal_Req_Cmd_Data,
	    p->Illegal_Req_Bit_Valid,
	    p->Illegal_Req_Bit_Pointer );
	fprintf( stderr,
	    "   16: byte_value = %02X\n",
	    c[16] );
	fprintf( stderr,
	    "   17: byte_value = %02X\n",
	    c[17] );

	if ( (count = p->Additional_Length - 10) > 0 ) {
		fprintf( stderr, "\nAdditional Sense Data:\n    " );
		for ( i = 1; i <= count; i++) {
			fprintf( stderr, "%02X ", c[17+i] );
			if ( i%20 == 0 )
				fprintf( stderr, "\n    " );
			fprintf( stderr, "\n" );
		}
	}
}

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

#define THIS	((DAC_EVENT *) this)

static void
put_identifier( this )
void *this;
{
	char ascii_time[80];

	/* Note: ascii_time receives a newline terminated string of the form */
	/* "Thu Feb 28 10:02:42 1991\n", with a length of 25 char's	     */
	strcpy( ascii_time, ctime( (long *) &(THIS->time_stamp) ) );

	/* The following line is commented out, but there in case the	     */
	/* newline ever needs to be stripped.				     */
	/* ascii_time[strlen(ascii_time)-1] = '\0'; /*  */

	fprintf( THIS->describe_fp,
	    /* Just so you don't have to count, there are 51 spaces here */
	    "                                                   %s",
	    ascii_time );
}

static void
display_ASC_ASCQ ( this )
void *this;
{
	fprintf(THIS->describe_fp,"\n  %x(ASC) %x(ASCQ) -\n",
		THIS->sense_data.Additional_Sense_Code,
		THIS->sense_data.Additional_Sense_Code_Qualifier);
}

static void
no_op( this )
void *this;
{
}

static hw_zipcode
hw_address( this )
void *this;
{
	return( THIS->hw_addr );
}

static int
sense_key( this )
void *this;
{
	return( THIS->sense_data.Sense_Key );
}

static int
info_bytes_valid( this )
void *this;
{
	SCSI_Sense_Data_t *p;

	p = &(THIS->sense_data);
	return( Is_Valid( p ) );
}

static u_long
info_bytes( this )
void *this;
{
	SCSI_Sense_Data_t *p;

	p = &(THIS->sense_data);
	return( Get_Residual( p ) );
}

static void
describe( this )
void *this;
{
	THIS->display_ASC_ASCQ(this);
	fprintf( THIS->describe_fp, "\n\n");
	THIS->put_identifier( this);
	fprintf( THIS->describe_fp, "%s\n", THIS->description );
	fprintf( THIS->describe_fp, "\n\n");
	THIS->describe_sense_specific_part( this );
	fprintf( THIS->describe_fp, "\n\n");
	THIS->describe_detail( this );
	fprintf( THIS->describe_fp, "\n\n\n\n");
}

static void
describe_sense_specific_part( this )
void *this;
{
	FILE *fp = THIS->describe_fp;
	status_group_t sg;

	/* First, print the sense key */
	describe_SENSE_KEY( fp, sense_key( this ) );

	/* Next, print ASC/ASCQ meaning */
	sg.AS_Code = (u_int) THIS->sense_data.Additional_Sense_Code;
	sg.AS_Qualifier =
	    (u_int) THIS->sense_data.Additional_Sense_Code_Qualifier;
	sg.Sense_Key = (u_int) sense_key( this );
	describe_ASC( THIS,
	    fp,
	    (u_char) sg.AS_Code,
	    (u_char) sg.AS_Qualifier,
	    (u_char) sg.Sense_Key,
	    /* choose one of the following; a long pre-description string */
	    "  Analysis of SCSI additional sense code and qualifier (ASC/ASCQ) is:" ); /*  */
	    /* or no pre-description string at all */
	    /* NULL ); /*  */
}


static void
describe_detail( this )
void *this;
{
	extern u_long cs_swap_4();
	extern u_short cs_swap_2();
	u_long ulval;
	u_short usval, *usp;
	u_char ucval;
	int i;
	SCSI_Sense_Data_t *sd = &(THIS->sense_data);
	u_char *sbyte = (u_char *) sd;
	FILE *fp = THIS->describe_fp;
	int sk = sense_key( this );
	u_char FRU1_code = sbyte[SENSE_FRU_OFFSET];
	u_char FRU2_code;
	FRU_Group_Qual_t blank_gq, *FRU1_gq, *FRU2_gq;
	Recovery_Action_t *ra;
	Host_Descriptor_t *hd;
	char array_serial_num[17], rev_level[5];
	status_group_t sg;

/*
#ifdef DEBUG_ON
		dump_sense_data( &(THIS->sense_data) );
#endif
*/

	/* OK, look at the SCSI standard sense data portion of the	     */
	/* sense data block.						     */

	/* Sense key specific bytes */
	if ( sd->Sense_Key_Specific_Valid && sk == Key_Illegal_Request ) {
		usval = cs_swap_2( sd + SENSE_SKSFP_OFFSET );
		ucval = (u_char) sd->Illegal_Req_Bit_Valid ?
		    sd->Illegal_Req_Bit_Pointer : 0;
		if ( sd->Illegal_Req_Cmd_Data )
			fprintf( fp,
			    "  Illegal parameter in CDB block; byte %u",
			    usval );
		else
			fprintf( fp,
			    "  Illegal parameter in DATA block; byte %u",
			    usval );
		if ( ucval )
			fprintf( fp, ", bit %u\n", ucval );
		else
			fprintf( fp, "\n" );
	}
	else if ( sd->Sense_Key_Specific_Valid && ( sk == Key_Recovered_Error
	    || sk == Key_Hardware_Error || sk == Key_Medium_Error ) ) {
		usval = cs_swap_2( sd + SENSE_SKSFP_OFFSET );
		fprintf( fp, "  Array controller recovery algorithm was executed %u times\n", usval );
	}

	/* Command Specific Information; for the DAC this is only used for   */
	/* the REASSIGN BLOCKS command.					     */
	if ( sd->Command_Specific_Information ) {
		ulval = cs_swap_4( sd + SENSE_CSI_OFFSET );
		fprintf( fp,
		    "  REASSIGN BLOCKS command failed for block number: ");
		if ( ulval == 0xFFFFFFFF )
			fprintf( fp, "(not available)\n" );
		else
			fprintf( fp, " %lu (0x%lX)\n", ulval, ulval );
	}

	/* Analysis of FRU code(s) */
	/* Initialize a blank group qualifier and other variables */
	blank_gq.Ctlr_Buf_Grp.mem_device = 0;
	blank_gq.Ctlr_Buf_Grp.reserved = 0;
	FRU1_gq = &blank_gq;
	FRU2_code = 0;
	FRU2_gq = &blank_gq;
	/* Find out if there is a second FRU code */
	/* Check for extended sense data; i.e. more than 18 bytes */
	if ( (int) SCSI_Sense_Size( sd ) > STD_SENSE_SIZE ) {
		FRU1_gq = (FRU_Group_Qual_t *) &(sbyte[SENSE_FRU2_OFFSET-2]);
		FRU2_code = sbyte[SENSE_FRU2_OFFSET];
		FRU2_gq = (FRU_Group_Qual_t *) &(sbyte[SENSE_FRU2_OFFSET+2]);
	}
	if ( FRU1_code ) {
		if ( FRU2_code )
			describe_FRU( fp, FRU1_code, FRU1_gq, "1st" );
		else
			describe_FRU( fp, FRU1_code, FRU1_gq, NULL );
	}
	if ( FRU2_code )
		describe_FRU( fp, FRU2_code, FRU2_gq, "2nd" );

#ifdef SENSE_EXTENDED_DESCRIPTION
	/* Assumption made here: if DAC returns more than STD_SENSE_SIZE     */
	/* (18 bytes) of sense data, it will return 160 bytes (per fspec).   */
	if ( (int) SCSI_Sense_Size( sd ) > STD_SENSE_SIZE ) {

		/* Conditionally break down and print recovery actions */
		ra = &(sbyte[SENSE_RECOVERY_OFFSET]);
		usp = (u_short *) &ra;
		if ( *usp )
			describe_recovery_action( fp, ra );

		/* Conditionally print total errors */
		if ( ucval = sbyte[SENSE_TOTERR_OFFSET] )
			fprintf( fp, "  Total number of errors = %u\t(for this operation)\n", ucval );

		/* Conditionally print total retry count */
		if ( ucval = sbyte[SENSE_TRC_OFFSET] )
			fprintf( fp, "  Total retry count = %u\t(for this operation)\n", ucval );

		/* Break down the ASC/ASCQ stack and conditionally print     */
		/* their meanings.					     */
		usp = (u_short *) &(sbyte[SENSE_ASCQ1_OFFSET]);
		if ( *usp ) {
			sg.AS_Code = (u_int) sbyte[SENSE_ASCQ1_OFFSET];
			sg.AS_Qualifier = (u_int) sbyte[SENSE_ASCQ1_OFFSET+1];
			sg.Sense_Key = (u_int) 0xFF;  /* means "don't care" */
			describe_ASC( THIS, fp, (u_char) sg.AS_Code,
			    (u_char) sg.AS_Qualifier, (u_char) sg.Sense_Key,
			    "  Analysis of 2nd most recent ASC/ASCQ pair is:" );
		}
		usp = (u_short *) &(sbyte[SENSE_ASCQ2_OFFSET]);
		if ( *usp ) {
			sg.AS_Code = (u_int) sbyte[SENSE_ASCQ2_OFFSET];
			sg.AS_Qualifier = (u_int) sbyte[SENSE_ASCQ2_OFFSET+1];
			sg.Sense_Key = (u_int) 0xFF;  /* means "don't care" */
			describe_ASC( THIS, fp, (u_char) sg.AS_Code,
			    (u_char) sg.AS_Qualifier, (u_char) sg.Sense_Key,
			    "  Analysis of 3rd most recent ASC/ASCQ pair is:" );
		}

	/* Break down environment information */

		/* Unconditionally print original CDB */
		fprintf( fp, "  Original Command Descriptor Block (CDB) = %02X", sbyte[SENSE_ORIG_CDB_OFFSET] );
		for ( i=1; i<=9; i++ )
			fprintf( fp, " %02X",
			    sbyte[SENSE_ORIG_CDB_OFFSET+i] );
		fprintf( fp, " (hex)\n" );

		/* Unconditionally print host ID */
		fprintf( fp, "  SCSI ID of host that selected array controller for this command = %u\n", sbyte[SENSE_HOSTID_OFFSET] );

		/* Conditionally print host descriptor info */
		hd = &(sbyte[SENSE_HDESCRIP_OFFSET]);
		usp = (u_short *) &hd;
		if ( *usp )
			describe_host( fp, hd );

		/* Unconditionally print Array Board Serial Number */
		strncpy( array_serial_num, &(sbyte[SENSE_ABSN_OFFSET]), 16 );
		array_serial_num[16] = '\0';
		fprintf( fp, "  Array board serial number = %s\n",
		    array_serial_num );

		/* Unconditionally print Array Appl S/W Rev Level */
		strncpy( rev_level, &(sbyte[SENSE_AASWRL_OFFSET]), 4 );
		rev_level[5] = '\0';
		fprintf( fp,
		    "  Array application software revision level = %s\n",
		    rev_level );

		/* Unconditionally print RAID function at time of error */
		ucval = sbyte[SENSE_RAIDOP_OFFSET];
		describe_raid_op( fp, ucval );

		/* Unconditionally print LUN number */
		ucval = sbyte[SENSE_LUNNUM_OFFSET];
		fprintf( fp, "  Array controller logical unit number (LUN) responding to selection = %u\n", ucval );
		
		/* Not taking this analysis any further for now, since it    */
		/* has been determined that dac_events should not be a	     */
		/* substitute for a good streams error logging analysis	     */
		/* tool.  The remaining fields of the sense data block,	     */
		/* along with there size and byte offsets are:		     */
		/*	LUN Status		(size=1, offset=76)	     */
		/*	Drive Identifier	(size=1, offset=77)	     */
		/*	Trans Start Drive ID	(size=1, offset=78)	     */
		/*	Disk Trans Start LBA	(size=4, offset=79)	     */
		/*	Drive Product ID	(size=16, offset=83)	     */
		/*	Array Power-Up Status	(size=2, offset=99)	     */
		/*	Drive Sense ID		(size=2?, offset=102)	     */
		/*	Drive Sense Data	(size=32?, offset=104)	     */

	} /* end of extended sense data break down */
#endif	/* SENSE_EXTENDED_DESCRIPTION */
}

void
destroy_DAC_EVENT( this )
void *this;
{
	free( THIS->description );
	destroy_OBJECT( this );
}

#undef THIS

DAC_EVENT *
new_DAC_EVENT( hw_addr, sense_data, full_description )
hw_zipcode hw_addr;
SCSI_Sense_Data_t *sense_data;
int full_description;
{
	OBJECT *s;
	DAC_EVENT *this;
	int hc_logical;
	char descrip[256];
	hw_zipcode_t *z = (hw_zipcode_t *) &hw_addr;

	debug( "entered new_DAC_EVENT\n" );
	s = new_OBJECT();
	this = (DAC_EVENT *) zalloc( sizeof( DAC_EVENT ) );
	memcpy( this, s, sizeof( OBJECT ) ); /* inherit from OBJECT */
	free( s );
	memcpy( (char *) &this->sense_data, sense_data, sizeof( SCSI_Sense_Data_t ) );
	if ( ( hc_logical = hc_ptol( hw_addr ) ) == -1 )
#ifdef PARAGON860 /* change SYSTEM BUS to NODE */
		sprintf(descrip, "  Disk array error condition detected for device:\n\tNODE %d, SCSI BUS %d, SCSI ID %d, LUN %d",
		    z->IO_Bus, z->SCSI_Bus, z->PUN, z->LUN);
	else
		sprintf(descrip, "  Disk array error condition detected for device:\n\tNODE %d, SCSI BUS %d, SCSI ID %d, LUN %d",
		    z->IO_Bus, z->SCSI_Bus, z->PUN, z->LUN);
#else
		sprintf(descrip, "  Disk array error condition detected for device:\n\tSYSTEM BUS %d, SCSI ADAPTER ? (SLOT %d), SCSI BUS %d, SCSI ID %d, LUN %d",
		    z->IO_Bus, z->SCSI_Controller, z->SCSI_Bus, z->PUN, z->LUN);
	else
		sprintf(descrip, "  Disk array error condition detected for device:\n\tSYSTEM BUS %d, SCSI ADAPTER %d (SLOT %d), SCSI BUS %d, SCSI ID %d, LUN %d",
		    z->IO_Bus, hc_logical, z->SCSI_Controller, z->SCSI_Bus,
		    z->PUN, z->LUN);
#endif
	this->description = (char *) zalloc( strlen( descrip ) + 1 );
	strcpy( this->description, descrip );
	this->hw_addr = hw_addr;
	this->time_stamp = time( NULL );
	this->describe_sense_specific_part = describe_sense_specific_part;
	this->describe_detail = full_description ? describe_detail : no_op;
	this->put_identifier = full_description ? put_identifier : no_op;
	this->display_ASC_ASCQ = display_ASC_ASCQ;
	this->destroy = destroy_DAC_EVENT;
	this->describe = describe; /* override describe */
	this->hw_address = hw_address; /* override hw_address */
	debug( "leaving new_DAC_EVENT\n" );
	return( this );
}
