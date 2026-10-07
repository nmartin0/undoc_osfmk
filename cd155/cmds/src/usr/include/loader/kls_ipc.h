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


/********************** Client IPC Interfaces **********************/

extern int
kls_client_ipc_connect_to_server(void);

extern int
kls_client_ipc_disconnect_from_server(void);

extern int
kls_client_ipc_send_request(kls_request_header_t *);

extern int
kls_client_ipc_receive_reply(kls_reply_header_t **);


#ifdef	_KLS_IPC_SOCK_DGRAM

#define	SERVER_SOCKET_NAME	"/dev/kloadsrv"

#else	/* _KLS_IPC_SOCK_DGRAM */

#ifdef	_KLS_IPC_SYSV_MSG

#define	SERVER_FTOK_PATH	KLS_SERVER_PATHNAME
#define	SERVER_FTOK_ID	'A'

#endif	/* _KLS_IPC_SYSV_MSG */
#endif	/* _KLS_IPC_SOCK_DGRAM */
