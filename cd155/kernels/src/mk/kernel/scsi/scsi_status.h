/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: scsi_status.h,v $
 * Revision 1.2.8.1  1995/06/11  18:34:32  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:49:21  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	scsi_status.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	04/94
 *
 *	SCSI device driver specific ioctl commands
 */


#ifndef	_SCSI_STATUS_H_
#define	_SCSI_STATUS_H_


/*
 * Structures used to get/set the SCSI driver statistics
 */
struct scsi_rw_stats {
	unsigned int		requests;	/* number of I/O requests  */
	unsigned int		xfer_count;	/* number of bytes/blocks  */
	unsigned int		driver_time;	/* driver service time	   */
	unsigned int		device_time;	/* device service time	   */
};

struct scsi_stats {
	struct scsi_rw_stats	read;		/* read statistics	   */
	struct scsi_rw_stats	write;		/* write statistics	   */
	unsigned int		queue_max;	/* maximum queue depth	   */
	unsigned int		chain_max;	/* maximum chain depth	   */
	unsigned int		queue_depth;	/* current queue depth	   */
	unsigned int		chain_depth;	/* current chain depth	   */
	unsigned int		driver_full;	/* driver queue full count */
	unsigned int		device_full;	/* device queue full count */
	unsigned int		retries;	/* driver command retries  */
};

/*
 * SCSI device driver statistics ioctl commands
 */
#define SCSIGSTATS	_IOR('s', 100, struct scsi_stats)	/* get */
#define SCSISSTATS	_IOW('s', 101, struct scsi_stats)	/* set */


/*
 * Data structures and defines used to control SCSI media changer devices
 */

struct mccap {
	unsigned char		page_code  : 6,
				reserved1  : 1,
				ps	   : 1;
	unsigned char		page_length;
	unsigned char		stor_mt	   : 1,
				stor_st	   : 1,
				stor_ie	   : 1,
				stor_dt	   : 1,
				reserved2  : 4;
	unsigned char		reserved3;
	unsigned char		mt_2_mt	   : 1,
				mt_2_st	   : 1,
				mt_2_ie	   : 1,
				mt_2_dt	   : 1,
				reserved4  : 4;
	unsigned char		st_2_mt	   : 1,
				st_2_st	   : 1,
				st_2_ie	   : 1,
				st_2_dt	   : 1,
				reserved5  : 4;
	unsigned char		ie_2_mt	   : 1,
				ie_2_st	   : 1,
				ie_2_ie	   : 1,
				ie_2_dt	   : 1,
				reserved6  : 4;
	unsigned char		dt_2_mt	   : 1,
				dt_2_st	   : 1,
				dt_2_ie	   : 1,
				dt_2_dt	   : 1,
				reserved7  : 4;
	unsigned char		reserved8;
	unsigned char		reserved9;
	unsigned char		reserved10;
	unsigned char		reserved11;
	unsigned char		mt_x_mt	   : 1,
				mt_x_st	   : 1,
				mt_x_ie	   : 1,
				mt_x_dt	   : 1,
				reserved12 : 4;
	unsigned char		st_x_mt	   : 1,
				st_x_st	   : 1,
				st_x_ie	   : 1,
				st_x_dt	   : 1,
				reserved13 : 4;
	unsigned char		ie_x_mt	   : 1,
				ie_x_st	   : 1,
				ie_x_ie	   : 1,
				ie_x_dt	   : 1,
				reserved14 : 4;
	unsigned char		dt_x_mt	   : 1,
				dt_x_st	   : 1,
				dt_x_ie	   : 1,
				dt_x_dt	   : 1,
				reserved15 : 4;
};

struct mcaddr {
	unsigned char		page_code : 6,
				reserved1 : 1,
				ps	  : 1;
	unsigned char		page_length;
	unsigned char		medium_xport_element_msb;
	unsigned char		medium_xport_element_lsb;
	unsigned char		num_of_medium_xport_element_msb;
	unsigned char		num_of_medium_xport_element_lsb;
	unsigned char		storage_element_msb;
	unsigned char		storage_element_lsb;
	unsigned char		num_of_storage_element_msb;
	unsigned char		num_of_storage_element_lsb;
	unsigned char		import_export_element_msb;
	unsigned char		import_export_element_lsb;
	unsigned char		num_of_import_export_element_msb;
	unsigned char		num_of_import_export_element_lsb;
	unsigned char		data_transfer_element_msb;
	unsigned char		data_transfer_element_lsb;
	unsigned char		num_of_data_transfer_element_msb;
	unsigned char		num_of_data_transfer_element_lsb;
	unsigned char		reserved2;
	unsigned char		reserved3;
};

struct mcgeo {
	unsigned char		page_code : 6,
				reserved1 : 1,
				ps	  : 1;
	unsigned char		page_length;
#define MCGEO_MAX_DESC			125
	struct {
		unsigned char		rotate	  : 6,
					reserved1 : 1;
		unsigned char		xport_element_set;
	} xport_geo_desc[MCGEO_MAX_DESC];	/* VARSIZE */
};

struct mcread {
	unsigned char		element_type;
#define	ALL_ELEMENTS			0
#define	MEDIUM_TRANSPORT_ELEMENT	1
#define	STORAGE_ELEMENT			2
#define	IMPORT_EXPORT_ELEMENT		3
#define	DATA_TRANSFER_ELEMENT		4
	boolean_t		volume_tag;
	unsigned short		starting_element_address;
	unsigned short		number_of_elements;
	unsigned int		allocation_length;
#define MCREAD_MAX_DATA			256
	unsigned char		data[MCREAD_MAX_DATA];	/* defined below */
};

