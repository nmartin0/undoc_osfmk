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
 Name:		dcf_action.c
 Title:		DCF_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:39 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DCF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dcf_action.h"
#include "dcf_menu.h"
#include "dac_drvsts.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
{
	return ( 0 );
}

void
destroy_DCF_ACTION( this )
void *this;
{
	debug( "entered destroy_CF_ACTION\n" );
	destroy_CF_ACTION( (CF_ACTION *) this );
}

DCF_ACTION *
new_DCF_ACTION( menu )
MENU *menu;
{
	CF_ACTION *s;
	DCF_ACTION *this;

	s = new_CF_ACTION();
	this = (DCF_ACTION *) zalloc( sizeof( DCF_ACTION ) );
	memcpy( this, s, sizeof( CF_ACTION ) ); /* inherit from CF_ACTION */
	free( s );
	this->new_menu = ( MENU *(*)() ) new_DCF_MENU;
	this->newdp[0] = ( DAC_PARAM *(*)() ) new_DAC_DRVSTS;
	strcpy( this->keyval, "d" );
	this->arg_needed = FALSE;
	this->description = "Set/Display Array Drive Parameters";
	this->destroy = destroy_DCF_ACTION;
	if ( menu ) {
		menu->actions->add( menu->actions, this );
	}
	debug( "leaving new_DCF_ACTION\n" );
	return( this );
}
