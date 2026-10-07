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
 Name:		dcf_menu.c
 Title:		DCF_MENU Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:26:52 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DCF_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "stddefs.h"
#include "menu.h"
#include "dcf_menu.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_DCF_MENU( this )
void *this;
{
	debug( "entered destroy_DCF_MENU\n" );
	destroy_MENU( (MENU *) this );
	debug( "leaving destroy_DCF_MENU\n" );
}

DCF_MENU *
new_DCF_MENU()
{
	MENU *s;
	DCF_MENU *this;

	debug( "entered new_DCF_MENU\n" );
	s = new_MENU();
	this = (DCF_MENU *) zalloc( sizeof( DCF_MENU ) );
	memcpy( this, s, sizeof( MENU ) ); /* inherit from MENU */
	free( s );
	this->description = "Array Drive Parameter Selections";
	this->destroy = destroy_DCF_MENU;
	debug( "leaving new_DCF_MENU\n" );
	return( this );
}
