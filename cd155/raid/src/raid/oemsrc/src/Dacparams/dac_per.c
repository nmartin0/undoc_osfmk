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
 Name:		dac_per.c
 Title:		DAC_PER Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:12 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_PER class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dsw_param.h"
#include "dac_per.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_PER *) this)
{
	int	status;

	debug("\nEntered set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->rwpage,
	    sizeof( THIS->rwpage ), RW_RECOVERY_PG_CODE );
	if (status == 0) {
		THIS->rwpage.PER = val;
		status = THIS->sd->mode_select(THIS->sd,
		    ((u_char * ) & (THIS->rwpage)),
	    	    sizeof(rw_recovery_page_t), PAGE_FORMAT_SCSI_2,
	    	    SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting PER Flag*/
		error(SCSI_CONDITION, SET_PER_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_PER *) this)
{
	int	status;

	debug("\nEntered read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->rwpage,
		sizeof(THIS->rwpage ),PAGE_CONTROL_CURRENT_VALUES,
		RW_RECOVERY_PG_CODE );
	if (status == 0) {
		*val = THIS->rwpage.PER;
	}
	if (status != 0) 
		/*Error in reading PER Flag*/
		error(SCSI_CONDITION, READ_PER_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_PER *) this)
{
	int	status;
	u_long	val = 0;

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
destroy_DAC_PER( this )
void *this;
{
	destroy_DSW_PARAM( (DSW_PARAM * ) this );
}


DAC_PER *
new_DAC_PER( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DSW_PARAM * s;
	DAC_PER * this;

	debug( "entered new_DAC_PER\n" );
	s = new_DSW_PARAM( hw_addr );
	this = (DAC_PER * ) zalloc( sizeof( DAC_PER ) );
	memcpy( this, s, sizeof( DSW_PARAM ) ); /* inherit from DSW_PARAM */
	free( s );
	strcpy( this->keyval, "P" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the Recovered Error Reporting (PER) Flag:";
	this->description = "  Recovered Error Reporting";
	this->display_text = "Recovered Error Reporting (PER) flag     = ";
	this->destroy = destroy_DAC_PER;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_PER\n" );
	return( this );
}


