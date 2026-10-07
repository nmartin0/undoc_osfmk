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
#ifndef _CCF_MENU_H
#define _CCF_MENU_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		ccf_menu.h
 Title:		CCF_MENU Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:26:27 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the CCF_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	CCF_MENU_CLASS	MENU_CLASS					\
/* end CCF_MENU_CLASS */

typedef struct ccf_menu {
	CCF_MENU_CLASS
} CCF_MENU;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
CCF_MENU *
new_CCF_MENU(); 

void
destroy_CCF_MENU( void *this );

#endif
