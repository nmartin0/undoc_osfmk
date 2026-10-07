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
 Name:		list_dlg.c
 Title:		LIST_DLG Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:22 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities, FS no. 348-0027726
		2. "Object-Oriented Programming in  C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the LIST_DLG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "list_dlg.h"
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
#ifdef PARAGON860
static hw_zipcode 
#else
static u_long 
#endif
interactive_prompt( this )
void *this;
#define THIS	( (LIST_DLG *) this )
{
	int ok;

	debug( "entered list_dlg.prompt (interactive)\n" );
	ok = FALSE;
	while ( ! ok ) {
		if ( THIS->prompt_str == NULL ) {
			printf( "Please enter a value for " );
			THIS->describe( THIS );
			printf( ": " );
		} else
			printf( "%s", THIS->prompt_str );
		fflush( stdin );
		gets( (char *) THIS->reply_str );
		if ( strlen( THIS->reply_str ) == 0 ) {
			THIS->null_reply = TRUE;
			return( NULL );
		}
		THIS->value = THIS->edit( THIS, THIS->reply_str, &ok );
	}
	debug( "leaving list_dlg.prompt (interactive)\n" );
	return( THIS->value );
#undef THIS
}

#ifdef PARAGON860
static hw_zipcode 
#else
static u_long 
#endif
commandline_prompt( this )
void *this;
#define THIS	( (LIST_DLG *) this )
{
	u_char *s;
	int ok = FALSE;

	if ( THIS->running_ptr == NULL ) {
		debug( "in list_dlg.cmdln_prompt, str = NULL\n");
		THIS->null_reply = TRUE;
		return( NULL );
	}
	debug( "in list_dlg.cmdln_prompt, str = %s\n", THIS->running_ptr );
	THIS->value = THIS->edit( THIS, THIS->running_ptr, &ok );
	if ( !ok ) {
		error( COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR );
	} else {
		if ( s = (u_char *) strchr( THIS->running_ptr, ',' ) )
			THIS->running_ptr = ++s;
		else
			THIS->running_ptr = NULL;
		return( THIS->value );
	}
#undef THIS
}

static u_long
list_complete( this )
void *this;
{
	return( ( (LIST_DLG *) this)->null_reply );
}

void
destroy_LIST_DLG( this )
void *this;
{
	destroy_DIALOG( (DIALOG *) this );
}

LIST_DLG *
new_LIST_DLG()
{
	DIALOG *s;
	LIST_DLG *this;

	debug( "entered new_LIST_DLG\n" );
	s = new_DIALOG();
	this = (LIST_DLG *) zalloc( sizeof( LIST_DLG ) );
	memcpy( this, s, sizeof( DIALOG ) ); /* inherit from DIALOG */
	free( s );
	this->description = "list_dlg object";
	this->running_ptr = Interactive ? NULL : optarg;
	this->destroy = destroy_LIST_DLG;
	this->prompt = Interactive ? interactive_prompt : commandline_prompt;
	this->list_complete = list_complete;
	debug( "leaving new_LIST_DLG\n" );
	return( this );
}
