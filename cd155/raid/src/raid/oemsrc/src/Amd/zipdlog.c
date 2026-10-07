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
  Name:		$RCSfile: zipdlog.c,v $
  Version:	$Id: zipdlog.c,v 1.1.4.1 1995/06/11 23:27:31 kat Exp $
  Title:	
  Revision:	$Revision: 1.1.4.1 $
  Update Date:	$Date: 1995/06/11 23:27:31 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272
  COPYRIGHT 1992, NCR Corporation

  Description:	This module coverts the 'zipcode' into the hardware address.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: zipdlog.c,v $
 * Revision 1.1.4.1  1995/06/11  23:27:31  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.1  1992/12/28  18:31:34  richardg
 * Initial revision
 *
 * Revision 1.2  1992/03/25  13:12:15  root
 * Fixed WattComm Compiler warnings. (ulong * parm instead of hw_zipcode_t * parm)
 *
 * Revision 1.1  1992/03/18  14:00:30  bmyers
 * Initial revision
 *
 */
/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include "stddefs.h"
/* #include "zipcod_dlg.h" */
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

u_long
zip_edit( in_str, str_valid )
u_char *in_str;
int *str_valid;
{
	u_char str[256];
	u_char *p;
	hw_zipcode hw_addr;
	hw_zipcode_t *hw;
	int lun_specified;

	strcpy( (char *) str, (char *) in_str );
	hw = (hw_zipcode_t *) &hw_addr;
	hw_addr = 0;			/* initialize all values to zero;    */
	hw->DAC_SCSI_Bus = 1;		/* except DAC_SCSI_Bus is 1-relative */
#ifdef NOVELL
	{
		u_char default_dac_pun = 1;
		hw->DAC_SCSI_Bus = default_dac_pun;
		hw->DAC_PUN = default_dac_pun;
		/* (constant "1" in above doesn't work with Novell */
	}
#endif
	lun_specified = FALSE;
	if ( p = (u_char *) strchr( (char *) str, ':' ) )
		*p = (u_char) NULL;
	*str_valid = TRUE;
	edit_DAC_part( str, hw, str_valid, &lun_specified );
	if ( p && *str_valid )
		if ( lun_specified )
			*str_valid = FALSE;
		else
			edit_drive_part( ++p, hw, str_valid );
	return( hw_addr );
}


