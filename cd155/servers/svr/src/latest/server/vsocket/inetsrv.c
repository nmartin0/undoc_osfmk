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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: inetsrv.c,v $
 * Revision 1.13  1995/02/01  23:13:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/11/18  20:51:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1993/09/20  23:58:23  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.10.6.1  1993/09/20  23:49:20  cfj
 * Bug fix for PTS #6663.  The correct netserver is now chosen.
 *
 * Revision 1.10  1993/07/14  18:48:05  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:12:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/05/18  03:20:54  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.8  1993/05/06  19:31:33  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 3.10  93/06/03  07:55:12  bhk
 * removed rip routing endpoint from the clearinghose 'local only' filters
 * to allow SSI routing
 * 
 * Revision 3.9  93/05/05  22:21:47  mjl
 * Remove unused stack variable declaration.
 * 
 * Revision 3.8  1993/03/26  12:02:46  mjl
 * Use network byte order when comparing ports and IP addresses in
 * force_bind_to_local_socket().
 *
 * Revision 3.7  93/02/23  11:06:05  mjl
 * Fix bogus debug printf.
 * 
 * Revision 3.6  93/02/22  17:33:10  mjl
 * Get rid of possible NULL pointer dereferrence in debug code.
 * 
 * Revision 3.5  93/02/09  18:13:14  mjl
 * Provide force_bind_to_local_socket() routine for determining if a particular
 * address should be handled locally (e.g. for routing daemon sockets, which
 * must speak to the local MI interface regardless of what the network server
 * clearinghouse may say).  Make vsinet_match() call this routine.
 * 
 * Revision 3.4  93/01/04  20:40:47  bhk
 * Added checks for local endpoints (i.e. roututing endpoints)
 * 
 * Revision 3.3  92/09/24  17:08:32  bhk
 * removed unnecessary interface registration code
 * 
 * Revision 3.2  92/07/26  17:45:24  bhk
 * Cleaned up debug
 * 
 * Revision 3.1  92/06/16  18:42:52  bhk
 * fixed return codes for if_check_in
 * 
 * Revision 3.0  92/04/20  17:14:32  bhk
 * Genesis  The Internet network server
 * 
 */

#include "sys/param.h"
#include <mach/boolean.h>
#include "uxkern/import_mach.h"
#include "sys/uio.h"
#include "sys/mbuf.h"
#include "vsocket/vsocket.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "vsocket/vs_types.h"

extern	node_t	netserver_node;
extern 	mach_port_t	clearinghouse_port;
extern	node_t	this_node;

/* this is the socket server port */
mach_port_t	inetserver_port = MACH_PORT_NULL;

#ifdef	OBSOLETE
/*
 *  Table of Internet socket addresses that force a socket to the
 *  local node when they are specified in a bind() system call.
 *  XXX Eventually such addresses must be set via an ioctl or
 *  syscall, not hardwired as here.
 */
#define SIN_ZERO		{ 0, 0, 0, 0, 0, 0, 0, 0 }
#define MKIPADDR(a,b,c,d)	(((a)<<24)|((b)<<16)|((c)<<8)|(d))
struct sockaddr_in loinetaddrs[] = {
/*	{ 16, AF_INET, 520, MKIPADDR(192,80,81,1),	SIN_ZERO }, /* */
/*	{ 16, AF_INET, 520, INADDR_ANY,			SIN_ZERO }, /* */
/*eot*/	{ 0,  0,       0,   0,				SIN_ZERO },
};


/*
 *  Return TRUE iff the given IP address is one that, when a socket is
 *  bound to them, always get a local socket and not a remote one.
 */
int
force_bind_to_local_socket(
	struct sockaddr_in	*sin)
{
	struct sockaddr_in	*lin	= &loinetaddrs[0];
	int			rc	= FALSE;

	for ( ; lin->sin_len; lin++ ) {
		if ((sin->sin_family == lin->sin_family) &&
		    (sin->sin_port == htons(lin->sin_port)) &&
		    (sin->sin_addr.s_addr == htonl(lin->sin_addr.s_addr)))
			rc = TRUE;
	}

	VSDEBUG(VSDEBCONTROL,("force_bind_to_local_socket: 0x%x/%d %sforced\n",
		sin->sin_addr.s_addr, sin->sin_port, (rc ? "" : "not ")));
	return (rc);
}
#endif	/* OBSOLETE */


void
inetsrv_init()
{
	if(mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
		&inetserver_port) != KERN_SUCCESS)
		panic("inetsrv_init: mach_port_allocate failure");
	if(mach_port_insert_right(mach_task_self(), 
				  inetserver_port,
				  inetserver_port,
				  MACH_MSG_TYPE_MAKE_SEND) != KERN_SUCCESS)
		panic("inetsrv_init: inetserver_port unable to set rights\n");
	ux_server_add_port(inetserver_port);
}


int
vsinet_match(addr,vsaddr)
struct sockaddr_in		*addr;
struct sockaddr_in		*vsaddr;
{
	struct sockaddr_in	*sin;

#ifdef	OBSOLETE
	/*
	 *  Matching an address against NULL means checking
	 *  to see if it is one of the "special" addresses
	 *  that should be forced to a local socket rather
	 *  than be shadowed by a secondary socket on a
	 *  remote network server.
	 */
	if ( vsaddr == NULL )
		return (force_bind_to_local_socket(addr));
#else
	ASSERT(vsaddr != NULL);
#endif

	/* do we have to ntohl here ? */
	VSDEBUG(VSDEBCONTROL,("vsinet_match: comparing 0x%x with 0x%x\n",
			addr->sin_addr.s_addr,vsaddr->sin_addr.s_addr));

	/*
	 *  Simple IP address matching.
	 */
	if(addr->sin_addr.s_addr == INADDR_ANY)
		return TRUE;
	if(addr->sin_addr.s_addr == vsaddr->sin_addr.s_addr)
		return TRUE;
	else
		return FALSE;
}

