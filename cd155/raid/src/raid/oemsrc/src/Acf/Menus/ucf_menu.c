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
 Name:		ucf_menu.c
 Title:		UCF_MENU Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:26:59 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the UCF_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "stddefs.h"
#include "menu.h"
#include "ucf_menu.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_UCF_MENU( this )
void *this;
{
	debug( "entered destroy_UCF_MENU\n" );
	destroy_MENU( (MENU *) this );
	debug( "leaving destroy_UCF_MENU\n" );
}

UCF_MENU *
new_UCF_MENU()
{
	MENU *s;
	UCF_MENU *this;

	debug( "entered new_UCF_MENU\n" );
	s = new_MENU();
	this = (UCF_MENU *) zalloc( sizeof( UCF_MENU ) );
	memcpy( this, s, sizeof( MENU ) ); /* inherit from MENU */
	free( s );
	this->description = "Array Logical Unit Parameter Selections\n\n\t(Note: a change to any item marked with a '*' will destroy existing\n\tdata and make it necessary to reformat the logical unit; items\n\tmarked with a '-' are unchangeable.)";
	this->destroy = destroy_UCF_MENU;
	debug( "leaving new_UCF_MENU\n" );
	return( this );
}