struct mcposn {
	unsigned short		transport_address;
	unsigned short		destination_address;
	boolean_t		invert;
};

struct mcmove {
	unsigned short		transport_address;
	unsigned short		source_address;
	unsigned short		destination_address;
	boolean_t		invert;
};

struct mcxchg {
	unsigned short		transport_address;
	unsigned short		source_address;
	unsigned short		destination_address1;
	unsigned short		destination_address2;
	boolean_t		invert1;
	boolean_t		invert2;
};

/*
 * SCSI media changer device ioctl commands
 */
#define MCCAP		 _IOR('m', 100, struct mccap)		/* get */
#define MCADDR		 _IOR('m', 101, struct mcaddr)		/* get */
#define MCGEO		 _IOR('m', 102, struct mcgeo)		/* get */
#define MCINIT		 _IOW('m', 103, unsigned int)		/* set */
#define MCREAD		_IOWR('m', 104, struct mcread)		/* set/get */
#define MCPOSN		 _IOW('m', 105, struct mcposn)		/* set */
#define MCMOVE		 _IOW('m', 106, struct mcmove)		/* set */
#define MCXCHG		 _IOW('m', 107, struct mcxchg)		/* set */

/*
 * The SCSI media changer read element status (mcread) data consists
 * of a status header followed by one or more element status pages.
 * Each element status page consists of a page header followed by
 * one or more element descriptors.
 */

typedef	struct {
	unsigned char		first_element_reported_msb;
	unsigned char		first_element_reported_lsb;
	unsigned char		number_of_elements_reported_msb;
	unsigned char		number_of_elements_reported_lsb;
	unsigned char		reserved1;
	unsigned char		byte_count_of_report_msb;
	unsigned char		byte_count_of_report;
	unsigned char		byte_count_of_report_lsb;
	/* element status page data might follow */
} element_status_header_t;

typedef struct {
	unsigned char		type;
	unsigned char		reserved1	: 6,
				avoltag		: 1,
				pvoltag		: 1;
	unsigned char		descriptor_length_msb;
	unsigned char		descriptor_length_lsb;
	unsigned char		reserved2;
	unsigned char		byte_count_of_descriptor_msb;
	unsigned char		byte_count_of_descriptor;
	unsigned char		byte_count_of_descriptor_lsb;
	/* element status descriptor data might follow */
} element_page_header_t;

typedef struct {
	unsigned char		element_address_msb;
	unsigned char		element_address_lsb;
	unsigned char		full		: 1,
				reserved1	: 1,
				except		: 1,
				reserved2	: 5;
	unsigned char		reserved3;
	unsigned char		asc;
	unsigned char		ascq;
	unsigned char		reserved4;
	unsigned char		reserved5;
	unsigned char		reserved6;
	unsigned char		reserved7	: 6,
				invert		: 1,
				svalid		: 1;
	unsigned char		source_storage_element_msb;
	unsigned char		source_storage_element_lsb;
	unsigned char		additional_bytes[1];	/* VARSIZE */
} medium_transport_descriptor_t;

typedef struct {
	unsigned char		element_address_msb;
	unsigned char		element_address_lsb;
	unsigned char		full		: 1,
				reserved1	: 1,
				except		: 1,
				access		: 1,
				reserved2	: 4;
	unsigned char		reserved3;
	unsigned char		asc;
	unsigned char		ascq;
	unsigned char		reserved4;
	unsigned char		reserved5;
	unsigned char		reserved6;
	unsigned char		reserved7	: 6,
				invert		: 1,
				svalid		: 1;
	unsigned char		source_storage_element_msb;
	unsigned char		source_storage_element_lsb;
	unsigned char		additional_bytes[1];	/* VARSIZE */
} storage_descriptor_t;

typedef struct {
	unsigned char		element_address_msb;
	unsigned char		element_address_lsb;
	unsigned char		full		: 1,
				impexp		: 1,
				except		: 1,
				access		: 1,
				exenab		: 1,
				inenab		: 1,
				reserved1	: 2;
	unsigned char		reserved2;
	unsigned char		asc;
	unsigned char		ascq;
	unsigned char		reserved3;
	unsigned char		reserved4;
	unsigned char		reserved5;
	unsigned char		reserved6	: 6,
				invert		: 1,
				svalid		: 1;
	unsigned char		source_storage_element_msb;
	unsigned char		source_storage_element_lsb;
	unsigned char		additional_bytes[1];	/* VARSIZE */
} import_export_descriptor_t;

typedef struct {
	unsigned char		element_address_msb;
	unsigned char		element_address_lsb;
	unsigned char		full		: 1,
				reserved1	: 1,
				except		: 1,
				access		: 1,
				reserved2	: 4;
	unsigned char		reserved3;
	unsigned char		asc;
	unsigned char		ascq;
	unsigned char		lun		: 3,
				reserved4	: 1,
				lu_valid	: 1,
				id_valid	: 1,
				reserved5	: 1,
				not_bus		: 1;
	unsigned char		scsi_bus_address;
	unsigned char		reserved6;
	unsigned char		reserved7	: 6,
				invert		: 1,
				svalid		: 1;
	unsigned char		source_storage_element_msb;
	unsigned char		source_storage_element_lsb;
	unsigned char		additional_bytes[1];	/* VARSIZE */
} data_transfer_descriptor_t;

#endif	_SCSI_STATUS_H_
