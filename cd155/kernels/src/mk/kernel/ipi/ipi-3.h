/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: ipi-3.h,v $
 * Revision 1.2  1994/11/18  20:50:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:16  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi-3.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Definitions from the Intelligent Peripheral Interface (IPI)
 *	Part 3: Device generic command set for magnetic and optical disk drives
 */

#ifndef	_IPI_3_H_
#define	_IPI_3_H_

#include <ipi/ipi_endian.h>

#define	ISO_IEC_9318_3


/******************************************************************************\
 * Commands, codes and aliases						      *
 *									      *
 * IPI-3 opcodes are interpreted as follows:				      *
 *									      *
 *	0x00 - 0x0f	control commands				      *
 *	0x10 - 0x2f	transfer commands				      *
 *	0x30 - 0x3f	combination commands				      *
 *	0x40 - 0x4f	postition commands				      *
 *	0x50 - 0x6f	other transfer commands				      *
 *	0x70 - 0x7f	reserved					      *
 *	0x80 - 0x9f	diagnostic commands				      *
 *	0xa0 - 0xbf	communication commands				      *
 *	0xc0 - 0xdf	reserved					      *
 *	0xe0 - 0xef	vendor-unique commands				      *
 *	0xf0 - 0xfe	reserved					      *
 *	0xff		asynchronous response indentifier		      *
\******************************************************************************/

/*
 * Basic command packet
 */
typedef struct {
	unsigned char	packet_length_msb;	/* bytes excluding this field */
	unsigned char	packet_length_lsb;	/* bytes excluding this field */
	unsigned char	reference_msb;		/* command identificaton tag  */
	unsigned char	reference_lsb;		/* command identificaton tag  */
	unsigned char	opcode;			/* operation		      */
	unsigned char	modifier;		/* common[7..4], opcode[3..0] */
#define		IPI_COMMON_MOD_MASK		0xf0
#define		IPI_OPCODE_MOD_MASK		0x0f
	/* common modifiers */
#define		IPI_INDIVIDIAL_CMD		0x00
#define		IPI_QUEUED_CMD			0x00
#define		IPI_CHAINED_CMD			0x10
#define		IPI_SEQUENTIAL_CMD		0x20
#define		IPI_ORDERED_CMD			0x30
#define		IPI_PRIORITY_CMD		BIT(6)
					     /* BIT(7) - reserved */
	unsigned char	slave;			/* slave (0x00 - 0x07)	      */
	unsigned char	facility;		/* facility (0x00 - 0xfe)     */
#define		IPI_SLAVE_ONLY			0xff
} ipi_command_t;


/******************************************************************************\
 *			Control Commands (0x00 - 0x0f)			      *
\******************************************************************************/

#define	IPI_CMD_NOP					0x00

#define IPI_CMD_FACILITY_OPERATION			0x01

#define	IPI_CMD_ATTRIBUTES				0x02
#define		IPI_CMD_ATTRIBUTES_REPORT				0x00
#define		IPI_CMD_ATTRIBUTES_INITIALIZE				0x01
#define		IPI_CMD_ATTRIBUTES_RESTORE				0x02
#define		IPI_CMD_ATTRIBUTES_LOAD					0x09
#define		IPI_CMD_ATTRIBUTES_SAVE					0x0a

#define	IPI_CMD_REPORT_ADDRESSEE_STATUS			0x03
#define		IPI_CMD_REPORT_ADDRESSEE_STATUS_CONDITION		BIT(0)
#define		IPI_CMD_REPORT_ADDRESSEE_STATUS_STATUS			BIT(1)
#define		IPI_CMD_REPORT_ADDRESSEE_STATUS_PORT_QUERY		BIT(2)

#define	IPI_CMD_PORT_ADDRESS				0x04
#define		IPI_CMD_PORT_ADDRESS_RESERVE				0
#define		IPI_CMD_PORT_ADDRESS_RELEASE				BIT(0)
#define		IPI_CMD_PORT_ADDRESS_PRIORITY_RESERVE			BIT(1)
#define		IPI_CMD_PORT_ADDRESS_NOTIFY_ALTERNATES			BIT(2)

#define	IPI_CMD_PATH_CONTROL				0x05
#define		IPI_CMD_PATH_CONTROL_PURGE_COMMANDS			BIT(0)
#define		IPI_CMD_PATH_CONTROL_PATH_SELECT			BIT(1)

#define	IPI_CMD_ATTENTION_CONTROL			0x06
#define		IPI_CMD_ATTENTION_CONTROL_ENABLE			0x00
#define		IPI_CMD_ATTENTION_CONTROL_DISABLE			0x02
#define		IPI_CMD_ATTENTION_CONTROL_CLEAR				0x04
#define		IPI_CMD_ATTENTION_CONTROL_SET				0x06

#define	IPI_CMD_OPERATING_MODE				0x07
/*
 * NOTE:
 *	The polarity of the set/report bit is reversed
 *	between ISO/IEC 9318-3 and ISO/IEC 9318-4.
 */
#ifdef	ISO_IEC_9318_3
#define		IPI_CMD_OPERATING_MODE_REPORT				BIT(2)
#define		IPI_CMD_OPERATING_MODE_SET				0
#endif
#ifdef	ISO_IEC_9318_4
#define		IPI_CMD_OPERATING_MODE_REPORT				0
#define		IPI_CMD_OPERATING_MODE_SET				BIT(2)
#endif

#define	IPI_CMD_ABORT					0x08
#define		IPI_CMD_ABORT_ORDERLY_TERMINATION			BIT(0)
#define		IPI_CMD_ABORT_TERMINATE_COMMAND				BIT(1)
#define		IPI_CMD_ABORT_TERMINATE_ALL				BIT(2)

#define	IPI_CMD_ACCESS_PERMITS				0x09
#define		IPI_CMD_ACCESS_PERMITS_REPORT				0x00
#define		IPI_CMD_ACCESS_PERMITS_INITIALIZE			0x01
#define		IPI_CMD_ACCESS_PERMITS_RESTORE				0x02
#define		IPI_CMD_ACCESS_PERMITS_DATA_BLOCK			0
#define		IPI_CMD_ACCESS_PERMITS_PHYSICAL_BLOCK			BIT(2)
#define		IPI_CMD_ACCESS_PERMITS_LOAD				0x09
#define		IPI_CMD_ACCESS_PERMITS_SAVE				0x0a

#define	IPI_CMD_RESUME					0x0a

#define	IPI_CMD_PORT_RESPONSE				0x0b

#define	IPI_CMD_ANTICIPATED_ACTION			0x0c

#define	IPI_CMD_OPERATOR_DISPLAY			0x0d
#define		IPI_CMD_OPERATOR_DISPLAY_REPORT				BIT(2)
#define		IPI_CMD_OPERATOR_DISPLAY_SET				0


/******************************************************************************\
 *			Position Commands (0x40 - 0x4f)			      *
\******************************************************************************/

#define	IPI_CMD_POSITION_CONTROL			0x41
#define		IPI_CMD_POSITION_CONTROL_OCTET_COUNT			0
#define		IPI_CMD_POSITION_CONTROL_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_POSITION_CONTROL_DATA_BLOCK			0
#define		IPI_CMD_POSITION_CONTROL_PHYSICAL_BLOCK			BIT(2)

#define	IPI_CMD_REPORT_POSITION				0x42
#define		IPI_CMD_REPORT_POSITION_OCTET_COUNT			0
#define		IPI_CMD_REPORT_POSITION_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_REPORT_POSITION_DATA_BLOCK			0
#define		IPI_CMD_REPORT_POSITION_PHYSICAL_BLOCK			BIT(2)

