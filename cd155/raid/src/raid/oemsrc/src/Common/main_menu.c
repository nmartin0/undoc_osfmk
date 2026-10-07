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
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		main_menu.c
 Title:		MAIN_MENU Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:24 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the MAIN_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "stddefs.h"
#include "menu.h"
#include "main_menu.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_MAIN_MENU( this )
void *this;
#define THIS	( (MAIN_MENU *) this )
{
	destroy_MENU( (MENU *) this );
#undef THIS
}

MAIN_MENU *
new_MAIN_MENU()
{
	MENU *s;
	MAIN_MENU *this;

	debug( "entered new_MAIN_MENU\n" );
	s = new_MENU();
	this = (MAIN_MENU *) zalloc( sizeof( MAIN_MENU ) );
	memcpy( this, s, sizeof( MENU ) ); /* inherit from MENU */
	free( s );
	this->is_level_zero = TRUE;
	this->description = "Main Menu Selections";
	this->destroy = destroy_MAIN_MENU;
	debug( "leaving new_MAIN_MENU\n" );
	return( this );
}
