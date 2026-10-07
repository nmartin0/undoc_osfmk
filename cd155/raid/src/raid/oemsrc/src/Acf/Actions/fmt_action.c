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
 Name:		fmt_action.c
 Title:		FMT_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:43 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the FMT_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "fmt_action.h"
#include "scsi_dk.h"
#include "zip_dialog.h"
/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
#define THIS ((FMT_ACTION *) this)
{
	hw_zipcode hw_addr;
	extern ZIP_DIALOG *zip_dialog;
	char ch, str_chr[80];
	u_char *data;
	u_int byte_count, fmtdata, cmplist, listfmt, interleave, status=0;
	SCSI_DK *sdk;

	zip_dialog->reset( (DIALOG *) zip_dialog );
	hw_addr = (hw_zipcode) zip_dialog->prompt( (DIALOG *) zip_dialog );
	sdk=new_SCSI_DK ( hw_addr, TRUE );
	status = sdk->lock( sdk );
	if ( status != 0 ) {
		error( DEVICE_OPEN, SCSI_LOCK_FAIL );
		destroy_SCSI_DK( sdk );
		return( status );
	}
#ifndef PARAGON860 /* don't display this prompt */
	printf("Proceeding with format can destroy data!\n");
	printf("Do you wish to proceed (y or n)? ");
	fflush(stdin);
	gets( str_chr );
	ch = str_chr[0];
	printf("\n");
	if (ch != 'y') return (status);
#endif
			/* otherwise do the format! */
	status=sdk->format_unit( sdk, data=NULL, byte_count=0, 
		fmtdata=0, cmplist=0, listfmt=0, interleave=0 );
	if (status!=0) error( SCSI_CONDITION, FORMAT_ERROR );
	destroy_SCSI_DK( sdk );

	return(status);

}
#undef THIS

void
destroy_FMT_ACTION( this )
void *this;
{
	destroy_ACTION( (ACTION *) this );
}

FMT_ACTION *
new_FMT_ACTION( menu )
MENU *menu;
{
	ACTION *s;
	FMT_ACTION *this;

	s = new_ACTION();
	this = (FMT_ACTION *) zalloc( sizeof( FMT_ACTION ) );
	memcpy( this, s, sizeof( ACTION ) ); /* inherit from ACTION */
	destroy_ACTION( s );
	strcpy( this->keyval, "f" );
	this->arg_needed = FALSE;
	this->description = "Format A Logical Unit Of A Disk Array";
	this->destroy = destroy_FMT_ACTION;
	this->Do = Do;
	menu->actions->add( menu->actions, this );
	debug( "leaving new_FMT_ACTION\n" );
	return( this );
}