#define	IPI_CMD_REPORT_DISCONTINUITY			0x44
#define		IPI_CMD_REPORT_DISCONTINUITY_OCTET_COUNT		0
#define		IPI_CMD_REPORT_DISCONTINUITY_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_REPORT_DISCONTINUITY_SEARCH			0
#define		IPI_CMD_REPORT_DISCONTINUITY_LIST			BIT(1)
#define		IPI_CMD_REPORT_DISCONTINUITY_DATA_BLOCK			0
#define		IPI_CMD_REPORT_DISCONTINUITY_PHYSICAL_BLOCK		BIT(2)


/******************************************************************************\
 *			Transfer Commands (0x10 - 0x2f)			      *
\******************************************************************************/

#define	IPI_CMD_READ					0x10
#define		IPI_CMD_READ_OCTET_COUNT				0
#define		IPI_CMD_READ_BLOCK_COUNT				BIT(0)
#define		IPI_CMD_READ_DATA_RECOVERY_ON				0
#define		IPI_CMD_READ_DATA_RECOVERY_OFF				BIT(1)
#define		IPI_CMD_READ_DATA_BLOCK					0
#define		IPI_CMD_READ_PHYSICAL_BLOCK				BIT(2)
#define		IPI_CMD_READ_FORWARD					0
#define		IPI_CMD_READ_REVERSE					BIT(3)

#define	IPI_CMD_READ_RAW_DATA				0x11
#define		IPI_CMD_READ_RAW_DATA_OCTET_COUNT			0
#define		IPI_CMD_READ_RAW_DATA_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_READ_RAW_DATA_DATA_RECOVERY_OFF			BIT(1)
#define		IPI_CMD_READ_RAW_DATA_DATA_BLOCK			0
#define		IPI_CMD_READ_RAW_DATA_PHYSICAL_BLOCK			BIT(2)
#define		IPI_CMD_READ_RAW_DATA_FORWARD				0
#define		IPI_CMD_READ_RAW_DATA_REVERSE				BIT(3)

#define	IPI_CMD_READ_REPLICATED_DATA			0x12
#define		IPI_CMD_READ_REPLICATED_DATA_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_READ_REPLICATED_DATA_DATA_RECOVERY_ON		0
#define		IPI_CMD_READ_REPLICATED_DATA_DATA_RECOVERY_OFF		BIT(1)
#define		IPI_CMD_READ_REPLICATED_DATA_DATA_BLOCK			0
#define		IPI_CMD_READ_REPLICATED_DATA_PHYSICAL_BLOCK		BIT(2)

#define	IPI_CMD_SEARCH					0x18
#define		IPI_CMD_SEARCH_OCTET_COUNT				0
#define		IPI_CMD_SEARCH_BLOCK_COUNT				BIT(0)
#define		IPI_CMD_SEARCH_DATA_RECOVERY_ON				0
#define		IPI_CMD_SEARCH_DATA_RECOVERY_OFF			BIT(1)
#define		IPI_CMD_SEARCH_DATA_BLOCK				0
#define		IPI_CMD_SEARCH_PHYSICAL_BLOCK				BIT(2)
#define		IPI_CMD_SEARCH_FORWARD					0
#define		IPI_CMD_SEARCH_REVERSE					BIT(3)

#define	IPI_CMD_WRITE					0x20
#define		IPI_CMD_WRITE_OCTET_COUNT				0
#define		IPI_CMD_WRITE_BLOCK_COUNT				BIT(0)
#define		IPI_CMD_WRITE_DATA_BLOCK				0
#define		IPI_CMD_WRITE_PHYSICAL_BLOCK				BIT(2)
#define		IPI_CMD_WRITE_FORWARD					0
#define		IPI_CMD_WRITE_REVERSE					BIT(3)

#define	IPI_CMD_WRITE_PATTERN				0x21
#define		IPI_CMD_WRITE_PATTERN_OCTET_COUNT			0
#define		IPI_CMD_WRITE_PATTERN_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_WRITE_PATTERN_DATA_BLOCK			0
#define		IPI_CMD_WRITE_PATTERN_PHYSICAL_BLOCK			BIT(2)
#define		IPI_CMD_WRITE_PATTERN_FORWARD				0
#define		IPI_CMD_WRITE_PATTERN_REVERSE				BIT(3)

#define	IPI_CMD_FORMAT					0x28
#define		IPI_CMD_FORMAT_OCTET_COUNT				0
#define		IPI_CMD_FORMAT_BLOCK_COUNT				BIT(0)
#define		IPI_CMD_FORMAT_INHIBIT_DEFECT_REALLOCATION		BIT(1)
#define		IPI_CMD_FORMAT_DATA_BLOCK				0
#define		IPI_CMD_FORMAT_PHYSICAL_BLOCK				BIT(2)
#define		IPI_CMD_FORMAT_INITIALIZE_FORMAT			BIT(3)


/******************************************************************************\
 *			Combination Commands (0x30 - 0x3f)		      *
\******************************************************************************/

#define	IPI_CMD_COPY					0x30

#define	IPI_CMD_COMPARE_SLAVE_DATA			0x31

#define	IPI_CMD_COMPARE_DATA				0x32

#define	IPI_CMD_REALLOCATE				0x33
#define		IPI_CMD_REALLOCATE_RELOCATE_DATA			BIT(0)

#define	IPI_CMD_ALLOCATE_RESTORE			0x34
#define		IPI_CMD_ALLOCATE_RESTORE_RELOCATE_DATA			BIT(0)

#define	IPI_CMD_SHADOW_READ				0x35

#define	IPI_CMD_SHADOW_WRITE				0x36

#define	IPI_CMD_SHADOW_RESTORE				0x37


/******************************************************************************\
 *			Other Transfer Commands (0x50 - 0x6f)		      *
\******************************************************************************/

#define	IPI_CMD_READ_VERIFY				0x50
#define		IPI_CMD_READ_VERIFY_OCTET_COUNT				0
#define		IPI_CMD_READ_VERIFY_BLOCK_COUNT				BIT(0)
#define		IPI_CMD_READ_VERIFY_HIGH_MARGINS			BIT(1)
#define		IPI_CMD_READ_VERIFY_DATA_BLOCK				0
#define		IPI_CMD_READ_VERIFY_PHYSICAL_BLOCK			BIT(2)
#define		IPI_CMD_READ_VERIFY_FORWARD				0
#define		IPI_CMD_READ_VERIFY_REVERSE				BIT(3)

#define	IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA		0x51
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_OCTET_COUNT	0
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_BLOCK_COUNT	BIT(0)
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_DATA_RECOVERY_ON	0
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_DATA_RECOVERY_OFF	BIT(1)
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_DATA_BLOCK		0
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_PHYSICAL_BLOCK	BIT(2)
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_FORWARD		0
#define		IPI_CMD_READ_AT_FIRST_AVAILABLE_DATA_REVERSE		BIT(3)

#define	IPI_CMD_READ_FROM_BUFFER			0x52

#define	IPI_CMD_READ_FACILITY_DATA_TO_BUFFER		0x53
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_OCTET_COUNT	0
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_BLOCK_COUNT	BIT(0)
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_DATA_RECOVERY_ON	0
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_DATA_RECOVERY_OFF	BIT(1)
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_DATA_BLOCK		0
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_PHYSICAL_BLOCK	BIT(2)
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_FORWARD		0
#define		IPI_CMD_READ_FACILITY_DATA_TO_BUFFER_REVERSE		BIT(3)

#define	IPI_CMD_READ_PHYSICAL_DATA_AND_ECC		0x54
#define		IPI_CMD_READ_PHYSICAL_DATA_AND_ECC_OCTET_COUNT		0
#define		IPI_CMD_READ_PHYSICAL_DATA_AND_ECC_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_READ_PHYSICAL_DATA_AND_ECC_ECC			0
#define		IPI_CMD_READ_PHYSICAL_DATA_AND_ECC_SYNDROME		BIT(1)
#define		IPI_CMD_READ_PHYSICAL_DATA_AND_ECC_FORWARD		0
#define		IPI_CMD_READ_PHYSICAL_DATA_AND_ECC_REVERSE		BIT(3)

