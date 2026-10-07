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
 
/*
 *        INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 */
/*
 *  File:   libio.c
 *  Author: Jerrie Coffman
 *          Intel Corporation Supercomputer Systems Division
 *  Date:   7/92
 *
 *  Functions to support the RAID Manager interface to the SCSI device driver
 */


/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "passthru.h"
#include "libio.h"
#include "cdb_struct.h"

/******************************************************************************
 ***				   DEFINITIONS				    ***
 *****************************************************************************/

/*
 * Retry count for IO_Open, IO_Close, and IO_SCSI functions
 */
#define RETRY_COUNT	8

/*
 * SCSI command group definitions
 */
#define GROUP_MASK	0xe0
#define GROUP_SHIFT	5

#define GROUP_0		0	/* six-byte commands */
#define GROUP_1		1	/* ten-byte commands */
#define GROUP_2		2	/* ten-byte commands */
#define GROUP_3		3	/* ANSI reserved */
#define GROUP_4		4	/* ANSI reserved */
#define GROUP_5		5	/* twelve-byte commands */
#define GROUP_6		6	/* vendor specific */
#define GROUP_7		7	/* vendor specific */

/*
 * Node board expansion slot physical base address
 */
#define EXPANSION	0x80000000

/*
 * Node board expansion slot number
 */
#define BUS_SLOT	0

/*
 * libio error logging macro
 */
#define LOG		if (log) fprintf

/******************************************************************************
 ***				 VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

/*
 * Enable/Disable libio error logging
 */
static int log = TRUE;

/******************************************************************************
 ***				  INTERNAL PROCEDURES			    ***
 *****************************************************************************/

int
IO_Open(Path, Flag, Mode)
char		*Path;
int		Flag;
int		Mode;
{
	register int	i;
	register int	fd;

	/*
	 * This allows an application to open a device without any error
	 * checking performed by the driver druing its open routine.  In
	 * other words, an IO_Open will pass when a normal open system
	 * call will fail, except for gross errors (e.g. No Such Device)..
	 *
	 * The normal system open call is tried first; if it fails, IO_Open
	 * is called.  The normal system open should have worked, but since
	 * it didn't this function will just retry it a few times.
	 */

	for (i = 0; i < RETRY_COUNT; i++) {
		if ((fd = open(Path, Flag, Mode)) != -1) break;
	}

	return fd;
}

int
IO_Close(fd)
int		fd;
{
	register int	i;
	register int	ret;

	/*
	 * This allows an application to close a device without any error
	 * checking performed by the driver druing its close routine.  In
	 * other words, an IO_Close will pass when a normal close system
	 * call will fail.
	 *
	 * The normal system close call is tried first; if it fails, IO_Close
	 * is called.  The normal system close should have worked, but since
	 * it didn't this function will just retry it a few times.
	 */

	for (i = 0; i < RETRY_COUNT; i++) {
		if ((ret = close(fd)) != -1) break;
	}

	return ret;
}

