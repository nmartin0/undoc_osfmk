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
 
#ifndef _CAN_ACTION_H
#define _CAN_ACTION_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		can_action.h
 Title:		CAN_ACTION Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:27 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the CAN_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "action.h"
#include "dau_err.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	CAN_ACTION_CLASS	ACTION_CLASS				\
	hw_zipcode hw_addr;						\
	char ccf_shell_script_name[80];					\
/* end CAN_ACTION_CLASS */

typedef struct can_action {
	CAN_ACTION_CLASS
} CAN_ACTION;

/* used for storing canned configuration information */
typedef struct can_file {
	char	key_value[32];
	char	description[256];
	char	shell_script_name[80];
} can_file_t;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
CAN_ACTION *
new_CAN_ACTION( hw_zipcode hw_addr, int i );

void
destroy_CAN_ACTION( void *this );

#endif