#define	IPI_CMD_READ_PHYSICAL_HEADER			0x55
#define		IPI_CMD_READ_PHYSICAL_HEADER_OCTET_COUNT		0
#define		IPI_CMD_READ_PHYSICAL_HEADER_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_READ_PHYSICAL_HEADER_DATA_RECOVERY_ON		0
#define		IPI_CMD_READ_PHYSICAL_HEADER_DATA_RECOVERY_OFF		BIT(1)
#define		IPI_CMD_READ_PHYSICAL_HEADER_FORWARD			0
#define		IPI_CMD_READ_PHYSICAL_HEADER_REVERSE			BIT(3)

#define	IPI_CMD_READ_IPL				0x56

#define	IPI_CMD_READ_PHYSICAL_HEADER_AND_ECC		0x57
#define		IPI_CMD_READ_PHYSICAL_HEADER_AND_ECC_ECC		0
#define		IPI_CMD_READ_PHYSICAL_HEADER_AND_ECC_SYNDROME		BIT(1)
#define		IPI_CMD_READ_PHYSICAL_HEADER_AND_ECC_FORWARD		0
#define		IPI_CMD_READ_PHYSICAL_HEADER_AND_ECC_REVERSE		BIT(3)

#define	IPI_CMD_WRITE_TO_BUFFER				0x62

#define	IPI_CMD_WRITE_BUFFER_TO_FACILITY		0x63
#define		IPI_CMD_WRITE_BUFFER_TO_FACILITY_OCTET_COUNT		0
#define		IPI_CMD_WRITE_BUFFER_TO_FACILITY_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_WRITE_BUFFER_TO_FACILITY_DATA_BLOCK		0
#define		IPI_CMD_WRITE_BUFFER_TO_FACILITY_PHYSICAL_BLOCK		BIT(2)
#define		IPI_CMD_WRITE_BUFFER_TO_FACILITY_FORWARD		0
#define		IPI_CMD_WRITE_BUFFER_TO_FACILITY_REVERSE		BIT(3)

#define	IPI_CMD_WRITE_PHYSICAL_DATA_AND_ECC		0x64
#define		IPI_CMD_WRITE_PHYSICAL_DATA_AND_ECC_OCTET_COUNT		0
#define		IPI_CMD_WRITE_PHYSICAL_DATA_AND_ECC_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_WRITE_PHYSICAL_DATA_AND_ECC_FORWARD		0
#define		IPI_CMD_WRITE_PHYSICAL_DATA_AND_ECC_REVERSE		BIT(3)

#define	IPI_CMD_WRITE_PHYSICAL_HEADER			0x65
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_OCTET_COUNT		0
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_BLOCK_COUNT		BIT(0)
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_FORWARD			0
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_REVERSE			BIT(3)

#define	IPI_CMD_LOAD_SLAVE_IML				0x66
#define		IPI_CMD_LOAD_SLAVE_IML_LOAD				0x00
#define		IPI_CMD_LOAD_SLAVE_IML_RELOAD				0x01
#define		IPI_CMD_LOAD_SLAVE_IML_REPORT				0x02
#define		IPI_CMD_LOAD_SLAVE_IML_STORE				0x04

#define	IPI_CMD_ERASE					0x67
#define		IPI_CMD_ERASE_OCTET_COUNT				0
#define		IPI_CMD_ERASE_BLOCK_COUNT				BIT(0)
#define		IPI_CMD_ERASE_DATA_BLOCK				0
#define		IPI_CMD_ERASE_PHYSICAL_BLOCK				BIT(2)
#define		IPI_CMD_ERASE_FORWARD					0
#define		IPI_CMD_ERASE_REVERSE					BIT(3)

#define	IPI_CMD_WRITE_PHYSICAL_HEADER_AND_ECC		0x68
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_AND_ECC_OCTET_COUNT	0
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_AND_ECC_BLOCK_COUNT	BIT(0)
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_AND_ECC_FORWARD		0
#define		IPI_CMD_WRITE_PHYSICAL_HEADER_AND_ECC_REVERSE		BIT(3)


/******************************************************************************\
 *			Diagnostic Commands (0x80 - 0x9f)		      *
\******************************************************************************/

#define	IPI_CMD_PERFORM_SLAVE_DIAG			0x80
#define		IPI_CMD_PERFORM_SLAVE_DIAG_OCTET_COUNT			0
#define		IPI_CMD_PERFORM_SLAVE_DIAG_BLOCK_COUNT			BIT(0)

#define	IPI_CMD_PERFORM_FACILITY_DIAG			0x81
#define		IPI_CMD_PERFORM_FACILITY_DIAG_OCTET_COUNT		0
#define		IPI_CMD_PERFORM_FACILITY_DIAG_BLOCK_COUNT		BIT(0)

#define	IPI_CMD_READ_DEFECT_LIST			0x82
#define		IPI_CMD_READ_DEFECT_LIST_OCTET_COUNT			0
#define		IPI_CMD_READ_DEFECT_LIST_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_READ_DEFECT_LIST_PERMANENT			0
#define		IPI_CMD_READ_DEFECT_LIST_TEMPORARY			BIT(1)
#define		IPI_CMD_READ_DEFECT_LIST_WORKING			0
#define		IPI_CMD_READ_DEFECT_LIST_SUSPECT			BIT(2)
#define		IPI_CMD_READ_DEFECT_LIST_SEQUENTIAL			0
#define		IPI_CMD_READ_DEFECT_LIST_CHRONOLOGICAL			BIT(3)

#define	IPI_CMD_WRITE_DEFECT_LIST			0x83
#define		IPI_CMD_WRITE_DEFECT_LIST_OCTET_COUNT			0
#define		IPI_CMD_WRITE_DEFECT_LIST_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_WRITE_DEFECT_LIST_PERMANENT			0
#define		IPI_CMD_WRITE_DEFECT_LIST_TEMPORARY			BIT(1)
#define		IPI_CMD_WRITE_DEFECT_LIST_CLEAR_DEFECT_LIST		BIT(2)
#define		IPI_CMD_WRITE_DEFECT_LIST_APPEND			0
#define		IPI_CMD_WRITE_DEFECT_LIST_CREATE			BIT(3)

#define	IPI_CMD_READ_ERROR_LOG				0x84
#define		IPI_CMD_READ_ERROR_LOG_OCTET_COUNT			0
#define		IPI_CMD_READ_ERROR_LOG_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_READ_ERROR_LOG_CLEAR_ERROR_LOG			BIT(1)

#define	IPI_CMD_WRITE_ERROR_LOG				0x85
#define		IPI_CMD_WRITE_ERROR_LOG_OCTET_COUNT			0
#define		IPI_CMD_WRITE_ERROR_LOG_BLOCK_COUNT			BIT(0)
#define		IPI_CMD_WRITE_ERROR_LOG_APPEND				0
#define		IPI_CMD_WRITE_ERROR_LOG_CREATE				BIT(3)

#define	IPI_CMD_DIAGNOSTIC_CONTROL			0x90
#define		IPI_CMD_DIAGNOSTIC_CONTROL_OCTET_COUNT			0
#define		IPI_CMD_DIAGNOSTIC_CONTROL_BLOCK_COUNT			BIT(0)


/******************************************************************************\
 *			Asynchronous Response Opcode (0xff)		      *
\******************************************************************************/

#define	IPI_CMD_ASYNC_RESPONSE				0xff


