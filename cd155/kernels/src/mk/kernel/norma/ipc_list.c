/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * SSD HISTORY
 * $Log: ipc_list.c,v $
 * Revision 1.10  1994/11/18  20:55:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/03/19  01:29:04  lenb
 * Benefit: use positive logic on locking assertions
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.8  1993/09/28  18:03:20  andyp
 * Update for the 1.2 release.
 *
 *
 *	Add placeholder for future debugging routine that tracks
 *	outstanding memory throughout all NORMA data structures.
 *	For now, at least there is a list in one place of all
 *	the NORMA data structures that could be holding ports,
 *	messages, copy objects, etc.  [alanl@osf.org]
 *
 * Revision 1.7  1993/07/28  01:16:49  stans
 * MP3 locking fixes
 *
 * Revision 1.6  1993/06/30  22:50:27  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/06/25  22:29:03  andyp
 * Merged up to the latest from OSF's tree.  Recovered OSF's log messages
 * where needed.  Added the fixes for CPU idle times.  Added some Paragon
 * instrumentation for billing time spent in ipc_wait.c:norma_ipc_kmsg_accept()
 * as idle time.
 *
 * Revision 1.4  1993/06/09  01:40:17  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:45:42  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:19:25  dleslie
 * Patch release of April 23
 *
 * Revision 1.3  1993/04/12  03:40:51  SSD
 * sencond code drop for pager flow control.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_list.c,v
 * Revision 1.1.4.5  1993/06/16  17:17:28  dwm
 * 	Jazz up db_count_all_kotypes() a bit.
 * 	[1993/06/16  17:16:38  dwm]
 *
 * Revision 1.1.4.4  1993/05/26  18:57:34  alanl
 * 	NORMA_IPC:  conditionally compile in port debugging fields.
 * 	[1993/05/26  18:06:34  alanl]
 * 
 * Revision 1.1.4.3  1993/05/23  18:07:30  alanl
 * 	Conditionalize debugging code on MACH_KDB.
 * 	[1993/05/23  17:47:45  alanl]
 * 
 * Revision 1.1.4.2  1993/04/15  22:44:54  alanl
 * 	Additional debugger code to count up and display ports.
 * 	Not yet hooked up to debugger's command-line interface.  [alanl]
 * 	[1993/04/15  22:22:37  alanl]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.1.7.1.10  92/09/15  17:33:41  jeffreyh
 * 	Remove norma dead name logic (fproxy_cancel).
 * 	[92/09/15            dlb]
 * 	New norma uid scheme using a bit array
 * 	[92/08/07            sjs]
 * 
 * 	Call norma_ipc_node_fproxy_cancel when removing migrated principal.
 * 	[92/08/07            dlb]
 * 
 * 	Optimizations: 
 * 	Combine norma_port_{insert,remove} with _locked versions
 * 		since latter are unused outside this file (lookup_locked is used).
 * 	Fold generation of new norma uid into norma_port_insert.
 * 	norma_port_tabled() becomes a macro in ipc_node.h
 * 	[92/08/06            dlb]
 * 
 * 	Simplify check in norma_port_remove_locked because ip_norma_forward
 * 	is now set in root node case.  Also, don't reinitialize no more
 * 	senders when deciding not to remove a dead port.
 * 	[92/08/06            dlb]
 * 
 * Revision 2.1.1.7.1.9  92/06/24  18:00:47  jeffreyh
 * 	Proxies now maintain sotransit; simplify norma_port_remove_try.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.1.1.7.1.8  92/05/27  00:49:09  jeffreyh
 * 	Do not remove ports that are being used to forward other
 * 	requests.  This should change in the future.
 * 	[92/05/18            sjs]
 * 
 * Revision 2.1.1.7.1.7  92/04/08  15:45:33  jeffreyh
 * 	Improve tests in norma_port_remove_try().
 * 	[92/04/01            dlb]
 * 
 * Revision 2.1.1.7.1.6  92/03/28  10:11:32  jeffreyh
 * 	When leaving a proxy to act as a forwarder, reinitialize
 * 	its no senders request so it can receive send rights.
 * 	[92/03/25            dlb]
 * 
 * 	Change norma_port_remove_try to check transit counts instead
 * 	of right counts.
 * 	[92/03/25            dlb]
 * 
 * Revision 2.1.1.7.1.4  92/02/21  11:24:24  jsb
 * 	Added new routine norma_port_remove_try.
 * 	[92/02/20  17:15:49  jsb]
 * 
 * 	Added db_show_{uid,all_uids}_verbose.
 * 	[92/02/20  10:33:47  jsb]
 * 
 * Revision 2.1.1.7.1.3  92/01/21  21:51:19  jsb
 * 	Now uses hash table. Now uses netipc lock instead of spls.
 * 	Removed seqno debugging hack. Added norma_port_iterate and
 * 	reimplemented db_show_uid (nee norma_list_all_ports) with it.
 * 	Added support for show all {proxies, principals}. Use kdbprintf
 * 	instead of printf for these routines for pagination.
 * 	[92/01/16  22:46:09  jsb]
 * 
 * Revision 2.1.1.7.1.2  92/01/03  16:37:27  jsb
 * 	Use ipc_port_release instead of ip_release to allow port deallocation.
 * 	[91/12/31  21:42:40  jsb]
 * 
 * 	Added norma_port_tabled function.
 * 	[91/12/31  17:17:02  jsb]
 * 
 * 	In norma_port_remove, refuse to remove a proxy that started out as a
 * 	principal, since our forwarding algorithms require it to stay around.
 * 	[91/12/31  12:19:08  jsb]
 * 
 * 	Hacked norma_new_uid to incorporate incarnation in generated uids.
 * 	Threw in a stub for norma_ipc_cleanup_incarnation.
 * 	[91/12/29  16:08:32  jsb]
 * 
 * 	Lots of ugly but useful debugging support.
 * 	[91/12/28  18:45:37  jsb]
 * 
 * 	Made norma_list_all_ports print port addresses as well as uids.
 * 	[91/12/25  16:57:46  jsb]
 * 
 * 	First checkin. Split from norma/ipc_transit.c.
 * 	[91/12/24  14:26:58  jsb]
 * 
 * Revision 2.1.1.7.1.1  92/01/03  08:54:06  jsb
 * 	First NORMA branch checkin.
 * 
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 *	File:	norma/ipc_list.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Maintains list of norma ports.
 */

