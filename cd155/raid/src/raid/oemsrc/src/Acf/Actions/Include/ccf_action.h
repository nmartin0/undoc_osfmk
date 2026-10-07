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
 
#ifndef _CCF_ACTION_H
#define _CCF_ACTION_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		ccf_action.h
 Title:		CCF_ACTION Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:31 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the CCF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "action.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	CCF_ACTION_CLASS	ACTION_CLASS
/* end CCF_ACTION_CLASS */

typedef struct ccf_action {
	CCF_ACTION_CLASS
} CCF_ACTION;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
CCF_ACTION *
new_CCF_ACTION( MENU *menu );

void
destroy_CCF_ACTION( void *this );

#endif
