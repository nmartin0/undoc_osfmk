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
 Name:		action.c
 Title:		ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:52 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"
#include "action.h"
#include "menu.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_ACTION( this )
void *this;
{
	debug( "entered destroy_ACTION, this = %x\n", this );
	destroy_OBJECT( this );
	debug( "leaving destroy_ACTION\n" );
}

static u_long
Do( this )
void *this;
{
	return ( 0 );
}

ACTION *
new_ACTION()
{
	OBJECT *s;
	ACTION *this;

	debug( "entered new_ACTION\n" );
	s = new_OBJECT();
	this = (ACTION *) zalloc( sizeof( ACTION ) );
	memcpy( this, s, sizeof( OBJECT ) ); /* inherit from OBJECT */
	free( s );
	this->destroy = destroy_ACTION;
	this->Do = Do;
	debug( "leaving new_ACTION\n" );
	return( this );
}
