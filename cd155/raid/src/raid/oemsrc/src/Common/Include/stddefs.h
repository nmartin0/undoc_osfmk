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
#ifndef _STDDEFS_H
#define _STDDEFS_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		stddefs.h
 Title:		Standard Definitions
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:38 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991-92, NCR Corp.

 Description:	This module contains common, standard definitions for the
		disk array utility modules.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "sysdep.h"
#include "scsidefs.h"

/******************************************************************************
 ***				  MACROS      				    ***
 *****************************************************************************/
/*
   c - must be a pointer to a charater string
   d - integer value
*/
#define set_long(c,d)	{ Bytes_In_Long_t *s; \
			  s = (Bytes_In_Long_t *) &d; \
			  c[0] = s->MSB; \
			  c[1] = s->MIB_1; \
			  c[2] = s->MIB_2; \
			  c[3] = s->LSB; \
			}

#define set_short(c,d)	{ Bytes_In_Short_t *s; \
			  s = (Bytes_In_Short_t *) &d; \
			  c[0] = s->MSB; \
			  c[1] = s->LSB; \
			}
/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define PROJREL "@(#) DAC Utils PR_03000102"

#define	TRUE	1
#define FALSE	0
#ifndef NULL
#define	NULL	(void *) 0
#define NO_FUNCTION	(void (*)()) 0
#endif
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
#define hw_zipcode	double
#endif
#define Bytes_t Bytes_In_Long_t

#define SYSBUS_DELIM	'm'
#define SLOTNUM_DELIM	'c'
#define SCSIBUS_DELIM	'b'
#define TARGET_DELIM	'p'
#define LUN_DELIM	'l'

#ifdef NO_ERRMODULE
#define	error( err_code, msg_code )	printf( "err code %d, msg code %d\n", \
						err_code, msg_code );
#endif

/* Exit Statuses */
#define COMMAND_LINE_SYNTAX		1
#define ARGUMENT_RANGE			2
#define DEVICE_OPEN			3
#define SCSI_CONDITION			4
#define NON_SPECIFIC_ERROR		5
#define NO_MEMORY			6
#define NON_EXISTENT_MESSAGE		7
#define INAPPROPRIATE_STATE		8
#define FILE_OR_DEVICE_OPEN		DEVICE_OPEN

typedef struct {
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
	u_long
			:3,
	SCSI_Controller	:4,	/* host adapter */
	SCSI_Bus	:3,	/* SCSI channel off of adapter */
	PUN		:4,	/* target SCSI ID */
	LUN		:3,	/* SCSI logical unit number */
	DAC_SCSI_Bus	:4,	/* SCSI channel off of array controller */
	DAC_PUN		:4,	/* array drive SCSI ID */
	DAC_LUN		:3;	/* array drive LUN */
	u_long		IO_Bus;		/* system bus */
#else
	u_long
	IO_Bus		:3,	/* system bus */
	SCSI_Controller	:4,	/* host adapter */
	SCSI_Bus	:3,	/* SCSI channel off of adapter */
	PUN		:4,	/* target SCSI ID */
	LUN		:3,	/* SCSI logical unit number */
	DAC_SCSI_Bus	:4,	/* SCSI channel off of array controller */
	DAC_PUN		:4,	/* array drive SCSI ID */
	DAC_LUN		:3;	/* array drive LUN */
#endif
} hw_zipcode_t;

/* The following range min/max values are based on the number of bits in     */
/* hw_zipcode_t, above.							     */
#define IO_BUS_MIN		0
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
#define IO_BUS_MAX		65536
#else
#define IO_BUS_MAX		7
#endif
#define SCSI_SLOT_MIN		0
#define SCSI_SLOT_MAX		15
#define SCSI_BUS_MIN		0
#define SCSI_BUS_MAX		7
#define SCSI_PUN_MIN		0
#define SCSI_PUN_MAX		15
#define SCSI_LUN_MIN		0
#define SCSI_LUN_MAX		7
#define DAC_SCSI_BUS_MIN	1	/* Yes, 1-relative */
#define DAC_SCSI_BUS_MAX	15
#define DAC_SCSI_PUN_MIN	0
#define DAC_SCSI_PUN_MAX	15
#define DAC_SCSI_LUN_MIN	0
#define DAC_SCSI_LUN_MAX	7

typedef struct Bytes_In_Long {
	u_long	LSB:	8,
		MIB_2:	8,
		MIB_1:	8,
		MSB:	8;
} Bytes_In_Long_t;
#define leftMIB		MIB_1
#define rightMIB	MIB_2

typedef struct Bytes_In_Short {
	u_short	LSB:	8,
		MSB:	8;
} Bytes_In_Short_t;

typedef struct Bits_In_Short {
	u_short	b8:	1,
		b9:	1,
		b10:	1,
		b11:	1,
		b12:	1,
		b13:	1,
		b14:	1,
		b15:	1,
		b0:	1,
		b1:	1,
		b2:	1,
		b3:	1,
		b4:	1,
		b5:	1,
		b6:	1,
		b7:	1;
} Bits_In_Short_t;

/* SCSI I/O return status format: */
typedef struct scsi_stat {
	u_long	Errno		:16,
		Reserved	:14,
		Sense_Present	:1,
		Error		:1;
} scsi_stat_t;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

char *
zalloc( int amount );		/* crucial to DOS port! */

#endif
