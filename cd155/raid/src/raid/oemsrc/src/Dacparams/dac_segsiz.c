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
 Name:		dac_blksiz.c
 Title:		DAC_SEGSIZ Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:31 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_SEGSIZ class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_segsiz.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_SEGSIZ *) this)
{
	return(TRUE);
}
#undef THIS

static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_SEGSIZ *) this)
{
	int	status;
	Bytes_In_Long_t *bval;

	debug("\nEntered set");
	bval = (Bytes_In_Long_t *) &val;
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.LUN_Segment_Size[0] = bval->MSB;
		THIS->lapage.LUN_Segment_Size[1] = bval->MIB_1;
		THIS->lapage.LUN_Segment_Size[2] = bval->MIB_2;
		THIS->lapage.LUN_Segment_Size[3] = bval->LSB;
		status = THIS->sd->lock(THIS->sd);
		if (status == 0)
			status = THIS->sd->mode_select(THIS->sd,
			((u_char * ) & (THIS->lapage)),
		    	sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
		    	SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting DAC Segment Size*/
		error(SCSI_CONDITION, SET_SEGSIZ_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_SEGSIZ *) this)
{
	int	status;
	Bytes_In_Long_t *bval;

	debug("\nEntered read");
	bval = (Bytes_In_Long_t *) val;
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		bval->MSB = THIS->lapage.LUN_Segment_Size[0];
		bval->MIB_1 = THIS->lapage.LUN_Segment_Size[1];
		bval->MIB_2 = THIS->lapage.LUN_Segment_Size[2];
		bval->LSB = THIS->lapage.LUN_Segment_Size[3];
	}
	if (status != 0) 
		/*Error in reading DAC Segment Size*/
		error(SCSI_CONDITION, READ_SEGSIZ_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_SEGSIZ *) this)
{
	int	status;
	u_long	val;

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
destroy_DAC_SEGSIZ( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_SEGSIZ *
new_DAC_SEGSIZ( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_SEGSIZ * this;

	debug( "entered new_DAC_SEGSIZ\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_SEGSIZ * ) zalloc( sizeof( DAC_SEGSIZ ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	strcpy( this->keyval, "S" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the unit segment size:";
	this->description = "* Unit Segment Size";
	this->display_text = "Unit segment size                        = ";
	this->destroy = destroy_DAC_SEGSIZ;
	this->fmt_required = fmt_required;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_SEGSIZ\n" );
	return( this );
}


