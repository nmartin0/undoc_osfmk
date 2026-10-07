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
#ifndef _ZIP_LIST_H
#define _ZIP_LIST_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		zip_list.h
 Title:		ZIP_LIST Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:44 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the ZIP_LIST class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "list_dlg.h"
#include "zipcod_dlg.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	ZIP_LIST_CLASS	LIST_DLG_CLASS					\
	ZIPCOD_DLG *zcd;
/* end ZIP_LIST_CLASS */

typedef struct zip_list {
	ZIP_LIST_CLASS
} ZIP_LIST;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
ZIP_LIST *
new_ZIP_LIST( hw_zipcode mask );

void
destroy_ZIP_LIST( void *this );

#endif
