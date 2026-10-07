/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 

/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	scsi.h 11.3 94/03/25 15:09:21
 */
/** static char	scsih_ver[] = "@(#) scsi.h 11.3 94/03/25 15:09:21";
/**/

/*
 *
 * . { TEXT
 * .H 4 scsi.h
 *
 * This module contains constants for the SCSI interface
 *
 * . } TEXT
 */

#define SCSI_FIFO	SCSIPHYS0
#define SCSI_ESP_BASE	SCSIPHYS1
#define SCSI_AUX_BASE	SCSIPHYS2
#define SCSI_LOOP_BASE	SCSIPHYS3

#define SCSI_CLEAR_CNT	*(long *)(SCSI_AUX_BASE + 0x00)
#define SCSI_RESET_ESP	*(long *)(SCSI_AUX_BASE + 0x40)
#define SCSI_RESET_FIFO	*(long *)(SCSI_AUX_BASE + 0x80)

#define SCSI_READ_MODE	*(long *)(SCSI_AUX_BASE + 0x00)
#define SCSI_WRITE_MODE	*(long *)(SCSI_AUX_BASE + 0x40)
#define SCSI_ENABLE_HF	*(long *)(SCSI_AUX_BASE + 0x80)
#define SCSI_DISABLE_HF	*(long *)(SCSI_AUX_BASE + 0xC0)

#define SCSI_COUNT_LO	(*(unchar *)(SCSI_ESP_BASE + 0x0000))
#define SCSI_COUNT_HI	(*(unchar *)(SCSI_ESP_BASE + 0x0004))
#define SCSI_ESP_FIFO	(*(unchar *)(SCSI_ESP_BASE + 0x0008))
#define SCSI_COMMAND	(*(unchar *)(SCSI_ESP_BASE + 0x000C))
#define SCSI_STATUS	(*(ushort *)(SCSI_ESP_BASE + 0x0010))
#define SCSI_ID		(*(unchar *)(SCSI_ESP_BASE + 0x0010))
#define SCSI_INT_STATUS	(*(unchar *)(SCSI_ESP_BASE + 0x0014))
#define SCSI_TIMEOUT	(*(unchar *)(SCSI_ESP_BASE + 0x0014))
#define SCSI_SEQUENCE	(*(unchar *)(SCSI_ESP_BASE + 0x0018))
#define SCSI_SYNC_PER	(*(unchar *)(SCSI_ESP_BASE + 0x0018))
#define SCSI_FIFO_FLAGS	(*(unchar *)(SCSI_ESP_BASE + 0x001C))
#define SCSI_SYNC_OFF	(*(unchar *)(SCSI_ESP_BASE + 0x001C))
#define SCSI_CONFIG	(*(unchar *)(SCSI_ESP_BASE + 0x0020))
#define SCSI_CLOCK	(*(unchar *)(SCSI_ESP_BASE + 0x0024))

#define ESP_NOP		0x00
#define ESP_FLUSH_FIFO	0x01
#define ESP_RESET_CHIP	0x02
#define ESP_RESET_BUS	0x03
#define ESP_SELECT	0x41
#define ESP_SELATN	0x42
#define ESP_SELATNSTOP	0x43
#define ESP_ENABLE_SEL	0x44
#define ESP_DISABLE_SEL	0x45
#define ESP_TRANSFER	0x10
#define ESP_COMPLETE	0x11
#define ESP_MSGACCEPT	0x12
#define ESP_PAD		0x18
#define ESP_ATN		0x1A
#define ESP_DMA		0x80

#define ESP_SELECT_INT	0x01
#define ESP_SELATN_INT	0x02
#define ESP_RESEL_INT	0x04
#define ESP_COMP_INT	0x08
#define ESP_BUS_INT	0x10
#define ESP_DISC_INT	0x20
#define ESP_ILLCMD_INT	0x40
#define ESP_RESET_INT	0x80

#define ESP_PHASE_STAT	0x07
#define ESP_XFER_STAT	0x08
#define ESP_COUNT_STAT	0x10
#define ESP_PARITY_STAT	0x20
#define ESP_GROSS_STAT	0x40

#define ESP_XFER_MAX    32768

#define SCSI_WEF_STAT	0x0100
#define SCSI_WHF_STAT	0x0200
#define SCSI_WFF_STAT	0x0400
#define SCSI_EINT_STAT	0x0800
#define SCSI_MODE_STAT	0x1000
#define SCSI_REF_STAT	0x2000
#define SCSI_RHF_STAT	0x4000
#define SCSI_RFF_STAT	0x8000

#define ESP_DATAOUT_PH	0x00
#define ESP_DATAIN_PH	0x01
#define ESP_COMMAND_PH	0x02
#define ESP_STATUS_PH	0x03
#define ESP_MSGOUT_PH	0x06
#define ESP_MSGIN_PH	0x07

