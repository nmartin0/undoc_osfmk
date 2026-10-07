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
 Name:		dac_time.c
 Title:		DAC_TIME Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:37 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_TIME class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_time.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_TIME *) this)
{

	int	status;
	Bytes_In_Long_t *bval;

	debug( "entered dac_time.set, val = %d\n", val );
	bval = (Bytes_In_Long_t *) &val;

	status = THIS->sd->mode_sense(THIS->sd,
	    ((u_char * ) &(THIS->dtpage)), sizeof(date_time_page_t),
	    PAGE_CONTROL_CURRENT_VALUES, DATE_TIME_PG_CODE);
	if (status == 0) {
		THIS->dtpage.Number_of_Seconds[0] = bval->MSB;
		THIS->dtpage.Number_of_Seconds[1] = bval->MIB_1;
		THIS->dtpage.Number_of_Seconds[2] = bval->MIB_2;
		THIS->dtpage.Number_of_Seconds[3] = bval->LSB;
		status = THIS->sd->mode_select(THIS->sd, ((u_char * )
		    &(THIS->dtpage)), sizeof(date_time_page_t),
		    PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS);
	}
	if (status != 0)
		/*Error in setting dac time*/
		error(SCSI_CONDITION, SET_TIME_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_TIME *) this)
{
	/*dac_time.read not implemented*/
	error(NON_SPECIFIC_ERROR, READ_TIME_NI_ERR);
	return ( 0 );
}
#undef THIS


static int	
display( this )
void *this;
#define THIS ((DAC_TIME *) this)
{

	/*dac_time.display not implemented*/
	error(NON_SPECIFIC_ERROR, DISPLAY_TIME_NI_ERR);
	return ( 0 );
}
#undef THIS


void
destroy_DAC_TIME( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_TIME *
new_DAC_TIME( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_TIME * this;

	debug( "entered new_DAC_TIME\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_TIME * ) zalloc( sizeof( DAC_TIME ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	strcpy( this->keyval, "" );
	this->arg_needed = FALSE;
	this->description = "Disk Array Controller Time-of-Day";
	this->destroy = destroy_DAC_TIME;
	this->set = set;
	this->read = read;
	this->display = display;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_TIME\n" );
	return( this );
}


