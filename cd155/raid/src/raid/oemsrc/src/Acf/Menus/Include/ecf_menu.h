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
#ifndef _ECF_MENU_H
#define _ECF_MENU_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		ecf_menu.h
 Title:		ECF_MENU Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:26:35 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the ECF_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	ECF_MENU_CLASS	MENU_CLASS					\
/* end ECF_MENU_CLASS */

typedef struct ecf_menu {
	ECF_MENU_CLASS
} ECF_MENU;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
ECF_MENU *
new_ECF_MENU(); 

void
destroy_ECF_MENU( void *this );

#endif
