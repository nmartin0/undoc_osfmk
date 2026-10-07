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
#ifndef _DCF_MENU_H
#define _DCF_MENU_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dcf_menu.h
 Title:		DCF_MENU Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:26:31 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DCF_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DCF_MENU_CLASS	MENU_CLASS					\
/* end DCF_MENU_CLASS */

typedef struct dcf_menu {
	DCF_MENU_CLASS
} DCF_MENU;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DCF_MENU *
new_DCF_MENU(); 

void
destroy_DCF_MENU( void *this );

#endif
