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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: port_hash.c,v $
 * Revision 1.7  1994/11/18  20:49:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/05/04  22:05:59  mjl
 * TNC select rewrite.  Added port_hash_walk() routine to walk thru the
 * contents of a hash table.  Useful for debugging.
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.5  1993/07/14  18:43:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:04:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:31:40  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/03/29  23:07:43  nandy
 * Merged from T9 branch
 *
 * Revision 1.2.8.1  1993/03/29  22:49:52  nandy
 * Fixes from loverso to fix the OIP int problem.
 *  Revision 2.11  93/03/24  15:38:36  loverso
 *  Remove unused funcs: mpportid_hash_{forall_remove,unlock}()
 *  Added new funcs mpportid_hash_{forall_func,lookup_func}(),
 *  which do an out-call while holding the hash lock.  This type of
 *  mechanism should replace mpport_hash_lookup_lock(). (loverso)
 *
 * Revision 2.11  93/03/24  15:38:36  loverso
 * 	Remove unused funcs: mpportid_hash_{forall_remove,unlock}()
 * 	Added new funcs mpportid_hash_{forall_func,lookup_func}(),
 * 	which do an out-call while holding the hash lock.  This type of
 * 	mechanism should replace mpport_hash_lookup_lock(). (loverso)
 * 
 * Revision 2.10  93/01/06  10:36:55  loverso
 * 	Fix notice.
 * 
 * Revision 2.11  1993/03/24  15:38:36  loverso
 * 	Remove unused funcs: mpportid_hash_{forall_remove,unlock}()
 * 	Added new funcs mpportid_hash_{forall_func,lookup_func}(),
 * 	which do an out-call while holding the hash lock.  This type of
 * 	mechanism should replace mpport_hash_lookup_lock(). (loverso)
 *
 * Revision 2.10  93/01/06  10:36:55  loverso
 * 	Fix notice.
 * 
 * Revision 2.9  92/06/30  22:48:08  loverso
 * 	Don't leak memory in mpportid_hash_enter_sub.  (loverso)
 * 
 * Revision 2.8  92/05/12  00:04:13  loverso
 * 	Add new calls.  mpportid_hash_enter_sub() allows caller to refuse to
 * 	use subchains while mpportid_hash_enter() keeps the previous interface,
 * 	and always uses subchains.  mpportid_hash_forall_remove() and
 * 	mpport_hash_lookup_lock() are special functions for safe dead-name
 * 	processing. (loverso)
 * 
 * Revision 2.7  92/05/01  10:03:03  rabii
 * 	Add "subchain" to mpportid routines since it is possible for a
 * 	single system call to visit the same node more than once.(loverso)
 * 
 * Revision 2.6  92/03/15  14:30:44  roy
 * 	Removed id from PORT_HASH and added PORTID_HASH (loverso)
 * 
 * Revision 2.5  92/03/03  13:50:49  pjg
 * 	Add mpportid_* routines (loverso).
 * 
 * Revision 2.4  92/03/01  18:33:55  pjg
 * 	92/02/28  17:03:43  noemi
 * 	Changed some 0 returns to NULLs.
 * 
 * Revision 2.3  92/01/05  20:16:19  roy
 * 	1991/10/14  20:49:15  noemi
 * 	Added hash table functions for hash tables with locks: 
 * 	mpport_hash_init, mpport_hash_enter,  mpport_hash_lookup, 
 * 	and mpport_hash_remove.
 * 
 * Revision 2.2  91/08/31  14:27:05  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:06:12  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:28:05  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:20:42  rpd]
 * 
 */

/*
 * Port-to-whatchamacallit hash routines.
 */

#include <sys/unix_defs.h>
#include <sys/types.h>
#include <uxkern/port_hash.h>

/*
 * Routines for hash tables without locks.
 */
port_hash_table_t
port_hash_init(size)
	int	size;
{
	int			i;
	port_hash_table_t	ht;

	ht = (port_hash_table_t) malloc(sizeof(struct port_hash_table) +
					  (size - 1) * sizeof(queue_head_t));
	ht->length = size;

	for (i = 0; i < size; i++) {
	    queue_init(&ht->head[i]);
	}

	return (ht);
}

