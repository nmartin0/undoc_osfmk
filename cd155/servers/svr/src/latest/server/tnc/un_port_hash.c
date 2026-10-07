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
 * $Log: un_port_hash.c,v $
 * Revision 1.5  1995/02/01  22:07:26  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.4  1994/11/18  20:44:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:36:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:09  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/05/06  19:28:31  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:12  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.0  1992/12/10  17:00:37  mjl
 * Routines for mapping socket ports to socket addresses.
 *
 * Revision 3.0  92/08/08  01:22:37  jdh
 * code to manage the socket to port maps for bound unix 
 * domain stream sockets -- jdh
 * 
 */

/*
 *		       !!!! A C H T U N G !!!!
 *
 *  Never, never call these routines directly.  Only call them via
 *  the macros in <tnc/un_port_hash.h>, as these implementation
 *  routines will be rewritten Real Soon Now!
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/errno.h>
#include <net/net_malloc.h>
#include <kern/macro_help.h>

#include <tnc/un.h>

/*
 * There could be several send rights floating around for a socket
 * port (one in the vnode, and one or more by sockets attempting a
 * connection).  Because of this, forcing a socket to use a particular
 * name for its receive right after relocating just doesn't work. 
 * Only one name for a port is allowed in any one task, and the task
 * to which a socket is relocating may already have rights for this
 * port.  Trying to keep these names up to date causes race
 * conditions.  Therefore, the port name used for socket ports will
 * always simply be the name chosen by Mach.  The bindports list
 * contains the port name to socket address mapping for bound
 * sockets in this task.
 */


/*
 * Possible improvements:
 *
 * 1) Use a reader/write lock instead of a mutex lock for the
 * bindports linked list -- reader for most cases, writer for
 * adding or deleting an element.
 *
 * 2) Store the bindport structure directly in the socket, instead
 * of having to look it up each time.
 *
 * 3) Use a doubly linked list, so deleting is more efficient.
 * Or possibly, use a hash so looking up is more efficient.
 */


static bindport_t bindports = NULL;
static struct mutex bindports_lock = MUTEX_INITIALIZER;


/*
 * return pointer to bindport structure which contains the
 * mach_port_t passed in
 */
bindport_t 
find_bindport(
	mach_port_t	port,
	int		do_ref)
{
	bindport_t b;

	mutex_lock(&bindports_lock);
	if( !(b = bindports) ) {
		mutex_unlock(&bindports_lock);
		return(NULL);
	}
	while(b->bp_port != port && b->bp_next) {
		b = b->bp_next;
	}
	/*
	 *  If we found the mapping and we should take a reference,
	 *  do so now while we hold the mutex.    -mjl
	 */
	if (do_ref && b->bp_port == port)
		b->bp_refcnt++;
	mutex_unlock(&bindports_lock);

	UNDEBUG(U_BINDPORT,("find_bindport: port=0x%x, b=0x%x\n", port, b));

	if( b->bp_port == port) {
		return(b);
	} else {
		return(NULL);
	}
}



/*
 * find structure remove_b in bindports_list and
 * remove it
 */
void
remove_bindport(
	bindport_t remove_b)
{
	bindport_t b;
	bindport_t prev = NULL;

	ASSERT(remove_b != NULL);

	mutex_lock(&bindports_lock);
	while (remove_b->bp_refcnt > 1)
		condition_wait(&remove_b->bp_refcond, &bindports_lock);

	b = bindports;
	ASSERT(b != NULL);
	while( b != remove_b ) {
		prev = b;
		b = b->bp_next;
	}

	ASSERT(b == remove_b);
	if( !prev ) {
		bindports = b->bp_next;
	} else {
		prev->bp_next = b->bp_next;
	}

	mutex_unlock(&bindports_lock);

	UNDEBUG(U_BINDPORT,("remove_bindport: b=0x%x\n", b));

	NET_FREE(b, M_TEMP);
}


/*
 * Add a bindport structure for the passed in port/socket pair to this
 * list.  This makes the port to socket mapping available to any
 * connecting socket. 
 */
