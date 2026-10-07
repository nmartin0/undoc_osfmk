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
#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		main_menu.h
 Title:		MAIN_MENU Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:11 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the MAIN_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	MAIN_MENU_CLASS	MENU_CLASS					\
/* end MAIN_MENU_CLASS */

typedef struct main_menu {
	MAIN_MENU_CLASS
} MAIN_MENU;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
MAIN_MENU *
new_MAIN_MENU(); 

void
destroy_MAIN_MENU( void *this );

#endif
