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
 Name:		convert_zip.c
 Title:		CONVERT ZIPCODE Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:59 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module converts a hardware addresss to a string
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "stddefs.h"

/******************************************************************************
 ***				  PROCEDURE 				    ***
 *****************************************************************************/
char *
convert_zip( hw_addr, buf )
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long hw_addr;
#endif
char *buf;
{

	hw_zipcode_t *hw_addr_ptr;
	char temp[3];
	char sysbus_d[2], slotnum_d[2], scsibus_d[2], target_d[2], lun_d[2];

	/* Initialize the delimeter strings */

	sysbus_d[1] = slotnum_d[1] = scsibus_d[1] = target_d[1] = lun_d[1] = 0;
	sysbus_d[0]  = SYSBUS_DELIM;
	slotnum_d[0] = SLOTNUM_DELIM;
	scsibus_d[0] = SCSIBUS_DELIM;
	target_d[0]  = TARGET_DELIM;
	lun_d[0]     = LUN_DELIM;

	hw_addr_ptr = (hw_zipcode_t *) &hw_addr;
	*buf=NULL; 	/* ensure that the buffer is empty */

	/* concatenate the system I/O bus */
	strcat (buf, sysbus_d);
	sprintf(temp,"%d", hw_addr_ptr->IO_Bus);
	strcat (buf,temp);

	/* concatenate the slot number of SCSI host controller */
	strcat (buf, slotnum_d);
	sprintf(temp,"%d", hw_addr_ptr->SCSI_Controller);
	strcat (buf,temp);

	/* concatenate the SCSI Bus */
	strcat (buf, scsibus_d);
	sprintf(temp,"%d", hw_addr_ptr->SCSI_Bus);
	strcat (buf,temp);

	/* concatenate the target ID */
	strcat (buf, target_d);
	sprintf(temp,"%d", hw_addr_ptr->PUN);
	strcat (buf,temp);

	/* concatenate the drive (LUN) */
	strcat (buf, lun_d);
	sprintf(temp,"%d", hw_addr_ptr->LUN);
	strcat (buf,temp);

/*
	************************************************************************
   	For now the DAC SCSI Bus and DAC PUN will not be included in the return-
	ing value of convert_zip.  Eventually a bit will be stored off in an 
	unused portion of the hardware address to determine whether convert_zip
	needs to return the drive LUN or the colon (:) separator followed by
	the DAC SCSI Bus and DAC PUN.
	************************************************************************

	** concatenate the DAC SCSI Bus **
	** use the : as the separator **
	strcat (buf, ":");
	strcat (buf, scsibus_d);
	sprintf(temp,"%d", hw_addr_ptr->DAC_SCSI_Bus);
	strcat (buf,temp);

	** concatenate the DAC target ID **
	strcat (buf, target_d);
	sprintf(temp,"%d", hw_addr_ptr->DAC_PUN);
	strcat (buf,temp);
*/

	return (buf);
}
