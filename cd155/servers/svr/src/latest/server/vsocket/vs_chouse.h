/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: vs_chouse.h,v $
 * Revision 1.8  1995/02/01  23:21:55  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/12/23  01:35:04  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:10356
 *  Testing:Test case and TCP/IP EATS with various network configs
 *  Module(s):
 * 	net/rtsock.c
 * 	netinet/in_proto.c
 * 	vsocket/vs_chouse.h
 * 	vsocket/vs_chouse.c
 * 	vsocket/vs.defs
 * 	vsocket/vs_ipc.c
 * 	vsocket/vs_subr.c
 * 	vsocket/vs_netops.c
 *
 *   The problem was only seen in systems with multiple network configurations
 *   when a user attempted to bind to INADDR_ANY/port 0. In this
 *   situation, the system is supposed to pick the port number for
 *   the caller. The problem occurred because the VSOCKET code
 *   didn't ensure that the primary socket and each secondary socket used
 *   the same port number.
 *
 *   Now, a bind to port 0 results in an RPC to the clearinghouse to
 *   pick a port number, based on the domain and protocol of the socket.
 *
 * Revision 1.6  1994/11/18  20:52:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:48:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:13:44  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/17  19:07:41  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 3.4  93/05/07  15:26:34  nina
 * Remove NETSERV_CHOUSE
 * 
 * Revision 3.3  93/05/05  22:41:54  mjl
 * New entry format stores netmask, dstaddr, flags, other interface info.
 * Support for passing new entries in MIv3 autoconfiguration RPCs.
 * 
 * Revision 3.2  93/04/12  15:58:15  nina
 * Extended clearinghouse database to include an ordered list
 * of network server nodes.
 * 
 * Revision 3.1  93/02/22  17:42:36  mjl
 * Add name string to chouse_t for debugging.
 * 
 * Revision 3.0  93/02/09  18:07:30  mjl
 * Clearinghouse management definitions.
 * 
 */

#ifndef	_VS_CHOUSE_H_
#define	_VS_CHOUSE_H_

#include "net/if.h"

/*
 *  Clearinghouse declarations
 */


/* 
 * Typedefs for portmaps. The types portmap_t and protocol_portns_t 
 * were added as part of the work to fix PTS 10356. This problem
 * was only seen in systems with multiple network configurations
 * when a user attempted to bind to INADDR_ANY/port 0. In this
 * situation, the system is supposed to pick the port number for
 * the caller. The problem occurred because the VSOCKET code 
 * didn't ensure that each secondary socket used the same port number. 
 *
 * Now, a bind to port 0 results in an RPC to the clearinghouse to
 * pick a port number, based on the domain and protocol of the socket.
 * The clearinghouse keeps track of the number of the last port number
 * handed out for each domain/protocol via the portmap_t and 
 * protocol_portns_t data structures.
 */

/*
 * Each protocol within a domain has its own port name space. 
 */
typedef struct protocol_portns {
	struct protocol_portns	*pns_next;	/* next protocol */
	short			pns_protocol; 	/* protocol number */
	ushort			pns_port;	/* next avail. port number */
	struct mutex		pns_lock;
} protocol_portns_t;

/*
 * Each domain is represented by a portmap_t, which in turn
 * anchors the list of protocol_portns_t for that domain.
 */
typedef struct portmap {
	int			pm_domain;	
	struct portmap		*pm_next;	/* portmap for next domain */
	struct protocol_portns	*pm_proto; /* port name spaces for domain*/
	struct mutex		pm_lock;	
} portmap_t;

/*
 *  Root of a clearinghouse
 */
typedef struct clearinghouse {
	struct netserv_data	*ch_map;
	struct mutex		ch_mtx;
	char			*ch_name;
	queue_head_t		ch_netserv_queue;
	struct portmap		*ch_portmap;
} chouse_t;
#define CH_NAME(chs)	( (chs)->ch_name == NULL ? "(nil)" : (chs)->ch_name )

/*
 *  Description of a network interface, as stored in the clearinghouse
 */
typedef struct iface_info {
	struct iface_info 	*ii_next;	/* next iface in af on node */
	struct netserv_data	*ii_server;	/* backptr to netserv_data_t */
	int			ii_id;		/* &ifnet on srvr node */
	int			ii_flags;	/* copy of ifnet.if_flags */
	struct sockaddr		ii_addr;	/* interface address */
	struct sockaddr		ii_sockmask;	/* interface netmask */
	struct sockaddr		ii_dstaddr;	/* dst. or bcast. address */
} iface_info_t;
#define	ii_broadaddr		ii_dstaddr


/*
 *  Clearinghouse representation of a network server node.
 *  NB there may actually be more than one of these per network
 *  server node, one for each address family configured on that
 *  node.
 */
typedef struct netserv_data {
	struct netserv_data	*next_addr;	/* netserv's within family */
	struct netserv_data	*next_fam;	/* address family list */
	struct netserv_data	*next_server;	/* for load leveling */
	struct iface_info	*if_info;	/* interfaces on node */
	mach_port_t		netserv_port;	/* server port */
	node_t			netserv_node;	/* server node */
	queue_chain_t		netserv_queue;
} netserv_data_t;

/*
 *  This key uniquely identifies an interface description in a
 *  clearinghouse.
 */
typedef struct chouse_key {
	int	ck_af;		/* address family */
	node_t	ck_node;	/* server node */
	int	ck_id;		/* interface id */
} chouse_key_t;

/*
 *  Largest possible ifioctl() data
 */
typedef union ifrequest {
	struct ifreq		u_ifr;
	struct ifaliasreq	u_ifra;
} ifreq_t;

/*
 *  Typedefs for MIG equivalents of some of these data structures.
 *  By using character arrays the same size as the previously defined
 *  structs, we convince MIG to just put the address on the server
 *  stub's stack (rather than copying the whole struct).  The insanely
 *  simple typedef ``char_p_t'' is used to mark places where we are
 *  faking out MIG (that is, places where a MIG hacker could profitably
 *  spend some time to Do The Right Thing instead of casting).
 */
typedef char ifreq_p_t		[sizeof(ifreq_t)];
typedef char chouse_key_p_t	[sizeof(chouse_key_t)];
typedef char iface_info_p_t	[sizeof(iface_info_t)];
typedef char *char_p_t;

/*
 *  Miscellany
 */

typedef int		(*ch_action_t)(mach_port_t, node_t, caddr_t);
extern chouse_t		netserv_chouse;
extern iface_info_t	*chouse_find_key(chouse_t *,
					 chouse_key_t *,
					 int,
					 int*);

#endif	/* ! _VS_CHOUSE_H_ */
