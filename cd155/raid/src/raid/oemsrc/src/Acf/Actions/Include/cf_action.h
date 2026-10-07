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
 
#ifndef _CF_ACTION_H
#define _CF_ACTION_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		cf_action.h
 Title:		CF_ACTION Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:35 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the CF_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "action.h"
#include "menu.h"
#include "dac_param.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	CF_ACTION_CLASS	ACTION_CLASS					\
	MENU *(*new_menu)();						\
	DAC_PARAM *(*newdp[32])();
/* end CF_ACTION_CLASS */

typedef struct cf_action {
	CF_ACTION_CLASS
} CF_ACTION;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
CF_ACTION *
new_CF_ACTION();

void
destroy_CF_ACTION( void *this );

#endif
