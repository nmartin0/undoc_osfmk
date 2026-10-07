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
 Name:		dld_action.c
 Title:		DLD_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:40 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DLD_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dld_action.h"
#include "zip_dialog.h"
#include "fnm_dialog.h"
#include "dac_drvsts.h"
#include "dau_err.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define DOWNLOAD_TIME_LIMIT	60
extern u_int Interactive;

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern u_long ACF_DOWNLOAD();
extern u_int  Suppress_Messages;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
#define THIS ((DLD_ACTION *) this)
{
        FILE *fp;
	DAC_DRVSTS *dp;
	hw_zipcode hw_addr;
        FNM_DIALOG *fnm_dialog;
	extern ZIP_DIALOG *zip_dialog;
        int i, s, map_flag, create;
		u_long drive_stat; 
	int status=0;
	extern sleep(long);

	zip_dialog->reset( (DIALOG *) zip_dialog );
	hw_addr = (hw_zipcode) zip_dialog->prompt( (DIALOG *) zip_dialog );
        fnm_dialog=new_FNM_DIALOG( stdin, create = FALSE );
        fp= (FILE *) fnm_dialog->prompt ( ( DIALOG *) fnm_dialog);
	status = ACF_DOWNLOAD (hw_addr, fp, map_flag = FALSE);
	if (status == 0) {
		printf( "\nDownload completed - waiting for controller to become ready\n" );
		/*
	 	* The theory behind the following code is that the read of the
	 	* DAC drive status will not succeed until the downloaded code
	 	* assumes control.
	 	*/ 
		sleep (20); /* wait for DAC to become ready */
		dp = new_DAC_DRVSTS( NULL, hw_addr );
		Suppress_Messages = TRUE;
		for ( i = 0; i < DOWNLOAD_TIME_LIMIT; i++ ) {
			sleep( 1 );
			if ((s=dp->read( dp,(u_long *)&drive_stat))==0){
				break;
			}
		}
		Suppress_Messages = FALSE;
		dp->destroy ( dp );
		if ( s )
			error( SCSI_CONDITION, DOWNLOAD_TIMEOUT_MSG );
		else
			printf( "Loaded firmware now in service\n\n" );
	}
	fclose(fp);
        destroy_FNM_DIALOG( fnm_dialog );
	return( (u_long) THIS );
}
#undef THIS

void
destroy_DLD_ACTION( this )
void *this;
{
	destroy_ACTION( (ACTION *) this );
}

DLD_ACTION *
new_DLD_ACTION( menu )
MENU *menu;
{
	ACTION *s;
	DLD_ACTION *this;

	s = new_ACTION();
	this = (DLD_ACTION *) zalloc( sizeof( DLD_ACTION ) );
	memcpy( this, s, sizeof( ACTION ) ); /* inherit from ACTION */
	destroy_ACTION( s );
	strcpy( this->keyval, "l" );
	this->arg_needed = FALSE;
	this->description = "Download Array Controller Firmware";
	this->destroy = destroy_DLD_ACTION;
	this->Do = Do;
	menu->actions->add( menu->actions, this );
	debug( "leaving new_DLD_ACTION\n" );
	return( this );
}
