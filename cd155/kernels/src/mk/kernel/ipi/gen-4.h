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
 * $Log: gen-4.h,v $
 * Revision 1.2  1994/11/18  20:50:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:13  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	gen-4.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Maximum Strategy GEN-4 system command specific definitions
 */


#ifndef	_GEN_4_H_
#define	_GEN_4_H_

/******************************************************************************\
 *		Maximum Strategy GEN-4 System Vendor Unique Definitions	      *
\******************************************************************************/

#define	GEN_4_DEVICE_SHIFT	1
#define	GEN_4_DEVICE_MASK	1

#define	GEN_4_DEVICE(dev)	\
	(1 << ((IPI_FACILITY(dev) >> GEN_4_DEVICE_SHIFT) & GEN_4_DEVICE_MASK))


/******************************************************************************\
 *		Maximum Strategy GEN-4 System Vendor Unique Parameters	      *
\******************************************************************************/

#define	GEN_4_VENDOR_ID_PARAM				0x50	/* S   */
typedef struct {
	unsigned char	length;			/* vendor param length	      */
	unsigned char	id;			/* vendor param id	      */
	unsigned char	identification[16];	/* identification  (ASCII)    */
	unsigned char	model[8];		/* model number    (ASCII)    */
	unsigned char	revision[4];		/* revision number (ASCII)    */
	unsigned char	unique[8];		/* unique id		      */
	unsigned char	switch_set[4];		/* switch settings	      */
	unsigned char	module_type;		/* module type		      */
	unsigned char	module_revision;	/* module revision	      */
	unsigned char	module_presense_msb;	/* module presence bits	      */
	unsigned char	module_presense_lsb;	/* more module presence bits  */
	BITFIELD_8 (unsigned char,		/* port presence bits	      */
			port_9		: 1,	/* console		      */
			reserved0	: 1,
			port_11		: 1,	/* serial Com Port #3	      */
			port_12		: 1,	/* serial Com Port #4	      */
			reserved1	: 1,
			port_14		: 1,	/* Ethernet Command-Only Port */
			port_15		: 1,	/* Ethernet Data-Only Port #1 */
			port_16		: 1);	/* Ethernet Data-Only Port #2 */
	BITFIELD_5 (unsigned char,		/* more port presence bits    */
			port_1		: 1,	/* IFP #1		      */
			port_2		: 1,	/* IFP #2		      */
			port_3		: 1,	/* IFP #3		      */
			port_4		: 1,	/* IFP #4		      */
			reserved2	: 4);
} gen_4_vendor_id_t;

#define	GEN_4_DISK_MODES_PARAM				0x51	/* M   */
#define	GEN_4_DISK_MODES_PARAM_LEN			3

#define	GEN_4_MODULE_SELECT_PARAM			0xd1	/* M   */
#define	GEN_4_MODULE_SELECT_PARAM_LEN			3

#define	GEN_4_DEVICE_SELECT_PARAM			0xd2	/* M   */
#define	GEN_4_DEVICE_SELECT_PARAM_LEN			3

#define	GEN_4_STATUS_PARAM				0x53	/* S   */
typedef struct {
	unsigned char	length;			/* status param length	      */
	unsigned char	id;			/* status param id	      */
	BITFIELD_4 (unsigned char,		/* facility status flags      */
			attached	: 1,
			mounted		: 1,
			access		: 2,
			reserved0	: 4);
#define		GEN_4_NO_ACCESS			0x00
#define		GEN_4_READ_ONLY			0x01
#define		GEN_4_WRITE_ONLY		0x02
#define		GEN_4_READ_WRITE		0x03
	BITFIELD_5 (unsigned char,		/* row 1 bank/device	      */
			mod9_row1	: 1,
			moda_row1	: 1,
			modb_row1	: 1,
			modc_row1	: 1,
			reserved1	: 4);
	BITFIELD_8 (unsigned char,		/* more row 1 bank/device     */
			mod1_row1	: 1,
			mod2_row1	: 1,
			mod3_row1	: 1,
			mod4_row1	: 1,
			mod5_row1	: 1,
			mod6_row1	: 1,
			mod7_row1	: 1,
			mod8_row1	: 1);
	BITFIELD_5 (unsigned char,		/* row 2 bank/device	      */
			mod9_row2	: 1,
			moda_row2	: 1,
			modb_row2	: 1,
			modc_row2	: 1,
			reserved2	: 4);
	BITFIELD_8 (unsigned char,		/* more row 2 bank/device     */
			mod1_row2	: 1,
			mod2_row2	: 1,
			mod3_row2	: 1,
			mod4_row2	: 1,
			mod5_row2	: 1,
			mod6_row2	: 1,
			mod7_row2	: 1,
			mod8_row2	: 1);
} gen_4_status_t;

#define	GEN_4_MODULE_MSG_PARAM				0xd8	/* S   */
typedef struct {
	unsigned char	length;			/* module msg param length    */
#define		GEN_4_SUCCESSFUL_LENGTH		3
	unsigned char	id;			/* module msg id	      */
	unsigned char	module_num;		/* module id (0x01 - 0x0c)    */
	BITFIELD_3 (unsigned char,		/* module status message      */
			reserved0	: 4,
			status		: 3,
			reserved1	: 1);
#define		GEN_4_NO_CORRECTION		0x00
#define		GEN_4_RETRY_MASK		0x03
#define		GEN_4_RETRY_CORRECTION		0x02
#define		GEN_4_ECC_MASK			0x05
#define		GEN_4_ECC_CORRECTION		0x04
	unsigned char	error_code;		/* module error code	      */
	BITFIELD_2 (unsigned char,		/* module bank/device	      */
			device		: 4,
			bank		: 4);
	unsigned char	block_count_msb;	/* block count written	      */
	unsigned char	block_count_lsb;
} gen_4_module_msg_t;

