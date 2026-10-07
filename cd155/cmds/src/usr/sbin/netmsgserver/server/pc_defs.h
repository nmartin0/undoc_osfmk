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
 * PC_DEFS.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/pc_defs.h,v 1.2 1994/11/19 03:11:42 mtm Exp $
 *
 */

/*
 * Internal definitions for the port checkups module.
 */


#ifndef	_PC_DEFS_
#define	_PC_DEFS_

#include <mach.h>

#include "mem.h"
#include "disp_hdr.h"
#include "nm_defs.h"
#include "port_defs.h"
#include "srr_defs.h"
#include "timer.h"
#include "lock_queue.h"

#define PC_DEBUG		1


/*
 * Structure used for checkup requests/responses.
 * Note that we do not send the SID of a network port in a checkup request/reply.
 */

typedef struct {
	netaddr_t	pc_np_receiver;
	netaddr_t	pc_np_owner;
	np_uid_t	pc_np_puid;
} pc_network_port_t;

/*
 * PC_ASSIGN_NPORT
 *	Assign the relevant fields of a network port to a pc network port.
 *
 */
#define PC_ASSIGN_NPORT(pc_np,np) {			\
	(pc_np).pc_np_receiver = (np).np_receiver;	\
	(pc_np).pc_np_owner = (np).np_owner;		\
	(pc_np).pc_np_puid = (np).np_puid;		\
}

/*
 * PC_EXTRACT_NPORT
 *	Assign the relevant fields of a pc network port to a normal network port.
 *
 */
#define PC_EXTRACT_NPORT(np,pc_np) {			\
	(np).np_receiver = (pc_np).pc_np_receiver;	\
	(np).np_owner = (pc_np).pc_np_owner;		\
	(np).np_puid = (pc_np).pc_np_puid;		\
	(np).np_sid.np_uid_high = 0;			\
	(np).np_sid.np_uid_low = 0;			\
}

/*
 * Make sure that PC_MAX_ENTRIES is a multiple of 4 to avoid
 * headaches with the size of pc_status[].
 */
#define	PC_MAX_ENTRIES	 						\
	((((SRR_MAX_DATA_SIZE - sizeof(disp_hdr_t) - sizeof(long) - 3)	\
		/ (sizeof(char) + sizeof(pc_network_port_t))))		\
		& 0xfffffffc)

typedef struct {
	disp_hdr_t		pc_disp_hdr;
	unsigned long		pc_num_entries;
	unsigned char		pc_status[PC_MAX_ENTRIES];
	pc_network_port_t	pc_nports[PC_MAX_ENTRIES];
} portcheck_t, *portcheck_ptr_t;

/*
 * Structure to remember the requests that we are sending.
 */
typedef struct pc_host_list {
	struct pc_host_list	*next;
	netaddr_t		pchl_destination;
	long			pchl_client_id;
	portcheck_ptr_t		pchl_portcheck;
} pc_host_list_t, *pc_host_list_ptr_t;

/*
 * Status of ports in a checkup request/reply.
 */
#define	PORTCHECK_BLOCK		0x01
#define	PORTCHECK_DEAD		0x02
#define	PORTCHECK_NOTOK		0x04
#define	PORTCHECK_O_R_CHANGED	0x08

/*
 * Timer which is used to schedule checkups.
 */
extern struct timer		pc_timer;

/*
 * Counter to remember how many requests are outstanding.
 */
typedef struct{
	pthread_mutex_t	pc_lock;
	int		pc_counter;
} pc_count_t;

extern pc_count_t		pc_request_counter;

/*
 * The queue of requests.
 */
extern struct lock_queue	pc_request_queue;

/*
 * Routines public within the port checkups module.
 */
extern int pc_cleanup();

extern int pc_handle_checkup_request();

extern int pc_handle_checkup_reply();

extern int pc_handle_startup_hint();

extern void pc_send_startup_hint();

/*
 * Memory management definitions.
 */
extern mem_objrec_t		MEM_PCITEM;


#endif	_PC_DEFS_