/******************************************************************************\
 * IPI-3 parameters are required to start on an even boundary		      *
 * and are interpreted as follows:					      *
 *									      *
 *	0x00		invalid						      *
 *	0x01		NOP						      *
 *	0x02		continuation of preceding parameter		      *
 *	0x03 - 0x0f	reserved					      *
 *	0x10 - 0x1f	slave major status				      *
 *	0x20 - 0x2f	facility major status				      *
 *	0x30 - 0x4f	common parameters				      *
 *	0x50 - 0x9f	command parameters				      *
 *	0xa0 - 0xbf	reserved					      *
 *	0xc0 - 0xcf	communication parameters			      *
 *	0xd0 - 0xef	vendor-unique parameters			      *
 *	0xf0 - 0xff	host adapter unique parameters			      *
\******************************************************************************/

/*
 * Parameter structure
 *
 *	A parameter must begin on an even byte boundary.
 *	A pad byte of 0x00 must be added following a parameter
 *	with a total length of an odd number of bytes.
 */
typedef struct {
	unsigned char	length;
	unsigned char	id;
	unsigned char	data[1];	/* variable length */
} ipi_param_t;


/******************************************************************************\
 * Unless otherwise noted, parameters without lengths defined are variable.   *
 *									      *
 * Originator:	M-aster	S-lave	B-oth					      *
 * Support:	O-ptional						      *
\******************************************************************************/

/******************************************************************************\
 *			NOP Parameter (0x01)				      *
\******************************************************************************/

#define	IPI_PARAM_NOP					0x01	/* B   */

/******************************************************************************\
 *			Continuation Parameter (0x02)			      *
\******************************************************************************/

#define	IPI_PARAM_CONTINUATION				0x02	/* B   */


/******************************************************************************\
 *			Slave Major Status Parameters (0x10 - 0x1f)	      *
\******************************************************************************/

#define	IPI_PARAM_SLAVE_MSG_CODE_EXCEPTION		0x13	/* S   */
typedef struct {
	unsigned char	length;			/* message param length       */
	unsigned char	id;			/* message param id	      */
	BITFIELD_8 (unsigned char,		/* exception bits	      */
			port_response	     : 1,
			port_disable_pending : 1,
			failure_message	     : 1, /* <-			      */
			code_execution_error : 1, /*   |- mutually exclusive  */
			message	  	     : 1, /* <-			      */
			slave_unable_to_iml  : 1,
			request_master_to_iml: 1,
			code_not_accepted    : 1);
	BITFIELD_2 (unsigned char,		/* more exception bits	      */
			reserved0	     : 7,
			facility_status	     : 1);
	unsigned char	reserved1;
	unsigned char	reserved2;
	unsigned char	extended_substatus[1];
} ipi_slave_msg_excep_t;

#define	IPI_PARAM_SLAVE_INTERVENTION_REQUIRED		0x14	/* S   */
typedef struct {
	unsigned char	length;			/* intervention param length  */
	unsigned char	id;			/* intervention param id      */
	BITFIELD_8 (unsigned char,		/* intervention bits	      */
			reserved0		   : 1,
			addressee_busy		   : 1,
			attribute_table_corrupted  : 1,
			physical_link_failure	   : 1,
			not_ready_transition	   : 1,
			not_p_available_transition : 1,
			not_ready		   : 1,
			not_p_available		   : 1);
	unsigned char	reserved1;
	unsigned char	reserved2;
	unsigned char	reserved3;
	unsigned char	extended_substatus[1];
} ipi_slave_intervention_t;

#define	IPI_PARAM_SLAVE_ALT_PORT_EXCEPTION		0x15	/* S   */
typedef struct {
	unsigned char	length;			/* alternate port param len   */
	unsigned char	id;			/* alternate port param id    */
	BITFIELD_6 (unsigned char,		/* alternate port bits	      */
			reserved0	   : 3,
			facility_switched  : 1,
			format_complete	   : 1,
			init_complete	   : 1,
			attributes_updated : 1,
			priority_reserve   : 1);
	BITFIELD_3 (unsigned char,		/* more alternate port bits   */
			reserved1	   : 6,
			diag_terminated	   : 1,
			diag_in_progress   : 1);
	unsigned char	reserved2;
	unsigned char	reserved3;
	unsigned char	extended_substatus[1];
} ipi_slave_alt_excep_t;

#define	IPI_PARAM_SLAVE_MACHINE_EXCEPTION		0x16	/* S   */
typedef struct {
	unsigned char	length;			/* machine param length	      */
	unsigned char	id;			/* machine param id	      */
	BITFIELD_8 (unsigned char,		/* machine exception bits     */
			power_fail_alert	 : 1,
			environmental_error	 : 1,
			slave_initiated_reset	 : 1,
			physical_interface_check : 1,
			operation_timeout	 : 1,
			ready_transition	 : 1, /* asynchronous	      */
			p_available_transition	 : 1, /* asynchronous	      */
			addressee_not_busy	 : 1);
	BITFIELD_7 (unsigned char,		/* more exception bits	      */
			reserved0		 : 2,
			command_failure		 : 1,
			queue_full		 : 1,
			hardware_write_protected : 1,
			fatal_error		 : 1,
			uncorrectable_data_check : 1,  /* (on perfect data)   */
			data_check		 : 1); /* (on raw data)	      */
	BITFIELD_8 (unsigned char,		/* more exception bits	      */
			unexpected_master_action : 1,
			end_of_extent_detected	 : 1,
			end_of_media_detected	 : 1,
			reallocation_exhausted	 : 1,
			data_overrun		 : 1,
			write_access_violation	 : 1,
			read_access_violation	 : 1,
			reserved1		 : 1);
	BITFIELD_5 (unsigned char,		/* more exception bits	      */
			reserved2		 : 4,
			position_lost		 : 1,
			logical_link_failure	 : 1,
			defect_dictionary_full	 : 1,
			error_log_full		 : 1);
	unsigned char	extended_substatus[1];
} ipi_slave_machine_excep_t;

#define	IPI_PARAM_SLAVE_COMMAND_EXCEPTION		0x17	/* S   */
typedef struct {
	unsigned char	length;			/* command param length	      */
	unsigned char	id;			/* command param id	      */
	BITFIELD_8 (unsigned char,		/* command exception bits     */
			invalid_modifier	  : 1,
			invalid_opcode		  : 1,
			reserved0		  : 1,
			invalid_selection_address : 1,
			invalid_facility_address  : 1,
			invalid_slave_address	  : 1,
			invalid_command_reference : 1,
			invalid_packet_length	  : 1);
	BITFIELD_7 (unsigned char,		/* more exception bits	      */
			invalid_combination	  : 1,
			reserved_value_not_zero	  : 1,
			missing_parameters	  : 1,
			invalid_parameters	  : 1,
			out_of_context		  : 1,
			invalid_extent		  : 1,
			reserved1		  : 2);
	BITFIELD_2 (unsigned char,		/* more exception bits	      */
			reserved2		  : 7,
			not_at_initial_position	  : 1);
	unsigned char	reserved3;
	unsigned char	extended_substatus[1];
} ipi_slave_command_excep_t;

#define	IPI_PARAM_SLAVE_COMMAND_ABORTED			0x18	/* S   */
typedef struct {
	unsigned char	length;			/* aborted param length	      */
	unsigned char	id;			/* aborted param id	      */
	BITFIELD_8 (unsigned char,		/* aborted bits		      */
			reserved0	       : 1,
			unexec_from_term_order : 1,
			command_order_term     : 1,
			unexec_from_term_chain : 1,
			command_chain_term     : 1,
			unexec_from_term_seq   : 1,
			command_sequence_term  : 1,
			command_aborted	       : 1);
	unsigned char	reserved1;
	unsigned char	reserved2;
	unsigned char	reserved3;
	unsigned char	extended_substatus[1];
} ipi_slave_command_aborted_t;

