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
 Name:		ccf_menu.c
 Title:		CCF_MENU Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:26:48 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the CCF_MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "stddefs.h"
#include "menu.h"
#include "ccf_menu.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_CCF_MENU( this )
void *this;
{
	debug( "entered destroy_CCF_MENU\n" );
	destroy_MENU( (MENU *) this );
	debug( "leaving destroy_CCF_MENU\n" );
}

CCF_MENU *
new_CCF_MENU()
{
	MENU *s;
	CCF_MENU *this;

	debug( "entered new_CCF_MENU\n" );
	s = new_MENU();
	this = (CCF_MENU *) zalloc( sizeof( CCF_MENU ) );
	memcpy( this, s, sizeof( MENU ) ); /* inherit from MENU */
	free( s );
	this->description = "Pre-Defined Configuration Selections";
	this->destroy = destroy_CCF_MENU;
	debug( "leaving new_CCF_MENU\n" );
	return( this );
}
