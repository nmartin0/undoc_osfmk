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
 * km_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/km_defs.h,v 1.2 1994/11/19 03:11:18 mtm Exp $
 *
 */

/*
 * Internal definitions for the Key Management module.
 */


#ifndef	_KM_DEFS_
#define	_KM_DEFS_

#include "key_defs.h"
#include "nm_defs.h"
#include "timer.h"

typedef struct {
    netaddr_t	kr_host_id;
    key_t	kr_key;
    key_t	kr_mpkey;
    key_t	kr_mpikey;
} key_rec_t, *key_rec_ptr_t;

#define KEY_REC_NULL	((key_rec_ptr_t)0)

/*
 * Definition of queue holding pending key exchange requests.
 */
typedef struct kmq_entry {
    struct kmq_entry	*next;
    struct timer	kmq_timer;
    netaddr_t		kmq_host_id;
    int			kmq_client_id;
    int			(*kmq_client_retry)();
} kmq_entry_t, *kmq_entry_ptr_t;


#define KM_RETRY_INTERVAL	60


/*
 * External definitions for functions implemented by km_utils.c
 */

extern key_rec_ptr_t km_host_enter();
/*
netaddr_t	host_id;
*/

extern key_rec_ptr_t km_host_lookup();
/*
netaddr_t	host_id;
*/

extern void km_utils_init();
/*
*/

extern void km_procs_init();
/*
*/

#endif	_KM_DEFS_
