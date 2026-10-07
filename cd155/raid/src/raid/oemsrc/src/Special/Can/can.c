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
 Name:		can.c
 Title:		Convert Array Name Program
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:48 $ 
 Programmer:	rmj
 Documents:	1.  UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2.  "Object-Oriented Programming in C", C User's Journal 7/90


 COPYRIGHT 1991, NCR Corp.

 Description:	The "can" program converts an NCR UNIX SVR4 device name into
		the "normal form" used by the disk array utilities.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "stddefs.h"
#include "dau_err.h"
#include "getrel.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
int Argc;
char **Argv;
int Interactive;
static char ident[] = PROJREL "/$Revision: 1.2.4.1 $ - $RCSfile: can.c,v $, Convert Array Name Program";

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

main( argc, argv )
int argc;
char *argv[];
{
	int fd, fd0;
	int length=0;
	u_long release_num;
	char *position;
	char wrkstr[256];
	char device[256];
	hw_zipcode hw_addr = 0;
	struct IO_DeviceParameters params, params0;
	hw_zipcode_t *hp = (hw_zipcode_t *) &hw_addr;

	Argc = argc;
	Argv = argv;
	Interactive = FALSE;
	if ( strlen( argv[1] ) == 0 )
		error( COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR );

	/* On UNIX V.4 1.03 & later LUN 0 must be opened before access to LUNs 
	   greater than 0 are permitted!!  Go thru gyrations to determine
	   which LUN were are converting.  If it is greater than LUN 0
	   open LUN 0, then proceed */

	release_num=get_release();
	if (release_num >= 103) {
		device[0]=NULL;
		strcpy(device,argv[1]);
		length = strlen(argv[1]);
		device[length]=NULL;
		/* find the d of the form /dev/rdsk/cntndnsn */
		position = (char *) strchr(&device[10],'d');
		if (position != NULL) { /* if no d entry this will eventually
					   print a usage message */
			position++;
			/*ck for trying to convert a LUN > 0*/
			if (*position != 0x30) { 
				*position = 0x30; /* yes !! */
				if ( ( fd0 = IO_Open( device, O_RDONLY ) ) < 0 )
			  	error(COMMAND_LINE_SYNTAX,
				      GENERAL_DEV_OPEN_ERROR_MSG);
				if (IO_GetDeviceParameters(fd0, &params0) < 0)
			  	error( NON_SPECIFIC_ERROR, IOCTL_FAILURE_MSG );
			}
		}
	}
	if ( ( fd = IO_Open( argv[1], O_RDONLY ) ) < 0 )
		error( COMMAND_LINE_SYNTAX, GENERAL_DEV_OPEN_ERROR_MSG );
	if ( IO_GetDeviceParameters( fd, &params ) < 0 )
		error( NON_SPECIFIC_ERROR, IOCTL_FAILURE_MSG );
	hp->IO_Bus = params.dp_IO_BusNumber;
	hp->SCSI_Controller = params.dp_IO_BusSlot;
	hp->SCSI_Bus = params.dp_SCSI_BusNumber;
	hp->PUN = params.dp_PUN;
	hp->LUN = params.dp_LUN;
	printf( "%s\n", convert_zip( hw_addr, wrkstr ) );
	exit( 0 );
}