#include <mach_kdb.h>

#include <machine/machparam.h>
#include <ipc/ipc_port.h>
#include <norma/ipc_node.h>
#include <ipc/ipc_object.h>
#include <kern/mach_param.h>

extern void netipc_thread_lock();
extern void netipc_thread_unlock();

/*
 * Definitions for norma special ports.
 */
extern unsigned long	node_incarnation;
extern ipc_port_t	host_special_port[MAX_SPECIAL_ID];
ipc_port_t norma_port_lookup_locked();

/*
 * A bit array scheme is used to garauntee norma uids are unique.
 * Note that all uses of uid_bitar must be under lock conditions.
 */
#define	LONGBITS	(sizeof(long) * 8)

unsigned long uid_bitar[PORT_MAX / LONGBITS];
long	uid_bit_ptr = (MAX_SPECIAL_KERNEL_ID + 10) / LONGBITS + 1;

norma_ipc_cleanup_incarnation(remote)
	unsigned long remote;
{
	/*
	 * XXX
	 * What exactly do we clean up???
	 */
}

#define	HASH_SIZE	128		/* must be power of two */
#define	HASH_MASK	(HASH_SIZE - 1)
#define	UID_HASH(uid)	((IP_NORMA_NODE(uid) + IP_NORMA_LID(uid)) & HASH_MASK)

ipc_port_t	norma_port_table[HASH_SIZE];

int c_norma_port_lookup = 0;
int c_norma_port_insert = 0;
int c_norma_port_remove = 0;

