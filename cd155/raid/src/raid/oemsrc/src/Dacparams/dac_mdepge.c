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
 Name:		dac_mdepge.c
 Title:		DAC_MDEPGE Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:06 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1992, NCR Corp.

 Description:	This module contains the methods for the DAC_MDEPGE class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dsw_param.h"
#include "dac_mdepge.h"
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
#define THIS ((DAC_MDEPGE *) this)
{
	int	i;
	u_long hexvalue;

	if ( strcmp( (char *) str, "d" ) == 0 ) {
		*str_valid = TRUE;
		return( -1 );
	}
	if (strcmp((char *) str, "default") == 0) {
		hexvalue = 0x00;
		*str_valid = TRUE;
		return(hexvalue);
	}
	if (strcmp((char *) str, "current") == 0) {
		hexvalue = 0x01;
		*str_valid = TRUE;
		return(hexvalue);
	}
	if (strcmp((char *) str, "saved") == 0) 
		hexvalue = 0x02;
	else {
		for (i = 0; i < strlen((char *) str) && isxdigit (str[i] ); i++);
		if (i < strlen((char *) str))  {        /* premature termination */
			/* Invalid Selection */
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
#define THIS ((DAC_MDEPGE *) this)
{
	int	status;

	debug("\nEntered set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.Type_of_Mode_Page_to_Sense = val;
		THIS->lapage.Action_Status = MODIFY_LUN;
		status = THIS->sd->mode_select(THIS->sd,
			((u_char * ) & (THIS->lapage)),
	    		sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
	   		SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting Type of Mode Page to Sense Flag*/
		error(SCSI_CONDITION, SET_MDEPGE_ERR);
	return(status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_MDEPGE *) this)
{
	int	status;

	debug("\nEntered read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		*val = THIS->lapage.Type_of_Mode_Page_to_Sense;
	}
	if (status != 0) 
		/*Error in reading Type of Mode Page to Sense Flag*/
		error(SCSI_CONDITION, READ_MDEPGE_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_MDEPGE *) this)
{
	int	status; 
	u_long	val = 0;

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if (status == 0)
	{
		if (val == 0) printf("default");
		else if (val == 1) printf("current");
		else if (val == 2) printf("saved");
		else printf("??");
	}
	else
		printf("??");
	return (status);
}
#undef THIS


void
destroy_DAC_MDEPGE( this )
void *this;
{
	destroy_DSW_PARAM( (DSW_PARAM * ) this );
}


DAC_MDEPGE *
new_DAC_MDEPGE( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DSW_PARAM * s;
	DAC_MDEPGE * this;

	debug( "entered new_DAC_MDEPGE\n" );
	s = new_DSW_PARAM( hw_addr );
	this = (DAC_MDEPGE * ) zalloc( sizeof( DAC_MDEPGE ) );
	memcpy( this, s, sizeof( DSW_PARAM ) ); /* inherit from DSW_PARAM */
	free( s );
	strcpy( this->keyval, "t" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the new mode page to sense - \"default,\" \"current,\" \"saved,\" or\na hexadecimal value:";
	this->description = "  Type Of Mode Page To Sense";
	this->display_text = "Type Of Mode Page To Sense               = ";
	this->destroy = destroy_DAC_MDEPGE;
	this->set = set;
	this->read = read;
	this->edit = edit;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_MDEPGE\n" );
	return( this );
}
