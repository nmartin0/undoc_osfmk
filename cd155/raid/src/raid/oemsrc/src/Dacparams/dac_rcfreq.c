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
 Name:		dac_rcfreq.c
 Title:		DAC_RCFREQ Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:20 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_RCFREQ class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_rcfreq.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_RCFREQ *) this)
{
	int	status;

	debug("\nEntered set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.Action_Status = MODIFY_LUN;
		THIS->lapage.Reconstruction_Frequency = val;
		status = THIS->sd->mode_select
	    	    (THIS->sd, ((u_char * ) & (THIS->lapage)),
	    	    sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
	    	    SAVE_PARAMETERS);
	}
	if (status != 0)
		/*Error in setting Reconstruction frequency*/
		error(SCSI_CONDITION, SET_RCFREQ_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_RCFREQ *) this)
{
	int	status;

	debug("\nEntered read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		*val = THIS->lapage.Reconstruction_Frequency;
	}
	if (status != 0)
		/*Error in reading Reconstruction frequency*/
		error(SCSI_CONDITION, READ_RCFREQ_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_RCFREQ *) this)
{
	int	status;
	u_long	val = 0;
	float 	fval;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if (status == 0) fval = (float) val / 10;
	if (status == 0)
		printf("%.1f seconds", fval);
	else
		printf("?? seconds");
	return (status);
}
#undef THIS


void
destroy_DAC_RCFREQ( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_RCFREQ *
new_DAC_RCFREQ( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_RCFREQ * this;

	debug( "entered new_DAC_RCFREQ\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_RCFREQ * ) zalloc( sizeof( DAC_RCFREQ ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->lower_bound = 0;	
	this->upper_bound = 255; 
	strcpy( this->keyval, "F" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the reconstruction frequency in tenths of a second:";
	this->description = "  Unit Reconstruction Frequency";
	this->display_text = "Unit reconstruction frequency            = ";
	this->destroy = destroy_DAC_RCFREQ;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_RCFREQ\n" );
	return( this );
}


