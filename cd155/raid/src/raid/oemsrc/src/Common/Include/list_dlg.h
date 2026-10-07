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
#ifndef _LIST_DLG_H
#define _LIST_DLG_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		list_dlg.h
 Title:		LIST_DLG Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:08 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the LIST_DLG class.
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
#define	LIST_DLG_CLASS	OBJECT_CLASS					\
        u_int  prompted         :1,                                     \
               default_valid    :1;                                     \
        hw_zipcode value;                                               \
        hw_zipcode default_value;                                       \
        char *prompt_str;                                               \
        u_char reply_str[128];                                          \
        hw_zipcode (*prompt)( void *this );                         \
        hw_zipcode (*edit) ( void *this, u_char *str, int *str_valid ); \
        void (*reset) ( void *this );					\
	u_int null_reply;						\
	u_char *running_ptr;						\
	u_long (*list_complete) (void *this);
/* end LIST_DLG_CLASS */
#else
#define	LIST_DLG_CLASS	DIALOG_CLASS					\
	u_int null_reply;						\
	u_char *running_ptr;						\
	u_long (*list_complete) (void *this);
/* end LIST_DLG_CLASS */
#endif

typedef struct list_dlg {
	LIST_DLG_CLASS
} LIST_DLG;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
LIST_DLG *
new_LIST_DLG();

void
destroy_LIST_DLG( void *this );

#endif
