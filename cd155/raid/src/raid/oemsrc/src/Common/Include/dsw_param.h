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
#ifndef _DSW_PARAM_H
#define _DSW_PARAM_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dsw_param.h
 Title:		DSW_PARAM Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:03 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DSW_PARAM class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dac_param.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	DSW_PARAM_CLASS	DAC_PARAM_CLASS
/* end DSW_PARAM_CLASS */

typedef struct dsw_param {
	DSW_PARAM_CLASS
} DSW_PARAM;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DSW_PARAM *
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
new_DSW_PARAM( hw_zipcode hw_addr );
#else
new_DSW_PARAM( u_long hw_addr );
#endif

void
destroy_DSW_PARAM( void *this );

#endif
