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
#ifndef _DIALOG_H
#define _DIALOG_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dialog.h
 Title:		DIALOG Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:00 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no.348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DIALOG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
#define	DIALOG_CLASS	OBJECT_CLASS					\
	u_int  prompted		:1,					\
	       default_valid	:1;					\
	hw_zipcode value;						\
	hw_zipcode default_value;					\
	char *prompt_str;						\
	u_char reply_str[128];						\
	hw_zipcode (*prompt)( void *this );				\
	hw_zipcode (*edit) ( void *this, u_char *str, int *str_valid );	\
	void (*reset) ( void *this );
/* end DIALOG_CLASS */
#else
#define	DIALOG_CLASS	OBJECT_CLASS					\
	u_int  prompted		:1,					\
	       default_valid	:1;					\
	u_long value;							\
	u_long default_value;						\
	char *prompt_str;						\
	u_char reply_str[128];						\
	u_long (*prompt)( void *this );					\
	u_long (*edit) ( void *this, u_char *str, int *str_valid );	\
	void (*reset) ( void *this );
/* end DIALOG_CLASS */
#endif

typedef struct dialog {
	DIALOG_CLASS
} DIALOG;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DIALOG *
new_DIALOG();

void
destroy_DIALOG( void *this );

#endif