#define ESP_CLK_NS	(1000/24)

#define CMD_ST_GOOD	0x00
#define CMD_ST_CHECK	0x02
#define CMD_ST_BUSY	0x08
#define CMD_ST_INTER	0x10
#define CMD_ST_RUNNING	0x60
#define	CMD_ST_TIMEOUT	0x62

#define EXTENDED_MSG_SAVE 85

#define MSG_CMD_DONE	0x00
#define MSG_EXTENDED 0x01
#define MSG_SAVE_DP	0x02
#define	MSG_REST_DP	0x03
#define MSG_DISCONNECT	0x04
#define MSG_INITIAT_ERR	0x05
#define MSG_ABORT	0x06
#define MSG_REJECT	0x07
#define MSG_NOP		0x08
#define MSG_PARITY	0x09
#define MSG_LINK_DONE	0x0A
#define MSG_LINK_FLAG	0x0B
#define MSG_RESET	0x0C
#define MSG_IDENTIFY	0xC0

#define SCSI_INQUIRY_CMD	0x12
typedef
struct scsi_inquiry {
	ulong	dev_type	:8;
	ulong	qualifier	:7;
	ulong	rmb		:1;
	ulong	version		:8;
	ulong	format		:4;
	ulong			:4;
	unchar	added;
	unchar	reserved[3];
	unchar	extra[28];
} SCSI_INQUIRY;

#define SCSI_SENSE_CMD		0x03
#define SCSI_SENSE_INFO_VALID	0x80
#define SCSI_KEY_MASK_HI        0xF0
#define SCSI_KEY_MASK_LO        0x0F
#define SCSI_KEY_ECC            0x03
#define SCSI_KEY_ILI		0x20
#define SCSI_KEY_EOM		0x40
#define SCSI_KEY_EOF		0x80

#define SCSI_KEY_NOT_READY      0x02
#define SCSI_KEY_ILLEGAL_REQ    0x05
#define SCSI_KEY_UNIT_ATTEN     0x06

/**** 3480 changes by Ernie Granese 11/12/91 - JTI ****/

#define SCSI_KEY_VOLUME_OVERFLOW    0x0D

/* scsi code (byte 12) values */

#define SCSI_CODE_PEOT              0x62

/* scsi qualifier (byte 13) values */

#define SCSI_QUALIFIER_LEOT         0x02

/**** end 3480 changes ****/

typedef
struct scsi_sense {
	unchar	class;
	unchar	segment;
	unchar	key;
	unchar	info[4];
	unchar	added;
	unchar	reserved[4];
	unchar	code;
	unchar	qualifier;
	unchar	fru;
	unchar	bit;
	ushort	field;
} SCSI_SENSE;

#define SCSI_CAPACITY_CMD	0x25
typedef
struct scsi_capacity {
	ulong	lba;
	ulong	blen;
} SCSI_CAPACITY;

#define SCSI_MODESELECT_CMD	0x15
#define SCSI_MODESENSE_CMD	0x1A
typedef
struct scsi_modesense {
	ulong	sense_data_length	:8;
	ulong	medium_type		:8;
	ulong	speed			:4;
	ulong	buffered_mode		:3;
	ulong	wp			:1;
	ulong	block_descriptor_length	:8;
	unchar	density_code;
	unchar	number_of_blocks[3];
	unchar	reserved;
	unchar	block_length[3];
} SCSI_MODESENSE;

#define SCSI_READ_BLK_LIMITS_CMD        0x05
typedef
struct scsi_blklimits {
        unchar  reserved;
        unchar  max_block_length[3];
        unchar  min_block_length[2];
} SCSI_BLK_LIMITS;

#define SCSI_TESTUNIT_CMD	0x00
#define SCSI_REWIND_CMD		0x01
#define SCSI_SREAD_CMD		0x08
#define SCSI_SWRITE_CMD		0x0A
#define SCSI_LREAD_CMD		0x28
#define SCSI_LWRITE_CMD		0x2A
#define SCSI_WRITEFM_CMD	0x10
#define SCSI_SPACE_CMD		0x11
#define SCSI_ERASE_CMD		0x19
#define SCSI_LOAD_CMD		0x1B

/**** 3480 changes by Ernie Granese 01/07/92 - JTI ****/

#define SCSI_LOAD_DISPLAY_CMD   0x06
#define SCSI_LOCATE_CMD         0x2B
#define SCSI_READ_POSITION_CMD  0x34
#define SCSI_SEND_DIAG_CMD      0x1D
#define SCSI_REC_DIAG_CMD       0x1C

/**** end 3480 changes ****/

#define PSEUDO_SCSI_ERROR 0xFF
#define PSE_DRIVE_REQUEST_TIMEOUT 0x01
#define PSE_TEST_UNIT_TIMEOUT 0x02
