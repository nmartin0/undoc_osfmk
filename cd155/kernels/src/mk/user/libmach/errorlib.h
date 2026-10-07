/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: errorlib.h,v $
 * Revision 1.5  1994/11/18  21:04:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:03:03  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:57:19  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:03:04  dleslie
 * First R1_0 release
 *
 * Revision 2.2.4.1  92/03/28  10:20:10  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:26:58  jeffreyh]
 * 
 * Revision 2.2  92/01/16  00:21:17  rpd
 * 	Moved from user collection to mk collection.
 * 
 * Revision 2.2  91/03/27  15:37:37  mrt
 * 	First checkin
 * 
 */
/*
 *	File:	errorlib.h
 *	Author:	Douglas Orr, Carnegie Mellon University
 *	Date:	Mar. 1988
 *
 *	Error bases for subsytems errors.
 */

#include <mach/error.h>

#define	MACH_IPC_SEND_MOD	(err_mach_ipc|err_sub(0))
#define	MACH_IPC_RCV_MOD	(err_mach_ipc|err_sub(1))
#define	MACH_IPC_MIG_MOD	(err_mach_ipc|err_sub(2))

#define	IPC_SEND_MOD		(err_ipc|err_sub(0))
#define	IPC_RCV_MOD		(err_ipc|err_sub(1))
#define	IPC_MIG_MOD		(err_ipc|err_sub(2))

#define	SERV_NETNAME_MOD	(err_server|err_sub(0))
#define	SERV_ENV_MOD		(err_server|err_sub(1))
#define	SERV_EXECD_MOD		(err_server|err_sub(2))


#define	NO_SUCH_ERROR		"unknown error code"

struct error_subsystem {
	char			* subsys_name;
	int			max_code;
	char			* * codes;
};

struct error_system {
	int			max_sub;
	char			* bad_sub;
	struct error_subsystem	* subsystem;
};

extern	struct error_system 	errors[err_max_system+1];

#define	errlib_count(s)		(sizeof(s)/sizeof(s[0]))