void
add_to_bindport_list(
	mach_port_t		port,
	struct socket		*so,
	mach_port_seqno_t	seqno)
{
	bindport_t b, nb;

	UNDEBUG(U_BINDPORT,
		("add_to_bindport_list: port=0x%x, so=0x%x, seqno=%d\n",
		 port, so, seqno));

	NET_MALLOC(nb, bindport_t, sizeof(bindport_data_t), M_TEMP, M_WAITOK);

	nb->bp_port = port;
	nb->bp_socket = so;
	nb->bp_seqno = seqno;
	nb->bp_next = NULL;
	nb->bp_refcnt = 1;
	condition_init(&nb->bp_refcond);

	mutex_lock(&bindports_lock);
	if( !(b = bindports) ) {
		bindports = nb;
	} else {
		b = bindports;
		while(b->bp_next != NULL && b->bp_next->bp_port > port)
			b = b->bp_next;
		if( b->bp_next && b->bp_next->bp_port == port ) {
			panic("found a bindports entry with port name 0x%x\n",
			      port);
		}
		nb->bp_next = b->bp_next;
		b->bp_next = nb;
	}
	mutex_unlock(&bindports_lock);
}


/*
 * Remove the bindport structure, which means that no more connect
 * attempts to this socket can be made.  
 *
 * Use sequence count on the socket port, and wait until outstanding
 * operations on this socket port finish before attempting to remove
 * the bindport structure (and relocate or remove socket).
 */
void
remove_from_bindport_list(
	mach_port_t	port)
{
	bindport_t	b, prev;
	struct socket	*so;

	b = find_bindport(port, FALSE);
	if( !b ) {
		return;
	}

	so = b->bp_socket;
	/*
	 * wait until no connect operations are in progress on
	 * this port (connect operations are the only type of
	 * operations that take place on a socket port).
	 */
	so->vs_flags |= VS_BP_SEQNO_WAIT;

	UNDEBUG(U_BINDPORT,
	    ("remove_from_bindport_list: seqno=%d, first mutex lock 0x%x\n",
	     b->bp_seqno, &so->vs_seqno_mutex));
	mutex_lock(&so->vs_seqno_mutex);

	while( seqno_from_port(b->bp_port) != b->bp_seqno ) {
		/*
		 *  Threads still active on this socket.
		 *  Cannot remove port yet.
		 *  Wait for a thread to finish up, increment
		 *  b->seqno and unlock this mutex lock.
		 */
		UNDEBUG(U_BINDPORT,
		  ("remove_from_bindport_list: seqno=%d, locking mutex 0x%x\n",
		   b->bp_seqno, &so->vs_seqno_mutex));
		mutex_lock(&so->vs_seqno_mutex);
	}
	/*
	 * assumption -- it is ok to unlock a mutex lock that may not
	 * actually be locked.
	 */
	UNDEBUG(U_BINDPORT,
	    ("remove_from_bindport_list: seqno=%d, final mutex unlock 0x%x\n",
	     b->bp_seqno, &so->vs_seqno_mutex));
	mutex_unlock(&so->vs_seqno_mutex);
	so->vs_flags &= ~VS_BP_SEQNO_WAIT;

	remove_bindport(b);
}


/*
 *  Decrement the bindport ref count.  This is done for all
 *  calls to SOCKET_LOOKUP_DONE().
 */
void
unref_bindport(
	bindport_t	b)
{
	ASSERT(b != NULL);
	mutex_lock(&bindports_lock);
	if (--b->bp_refcnt == 1)
		condition_signal(&b->bp_refcond);
	mutex_unlock(&bindports_lock);
}


/*
 * Finished connect attempt -- increase the bindport 
 * seqno to show that this operation is complete as
 * far as the socket and bound socket port are concerned.
 *
 * This is done only from calls to SOCKET_LOOKUP_DONE() that
 * complete an operation caused by a message received on the
 * bind port.
 */
void
increase_bindport_seqno(
	bindport_t b)
{
	/*
	 * Is incrementing an uninterruptible operation?
	 * This is also a question for the file port sequence
	 * counting.
	 */
	
	ASSERT(b && b->bp_socket);

	mutex_lock(&bindports_lock);

	b->bp_seqno++;
	UNDEBUG(U_BINDPORT,
		("increase_bindport_seqno: seqno=%d\n", b->bp_seqno));

	/*
	 * assumption -- it is ok to unlock a mutex
	 * lock that may not actually be locked.
	 */
	if( WAITING_ON_BP_SEQNO(b->bp_socket) ) {
		UNDEBUG(U_BINDPORT,
			("increase_bindport_seqno: mutex_unlock(0x%x\n",
			 &(b->bp_socket->vs_seqno_mutex)));
		mutex_unlock(&(b->bp_socket->vs_seqno_mutex));
	}

	mutex_unlock(&bindports_lock);
}
