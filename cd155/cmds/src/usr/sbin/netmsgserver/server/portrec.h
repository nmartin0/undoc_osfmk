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
 * portrec.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/portrec.h,v 1.2 1994/11/19 03:11:58 mtm Exp $
 *
 */

/*
 * External definitions for the Port Records module.
 */


#ifndef	_PORTREC_
#define	_PORT_REC_

#include <mach/boolean.h>

#include "lock_queue.h"
#include "lock_queue_macros.h"
#include "port_defs.h"

/*
 * The null network port.
 */
extern network_port_t		null_network_port;


/*
 * Functions exported.
 */

extern boolean_t pr_init();
/*
*/

extern void pr_destroy();
/*
port_rec_ptr_t		port_rec_ptr;
*/

extern port_rec_ptr_t pr_np_puid_lookup();
/*
np_uid_t		np_puid;
*/

extern port_rec_ptr_t pr_nportlookup();
/*
network_port_ptr_t	nport_ptr;
*/

extern port_rec_ptr_t pr_ntran();
/*
network_port_ptr_t	nport_ptr;
*/

extern port_rec_ptr_t pr_lportlookup();
/*
port_t			lport;
*/

extern port_rec_ptr_t pr_ltran();
/*
port_t			lport;
*/

extern boolean_t pr_nport_equal();
/*
network_port_ptr_t	nport_ptr_1, nport_ptr_2;
*/

extern void pr_nporttostring();
/*
char			*nport_str;
network_port_ptr_t	nport_ptr;
*/

extern lock_queue_t pr_list();
/*
*/


#endif	_PORT_REC_
