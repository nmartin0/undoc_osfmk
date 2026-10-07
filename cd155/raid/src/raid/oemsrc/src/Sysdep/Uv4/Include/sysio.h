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
 
#ifndef _SYSIO_H
#define _SYSIO_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		sysio.h
 Title:		SYSIO Class Definition
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:23 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module defines the SYSIO class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "object.h"
#include "cdb_struct.h"
#include "sys/scsi/cs.h"
#ifdef	NOT_PRESENT
#undef	NOT_PRESENT
#endif
#include "sys/scsi/SCSI.h"
#include <sys/libio.h>
#include <fcntl.h>

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#define	SYSIO_CLASS	OBJECT_CLASS					\
	hw_zipcode hw_addr;						\
	u_char *sense_data;						\
	int (*controller_command) ( void *this, u_char *cdb );		\
	int (*lock) ( void *this, hw_zipcode hw_addr );			\
	int (*unlock) ( void *this, hw_zipcode hw_addr );		\
	int (*is_root) ( void *this );
/* end SYSIO_CLASS */

typedef struct sysio {
	SYSIO_CLASS
} SYSIO;

static u_char	IO_Direction[] = {
				1, 1, 1, 3, 2, 3, 2, 2, 3, 1, 2, 1, 1, 1, 1, 3,
	/* 1 = IO_NONE   */	1, 1, 3, 1, 3, 2, 1, 1, 2, 1, 3, 1, 3, 2, 1, 1,
	/* 2 = IO_MEM_TO_DEV */	1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 2, 1, 1, 1, 2, 1,
	/* 3 = IO_DEV_TO_MEM */	2, 2, 2, 1, 1, 1, 1, 3, 1, 3, 3, 2, 3, 1, 1, 1,
				1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 3 };


typedef struct fd_entry {
	int		allocated_slot;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
	hw_zipcode	hw_addr;
#else
	u_long		hw_addr;
#endif
	int		fd;
	int		excl;
	char		dev_path[30];
} fd_entry_t ;

#define FD_TABLE_SIZE	32

typedef struct fd_table {
	u_int 		count;
	fd_entry_t	entry[FD_TABLE_SIZE];
} fd_table_t;

fd_table_t	fdtab;
/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
SYSIO *
new_SYSIO( hw_zipcode hw_addr, int exclusive );

void
destroy_SYSIO( void *this );

int
convert_maj_min( hw_zipcode hw_addr, char *cbuf );

#endif
