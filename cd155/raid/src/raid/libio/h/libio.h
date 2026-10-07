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
 
/* Copyright 1990 NCR Corporation - Dayton, Ohio, USA */


#ident	"@(#)usr/src/arch/mca/uts/i386/sys:libio.h	1.2"

#ifndef _LIBIO_H
#define _LIBIO_H
#include "scsi/SCSI.h"

/* 
 * The following structures and sets of defines are required for
 * the "libio" library.  This library is composed of several
 * routines to abstract the user from lower-level ioctl routines.
 */

/*
 * Device Types.
 */
#define  DIRECT_ACCESS          0x00
#define  SEQUENTIAL_ACCESS      0x01
#define  PRINTER                0x02
#define  PROCESSOR              0x03
#define  NOT_PRESENT            0x7f


/*
 * Format Specifications
 *
 * During a device format, using "IO_Format()", the user may specify
 * the action to take with the defect data (growth) list.  He may
 * optionally choose to preserve or clear the field grown defect list.
 */
#define GL_NOCLEAR            0   /* Do not clear field-grown defect list */
#define GL_CLEAR              1   /* Clear field-grown defect list        */


/*
 * Spacing Definitions
 *
 * Defines needed to represent the type of spacing involved using
 * the "IO_Space()" command.
 */
#define SP_BLOCK                0x00
#define SP_FILEMARK             0x01
#define SP_SEQ_FILEMARK         0x02
#define SP_END_OF_DATA          0x03


/*
 * Page Control names
 *
 * This must be specified when using the "IO_ModeSense()" routine.
 */
#define PC_CURRENT              0
#define PC_CHANGEABLE           1
#define PC_DEFAULT              2
#define PC_SAVED                3


/*
 * Read Defect Data Requests
 *
 * The following defines describe the types of defect data which may
 * be requested during the "IO_ReadDefectData()" routine.  These
 * commands are not exclusive, i.e., the user may specify to read
 * the field grown defects in bytes from index format by OR'ing the
 * bit definitions together.
 */
#define RD_PRIMARY              0x10
#define RD_FIELD_GROWN          0x08
#define RD_PHYSICAL_SECTOR      0x05
#define RD_BYTES_FROM_INDEX     0x04


/*
 * Ready Capacity structure
 *
 * The "IO_ReadCapacity()" routine returns a pointer to this structure,
 * including the last block address as well as the block size.
 */
typedef struct CapacityStruct {
        unsigned long   LastBlockAddress;
        unsigned long   BlockLength;
} Capacity_t;

/*
 *
 * Modeselect structure
 *
 * The "IO_ModeSelect()" routine is passed a pointer to the mode select data
 * which includes a header and the page information,
 * the number of data bytes in the mode select data (including the header), and the 
 * save parameters indicator.  The user must set up the header and the page information
 * before issuing the IO_ModeSelect command.
 */
typedef struct ModeSelectData{
	char	Header[4];
	char	Page[256];
} ModeSelect_t;



/*
 * Reassign Blocks structure
 *
 * The "IO_ReassignBlock()" routine is passed a pointer to this structure,
 * which contains a list of logical block numbers and a count of the
 * number of blocks to be reassigned.
 */
#define        MAX_REASSIGN_BLOCKS     128

typedef struct BlockList {
	short		Reserved;
	short		NumberOfBlocks;
        unsigned long   LogicalBlocks[MAX_REASSIGN_BLOCKS];
} BlockList_t;



/*
 * Sense Key Definitions
 *
 * This field is defined in the Extended device status portion of
 * the sense data structure.
 */
#define SK_NO_SENSEDATA          0x00
#define SK_RECOVERED_ERROR       0x01
#define SK_UNIT_NOT_READY        0x02
#define SK_MEDIUM_ERROR          0x03
#define SK_HARDWARE_ERROR        0x04
#define SK_ILLEGAL_REQUEST       0x05
#define SK_UNIT_ATTENTION        0x06
#define SK_DATA_PROTECT          0x07
#define SK_BLANK_CHECK           0x08
#define SK_VENDOR_UNIQUE         0x09
#define SK_COPY_ABORTED          0x0a
#define SK_ABORTED_COMMAND       0x0b
#define SK_EQUAL_ON_SEARCHDATA   0x0c
#define SK_VOLUME_OVERFLOW       0x0d
#define SK_MISCOMPARE            0x0e
#define SK_RESERVED              0x0f


/*
 * Primary status definitions
 *
 * This is defined in by the field "PrimaryStatus" in the
 * sense data structure.
 */
#define IS_NO_ERROR              0x00
#define IS_NON_FATAL             0x01
#define IS_TERMINATED            0x02
#define IS_TERMINATED_RESET      0x03
#define IS_COMMAND_ERROR         0x04

/*
 * TTY Signal definitions
 *
 * Signal definitions may be found in termios.h
*/

#define  SCSI_SNS_RESERVED       0xf

static char *sns_msg[SCSI_SNS_RESERVED+1] = {
        "No Sense Data",/* shouldn't happen */
        "Recovered",
        "Unit not ready",
        "Medium",
        "Hardware failure",
        "Illegal request",
        "Unit Attention Condition",
        "Protection",
        "Blank Check",
        "Vendor Unique",
        "Copy Operation Aborted",
        "Aborted Command",
        "Equal Comparison",
        "Volume Overflow",
        "Miscompare",
        "Reserved"
};
#endif
