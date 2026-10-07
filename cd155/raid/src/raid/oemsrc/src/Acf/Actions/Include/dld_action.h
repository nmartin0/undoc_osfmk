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
 
#ifndef _DLD_ACTION_H
#define _DLD_ACTION_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dld_action.h
 Title:		DLD_ACTION Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:41 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DLD_ACTION class.
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
#define	DLD_ACTION_CLASS	ACTION_CLASS				\
	/* extra stuff unique to dld_action - TBD */
/* end DLD_ACTION_CLASS */

typedef struct dld_action {
	DLD_ACTION_CLASS
} DLD_ACTION;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DLD_ACTION *
new_DLD_ACTION( MENU *menu );

void
destroy_DLD_ACTION( void *this );

#endif