#define	GEN_4_IFP_FATAL_ERROR_PARAM			0xd9	/* S   */
#define	GEN_4_IFP_FATAL_ERROR_PARAM_LEN			11
typedef struct {
	unsigned char	length;			/* IFP fatal err param length */
	unsigned char	id;			/* IFP fatal err id	      */
	unsigned char	port_number;		/* port number		      */
	unsigned char	reserved0;
	unsigned char	in_error_msb;		/* IN-Chan Error	      */
	unsigned char	in_error_lsb;		/* IN-Chan Error	      */
	unsigned char	out_error_msb;		/* OUT-Chan Error	      */
	unsigned char	out_error_lsb;		/* OUT-Chan Error	      */
	unsigned char	ifp_error_msb;		/* Internal IFP Error	      */
	unsigned char	ifp_error_lsb;		/* Internal IFP Error	      */
	BITFIELD_2 (unsigned char,		/* module bank/device	      */
			module_mask_msb	: 4,	/* Module Error Mask	      */
			reserved1	: 4);
	unsigned char	module_mask_lsb;	/* Module Error Mask	      */
} gen_4_ifp_fatal_error_t;

#define	GEN_4_PORT_DATA_PARAM				0xe2	/* B   */
#define	GEN_4_PORT_DATA_PARAM_LEN			15
typedef struct {
	unsigned char	length;			/* port data param length     */
	unsigned char	id;			/* port data id		      */
	unsigned char	port_number;		/* port number		      */
	unsigned char	reserved0;
	unsigned char	default_facility;	/* default facility number    */
	unsigned char	default_port;		/* default port number	      */
	unsigned char	mode;			/* port mode		      */
#define		GEN_4_PORT_DATA_ONLY		0x01
#define		GEN_4_PORT_COMMAND_ONLY		0x02
#define		GEN_4_PORT_COMMAND_DATA		0x03
#define		GEN_4_PORT_HOST_GATE		0x04
	unsigned char	reserved1;
	BITFIELD_8 (unsigned char,		/* IFP control flags	      */
			full_first_burst    : 1,/* pad out short bursts	      */
			notify_command_host : 1,/* TRN on command port	      */
			stay_connected	    : 1,/* leave REQUEST asserted     */
			four_k_header	    : 1,/* enable 4k headder mode     */
			no_header	    : 1,/* send only data bursts      */
			reserved2	    : 1,
			notify_data_host    : 1,/* TRN on data port	      */
			reserved3	    : 1);
	unsigned char	connect_timeout;	/* channel timeout in seconds */
	unsigned char	default_ifield_msb;	/* default I-field	      */
	unsigned char	default_ifield_b2;	/* default I-field	      */
	unsigned char	default_ifield_b1;	/* default I-field	      */
	unsigned char	default_ifield_lsb;	/* default I-field	      */
	unsigned char	reserved4;
	unsigned char	reserved5;
} gen_4_port_data_t;

/******************************************************************************\
 *			Unit Control Command Structrues			      *
\******************************************************************************/

typedef struct {
	unsigned char	disk_modes_len;		/* disk modes param length    */
	unsigned char	disk_modes_id;		/* disk modes param id	      */
	unsigned char	mode;			/* disk mode bits	      */
	unsigned char	reserved;
	unsigned char	module_sel_len;		/* module select param length */
	unsigned char	module_sel_id;		/* module select param id     */
	unsigned char	module_sel_msb;		/* module select bits	      */
	unsigned char	module_sel_lsb;		/* more module select bits    */
} gen_4_spin_up_t;

typedef struct {
	unsigned char	disk_modes_len;		/* disk modes param length    */
	unsigned char	disk_modes_id;		/* disk modes param id	      */
	unsigned char	mode;			/* disk mode bits	      */
	unsigned char	reserved0;
	unsigned char	module_sel_len;		/* module select param length */
	unsigned char	module_sel_id;		/* module select param id     */
	unsigned char	module_sel_msb;		/* module select bits	      */
	unsigned char	module_sel_lsb;		/* more module select bits    */
	unsigned char	device_sel_len;		/* device select param length */
	unsigned char	device_sel_id;		/* device select param length */
	unsigned char	reserved1;
	unsigned char	device_select;		/* device select bits	      */
	unsigned char	pad0;
	unsigned char	pad1;
	unsigned char	pad2;
	unsigned char	pad3;
} gen_4_spin_down_t;

/******************************************************************************\
 *			Read Capacity Command Structrue			      *
\******************************************************************************/

typedef struct {
	unsigned char	length;			/* request param length	      */
	unsigned char	id;			/* request param id	      */
	unsigned char	control;		/* request control	      */
	unsigned char	param_id;		/* requested parameter	      */
} gen_4_read_capacity_t;

#endif	_GEN_4_H_
