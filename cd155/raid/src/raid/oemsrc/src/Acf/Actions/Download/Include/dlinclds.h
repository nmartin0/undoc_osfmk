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
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
 Name:		$RCSfile: dlinclds.h,v $
 Version:	$Id: dlinclds.h,v 1.2.4.1 1995/06/11 23:25:08 kat Exp $
  Title:	Download file inclusion list
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:25:08 $
		(last change by: $Author: kat $)
  Programmer:	rmj
  Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains the include files fot the download.
*/
/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:25:08 $"
#undef H_ID

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
/* Added for DAC firmware download                                     */
/* Moved next two includes from last in the list to here so that the
   programmer who redifined NULL in ccdrv.h gets what he expects       */
#include "stddefs.h"
#include "scsi_dev.h"
/* End of additions                                                    */  
#include "dlerror.h"
#include "ccdrv.h"
#include "ccioctl.h"
#include "ccerrors.h"

/******************************************************************************
 ***				VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

#define	FALSE 	0
#define TRUE  	1


struct dldata
{
				long	count;
				long	address;
				byte	*buffer;
				Lint	cs;
};
