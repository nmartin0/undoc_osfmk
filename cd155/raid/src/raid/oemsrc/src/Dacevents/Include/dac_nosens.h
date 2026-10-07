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
#ifndef _DAC_NOSENS_H
#define _DAC_NOSENS_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_nosens.h
 Title:		DAC_NOSENS Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:31:38 $ 
 Programmer:	sjr
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DAC_NOSENS class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_event.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DAC_NOSENS_CLASS	DAC_EVENT_CLASS
/* end DAC_NOSENS_CLASS */

typedef struct dac_nosens {
	DAC_NOSENS_CLASS
} DAC_NOSENS;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DAC_NOSENS *
new_DAC_NOSENS( hw_zipcode hw_addr, SCSI_Sense_Data_t *sense_data,
	       int full_description );

void
destroy_DAC_NOSENS( void *this );

#endif
