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
 * $Log: un_port_hash.h,v $
 * Revision 1.5  1995/02/01  22:08:13  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.4  1994/11/18  20:44:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:36:18  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:15  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/05/06  19:28:41  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:14  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.0  1992/12/10  17:01:23  mjl
 * Definitions for mapping socket ports to socket addresses.
 *
 */

/*
 *  Port name to socket address mapping for using AF_UNIX sockets
 *  with bind() and connect() system calls. 
 */

/*
 *  Insert new map entry in hash chain.  Get seqno from port.
 *  Assumes port not yet in server port set.
 */
#define MAP_PORT_TO_SOCKET(port,so)					\
MACRO_BEGIN								\
	add_to_bindport_list(port, so, seqno_from_port(port));		\
MACRO_END

/*
 *  Mark self and block if seqno mismatch; see SOCKET_LOOKUP_DONE below.
 *  Assumes port already removed from server port set.
 */
#define UNMAP_PORT_TO_SOCKET(port,so)			\
MACRO_BEGIN						\
	remove_from_bindport_list(port);		\
MACRO_END

/*
 *  Map port name to socket address.
 */
#define PORT_TO_SOCKET_LOOKUP(port, so)				\
MACRO_BEGIN							\
	bindport_t	b;					\
	b = find_bindport(port, TRUE);				\
	(so) = ( b == NULL ? NULL : b->bp_socket );		\
MACRO_END

/*
 *  Done using port-to-socket mapping.  If msg_rcvd flag was
 *  set, we are finishing a bind port operation, so do sequence
 *  number bookkeeping for the port.
 */
#define SOCKET_LOOKUP_DONE(so, msg_rcvd)			\
MACRO_BEGIN							\
	bindport_t	b;					\
	ASSERT((so)->vs_bindport != MACH_PORT_NULL);		\
	b = find_bindport((so)->vs_bindport, FALSE);		\
	ASSERT(b != NULL);					\
	unref_bindport(b);					\
	if (msg_rcvd)						\
		increase_bindport_seqno(b);			\
MACRO_END


/* This should be called port_to_socket_map_entry or something. -mjl */
struct bindport {
	mach_port_t		bp_port;
	struct socket		*bp_socket;
	mach_port_seqno_t	bp_seqno;
	struct bindport		*bp_next;
	/*
	 *  Added these so that we can call PORT_TO_SOCKET_LOOKUP()
	 *  in places other than service routines for messages received
	 *  on the bind port.  I.e., before getting rid of a mapping we
	 *  have to make sure of two conditions: bp_seqno matches the
	 *  kernel's version of seqno, and bp_refcnt == 1.   -mjl
	 */
	int			bp_refcnt;
	struct condition	bp_refcond;
};

typedef struct bindport bindport_data_t;
typedef struct bindport *bindport_t;

extern struct bindport *find_bindport();
extern void remove_bindport();
extern void add_to_bindport_list();
extern void remove_from_bindport_list();
extern void increase_bindport_seqno();

