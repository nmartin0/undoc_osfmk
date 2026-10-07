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
 Name:		ucf_action.c
 Title:		UCF_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:45 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the UCF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "ucf_action.h"
#include "ucf_menu.h"
#include "dac_lunsts.h"
#include "dac_drvmap.h"
#include "dac_raidlv.h"
#include "dac_secsiz.h"
#include "dac_blksiz.h"
#include "dac_segsiz.h"
#include "dac_sg0siz.h"
#include "dac_nblks.h"
#include "dac_rcamt.h"
#include "dac_rcfreq.h"
#include "dac_adr.h"
#include "dac_dmw.h"
#include "dac_aad.h"
#include "dac_mdepge.h"

#ifdef RDAC_TESTWARE
#include "dac_rcmde.h"
#include "dac_arcmde.h"
#endif

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
{
	return ( 0 );
}

void
destroy_UCF_ACTION( this )
void *this;
{
	debug( "entered destroy_CF_ACTION\n" );
	destroy_CF_ACTION( (CF_ACTION *) this );
}

UCF_ACTION *
new_UCF_ACTION( menu )
MENU *menu;
{
	CF_ACTION *s;
	UCF_ACTION *this;

	s = new_CF_ACTION();
	this = (UCF_ACTION *) zalloc( sizeof( UCF_ACTION ) );
	memcpy( this, s, sizeof( CF_ACTION ) ); /* inherit from CF_ACTION */
	free( s );
	this->new_menu = ( MENU *(*)() ) new_UCF_MENU;
	this->newdp[0] = ( DAC_PARAM *(*)() ) new_DAC_LUNSTS;
	this->newdp[1] = ( DAC_PARAM *(*)() ) new_DAC_DRVMAP;
	this->newdp[2] = ( DAC_PARAM *(*)() ) new_DAC_RAIDLV;
	this->newdp[3] = ( DAC_PARAM *(*)() ) new_DAC_SECSIZ;
	this->newdp[4] = ( DAC_PARAM *(*)() ) new_DAC_BLKSIZ;
	this->newdp[5] = ( DAC_PARAM *(*)() ) new_DAC_SEGSIZ;
	this->newdp[6] = ( DAC_PARAM *(*)() ) new_DAC_SG0SIZ;
	this->newdp[7] = ( DAC_PARAM *(*)() ) new_DAC_NBLKS;
	this->newdp[8] = ( DAC_PARAM *(*)() ) new_DAC_RCAMT;
	this->newdp[9] = ( DAC_PARAM *(*)() ) new_DAC_RCFREQ;
	this->newdp[10] = ( DAC_PARAM *(*)() ) new_DAC_ADR;
	this->newdp[11] = ( DAC_PARAM *(*)() ) new_DAC_MDEPGE;
#ifdef RDAC_TESTWARE
	this->newdp[12] = ( DAC_PARAM *(*)() ) new_DAC_RCMDE;
	this->newdp[13] = ( DAC_PARAM *(*)() ) new_DAC_ARCMDE;
#endif
/*
 * Next two dacparams not supported in release 1.0
 * 
	this->newdp[11] = ( DAC_PARAM *(*)() ) new_DAC_DMW;
	this->newdp[12] = ( DAC_PARAM *(*)() ) new_DAC_AAD;
 */
	strcpy( this->keyval, "u" );
	this->arg_needed = FALSE;
	this->description = "Set/Display Array Logical Unit Parameters";
	this->destroy = destroy_UCF_ACTION;
	if ( menu ) {
		menu->actions->add( menu->actions, this );
	}
	debug( "leaving new_UCF_ACTION\n" );
	return( this );
}
