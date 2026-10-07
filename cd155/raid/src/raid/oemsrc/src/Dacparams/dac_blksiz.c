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
 Title:		DAC_BLKSIZ Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:33:48 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_BLKSIZ class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_blksiz.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_BLKSIZ *) this)
{
	return(TRUE);
}
#undef THIS

static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_BLKSIZ *) this)
{
	int	status;
	Bytes_In_Long_t *bval;

	debug("\nEntered set");
	return (0); /* return because this param is unchangeable */
/*
	bval = (Bytes_In_Long_t *) &val;
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.LUN_Block_Size[0] = bval->MSB;
		THIS->lapage.LUN_Block_Size[1] = bval->MIB_1;
		THIS->lapage.LUN_Block_Size[2] = bval->MIB_2;
		THIS->lapage.LUN_Block_Size[3] = bval->LSB;
		status = THIS->sd->lock(THIS->sd);
		if (status == 0)
			status = THIS->sd->mode_select(THIS->sd, ((u_char * )
		    	&(THIS->lapage)), sizeof(logical_array_page_t),
		    	PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS);
	}
	if (status != 0)
		error(SCSI_CONDITION, SET_BLKSIZ_ERR);
	return(status);
*/
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_BLKSIZ *) this)
{
	int	status;
	Bytes_In_Long_t *bval;

	debug("\nEntered read");
	bval = (Bytes_In_Long_t *) val;
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		bval->MSB = THIS->lapage.LUN_Block_Size[0];
		bval->MIB_1 = THIS->lapage.LUN_Block_Size[1];
		bval->MIB_2 = THIS->lapage.LUN_Block_Size[2];
		bval->LSB = THIS->lapage.LUN_Block_Size[3];
	}
	if (status != 0)
		/*Error in reading DAC Block Size*/
		error(SCSI_CONDITION, READ_BLKSIZ_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_BLKSIZ *) this)
{
	int	status;
	u_long	val = 0;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if (status == 0)
		printf("%ld bytes", val);
	else
		printf("?? bytes");
	return (status);
}
#undef THIS


void
destroy_DAC_BLKSIZ( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_BLKSIZ *
new_DAC_BLKSIZ( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_BLKSIZ * this;

	debug( "entered new_DAC_BLKSIZ\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_BLKSIZ * ) zalloc( sizeof( DAC_BLKSIZ ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	strcpy( this->keyval, "b" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the unit logical block size:";
	this->description = "- Unit Logical Block Size";
	this->display_text = "Unit logical block size                  = ";
	this->destroy = destroy_DAC_BLKSIZ;
	/* this->fmt_required = fmt_required; (reconsider later) */
	this->unchangeable = TRUE;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_BLKSIZ\n" );
	return( this );
}


