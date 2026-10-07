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
 * po_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/po_defs.h,v 1.2 1994/11/19 03:11:45 mtm Exp $
 *
 */

/*
 * Internal definitions for the Port Operations module.
 */


#ifndef	_PO_DEFS_
#define	_PO_DEFS_

#include <mach/boolean.h>

#include "mem.h"
#include "disp_hdr.h"
#include "key_defs.h"
#include "nm_defs.h"
#include "port_defs.h"

#define PO_DEBUG	1

/*
 * Structures used for remembering about transfer of access rights etc.
 */
typedef struct po_queue {
    struct po_queue	*link;
    int			poq_client_id;
    port_t		poq_lport;
    int			poq_right;
    int			poq_security_level;
} po_queue_t, *po_queue_ptr_t;

/*
 * Structure used to send port data over the network.
 */
typedef struct {
    unsigned short	pod_size;
    unsigned short	pod_right;
    network_port_t	pod_nport;
    secure_info_t	pod_sinfo;
    long		pod_extra;
} po_data_t, *po_data_ptr_t;

/*
 * Structure used to send port operations messages over the network.
 */
typedef struct po_message {
    disp_hdr_t		pom_disp_hdr;
    po_data_t		pom_po_data;
} po_message_t, *po_message_ptr_t;

/*
 * Structure used to remember what information a host has for a port.
 */
typedef struct po_host_info {
    struct po_host_info	*phi_next;
    netaddr_t		phi_host_id;
    boolean_t		phi_sent_token;
    long		phi_ipc_seq_no;
} po_host_info_t, *po_host_info_ptr_t;

#define PO_HOST_INFO_NULL	(po_host_info_ptr_t)0


/*
 * External definitions for functions implemented
 * by po_handler.c, po_notify.c and po_utils.c.
 */

extern po_handle_token_reply();
/*
int		client_id;
sbuf_ptr_t	reply;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern po_handle_token_request();
/*
sbuf_ptr_t	request;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern void po_request_token();
/*
port_rec_ptr_t	port_rec_ptr;
netaddr_t	source;
int		security_level;
*/


extern po_handle_ro_xfer_hint();
/*
int		client_id;
sbuf_ptr_t	data;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern void po_send_ro_xfer_hint();
/*
port_rec_ptr_t		port_rec_ptr;
netaddr_t		destination;
*/


extern po_handle_nport_death();
/*
int		client_id;
sbuf_ptr_t	data;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/


extern po_handle_ro_xfer_reply();
/*
int		client_id;
sbuf_ptr_t	reply;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern po_handle_ro_xfer_request();
/*
sbuf_ptr_t	request;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern po_xfer_ownership();
/*
port_rec_ptr_t	port_rec_ptr;
*/

extern po_xfer_receive();
/*
port_rec_ptr_t	port_rec_ptr;
*/


extern void po_notify_init();


extern boolean_t po_check_ro_key();
/*
port_rec_ptr_t		port_rec_ptr;
secure_info_ptr_t	secure_info_ptr
*/

extern void po_create_ro_key();
/*
port_rec_ptr_t		port_rec_ptr;
*/


extern boolean_t po_utils_init();
/*
*/

/*
 * Memory management definitions.
 */
extern mem_objrec_t		MEM_POITEM;


#endif	_PO_DEFS_