boolean_t
port_hash_enter(ht, port, value)
	port_hash_table_t	ht;
	mach_port_t	port;
	char *		value;
{
	queue_t			q;
	port_hash_entry_t	entry;

	q = &ht->head[PORT_HASH(port) % ht->length];

	entry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
	    if (entry->port == port)
		return (FALSE);	/* already there */
	    entry = (port_hash_entry_t)queue_next(&entry->chain);
	}
	entry = (port_hash_entry_t)malloc(sizeof(struct port_hash_entry));

	entry->port = port;
	entry->value = value;

	queue_enter(q, entry, port_hash_entry_t, chain);

	return (TRUE);
}

char *
port_hash_lookup(ht, port)
	port_hash_table_t	ht;
	mach_port_t		port;
{
	queue_t			q;
	port_hash_entry_t	entry;

	q = &ht->head[PORT_HASH(port) % ht->length];

	entry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
	    if (entry->port == port)
		return (entry->value);
	    entry = (port_hash_entry_t)queue_next(&entry->chain);
	}
	return (NULL);
}

boolean_t
port_hash_remove(ht, port)
	port_hash_table_t	ht;
	mach_port_t		port;
{
	queue_t			q;
	port_hash_entry_t	entry;

	q = &ht->head[PORT_HASH(port) % ht->length];

	entry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
	    if (entry->port == port) {
		queue_remove(q, entry, port_hash_entry_t, chain);
		free(entry);

		return (TRUE);
	    }
	    entry = (port_hash_entry_t)queue_next(&entry->chain);
	}
	return (FALSE);
}


#if	MACH_ASSERT
int
port_hash_walk(ht, func, farg)
	port_hash_table_t	ht;
	int			(*func)(port_hash_entry_t, int);
	int			farg;
{
	queue_t			q;
	port_hash_entry_t	entry;
	int			i, rc;
	
	for (i = 0; i < ht->length; i++) {
		q = &ht->head[i];
		entry = (port_hash_entry_t)queue_first(q);
		while (!queue_end(q, (queue_entry_t)entry)) {
			if ( (rc = (*func)(entry, farg)) != 0 )
				return rc;
			entry = (port_hash_entry_t)queue_next(&entry->chain);
		}
	}
	return 0;
}
#endif	/* MACH_ASSERT */


#ifdef	OSF1_ADFS
/*
 * Routines for hash tables with locks.
 */

mpport_hash_table_t
mpport_hash_init(size)
	int	size;
{
	int			i;
	mpport_hash_table_t	ht;

	ht = (mpport_hash_table_t) malloc(sizeof(struct mpport_hash_table) +
					  (size - 1) * sizeof(mpqueue_head_t));
	ht->length = size;

	for (i = 0; i < size; i++) {
		queue_init(&ht->mphead[i].head);
		simple_lock_init(&ht->mphead[i].lock);
	}
	return (ht);
}

boolean_t
mpport_hash_enter(ht, port, value)
	mpport_hash_table_t	ht;
	mach_port_t		port;
	char 			*value;
{
	queue_t			q;
	port_hash_entry_t	entry, qentry;
	mpqueue_head_t		*mpq;

	/* This could screw up lookup */
	ASSERT(value != NULL);

	/*
	 * Allocate a new hash table entry and initialize it.
	 */
	entry = (port_hash_entry_t)malloc(sizeof(struct port_hash_entry));
	entry->port = port;
	entry->value = value;

	mpq = &ht->mphead[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	qentry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)qentry)) {
		/*
		 * If the entry was already inserted, return unsuccessfully.
		 */
		if (qentry->port == port) {
			usimple_unlock(&mpq->lock);
			free(entry);
			return (FALSE);		/* already there */
		}
		qentry = (port_hash_entry_t)queue_next(&qentry->chain);
	}
	queue_enter(q, entry, port_hash_entry_t, chain);
	usimple_unlock(&mpq->lock);
	return (TRUE);
}

