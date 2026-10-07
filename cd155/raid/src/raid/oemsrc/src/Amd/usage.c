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
 Revision:	$Revision: 1.3.4.1 $
 Update Date:	$Date: 1995/06/11 23:27:27 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This function displays the usage information for amd.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static char *usage_descr =
#ifdef PARAGON860
"Usage:\n\tamd -z<hw_addr> -n<rf-window> -i<pl-interval> -r<rf-cycle> [-s]\n\tamd -z<hw_addr> {-d|-k{message-number-list|all}}\n";
#else
"Usage:\n\tamd -z<hw-addr> -n<rf-window> -i<pl-interval> -r<rf-cycle> [-s]\n\t[-f<file>]\n\tamd -z<hw_addr> {-d|-k{message-number-list|all}}\n";
#endif

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

void
usage()
{
	printf( "%s", usage_descr );
}
