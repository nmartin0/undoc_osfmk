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
 * portops.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/portops.h,v 1.2 1994/11/19 03:11:56 mtm Exp $
 *
 */

/*
 * Definitions for the Port Operations module.
 */


#ifndef _PORTOPS_
#define _PORTOPS_

/*
 * Sizes used by po_translate_[ln]port_rights.
 */
#define PO_MAX_NPD_ENTRY_SIZE	48
#define PO_MIN_NPD_ENTRY_SIZE	28
#define PO_NPD_SEG_SIZE		256

/*
 * Completion codes passed to po_port_rights_commit.
 */
#define PO_RIGHTS_XFER_SUCCESS	0
#define PO_RIGHTS_XFER_FAILURE	1


/*
 * Functions exported by the port operations module.
 */
#include <mach/boolean.h>

extern boolean_t po_init();
/*
*/

extern boolean_t po_check_ipc_seq_no();
/*
port_rec_ptr_t	portrec_ptr;
netaddr_t	host_id;
long		ipc_seq_no;
*/

extern long po_create_token();
/*
port_rec_ptr_t		port_rec_ptr;
secure_info_ptr_t	token_ptr;
*/

extern void po_notify_port_death();
/*
port_rec_ptr_t	port_rec_ptr;
*/

extern void po_port_deallocate();
/*
port_t			lport;
*/

extern int po_translate_lport_rights();
/*
int		client_id;
port_t		lport;
int		right;
int		security_level;
netaddr_t	destination_hint;
pointer_t	port_data;	To be sent to the remote network server.
*/

extern void po_port_rights_commit();
/*
int		client_id;
int		completion_code;
netaddr_t	destination;
*/

extern int po_translate_nport_rights();
/*
netaddr_t	source;
pointer_t	port_data;	Received from the remote network server.
int		security_level;
port_t		*lport;
int		*right;
*/

#endif _PORTOPS_
