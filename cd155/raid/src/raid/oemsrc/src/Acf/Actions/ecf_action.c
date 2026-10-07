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
 Name:		ecf_action.c
 Title:		ECF_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:41 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the ECF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "ecf_action.h"
#include "ecf_menu.h"
#include "dac_rretry.h"
#include "dac_wretry.h"
#include "dac_arre.h"
#include "dac_awre.h"
#include "dac_per.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
ECF_ACTION *this;
{
	return ( 0 );
}

void
destroy_ECF_ACTION( this )
void *this;
{
	debug( "entered destroy_CF_ACTION\n" );
	destroy_CF_ACTION( (CF_ACTION *) this );
}

ECF_ACTION *
new_ECF_ACTION( menu )
MENU *menu;
{
	CF_ACTION *s;
	ECF_ACTION *this;

	s = new_CF_ACTION();
	this = (ECF_ACTION *) zalloc( sizeof( ECF_ACTION ) );
	memcpy( this, s, sizeof( CF_ACTION ) ); /* inherit from CF_ACTION */
	free( s );
	this->new_menu = ( MENU *(*)() ) new_ECF_MENU;
	this->newdp[0] = ( DAC_PARAM *(*)() ) new_DAC_RRETRY;
	this->newdp[1] = ( DAC_PARAM *(*)() ) new_DAC_WRETRY;
	this->newdp[2] = ( DAC_PARAM *(*)() ) new_DAC_ARRE;
	this->newdp[3] = ( DAC_PARAM *(*)() ) new_DAC_AWRE;
	this->newdp[4] = ( DAC_PARAM *(*)() ) new_DAC_PER;
	strcpy( this->keyval, "e" );
	this->arg_needed = FALSE;
	this->description = "Set/Display Array Error Recovery Parameters";
	this->destroy = destroy_ECF_ACTION;
	if ( menu ) {
		menu->actions->add( menu->actions, this );
	}
	debug( "leaving new_ECF_ACTION\n" );
	return( this );
}
