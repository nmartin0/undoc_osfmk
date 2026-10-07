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
 Name:		dac_arcmde.c
 Title:		DAC_ARCMDE Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:33:39 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1992, NCR Corp.

 Description:	This module contains the methods for the DAC_ARCMDE class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_arcmde.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

static u_long
edit( this, str, str_valid )
void	*this;
u_char *str;
int	*str_valid;
#define THIS ((DAC_ARCMDE *) this)
{
	u_long i;
	u_long hexvalue;

	if ( strcmp( (char *) str, "d" ) == 0 ) {
		*str_valid = TRUE;
		return( -1 );
	}
	if (strcmp((char *) str, "release") == 0) 
		hexvalue = 0x08;
	else if (strcmp((char *) str, "fail") == 0) 
		hexvalue = 0x0C;
	else {
		for (i = 0; i < strlen((char *)str) && isxdigit (str[i] ); i++);
		if (i < strlen((char *) str))  {       /*premature termination*/
			/*Invalid Selection*/
			error(ARGUMENT_RANGE, INVALID_SELECT);
			*str_valid = FALSE;
			return (0);
		} else 
			hexvalue = strtol((char *) str, NULL, 16);
	}
	*str_valid = TRUE;
	return(hexvalue);
}
#undef THIS


static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_ARCMDE *) this)
{
	u_long	status;
	Bytes_In_Short_t *bval;

	debug("\nEntered set");
	bval =  (Bytes_In_Short_t *) &val;
	status = THIS->get_page( THIS, (u_char * ) & THIS->rcpage,
	    sizeof( THIS->rcpage ), REDUNDANT_CONTROLLER_PG_CODE );
	if (status == 0) {
		THIS->rcpage.Alt_RDAC_Mode[0] = bval->MSB;
		THIS->rcpage.Alt_RDAC_Mode[1] = bval->LSB;
		status = THIS->sd->mode_select(THIS->sd,
	    	    ((u_char * ) & (THIS->rcpage)),
	    	    sizeof(redundant_controller_page_t), PAGE_FORMAT_SCSI_2,
	    	    SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting alternate redundant controller mode*/
		error(SCSI_CONDITION, SET_ARCMDE_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_ARCMDE *) this)
{
	u_long	status;
	Bytes_In_Short_t *bval;
	Bytes_In_Long_t  *blval;

	debug("\nEntered read");
	bval =  (Bytes_In_Short_t *) val;
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->rcpage,
		sizeof(THIS->rcpage ),PAGE_CONTROL_CURRENT_VALUES,
		REDUNDANT_CONTROLLER_PG_CODE );
	if (status == 0) {
		bval->MSB = THIS->rcpage.Alt_RDAC_Mode[0];
		bval->LSB = THIS->rcpage.Alt_RDAC_Mode[1];
	}
	if (status != 0) 
		/*Error in reading alternate redundant controller mode*/
		error(SCSI_CONDITION, READ_ARCMDE_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_ARCMDE *) this)
{
	u_long	status;
	u_long	val = 0;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if (status == 0)
	{
		if (val == 0) printf("passive");
		else
			if (val == 1) printf("active");
			else
				printf("??");
	}
	else 
		printf("??");
	return (status);
}
#undef THIS


void
destroy_DAC_ARCMDE( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_ARCMDE *
new_DAC_ARCMDE( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_ARCMDE * this;

	debug( "entered new_DAC_ARCMDE\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_ARCMDE * ) zalloc( sizeof( DAC_ARCMDE ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->lower_bound = 0;	
	this->upper_bound = 65535; 
	strcpy( this->keyval, "m" );
	this->arg_needed = TRUE;
	this->prompt_str = "Set the alternate redundant controller operational mode - \n\"fail\" or \"release\": ";
	this->description = "  Alternate Redundant Controller Mode";
	this->display_text = "Alternate Redundant Controller Mode      = ";
	this->destroy = destroy_DAC_ARCMDE;
	this->set = set;
	this->read = read;
	this->edit = edit;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_ARCMDE\n" );
	return( this );
}


