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
#ifndef _MENU_H
#define _MENU_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		menu.h
 Title:		MENU Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:14 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object_Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the MENU class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dialog.h"
#include "object_set.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
#define	MENU_CLASS	OBJECT_CLASS					\
        u_int  prompted         :1,                                     \
               default_valid    :1;                                     \
        hw_zipcode value;                                               \
        hw_zipcode default_value;                                       \
        char *prompt_str;                                               \
        u_char reply_str[128];                                          \
        u_long (*prompt)( void *this );                                 \
        u_long (*edit) ( void *this, u_char *str, int *str_valid ); 	\
        void (*reset) ( void *this );                                   \
	u_int	opts_found	:1,					\
		is_level_zero	:1;					\
	OBJECT_SET *actions;						\
	u_int (*add_selection) ( void *this, void *object );
/* end MENU_CLASS */
#else
#define	MENU_CLASS	DIALOG_CLASS					\
	u_int	opts_found	:1,					\
		is_level_zero	:1;					\
	OBJECT_SET *actions;						\
	u_int (*add_selection) ( void *this, void *object );
/* end MENU_CLASS */
#endif

typedef struct menu {
	MENU_CLASS
} MENU;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
MENU *
new_MENU(); 

void
destroy_MENU( void *this );

#endif