#define	IPI_PARAM_SLAVE_CONDITIONAL_SUCCESS		0x19	/* S   */
typedef struct {
	unsigned char	length;			/* conditional param length   */
	unsigned char	id;			/* conditional param id	      */
	BITFIELD_8 (unsigned char,		/* conditional bits	      */
			reallocation_discontinuity   : 1, /* (if automatic)   */
			reallocation_required	     : 1,
			anticipated_data_error	     : 1,
			anticipated_error	     : 1,
			abort_not_operational	     : 1,
			abort_status_pending	     : 1,
			abort_no_command_active	     : 1,
			logging_data_appended	     : 1);
	BITFIELD_8 (unsigned char,		/* more conditional bits      */
			request_diagnostic_command   : 1,
			release_unreserved_addressee : 1,
			soft_error		     : 1,
			data_correction_performed    : 1,
			motion_retry_performed	     : 1,
			data_retry_performed	     : 1,
			error_retry_performed	     : 1,
			defect_threshold_exceeded    : 1);
	BITFIELD_8 (unsigned char,		/* more conditional bits      */
			master_terminated_transfer   : 1,
			asynchronous_event	     : 1, /* asynchronous     */
			parameter_update_requested   : 1,
			statistics_update_requested  : 1,
			end_of_media_warning	     : 1,
			retention_required	     : 1,
			noninterchange_volume	     : 1,
			error_log_request	     : 1);
	unsigned char	reserved0;
	unsigned char	extended_substatus[1];
} ipi_slave_conditional_t;

#define	IPI_PARAM_SLAVE_INCOMPLETE			0x1a	/* S   */
typedef struct {
	unsigned char	length;			/* incomplete param length    */
	unsigned char	id;			/* incomplete param id	      */
	BITFIELD_6 (unsigned char,		/* incomplete bits	      */
			reserved0		    : 3,
			select_subservient_slave    : 1,
			response_packet_truncated   : 1,
			copy_source_space_empty	    : 1,
			reserved1		    : 1,
			command_may_be_resumed	    : 1);
	BITFIELD_5 (unsigned char,		/* more incomplete bits	      */
			reserved2		    : 4,
			link_not_connected	    : 1,
			connect_identifier_assigned : 1,
			disconnect_unsuccessful	    : 1,
			connect_unsuccessful	    : 1);
	BITFIELD_8 (unsigned char,		/* more incomplete bits	      */
			reserved3		    : 1,
			block_not_found		    : 1,
			data_length_difference	    : 1,
			unrecorded_media	    : 1,
			block_length_difference	    : 1,
			end_of_extent_detected	    : 1,
			end_of_media_warning	    : 1,
			beginning_of_media	    : 1);
	unsigned char	reserved4;
	unsigned char	extended_substatus[1];
} ipi_slave_incomplete_t;


/******************************************************************************\
 *			Facility Major Status Parameters (0x20 - 0x2f)	      *
\******************************************************************************/

#define	IPI_PARAM_FACILITY_MSG_CODE_EXCEPTION		0x23	/* S   */
#define	ipi_facility_msg_excep_t	ipi_slave_msg_excep_t

#define	IPI_PARAM_FACILITY_INTERVENTION_REQUIRED	0x24	/* S   */
#define	ipi_facility_intervention_t	ipi_slave_intervention_t

#define	IPI_PARAM_FACILITY_ALT_PORT_EXCEPTION		0x25	/* S   */
#define	ipi_facility_alt_excep_t	ipi_slave_alt_excep_t

#define	IPI_PARAM_FACILITY_MACHINE_EXCEPTION		0x26	/* S   */
#define	ipi_facility_machine_excep_t	ipi_slave_machine_excep_t

#define	IPI_PARAM_FACILITY_COMMAND_EXCEPTION		0x27	/* S   */
#define	ipi_facility_command_excep_t	ipi_slave_command_excep_t

#define	IPI_PARAM_FACILITY_COMMAND_ABORTED		0x28	/* S   */
#define	ipi_facility_command_aborted_t	ipi_slave_command_aborted_t

#define	IPI_PARAM_FACILITY_CONDITIONAL_SUCCESS		0x29	/* S   */
#define	ipi_facility_conditional_t	ipi_slave_conditional_t

#define	IPI_PARAM_FACILITY_INCOMPLETE			0x2a	/* S   */
#define	ipi_facility_incomplete_t	ipi_slave_incomplete_t


/******************************************************************************\
 *			Common Parameters (0x30 - 0x4f)			      *
\******************************************************************************/

#define	IPI_PARAM_TRANSFER_NOTIFICATION			0x30	/* B O */

#define	IPI_PARAM_COMMAND_EXTENT			0x31	/* M   */
#define	IPI_PARAM_COMMAND_EXTENT_LEN			9

#define	IPI_PARAM_RESPONSE_EXTENT			0x32	/* S   */
#define	IPI_PARAM_RESPONSE_EXTENT_LEN			9
typedef struct {
	unsigned char	length;			/* response extent param len  */
	unsigned char	id;			/* response extent param id   */
	unsigned char	residual_msb;		/* remaining to be xferred    */
	unsigned char	residual_b2;
	unsigned char	residual_b1;
	unsigned char	residual_lsb;
	unsigned char	address_msb;		/* next address to be xferred */
	unsigned char	address_b2;
	unsigned char	address_b1;
	unsigned char	address_lsb;
} ipi_response_extent_t;

#define	IPI_PARAM_COMBINATION_COMMAND_EXTENT		0x33	/* M O */

#define	IPI_PARAM_COMBINATION_RESPONSE_EXTENT		0x34	/* S O */

#define	IPI_PARAM_ACCESS_KEY				0x35	/* M O */
#define	IPI_PARAM_ACCESS_KEY_LEN			5

#define	IPI_PARAM_INVALID_PARAM				0x38	/* S O */
typedef struct {
	unsigned char	length;			/* invalid param param len    */
	unsigned char	id;			/* invalid param param id     */
	unsigned char	param_offset_msb;
	unsigned char	param_offset_lsb;
	unsigned char	field_offset_msb;
	unsigned char	field_offset_lsb;
	unsigned char	param_data[1];
} ipi_invalid_param_t;

#define	IPI_PARAM_MISSING_PARAM				0x39	/* S O */

#define	IPI_PARAM_DATA_ADDRESS				0x3a	/* B O */

#define	IPI_PARAM_BLOCK_SIZE				0x3b	/* B O */
#define	IPI_PARAM_BLOCK_SIZE_LEN			5

#define	IPI_PARAM_TRANSFER				0x3c	/* M   */
#define	IPI_PARAM_TRANSFER_LEN				4

#define	IPI_PARAM_ENCAPSULATION				0x3d	/* B O */

#define	IPI_PARAM_PARTITION				0x3e	/* B O */
#define	IPI_PARAM_PARTITION_LEN				3
typedef struct {
	unsigned char	length;			/* partition param length     */
	unsigned char	id;			/* partition param id	      */
	unsigned char	reserved;
	unsigned char	partition;		/* partition		      */
} ipi_partition_param_t;

#define	IPI_PARAM_STOP_ON_DISCONTINUITY			0x3f	/* B O */
#define	IPI_PARAM_STOP_ON_DISCONTINUITY_LEN		7

#define	IPI_PARAM_EMBEDDED_DATA				0x40	/* S O */


/******************************************************************************\
 *			Command Parameters (0x50 - 0x9f)		      *
\******************************************************************************/

#define	IPI_PARAM_VENDOR_ID				0x50	/* B   */
typedef struct {
	unsigned char	length;			/* vendor param length	      */
	unsigned char	id;			/* vendor param id	      */
	unsigned char	identification[16];	/* identification  (ASCII)    */
	unsigned char	model[8];		/* model number    (ASCII)    */
	unsigned char	revision[4];		/* revision number (ASCII)    */
	unsigned char	unique[8];		/* unique id		      */
	unsigned char	switch_set[4];		/* switch settings	      */
	unsigned char	vendor_defined[1];	/* vendor defined fields      */
} ipi_vendor_t;

