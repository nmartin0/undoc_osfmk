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
 Name:		dac_sg0siz.c
 Title:		DAC_SG0SIZ Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:35 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_SG0SIZ class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_sg0siz.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_SG0SIZ *) this)
{
	return(TRUE);
}
#undef THIS

static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_SG0SIZ *) this)
{
	int	status;

	debug("\nEntered set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.Segment_Zero_Size = val;
		status = THIS->sd->lock(THIS->sd);
		if (status == 0)
			status = THIS->sd->mode_select(THIS->sd,
			((u_char * ) & (THIS->lapage)),
		    	sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
		    	SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting Segment 0 Size*/
		error(SCSI_CONDITION, SET_SG0SIZ_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_SG0SIZ *) this)
{
	int	status;

	debug("\nEntered read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		*val = THIS->lapage.Segment_Zero_Size;
	}
	if (status != 0) 
		/*Error in reading Segment 0 Size*/
		error(SCSI_CONDITION, READ_SG0SIZ_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_SG0SIZ *) this)
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
destroy_DAC_SG0SIZ( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_SG0SIZ *
new_DAC_SG0SIZ( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_SG0SIZ * this;

	debug( "entered new_DAC_SG0SIZ\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_SG0SIZ * ) zalloc( sizeof( DAC_SG0SIZ ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->lower_bound = 0;	
	this->upper_bound = 255; 
	strcpy( this->keyval, "Z" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the number of blocks for unit segment zero:";
	this->description = "* Unit Segment Zero Size";
	this->display_text = "Unit segment zero size                   = ";
	this->destroy = destroy_DAC_SG0SIZ;
	this->fmt_required = fmt_required;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_SG0SIZ\n" );
	return( this );
}


