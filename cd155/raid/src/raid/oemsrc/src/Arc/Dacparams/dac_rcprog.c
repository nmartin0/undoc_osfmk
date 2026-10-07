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
 Name:		dac_rcprog.c
 Title:		DAC_RCPROG Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:19 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the DAC_RCPROG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "dac_rcprog.h"
#include "menu.h"
#include "dau_err.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
prompt( this )
void	*this;
{
	return( (u_long) - 1 );
}


static int	
set( this, val )
void	*this;
u_long val;
{
#define THIS	( (DAC_RCPROG *) this )
	int	status;
	Bytes_In_Long_t *bval;

	debug("\nEntered set");
	/*Error - Illegal to set highest block number reconstructed*/
	error(NON_SPECIFIC_ERROR, SET_RCPROG_ERR);
	return(1);
#undef THIS
}


static int	
read( this, val )
void	*this;
u_long *val;
#define THIS ( (DAC_RCPROG *) this )
{
	int	status;
	Bytes_In_Long_t *bval;

	debug("\nEntered read");
	bval = (Bytes_In_Long_t *) val;
	status = THIS->sd->mode_sense( THIS->sd, (u_char * ) & THIS->lapage,
		sizeof(THIS->lapage ),PAGE_CONTROL_CURRENT_VALUES,
		LOGICAL_ARRAY_PG_CODE );
	if (status == 0) {
		bval->MSB = THIS->lapage.Block_Reconstructed[0];
		bval->MIB_1 = THIS->lapage.Block_Reconstructed[1];
		bval->MIB_2 = THIS->lapage.Block_Reconstructed[2];
		bval->LSB = THIS->lapage.Block_Reconstructed[3];
	}
	if (status != 0)
		/*Error in reading highest block number reconstructed*/
		error(SCSI_CONDITION, READ_RCPROG_ERR);
	return (status);
}


#undef THIS

static int	
display_value( this )
void	*this;
{
#define THIS	( (DAC_RCPROG *) this )
	struct capacity_and_blocksize {
		u_int capacity;
		u_int blocksize;
	};

	SCSI_DK *sdk;
	u_int status, last_block_reconstructed;
	u_int pmi, block = 0;
	struct capacity_and_blocksize cb;
	float	percentage_reconstructed;
	Bytes_In_Long_t *bil_capacity, *tmp_capacity;
	u_int swapped_capacity;

	debug("\nEntered display");
	status = THIS->read(THIS, (u_long * ) & last_block_reconstructed);
	if (status == 0) {
		sdk = new_SCSI_DK(THIS->hw_addr, TRUE);
		status = sdk->read_capacity(sdk, (u_char * ) & cb, block, pmi = 
		    FALSE);
		if (status == 0) {
			tmp_capacity = (Bytes_In_Long_t *) &cb.capacity;
			bil_capacity = (Bytes_In_Long_t *) &swapped_capacity;
			bil_capacity->MSB = tmp_capacity->LSB;
			bil_capacity->MIB_1 = tmp_capacity->MIB_2;
			bil_capacity->MIB_2 = tmp_capacity->MIB_1;
			bil_capacity->LSB = tmp_capacity->MSB;
			debug("\ncapacity is %d\n", swapped_capacity);
			debug("last_block_reconstructed is %d\n",
				last_block_reconstructed);
			percentage_reconstructed = (float)
				(last_block_reconstructed * 100) / 
			    	swapped_capacity;
			printf("%.2f%%", percentage_reconstructed);
		} else
			printf("??");
		destroy_SCSI_DK (sdk);
	} else
		printf("??");
	return (status);
#undef THIS
}


void
destroy_DAC_RCPROG( this )
void	*this;
{
	destroy_DAC_PARAM( (DAC_PARAM * ) this );
}


DAC_RCPROG *
new_DAC_RCPROG( menu, hw_addr )
MENU *menu;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode hw_addr;
#else
u_long hw_addr;
#endif
{
	DAC_PARAM * s;
	DAC_RCPROG * this;

	debug( "entered new_DAC_RCPROG\n" );
	s = new_DAC_PARAM( hw_addr );
	this = (DAC_RCPROG * ) zalloc( sizeof( DAC_RCPROG ) );
	memcpy( this, s, sizeof( DAC_PARAM ) ); /* inherit from DAC_PARAM */
	free( s );
	strcpy( this->keyval, "P" );
	this->arg_needed = FALSE;
	this->description = "  Display Reconstruction Progress";
	this->display_text = "Progress of the reconstruction           = ";
	this->unchangeable=TRUE;
	this->destroy = destroy_DAC_RCPROG;
	this->prompt = prompt;
	this->set = set;
	this->read = read;
	this->display_value = display_value;
	if ( menu )
		menu->add_selection( menu, this );
	debug( "leaving new_DAC_RCPROG\n" );
	return( this );
}
