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
#ifndef _SINK_H
#define _SINK_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		sink.h
 Title:		SINK Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:36 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the SINK class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	SINK_CLASS	OBJECT_CLASS					\
	int (*put) ( void *this, u_long something );
/* end SINK_CLASS */

typedef struct sink {
	SINK_CLASS
} SINK;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
SINK *
new_SINK();

void
destroy_SINK( void *this );

#endif
