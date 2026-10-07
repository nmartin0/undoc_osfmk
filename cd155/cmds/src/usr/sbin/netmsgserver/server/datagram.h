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
 * datagram.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/datagram.h,v 1.2 1994/11/19 03:10:36 mtm Exp $
 *
 */

/*
 * Definitions for the datagram transport protocol.
 */


#ifndef	_DATAGRAM_
#define	_DATAGRAM_

#include <mach/boolean.h>
#include "transport.h"

/*
 * Datagram specific failure codes.
 */
#define DATAGRAM_ERROR_BASE		(-(TR_DATAGRAM_ENTRY * 16))
#define DATAGRAM_TOO_LARGE		(1 + DATAGRAM_ERROR_BASE)
#define DATAGRAM_SEND_FAILURE		(2 + DATAGRAM_ERROR_BASE)

/*
 * The maximum amount of data that can be placed in a datagram.
 */
extern int datagram_max_data_size;

/*
 * Exported functions.
 */
extern boolean_t datagram_init();
extern int datagram_send();

#endif	_DATAGRAM_
