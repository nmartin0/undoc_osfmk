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
 * port_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/port_defs.h,v 1.2 1994/11/19 03:11:50 mtm Exp $
 *
 */

/*
 * Definitions of network port structures and associated constants.
 */


#ifndef	_PORT_DEFS_
#define	_PORT_DEFS_

#include <mach_types.h>

#include "key_defs.h"
#include "ls_defs.h"
#include "nm_defs.h"
#include "rwlock.h"
#include "mem.h"

#include "sys_queue.h"

/*
 * np_uid_t and network_port_t are declared with the kernel files,
 * in sys/mach_ipc_netport.h.
 */
#ifdef	notdef
/*
 * Network Port structure.
 */
typedef struct {
    long	np_uid_high;
    long	np_uid_low;
} np_uid_t;

typedef struct {
    netaddr_t	np_receiver;
    netaddr_t	np_owner;
    np_uid_t	np_puid;
    np_uid_t	np_sid;
} network_port_t;
#endif	notdef

typedef network_port_t	*network_port_ptr_t;

/*
 * Macros to determine the rights that we have to a port.
 */
#define NPORT_HAVE_SEND_RIGHTS(nport) \
    (((nport).np_receiver != my_host_id) && ((nport).np_owner != my_host_id))
#define NPORT_HAVE_REC_RIGHTS(nport) \
    ((nport).np_receiver == my_host_id)
#define NPORT_HAVE_RO_RIGHTS(nport) \
    (((nport).np_receiver == my_host_id) || ((nport).np_owner == my_host_id))
#define NPORT_HAVE_ALL_RIGHTS(nport) \
    (((nport).np_receiver == my_host_id) && ((nport).np_owner == my_host_id))

/*
 * Macro to test for network port equality.
 */
#define NPORT_EQUAL(nport1,nport2) (					\
	((nport1).np_puid.np_uid_high == (nport2).np_puid.np_uid_high)	\
	&& ((nport1).np_puid.np_uid_low == (nport2).np_puid.np_uid_low)	\
	&& ((nport1).np_sid.np_uid_low == (nport2).np_sid.np_uid_low)	\
	&& ((nport1).np_sid.np_uid_low == (nport2).np_sid.np_uid_low))


/*
 * Stucture used by pr_list.
 */
typedef struct port_item {
	struct port_item	*next;
	port_t			pi_port;
} port_item_t, *port_item_ptr_t;


/*
 * Information maintained about ports.
 */
typedef struct pq_hash {
    struct pq_hash	*next;
    struct port_rec_	*pqh_portrec;
} pq_hash_t, *pq_hash_ptr_t;

typedef struct port_rec_{
    pq_hash_t		portrec_localitem;
    pq_hash_t		portrec_networkitem;
#if	RPCMOD
    int			portrec_refcount;
#endif	RPCMOD
    int			portrec_info;
    int			portrec_port_rights;
    port_t		portrec_local_port;
    network_port_t	portrec_network_port;
    secure_info_t	portrec_secure_info;
    long		portrec_random;
    long		portrec_clock;
    pointer_t		portrec_po_host_list;	/* List of network servers for port ops module. */
    short		portrec_security_level;
    short		portrec_aliveness;
#if	RPCMOD
    long		portrec_retry_level;
    long		portrec_waiting_count;
    long		portrec_transit_count;
    sys_queue_head_t	portrec_out_ipcrec;
    pointer_t		portrec_lazy_ipcrec;
    pointer_t		portrec_reply_ipcrec;
#else	RPCMOD
    pointer_t		portrec_current_ipcrec;
    pointer_t		portrec_backlog_ipcrec;
#endif	RPCMOD
    pointer_t		portrec_block_queue;	/* List of senders waiting if the port is blocked. */
    struct lock		portrec_lock;
    port_stat_ptr_t	portrec_stat;
} port_rec_t, *port_rec_ptr_t;

#define PORT_REC_NULL	(port_rec_ptr_t)0
#define PORT_REC_OWNER(port_rec_ptr) ((port_rec_ptr)->portrec_network_port.np_owner)
#define PORT_REC_RECEIVER(port_rec_ptr) ((port_rec_ptr)->portrec_network_port.np_receiver)


/*
 *  Values of portrec_info field.
 */
#define	PORT_INFO_SUSPENDED	0x1
#define PORT_INFO_DISABLED	0x2
#define PORT_INFO_BLOCKED	0x4
#define	PORT_INFO_PROBED	0x8
#define PORT_INFO_DEAD		0x10
#define PORT_INFO_ACTIVE	0x20
#define	PORT_INFO_NOLOOKUP	0x40

#define	PORT_BUSY(port_rec_ptr) (				\
	((port_rec_ptr->portrec_info) & 			\
		(PORT_INFO_SUSPENDED | PORT_INFO_BLOCKED)) ||	\
	(port_rec_ptr->portrec_transit_count > 0))		\


/*
 * Values of portrec_aliveness field.
 */
#define PORT_ACTIVE		2
#define PORT_INACTIVE		0

/*
 * Values for port access rights.
 * These are the same as the values in /usr/mach/include/mach/message.h
 */
#define PORT_OWNERSHIP_RIGHTS	3
#define PORT_RECEIVE_RIGHTS	4
#define PORT_ALL_RIGHTS		5
#define PORT_SEND_RIGHTS	6


#if	RPCMOD
/*
 * Port reference counts.
 *
 * Any entity that intends to use a port record after having released
 * the lock on it must make a reference to it before releasing that lock.
 * There is normally a single reference for all the lookup queues, one
 * reference for each ipc_rec pending on the destination port, and one
 * reference for the reply_ipcrec, if any (for simplicity, that last
 * reference is kept until the ipc_rec is deallocated, even if the ipc_rec
 * is unlinked from the port record long before that).
 *
 * The port lookup procedures do not create any extra references, but use
 * the one made when the port is created. This is acceptable since they
 * keep the port record locked on exit. The port record is protected from
 * early removal from the queues by holding the queue lock(s) while acquiring
 * the port record lock, and by checking for the PORT_INFO_NOLOOKUP flag when
 * this is not possible. To avoid deadlocks, all procedures must acquire queue
 * locks before port locks (and only one port lock should ever be held at
 * a time).
 */
#define	pr_reference(port_rec_ptr) {					\
	/* port_rec_ptr LOCK RW/RW */					\
	port_rec_ptr->portrec_refcount++;				\
	/* port_rec_ptr LOCK RW/RW */					\
}

#define	pr_release(port_rec_ptr) {					\
	/* port_rec_ptr LOCK RW/RW */					\
	if (--port_rec_ptr->portrec_refcount) {				\
		lk_unlock(&port_rec_ptr->portrec_lock);			\
		/* port_rec_ptr LOCK -/- */				\
	} else {							\
		lk_clear(&port_rec_ptr->portrec_lock);			\
		MEM_DEALLOCOBJ(port_rec_ptr, MEM_PORTREC);		\
	}								\
	/* port_rec_ptr LOCK -/- */					\
}
#endif	RPCMOD

/*
 * Memory management definitions.
 */
extern mem_objrec_t		MEM_PORTREC;
extern mem_objrec_t		MEM_PORTITEM;


#endif	_PORT_DEFS_
