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
#ifndef _DAC_ABTCMD_H
#define _DAC_ABTCMD_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_abtcmd.h
 Title:		DAC_ABTCMD Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:31:22 $ 
 Programmer:	sjr
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DAC_ABTCMD class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_event.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DAC_ABTCMD_CLASS	DAC_EVENT_CLASS
/* end DAC_ABTCMD_CLASS */

typedef struct dac_abtcmd {
	DAC_ABTCMD_CLASS
} DAC_ABTCMD;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DAC_ABTCMD *
new_DAC_ABTCMD( hw_zipcode hw_addr, SCSI_Sense_Data_t *sense_data,
	       int full_description );

void
destroy_DAC_ABTCMD( void *this );

#endif
