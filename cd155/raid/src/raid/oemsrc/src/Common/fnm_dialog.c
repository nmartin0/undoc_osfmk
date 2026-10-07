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
 Name:		fnm_dialog.c
 Title:		FNM_DIALOG Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:19 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DIALOG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef MSDOS		/* (ifdef may not be the long-term solution) */
#include <io.h>
#endif
#include "stddefs.h"
#include "fnm_dialog.h"
#include "dau_err.h"

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern int errno;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
edit( this, str, str_valid )
void *this;
u_char *str;
int *str_valid;
#define THIS	( (FNM_DIALOG *) this )
{
	int fd, oflag;
	FILE *fp;

#ifdef MSDOS
	{
		char Buffer[16];
		fprintf(stdout,"Please insert the diskette containing %s.\n",
			(char *) str);
		fprintf(stdout,"Enter return when ready\n");
		fgets(Buffer, 16, stdin);
	}
#endif

	*str_valid = TRUE;

/* Begin OS/2 change - bas */
	if ( THIS->create_if_nonexistent )
	{
		oflag = ( THIS->create_if_nonexistent ) ? 
			( O_RDWR | O_CREAT ) : O_RDWR;
		if ( ( fd = open( (char *) str, oflag, 0644 ) ) != -1 )
		{
		 close( fd );
		 if ( ( fp = fopen( (char *) str, "w" ) ) != NULL ) 
			return( (u_long) fp );
                 else
		 {
			if ( errno == ENOENT )
			{
				error(FILE_OR_DEVICE_OPEN,
				GENERAL_FILE_NOT_FOUND_MSG);
			}
			else
			{
				error(FILE_OR_DEVICE_OPEN, 
				GENERAL_FILE_OPEN_ERROR_MSG);
			}
			*str_valid = FALSE;
			return( NULL );
		 }
		}
	}
	else
	{
		if ( ( fp = fopen( (char *) str, "r" ) ) != NULL ) {
#ifdef MSDOS
			fprintf(stdout,"Downloading firmware file %s.\n", 
				(char *) str);
#endif
			return( (u_long) fp );
		}	
                else 
		{
			if ( errno == ENOENT )
			{
				error(FILE_OR_DEVICE_OPEN, 
			 	GENERAL_FILE_NOT_FOUND_MSG);
			}
			else
			{
				error(FILE_OR_DEVICE_OPEN, 
				GENERAL_FILE_OPEN_ERROR_MSG);
			}
			*str_valid = FALSE;
			return( NULL );
		}
	}
/* End OS/2 change - bas */
#undef THIS
}

void
destroy_FNM_DIALOG( this )
void *this;
{
	debug( "entered destroy_FNM_DIALOG, this = %x\n", this );
	destroy_DIALOG( this );
	debug( "leaving destroy_FNM_DIALOG\n" );
}

FNM_DIALOG *
new_FNM_DIALOG( default_fp, create )
FILE *default_fp;
int create;
{
	DIALOG *s;
	FNM_DIALOG *this;

	debug( "entered new_FNM_DIALOG\n" );
	s = new_DIALOG();
	this = (FNM_DIALOG *) zalloc( sizeof( FNM_DIALOG ) );
	memcpy( this, s, sizeof( DIALOG ) ); /* inherit from DIALOG */
	free( s );
	strcpy( this->keyval, "f" );
	this->description = "filename";
	this->default_value = (u_long) default_fp;
	this->default_valid = TRUE;
	this->arg_needed = TRUE;
	this->create_if_nonexistent = create;
#ifdef PARAGON860 /* this is returning a file pointer */
	this->edit = edit;
#else
	this->edit = edit;
#endif
	debug( "leaving new_FNM_DIALOG\n" );
	return( this );
}
