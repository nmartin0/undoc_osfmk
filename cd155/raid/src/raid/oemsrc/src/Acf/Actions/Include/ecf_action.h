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
 
#ifndef _ECF_ACTION_H
#define _ECF_ACTION_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		ecf_action.h
 Title:		ECF_ACTION Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:43 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the ECF_ACTION class.
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
#define	ECF_ACTION_CLASS	CF_ACTION_CLASS
/* end ECF_ACTION_CLASS */

typedef struct ecf_action {
	ECF_ACTION_CLASS
} ECF_ACTION;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
ECF_ACTION *
new_ECF_ACTION( MENU *menu );

void
destroy_ECF_ACTION( void *this );

#endif
