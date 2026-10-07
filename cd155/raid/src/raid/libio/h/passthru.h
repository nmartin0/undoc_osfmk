/*
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 */
/*
 *  File:   passthru.h
 *  Author: Jerrie Coffman
 *          Intel Corporation Supercomputer Systems Division
 *  Date:   9/92
 *
 *  SCSI pass-through ioctls and structures
 */


#ifndef	_PASSTHRU_
#define _PASSTHRU_

/*
 * Structure used to perform SCSI pass-through commands
 */
struct scsi_io {
#define SCSI_CDB_LENGTH 12
	unsigned char	cdb[SCSI_CDB_LENGTH];
	unsigned long	cmd_count;
#define SCSI_NONE	(0 << 0)
#define MEM_TO_SCSI	(1 << 0)
#define SCSI_TO_MEM	(1 << 1)
	unsigned long	direction;
#define SCSI_GOOD			0x00
#define SCSI_CHECK_CONDITION		0x01
#define SCSI_CONDITION_MET		0x02
#define SCSI_BUSY			0x03
#define SCSI_INTERMEDIATE		0x04
#define SCSI_INTERMEDIATE_CONDITION_MET	0x05
#define SCSI_RESERVATION_CONFLICT	0x06
#define SCSI_COMMAND_TERMINATED		0x07
#define SCSI_QUEUE_FULL			0x08
	unsigned long	status;
	unsigned long	buf_len;
#define SCSI_BUF_MAX	(2 * 1024)
	unsigned char	buf[SCSI_BUF_MAX];
};

/* SCSI pass-through command */
#define DIOCSCSI	_IOWR('d', 111, struct scsi_io)

/*
 * Structure used to get SCSI device physical information
 */
struct scsi_phys {
	unsigned long	slot;
	unsigned char	controller;
	unsigned char	target_id;
	unsigned char	lun;
};

/* get SCSI device physical information */
#define DIOCGPHYS	_IOR('d', 112, struct scsi_phys)

#endif	_PASSTHRU_
