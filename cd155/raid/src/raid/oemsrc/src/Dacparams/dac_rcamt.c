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
 Name:		dac_rcamt.c
 Title:		DAC_RCAMT Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:17 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_RCAMT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_rcamt.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_RCAMT *) this)
{
	int	status;
	Bytes_In_Short_t *bval;

	debug("\nEntered set");
	bval =  (Bytes_In_Short_t *) &val;
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.Reconstruction_Amount[0] = bval->MSB;
		THIS->lapage.Reconstruction_Amount[1] = bval->LSB;
		THIS->lapage.Action_Status = MODIFY_LUN;
		status = THIS->sd->mode_select(THIS->sd,
	    	    ((u_char * ) & (THIS->lapage)),
	    	    sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
	    	    SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting Reconstruction Amount*/
		error(SCSI_CONDITION, SET_RCAMT_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_RCAMT *) this)
{
	int	status;
	Bytes_In_Short_t *bval;
	Bytes_In_Long_t  *blval;

	debug("\nEntered read");
	bval =  (Bytes_In_Short_t *) val;
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		bval->MSB = THIS->lapage.Reconstruction_Amount[0];
		bval->LSB = THIS->lapage.Reconstruction_Amount[1];
	}
	if (status != 0) 
		/*Error in reading Reconstruction Amount*/
		error(SCSI_CONDITION, READ_RCAMT_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_RCAMT *) this)
{
	int	status;
	u_long	val = 0;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if (status == 0)
		printf("%ld blocks", val);
	else 
		printf("?? blocks");
	return (status);
}
#undef THIS


void
destroy_DAC_RCAMT( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_RCAMT *
new_DAC_RCAMT( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_RCAMT * this;

	debug( "entered new_DAC_RCAMT\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_RCAMT * ) zalloc( sizeof( DAC_RCAMT ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->lower_bound = 0;	
	this->upper_bound = 32768; 
	strcpy( this->keyval, "a" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the number of blocks to reconstruct per interval:";
	this->description = "  Unit Reconstruction Amount";
	this->display_text = "Unit reconstruction amount               = ";
	this->destroy = destroy_DAC_RCAMT;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_RCAMT\n" );
	return( this );
}


