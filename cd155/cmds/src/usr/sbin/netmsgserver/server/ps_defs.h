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
 * ps_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/ps_defs.h,v 1.2 1994/11/19 03:12:04 mtm Exp $
 *
 */

/*
 * Internal definitions for the Port Search module.
 */


#ifndef	_PS_DEFS_
#define	_PS_DEFS_

#define PS_DEBUG		1

#include <mach/port.h>

#include "mem.h"
#include "disp_hdr.h"
#include "key_defs.h"
#include "nm_defs.h"
#include "port_defs.h"

/*
 * Structure used to send and reply to port search requests.
 */
typedef struct {
    disp_hdr_t		psd_disp_hdr;
    unsigned long	psd_status;
    np_uid_t		psd_puid;
    netaddr_t		psd_owner;
    netaddr_t		psd_receiver;
} ps_data_t, *ps_data_ptr_t;

/*
 * Status values.
 */
#define PS_OWNER_MOVED		1
#define PS_RECEIVER_MOVED	2
#define PS_PORT_DEAD		4
#define PS_PORT_UNKNOWN		8
#define PS_PORT_HERE		16


/*
 * Structure used to send and reply to authentication requests.
 */
typedef struct {
    disp_hdr_t		psa_disp_hdr;
    np_uid_t		psa_puid;
    secure_info_t	psa_token;
    long		psa_random;
} ps_auth_t, *ps_auth_ptr_t;


/*
 * Structure used to remember about port searches in progress.
 */
typedef struct {
    int		pse_state;
    netaddr_t	pse_destination;
    port_t	pse_lport;
    int		(*pse_retry)();
} ps_event_t, *ps_event_ptr_t;

/*
 * Possible event states.
 */
#define PS_OWNER_QUERIED	1
#define PS_RECEIVER_QUERIED	2
#define PS_DONE_BROADCAST	4
#define PS_AUTHENTICATION	8


/*
 * extern definitions internal to the Port Search module.
 */
extern ps_cleanup();
/*
ps_event_ptr_t	event_ptr;
int		reason;
*/


extern ps_handle_auth_reply();
/*
int		client_id;
sbuf_ptr_t	reply;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern ps_handle_auth_request();
/*
sbuf_ptr_t	request;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/


extern ps_retry();
/*
ps_event_ptr_t	event_ptr;
*/

extern void ps_send_query();
/*
ps_event_ptr_t	event_ptr;
port_rec_ptr_t	port_rec_ptr;
*/

extern void ps_authenticate_port();
/*
ps_event_ptr_t	event_ptr;
port_rec_ptr_t	port_rec_ptr;
*/

/*
 * Memory management definitions.
 */
extern mem_objrec_t		MEM_PSEVENT;


#endif	_PS_DEFS_
