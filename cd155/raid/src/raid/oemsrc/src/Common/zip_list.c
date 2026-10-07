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
 Name:		zip_list.c
 Title:		ZIP_LIST Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:31:07 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the ZIP_LIST class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "zip_list.h"

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
edit( this, str, str_valid )
void *this;
u_char *str;
int *str_valid;
#define THIS ((ZIP_LIST *) this)
{
	char work_str[16];

	if ( str[0] == 'd' ) {
		*str_valid = TRUE;
		return( -1 );
	} else {
		memset( work_str, 0, sizeof( work_str ) );
		work_str[0] = ':'; /* pre-pend delimeter */
		strncpy( &work_str[1], str, strcspn( str, "," ) );
		return( THIS->zcd->edit( THIS->zcd, work_str, str_valid ) );
	}
}
#undef THIS

void
destroy_ZIP_LIST( this )
void *this;
#define THIS ((ZIP_LIST *) this)
{
	THIS->zcd->destroy( THIS->zcd );
	destroy_LIST_DLG( (LIST_DLG *) this );
}
#undef THIS

ZIP_LIST *
new_ZIP_LIST( mask )
hw_zipcode mask;
{
	LIST_DLG *s;
	ZIP_LIST *this;

	debug( "entered new_ZIP_LIST\n" );
	s = new_LIST_DLG();
	this = (ZIP_LIST *) zalloc( sizeof( ZIP_LIST ) );
	memcpy( this, s, sizeof( LIST_DLG ) ); /* inherit from LIST_DLG */
	free( s );
	this->description = "zip_list object";
	this->prompt_str = "    Enter drive specification (or 'd') (ENTER will terminate): ";
	this->zcd = new_ZIPCOD_DLG( mask );
	this->edit = edit;
	this->destroy = destroy_ZIP_LIST;
	debug( "leaving new_ZIP_LIST\n" );
	return( this );
}
