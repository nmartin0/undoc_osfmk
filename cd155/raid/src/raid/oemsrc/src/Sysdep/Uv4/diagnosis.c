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
 Name:		diagnosis.c
 Title:		Error Diagnosis Display
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:35 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This function will display a system-dependent error
		diagnositc to standard error.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <errno.h>

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

void
diagnosis()
{
	if ( errno ) {
		perror( "\n      Diagnosis" );
		errno = 0;
	}
}
