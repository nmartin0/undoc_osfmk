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
 Name:		object_set.c
 Title:		OBJECT_SET Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:35 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the OBJECT_SET class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"
#include "object_set.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static OBJECT*
add( this, object )
void *this;
void *object;
#define THIS ( (OBJECT_SET *) this )
{
	debug( "adding %x to object_set %x\n", object, THIS );
	(THIS->contents)[THIS->next_available] = object;
	THIS->next_available = (THIS->next_available + 1) % THIS->maxelements;
	if ( THIS->next_available == THIS->first_element )
		THIS->first_element = (THIS->first_element + 1) % THIS->maxelements;
	if ( THIS->nelements < THIS->maxelements )
		THIS->nelements++;
	debug( "leaving add\n" );
	return( object );
}
#undef THIS

static void
top( this )
void *this;
{
	( (OBJECT_SET *) this)->current_position = 0;
}

static void *
next( this )
void *this;
#define THIS ( (OBJECT_SET *) this )
{
	int n;

	if ( THIS->at_end( THIS ) )
		return( NULL );
	else {
		n = (THIS->first_element + THIS->current_position) % THIS->maxelements;
		THIS->current_position++;
		return( (THIS->contents)[n] );
	}
}
#undef THIS

static void *
find( this, key )
void *this;
u_char *key;
#define THIS ( (OBJECT_SET *) this )
{
	int i, n;
	OBJECT *p;

	for ( i=0; i < THIS->count( THIS ); i++ ) {
		n = (THIS->first_element + i) % THIS->maxelements;
		p = THIS->contents[n];
		if ( strcmp( key, p->key( p ) ) == 0 )
			return( p );
	}
	return( NULL );
}
#undef THIS

static void *
select( this, which )
void *this;
u_int which;
#define THIS ( (OBJECT_SET *) this )
{
	int n;
	void *selection = NULL;

	if ( which >= 0 && which <= THIS->count( THIS ) ) {
		n = (THIS->first_element + which) % THIS->maxelements;
		selection = (THIS->contents)[n];
	}
	debug( "leaving select, returning %x\n", selection );
	return( selection );
}
#undef THIS

static int
at_end( this )
void *this;
#define THIS ( (OBJECT_SET *) this )
{
	return( THIS->current_position >= THIS->count( THIS ) );
}
#undef THIS

static u_int
tell( this )
void *this;
#define THIS ( (OBJECT_SET *) this )
{
	return( THIS->at_end( THIS ) ? -1 : THIS->current_position );
}
#undef THIS

static u_int
count( this )
void *this;
{
	return( ( (OBJECT_SET *) this)->nelements );
}

void
destroy_OBJECT_SET( this )
void *this;
#define THIS	( (OBJECT_SET *) this )
{
	OBJECT *o;

	debug( "entered destroy_OBJECT_SET\n" );
	THIS->top( THIS );
	while ( ! THIS->at_end( THIS ) ) {
		o = (OBJECT *) THIS->next( THIS );
		o->destroy( o );		
	}
	free( THIS->contents );
	destroy_OBJECT( (OBJECT *) this );
#undef THIS
}

OBJECT_SET *
new_OBJECT_SET( maxelements )
u_int maxelements;
{
	OBJECT *s;
	OBJECT_SET *this;

	debug( "entered new_OBJECT_SET\n" );
	s = new_OBJECT();
	this = (OBJECT_SET *) zalloc( sizeof( OBJECT_SET ) );
	memcpy( this, s, sizeof( OBJECT ) ); /* inherit from OBJECT */
	free( s );
	this->contents = (void *) zalloc( (maxelements * sizeof( void * )) );
	this->current_position = 0;
	this->maxelements = maxelements;
	this->destroy = destroy_OBJECT_SET;
	this->add = add;
	this->top = top;
	this->next = next;
	this->find = find;
	this->select = select;
	this->at_end = at_end;
	this->tell = tell;
	this->count = count;
	debug( "leaving new_OBJECT_SET\n" );
	return( this );
}
