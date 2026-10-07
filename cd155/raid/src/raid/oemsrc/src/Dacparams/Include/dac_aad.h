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
 
#ifndef _DAC_AAD_H
#define _DAC_AAD_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_aad.h
 Title:		DAC_AAD Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:32:23 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DAC_AAD class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dsw_param.h"
#include "menu.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DAC_AAD_CLASS 		DSW_PARAM_CLASS				\
        logical_array_page_t lapage;
/* end DAC_AAD_CLASS */

typedef struct dac_aad {
        DAC_AAD_CLASS
} DAC_AAD;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DAC_AAD *
#ifdef PARAGON860  /* remove 3 bit storage limitation for IOBus */
new_DAC_AAD( MENU *menu, hw_zipcode hw_addr );
#else
new_DAC_AAD( MENU *menu, u_long hw_addr );
#endif

void
destroy_DAC_AAD( void *this );

#endif
