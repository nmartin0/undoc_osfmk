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
#ifndef _FNM_DIALOG_H
#define _FNM_DIALOG_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		fnm_dialog.h
 Title:		FNM_DIALOG Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:06 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the FNM_DIALOG class.
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
#define	FNM_DIALOG_CLASS	OBJECT_CLASS                            \
        u_int  prompted         :1,                                     \
               default_valid    :1;                                     \
        hw_zipcode value;                                               \
        hw_zipcode default_value;                                       \
        char *prompt_str;                                               \
        u_char reply_str[128];                                          \
        u_long (*prompt)( void *this );                                 \
        u_long (*edit) ( void *this, u_char *str, int *str_valid ); \
        void (*reset) ( void *this );					\
	int create_if_nonexistent;
/* end FNM_DIALOG_CLASS */
#else
#define	FNM_DIALOG_CLASS	DIALOG_CLASS				\
	int create_if_nonexistent;
/* end FNM_DIALOG_CLASS */
#endif

typedef struct fnm_dialog {
	FNM_DIALOG_CLASS
} FNM_DIALOG;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
FNM_DIALOG *
new_FNM_DIALOG( FILE *default_fp, int create );

void
destroy_FNM_DIALOG( void *this );

#endif
