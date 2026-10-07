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
 Name:		dac_drvmap.c
 Title:		DAC_DRVMAP Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:33:54 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_DRVMAP class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_drvmap.h"
#include "menu.h"
#include "dau_err.h"
#include "zip_list.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
extern int	Interactive;
extern u_int	Suppress_Messages;

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern void set_bit();

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
fmt_required( this )
void 	*this;
#define THIS ((DAC_DRVMAP *) this)
{
	return(TRUE);
}
#undef THIS

static void	
set_map( map, channel, drive )
u_short *map;
u_int channel, drive;
{
	debug( "in set map, map = %x, channel = %d, drive = %d\n", map, channel, drive );
	set_bit( &map[drive], channel - 1 );
}


static void	
describe( this )
void	*this;
#define THIS ((DAC_DRVMAP *) this)
{
	Suppress_Messages = TRUE;
	printf( THIS->description );
	printf( " [%d drives]",
	    THIS->ndrives( THIS ) );
	Suppress_Messages = FALSE;
}


#undef THIS

static int	
display_value( this )
void	*this;
#define THIS ((DAC_DRVMAP *) this)
{
	int	status, i, j;
	u_short * map;

	debug( "Entered display_value\n" );
	if ( THIS->ndrives( THIS ) == 0 ) {
		printf( "NULL" );
		return ( 0 );
	}
	status = THIS->read( THIS, (u_long * ) & map );
	if ( status == 0 ) {
		for ( i = 0; i < 16; i++)
			for ( j = 0; j < 15; j++)
				if ( bit_set( map[i], j ) ) {
					printf( "(%d,%d) ", j + 1, i );
				}
	} else
		printf( "??" );
	return ( 0 );
}


#undef THIS

static u_int
ndrives( this )
void	*this;
#define THIS ((DAC_DRVMAP *) this)
{
	u_short * map;
	int	ndrives = 0, i, j;

	if ( ( THIS->read( THIS, (u_long * ) & map ) ) == 0 )
		for ( i = 0; i < 16; i++)
			for ( j = 0; j < 16; j++)
				if ( bit_set( map[i], j ) )
					ndrives++;
	return( ndrives ); 
}


#undef THIS

static u_long
prompt( this )
void	*this;
#define THIS ((DAC_DRVMAP *) this)
{
	ZIP_LIST * z;
	hw_zipcode mask, zip;
	hw_zipcode_t * pzip = (hw_zipcode_t * ) & zip;
	hw_zipcode_t * pmask = (hw_zipcode_t * ) & mask;

	if ( Interactive )
		printf( "%s\n", THIS->prompt_str );
	mask = NULL;
	pmask->DAC_SCSI_Bus = TRUE;
	pmask->DAC_PUN = TRUE;
	z = new_ZIP_LIST( mask );
	memset( THIS->drive_map, 0x00, 32 );
	do {
		zip = z->prompt( z );
		debug( "dac_drvmap.c:  back from zip_list.prompt, zip = %d\n", (long)zip );
		if ( (long) zip == -1 ) {
			z->destroy( z );
			return( zip );
		}
		if ( z->list_complete( z ) == FALSE ) {
			set_map( (u_short *)THIS->drive_map, pzip->DAC_SCSI_Bus,
			    pzip->DAC_PUN );
		}
	} while ( z->list_complete( z ) == FALSE );
	z->destroy( z );
	return( (u_long) THIS->drive_map );
}


#undef THIS

static int	
set( this, val )
void	*this;
u_long	val;
#define THIS ((DAC_DRVMAP *) this)
{
	int	status;

	debug("\nEntered dac_drvmap.set");
	status = THIS->get_page( THIS, (u_char * ) & THIS->lapage,
	    sizeof( THIS->lapage ), LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		memcpy( THIS->lapage.Disk_Bit_Map, val, 32 );
		status = THIS->sd->lock(THIS->sd);
		if (status == 0)
			status = THIS->sd->mode_select(THIS->sd,
			((u_char * ) &(THIS->lapage)),
			sizeof(logical_array_page_t),
		    	PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS);
	}
	if (status != 0)
		/*Error in setting Drive Map*/
		error(SCSI_CONDITION, SET_DRVMAP_ERR);
	return(status);
}


#undef THIS

static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ((DAC_DRVMAP *) this)
{
	int	status;

	debug("\nEntered dac_drvmap.read");
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		memcpy( THIS->drive_map, &THIS->lapage.Disk_Bit_Map, 32 );
		*val = (long) THIS->drive_map;
	} else
		/*Error in reading Drive Map*/
		error(SCSI_CONDITION, READ_DRVMAP_ERR);
	debug("\nLeaving dac_drvmap.read");
	return (status);
}


#undef THIS

void
destroy_DAC_DRVMAP( this )
void	*this;
#define THIS ((DAC_DRVMAP *) this)
{
	free( THIS->drive_map );
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


#undef THIS

DAC_DRVMAP *
new_DAC_DRVMAP( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_DRVMAP * this;

	debug( "entered new_DAC_DRVMAP\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_DRVMAP * ) zalloc( sizeof( DAC_DRVMAP ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->drive_map = (u_char * ) zalloc( 32 );
	strcpy( this->keyval, "d" );
	this->arg_needed = TRUE;
	this->prompt_str = "\nThis selection will allow you to enter array drive specifications until you\nterminate the entry by depressing ENTER.  Alternatively, you may enter 'd' to\nsee a display of the drive list.  An 'array drive specification' has the form\nb<n>p<m>, where <n> is the array channel number, and <m> is the drive SCSI ID.\n";
	this->description = "* Unit Drive Map";
	this->display_text = "(Channel,ID) list                        =";
	this->destroy = destroy_DAC_DRVMAP;
	this->prompt = prompt;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	this->fmt_required = fmt_required;
	this->describe = describe;
	this->ndrives = ndrives;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_DRVMAP\n" );
	return( this );
}


