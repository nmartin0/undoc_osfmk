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
 * OSF/1 Release 1.0
 */

/*
 * Four IPC Facilities Available			
 *
 *	Stream Sockets		(kls_ipc_socket_stream.c)
 *	Datagram Sockets	(kls_ipc_socket_dgram.c)
 *	System V Messages	(kls_ipc_sysV_msg.c)
 *	Mach IPC		(kls_ipc_mach.c)
 */


/********************** Server IPC Interfaces **********************/

extern int
kls_server_ipc_initialize(void);

extern void
kls_server_ipc_terminate(void);

extern int
kls_server_ipc_receive_request(kls_request_header_t **);

extern void
kls_server_ipc_send_reply(kls_request_header_t *, kls_reply_header_t *);
