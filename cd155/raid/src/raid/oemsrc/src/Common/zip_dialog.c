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
 Name:		zip_dialog.c
 Title:		ZIP_DIALOG Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:31:04 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DIALOG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "zipcod_dlg.h"
#include "zip_dialog.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_ZIP_DIALOG( this )
void *this;
{
	debug( "entered destroy_ZIP_DIALOG, this = %x\n", this );
	destroy_ZIPCOD_DLG( (ZIPCOD_DLG *) this );
	debug( "leaving destroy_ZIP_DIALOG\n" );
}

ZIP_DIALOG *
new_ZIP_DIALOG()
{
	ZIPCOD_DLG *s;
	ZIP_DIALOG *this;

	debug( "entered new_ZIP_DIALOG\n" );
	s = new_ZIPCOD_DLG( 0xffffffff );
	this = (ZIP_DIALOG *) zalloc( sizeof( ZIP_DIALOG ) );
	memcpy( this, s, sizeof( ZIPCOD_DLG ) ); /* inherit from ZIPCOD_DLG */
	free( s );
	this->destroy = destroy_ZIP_DIALOG;
	debug( "leaving new_ZIP_DIALOG\n" );
	return( this );
}
