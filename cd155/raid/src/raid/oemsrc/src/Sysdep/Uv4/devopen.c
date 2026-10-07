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
 Name:		devopen.c
 Title:		Device Open
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:32 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	DEV_Open is provided to replace the calls to IO_open in
		the disk array utilities.  DEV_open first tries the normal
		form of the open, then, if that fails, tries the NDELAY
		form of the open - this should limit considerably the
		times when NDELAY will have to be used.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <fcntl.h>

/******************************************************************************
 ***				  INTERNAL PROCEDURES			    ***
 *****************************************************************************/

int
DEV_Open( Path, Flag, Mode )
char *Path;
int Flag, Mode;

{
	int fd;

	if ( ( fd = open( Path, Flag, Mode ) ) != -1 )
		return( fd );
	else
		return( IO_Open( Path, Flag, Mode ) );
}
