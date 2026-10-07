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
 Name:		menu.c
 Title:		MENU Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:27 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include "stddefs.h"
#include "object.h"
#include "menu.h"
#include "action.h"
#include "object_set.h"
#include "dau_err.h"

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern int Argc;
extern char **Argv;
extern u_int Interactive;
extern int optind;
extern char *optarg;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
void
destroy_MENU( this )
void *this;
#define THIS	( (MENU *) this )
{
	debug( "entered destroy_MENU, this = %x\n", this );
	THIS->actions->destroy( THIS->actions );
	destroy_OBJECT( (OBJECT *) this );
#undef THIS
}

static u_long
interactive_prompt( this )
void *this;
#define THIS	( (MENU *) this )
{
	ACTION *a;
	u_int i, s;
	u_char str[80];

	debug( "entered menu.prompt, THIS = %x\n", THIS );
	while ( TRUE ) {
		THIS->describe( THIS );
		printf("\n\n");
		THIS->actions->top( THIS->actions );
		for ( i = 1; ! ( THIS->actions->at_end(THIS->actions) ); i++ ) {
			printf("  %d. ", i);
			if ( i < 10 )
				printf( " " );
			a = THIS->actions->next( THIS->actions );
			a->describe( a );
			printf("\n");
		}
		if ( THIS->is_level_zero )
			printf( "\nEnter selection or 'q' to exit program: " );
		else
			printf( "\nEnter selection or 'q' to return to previous level: " );
		fflush( stdin );
#ifdef PARAGON860 /* cast to make the compiler happy */
		gets( (char *)str );
#else
		gets( str );
#endif
		if ( tolower( str[0] ) == 'q' )
			return( NULL );
#ifdef PARAGON860 /* cast to make the compiler happy */
		for ( i = 0; i < strlen( (char *)str ) && isdigit( str[i] ); i++ )
#else
		for ( i = 0; i < strlen( str ) && isdigit( str[i] ); i++ )
#endif
			;
#ifdef PARAGON860 /* cast to make the compiler happy */
		if ( i < strlen( (char *)str ) ) /* premature termination */
#else
		if ( i < strlen( str ) ) /* premature termination */
#endif
			printf( "\nInvalid selection\n\n" );
		else {
			s = atoi( (char *) str ) - 1;
			if ( (a = THIS->actions->select( THIS->actions, s )) == NULL )
				printf( "\nInvalid selection\n\n" );
			else {
				debug( "returned from object_set.select\n" );
				return( (u_long) a );
			}
		}
	}
#undef THIS
}

static u_long
commandline_prompt( this )
void *this;
#define THIS	( (MENU *) this )
{
	int c;
	ACTION *a, *o;
	OBJECT_SET *acts;
	u_char sw[8], opts[64];

	debug( "entered menu.prompt (command line version)\n" );
	a = NULL;
	acts = THIS->actions;
#ifdef PARAGON860 /* cast to make the compiler happy */
	strcpy( (char *)opts, "" ); 
#else
	strcpy( opts, "" ); 
#endif
	acts->top( acts );
	while ( !acts->at_end( acts ) ) {
		o = acts->next( acts );
#ifdef PARAGON860 /* cast to make the compiler happy */
		strcat( (char *)opts, (char *)o->key( o ) );
#else
		strcat( opts, o->key( o ) );
#endif
		if ( o->needs_arg( o ) )
#ifdef PARAGON860 /* cast to make the compiler happy */
			strcat( (char *) opts, ":" );
#else
			strcat( opts, ":" );
#endif
	}
	debug( "option str: %s\n", opts );
	if ( ( c = getopt( Argc, Argv, (char *) opts ) ) != EOF ) {
		debug(" back from getopt, opt = %c %d\n", c, c ); 
#ifdef PARAGON860 /* cast to make the compiler happy */
		strcpy( (char *) sw, " " );
#else
		strcpy( sw, " " );
#endif
		sw[0] = c;
		debug( "trying to find key: %s\n", sw );
		a = acts->find( acts, sw );
	}
	debug(" back from getopt, opt = %c %d\n", c, c ); 
	if ( a ) {
		debug( "found action = %x\n", a );
		THIS->opts_found++;
		return( (u_long) a );
	}
	else {
		debug( "couldn't find action\n" );
#ifdef PARAGON860
		debug("optind = %d Argc = %d\n",optind,Argc);
		if ( c == '?' || optind < Argc)
#else
		if ( c == '?' )
#endif
			/* saw something we didn't like before end of line */
			error( COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR );
		else
			/* all arguments processed */
			return( NULL );
	}
	debug( "leaving menu.prompt (command line version)\n" );
#undef THIS
}

static u_int
add_selection( this, object )
void *this;
void *object;
#define THIS	( (MENU *) this )
{
	THIS->actions->add( THIS->actions, object );
	return ( 0 );
}
#undef THIS

MENU *
new_MENU()
{
	OBJECT *s;
	MENU *this;

	debug( "entered new_MENU\n" );
	s = new_OBJECT();
	this = (MENU *) zalloc( sizeof( MENU ) );
	memcpy( this, s, sizeof( OBJECT ) ); /* inherit from OBJECT */
	free( s );
	this->actions = new_OBJECT_SET( 32 );
	this->destroy = destroy_MENU;
	this->prompt = Interactive ? interactive_prompt : commandline_prompt;
	this->add_selection = add_selection;
	debug( "leaving new_MENU, this = %x\n", this );
	return( this );
}
