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
#ifndef _SCSI_DEV_H
#define _SCSI_DEV_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		scsi_dev.h
 Title:		SCSI_DEV Class Definition
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:27 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 7/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the SCSI_DEV class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"
#include "sysio.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	SCSI_DEV_CLASS	OBJECT_CLASS					\
	hw_zipcode hw_addr;						\
	SYSIO *sysio;							\
	int (*inquiry) ( void *this, u_char *data, u_int byte_count );	\
	int (*read) ( void *this, u_char *data,				\
			u_long byte_count, u_long block_addr,		\
			u_short block_count );				\
	int (*write) ( void *this, u_char *data,			\
			u_long byte_count, u_long block_addr,		\
			u_short block_count );				\
	int (*write_buffer) ( void *this, u_char *data,			\
			u_int byte_count, u_int offset, u_int keep );	\
	int (*mode_sense) ( void *this, u_char *data,			\
			    u_int byte_count, u_char pg_ctrl,		\
			    u_char pg_code );				\
	int (*mode_select) ( void *this, u_char *data,			\
			u_int byte_count, u_int pg_format, u_int sp );	\
	int (*send_diag) ( void *this, u_char *data,			\
			   u_int byte_count, u_int pg_format );		\
	int (*receive_diag) ( void *this, u_char *data,			\
			      u_int byte_count );			\
	/*******************(new stuff starts here)*******************/ \
	int (*request_sense) ( void *this, u_char *data,		\
			      u_int byte_count, u_int aen );		\
	int (*log_select) ( void *this, u_char *data,			\
			    u_int byte_count, u_int pcr, u_int sp,	\
			    u_char pg_ctrl );				\
	int (*log_sense) ( void *this, u_char *data,			\
		       	   u_int byte_count, u_int ppc, u_int sp,	\
			   u_char pg_ctrl );				\
	/******************* Added 901220 by SJR *********************/ \
	int (*lock) ( void *this );					\
	int (*unlock) ( void *this );
/* end SCSI_DEV_CLASS */

typedef struct scsi_dev {
	SCSI_DEV_CLASS
} SCSI_DEV;

/******************************************************************************
 ***				  MACROS				    ***
 *****************************************************************************/
#define get_LUN(hw_addr)	((hw_zipcode_t *) &hw_addr)->LUN

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
SCSI_DEV *
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
new_SCSI_DEV( hw_zipcode hw_addr, int exclusive );
#else
new_SCSI_DEV( u_long hw_addr, int exclusive );
#endif

void
destroy_SCSI_DEV( void *this );

#endif
