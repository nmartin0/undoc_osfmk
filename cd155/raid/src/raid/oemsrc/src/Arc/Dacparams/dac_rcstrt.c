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
 Name:		dac_rcstrt.c
 Title:		DAC_RCSTRT Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:22 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_RCSTRT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_drvsts.h"
#include "dac_rcstrt.h"
#include "menu.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
prompt( this )
void	*this;
{
	/* return( CHANGE_STAT | MAIN_STAT_04 | RECONSTRUCT ); */
	return( 0x94 ); /* "Replace, format, and Reconstruct */
}


void
destroy_DAC_RCSTRT( this )
void	*this;
{
	destroy_DAC_DRVSTS( (DAC_DRVSTS * ) this );
}


DAC_RCSTRT *
new_DAC_RCSTRT( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long hw_addr;
#endif
{
	DAC_DRVSTS * s;
	DAC_RCSTRT * this;

	debug( "entered new_DAC_RCSTRT\n" );
	s = new_DAC_DRVSTS( NULL, hw_addr );
	this = (DAC_RCSTRT * ) zalloc( sizeof( DAC_RCSTRT ) );
	memcpy( this, s, sizeof( DAC_DRVSTS ) ); /* inherit from DAC_DRVSTS */
	free( s );
	strcpy( this->keyval, "s" );
	this->arg_needed = FALSE;
	this->description = "  Initiate Drive Reconstruction";
	this->destroy = destroy_DAC_RCSTRT;
	this->prompt = prompt;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_RCSTRT\n" );
	return( this );
}
