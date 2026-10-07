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
 Name:		dac_drvsts.c
 Title:		DAC_DRVSTS Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:33:57 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_DRVSTS class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_drvsts.h"
#include "menu.h"
#include "dau_err.h"
#include <ctype.h>
#include <stdlib.h>

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
edit( this, str, str_valid )
void	*this;
u_char *str;
int	*str_valid;
#define THIS ((DAC_DRVSTS *) this)
{
	int	i;
	u_long hexvalue;
	u_long MASK = 128;    /*1000 0000*/
	if ( strcmp( (char *) str, "d" ) == 0 ) {
		*str_valid = TRUE;
		return( -1 );
	}
	if (strcmp((char *) str, "add") == 0) 
		hexvalue = 0x80;
	else if (strcmp((char *) str, "delete") == 0) 
		hexvalue = 0x81;
	else if (strcmp((char *) str, "fail") == 0) 
		hexvalue = 0x83;
	else if (strcmp((char *) str, "replace") == 0) 
		hexvalue = 0x94;
	else {
		for (i = 0; i < strlen((char *) str) && isxdigit (str[i] ); i++);
		if (i < strlen((char *) str))  {        /* premature termination */
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
#define THIS ((DAC_DRVSTS *) this)
{
	int	status;
	int	status_byte;
	hw_zipcode_t * hw_addr;

	debug("\nEntered set");
	hw_addr = (hw_zipcode_t *) &THIS->hw_addr;
	debug("  hw_addr is %d", *hw_addr);
	status = THIS->get_page( THIS, (u_char * ) & THIS->papage,
	    sizeof( THIS->papage ), PHYSICAL_ARRAY_PG_CODE );
	if (status == 0) {
		debug("\nDAC_PUN is %d", hw_addr->DAC_PUN);
		debug("  DAC_SCSI_Bus is %d", hw_addr->DAC_SCSI_Bus);
		/* SCSI Channel is 1 relative */
		status_byte=((hw_addr->DAC_PUN * 15)+(hw_addr->DAC_SCSI_Bus
			- DAC_SCSI_BUS_MIN));
		THIS->papage.Drive_Status[status_byte] = val;
		status = THIS->sd->mode_select(THIS->sd,
		    ((u_char * ) & (THIS->papage)),
	    	    sizeof(physical_array_page_t), PAGE_FORMAT_SCSI_2,
	    	    SAVE_PARAMETERS);
	}
	if (status != 0) 
		/*Error in setting drive status*/
		error(SCSI_CONDITION, SET_DRVSTS_ERR);
	return (status);
}
#undef THIS


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_DRVSTS *) this)
{
	int	status;
	int	status_byte;
	hw_zipcode_t * hw_addr;

	debug("\nEntered read");
	hw_addr = (hw_zipcode_t *) &THIS->hw_addr;
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->papage,
		sizeof(THIS->papage ),PAGE_CONTROL_CURRENT_VALUES,
		PHYSICAL_ARRAY_PG_CODE );
	if (status == 0) {
		/* SCSI Channel is 1 relative */
		status_byte=((hw_addr->DAC_PUN * 15)+(hw_addr->DAC_SCSI_Bus
			- DAC_SCSI_BUS_MIN));
		*val = THIS->papage.Drive_Status[status_byte];
	}
	if (status != 0) 
		/*Error in reading drive status*/
		error(SCSI_CONDITION, READ_DRVSTS_ERR);
	return (status);
}
#undef THIS


