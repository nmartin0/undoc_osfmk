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
 
#ifndef _DAC_ARCMDE_H
#define _DAC_ARCMDE_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_arcmde.h
 Title:		DAC_ARCMDE Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:32:28 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1992, NCR Corp.

 Description:	This module defines the DAC_ARCMDE class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DAC_ARCMDE_CLASS	DAC_PARAM_CLASS				\
        redundant_controller_page_t rcpage;
/* end DAC_ARCMDE_CLASS */

typedef struct dac_arcmde {
	DAC_ARCMDE_CLASS
} DAC_ARCMDE;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DAC_ARCMDE *
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
new_DAC_ARCMDE( MENU *menu, hw_zipcode hw_addr );
#else
new_DAC_ARCMDE( MENU *menu, u_long hw_addr );
#endif

void
destroy_DAC_ARCMDE( void *this );

#endif
