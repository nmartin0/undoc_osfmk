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
 Name:		dlerror.c
 Title:		Download error routine
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:14 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the code of the error handling for  
		downloading the disk array controller firmware.
*/

/*****************************************************************************
Possible Errors:
	Number	=== Description ===
dl_main_err+0	OK, no error
	    1	Channel option already selected
	    2	Open error for MAP file
	    3	Open error for download file
	    4	

dl_file_err+0	Accessing (reading) file
	    1	Invalid HEX file
	    2	Invalid record format
	    3	Invalid checksum
	    4	Invalid hex digit
	    5	Invalid file status
	    6	File corrupted

dl_xfer_err+0	Opening IPC device driver
	    1	IOctl error
	    2	CPF channel error
	    3	Timeout trying to access
	    4	Timeout waiting for board ready
	    5	No board exists at channel
	    6	Failed start-up
	    7
************************************************************************** */

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include "dlinclds.h"
#include <setjmp.h>

/******************************************************************************
 ****				VARIABLE DEFINITIONS		       	   ****
 *****************************************************************************/
char	*dlm_msg[] =
{
/* 0 */	"No error",
/* 1 */	"Channel option already selected",
/* 2 */	"Open error for MAP file",
/* 3 */	"Open error for download file"
};

char	*dlf_msg[] =
{
/* 0 */	"Accessing (reading) download file",
/* 1 */	"Invalid Hex file, not Motorola-S or Intel-HEX",
/* 2 */	"Invalid record format",
/* 3 */	"Invalid line checksum",
/* 4 */	"Invalid hex digit",
/* 5 */	"Invalid file status",
/* 6 */ "File corrupted"
};

char	*dlx_msg[] =
{
/* 0 */	"Opening IPC device driver",
/* 1 */	"IOctl error",
/* 2 */	"CPF channel error",
/* 3 */	"Timeout trying to access",
/* 4 */	"Timeout waiting for LPF board ready",
/* 5 */	"No LPF board at channel",
/* 6 */	"Failed start-up"
};

char	*dle_msg[] =		/* extra error messages */
{
	"\n     Status = 0x%04x",		/* dle_status */
	"\n     Expecting 0x%04x",		/* dle_expecting */
	" to 0x%04x",				/* dle_range */
	" or 0x%04x",				/* dle_or */
	", Found 0x%04x.",			/* dle_found */
	"\n     IPC status = 0x%04x",		/* dle_ipc */
	"\n     CPF channel status = 0x%04x",	/* dle_cpf */
	"\n     File name is \"%s\"",		/* dle_file */
	"\n     Character is 0x%02x"		/* dle_char */
};

extern  int     Interactive;
extern	int	verbose;
extern  jmp_buf env;
extern  void exit();

char	*undefined_msg = "Error Undefined";
lword	eval[DL_MAX_ERROR_TYPES];

/******************************************************************************
 ****				PROCEDURES			       	   ****
 *****************************************************************************/

udl_error(errnum,errcnt,errval)
int	errnum;		/* string number to display (ttnn) tt = type */
int	errcnt;		/* count of values, (ffffffcc) f = flag, cc = count */
lword	errval[];	/* array of values */
{
	extern SCSI_DEV *sd;
	struct dldata FDS;
	lword	num;
	int	i;
	char	*estr;
	
	num = errnum & 0xf;
	switch( (char) (errnum >> 4) )		/* check string type */
	{
	case error_main:
		estr = dlm_msg[num];
		break;
	case error_file:
/* Added to clean up hung controller - jpk */
		
	/* The purpose of this write_buffer is in the event of a failed
	   write buffer, after a _N_ successful write, the controller is
	   left in a _HUNG_ state.  To get its attention need to send this
	   down to let it know that we're done. */
	
	    FDS.count = 0;
	    FDS.address = 0;
	    FDS.buffer = 0;
		
	    sd->write_buffer(sd, FDS.buffer, FDS.count, FDS.address,TRUE);
		
/* End of additions - jpk */
		estr = dlf_msg[num];
		break;
	case error_xfer:
		estr = dlx_msg[num];
		break;
	default:
		estr = undefined_msg;
	}
	fprintf(stderr,"\n");
	if (verbose)	fprintf(stderr,"%3d: ",errnum);
	fprintf(stderr,"* Downloader error *: %s.",estr);
	if ((errcnt & 0xf) != 0)
	{
		errcnt >>= 4;
		for (i=0; i < DL_MAX_ERROR_TYPES; i++,errcnt >>= 1)
			if (errcnt & 0x01)
				fprintf(stderr, dle_msg[i], errval[i]);

	}
	fprintf(stderr,"\n");
        if (!Interactive) exit (5);
        else
         longjmp(env,1);
}