norma_uid_array_init()
{
	register i;
	register unsigned long *iv = uid_bitar;

	for (i = 0; i < PORT_MAX / LONGBITS; i++)
		*iv++ = 0xffffffff;
}
/*
 * We currently use a fairly conservative locking strategy, in which we
 * always grab the netipc_lock before inserts, removals, and lookups.
 * We could do better once we move port removal out of norma_ipc_receive_dest,
 * at which point only port lookups would be done at interrupt level.
 * At this point, we could use a simple lock for lookups and only
 * use netipc_lock (in combination with the simple lock) for inserts and
 * removals. Currently this doesn't buy you much because send-once rights
 * are continually being inserted and removed; if we added a bit of laziness
 * to send-once right handling, then presumably lookups would dominate
 * inserts and removals.
 *
 * XXX The above is an old comment.  norma_ipc_receive_dest no longer
 * XXX executes at interrupt level.  Send once rights could still benefit
 * XXX from laziness in their handling. --dlb
 */

ipc_port_t
norma_port_lookup_locked(uid)
	register unsigned long uid;
{
	register ipc_port_t p;

	c_norma_port_lookup++;
	if (uid == 0) {
		return IP_NULL;
	}
	for (p = norma_port_table[UID_HASH(uid)]; p; p = p->ip_norma_next) {
		if (p->ip_norma_uid == uid) {
			return p;
		}
	}
	return IP_NULL;
}


void
norma_port_insert(port)
	register ipc_port_t port;
{
	register ipc_port_t *bucket;
	register unsigned long uid;

	assert(netipc_unlocked());
	netipc_thread_lock();

	c_norma_port_insert++;
	assert(!norma_port_tabled(port));

	/*
	 * If uid is zero, allocate one (newly exported port).
	 * If caller had another uid in mind, it's already in
	 * port->ip_norma_uid.
	 *
	 * This scheme will guarantee unique uids so long as 
	 * MAX_SPECIAL_KERNEL_ID < 64K (fits in 16 bits).
	 */
	uid = port->ip_norma_uid;
	if (uid == 0) {
		unsigned long bentry = uid_bitar[uid_bit_ptr];
		int wbit = ffs(bentry) - 1;

		uid = uid_bit_ptr * LONGBITS + wbit;
		uid_bitar[uid_bit_ptr] = bentry ^ (1 << wbit);

		/*
		 * point the index to the next available entry.
		 * XXX we currently dont worry about running out of bits,
		 * XXX which should result in a panic.
		 */
		while (uid_bitar[uid_bit_ptr] == 0) {
			if (++uid_bit_ptr >= PORT_MAX / LONGBITS)
				uid_bit_ptr = (MAX_SPECIAL_KERNEL_ID + 10) /
				    	      LONGBITS + 1;
		}
		uid = IP_NORMA_UID(node_self(), uid);
		port->ip_norma_uid = uid;
	}
	assert(uid != 0);

	bucket = &norma_port_table[UID_HASH(uid)];
	port->ip_norma_next = *bucket;
	*bucket = port;
	ip_reference(port);
	netipc_thread_unlock();
}

