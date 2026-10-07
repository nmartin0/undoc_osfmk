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
  Name:		$RCSfile: sysdname.c,v $
  Version:	$Id: sysdname.c,v 1.2.4.1 1995/06/11 22:20:07 kat Exp $
  Title:	System Device Name ( UNIX )
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:20:07 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272


  COPYRIGHT 1992, NCR Corporation

  Description:	This module constructs the system device name. 
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: sysdname.c,v $
 * Revision 1.2.4.1  1995/06/11  22:20:07  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:55  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:57  richardg
 * Initial revision
 *
 * Revision 1.1  1992/03/23  22:15:04  bmyers
 * Initial revision
 *
 * Revision 1.2  1992/03/20  22:06:09  bmyers
 * Changed to accomodate more than 1 bus
 *
 * Revision 1.1  1992/03/18  14:07:35  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#ifdef PARAGON_860
/******************************************************************************
 ***                          GLOBAL VARIABLES                              ***
 *****************************************************************************/
char Devname[100];
#endif
/******************************************************************************
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern hw_zipcode	hw_addr;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
extern hw_zipcode	IOBus;
#else
extern int		IOBus;
#endif
/******************************************************************************
 ***				   PROCEDURES				    ***
 *****************************************************************************/

void get_system_dev_name ( char *s, int lun )
{
#ifdef PARAGON860 /* display the h/w zipcode in the device field */
	char wrkstr[256];
#endif
	hw_zipcode_t *p = ( hw_zipcode_t * ) &hw_addr;

	if ( p->IO_Bus )
#ifdef PARAGON860 /* display the h/w zipcode in the device field */
		sprintf ( s, "%s ", convert_zip(hw_addr,wrkstr));
#else
		sprintf ( s, "c%x%x%xt%xd%x    ",
			p->IO_Bus, IOBus, p->SCSI_Bus, p->PUN, lun );
#endif
	else if ( IOBus )
#ifdef PARAGON860 /* display the h/w zipcode in the device field */
		sprintf ( s, "%s ", convert_zip(hw_addr,wrkstr));
#else
		sprintf ( s, " c%x%xt%xd%x    ",
			IOBus, p->SCSI_Bus, p->PUN, lun );
#endif
	else
#ifdef PARAGON860 /* display the h/w zipcode in the device field */
		sprintf ( s, "%s ", convert_zip(hw_addr,wrkstr));
#else
		sprintf ( s, "  c%xt%xd%x    ", p->SCSI_Bus, p->PUN, lun );
#endif
}
