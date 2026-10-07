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
 Name:		dac_inqdat.c
 Title:		DAC_INQDAT Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:34:00 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilitlies FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_INQDAT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_inqdat.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int	
display( this )
void *this;
#define THIS	( (DAC_INQDAT *) this )
{
	int	status;
	char vendor_id[9], product_id[17], rev_level[5];

	SCSI_Inquiry_Data_t inqdata;

	debug("\nEntered dac_inqdat.display");
	status = THIS->sd->inquiry( THIS->sd, (u_char * ) & inqdata,
	    sizeof( inqdata ) );
	if ( status == 0 ) {
		strncpy( vendor_id, inqdata.Vendor_ID, 8 );
		vendor_id[8] = NULL;
		strncpy( product_id, inqdata.Product_ID, 16 );
		product_id[16] = NULL;
		strncpy( rev_level, inqdata.Revision_Level, 4 );
		rev_level[4] = NULL;
		/* display inquiry data */
		printf( "Inquiry information                      = %s %s %s\n",
		    vendor_id, product_id, rev_level );
	} else
		error( SCSI_CONDITION, NO_INQUIRY_DATA);
	return (status);
}
#undef THIS


void
destroy_DAC_INQDAT( this )
void *this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_INQDAT *
new_DAC_INQDAT( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long  hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_INQDAT * this;

	debug( "entered new_DAC_INQDAT\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_INQDAT * ) zalloc( sizeof( DAC_INQDAT ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	this->description = "Inquiry Data";
	this->destroy = destroy_DAC_INQDAT;
	this->display = display;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_INQDAT\n" );
	return( this );
}
