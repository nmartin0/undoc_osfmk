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
 Name:		dac_param.c
 Title:		DAC_PARAM Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:08 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_PARAM class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <ctype.h>
#include <stdlib.h>
#include "stddefs.h"
#include "dac_param.h"
#include "dau_err.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

logical_array_page_t	default_logical_page = {
	     	 			/* Mode10ParameterHeader */
	0x00,				/* ModeDataLength (MSB) */
	0x00,				/* ModeDataLength (LSB) */
	0x00,				/* MediumType */
	0x00,				/* DeviceSpecificParameter */
	0x00,				/* Reserved1 */
	0x00,				/* Reserved2 */
	0x00,				/* BlockDescriptorLength  (MSB) */
	0x08,				/* BlockDescriptorLength (LSB) */
	  				/* Mode10BlockDescriptor */
	0,				/* NumberOfBlocks */
	0,				/* DensityCode */
	0,				/* BlockLength */
	0,				/* reserved */
	LOGICAL_ARRAY_PG_CODE,		/* Page_Code */
	0,				/* Parameters_Savable */
	130,				/* Page_Length */
	ADD_LUN,			/* Action_Status */
#ifdef PARAGON860
	5,				/* RAID Level */
#else
	0,				/* RAID Level */
#endif
	0,				/* LUN_Type */
	0x00,				/* LUN_Block_Size (MSB) */
	0x00,				/* LUN_Block_Size */
	0x02,				/* LUN_Block_Size */
	0x00,				/* LUN_Block_Size (LSB) */
	0x02,				/* Drive_Sector_Size (MSB) */
	0x00,				/* Drive_Sector_Size (LSB) */
	0x00,				/* LUN_Number_of_Blocks (MSB) */
	0x00,				/* LUN_Number_of_Blocks */
	0x00,				/* LUN_Number_of_Blocks */
	0x00,				/* LUN_Number_of_Blocks (LSB) */
	0,				/* Reserved */
	0,				/* Reserved */
	0,				/* Reserved */
	0,				/* Reserved */
	0x00,				/* LUN_Segment_Size (MSB) */
	0x00,				/* LUN_Segment_Size */
	0x02,				/* LUN_Segment_Size */
	0x00,				/* LUN_Segment_Size (LSB) */
	1,				/* Segment_Zero_Size */
	/* LUN Flags, byte 1: */
		0,			/* Type of Mode Page to Sense */
		0,    			/* Reserved */
	/* LUN Flags, byte 2: */
		0,			/* Reserved */
		1,			/* Auto_Detect_Replacement_Disabled */
		0,			/* Reserved */
		0,			/* Reserved */
		1,			/* AEN_Polling_Enabled */
		1,			/* Parity_Verification_Enabled */
		1,			/* Write_Parity_Verification_Enabled */
		0,			/* Reserved */
	0,				/* Block_Reconstructed */
	0,				/* Block_Reconstructed */
	0,				/* Block_Reconstructed */
	0,				/* Block_Reconstructed */
	0x01,				/* Reconstruction_Frequency */
	0x00,				/* Reconstruction_Amount */
	0x40,				/* Reconstruction_Amount */
	0,				/* Reserved */
	0,				/* Reserved */
	0,				/* Reserved */
	0,0,0,0,0,0,0,0,		/* Disk Bit Map */
	0,0,0,0,0,0,0,0,		/* Disk Bit Map */
	0,0,0,0,0,0,0,0,		/* Disk Bit Map */
	0,0,0,0,0,0,0,0,		/* Disk Bit Map */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0,		/* Configuration Table */
	0,0,0,0,0,0,0,0			/* Configuration Table */
};