#define	IPI_PARAM_COMMAND_REFERENCE_NUMBER		0x50	/* M   */
#define	IPI_PARAM_COMMAND_REFERENCE_NUMBER_LEN		3
typedef struct {
	unsigned char	length;			/* cmd reference param length */
	unsigned char	id;			/* cmd reference param id     */
	unsigned char	cmd_ref_msb;		/* cmd reference number	      */
	unsigned char	cmd_ref_lsb;
} ipi_cmd_ref_t;

#define	IPI_PARAM_SIZE_OF_DISK_DATA_BLOCKS		0x51	/* B   */
#define	IPI_PARAM_SIZE_OF_DISK_DATA_BLOCKS_LEN		5
typedef struct {
	unsigned char	length;			/* data block size param len  */
	unsigned char	id;			/* data block size param id   */
	unsigned char	size_msb;		/* data block size	      */
	unsigned char	size_b2;
	unsigned char	size_b1;
	unsigned char	size_lsb;
} ipi_data_block_t;

#define	IPI_PARAM_CONDITION				0x51	/* S   */
#define	IPI_PARAM_CONDITION_LEN				3
typedef struct {
	unsigned char	length;			/* condition param length     */
	unsigned char	id;			/* condition param id	      */
	BITFIELD_8 (unsigned char,		/* condition bits	      */
			p_busy		  : 1,
			l_available	  : 1,
			port_neutral	  : 1,
			facility_switched : 1,
			not_ready	  : 1,
			ready		  : 1,
			not_operatoinal	  : 1,
			operational	  : 1);
	BITFIELD_6 (unsigned char,		/* more condition bits	      */
			reserved	  : 3,
			not_p_available	  : 1,
			p_available	  : 1,
			inactive	  : 1,
			active		  : 1,
			status_pending	  : 1);
} ipi_condition_t;

#define	IPI_PARAM_DISK_MODES				0x51	/* M   */
#define	IPI_PARAM_DISK_MODES_LEN			3
typedef struct {
	unsigned char	length;			/* disk modes param length    */
	unsigned char	id;			/* disk modes param id	      */
	unsigned char	mode;			/* disk mode bits	      */
#define		REZERO				BIT(0)
#define		LOCK_CARRIAGE			BIT(1)
#define		UNLOAD_HEADS			BIT(2)
#define		LOAD_HEADS			BIT(3)
#define		UNLOCK_CARTRIDGE		BIT(4)
#define		LOCK_CARTRIDGE			BIT(5)
#define		SPIN_DOWN			BIT(6)
#define		SPIN_UP				BIT(7)
	unsigned char	reserved;
	unsigned char	pad0;
	unsigned char	pad1;
	unsigned char	pad2;
	unsigned char	pad3;
} ipi_disk_modes_t;

#define	IPI_PARAM_SIZE_OF_DISK_PHYSICAL_BLOCKS		0x52	/* S   */
#define	IPI_PARAM_SIZE_OF_DISK_PHYSICAL_BLOCKS_LEN	5

#define	IPI_PARAM_MEDIA_STATUS				0x52	/* S   */
#define	IPI_PARAM_MEDIA_STATUS_LEN			2
typedef struct {
	unsigned char	length;			/* media status param length  */
	unsigned char	id;			/* media status param id      */
	BITFIELD_5 (unsigned char,		/* media status bits	      */
			write_protect	: 1,
			default_format	: 1,
			reserved0	: 3,
			media_present	: 1,
			reserved1	: 2);
} ipi_media_status_t;

#define	IPI_PARAM_NUMBER_DISK_DATA_BLOCKS		0x53	/* S   */
typedef struct {
	unsigned char	length;			/* disk data block param len  */
	unsigned char	id;			/* disk data block param id   */
	unsigned char	blocks_per_partition_msb;
	unsigned char	blocks_per_partition_b2;
	unsigned char	blocks_per_partition_b1;
	unsigned char	blocks_per_partition_lsb;
	unsigned char	blocks_per_cylinder_msb;
	unsigned char	blocks_per_cylinder_b2;
	unsigned char	blocks_per_cylinder_b1;
	unsigned char	blocks_per_cylinder_lsb;
	unsigned char	blocks_per_track_msb;
	unsigned char	blocks_per_track_b2;
	unsigned char	blocks_per_track_b1;
	unsigned char	blocks_per_track_lsb;
	unsigned char	data_address_msb;
	unsigned char	data_address_b2;
	unsigned char	data_address_b1;
	unsigned char	data_address_lsb;
} ipi_num_block_t;

#define	IPI_PARAM_VENDOR_UNIQUE_STATUS			0x53	/* S   */

#define	IPI_PARAM_NUMBER_DISK_PHYSICAL_BLOCKS		0x54	/* S   */

#define	IPI_PARAM_DATA_BLOCK_SIZES_SUPPORTED		0x55	/* S   */

#define	IPI_PARAM_PHYSICAL_BLOCK_SIZES_SUPPORTED	0x56	/* S   */

#define	IPI_PARAM_SIZE_OF_PHYSICAL_GROUPS		0x57	/* S   */
#define	IPI_PARAM_SIZE_OF_PHYSICAL_GROUPS_LEN		3

#define	IPI_PARAM_HARD_DISK_FORMAT			0x58	/* S   */
#define	IPI_PARAM_HARD_DISK_FORMAT_LEN			4

#define	IPI_PARAM_ATTRIBUTES_TABLE_CONDITONS		0x59	/* S   */
#define	IPI_PARAM_ATTRIBUTES_TABLE_CONDITONS_LEN	2

#define	IPI_PARAM_PAD_WITH_FILL_CHARS			0x5a	/* B   */

#define	IPI_PARAM_DISK_PARTITION_DEFINITION		0x5b	/* B   */
typedef struct {
	unsigned char	length;			/* disk partition param len   */
	unsigned char	id;			/* disk partition param id    */
	unsigned char	partition_id;		/* partition id		      */
	unsigned char	facility_addr;		/* facility address	      */
	BITFIELD_8 (unsigned char,		/* disk type bits	      */
			reserved1	: 1,
			cluster		: 1,
			solid_state	: 1,
			moving_head	: 1,
			fixed_head      : 1,
			floppy		: 1,
			removable	: 1,
			non_removable	: 1);
	unsigned char	reserved2;
	unsigned char	block_count_msb;	/* block count		      */
	unsigned char	block_count_b2;
	unsigned char	block_count_b1;
	unsigned char	block_count_lsb;
	unsigned char	data_blk_addr_msb;	/* data block address	      */
	unsigned char	data_blk_addr_b2;
	unsigned char	data_blk_addr_b1;
	unsigned char	data_blk_addr_lsb;
} ipi_disk_part_t;

#define	IPI_PARAM_SYNONYM_DEFINITION			0x5c	/* B   */

#define	IPI_PARAM_ALIAS_DEFINITION			0x5d	/* B   */

#define	IPI_PARAM_MULTI_PORT_CHARACTERISTICS		0x5e	/* B   */

