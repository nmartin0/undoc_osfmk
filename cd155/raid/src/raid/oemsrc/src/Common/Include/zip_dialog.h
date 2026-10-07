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
#ifndef _ZIP_DIALOG_H
#define _ZIP_DIALOG_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		zip_dialog.h
 Title:		ZIP_DIALOG Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:41 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the ZIP_DIALOG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dialog.h"
#include "zipcod_dlg.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	ZIP_DIALOG_CLASS	ZIPCOD_DLG_CLASS
/* end ZIP_DIALOG_CLASS */

typedef struct zip_dialog {
	ZIP_DIALOG_CLASS
} ZIP_DIALOG;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
ZIP_DIALOG *
new_ZIP_DIALOG();

void
destroy_ZIP_DIALOG( void *this );

#endif