rw_recovery_page_t	default_recovery_page = {
	     	 			/* Mode10ParameterHeader */
	0x00,				/* ModeDataLength (MSB) */
	0x00,				/* ModeDataLength (LSB) */
	0x00,				/* MediumType */
	0x00,				/* DeviceSpecificParameter */
	0x00,				/* Reserved1 */
	0x00,				/* Reserved2 */
	0x00,				/* BlockDescriptorLength  (MSB) */
	0x08,				/* BlockDescriptorLength (LSB) */
	  				/* Mode10BlockDescriptor */
	0,				/* NumberOfBlocks */
	0,				/* DensityCode */
	0,				/* BlockLength */
	0,				/* reserved */
	RW_RECOVERY_PG_CODE,		/* Page_Code */
	0,				/* Parameters_Savable */
	10,				/* Page_Length */
	0,				/* DCR */
	0,				/* DTE */
	1,				/* PER */
	0,				/* EER */
	0,				/* RC */
	0,				/* TB */
	1,				/* ARRE */
	1,				/* AWRE */
	1,				/* Read_Retry_Count */
	0,				/* Correction_Span */
	0,				/* Head_Offset_Count */
	0,				/* Data_Strobe_Offset_Count */
	0,				/* Reserved */
	1,				/* Write_Retry_Count */
	0,				/* Reserved */
	0,				/* Recovery_Time_Limit[MSB] */
	0				/* Recovery_Time_Limit[LSB] */
};

extern u_int Suppress_Messages;
static int	get_page( void *, u_char *, u_int, u_char );

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_PARAM *) this)
{
	return(FALSE);
}
#undef THIS
 
static void	
describe( this )
void	*this;
#define THIS ((DAC_PARAM *) this)
{
	Suppress_Messages = TRUE;
	printf( "%s ", THIS->description );
	printf( "%c", '[' );
	THIS->display_value( THIS );
	printf( "%c", ']' );
	Suppress_Messages = FALSE;
}
#undef THIS


static u_long
edit( this, str, str_valid )
void	*this;
u_char	*str;
int	*str_valid;
#define THIS ((DAC_PARAM *) this)
{
	int	i;

	debug("\nEntered dac_param.c:edit str = %s\n",str);
	if ( strcmp( (char *) str, "d" ) == 0 ) {
		*str_valid = TRUE;
		return( -1 );
	}
	for (i = 0; i < strlen((char *) str) && isdigit(str[i]); i++)		;
	if (i < strlen((char *) str)) {           /* premature termination */
		/* Invalid selection */
		error(ARGUMENT_RANGE, INVALID_SELECT);
		*str_valid = FALSE;
		return (0);
	} 
	if ((atol( (char *) str ) >= THIS->lower_bound ) && 
		(atol((char *) str) <= THIS->upper_bound )) /* ck if in range */
	{
		*str_valid = TRUE;
		return( atol( (char *) str ) );
	}
	else
	{
		/* Invalid range */
		error(ARGUMENT_RANGE, INVALID_RANGE);
		*str_valid = FALSE;
		return (0);
	} 
}
#undef THIS



static int	
set( this, val )
void	*this;
u_long val;
{
	return ( 0 );
}


static int	
read( this, val )
void	*this;
u_long *val;
{
	return ( 0 );
}


static int	
display( this )
void	*this;
#define THIS ((DAC_PARAM *) this)
{
	printf( "%s ", THIS->display_text );
	fflush( stdout );
	THIS->display_value( THIS );
	printf( "\n" );

	return ( 0 );
}


#undef THIS

static int	
display_value( this )
void	*this;
{
	return ( 0 );
}


static int	
lu_exists( this )
void	*this;
#define THIS ((DAC_PARAM *) this)
{
	int	exists = FALSE;
	SCSI_Inquiry_Data_t inqdata;

	if ( THIS->sd->inquiry( THIS->sd, (u_char * ) & inqdata,
	    sizeof( inqdata ) ) == 0 )
		if ( inqdata.Periph_Qualifier == LUN_CONNECTED )
			exists = TRUE;
#ifdef TEST
	return( FALSE );
#else
	return( exists );
#endif
}


#undef THIS

