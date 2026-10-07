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
 
#ifndef _UCF_ACTION_H
#define _UCF_ACTION_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		ucf_action.h
 Title:		UCF_ACTION Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:55 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the UCF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "cf_action.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	UCF_ACTION_CLASS	CF_ACTION_CLASS
/* end UCF_ACTION_CLASS */

typedef struct ucf_action {
	UCF_ACTION_CLASS
} UCF_ACTION;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
UCF_ACTION *
new_UCF_ACTION( MENU *menu );

void
destroy_UCF_ACTION( void *this );

#endif
