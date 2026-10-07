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
#ifndef _OBJECT_H
#define _OBJECT_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		object.h
 Title:		OBJECT Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:19 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities, FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the OBJECT class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include <stdio.h>

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	OBJECT_CLASS							\
	u_char keyval[32];						\
	char *description;						\
	FILE *describe_fp;						\
	u_int								\
	is_a_nop_obj:	1,						\
	arg_needed:	1;						\
	void (*destroy) ( void *this );					\
	void (*describe) ( void *this );				\
	void (*redirect) ( void *this, FILE *fp );			\
	u_char *(*key) ( void *this );					\
	u_int (*needs_arg) ( void *this );				\
	u_int (*is_a_nop) ( void *this );				\
	hw_zipcode (*hw_address) ( void *this );
/* end OBJECT_CLASS */

typedef struct object {
	OBJECT_CLASS
} OBJECT;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
OBJECT *
new_OBJECT();

void
destroy_OBJECT( void *this );

#endif
