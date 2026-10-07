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
#ifndef _DAC_PARAM_H
#define _DAC_PARAM_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dac_param.h
 Title:		DAC_PARAM Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:53 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the DAC_PARAM class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "dialog.h"
#include "scsi_dev.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
#define	DAC_PARAM_CLASS	OBJECT_CLASS					\
        u_int  prompted         :1,                                     \
               default_valid    :1;                                     \
        hw_zipcode value;                                               \
        hw_zipcode default_value;                                       \
        char *prompt_str;                                               \
        u_char reply_str[128];                                          \
        u_long (*prompt)( void *this );                             \
        u_long (*edit) ( void *this, u_char *str, int *str_valid ); \
        void (*reset) ( void *this );					\
	char *display_text;						\
	hw_zipcode hw_addr;						\
	SCSI_DEV *sd;							\
	u_long	lower_bound;						\
	u_long	upper_bound;						\
	int	unchangeable;						\
	int (*fmt_required) ( void *this );				\
	int (*set) ( void *this, u_long val );				\
	int (*read) ( void *this, u_long *val );			\
	int (*display) ( void *this );					\
	int (*display_value) ( void *this );				\
	int (*lu_exists) (void *this );					\
	int (*get_page) ( void *this, u_char *page, u_int pg_size,	\
			  u_char pg_code );
/* end DAC_PARAM_CLASS */
#else
#define	DAC_PARAM_CLASS	DIALOG_CLASS					\
	char *display_text;						\
	hw_zipcode hw_addr;						\
	SCSI_DEV *sd;							\
	u_long	lower_bound;						\
	u_long	upper_bound;						\
	int	unchangeable;						\
	int (*fmt_required) ( void *this );				\
	int (*set) ( void *this, u_long val );				\
	int (*read) ( void *this, u_long *val );			\
	int (*display) ( void *this );					\
	int (*display_value) ( void *this );				\
	int (*lu_exists) (void *this );					\
	int (*get_page) ( void *this, u_char *page, u_int pg_size,	\
			  u_char pg_code );
/* end DAC_PARAM_CLASS */
#endif
typedef struct dac_param {
	DAC_PARAM_CLASS
} DAC_PARAM;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
DAC_PARAM *
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
new_DAC_PARAM( hw_zipcode hw_addr );
#else
new_DAC_PARAM( u_long hw_addr );
#endif

void
destroy_DAC_PARAM( void *this );

#endif
