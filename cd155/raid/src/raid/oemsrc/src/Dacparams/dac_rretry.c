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
 Name:		dac_rretry.c
 Title:		DAC_RRETRY Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:26 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_RRETRY class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_rretry.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_RRETRY *) this)
{
	int	status;

	debug("\nEntered set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->rwpage,
	    sizeof( THIS->rwpage ), RW_RECOVERY_PG_CODE );
	if (status == 0) {
		THIS->rwpage.Read_Retry_Count = val;
		status = THIS->sd->mode_select(THIS->sd,
		    ((u_char * ) & (THIS->rwpage)),
	    	    sizeof(rw_recovery_page_t), PAGE_FORMAT_SCSI_2,
	    	    SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting read retry count*/
		error(SCSI_CONDITION, SET_RRETRY_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_RRETRY *) this)
{
	int	status;

	debug("\nEntered read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->rwpage,
		sizeof(THIS->rwpage ),PAGE_CONTROL_CURRENT_VALUES,
		RW_RECOVERY_PG_CODE );
	if (status == 0) {
		*val = THIS->rwpage.Read_Retry_Count;
	}
	if (status != 0) 
		/*Error in reading read retry count*/
		error(SCSI_CONDITION, READ_RRETRY_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_RRETRY *) this)
{
	int	status;
	u_long	val = 0;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if (status == 0)
		printf("%ld", val);
	else 
		printf("??");
	return (status);
}
#undef THIS


void
destroy_DAC_RRETRY( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_RRETRY *
new_DAC_RRETRY( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_RRETRY * this;

	debug( "entered new_DAC_RRETRY\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_RRETRY * ) zalloc( sizeof( DAC_RRETRY ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->lower_bound = 0;	
	this->upper_bound = 255; 
	strcpy( this->keyval, "r" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the read retry count:";
	this->description = "  Read Retry Count";
	this->display_text = "Read Retry Count                         = ";
	this->destroy = destroy_DAC_RRETRY;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_RRETRY\n" );
	return( this );
}


