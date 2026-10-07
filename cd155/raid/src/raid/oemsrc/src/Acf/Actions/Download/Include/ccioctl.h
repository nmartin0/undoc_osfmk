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
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
 Name:		$RCSfile: ccioctl.h,v $
 Version:	$Id: ccioctl.h,v 1.2.4.1 1995/06/11 23:24:55 kat Exp $
  Title:	Common controller I/O control definitions (used by dac download 
		only to ensure proper compilation)
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:24:55 $
		(last change by: $Author: kat $)
  Programmer:	rmj
  Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


  COPYRIGHT 1992, NCR Corporation

  Description:	This module is used by the dac download only to ensure proper
		compilation.
*/

/******************************************************************************
 ***				VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:24:55 $"
#undef H_ID


/*============================================================================
			IOCTL Functions
============================================================================*/

/* IOCTL Commands */
#define CC_CHECK_STATUS					0x00
#define CC_RETURN_STATUS				0x01
#define CC_WAIT_READY					0x02
#define CC_SET_CONFIGURATION			0x03
#define CC_GET_CONFIGURATION			0x04
#define CC_RETURN_IPC_STATISTICS		0x05
#define CC_CLEAR_IPC_STATISTICS			0x06
#define CC_SET_TRACE_MODE				0x07
#define CC_RESET_BOARD					0x08
#define CC_ISSUE_CHANNEL_COMMAND		0x09
#define CC_REINITIALIZE_DRIVER			0x0A
#define CC_RETURN_ROUTER_STATISTICS		0x0B
#define CC_CLEAR_ROUTER_STATISTICS		0x0C



/* CPF Channel Commands */
#define	CH_CMD_BASE						0x20
#define	CH_NO_OP						CH_CMD_BASE+0
#define	CH_SET_BOARD_CONFIGURATION		CH_CMD_BASE+1
#define	CH_GET_BOARD_CONFIGURATION		CH_CMD_BASE+2
#define	CH_RESET_SUSPEND_BOARD			CH_CMD_BASE+3
#define	CH_SET_SUSPENSION_MODE			CH_CMD_BASE+4
#define	CH_SET_TERMINAL_MODE			CH_CMD_BASE+5
#define	CH_GET_STATISTICS				CH_CMD_BASE+6
#define	CH_SET_DATE_TIME_FROM_HOST		CH_CMD_BASE+7
#define	CH_GET_DATE_TIME_FOR_HOST		CH_CMD_BASE+8
#define	CH_ACTIVATE_FUNCTION			CH_CMD_BASE+9
#define	CH_GET_ACTIVATE_RESULTS			CH_CMD_BASE+10
#define	CH_FILL_MEMORY					CH_CMD_BASE+11
#define	CH_DOWNLOAD						CH_CMD_BASE+12
#define	CH_UPLOAD						CH_CMD_BASE+13
#define	CH_SET_START_ADDRESS			CH_CMD_BASE+14
#define	CH_TERMINAL_DATA				CH_CMD_BASE+15
#define	CH_CLOSE_CPF_CHANNEL			CH_CMD_BASE+16
#define CH_ENTER_DEBUG					CH_CMD_BASE+17
#define	CH_CLEAR_STATISTICS				CH_CMD_BASE+18
#define	CH_SET_PACKET_MODE				CH_CMD_BASE+19
#define	CH_INPUT_PACKET_DATA			CH_CMD_BASE+20
#define	CH_OUTPUT_PACKET_DATA			CH_CMD_BASE+21
#define CH_PACKET_ACKNOWLEDGE			CH_CMD_BASE+22
#define CH_TRACE_FUNCTION				CH_CMD_BASE+23

/* CPF Channel Statuses */
#define	CH_OK_STATUS						0x80
#define	CH_INVALID_PARAMETER_STATUS			0x81
#define	CH_CHECKSUM_ERROR_STATUS			0x82
#define	CH_DATA_TOO_LONG_ERROR_STATUS		0x83
#define	CH_TERMINAL_NOT_ACTIVE_STATUS		0x84
#define	CH_ALREADY_ACTIVATED_STATUS			0x85
#define	CH_PACKET_NOT_ACTIVE_STATUS			0x86
#define	CH_PACKET_REJECTED					0x87
#define CH_NO_TRACE_STATUS					0X88
#define	CH_ACTIVATED_COMPLETE_FLAG			0x0100
#define	CH_PACKET_OUTPUT_REQUESTED			0x0200
#define	CH_PACKET_INPUT_REQUESTED			0x0400

#define CH_VALID_CHANNEL_STATUS_LOW			0x80
#define CH_VALID_CHANNEL_STATUS_HIGH		0x85

/*============================================================================
			IOCTL Data Structures
============================================================================*/

struct CHPB
{
	lword	command;			/* CPF Channel command */
	lword	status;				/* Returned status code */
	lword	length;				/* Byte length in data buffer */
	word	buffer[(256+10)/2];	/* Data buffer */
};

