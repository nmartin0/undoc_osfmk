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
 Name:		usage.c
 Title:		Usage Display
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:51 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This function displays the usage information for can.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "stddefs.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static u_char usage_descr[] =
#ifdef PARAGON860 /* we don't have rdsk sub-directories */
"Usage:\n\tcan /dev/...\n";
#else
"Usage:\n\tcan /dev/rdsk/...\n";
#endif
/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

void
usage()
{
	fprintf(stderr, "%s", usage_descr );
}