norma_port_remove(port)
	register ipc_port_t port;
{
	register unsigned long uid;
	register ipc_port_t p, *pp;

	assert(netipc_unlocked());
	netipc_thread_lock();

	c_norma_port_remove++;
	assert(norma_port_tabled(port));

	uid = port->ip_norma_uid;
	assert(uid != 0);

	/*
	 * Don't remove proxies that are forwarding pointers to receive
	 * rights.  They get cleaned up separately.
	 */
	if (port->ip_norma_is_proxy && port->ip_norma_forward) {
		/*
		 *	Reinitialize the nsrequest field in case this port
		 *	acquires any more rights (it was likely cleared
		 *	on the way into here).  This logic doesn't
		 *	belong here either.
		 */
		if (ip_active(port)) {
		        port->ip_nsrequest = ip_nsproxym(port);
		}
		printf1("norma_port_remove: not removing port %x\n", port);
		netipc_thread_unlock();
		return;
	}

	/*
	 * We can go ahead now.
	 */
	pp = &norma_port_table[UID_HASH(uid)];
	for (p = *pp; p; pp = &p->ip_norma_next, p = *pp) {
		if (p == port) {
		    	int wbit, bentry;
			register int node;

			/*
			 * return the uid to the pool
			 */
			if ((node = IP_NORMA_NODE(uid)) == node_self()){
				bentry = IP_NORMA_LID(uid);
				wbit = bentry & (LONGBITS - 1);
				bentry = bentry / LONGBITS;
				/*
				 * XXX This assert should go away in the
				 * XXX near future.
				 */
				assert((uid_bitar[bentry] & (1 << wbit)) == 0);
				uid_bitar[bentry] |= 1 << wbit;

				/*
				 *	If the uid bit pointer points at
				 *	this uid entry, we risk a quick
				 *	recycle.  Bump the pointer.
				 */
				if (bentry == uid_bit_ptr) {
				    do {
					if (++uid_bit_ptr >=
						PORT_MAX / LONGBITS)
				    	    uid_bit_ptr =
					      (MAX_SPECIAL_KERNEL_ID + 10) /
				    	          LONGBITS + 1;
		   		    } while (uid_bitar[uid_bit_ptr] == 0);
				}
			}

			*pp = port->ip_norma_next;
			port->ip_norma_next = port;
			printf1("norma_port_remove(0x%x:%x): refs %d\n",
				port, uid, port->ip_references - 1);
			netipc_thread_unlock();
#if     NCPUS > 1 && defined(PARAGON860)
			ipc_object_release_locked( &(port)->ip_object );
#else
			ipc_port_release(port);
#endif
			return;
		}
	}
	panic("norma_port_remove(0x%x:%x): not found\n", port, uid);
}

ipc_port_t
norma_port_lookup(uid)
	register unsigned long uid;
{
	register ipc_port_t port;

	assert(netipc_unlocked());
	netipc_thread_lock();
	port = norma_port_lookup_locked(uid);
	netipc_thread_unlock();
	return port;
}

void
norma_port_remove_try(port)
	register ipc_port_t port;
{
	/*
	 *	This can be called on both ports and proxies.
	 *	For ports, we want to know that there are no
	 *	rights in the norma system (transits are zero).
	 *	For ports we leave the port tabled
	 *	as long as it is not dead.  Also don't remove
	 *	untabled ports (they're already gone).
	 */
	if (port->ip_norma_stransit == 0 && port->ip_norma_sotransit == 0 &&
	    norma_port_tabled(port) &&
	    (IP_NORMA_IS_PROXY(port) || !ip_active(port))) {
		printf1("norma_port_remove_try: releasing 0x%x:%x\n",
			port, port->ip_norma_uid);
		norma_port_remove(port);
	}
}


#if	MACH_KDB

norma_port_table_statistics()
{
	int i, total, average, len, longest;
	int l1, l2, l3;
	ipc_port_t p;

	total = 0;
	for (i = 0; i < HASH_SIZE; i++) {
		for (p = norma_port_table[i]; p; p = p->ip_norma_next) {
			total++;
		}
	}
	average = total / HASH_SIZE;
	if (average == 0) {
		average = 1;
	}
	longest = l1 = l2 = l3 = 0;
	for (i = 0; i < HASH_SIZE; i++) {
		len = 0;
		for (p = norma_port_table[i]; p; p = p->ip_norma_next) {
			len++;
		}
		if (longest < len) {
			longest = len;
		}
		if (len > average) {
			l1++;
			if (len > 2 * average) {
				l2++;
				if (len > 3 * average) {
					l3++;
				}
			}
		}
	}
	kdbprintf("%d ports, %d buckets, average %d per bucket\n",
		  total, HASH_SIZE, total / HASH_SIZE);
	kdbprintf("%d chains longer than %d, %d > %d, %d > %d; longest = %d\n",
		  l1, average, l2, 2 * average, l3, 3 * average, longest);
}