int
IO_SCSI(fd, CDB_pointer, Direction)
int		fd;
CDB_t		*CDB_pointer;
int		Direction;
{
	struct scsi_io	scsi;
	register int	i;

	/*
	 * This function allows the user to construct a SCSI command (CDB)
	 * and pass it through to the driver.  It returns 0 for successful
	 * completion, -1 for an error, or -2 for a check sense condition.
	 */

	/*
	 * Calculate the SCSI command count
	 */
	switch ((CDB_pointer->Byte[0] & GROUP_MASK) >> GROUP_SHIFT) {

		case GROUP_0:
			scsi.cmd_count = 6;	/* six-byte commands */
			break;

		case GROUP_1:
		case GROUP_2:
			scsi.cmd_count = 10;	/* ten-byte commands */
			break;

		case GROUP_5:
			scsi.cmd_count = 12;	/* twelve-byte commands */
			break;

		default:
			LOG(stderr,
		"IO_SCSI: Reserved or Vendor Specific Command (0x%02x)\n",
				CDB_pointer->Byte[0]);
			return -1;
	}

	/*
	 * Set up direction field
	 */
	switch (Direction) {

		case IO_NONE:
			if (CDB_pointer->BufferLength != 0) {
				LOG(stderr,
			"IO_SCSI: IO_NONE with non-zero buffer length\n");
				return -1;
			}
			scsi.direction = SCSI_NONE;
			break;

		case IO_DEV_TO_MEM:
			if (CDB_pointer->BufferLength == 0) {
				LOG(stderr,
			"IO_SCSI: IO_DEV_TO_MEM with zero buffer length\n");
				return -1;
			}
			scsi.direction = SCSI_TO_MEM;
			break;

		case IO_MEM_TO_DEV:
			if (CDB_pointer->BufferLength == 0) {
				LOG(stderr,
			"IO_SCSI: IO_MEM_TO_DEV with zero buffer length\n");
				return -1;
			}
			scsi.direction = MEM_TO_SCSI;
			break;

		default:
			LOG(stderr, "IO_SCSI: Invalid direction (%d)\n",
				Direction);
			return -1;
	}

	/*
	 * Check size of CDB (in case NCR changes their definition)
	 * and copy the CDB parameters into the ioctl structure
	 */
	if (CDB_LENGTH != SCSI_CDB_LENGTH) {
		LOG(stderr, "IO_SCSI: CDB_LENGTH != SCSI_CDB_LENGTH\n");
		return -1;
	}

	/*
	 * Copy command to ioctl buffer
	 */
	for (i = 0; i < SCSI_CDB_LENGTH; i++) {
		scsi.cdb[i] = CDB_pointer->Byte[i];
	}

	/*
	 * Check size of Buffer to make sure NCR doesn't exceed our limits
	 */
	if (CDB_pointer->BufferLength > SCSI_BUF_MAX) {
		LOG(stderr,
			"IO_SCSI: CDB_pointer->BufferLength > SCSI_BUF_MAX\n");
		return -1;
	}

	/*
	 * Set ioctl buffer length
	 */
	scsi.buf_len = CDB_pointer->BufferLength;

	/*
	 * If direction is out, copy out the buffer
	 */
	if (Direction == IO_MEM_TO_DEV) {
		for (i = 0; i < CDB_pointer->BufferLength; i++) {
			scsi.buf[i] = CDB_pointer->Buffer[i];
		}
	}

	/*
	 * Issue the ioctl command and check the return values
	 */
	for (i = 0; i < RETRY_COUNT; i++) {
		if (ioctl(fd, DIOCSCSI, (char *)&scsi) == -1) return -1;
		if (scsi.status != SCSI_BUSY) break;
		sleep(1);
	}

	if (scsi.status == SCSI_BUSY) {
		LOG(stderr, "IO_SCSI: Device Busy\n");
		return -1;
	}

	if (scsi.status == SCSI_CHECK_CONDITION) return -2;

	if (scsi.status != SCSI_GOOD) {
		LOG(stderr, "IO_SCSI: Unknown status (0x%02x)\n",
			scsi.status);
		return -1;
	}

	/*
	 * If direction is in, copy in the buffer
	 */
	if (Direction == IO_DEV_TO_MEM) {
		for (i = 0; i < scsi.buf_len; i++) {
			CDB_pointer->Buffer[i] = scsi.buf[i];
		}
	}

	return 0;
}

int
IO_GetDeviceParameters(fd, DeviceParameters)
int		fd;
struct IO_DeviceParameters	*DeviceParameters;
{
	struct scsi_phys	phys;

	/*
	 * This function will fill in the DeviceParameters structure with the
	 * information pertaining to the SCSI device.
	 */

	/*
	 * Issue the ioctl command to get the SCSI device physical information
	 */
	if (ioctl(fd, DIOCGPHYS, (char *)&phys) == -1) return -1;

