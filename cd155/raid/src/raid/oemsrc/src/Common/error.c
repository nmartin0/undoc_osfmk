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
 Name:		error.c
 Title:		ERROR Interface Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:16 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the general error interface.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "stddefs.h"
#define IN_ERROR_C
#include "dau_err.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS		    ***
 ******************************************************************************/

 u_int Suppress_Messages;

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 ******************************************************************************/

extern int Argc;
extern char **Argv;
extern u_int Interactive;
extern void usage();
extern void exit();

/******************************************************************************
 ***				  PROCEDURE 				    ***
 *****************************************************************************/
void
error( errcode, msgcode )
int errcode;
int msgcode;
{
	char *progname;

	if ((progname = (char *) strrchr(Argv[0], '/')) != NULL)
		progname++;
	else
		progname = Argv[0];

	if (!Suppress_Messages) {
		fprintf(stderr, "\n[%s] ", progname);
		fprintf(stderr, error_table[msgcode]);
		fprintf(stderr, "\n\n");
	}
	switch (errcode) {
		case COMMAND_LINE_SYNTAX:
			if ( !Interactive )
			 	usage();
			break;

		case ARGUMENT_RANGE: 		break;	

		case DEVICE_OPEN: 		break;	

		case SCSI_CONDITION: 		break;	

		case NON_SPECIFIC_ERROR: 	break;	

		case NO_MEMORY: 		break;	

		default:			break;
	}
	if (!Interactive) exit (errcode);
	return;
}
