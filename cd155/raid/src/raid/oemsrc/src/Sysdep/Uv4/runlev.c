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
 Name:		runlev.c
 Title:		System Run Level Query Function
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:47 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This function will return the system run level.
*/

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

int runlevel()	/* Get the system run level */
{
	int runlev;

	runlev =  system( "/bin/sh  <<EOF\nexit `who -r | awk '{if ($3 == \"S\") print \"32767\"; else print $3}'`\nEOF") >> 8;
	return( runlev );
}
