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
 Name:		dac_datpro.c
 Title:		DAC_DATPRO Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:31:54 $ 
 Programmer:	sjr
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_DATPRO class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_datpro.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

#define THIS	((DAC_DATPRO *) this )

void
destroy_DAC_DATPRO( this )
void *this;
{
	destroy_DAC_EVENT(this);
}

#undef THIS

DAC_DATPRO *
new_DAC_DATPRO( hw_addr, sense_data, full_description )
hw_zipcode hw_addr;
SCSI_Sense_Data_t *sense_data;
int full_description;
{
	DAC_EVENT *s;
	DAC_DATPRO *this;

	debug( "entered new_DAC_DATPRO\n" );
	s = new_DAC_EVENT( hw_addr, sense_data, full_description );
	this = (DAC_DATPRO *) zalloc( sizeof( DAC_DATPRO ) );
	memcpy( this, s, sizeof( DAC_EVENT ) ); /* inherit from DAC_EVENT */
	free( s );
	this->hw_addr = hw_addr;
	this->destroy = destroy_DAC_DATPRO;
	debug( "leaving new_DAC_DATPRO\n" );
	return( this );
}