char *
mpport_hash_lookup(ht, port)
	mpport_hash_table_t	ht;
	mach_port_t		port;
{
	queue_t			q;
	port_hash_entry_t	entry;
	mpqueue_head_t		*mpq;
	char			*value;

	mpq = &ht->mphead[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port) {
			value = entry->value;
			usimple_unlock(&mpq->lock);
			return value;
		}
		entry = (port_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return (NULL);
}

/*
 * Same as lookup, but if it finds the port, it will return with
 * the hash queue locked.
 * Caller MUST unlock queue before any other queue operation.
 *
 * This should be replaced with something along the lines of
 * mpportid_hash_lookup_func().
 */
char *
mpport_hash_lookup_lock(ht, port)
	mpport_hash_table_t	ht;
	mach_port_t		port;
{
	queue_t			q;
	port_hash_entry_t	entry;
	mpqueue_head_t		*mpq;
	char			*value;

	mpq = &ht->mphead[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port) {
			value = entry->value;
			/* return with queue locked */
			return value;
		}
		entry = (port_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return (NULL);
}

/*
 * This should go away.  See comments above.
 */
void
mpport_hash_unlock(ht, port)
	mpport_hash_table_t	ht;
	mach_port_t		port;
{
	mpqueue_head_t		*mpq;

	mpq = &ht->mphead[PORT_HASH(port) % ht->length];
	usimple_unlock(&mpq->lock);
}

boolean_t
mpport_hash_remove(ht, port)
	mpport_hash_table_t	ht;
	mach_port_t		port;
{
	queue_t			q;
	port_hash_entry_t	entry;
	mpqueue_head_t		*mpq;

	mpq = &ht->mphead[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (port_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port) {
			queue_remove(q, entry, port_hash_entry_t, chain);
			usimple_unlock(&mpq->lock);
			free(entry);
			return (TRUE);
		}
		entry = (port_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return (FALSE);
}


/*
 * Routines for hash tables of port/id pairs with locks.
 */

mpportid_hash_table_t
mpportid_hash_init(size)
	int	size;
{
	int			i;
	mpportid_hash_table_t	ht;

	ht = (mpportid_hash_table_t) malloc(sizeof(struct mpportid_hash_table) +
					  (size - 1) * sizeof(mpqueue_head_t));
	ht->length = size;

	for (i = 0; i < size; i++) {
		queue_init(&ht->head[i].head);
		simple_lock_init(&ht->head[i].lock);
	}
	return (ht);
}

/*
 * Enter into a hash table based upon "port".
 * The port/id pair is used to determine if already in the table.
 * "value" is the key being entered, and will be returned on lookup.
 * "value" cannot be NULL, or else you cannot tell lookup failures.
 * If subchain is true, then it is ok to use subchains of "value".
 */
boolean_t
mpportid_hash_enter_sub(ht, port, id, value, subchain)
	mpportid_hash_table_t	ht;
	mach_port_t		port;
	unsigned long		id;
	char 			*value;
	boolean_t		subchain;
{
	queue_t			q;
	portid_hash_entry_t	entry, qentry;
	mpqueue_head_t		*mpq;

	/* This could screw up lookup */
	ASSERT(value != NULL);

	/*
	 * Allocate a new hash table entry and initialize it.
	 */
	entry = (portid_hash_entry_t)malloc(sizeof(struct portid_hash_entry));
	entry->port = port;
	entry->id = id;
	entry->value = value;
	entry->subchain = (portid_hash_entry_t)NULL;

	mpq = &ht->head[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	qentry = (portid_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)qentry)) {
		/*
		 * If the port/id is already in the table, then we use the
		 * subchain.  This is for an operation that has looped btwn
		 * back to the same node.
		 */
		if (qentry->port == port && qentry->id == id) {
			if (subchain == FALSE) {
				free(entry);
				usimple_unlock(&mpq->lock);
				return FALSE;
			}
#if	MACH_ASSERT
			/*
			 * The same "value" cannot be in the subchain twice.
			 */
			{
				portid_hash_entry_t list = qentry;
				for (; list != NULL; list = list->subchain) {
					ASSERT(list->value != value);
				}
			}
#endif
			entry->subchain = qentry->subchain;
			qentry->subchain = entry;

			usimple_unlock(&mpq->lock);
			return TRUE;
		}
		qentry = (portid_hash_entry_t)queue_next(&qentry->chain);
	}
	queue_enter(q, entry, portid_hash_entry_t, chain);
	usimple_unlock(&mpq->lock);
	return TRUE;
}


/*
 * Allow subchains.
 */
void
mpportid_hash_enter(ht, port, id, value)
	mpportid_hash_table_t	ht;
	mach_port_t		port;
	unsigned long		id;
	char 			*value;
{
	/* It can't return FALSE when subchain == TRUE */
	(void)mpportid_hash_enter_sub(ht, port, id, value, TRUE);
}
	

char *
mpportid_hash_lookup(ht, port, id)
	mpportid_hash_table_t	ht;
	mach_port_t		port;
	unsigned long		id;
{
	queue_t			q;
	portid_hash_entry_t	entry;
	mpqueue_head_t		*mpq;
	char			*value;

	mpq = &ht->head[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (portid_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port && entry->id == id) {
			if (entry->subchain)
				value = entry->subchain->value;
			else
				value = entry->value;
			usimple_unlock(&mpq->lock);
			return value;
		}
		entry = (portid_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return (NULL);
}


char *
mpportid_hash_lookup_func(ht, port, id, func)
	mpportid_hash_table_t	ht;
	mach_port_t		port;
	unsigned long		id;
	boolean_t   		(*func)();
{
	queue_t			q;
	portid_hash_entry_t	entry;
	mpqueue_head_t		*mpq;
	char			*value;

	mpq = &ht->head[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (portid_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port && entry->id == id) {
			value = (entry->subchain)
				? entry->subchain->value
				: entry->value;
			(void)(*func)(port, id, value);
			usimple_unlock(&mpq->lock);
			return value;
		}
		entry = (portid_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return NULL;
}


int
mpportid_hash_foreach_func(ht, port, func)
	mpportid_hash_table_t	ht;
	mach_port_t		port;
	int 	   		(*func)();
{
	queue_t			q;
	portid_hash_entry_t	entry;
	mpqueue_head_t		*mpq;
	int			count = 0;

	mpq = &ht->head[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (portid_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port) {
			count++;
			(void)(*func)(port, entry->id, (entry->subchain)
						 	? entry->subchain->value
							: entry->value);
		}
		entry = (portid_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return count;
}

boolean_t
mpportid_hash_remove(ht, port, id, value)
	mpportid_hash_table_t	ht;
	mach_port_t		port;
	unsigned long		id;
	char 			*value;
{
	queue_t			q;
	portid_hash_entry_t	entry;
	mpqueue_head_t		*mpq;

	mpq = &ht->head[PORT_HASH(port) % ht->length];
	q = &mpq->head;
	usimple_lock(&mpq->lock);

	entry = (portid_hash_entry_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)entry)) {
		if (entry->port == port && entry->id == id) {
			/*
			 * If there is no subchain (typical), then just remove
			 * this from the queue.
			 * Else, this remove MUST be for the top entry on the
			 * subchain.
			 */
			if (entry->subchain == (portid_hash_entry_t)NULL) {
				ASSERT(entry->value == value);
				queue_remove(q, entry, portid_hash_entry_t,
					chain);
				free(entry);
			} else {
				portid_hash_entry_t hold = entry->subchain;
				ASSERT(entry->subchain->value == value);
				entry->subchain = entry->subchain->subchain;
				free(hold);
			}
			usimple_unlock(&mpq->lock);
			return (TRUE);
		}
		entry = (portid_hash_entry_t)queue_next(&entry->chain);
	}
	usimple_unlock(&mpq->lock);
	return (FALSE);
}
#endif	/* OSF1_ADFS */
