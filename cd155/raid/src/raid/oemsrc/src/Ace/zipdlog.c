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
  Version:	$Id: zipdlog.c,v 1.2.4.1 1995/06/11 22:20:08 kat Exp $
  Title:	
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:20:08 $
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
 * Revision 1.2.4.1  1995/06/11  22:20:08  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:59:11  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:30:00  richardg
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

#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode 
#else
u_long
#endif
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

edit_DAC_part( str, hw, str_valid, lun_specified )
u_char *str;
hw_zipcode_t *hw;
int *str_valid;
int *lun_specified;
{
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
	int	i;
	u_long value;
#else
	int i, value;
#endif
	u_char s[8], *p, hw_designator;
	u_int cmp;

	debug( "editing zipcode\n" );
	s[0] = SYSBUS_DELIM;
	s[1] = SLOTNUM_DELIM;
	s[2] = SCSIBUS_DELIM;
	s[3] = TARGET_DELIM;
	s[4] = LUN_DELIM;
	s[5] = (u_char) NULL;
	for ( p = str; *p; p++ ) {
		hw_designator = *p;
		/* If the following condition is true,		*/
		/* then *p IS one of the characters in s	*/
		if ( ( cmp = (u_int) strcspn((char *) p++, (char *) s) ) == 0 ) {
			if ( ! isdigit( *p ) ) {
				*str_valid = FALSE;
				ace_error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
				return( 0 );
			}
			else {
				value = 0;
				while ( isdigit( *p ) )
					value = (value * 10) + (*p++ - '0');
				--p;
			}
			switch ( hw_designator ) {
			case SYSBUS_DELIM:
				if ( IO_BUS_MIN <= value
				  && value <= IO_BUS_MAX )
					hw->IO_Bus = value;
				else {
					ace_error( ARGUMENT_RANGE, IOB_RANGE_ERROR );
					*str_valid = FALSE;
				}
					
				break;
			case SLOTNUM_DELIM:
				if ( SCSI_SLOT_MIN <= value
				  && value <= SCSI_SLOT_MAX )
					hw->SCSI_Controller = value;
				else {
					ace_error( ARGUMENT_RANGE, SLOT_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case SCSIBUS_DELIM:
				if ( SCSI_BUS_MIN <= value
				  && value <= SCSI_BUS_MAX )
					hw->SCSI_Bus = value;
				else {
					ace_error( ARGUMENT_RANGE, SBUS_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case TARGET_DELIM:
				if ( SCSI_PUN_MIN <= value
				  && value <= SCSI_PUN_MAX )
					hw->PUN = value;
				else {
					ace_error( ARGUMENT_RANGE, SPUN_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case LUN_DELIM:
				if ( SCSI_LUN_MIN <= value
				  && value <= SCSI_LUN_MAX ) {
					*lun_specified = TRUE;
					hw->LUN = value;
				}
				else {
					ace_error( ARGUMENT_RANGE, SLUN_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			}
		}
		else { /*  Else *p IS NOT a character in s  */
			*str_valid = FALSE;
			ace_error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
			return( 0 );
		}
	}
}

edit_drive_part( str, hw, str_valid )
u_char *str;
hw_zipcode_t *hw;
int *str_valid;
{
	int i, value;
	u_char s[8], *p, hw_designator;
	u_int cmp;

	debug( "editing zipcode\n" );
	s[0] = SCSIBUS_DELIM;
	s[1] = TARGET_DELIM;
	s[2] = (u_char) NULL;
	for ( p = str; *p; p++ ) {
		hw_designator = *p;
		/* If the following condition is true,		*/
		/* then *p IS one of the characters in s	*/
		if ( ( cmp = (u_int) strcspn((char *) p++, (char *) s) ) == 0 ) {
			if ( ! isdigit( *p ) ) {
				*str_valid = FALSE;
				ace_error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
				return( 0 );
			}
			else {
				value = 0;
				while ( isdigit( *p ) )
					value = (value * 10) + (*p++ - '0');
				--p;
			}
			switch ( hw_designator ) {
			case SCSIBUS_DELIM:
				if ( DAC_SCSI_BUS_MIN <= value
				  && value <= DAC_SCSI_BUS_MAX )
					hw->DAC_SCSI_Bus = value;
				else {
					ace_error( ARGUMENT_RANGE, DAC_SBUS_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case TARGET_DELIM:
				if ( DAC_SCSI_PUN_MIN <= value
				  && value <= DAC_SCSI_PUN_MAX )
					hw->DAC_PUN = value;
				else {
					ace_error( ARGUMENT_RANGE, DAC_SPUN_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			}
		}
		else { /*  Else *p IS NOT a character in s  */
			*str_valid = FALSE;
			ace_error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
			return( 0 );
		}
	}
}

