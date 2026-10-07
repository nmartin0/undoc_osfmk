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
 Name:		object.c
 Title:		OBJECT Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:33 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities, FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the OBJECT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "stddefs.h"
#include "object.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static void
describe( this )
void *this;
#define THIS	( (OBJECT *) this )
{
	fprintf( THIS->describe_fp, "%s", THIS->description );
}
#undef THIS

static void
redirect( this, fp )
void *this;
FILE *fp;
{
	((OBJECT *) this)->describe_fp = fp;
}

static u_char *
key( this )
void *this;
{
	return( ( (OBJECT *) this)->keyval );
}

static u_int
needs_arg( this )
void *this;
#define THIS	( (OBJECT *) this )
{
	debug(" entered needs_arg, arg_needed = %d\n", THIS->arg_needed );
	return( THIS->arg_needed );
#undef THIS
}

static u_int
is_a_nop( this )
void *this;
#define THIS	( (OBJECT *) this )
{
	debug( "returning nop attribute = %d\n", THIS->is_a_nop_obj );
	return( THIS->is_a_nop_obj );
#undef THIS
}

static hw_zipcode
hw_address( this )
void *this;
{
	return( 0xffffffff );
}

void
destroy_OBJECT( this )
void *this;
{
	debug( "entered destroy_OBJECT, this = %x\n", this );
	free( this );
	debug( "leaving destroy_OBJECT\n" );
}

OBJECT *
new_OBJECT()
{
	OBJECT *this;

	this = (OBJECT *) zalloc( sizeof( OBJECT ) );
	strcpy( this->keyval, "" );
	this->description = "Generic Object";
	this->key = key;
	this->describe_fp = stdout;
	this->describe = describe;
	this->redirect = redirect;
	this->needs_arg = needs_arg;
	this->is_a_nop = is_a_nop;
	this->hw_address = hw_address;
	this->destroy = destroy_OBJECT;
	return( this );
}
