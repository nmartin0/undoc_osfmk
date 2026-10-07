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
 Name:		dsw_param.c
 Title:		DSW_PARAM Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:13 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DSW_PARAM class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dsw_param.h"

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern void error();

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long 
edit( this, str, str_valid )
void *this;
u_char *str;
int *str_valid;
{
	debug( "entered dsw_param.edit, str = %s\n", str );
	*str_valid = TRUE;
	if ( strcmp( str, "d" ) == 0 )
		return( -1 );
	if ( strcmp( str, "on" ) == 0 )
		return( 1 );
	if ( strcmp( str, "off" ) == 0 )
		return( 0 );
	else {
		/* Invalid selection */
		error(ARGUMENT_RANGE, 1);
		*str_valid = FALSE;
		return( NULL );
	}
}

void
destroy_DSW_PARAM( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM *) this );
}

DSW_PARAM *
new_DSW_PARAM( hw_addr )
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long hw_addr;
#endif
{
	DAC_PARAM *s;
	DSW_PARAM *this;

	debug( "entered new_DSW_PARAM\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DSW_PARAM *) zalloc( sizeof( DSW_PARAM ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->destroy = destroy_DSW_PARAM;
	this->edit = edit;
	debug( "leaving new_DSW_PARAM\n" );
	return( this );
}
