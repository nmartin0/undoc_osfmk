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
 Name:		dac_lunsts.c
 Title:		DAC_LUNSTS Class Implementation
 Version:	
 Revision:	$Revision: 1.3.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:03 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_LUNSTS class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_lunsts.h"
#include "menu.h"
#include "dau_err.h"
#include <ctype.h>
#include <stdlib.h>

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_LUNSTS *) this)
{
	return(TRUE);
}
#undef THIS

static u_long
edit( this, str, str_valid )
void	*this;
u_char *str;
int	*str_valid;
#define THIS ((DAC_LUNSTS *) this)
{
	int	i;
	u_long hexvalue;

	if ( strcmp( (char *) str, "d" ) == 0 ) {
		*str_valid = TRUE;
		return( -1 );
	}
	if (strcmp((char *) str, "a") == 0) {
#ifdef PARAGON860
 /* Invalid Selection */
		 error(ARGUMENT_RANGE, INVALID_SELECT);
		 *str_valid = FALSE;
		 return (0);
#else
		hexvalue = 0x80;
		*str_valid = TRUE;
		return(hexvalue);
#endif
	}
	if (strcmp((char *) str, "x") == 0) 
		hexvalue = 0x81;
	else {
#ifdef PARAGON860
 /* Invalid Selection */
		 error(ARGUMENT_RANGE, INVALID_SELECT);
		 *str_valid = FALSE;
		 return (0);
#else
		for (i = 0; i < strlen((char *) str) && isxdigit (str[i] ); i++);
		if (i < strlen((char *) str))  {    /* premature termination */
			/* Invalid Selection */
			error(ARGUMENT_RANGE, INVALID_SELECT);
			*str_valid = FALSE;
			return (0);
		} else 
			hexvalue = strtol((char *) str, NULL, 16);
#endif
	}
	*str_valid = TRUE;
	return(hexvalue);
}


#undef THIS

static int	
set( this, val )
void	*this;
u_long val;
#define THIS ((DAC_LUNSTS *) this)
{
	int	status = 0;

	debug("\nEntered set");

	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		THIS->lapage.Action_Status = val;
		status = THIS->sd->lock(THIS->sd);
		if (status == 0)
			status = THIS->sd->mode_select(THIS->sd,
		    	((u_char * ) & (THIS->lapage)),
		    	sizeof(logical_array_page_t), PAGE_FORMAT_SCSI_2,
		    	SAVE_PARAMETERS);
	}
	if (status != 0)
		/*Error in setting LUN status*/
		error(SCSI_CONDITION, SET_LUNSTS_ERR);
	return (status);
}


#undef THIS

static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_LUNSTS *) this)
{
	int	status = 0;

	/*
		 * should look something like this:
		 *
		 * if ( LU exists )
		 *	use mode_sense (not get_page) to read status
		 * else
		 * 	*val = "non-existent" (code TBD)
		 */
	debug("\nEntered read");
	if (THIS->lu_exists(THIS)) {
		status = THIS->sd->mode_sense(THIS->sd, ((u_char * ) & 
		    (THIS->lapage)), sizeof(logical_array_page_t),
		    PAGE_CONTROL_CURRENT_VALUES,
		    LOGICAL_ARRAY_PG_CODE);
		if (status == 0) {
			*val = THIS->lapage.Action_Status;
		}
		if (status != 0)
			/*Error in reading LUN status*/
			error(SCSI_CONDITION, READ_LUNSTS_ERR);
		return (status);
	} else {
		*val = 0xff;
		return(status);
	}
}


#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_LUNSTS *) this)
{
	int	i, status;
	u_long	val = 0;
	char	*ms_messages [0x85]; /* Hex 84 + 1 */


	debug("\nEntered display_value");
	for (i = 0; i < strtol("85", NULL, 16); i++)
		ms_messages [i] = "Not Implemented";
	ms_messages[strtol("00",NULL,16)] = "Optimal: Optimal condition";
	ms_messages[strtol("01",NULL,16)] = "Degraded: Drive failure";
	ms_messages[strtol("02",NULL,16)] = "Reconstructing: Initiated";
	ms_messages[strtol("04",NULL,16)] = "Dead: Multiple drive failures";
	ms_messages[strtol("10",NULL,16)] = "Optimal: Parameter mismatch";
	ms_messages[strtol("11",NULL,16)] = "Degraded: Parameter mismatch";
	ms_messages[strtol("12",NULL,16)] = "Reconstructing: Parameter mismatch";
	ms_messages[strtol("14",NULL,16)] = "Dead: Parameter mismatch";
	ms_messages[strtol("21",NULL,16)] = "Degraded: Channel mismatch";
	ms_messages[strtol("22",NULL,16)] = "Reconstructing: Channel mismatch";
	ms_messages[strtol("24",NULL,16)] = "Dead: Channel mismatch";
	ms_messages[strtol("31",NULL,16)] = "Degraded: ID mismatch";
	ms_messages[strtol("34",NULL,16)] = "Dead: ID mismatch";
	ms_messages[strtol("40",NULL,16)] = "Optimal: RAID 0 drive formatting";
	ms_messages[strtol("41",NULL,16)] = "Degraded: Replaced drive formatting";
	ms_messages[strtol("44",NULL,16)] = "Dead: Format in progress";
	ms_messages[strtol("54",NULL,16)] = "Dead: Awaiting format";
	ms_messages[strtol("74",NULL,16)] = "Dead: Replaced wrong drive";
	ms_messages[strtol("81",NULL,16)] = "Degraded: Component failure";
	ms_messages[strtol("82",NULL,16)] = "Reconstructing: Component failure";
	ms_messages[strtol("84",NULL,16)] = "Dead: Component failure";
	status = THIS->read(THIS, &val);
	if ((status == 0) && ((val != 0x00) && (val != 0x01) && (val != 0x02)
	     && (val != 0x04) && (val != 0x10) && (val != 0x11) && (val != 0x12)
	     && (val != 0x14) && (val != 0x21) && (val != 0x22) && (val != 0x24)
	     && (val != 0x31) && (val != 0x34) && (val != 0x44) && (val != 0x54)
	     && (val != 0x74) && (val != 0x81) && (val != 0x82) && (val != 0x41)
	     && (val != 0x84) && (val != 0x40)))
		printf("%x", val);
	else if (status == 0) 
		printf("%s", ms_messages [val]);
	else 
		printf("??");
	return (status);
}


#undef THIS

void
destroy_DAC_LUNSTS( this )
void	*this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_LUNSTS *
new_DAC_LUNSTS( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_LUNSTS * this;

	debug( "entered new_DAC_LUNSTS\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_LUNSTS * ) zalloc( sizeof( DAC_LUNSTS ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	strcpy( this->keyval, "U" );
	this->arg_needed = TRUE;
	this->prompt_str ="Enter 'a' to add or 'x' to delete:";
	this->description = "* Unit State";
	this->display_text = "Unit state                               = ";
	this->destroy = destroy_DAC_LUNSTS;
	this->fmt_required = fmt_required;
	this->edit = edit;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_LUNSTS\n" );
	return( this );
}


