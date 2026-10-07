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
 
#ifndef _CONSOLE_H
#define _CONSOLE_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		console.h
 Title:		CONSOLE Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:27:06 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the CONSOLE class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <signal.h>
#include "stddefs.h"
#include "sink.h"
#include "object_set.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	CONSOLE_CLASS	SINK_CLASS					\
	FILE *console_fp;						\
	OBJECT_SET *dac_events;						\
	int (*refresh) ( void *this, hw_zipcode hw_addr, u_long rf_window );
/* end CONSOLE_CLASS */

typedef struct console {
	CONSOLE_CLASS
} CONSOLE;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
CONSOLE *
new_CONSOLE( int nevents );

void
destroy_CONSOLE( void *this );

#endif
