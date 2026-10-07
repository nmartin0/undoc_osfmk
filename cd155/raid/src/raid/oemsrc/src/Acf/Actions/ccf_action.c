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
 Name:		ccf_action.c
 Title:		CCF_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:36 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the CCF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "ccf_menu.h"
#include "ccf_action.h"
#include "can_action.h"
#include "zip_dialog.h"
/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
#define THIS ((CCF_ACTION *) this)
{
	int i;
	CCF_MENU *m;
	hw_zipcode hw_addr;
	CAN_ACTION *action;
	extern ZIP_DIALOG *zip_dialog;

	zip_dialog->reset( (DIALOG *) zip_dialog );
	hw_addr = (hw_zipcode) zip_dialog->prompt( (DIALOG *) zip_dialog );
	m = new_CCF_MENU();
	for ( i = 0; action = new_CAN_ACTION( hw_addr, i ); i++ )
		m->add_selection( m, action );
	action = (CAN_ACTION *) TRUE;
	while ( action ) {
		action = ( CAN_ACTION * ) m->prompt( m );
		if ( (int) action )
			action->Do( action );
	}
	m->destroy( m );
	return ( 0 );
}
#undef THIS

void
destroy_CCF_ACTION( this )
void *this;
{
	destroy_ACTION( (ACTION *) this );
}

CCF_ACTION *
new_CCF_ACTION( menu )
MENU *menu;
{
	ACTION *s;
	CCF_ACTION *this;

	s = new_ACTION();
	this = (CCF_ACTION *) zalloc( sizeof( CCF_ACTION ) );
	memcpy( this, s, sizeof( ACTION ) ); /* inherit from ACTION */
	destroy_ACTION( s );
	strcpy( this->keyval, "c" );
	this->arg_needed = FALSE;
	this->description = "Select Pre-defined Array Configuration";
	this->destroy = destroy_CCF_ACTION;
	this->Do = Do;
	menu->add_selection( menu, this );
	debug( "leaving new_CCF_ACTION\n" );
	return( this );
}