#define	IPI_PARAM_PHYSICAL_DISK_CONFIGURATION		0x5f	/* B   */
typedef struct {
	unsigned char	length;			/* disk config param len      */
	unsigned char	id;			/* disk config param id	      */
	unsigned char	last_data_cylinder_msb;
	unsigned char	last_data_cylinder_b2;
	unsigned char	last_data_cylinder_b1;
	unsigned char	last_data_cylinder_lsb;
	unsigned char	last_defect_cylinder_msb;
	unsigned char	last_defect_cylinder_b2;
	unsigned char	last_defect_cylinder_b1;
	unsigned char	last_defect_cylinder_lsb;
	unsigned char	heads_per_cylinder_msb;
	unsigned char	heads_per_cylinder_lsb;
	unsigned char	sectors_per_revolution_msb;
	unsigned char	sectors_per_revolution_b2;
	unsigned char	sectors_per_revolution_b1;
	unsigned char	sectors_per_revolution_lsb;
	unsigned char	octets_per_track_msb;
	unsigned char	octets_per_track_b2;
	unsigned char	octets_per_track_b1;
	unsigned char	octets_per_track_lsb;
	unsigned char	single_cyl_seek_time_msb;
	unsigned char	single_cyl_seek_time_b2;
	unsigned char	single_cyl_seek_time_b1;
	unsigned char	single_cyl_seek_time_lsb;
	unsigned char	average_cyl_seek_time_msb;
	unsigned char	average_cyl_seek_time_b2;
	unsigned char	average_cyl_seek_time_b1;
	unsigned char	average_cyl_seek_time_lsb;
	unsigned char	max_cyl_seek_time_msb;
	unsigned char	max_cyl_seek_time_b2;
	unsigned char	max_cyl_seek_time_b1;
	unsigned char	max_cyl_seek_time_lsb;
	unsigned char	rotation_time_msb;
	unsigned char	rotation_time_b2;
	unsigned char	rotation_time_b1;
	unsigned char	rotation_time_lsb;
	unsigned char	head_switch_time_msb;
	unsigned char	head_switch_time_b2;
	unsigned char	head_switch_time_b1;
	unsigned char	head_switch_time_lsb;
	unsigned char	write_to_read_time_msb;
	unsigned char	write_to_read_time_b2;
	unsigned char	write_to_read_time_b1;
	unsigned char	write_to_read_time_lsb;
	unsigned char	vendor_specific_data[1];
} ipi_disk_config_t;

#define	IPI_PARAM_DATA_BLOCK_INTERLEAVE			0x60	/* S   */

#define	IPI_PARAM_TRANSFER_RATE				0x61	/* S   */
#define	IPI_PARAM_TRANSFER_RATE_LEN			9

#define	IPI_PARAM_PHYSICAL_BLOCK_PERF_SUPPORTED		0x62	/* S   */

#define	IPI_PARAM_CURRENT_PHYSICAL_BLOCK_PERF_SETTINGS	0x63	/* S   */

#define	IPI_PARAM_ADDRESSEE_CONFIGURATION		0x65	/* S   */
#define	IPI_PARAM_ADDRESSEE_CONFIGURATION_LEN		17
typedef struct {
	unsigned char	length;			/* addressee conf param len   */
	unsigned char	id;			/* addressee conf param id    */
	unsigned char	data_buf_size_msb;	/* data buffer size	      */
	unsigned char	data_buf_size_b2;
	unsigned char	data_buf_size_b1;
	unsigned char	data_buf_size_lsb;
	unsigned char	cmd_buf_size_msb;	/* command buffer size	      */
	unsigned char	cmd_buf_size_b2;
	unsigned char	cmd_buf_size_b1;
	unsigned char	cmd_buf_size_lsb;
	unsigned char	cmd_bytes_msb;		/* max num of octets in cmd   */
	unsigned char	cmd_bytes_lsb;
	unsigned char	rsp_bytes_msb;		/* max num of octets in rsp   */
	unsigned char	rsp_bytes_lsb;
	unsigned char	max_access;		/* max num of access permits  */
	unsigned char	min_queued;		/* min num of queued commands */
	unsigned char	max_queued;		/* max num of queued commands */
	unsigned char	cmd_stack;		/* size of command stack      */
} ipi_addressee_config_t;

#define	IPI_PARAM_SLAVE_CONFIGURATION			0x66	/* S   */
#define	IPI_PARAM_SLAVE_CONFIGURATION_LEN		5
typedef struct {
	unsigned char	length;			/* slave config param length  */
	unsigned char	id;			/* slave config param id      */
	BITFIELD_8 (unsigned char,		/* slave configuration bits   */
			mux_data_transfers    : 1,
			extended_substatus    : 1,
			master_termination    : 1,
			odd_transfers	      : 1,
			alias_addressing      : 1,
			synonym_addressing    : 1,
			facility_transfers    : 1,
			facilities_different  : 1);
	BITFIELD_8 (unsigned char,		/* slave configuration bits   */
			interlock_transfers   : 1,
			streaming_transfers   : 1,
			multi_cmd_extents     : 1,
			master_throttling     : 1,
			facility_config	      : 1,
			defineable_part	      : 1,
			imbedded_data	      : 1,
			transfer_notification : 1);
	unsigned char	reserved0;
	BITFIELD_3 (unsigned char,		/* logical interfaces	      */
			reserved1	       : 6,
			level_3		       : 1,
			level_2		       : 1);
} ipi_slave_config_t;

#define	IPI_PARAM_SLAVE_RECONFIGURATION			0x6f	/* B   */
#define	IPI_PARAM_SLAVE_RECONFIGURATION_LEN		39
typedef struct {
	unsigned char	length;			/* slave reconfig param len   */
	unsigned char	id;			/* slave reconfig param id    */
	unsigned char	time_dep_msb;		/* time dependency	      */
	unsigned char	time_dep_b2;
	unsigned char	time_dep_b1;
	unsigned char	time_dep_lsb;
	unsigned char	num_queued_msb;		/* number of queued commands  */
	unsigned char	num_queued_lsb;
	unsigned char	max_cmd_len_msb;	/* max command length	      */
	unsigned char	max_cmd_len_lsb;
	unsigned char	max_rsp_len_msb;	/* max response length	      */
	unsigned char	max_rsp_len_lsb;
	unsigned char	num_syncs_msb;		/* number of unanswered syncs */
	unsigned char	num_syncs_lsb;
	unsigned char	max_mux_xfer_msb;	/* max num mux data transfers */
	unsigned char	max_mux_xfer_lsb;
	unsigned char	gen_class_2_msb;	/* generate class 2 interrupt */
	unsigned char	gen_class_2_b2;
	unsigned char	gen_class_2_b1;
	unsigned char	gen_class_2_lsb;
	unsigned char	burst_size_msb;		/* burst size		      */
	unsigned char	burst_size_b2;
	unsigned char	burst_size_b1;
	unsigned char	burst_size_lsb;
	unsigned char	sync_tmo_msb;		/* sync timeout		      */
	unsigned char	sync_tmo_b2;
	unsigned char	sync_tmo_b1;
	unsigned char	sync_tmo_lsb;
	unsigned char	port_tmo_msb;		/* port release timeout	      */
	unsigned char	port_tmo_b2;
	unsigned char	port_tmo_b1;
	unsigned char	port_tmo_lsb;
	unsigned char	facility_tmo_msb;	/* facility timeout	      */
	unsigned char	facility_tmo_b2;
	unsigned char	facility_tmo_b1;
	unsigned char	facility_tmo_lsb;
	unsigned char	rel_delay_msb;		/* release delay	      */
	unsigned char	rel_delay_b2;
	unsigned char	rel_delay_b1;
	unsigned char	rel_delay_lsb;
} ipi_slave_reconfig_t;

#define IPI_PARAM_SLAVE_FACILITIES			0x68	/* B   */
typedef struct {
	unsigned char	length;			/* facilities param length    */
	unsigned char	id;			/* facilities param id	      */
	unsigned char	address;		/* facility address	      */
	unsigned char	class;			/* facility class	      */
#define		IPI_DISK			0x01	/* magnetic disk      */
#define		IPI_OPTICAL			0x02	/* optical  disk      */
#define		IPI_TAPE			0x03	/* magnetic tape      */
#define		IPI_COMMUNICATIONS		0x04	/* communications     */
	unsigned char	type;			/* facility type for mag disk */
#define		CLUSTER				BIT(1)	/* (optional)	      */
#define		SOLID_STATE			BIT(2)
#define		MOVING_HEAD			BIT(3)
#define		FIXED_HEAD			BIT(4)
#define		FLOPPY				BIT(5)
#define		REMOVABLE			BIT(6)
#define		NON_REMOVABLE			BIT(7)
	unsigned char	cluster_identifier;	/* facility cluster id	      */
#define		HIGH_AVAILABILITY		BIT(3)
#define		PARITY				BIT(4)
#define		CHECKSUM			BIT(5)
#define		SHADOW				BIT(6)
#define		PARALLEL			BIT(7)
} ipi_facilities_t;