static int	
get_page( this, page, pg_size, pg_code )
void	*this;
u_char *page;
u_int pg_size;
u_char pg_code;
#define THIS ((DAC_PARAM *) this)
{
	u_char chgpage[MAX_PAGE_SIZE];
	u_char curpage[MAX_PAGE_SIZE];
	u_char *z;
	int	stat, i;

	switch ( pg_code ) {
	case LOGICAL_ARRAY_PG_CODE:
		if ( THIS->lu_exists( THIS ) == FALSE ) {
			u_short	*map;
			int	size;
			extern void set_bit();
			hw_zipcode_t *hwaddr_p;

			size = sizeof( logical_array_page_t );
			memcpy( page, &default_logical_page, size );
			((logical_array_page_t *)page)->Action_Status = ADD_LUN;
			/* fill in disk bit map appropriately */
			hwaddr_p = (hw_zipcode_t *) &THIS->hw_addr;
			map = (u_short *)
			     &((logical_array_page_t *)page)->Disk_Bit_Map;
#ifdef PARAGON860
			set_bit( &map[0], 0 );
			set_bit( &map[0], 1 );
			set_bit( &map[0], 2 );
			set_bit( &map[0], 3 );
			set_bit( &map[0], 4 );
#else
			set_bit( &map[1], 0 );
			set_bit( &map[1], 1 );
			set_bit( &map[1], 2 );
			set_bit( &map[1], 3 );
			set_bit( &map[1], 4 );
#endif
			return( 0 );
		} else {
	/********* Modified to correct mode_select call ***********/
	if (THIS->unchangeable==FALSE) {
		stat = THIS->sd->mode_sense( THIS->sd, chgpage, pg_size,
    			PAGE_CONTROL_CHANGEABLE_VALUES, pg_code );
		stat = THIS->sd->mode_sense( THIS->sd, curpage, pg_size,
			PAGE_CONTROL_CURRENT_VALUES, pg_code );
		z=page;
		for (i=0; i<pg_size; i++) {
			*z=curpage[i] & chgpage[i];
			z++;
		}
	}
	else {
		stat = THIS->sd->mode_sense( THIS->sd, page, pg_size,
			PAGE_CONTROL_CURRENT_VALUES, pg_code );
	}
	/************** End of modification **************/
			((logical_array_page_t *)page)->Action_Status =
			    REPLACE_LUN;
			return( stat );
		}
	case RW_RECOVERY_PG_CODE:
		if ( THIS->lu_exists( THIS ) == FALSE ) {
			int	size;

			size = sizeof( rw_recovery_page_t );
			memcpy( page, &default_recovery_page, size );
			return( 0 );
		}
	}
	/********* Modified to correct mode_select call ***********/
	if (THIS->unchangeable==FALSE) {
		stat = THIS->sd->mode_sense( THIS->sd, chgpage, pg_size,
    			PAGE_CONTROL_CHANGEABLE_VALUES, pg_code );
		stat = THIS->sd->mode_sense( THIS->sd, curpage, pg_size,
			PAGE_CONTROL_CURRENT_VALUES, pg_code );
		z=page;
		for (i=0; i<pg_size; i++) {
			*z=curpage[i] & chgpage[i];
			z++;
		}
	}
	else {
		stat = THIS->sd->mode_sense( THIS->sd, page, pg_size,
			PAGE_CONTROL_CURRENT_VALUES, pg_code );
	}
	return(stat);
	/************** End of modification **************/
}


#undef THIS

void
destroy_DAC_PARAM( this )
void	*this;
#define THIS	( (DAC_PARAM *) this )
{
	destroy_SCSI_DEV( (SCSI_DEV * ) THIS->sd );
	destroy_DIALOG( (DIALOG * ) this );
#undef THIS
}


DAC_PARAM *
new_DAC_PARAM( hw_addr )
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long hw_addr;
#endif
{
	int exclusive;
	DIALOG * s;
	DAC_PARAM * this;

	debug( "entered new_DAC_PARAM\n" );
	s = new_DIALOG();
	this = (DAC_PARAM * ) zalloc( sizeof( DAC_PARAM ) );
	memcpy( this, s, sizeof( DIALOG ) ); /* inherit from DIALOG */
	free( s );
	this->lower_bound = 0;
	this->upper_bound = 2147483647 /* 4294967295 (too big) */;
	this->sd = new_SCSI_DEV ( hw_addr, exclusive = FALSE );
	this->hw_addr = hw_addr;
	this->destroy = destroy_DAC_PARAM;
	this->describe = describe;
	this->fmt_required = fmt_required;
	this->edit = edit;
	this->set = set;
	this->read = read;
	this->display = display;
	this->display_value = display_value;
	this->lu_exists = lu_exists;
	this->get_page = get_page;
	debug( "leaving new_DAC_PARAM\n" );
	return( this );
}


