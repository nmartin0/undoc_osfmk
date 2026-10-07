/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: scsi2.h,v $
 * Revision 1.8  1995/03/14  23:48:49  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.7  1994/11/18  20:59:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/03/03  23:11:30  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: adding support for 3480 tape library functions -
 * 			READ POSITION.
 *  Testing: tape EAT's, wrote a test program that uses the new ioctl.
 *  Module(s): define of the scsi_position_data_t structure.
 *
 * Revision 1.5  1993/06/30  22:53:54  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:48:00  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:29  dleslie
 * First R1_0 release
 *
 * Revision 2.7  91/06/19  11:57:28  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.6  91/05/14  17:28:31  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/13  06:04:53  af
 * 	Added read_defect_data.
 * 	[91/05/12  16:02:41  af]
 * 
 * Revision 2.4  91/02/05  17:44:53  mrt
 * 	Added author notices
 * 	[91/02/04  11:18:21  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:17:02  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:44  af
 * 	Status byte decl was wrong.
 * 	[90/12/03  23:39:07  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:38:39  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/09/03            af]
 * 
 */
/*
 *	File: scsi2.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Additions and changes of the SCSI-II standard viz SCSI-I
 */

#ifndef	_SCSI_SCSI2_H_
#define	_SCSI_SCSI2_H_

#include <scsi/scsi_endian.h>

/*
 * Single byte messages
 *
 * originator:	I-nitiator T-arget
 * T-support:	M-andatory O-ptional
 */

#define SCSI_ABORT_TAG			0x0d	/* O I 2 */
#define SCSI_CLEAR_QUEUE		0x0e	/* O I 2 */
#define SCSI_INITIATE_RECOVERY		0x0f	/* O IT2 */
#define SCSI_RELEASE_RECOVERY		0x10	/* O I 2 */
#define SCSI_TERMINATE_IO_PROCESS	0x11	/* O I 2 */

/*
 * Two byte messages
 */
#define SCSI_SIMPLE_QUEUE_TAG		0x20	/* O IT2 */
#define SCSI_HEADOF_QUEUE_TAG		0x21	/* O I 2 */
#define SCSI_ORDERED_QUEUE_TAG		0x22	/* O I 2 */
#define SCSI_IGNORE_WIDE_RESIDUE	0x23	/* O  T2 */
					/* 0x24..0x2f reserved */

/*
 * Extended messages, codes and formats
 */

#define SCSI_WIDE_XFER_REQUEST		0x03	/* IT 2 */
typedef struct {
	unsigned char	xtn_msg_tag;		/* const 0x01 */
	unsigned char	xtn_msg_len;		/* const 0x02 */
	unsigned char	xtn_msg_code;		/* const 0x03 */
	unsigned char	xtn_msg_xfer_width;
} scsi_wide_xfer_t;

/*
 * NOTE: some command-specific mods and extensions
 * are actually defined in the scsi.h file for
 * readability reasons
 */

				/* GROUP 0 */

#define	SCSI_CMD_INIT_ELEMENT_STATUS	0x07	/* 02 jukebox */
#define	scsi_cmd_init_element_status_t	scsi_command_group_0

				/* GROUP 1 */

#define	SCSI_CMD_POSITION_TO_ELEMENT	0x2b	/* 02 jukebox */
#define	scsi_cmd_position_to_element_t	scsi_command_group_1
#	define scsi_cmd_posn_trans_msb		scsi_cmd_lba1
#	define scsi_cmd_posn_trans_lsb		scsi_cmd_lba2
#	define scsi_cmd_posn_dest_msb		scsi_cmd_lba3
#	define scsi_cmd_posn_dest_lsb		scsi_cmd_lba4
#	define scsi_cmd_posn_invert		scsi_cmd_xfer_len_2
#	define SCSI_CMD_POSN_INVERT		0x01

#define SCSI_CMD_READ_POSITION		0x34	/* O2 tape */
#define	scsi_cmd_read_position_t scsi_command_group_1
#	define SCSI_CMD_RP_BT			0x01

#define	SCSI_CMD_READ_DEFECT_DATA	0x37	/* O2 disk opti */
typedef scsi_command_group_1	scsi_cmd_read_defect_t;
#	define SCSI_CMD_RDD_LIST_TYPE		0x07
#	define SCSI_CMD_RDD_GLIST		0x08
#	define SCSI_CMD_RDD_PLIST		0x10

#define SCSI_CMD_WRITE_BUFFER		0x3b	/* O2 all */
typedef scsi_command_group_1	scsi_cmd_write_buffer_t;
#	define SCSI_CMD_BUF_MODE		0x07
#	define scsi_cmd_buf_id			scsi_cmd_lba1
#	define scsi_cmd_buf_offset1		scsi_cmd_lba2
#	define scsi_cmd_buf_offset2		scsi_cmd_lba3
#	define scsi_cmd_buf_offset3		scsi_cmd_lba4
#	define scsi_cmd_buf_alloc1		scsi_cmd_xxx
#	define scsi_cmd_buf_alloc2		scsi_cmd_xfer_len_1
#	define scsi_cmd_buf_alloc3		scsi_cmd_xfer_len_2

#define SCSI_CMD_READ_BUFFER		0x3c	/* O2 all */
#define	scsi_cmd_read_buffer_t scsi_command_group_1

				/* GROUP 2 */

#define SCSI_CMD_CHANGE_DEFINITION	0x40	/* O2 all */
#define	scsi_cmd_change_def_t	scsi_command_group_2
#	define scsi_cmd_chg_save		scsi_cmd_lba1
#	define scsi_cmd_chg_definition		scsi_cmd_lba2
#	define SCSI_CMD_CHG_CURRENT		0x00
#	define SCSI_CMD_CHG_SCSI_1		0x01
#	define SCSI_CMD_CHG_CCS			0x02
#	define SCSI_CMD_CHG_SCSI_2		0x03

					/* 0x41..0x4b reserved */

#define SCSI_CMD_LOG_SELECT		0x4c	/* O2 all */
#define	scsi_cmd_logsel_t	scsi_command_group_2
#	define SCSI_CMD_LOG_SP			0x01
#	define SCSI_CMD_LOG_PCR			0x02
#	define scsi_cmd_log_page_control	scsi_cmd_lba1

#define SCSI_CMD_LOG_SENSE		0x4d	/* O2 all */
#define	scsi_cmd_logsense_t	scsi_command_group_2
#	define SCSI_CMD_LOG_PPC			0x02
#	define scsi_cmd_log_page_code		scsi_cmd_lba1
#	define scsi_cmd_log_param_ptr1		scsi_cmd_lba4
#	define scsi_cmd_log_param_ptr2		scsi_cmd_xxx


					/* 0x4e..0x54 reserved */

#define SCSI_CMD_MODE_SELECT_2		0x55	/* Z2 */
#define	scsi_cmd_mode_select_long_t	scsi_command_group_2
#	define SCSI_CMD_MSL2_PF		0x10
#	define SCSI_CMD_MSL2_SP		0x01

					/* 0x56..0x59 reserved */

#define SCSI_CMD_MODE_SENSE_2		0x5a	/* Z2 */
#define	scsi_cmd_mode_sense_long_t	scsi_command_group_2
#	define SCSI_CMD_MSS2_DBD	0x08

					/* 0x5b..0x5f reserved */

				/* GROUP 5 */

#define	SCSI_CMD_MOVE_MEDIUM		0xa5	/* M2 jukebox */
#define	scsi_cmd_move_medium_t			scsi_command_group_5
#	define scsi_cmd_move_trans_msb		scsi_cmd_lba1
#	define scsi_cmd_move_trans_lsb		scsi_cmd_lba2
#	define scsi_cmd_move_src_msb		scsi_cmd_lba3
#	define scsi_cmd_move_src_lsb		scsi_cmd_lba4
#	define scsi_cmd_move_dest_msb		scsi_cmd_xfer_len_1
#	define scsi_cmd_move_dest_lsb		scsi_cmd_xfer_len_2
#	define scsi_cmd_move_invert		scsi_cmd_xxx1
#	define SCSI_CMD_MV_INVERT		0x01

#define	SCSI_CMD_EXCHANGE_MEDIUM	0xa6	/* O2 jukebox */
#define	scsi_cmd_exchange_medium_t		scsi_command_group_5
#	define scsi_cmd_exchange_trans_msb	scsi_cmd_lba1
#	define scsi_cmd_exchange_trans_lsb	scsi_cmd_lba2
#	define scsi_cmd_exchange_src_msb	scsi_cmd_lba3
#	define scsi_cmd_exchange_src_lsb	scsi_cmd_lba4
#	define scsi_cmd_exchange_dest1_msb	scsi_cmd_xfer_len_1
#	define scsi_cmd_exchange_dest1_lsb	scsi_cmd_xfer_len_2
#	define scsi_cmd_exchange_dest2_msb	scsi_cmd_xfer_len_3
#	define scsi_cmd_exchange_dest2_lsb	scsi_cmd_xfer_len_4
#	define scsi_cmd_exchange_invert		scsi_cmd_xxx1
#	define SCSI_CMD_EX_INVERT_1		0x01
#	define SCSI_CMD_EX_INVERT_2		0x02

#define	SCSI_CMD_READ_ELEMENT_STATUS	0xb8	/* O2 jukebox */
#define	scsi_cmd_read_element_status_t		scsi_command_group_5
#	define SCSI_CMD_ELEMENT_TYPE_MASK	0x0f
#	define SCSI_CMD_ALL_ELEMENTS			0x0
#	define SCSI_CMD_MEDIUM_XPORT_ELEMENT		0x1
#	define SCSI_CMD_STORAGE_ELEMENT			0x2
#	define SCSI_CMD_IMPORT_EXPORT_ELEMENT		0x3
#	define SCSI_CMD_DATA_XFER_ELEMENT		0x4
#	define SCSI_CMD_RD_VOL_TAG		0x10
#	define scsi_cmd_rd_elm_start_msb	scsi_cmd_lba1
#	define scsi_cmd_rd_elm_start_lsb	scsi_cmd_lba2
#	define scsi_cmd_rd_elm_nelements_msb	scsi_cmd_lba3
#	define scsi_cmd_rd_elm_nelements_lsb	scsi_cmd_lba4
#	define scsi_cmd_rd_elm_alloc_msb	scsi_cmd_xfer_len_2
#	define scsi_cmd_rd_elm_alloc_sb1	scsi_cmd_xfer_len_3
#	define scsi_cmd_rd_elm_alloc_lsb	scsi_cmd_xfer_len_4

/*
 * Command specific defines
 */
typedef struct {
	BITFIELD_2(unsigned char,
			periph_type : 5,
			periph_qual : 3);
#define	SCSI_SCANNER		0x06	/* periph_type values */
#define	SCSI_MEMORY		0x07
#define	SCSI_J_BOX		0x08
#define	SCSI_COMM		0x09
#define	SCSI_PREPRESS1		0x0a
#define	SCSI_PREPRESS2		0x0b

#define	SCSI_PERIPH_CONNECTED	0x00	/* periph_qual values */
#define	SCSI_PERIPH_CAPABLE	0x01
#define	SCSI_PERIPH_INCAPABLE	0x03

	BITFIELD_2(unsigned char,
			device_type : 7,
			rmb : 1);

	BITFIELD_3( unsigned char,
			ansi : 3,
			ecma : 3,
			iso : 2);

	BITFIELD_4( unsigned char,
			response_fmt : 4,
			res1 : 2,
			trmIOP : 1,
			aenc : 1);
	unsigned char	length;
	unsigned char	res2;
	unsigned char	res3;

	BITFIELD_8(unsigned char,
			SftRe : 1,
			CmdQue : 1,
			res4 : 1,
			Linked : 1,
			Sync : 1,
			Wbus16 : 1,
			Wbus32 : 1,
			RelAdr : 1);

	unsigned char	vendor_id[8];
	unsigned char	product_id[16];
	unsigned char	product_rev[4];
	unsigned char	vendor_uqe[20];
	unsigned char	reserved[40];
	unsigned char	vendor_uqe1[1];	/* varsize */
} scsi2_inquiry_data_t;
#define	SCSI_INQ_SUPP_PAGES	0x00
#define	SCSI_INQ_A_INFO		0x01	/* 0x01..0x1f, really */
#define	SCSI_INQ_SERIALNO	0x80
#define	SCSI_INQ_IMPL_OPDEF	0x81
#define	SCSI_INQ_A_IMPL_OPDEF	0x82


/*
 * Status byte (a-la scsi2)
 */

typedef union {
    struct {
	BITFIELD_3( unsigned char,
			scsi_status_reserved1:1,
			scsi_status_code:5,
			scsi_status_reserved2:2);
							/* more scsi_status_code values */
					/* 00..0c as in SCSI-I */
#	define SCSI_ST2_CMD_TERMINATED	0x11	/* 2 */
#	define SCSI_ST2_QUEUE_FULL	0x14	/* 2 */
					/* anything else is reserved */
    } st;
    unsigned char bits;
} scsi2_status_byte_t;


/* read position (tape) */
typedef struct {
        BITFIELD_5(unsigned char,
                res  : 2,
                bpu  : 1,
                res1 : 3,
                eop  : 1,
                bop  : 1);
        unsigned char   partition_number;
        unsigned char   res2;
        unsigned char   res3;
        unsigned char   firstblk_msb;
        unsigned char   firstblk_sb1;
        unsigned char   firstblk_sb2;
        unsigned char   firstblk_lsb;
        unsigned char   lastblk_msb;
        unsigned char   lastblk_sb1;
        unsigned char   lastblk_sb2;
        unsigned char   lastblk_lsb;
        unsigned char   res12;
        unsigned char   bufblks_msb;
        unsigned char   bufblks_sb1;
        unsigned char   bufblks_lsb;
        unsigned char   bufbytes_msb;
        unsigned char   bufbytes_sb1;
        unsigned char   bufbytes_sb2;
        unsigned char   bufbytes_lsb;
} scsi_position_data_t;

/* mode sense data */

/* page 10 (0x0a) - control mode page */
typedef	struct {
	BITFIELD_3(unsigned char,
		page_code   : 6,
		reserved1   : 1,
		ps	    : 1);
	unsigned char	page_length;
	BITFIELD_2(unsigned char,
		rlec	    : 1,
		reserved2   : 7);
	BITFIELD_4(unsigned char,
		dque	    : 1,
		qerr	    : 1,
		reserved3   : 2,
		q_algorithm : 4);
	/* Queue Algorithm Modifiers */
#	define	SCSI_RESTRICTED_REORDERING	0
#	define	SCSI_UNRESTRICTED_REORDERING	1
	BITFIELD_5(unsigned char,
		eaenp	    : 1,
		uaaenp	    : 1,
		raenp	    : 1,
		reserved4   : 4,
		eeca	    : 1);
	unsigned char	reserved5;
	unsigned char	aen_holdoff_period_msb;
	unsigned char	aen_holdoff_period_lsb;
} scsi_mode_sense_page10_t;

/* page 29 (0x1d) - element address assignment page */
typedef	struct {
	BITFIELD_3(unsigned char,
		page_code : 6,
		reserved1 : 1,
		ps	  : 1);
	unsigned char	page_length;
	unsigned char	medium_xport_elm_msb;
	unsigned char	medium_xport_elm_lsb;
	unsigned char	num_medium_xport_elm_msb;
	unsigned char	num_medium_xport_elm_lsb;
	unsigned char	storage_elm_msb;
	unsigned char	storage_elm_lsb;
	unsigned char	num_storage_elm_msb;
	unsigned char	num_storage_elm_lsb;
	unsigned char	import_export_elm_msb;
	unsigned char	import_export_elm_lsb;
	unsigned char	num_import_export_elm_msb;
	unsigned char	num_import_export_elm_lsb;
	unsigned char	data_xfer_elm_msb;
	unsigned char	data_xfer_elm_lsb;
	unsigned char	num_data_xfer_elm_msb;
	unsigned char	num_data_xfer_elm_lsb;
	unsigned char	reserved2;
	unsigned char	reserved3;
} scsi_mode_sense_page29_t;

/* page 30 (0x1e) - transport geometry parameters page */
typedef	struct {
	BITFIELD_3(unsigned char,
		page_code : 6,
		reserved1 : 1,
		ps	  : 1);
	unsigned char	page_length;
	struct {
		BITFIELD_2(unsigned char,
			rotate	  : 6,
			reserved1 : 1);
		unsigned char	xport_elm_set;
	} xport_geo_desc[1];	/* VARSIZE */
} scsi_mode_sense_page30_t;

/* page 31 (0x1f) - device capabilities page */
typedef	struct {
	BITFIELD_3(unsigned char,
		page_code  : 6,
		reserved1  : 1,
		ps	   : 1);
	unsigned char	page_length;
	BITFIELD_5(unsigned char,
		stor_mt	   : 1,
		stor_st	   : 1,
		stor_ie	   : 1,
		stor_dt	   : 1,
		reserved2  : 4);
	unsigned char	reserved3;
	BITFIELD_5(unsigned char,
		mt_2_mt	   : 1,
		mt_2_st	   : 1,
		mt_2_ie	   : 1,
		mt_2_dt	   : 1,
		reserved4  : 4);
	BITFIELD_5(unsigned char,
		st_2_mt	   : 1,
		st_2_st	   : 1,
		st_2_ie	   : 1,
		st_2_dt	   : 1,
		reserved5  : 4);
	BITFIELD_5(unsigned char,
		ie_2_mt	   : 1,
		ie_2_st	   : 1,
		ie_2_ie	   : 1,
		ie_2_dt	   : 1,
		reserved6  : 4);
	BITFIELD_5(unsigned char,
		dt_2_mt	   : 1,
		dt_2_st	   : 1,
		dt_2_ie	   : 1,
		dt_2_dt	   : 1,
		reserved7  : 4);
	unsigned char	reserved8;
	unsigned char	reserved9;
	unsigned char	reserved10;
	unsigned char	reserved11;
	BITFIELD_5(unsigned char,
		mt_x_mt	   : 1,
		mt_x_st	   : 1,
		mt_x_ie	   : 1,
		mt_x_dt	   : 1,
		reserved12 : 4);
	BITFIELD_5(unsigned char,
		st_x_mt	   : 1,
		st_x_st	   : 1,
		st_x_ie	   : 1,
		st_x_dt	   : 1,
		reserved13 : 4);
	BITFIELD_5(unsigned char,
		ie_x_mt	   : 1,
		ie_x_st	   : 1,
		ie_x_ie	   : 1,
		ie_x_dt	   : 1,
		reserved14 : 4);
	BITFIELD_5(unsigned char,
		dt_x_mt	   : 1,
		dt_x_st	   : 1,
		dt_x_ie	   : 1,
		dt_x_dt	   : 1,
		reserved15 : 4);
} scsi_mode_sense_page31_t;

/* read element status (jukebox) */

typedef	struct {
	unsigned char	first_elm_reported_msb;
	unsigned char	first_elm_reported_lsb;
	unsigned char	num_elm_reported_msb;
	unsigned char	num_elm_reported_lsb;
	unsigned char	reserved1;
	unsigned char	byte_count_of_report_msb;
	unsigned char	byte_count_of_report_sb1;
	unsigned char	byte_count_of_report_lsb;
	/* element page data might follow */
} scsi_element_status_header_t;

typedef struct {
	unsigned char	type;
	BITFIELD_3(unsigned char,
		reserved1	: 6,
		avoltag		: 1,
		pvoltag		: 1);
	unsigned char	desc_len_msb;
	unsigned char	desc_len_lsb;
	unsigned char	reserved2;
	unsigned char	byte_count_of_desc_msb;
	unsigned char	byte_count_of_desc_sb1;
	unsigned char	byte_count_of_desc_lsb;
	/* element descriptor data might follow */
} scsi_element_page_header_t;

typedef struct {
	unsigned char	elm_addr_msb;
	unsigned char	elm_addr_lsb;
	BITFIELD_4(unsigned char,
		full		: 1,
		reserved1	: 1,
		except		: 1,
		reserved2	: 5);
	unsigned char	reserved3;
	unsigned char	asc;
	unsigned char	ascq;
	unsigned char	reserved4;
	unsigned char	reserved5;
	unsigned char	reserved6;
	BITFIELD_3(unsigned char,
		reserved7	: 6,
		invert		: 1,
		svalid		: 1);
	unsigned char	src_storage_elm_msb;
	unsigned char	src_storage_elm_lsb;
	unsigned char	add_bytes[1];	/* VARSIZE */
} scsi_medium_xport_desc_t;

typedef struct {
	unsigned char	elm_addr_msb;
	unsigned char	elm_addr_lsb;
	BITFIELD_5(unsigned char,
		full		: 1,
		reserved1	: 1,
		except		: 1,
		access		: 1,
		reserved2	: 4);
	unsigned char	reserved3;
	unsigned char	asc;
	unsigned char	ascq;
	unsigned char	reserved4;
	unsigned char	reserved5;
	unsigned char	reserved6;
	BITFIELD_3(unsigned char,
		reserved7	: 6,
		invert		: 1,
		svalid		: 1);
	unsigned char	src_storage_elm_msb;
	unsigned char	src_storage_elm_lsb;
	unsigned char	add_bytes[1];	/* VARSIZE */
} scsi_storage_desc_t;

typedef struct {
	unsigned char	elm_addr_msb;
	unsigned char	elm_addr_lsb;
	BITFIELD_7(unsigned char,
		full		: 1,
		impexp		: 1,
		except		: 1,
		access		: 1,
		exenab		: 1,
		inenab		: 1,
		reserved1	: 2);
	unsigned char	reserved2;
	unsigned char	asc;
	unsigned char	ascq;
	unsigned char	reserved3;
	unsigned char	reserved4;
	unsigned char	reserved5;
	BITFIELD_3(unsigned char,
		reserved6	: 6,
		invert		: 1,
		svalid		: 1);
	unsigned char	src_storage_elm_msb;
	unsigned char	src_storage_elm_lsb;
	unsigned char	add_bytes[1];	/* VARSIZE */
} scsi_import_export_desc_t;

typedef struct {
	unsigned char	elm_addr_msb;
	unsigned char	elm_addr_lsb;
	BITFIELD_5(unsigned char,
		full		: 1,
		reserved1	: 1,
		except		: 1,
		access		: 1,
		reserved2	: 4);
	unsigned char	reserved3;
	unsigned char	asc;
	unsigned char	ascq;
	BITFIELD_6(unsigned char,
		lun		: 3,
		reserved4	: 1,
		lu_valid	: 1,
		id_valid	: 1,
		reserved5	: 1,
		not_bus		: 1);
	unsigned char	scsi_bus_addr;
	unsigned char	reserved6;
	BITFIELD_3(unsigned char,
		reserved7	: 6,
		invert		: 1,
		svalid		: 1);
	unsigned char	src_storage_elm_msb;
	unsigned char	src_storage_elm_lsb;
	unsigned char	add_bytes[1];	/* VARSIZE */
} scsi_data_xfer_desc_t;

#endif	_SCSI_SCSI2_H_