norma_port_iterate(function, args)
	void (*function)();
	void *args;
{
	int i;
	ipc_port_t p;

	for (i = 0; i < HASH_SIZE; i++) {
		for (p = norma_port_table[i]; p; p = p->ip_norma_next) {
			(*function)(p, args);
		}
	}
	(*function)(IP_NULL, args);
}

#include <ddb/db_sym.h>

void
db_show_uid(port, args)
	ipc_port_t port;
	void *args;
{
	int *countp = (int *) args;

	if (port == IP_NULL) {
		printf("\n");
		return;
	}
	if (++*countp == 4) {
		kdbprintf("%x:%x\n", port, port->ip_norma_uid);
		*countp = 0;
	} else {
		kdbprintf("%x:%x ", port, port->ip_norma_uid);
	}
}

void
db_show_uid_verbose(port, args)
	ipc_port_t port;
	void *args;
{
	if (port == IP_NULL) {
		return;
	}
#if	MACH_ASSERT
	kdbprintf("%x:%x %6d ",
		  port, port->ip_norma_uid, port->ip_norma_spare2);
	db_printsym(port->ip_norma_spare1, DB_STGY_PROC);
	kdbprintf("\n");
#endif
}

void
db_show_proxy(port, args)
	ipc_port_t port;
	void *args;
{
	if (port == IP_NULL || port->ip_norma_is_proxy) {
		db_show_uid(port, args);
	}
}

void
db_show_principal(port, args)
	ipc_port_t port;
	void *args;
{
	if (port == IP_NULL || ! port->ip_norma_is_proxy) {
		db_show_uid(port, args);
	}
}


void
db_show_dead(port, args)
	ipc_port_t port;
	void *args;
{
	if (port != IP_NULL && !io_active(&port->ip_object))
		db_show_uid(port, args);
}


void
db_count_proxy(port, args)
ipc_port_t	port;
void		*args;
{
	int *countp = (int *) args;

	if (port != IP_NULL && port->ip_norma_is_proxy)
		++*countp;
}

void
db_count_principal(port, args)
ipc_port_t	port;
void		*args;
{
	int *countp = (int *) args;

	if (port != IP_NULL && !port->ip_norma_is_proxy)
		++*countp;
}


void
db_count_null_port(port, args)
ipc_port_t	port;
void		*args;
{
	int *countp = (int *) args;

	if (port == IP_NULL)
		++*countp;
}


void
db_count_pullrecv_proxy(port, args)
ipc_port_t	port;
void		*args;
{
	int *countp = (int *) args;

#if	MACH_ASSERT
	if (port != IP_NULL && port->ip_norma_is_proxy
	    && (port->ip_norma_spare2 == 556104 ||
		port->ip_norma_spare2 == -556104))
		++*countp;
#else
	*countp = -1;
#endif
}


#define	IKOT_MAX_TYPE	20
unsigned int ikot_types[IKOT_MAX_TYPE];
char *ikot_names[IKOT_MAX_TYPE] = {	/* Strings from kern/ipc_kobject.h */
    "(none)       ",	/* 0 */
    "(thread)     ",	/* 1 */
    "(task)       ",	/* 2 */
    "(host)       ",	/* 3 */
    "(host_priv)  ",	/* 4 */
    "(processor)  ",	/* 5 */
    "(pset)       ",	/* 6 */
    "(pset_name)  ",	/* 7 */
    "(pager)      ",	/* 8 */
    "(paging_req) ",	/* 9 */
    "(device)     ",	/* 10 */
    "(xmm_object) ",	/* 11 */
    "(xmm_pager)  ",	/* 12 */
    "(xmm_kernel) ",	/* 13 */
    "(xmm_reply)  ",	/* 14 */
    "(pager_term) ",	/* 15 */
    "(paging_name)",	/* 16 */
    "(host_paging)",	/* 17 */
    "(unused)     ",	/* 18 */
    "(unused)     ",	/* 19 */
};

