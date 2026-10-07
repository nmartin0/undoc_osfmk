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
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		getrel.h
 Title:		Get Release
 Version:	   
 Revision:	$Revision: 1.3.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:12 $
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90


 COPYRIGHT 1991-92, NCR Corp.

 Description:	Gets the operating system release number.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include "dau_err.h"
#include "stddefs.h"

/******************************************************************************
 ***				  DEFINITIONS			    ***
 *****************************************************************************/

typedef struct OS1_Device {
	unsigned int	Partition	:4,
			PUN		:4,
			LUN		:3,
			SCSI_Bus	:2,
			SCSI_Controller	:3,
			IO_Bus		:2,
			OS_Major	:14;
	} OS1_Device_t;


typedef struct OS2_Device {
	unsigned int	Partition	:4,
			PUN		:3,
			SCSI_Controller	:1,
			OS_Major	:8,
			LUN		:3,
			SCSI_Bus	:2,
			IO_Bus		:3,
			dummy_field	:8;
	} OS2_Device_t;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

u_long 
get_release ();
