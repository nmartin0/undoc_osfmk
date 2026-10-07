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
 * keyman.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/keyman.h,v 1.2 1994/11/19 03:11:16 mtm Exp $
 *
 */

/*
 * External definitions for Key Management module.
 */


#ifndef	_KEYMAN_
#define	_KEYMAN_

#include <mach/boolean.h>

#include "nm_defs.h"

#define KM_SERVICE_NAME	"NETWORK_SERVER_KEY_MANAGER"

/*
 * Error Codes.
 */
#define KM_SUCCESS	0
#define KM_FAILURE	-1

extern boolean_t km_init();
/*
*/

extern boolean_t km_get_key();
/*
netaddr_t	host_id;
key_ptr_t	key_ptr;
*/

extern boolean_t km_get_dkey();
/*
netaddr_t	host_id;
key_ptr_t	ikey_ptr;
*/

extern void km_do_key_exchange();
/*
int		client_id;
int		(*client_retry)();
netaddr_t	host_id;
*/

#endif	_KEYMAN_