void
db_count_kotype(port, args)
ipc_port_t	port;
void		*args;
{
	int *countp = (int *) args;

	if (port != IP_NULL && !port->ip_norma_is_proxy) {
		int type = ip_kotype(port);
		if (type >= 0 && type < IKOT_MAX_TYPE) {
			ikot_types[type]++;
			++*countp;
		}
	}
}


void
db_count_dead_port(port, args)
ipc_port_t	port;
void		*args;
{
	int *countp = (int *) args;

	if (port != IP_NULL && !io_active(&port->ip_object))
		++*countp;
}


db_show_all_uids()
{
	int count = 0;

	norma_port_table_statistics();
	norma_port_iterate(db_show_uid, (void *) &count);
	return(count);
}

db_show_all_uids_verbose()
{
	norma_port_table_statistics();
	norma_port_iterate(db_show_uid_verbose, (void *) 0);
	return(0);
}

db_show_all_proxies()
{
	int count = 0;

	norma_port_iterate(db_show_proxy, (void *) &count);
	return(count);
}

db_show_all_principals()
{
	int count = 0;

	norma_port_iterate(db_show_principal, (void *) &count);
	return(count);
}

db_show_all_deads()
{
	int count = 0;

	norma_port_iterate(db_show_dead, (void *) &count);
	return(count);
}

db_count_all_proxies()
{
	int	count = 0;

	norma_port_iterate(db_count_proxy, (void *) &count);
	kdbprintf("total proxies:  0x%x\n", count);
	return(count);
}

db_count_all_principals()
{
	int	count = 0;

	norma_port_iterate(db_count_principal, (void *) &count);
	kdbprintf("total principals:  0x%x\n", count);
	return(count);
}

db_count_all_null_ports()
{
	int	count = 0;

	norma_port_iterate(db_count_null_port, (void *) &count);
	kdbprintf("total null ports:  0x%x\n", count);
	return(count);
}

db_count_all_pullrecv_proxies()
{
	int	count = 0;

	norma_port_iterate(db_count_pullrecv_proxy, (void *) &count);
	kdbprintf("total pullrecv proxies:  0x%x\n", count);
	return(count);
}

db_count_all_kotypes()
{
	int	i, count = 0;

	for (i = 0; i < IKOT_MAX_TYPE; ++i)
		ikot_types[i] = 0;
	norma_port_iterate(db_count_kotype, (void *) &count);
	for (i = 0; i < IKOT_MAX_TYPE; ++i)
		kdbprintf("ikot type %d%s ports:  0x%x\n",
				i, ikot_names[i], ikot_types[i]);
	kdbprintf("Total count: ");
	return(count);
}

db_count_all_dead_ports()
{
	int	count = 0;

	norma_port_iterate(db_count_dead_port, (void *) &count);
	kdbprintf("total dead ports:  0x%x\n", count);
	return(count);
}


db_norma_memory()
{
	/*
	 *	netipc_vm_map_copy_list
	 *	netipc_safe_ikm_free_list
	 *	norma_kmsg_incomplete_queue_head
	 *	norma_kmsg_complete
	 *	kernel_kmsg
	 *	kernel_critical_kmsg
	 *	netipc_page_list
	 *	norma_kmsg_receive_error
	 *	netipc_dead_port_list
	 *	netipc_safe_vm_map_copy_discard_list
	 *	netipc_continuing_packet_list
	 *	all ports in NORMA
	 *	io_done_list
	 *	pcb_free_list (comes from pcb_zone)
	 *
	 *	fix netipc page pool allocation (less aggressive)
	 *	kserver thread allocation (less aggressive)
	 *	fixed priority for all threads (vnode pager, iodone, etc.)
	 *
	 *	shut off netipc as a function of wire count, too
	 *		...detect "shoot-up" conditions
	 *	send messages out more slowly when suspended
	 *	make pageout scan MORE aggressive
	 */
}

#endif	/* MACH_KDB */