#define	IPI_PARAM_REQUEST_PARAM				0x6c	/* M   */
typedef struct {
	unsigned char	length;			/* request param length	      */
	unsigned char	id;			/* request param id	      */
	unsigned char	control;		/* request control	      */
#define		NAKED_PARAMS_AS_DATA		BIT(4)
#define		LENGTH_BIT			BIT(5)
#define		PARAMS_IN_RESPONSE		BIT(6)
#define		PARAMS_AS_DATA			BIT(7)
	unsigned char	param_id;		/* requested param may repeat */
	unsigned char	pad0;			/* but we only request one    */
	unsigned char	pad1;			/* parameter at a time so pad */
	unsigned char	pad2;			/* out the rest to a 64-bit   */
	unsigned char	pad3;			/* boundary		      */
} ipi_request_t;

#define	IPI_PARAM_LENGTH				0x6d	/* S   */
#define	IPI_PARAM_LENGTH_LEN				5


/******************************************************************************\
 * Response structure							      *
 *									      *
 *	The first six octets echo the contents of the basic command packet.   *
 *	Asynchronous responses contain special values in these fields	      *
\******************************************************************************/

/*
 * Basic response packet
 */
typedef struct {
	unsigned char	packet_len_msb;		/* bytes excluding this field */
	unsigned char	packet_len_lsb;		/* bytes excluding this field */
	unsigned char	reference_msb;		/* command identificaton tag  */
	unsigned char	reference_lsb;		/* command identificaton tag  */
	unsigned char	opcode;			/* operation		      */
	unsigned char	modifier;		/* common[7..4], opcode[3..0] */
	unsigned char	slave;			/* slave (0x00 - 0x07)	      */
	unsigned char	facility;		/* facility (0x00 - 0xff)     */
	unsigned char	status_lsb;		/* major status		      */
	/* lsb major status bit definitions */
					     /* BIT(0) -   reserved */
					     /* BIT(1) -   reserved */
#define		IPI_VENDOR_UNIQUE		BIT(2)	/* parameter id 0x02 */
#define		IPI_MSG_OR_CODE_EXCEPTION	BIT(3)	/* parameter id 0x03 */
#define		IPI_INTERVENTION_REQUIRED	BIT(4)	/* parameter id 0x04 */
#define		IPI_ALT_PORT_EXCEPTION		BIT(5)	/* parameter id 0x05 */
#define		IPI_MACHINE_EXCEPTION		BIT(6)	/* parameter id 0x06 */
#define		IPI_COMMAND_EXCEPTION		BIT(7)	/* parameter id 0x07 */
	BITFIELD_2(unsigned char,
			status_msb	: 4,	/* major status		      */
			type		: 4);	/* type of response	      */
	/* msb major status bit definitions */
#define		IPI_COMMAND_ABORTED		BIT(0)	/* parameter id 0x08 */
#define		IPI_CONDITIONAL_SUCCESS		BIT(1)	/* parameter id 0x09 */
#define		IPI_INCOMPLETE			BIT(2)	/* parameter id 0x0a */
#define		IPI_SUCCESSFUL			BIT(3)
	/* response type field encoding */
					     /* 0x00  - reserved */
#define		IPI_STANDARD_RESPONSE		0x01
					     /* 0x02  - reserved */
#define		IPI_EXTENDED_RESPONSE		0x03
#define		IPI_ASYNCHRONOUS_RESPONSE	0x04
#define		IPI_TRANSFER_NOTIFICATION	0x05
#define		IPI_EMBEDDED_DATA_RESPONSE	0x06
				/* 0x07 through 0x0f  - reserved */
} ipi_response_t;


/******************************************************************************\
 * IPI Device Driver Command Structrues					      *
 *									      *
 *	These structures define the most commonly used commands.	      *
\******************************************************************************/

/******************************************************************************\
 *			NOP Command Structure				      *
\******************************************************************************/

#define	ipi_cmd_nop_t		ipi_command_t

/******************************************************************************\
 *			Read/Write Command Structrues			      *
\******************************************************************************/

typedef struct {
	unsigned char	packet_length_msb;	/* bytes excluding this field */
	unsigned char	packet_length_lsb;	/* bytes excluding this field */
	unsigned char	reference_msb;		/* command identificaton tag  */
	unsigned char	reference_lsb;		/* command identificaton tag  */
	unsigned char	opcode;			/* operation		      */
	unsigned char	modifier;		/* common[7..4], opcode[3..0] */
	unsigned char	slave;			/* slave (0x00 - 0x07)	      */
	unsigned char	facility;		/* facility (0x00 - 0xfe)     */
	unsigned char	partition_len;		/* partition param length     */
	unsigned char	partition_id;		/* partition param id	      */
	unsigned char	partition_reserved;	/* partition param reserved   */
	unsigned char	partition_partition;	/* partition param partition  */
	unsigned char	cmd_extent_len;		/* cmd extent param length    */
	unsigned char	cmd_extent_id;		/* cmd extent param id	      */
	unsigned char	cmd_extent_count_msb;	/* cmd extent param count     */
	unsigned char	cmd_extent_count_b2;	/* cmd extent param count     */
	unsigned char	cmd_extent_count_b1;	/* cmd extent param count     */
	unsigned char	cmd_extent_count_lsb;	/* cmd extent param count     */
	unsigned char	cmd_extent_addr_msb;	/* cmd extent param address   */
	unsigned char	cmd_extent_addr_b2;	/* cmd extent param address   */
	unsigned char	cmd_extent_addr_b1;	/* cmd extent param address   */
	unsigned char	cmd_extent_addr_lsb;	/* cmd extent param address   */
	unsigned char	pad0;
	unsigned char	pad1;
} ipi_cmd_read_write_t;

#define	ipi_cmd_read_t		ipi_cmd_read_write_t
#define	ipi_cmd_write_t		ipi_cmd_read_write_t

/******************************************************************************\
 *			   Attributes Command Structure			      *
\******************************************************************************/

#define	ipi_cmd_attributes_t	ipi_command_t

/******************************************************************************\
 *			Operating Mode Command Structure		      *
\******************************************************************************/

#define	ipi_cmd_op_mode_t	ipi_command_t

/******************************************************************************\
 *		    Report Addressee Status Command Structure		      *
\******************************************************************************/

#define	ipi_report_status_t	ipi_command_t

/******************************************************************************\
 *			    Abort Command Structure			      *
\******************************************************************************/

typedef struct {
	unsigned char	packet_length_msb;	/* bytes excluding this field */
	unsigned char	packet_length_lsb;	/* bytes excluding this field */
	unsigned char	reference_msb;		/* command identificaton tag  */
	unsigned char	reference_lsb;		/* command identificaton tag  */
	unsigned char	opcode;			/* operation		      */
	unsigned char	modifier;		/* common[7..4], opcode[3..0] */
	unsigned char	slave;			/* slave (0x00 - 0x07)	      */
	unsigned char	facility;		/* facility (0x00 - 0xfe)     */
	unsigned char	cmd_ref_len;		/* cmd reference param length */
	unsigned char	cmd_ref_id;		/* cmd reference param id     */
	unsigned char	cmd_ref_msb;		/* cmd reference param number */
	unsigned char	cmd_ref_lsb;		/* cmd reference param number */
	unsigned char	pad0;
	unsigned char	pad1;
	unsigned char	pad2;
	unsigned char	pad3;
} ipi_abort_t;

/******************************************************************************\
 *			   Reallocate Command Structure			      *
\******************************************************************************/

#define	ipi_cmd_reallocate_t	ipi_cmd_read_write_t

#endif	_IPI_3_H_