	/* System bus number */
	DeviceParameters->dp_IO_BusNumber = phys.slot;

	/* Base address of the board */
	DeviceParameters->dp_BaseAddress = EXPANSION;

	/* Slot number of HBA in system bus */
	DeviceParameters->dp_IO_BusSlot = BUS_SLOT;

	/* SCSI channel off of HBA */
	DeviceParameters->dp_SCSI_BusNumber = phys.controller;

	/* target SCSI ID */
	DeviceParameters->dp_PUN = phys.target_id;

	/* SCSI logical unit number */
	DeviceParameters->dp_LUN = phys.lun;

	return 0;
}

static void
init_cdb(cdb)
CDB_t		*cdb;
{
	register unsigned int i;
	register unsigned char *p;
 
	/*
	 * Clear the given CDB structure
	 */
	for (i = 0, p = (unsigned char *)cdb; i < sizeof (CDB_t); i++)
		p[i] = 0;
}

int
IO_Inquiry(fd, InquiryData,lun)
int			fd;
SCSI_Inquiry_Data_t	*InquiryData;
int			lun;
{
	CDB_t cdb;
	Inquiry_CDB_t *p;

	/*
	 * This function executes the SCSI inquiry command (0x12), and is
	 * applicable to all SCSI devices.  It returns 0 for successful
	 * completion, -1 for an error, or -2 for a check sense condition.
	 */
 
	/*
	 * Build the SCSI Inquiry CDB and use IO_SCSI to issue the command
	 */
	init_cdb(&cdb);
        p = (Inquiry_CDB_t *)cdb.Byte;
        p->Op_Code = SCSI_Inquiry;
	p->LUN = 0;
	if(lun)
		p->LUN = lun;

        p->Allocation_Length = sizeof(SCSI_Inquiry_Data_t);
        cdb.Buffer = (unsigned char *)InquiryData;
        cdb.BufferLength = sizeof(SCSI_Inquiry_Data_t);

#ifdef IO_TEST
	return io_test(fd, &cdb, IO_DEV_TO_MEM);
#else
#ifdef NO_IO
	return 0;
#else
	return IO_SCSI(fd, &cdb, IO_DEV_TO_MEM);
#endif
#endif
}

int
IO_RequestSense(fd, SenseData,lun)
int			fd;
SCSI_Sense_Data_t	*SenseData;
int			lun;
{
	CDB_t cdb;
	Request_Sense_CDB_t *p;

	/*
	 * This function executes the SCSI Request Sense command (0x03), and is
	 * applicable to all SCSI devices.  It returns 0 for successful
	 * completion, -1 for an error, or -2 for a check sense condition.
	 */
 
	/*
	 * Build the SCSI Request Sense CDB,use IO_SCSI to issue the command
	 */
	init_cdb(&cdb);
        p = (Request_Sense_CDB_t *)cdb.Byte;
        p->Op_Code = SCSI_RequestSense;
        p->LUN = 0;				/* always use LUN zero */
	if(lun)
		p->LUN = lun;
        p->Allocation_Length = sizeof(SCSI_Sense_Data_t);
        cdb.Buffer = (unsigned char *)SenseData;
        cdb.BufferLength = sizeof(SCSI_Sense_Data_t);

#ifdef IO_TEST
	return io_test(fd, &cdb, IO_DEV_TO_MEM);
#else
#ifdef NO_IO
	return 0;
#else
	return IO_SCSI(fd, &cdb, IO_DEV_TO_MEM);
#endif
#endif
}

int
IO_NoErrorLogging(NoError)
int			NoError;
{
	/*
	 * This function turns off error logging for all libio routines
	 * if the NoError variable is > 0.  If NoError is <= 0 error
	 * logging is turned on.
	 */

	log = (NoError > 0) ? FALSE : TRUE;
}
