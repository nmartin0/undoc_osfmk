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
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:32 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This function displays the usage information for acf.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static char *usage_descr =
"Usage:\n\tarc -z<hw-addr> [-P] [-s] [-F[d|freq]] [-a[d|amount]]\n";

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

void
usage()
{
	printf( "%s", usage_descr );
}
