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
 Name:		inq_action.c
 Title:		INQ_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:44 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the INQ_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "inq_action.h"
#include "dac_inqdat.h"
#include "zip_dialog.h"
/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
#define THIS ((INQ_ACTION *) this)
{
	hw_zipcode hw_addr;

	hw_zipcode_t *hw;

	extern ZIP_DIALOG *zip_dialog;
	DAC_INQDAT *dp;
	u_int status = 0;
	zip_dialog->reset( (DIALOG *) zip_dialog );
	hw_addr = (hw_zipcode) zip_dialog->prompt( (DIALOG *) zip_dialog );

	dp = new_DAC_INQDAT( NULL, hw_addr );


	status = dp->display( dp ); 
	dp->destroy( dp );
	return(status);

}
#undef THIS

void
destroy_INQ_ACTION( this )
void *this;
{
	destroy_ACTION( (ACTION *) this );
}

INQ_ACTION *
new_INQ_ACTION( menu )
MENU *menu;
{
	ACTION *s;
	INQ_ACTION *this;

	s = new_ACTION();
	this = (INQ_ACTION *) zalloc( sizeof( INQ_ACTION ) );
	memcpy( this, s, sizeof( ACTION ) ); /* inherit from ACTION */
	destroy_ACTION( s );
	strcpy( this->keyval, "i" );
	this->arg_needed = FALSE;
	this->description = "Display Device Inquiry Data";
	this->destroy = destroy_INQ_ACTION;
	this->Do = Do;
	menu->add_selection( menu, this );
	debug( "leaving new_INQ_ACTION\n" );
	return( this );
}
