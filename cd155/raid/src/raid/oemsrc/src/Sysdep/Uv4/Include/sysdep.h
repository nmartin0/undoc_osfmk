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
 
#ifndef _SYSDEP_H
#define _SYSDEP_H
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		sysdep.h
 Title:		System-Dependent Definitions
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:20 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains system-dependent definitions for the
		disk array utility modules.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <sys/types.h>
#include <sys/scsi/cs.h> 
#include "sys/scsi/SCSI.h" 

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
#undef PF			/* how is this getting defined??? */
#ifdef STATIC_KLUDGE
#define	static	extern		/* (interim fix for SCO compiler bug) */
#endif

#ifdef MSC60
/* UNIX V typedefs */
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
#endif

typedef unsigned long	u_int32;	/* to be used for 32-bit bitfields */

#define	LOGDIR		"/var/array/logs"/* location of array daemon logs */
#define	STDCFDIR	"/etc/default"	/* location of array daemon logs */

#define SINGLE_USER_MODE	255	/* constant used with runlev.c */

#endif /* _SYSDEP_H */
