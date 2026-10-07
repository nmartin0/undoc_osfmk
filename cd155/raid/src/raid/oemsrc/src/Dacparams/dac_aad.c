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
 Name:		dac_aad.c
 Title:		DAC_AAD Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:33:34 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_AAD class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dsw_param.h"
#include "dac_aad.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_AAD *) this)
{
	return(TRUE);
}
#undef THIS

static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_AAD *) this)
{
	int	status;

	debug("\nEntered set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.Array_Assurance_Disabled = val;
		status = THIS->sd->lock(THIS->sd);
		if (status == 0)
			status = THIS->sd->mode_select(THIS->sd, 
			((u_char * ) & (THIS->lapage)),
		    	sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
		    	SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting Disable Array Assurance Flag*/
		error(SCSI_CONDITION, SET_AAD_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_AAD *) this)
{
	int	status;

	debug("\nEntered read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		*val = THIS->lapage.Array_Assurance_Disabled;
	}
	if (status != 0) 
		/*Error in reading Disable Array Assurance Flag*/
		error(SCSI_CONDITION, READ_AAD_ERR);
	return (status);
}
#undef THIS

static int	
display_value( this )
void	*this;
#define THIS ((DAC_AAD *) this)
{
	int	status;
	u_long 	val=0;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if ((status == 0) && (val == 1))
		printf("on");
	else if ((status == 0) && (val == 0))
		printf("off");
	else 
		printf("??");
	return (status);
}
#undef THIS

void
destroy_DAC_AAD( this )
void	*this;
{
	destroy_DSW_PARAM( (DSW_PARAM * ) this );
}


DAC_AAD *
new_DAC_AAD( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long	hw_addr;
#endif
{
	DSW_PARAM * s;
	DAC_AAD * this;

	debug( "entered new_DAC_AAD\n" );
	s = new_DSW_PARAM( hw_addr );
	this = (DAC_AAD * ) zalloc( sizeof( DAC_AAD ) );
	memcpy( this, s, sizeof( DSW_PARAM ) ); /* inherit from DSW_PARAM */
	free( s );
	strcpy( this->keyval, "T" );
	this->arg_needed = TRUE;
	this->unchangeable=TRUE;
	this->prompt_str = "Enter the array assurance diagnostics flag:";
	this->description = "- Array assurance diagnostics";
	this->display_text = "Array assurance diagnostics              = ";
	this->destroy = destroy_DAC_AAD;
	this->fmt_required = fmt_required;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_AAD\n" );
	return( this );
}


