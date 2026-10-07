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
 Name:		dialog.c
 Title:		DIALOG Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:10 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DIALOG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "dialog.h"
#include "dau_err.h"

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern int Argc;
extern char **Argv;
extern u_int Interactive;
extern u_char *optarg;
extern int getopt();

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
static hw_zipcode
#else
static u_long 
#endif
interactive_prompt( this )
void *this;
#define THIS	( (DIALOG *) this )
{
	int ok;
	char *s;

	debug( "entered prompt (interactive)\n" );
	if ( ! THIS->prompted ) {
		ok = FALSE;
		while ( ! ok ) {
			if ( THIS->prompt_str == NULL ) {
				printf( "Please enter a value for " );
				THIS->describe( THIS );
				printf( ": " );
			} else
				printf( "%s", THIS->prompt_str );
			fflush( stdin );
#ifdef PARAGON860 /* cast to make the compiler happy */
			gets( (char *)THIS->reply_str );
#else
			gets( THIS->reply_str );
#endif
			if ( strlen( THIS->reply_str ) == 0 ) {
				if ( THIS->default_valid ) {
					THIS->value = THIS->default_value;
					ok = TRUE;
				}
			} else
				THIS->value = THIS->edit( THIS, (u_char *) THIS->reply_str, &ok );
		}
		THIS->prompted = TRUE;
	}
	debug( "leaving prompt (interactive)\n" );
	return( THIS->value );
#undef THIS
}

#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
static hw_zipcode
#else
static u_long
#endif
commandline_prompt( this )
void *this;
#define THIS	( (DIALOG *) this )
{
	int c, ok = FALSE;
	u_char opts[4];
	hw_zipcode_t *hw;

	if ( ! THIS->prompted ) {
		strcpy( opts, THIS->key( THIS ) );
		strcat( opts, ":" );
		debug( "opts = %s\n", opts );
		if ( optarg || ((c = getopt( Argc, Argv, opts)) == opts[0]) ) {
			THIS->value = THIS->edit( THIS, optarg, &ok ); 
			optarg = NULL;
			if ( !ok )
				error( COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR );
		} else  {
			if ( c == EOF && THIS->default_valid )
				THIS->value = THIS->default_value;
			else
				error( COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR );
		}
		THIS->prompted = TRUE;
	}	
	return( THIS->value );
#undef THIS
}

static u_long
edit( this, str, str_valid )
void *this;
u_char *str;
int *str_valid;
{
	*str_valid = TRUE;
	return( (u_long) str );
}

static void
reset( this )
void *this;
{
	( (DIALOG *) this)->prompted = FALSE;
}

void
destroy_DIALOG( this )
void *this;
{
	destroy_OBJECT( (OBJECT *) this );
}

DIALOG *
new_DIALOG()
{
	OBJECT *s;
	DIALOG *this;

	debug( "entered new_DIALOG\n" );
	s = new_OBJECT();
	this = (DIALOG *) zalloc( sizeof( DIALOG ) );
	memcpy( this, s, sizeof( OBJECT ) ); /* inherit from OBJECT */
	free( (char *)s );
	this->description = "dialog object";
	this->destroy = destroy_DIALOG;
	this->prompt = Interactive ? interactive_prompt : commandline_prompt;
	this->edit = edit;
	this->reset = reset;
	debug( "leaving new_DIALOG\n" );
	return( this );
}
