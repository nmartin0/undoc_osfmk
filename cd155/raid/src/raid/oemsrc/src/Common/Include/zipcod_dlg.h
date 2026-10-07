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
#ifndef _ZIPCOD_DLG_H
#define _ZIPCOD_DLG_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		zipcod_dlg.h
 Title:		ZIPCOD_DLG Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:47 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the ZIPCOD_DLG class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dialog.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
#define	ZIPCOD_DLG_CLASS	OBJECT_CLASS				\
        u_int  prompted         :1,                                     \
               default_valid    :1;                                     \
        hw_zipcode value;                                               \
        hw_zipcode default_value;                                       \
        char *prompt_str;                                               \
        u_char reply_str[128];                                          \
        hw_zipcode (*prompt)( void *this );                         \
        hw_zipcode (*edit) ( void *this, u_char *str, int *str_valid );     \
        void (*reset) ( void *this );					\
	hw_zipcode prompt_mask;
/* end ZIPCOD_DLG_CLASS */
#else
#define	ZIPCOD_DLG_CLASS	DIALOG_CLASS				\
	hw_zipcode prompt_mask;
/* end ZIPCOD_DLG_CLASS */
#endif

typedef struct zipcod_dlg {
	ZIPCOD_DLG_CLASS
} ZIPCOD_DLG;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
ZIPCOD_DLG *
new_ZIPCOD_DLG( hw_zipcode prompt_mask );

void
destroy_ZIPCOD_DLG( void *this );

#endif
