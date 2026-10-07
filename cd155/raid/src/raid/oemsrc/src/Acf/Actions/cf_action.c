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
 Name:		cf_action.c
 Title:		CF_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:37 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the CF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "cf_action.h"
#include "menu.h"
#include "zip_dialog.h"

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern ZIP_DIALOG *zip_dialog;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
#define THIS	( (CF_ACTION *) this )
{
	MENU *m;
	long i, prm, status = 0;
	DAC_PARAM *dp;
	ZIP_DIALOG *z;
	hw_zipcode hw_addr;
	hw_zipcode_t *hw;
	int  fmt_required_flag=FALSE;

	debug( "entered cf_action.Do, THIS = %x\n" );
	zip_dialog->reset( zip_dialog );
	hw_addr = (hw_zipcode) zip_dialog->prompt( zip_dialog );
		
	hw = (hw_zipcode_t *) &hw_addr;

	m = THIS->new_menu();
	for ( i = 0; THIS->newdp[i]; i++ )
		THIS->newdp[i]( m, hw_addr );
	dp = (DAC_PARAM *) TRUE;
	while ( (u_long) dp ) {
		dp = (DAC_PARAM *)m->prompt( m );
		if ( (u_long) dp ) {
			prm =  (long) dp->prompt( dp );
			if ( prm == -1 )
				dp->display( dp );
			else
			{
				status = dp->set( dp, prm ); 
				if (( status == 0) && (dp->fmt_required( dp )))
					fmt_required_flag=TRUE;
			}
			dp->reset( dp );
		}
	}
#ifndef PARAGON860 /* don't display this message */
	if (fmt_required_flag == TRUE)
	       printf("\nYou have entered a selection that requires a format!\n\n");
#endif
	m->destroy( m );
	debug( "leaving cf_action.Do\n" );
	return( (u_long) this );
}

void
destroy_CF_ACTION( this )
void *this;
{
	debug( "entered destroy_CF_ACTION\n" );
	destroy_ACTION( (ACTION *) this );
}

CF_ACTION *
new_CF_ACTION()
{
	ACTION *s;
	CF_ACTION *this;

	s = new_ACTION();
	this = (CF_ACTION *) zalloc( sizeof( CF_ACTION ) );
	memcpy( this, s, sizeof( ACTION ) ); /* inherit from ACTION */
	free( s );
	this->destroy = destroy_CF_ACTION;
	this->Do =  Do;
	debug( "leaving new_CF_ACTION\n" );
	return( this );
}
