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
 * srr.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/srr.h,v 1.2 1994/11/19 03:12:15 mtm Exp $
 *
 */

/*
 * Definitions for the Simple request-response transport protocol.
 */


#ifndef	_SRR_
#define	_SRR_

#include <mach/boolean.h>

#include "transport.h"

/*
 * srr specific failure codes.
 */
#define SRR_SUCCESS		(0)
#define SRR_ERROR_BASE		(-(TR_SRR_ENTRY * 16))
#define SRR_TOO_LARGE		(1 + SRR_ERROR_BASE)
#define SRR_FAILURE		(2 + SRR_ERROR_BASE)
#define SRR_ENCRYPT_FAILURE	(3 + SRR_ERROR_BASE)


/*
 * The maximum amount of data that can be placed in a request or a response.
 */
extern int srr_max_data_size;

/*
 * Exported functions.
 */
extern boolean_t srr_init();
extern int srr_send();

#endif	_SRR_
