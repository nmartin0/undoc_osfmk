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
 Name:		zipcod_dlg.c
 Title:		ZIPCOD_DLG Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:31:09 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DIALOG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include "stddefs.h"
#include "zipcod_dlg.h"
#include "dau_err.h"

extern u_int Interactive;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
#define THIS	( (ZIPCOD_DLG *) this )

#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
static hw_zipcode
#else
static u_long
#endif
edit( this, in_str, str_valid )
void *this;
u_char *in_str;
int *str_valid;
{
	u_char str[256];
	u_char *p;
	hw_zipcode hw_addr;
	hw_zipcode_t *hw;
	int lun_specified;

#ifdef PARAGON860 /* cast to make the compiler happy */
	strcpy( (char *) str, (char *)  in_str );
#else
	strcpy( (u_char *) str, (u_char *)  in_str );
#endif
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
	if ( strlen( (char *) THIS->reply_str ) && *str_valid ) {
		THIS->default_valid = TRUE;
		THIS->default_value = hw_addr;
		*( strrchr( THIS->prompt_str, '(' ) ) = NULL;
		strcat( THIS->prompt_str, "(" );
		strcat( THIS->prompt_str, (char *) THIS->reply_str );
		strcat( THIS->prompt_str, "):" );
	}
	return( hw_addr );
}

edit_DAC_part( str, hw, str_valid, lun_specified )
u_char *str;
hw_zipcode_t *hw;
int *str_valid;
int *lun_specified;
{
#ifdef PARAGON860  /* remove 3 bit storage limitation for IOBus */
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
				error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
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
					error( ARGUMENT_RANGE, IOB_RANGE_ERROR );
					*str_valid = FALSE;
				}
					
				break;
			case SLOTNUM_DELIM:
				if ( SCSI_SLOT_MIN <= value
				  && value <= SCSI_SLOT_MAX )
					hw->SCSI_Controller = value;
				else {
					error( ARGUMENT_RANGE, SLOT_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case SCSIBUS_DELIM:
				if ( SCSI_BUS_MIN <= value
				  && value <= SCSI_BUS_MAX )
					hw->SCSI_Bus = value;
				else {
					error( ARGUMENT_RANGE, SBUS_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case TARGET_DELIM:
				if ( SCSI_PUN_MIN <= value
				  && value <= SCSI_PUN_MAX )
					hw->PUN = value;
				else {
					error( ARGUMENT_RANGE, SPUN_RANGE_ERROR );
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
					error( ARGUMENT_RANGE, SLUN_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			}
		}
		else { /*  Else *p IS NOT a character in s  */
			*str_valid = FALSE;
			error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
			return( 0 );
		}
	}
}

edit_drive_part( str, hw, str_valid )
u_char *str;
hw_zipcode_t *hw;
int *str_valid;
{
#ifdef PARAGON860  /* remove 3 bit storage limitation for IOBus */
	int i;
	u_long	value;
#else
	int i, value;
#endif
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
				error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
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
					error( ARGUMENT_RANGE, DAC_SBUS_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			case TARGET_DELIM:
				if ( DAC_SCSI_PUN_MIN <= value
				  && value <= DAC_SCSI_PUN_MAX )
					hw->DAC_PUN = value;
				else {
					error( ARGUMENT_RANGE, DAC_SPUN_RANGE_ERROR );
					*str_valid = FALSE;
				}
				break;
			}
		}
		else { /*  Else *p IS NOT a character in s  */
			*str_valid = FALSE;
			error( COMMAND_LINE_SYNTAX, INVALID_HWADDR );
			return( 0 );
		}
	}
}

void
destroy_ZIPCOD_DLG( this )
void *this;
{
	debug( "entered destroy_ZIPCOD_DLG, this = %x\n", this );
	free( THIS->prompt_str );
	destroy_DIALOG( this );
	debug( "leaving destroy_ZIPCOD_DLG\n" );
}

#undef THIS

ZIPCOD_DLG *
new_ZIPCOD_DLG( prompt_mask )
hw_zipcode prompt_mask;
{
	DIALOG *s;
	char czbuf[32];
	ZIPCOD_DLG *this;
	extern char *convert_zip();

	debug( "entered new_ZIPCOD_DLG\n" );
	s = new_DIALOG();
	this = (ZIPCOD_DLG *) zalloc( sizeof( ZIPCOD_DLG ) );
	memcpy( this, s, sizeof( DIALOG ) ); /* inherit from DIALOG */
	free( s );
	strcpy( (char *) this->keyval, "z" );
	this->description = "device hardware address";
	this->prompt_str = (char *) zalloc( 512 );
	if (!Interactive)
		this->default_valid = FALSE;
	else
		this->default_valid = TRUE;
	this->default_value = (u_long) this->hw_address;
	sprintf( this->prompt_str, "\nA value for the device hardware address is required.  It should be\nof the form %cX%cX%cX%cX%cX or %cX%cX%cX%cX:%cX%cX, where m, c, b, p, and l\nrepresent delimiters and each X represents a decimal value.  If X = 0,\nthen both the delimiter and X value are omitted.\n\nPlease enter a value for the device hardware address (%s):", SYSBUS_DELIM, SLOTNUM_DELIM, SCSIBUS_DELIM, TARGET_DELIM, LUN_DELIM, SYSBUS_DELIM, SLOTNUM_DELIM, SCSIBUS_DELIM, TARGET_DELIM, SCSIBUS_DELIM, TARGET_DELIM, convert_zip( (hw_zipcode) 0, czbuf ) );
	this->arg_needed = TRUE;
	this->prompt_mask = prompt_mask;
	this->edit = edit;
	this->destroy = destroy_ZIPCOD_DLG;
	debug( "leaving new_ZIPCOD_DLG\n" );
	return( this );
}
