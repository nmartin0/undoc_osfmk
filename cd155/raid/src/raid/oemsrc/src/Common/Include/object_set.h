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
#ifndef _OBJECT_SET_H
#define _OBJECT_SET_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		object_set.h
 Title:		OBJECT_SET Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:24 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the OBJECT_SET class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	OBJECT_SET_CLASS	OBJECT_CLASS				\
	u_int maxelements;						\
	u_int nelements;						\
	u_int first_element;						\
	u_int next_available;						\
	int current_position;						\
	OBJECT **contents;						\
	OBJECT *(*add) ( void *this, void *object );			\
	void (*top) ( void *this );					\
	void *(*next) ( void *this );					\
	void *(*find) ( void *this, u_char *key );			\
	void *(*select) (void *this, u_int which );			\
	int (*at_end) ( void *this );					\
	u_int (*tell) ( void *this );					\
	u_int (*count) ( void *this );
/* end OBJECT_SET_CLASS */

typedef struct object_set {
	OBJECT_SET_CLASS
} OBJECT_SET;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
OBJECT_SET *
new_OBJECT_SET( u_int maxelements );

void
destroy_OBJECT_SET( void *this );

#endif