static int	
display_value( this )
void	*this;
#define THIS ((DAC_DRVSTS *) this)
{
	int	i, status;
	u_long	val = 0;
	char	*mode_sense_messages [0x67]; /* Hex 66 + 1 */


	for (i = 0; i < strtol("67", NULL, 16); i++)
		mode_sense_messages [i] = "Not Implemented";
	mode_sense_messages[strtol("00",NULL,16)] = "Optimal: Optimal drive";
	mode_sense_messages[strtol("01",NULL,16)] = "Non-existent: Non-existent drive";
	mode_sense_messages[strtol("02",NULL,16)] = "Spare: Unconfigured drive";
	mode_sense_messages[strtol("03",NULL,16)] = "Failed: Failed drive";
	mode_sense_messages[strtol("04",NULL,16)] = "Replaced: No action in progress";
	mode_sense_messages[strtol("05",NULL,16)] = "Warning: Drive warning";
	mode_sense_messages[strtol("06",NULL,16)] = "Mismatch: No info available";
	mode_sense_messages[strtol("07",NULL,16)] = "Controller: This controller";
	mode_sense_messages[strtol("08",NULL,16)] = "Format: Drive format initiated";
	mode_sense_messages[strtol("09",NULL,16)] = "Incorrect Drive: Wrong replacement";
	mode_sense_messages[strtol("11",NULL,16)] = "Non-existent: Non-supported channel";
	mode_sense_messages[strtol("13",NULL,16)] = "Failed: Component failure";
	mode_sense_messages[strtol("14",NULL,16)] = "Replaced: Format initiated";
	mode_sense_messages[strtol("16",NULL,16)] = "Mismatch: Wrong sector size";
	mode_sense_messages[strtol("21",NULL,16)] = "Non-existent: Non-supported ID";
	mode_sense_messages[strtol("23",NULL,16)] = "Failed: Test Unit Ready failure";
	mode_sense_messages[strtol("24",NULL,16)] = "Replaced: Reconstruction initiated";
	mode_sense_messages[strtol("26",NULL,16)] = "Mismatch: Wrong capacity";
	mode_sense_messages[strtol("31",NULL,16)] = "Non-existent: Invalid channel and ID";
	mode_sense_messages[strtol("33",NULL,16)] = "Failed: Format failure";
	mode_sense_messages[strtol("36",NULL,16)] = "Mismatch: Incorrect mode params";
	mode_sense_messages[strtol("43",NULL,16)] = "Failed: Write failure";
	mode_sense_messages[strtol("46",NULL,16)] = "Mismatch: Wrong serial number";
	mode_sense_messages[strtol("53",NULL,16)] = "Failed: User failed via Mode Select";
	mode_sense_messages[strtol("56",NULL,16)] = "Mismatch: Channel mismatch";
	mode_sense_messages[strtol("63",NULL,16)] = "Failed: Start of Day failure";
	mode_sense_messages[strtol("66",NULL,16)] = "Mismatch: ID mismatch";

	debug("\nEntered display");
	status = THIS->read(THIS, &val);
	if ((status == 0) && ((val != 0x00) && (val != 0x01) && (val != 0x03)
	     && (val != 0x04) && (val != 0x05) && (val != 0x06) && (val != 0x07)
	     && (val != 0x08) && (val != 0x11) && (val != 0x14) && (val != 0x16)
	     && (val != 0x21) && (val != 0x24) && (val != 0x26) && (val != 0x31)
	     && (val != 0x02) && (val != 0x56) && (val != 0x66)
	     && (val != 0x13) && (val != 0x23) && (val != 0x33) && (val != 0x43)
	     && (val != 0x53) && (val != 0x63) /* added more drive failures */
	     && (val != 0x36) && (val != 0x46) && (val != 0x09)))
		printf("%x", val);
	else if (status == 0) 
		printf("%s", mode_sense_messages [val]);
	else 
		printf("??");
	return (status);
}
#undef THIS


void
destroy_DAC_DRVSTS( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}



DAC_DRVSTS *
new_DAC_DRVSTS( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_DRVSTS * this;

	debug( "entered new_DAC_DRVSTS\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_DRVSTS * ) zalloc( sizeof( DAC_DRVSTS ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	strcpy( this->keyval, "c" );
	this->arg_needed = TRUE;
	this->prompt_str = "Enter the new drive status - \"add,\" \"delete,\" \"fail,\" \"replace,\" or\na hexadecimal value:";
	this->description = "Drive status";
	this->display_text = "Drive Status                             = ";
	this->hw_addr = hw_addr;
	debug("hw_addr is %ld", hw_addr);
	this->edit = edit;
	this->destroy = destroy_DAC_DRVSTS;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_DRVSTS\n" );
	return( this );
}


