/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * delta.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/deltat.h,v 1.2 1994/11/19 03:10:41 mtm Exp $
 *
 */

/*
 * Public definitions for delta-t transport protocol.
 */


#ifndef	_DELTAT_
#define	_DELTAT_

#include <mach/boolean.h>

#include "transport.h"

/*
 * Delta-t specific failure codes.
 */
#define DELTAT_SUCCESS		TR_SUCCESS
#define DELTAT_ERROR_BASE	(-(TR_DELTAT_ENTRY * 16))
#define DELTAT_FAILURE		(1 + DELTAT_ERROR_BASE)
#define DELTAT_REMOTE_ABORT	(2 + DELTAT_ERROR_BASE)

/*
 * Exported functions.
 */
extern boolean_t deltat_init();
extern int deltat_retry();
#if	RPCMOD
extern int deltat_sendrequest();
#else	RPCMOD
extern int deltat_send();
#endif	RPCMOD

#endif	_DELTAT_
