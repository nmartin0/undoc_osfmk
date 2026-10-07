/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * Functions to support dipc special ports
 *
 * $Id: dipc_uid.c,v 1.8 1995/03/28 03:08:27 stans Exp $
 *
 * HISTORY:
 */

#include "mach_kdb.h"
#include "mach_assert.h"

#include <mach/norma_special_ports.h>
#include <ipc/ipc_port.h>
#include <norma2/dipc_uid.h>
#include <norma2/norma_log.h>

#define	UID_HASH_SIZE	128		/* must be a power of 2 */
#define	UID_HASH_MASK	(UID_HASH_SIZE - 1)

/*
 *  UID globals
 */
decl_simple_lock_data(, dipc_uid_table_lock)
ipc_port_t		dipc_uid_table[ UID_HASH_SIZE ];
unsigned long		dipc_next_sequence_id;
dipc_uid_stats_t	dipc_uid_stats;

/*
 *  MACROS
 */
#define	LOCK_UID_TABLE()	simple_lock(&dipc_uid_table_lock)
#define	UNLOCK_UID_TABLE()	simple_unlock(&dipc_uid_table_lock)
#define DIPC_UID_HASH(i)	((i +  NODE_FROM_UID(i)) & UID_HASH_MASK)


/*
 *  Name:	dipc_uid_init()
 *
 *  Input:	None
 *
 *  Output:	UID hash table is initialized.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	This function should only be called once.
 *
 *  Description:	Initialze the hash table and initial UID value.
 */
void
dipc_uid_init()
{
	int	i;

	uid_entry0(dipc_uid_init);

	/*
	 *  Initialize the starting value for UID sequence numbers.
	 */
	dipc_next_sequence_id = MAX_SPECIAL_ID + 1;

	/*
	 *  Initialize the UID table.
	 */
	simple_lock_init(&dipc_uid_table_lock);
	for (i = 0; i < UID_HASH_SIZE; i++)
		dipc_uid_table[ i ] = IP_NULL;

	/*
	 *  Clear of the stats structure.
	 */
	bzero(&dipc_uid_stats, sizeof(dipc_uid_stats));
}

/*
 *  Name:	dipc_port_lookup_locked()
 *
 *  Input:	UID value to lookup.
 *
 *  Output:	None.
 *
 *  Returns:		Pointer to port associated with the UID or IP_NULL
 *			if UID could not be found in the table.
 *
 *  MP and locking
 *  consideration:	Assumes caller already holds the dipc_uid_table_lock
 *			and that interrupt level code will not insert UID's.
 *
 *  Description:	Search the dipc_uid_table for the spceified UID.
 *			It is assumed that the UID has already been stripped
 *			of any transit bits.
 */
static ipc_port_t	
dipc_port_lookup_locked(uid)
	dipc_uid_t	uid;
{
	ipc_port_t	p;
	int		i;

	uid_entry1(dipc_port_lookup_locked, uid);

	if (uid == DIPC_UID_NULL)
		return IP_NULL;

	UID_STATS(lookups++);

	p = dipc_uid_table[ DIPC_UID_HASH(uid) ];
	i = 0;
	while (p) {
		i++;
		if (p->dipc_uid == uid) {
			if (UID_STATS(max_lookup_depth < i ))
				UID_STATS(max_lookup_depth = i);
			return (p);
		}
		p = p->dipc_uid_next;
	}

	UID_STATS(lookup_failures++);
	if (UID_STATS(max_lookup_depth < i ))
		UID_STATS(max_lookup_depth = i);

	return (IP_NULL);
}

/*
 *  Name:	dipc_uid_install_locked()
 *
 *  Input:	Pointer to the port the specified UID is being installed
 *		against.
 *
 *  Output:	Specified port is linked into the dipc_uid_table and a
 *		reference is taken on the port.
 *
 *  Returns:		DIPC_SUCESS is returned if this is a unique UID that
 *			is successfull installed into the dipc_uid_table.
 *			DIPC_DUPLICATE is returned if the specified UID
 *			matches an already installed UID.
 *
 *  MP and locking
 *  consideration:	The dipc_uid_table is locked while the search and
 *			installation take place.  Spl level is set to sploff
 *			at the time of installation to allow lookups from
 *			interrupt level.  Assumes that interrupt level code
 *			will not call this function.
 *
 *  Description:	Search the dipc_uid_table for the specified UID.
 *			If the UID is not found, insert the port into the
 *			dipc_uid_table.  Otherwise indicate this is a
 *			duplicate.  It is assumed that the UID has already
 *			been stripped of any transit bits.
 *
 * Side Effects:
 *			node and uid fields are in port structure.
 */

static dipc_return_t
dipc_uid_install_locked(port, uid)
	ipc_port_t	port;
	dipc_uid_t	uid;
{
	ipc_port_t	*pp;

	uid_entry2(dipc_uid_install_locked, port, uid);

	assert(ip_active(port));
	UID_STATS(installs++);

	/*
	 *  was a UID already assigned?
	 */
	if ( DIPC_UID_VALID(port) ) {
		UID_STATS(install_collision++);
		return (DIPC_DUPLICATE);
	}

	/*
	 *  This UID better not already be installed on another port!
	 */
	if ( dipc_port_lookup_locked(uid) != IP_NULL ) {
		UID_STATS(install_collision++);
		return (DIPC_DUPLICATE);
	}

	/*
	 *  Insert it.
	 */
	pp = &dipc_uid_table[ DIPC_UID_HASH(uid) ];
	if (*pp != IP_NULL) {
		int	old;

		port->dipc_uid_next = *pp;
		old = sploff();
		*pp = port;
		splon(old);
	} else {
		*pp = port;
	}

	/*
	 *  Take a reference on the port.
	 */
	ip_reference(port);

	/*
	 * store uid and node in the port structure
	 */
	ip_lock( port );

	port->dipc_uid = (unsigned long) uid;

	/*
	 *  Squirrel away a copy of the UID that will not be cleared when
	 *  this UID is destroyed.
	 */
	port->dipc_exported_uid = (unsigned long) uid;

	port->dipc_node = NODE_FROM_UID( uid );

	ip_unlock( port );

	return (DIPC_SUCCESS);
}

/*
 *  Name:	dipc_uid_allocate()
 *
 *  Input:	Pointer to port UID is beging allocated for.
 *
 *  Output:	A UID is created and installed in the ipc_uid_table.
 *
 *  Returns:		DIPC_SUCCESS is all went well.
 *			DIPC_DUPLICATE if the port already has a UID assigned.
 *
 *  MP and locking
 *  consideration:	The dipc_uid_table is locked while the UID is being
 *			constucted and inserted.  It is assumed that interrupt
 *			code will not call this funtion.
 *
 *  Description:	This function will assign a unique UID for the port
 *			passed into the call.  If a UID has already been
 *			assigned, DIPC_DUPLICATE will be returned.  Otherwise
 *			the port reference count is incremented and
 *			DIPC_SUCCESS is returned.
 *
 */
dipc_return_t
dipc_uid_allocate(port)
	ipc_port_t	port;
{
	dipc_uid_t	uid;

	uid_entry1(dipc_uid_allocate, port);

	UID_STATS(allocations++);

	/*
	 *  The UID may have already been assigned.
	 */
	if ( DIPC_UID_VALID(port) ) {
		UID_STATS(duplicate_allocate++);
		return (DIPC_DUPLICATE);
	}

	/*
	 *  Lock the UID table and generate a unique ID.  If using
	 *  dipc_port_lookup_locked() is too slow, we'll go to a
	 *  bitfield arrangement.
	 */
	LOCK_UID_TABLE();
	do {
		uid = CREATE_UID(dipc_next_sequence_id);
		dipc_next_sequence_id++;
		if (dipc_next_sequence_id == UID_SEQID_MAX)
			dipc_next_sequence_id = MAX_SPECIAL_ID + 1;

		/*
		 *  True collisions will be derived by subtracting
		 *  new_uid_collisions - allocations.
		 */
		UID_STATS(new_uid_collisions++);
	} while (dipc_port_lookup_locked(uid) != IP_NULL);

	/*
	 *  Install the new UID.  This will take a reference on the port.
	 */
	if (dipc_uid_install_locked(port, uid) != DIPC_SUCCESS)
		panic("dipc_uid_allocate(): unexpected duplicate UID");

	UNLOCK_UID_TABLE();
	return (DIPC_SUCCESS);
}

/*
 *  Name:	dipc_uid_install()
 *
 *  Input:	Pointer to port and the UID to install against the port.
 *
 *  Output:	Passed UID is installed against the port.   A reference
 *		is taken against the passed port.
 *
 *  Returns:		DIPC_SUCCESS if all went well.
 *			DIPC_DUPLICATE if a UID has already been assigned.
 *
 *  MP and locking
 *  consideration:	The dipc_uid_table will be locked while the UID
 *			is installed.  It is assumed that interrupt level
 *			code will not install a UID.
 *
 *  Description:	Descrive function.
 *
 */
dipc_return_t
dipc_uid_install(port, uid)
	ipc_port_t	port;
	dipc_uid_t	uid;
{
	dipc_return_t	ret;

	uid_entry2(dipc_uid_install, port, CLEAN_UID(uid));

	assert( ! DIPC_UID_VALID(port) );

	/*
	 *  Lock the UID table and install UID.
	 */
	LOCK_UID_TABLE();              /* strip transit bits */
	ret = dipc_uid_install_locked(port, CLEAN_UID(uid));
	UNLOCK_UID_TABLE();

	return (ret);
}

/*
 *  Name:	dipc_port_lookup()
 *
 *  Input:	UID to port lookup.
 *
 *  Output:	None.
 *
 *  Returns:		Pointer to port with passed UID or IP_NULL if the
 *			UID could not be found.
 *
 *  MP and locking
 *  consideration:	The dipc_uid_table will be locked while the UID
 *			looked up.  It is assumed that interrupt level
 *			code will not install a UID.
 *
 *  Description:	Descrive function.
 *
 */
ipc_port_t
dipc_port_lookup(uid)
	dipc_uid_t	uid;
{
	ipc_port_t	port;

	uid_entry1(dipc_port_lookup, CLEAN_UID(uid));

	/*
	 *  Lock the UID table and try to find the specified UID.
	 */
	LOCK_UID_TABLE();          /* strip transit bits */
	port = dipc_port_lookup_locked(CLEAN_UID(uid));
	UNLOCK_UID_TABLE();

	return (port);
}

/*
 *  Name:	dipc_uid_destroy()
 *
 *  Input:	Pointer to the port associated with the UID to destroy.
 *
 *  Output:	Port containing the specified UID is removed from the
 *		dipc_uid_table.
 *
 *  Returns:		DIPC_SUCCESS is returned if all went well.
 *			DIPC_NO_UID is returned if the UID could not be found.
 *
 *  MP and locking
 *  consideration:	The dipc_uid_table is locked while the search and
 *			removal take place.  Spl level is set to sploff
 *			at the time of removal to allow lookups from interrupt
 *			level.  Assumes that interrupt level code will not
 *			call this function.
 *
 *  Description:	The UID associated with the specified port is removed
 *			from the dipc_uid_table.
 *
 */
dipc_return_t
dipc_uid_destroy(port)
	ipc_port_t	port;
{
	ipc_port_t	*pp;
	ipc_port_t	p;

	uid_entry1(dipc_uid_destroy, port);

	assert(DIPC_UID_VALID(port));
	assert(!ip_active(port));
	
	UID_STATS(destroys++);

	pp = &dipc_uid_table[ DIPC_UID_HASH( port->dipc_uid ) ];
	LOCK_UID_TABLE();
	for (p = *pp; p; pp = &p->dipc_uid_next, p = *pp) {
		if (p == port) {
			int	old;

			old = sploff();
			*pp = port->dipc_uid_next;
			splon(old);

			port->dipc_uid_next = IP_NULL;

			assert(port->dipc_exported_uid == port->dipc_uid);
			port->dipc_uid = DIPC_UID_NULL;

			ip_release(port);
			return (DIPC_SUCCESS);
		}
	}
	UNLOCK_UID_TABLE();

	UID_STATS(destroys_no_uid++);
	return (DIPC_NO_UID);
}

#if	MACH_KDB

db_uid_stats()
{
	dipc_uid_stats_t	*p = &dipc_uid_stats;

	db_printf("dipc UID stats:\n");
	db_printf("  UID allocs   = %8d        dups = %8d    collisions = %d\n",
			p->allocations,
			p->duplicate_allocate,
			p->new_uid_collisions - p->allocations);
	db_printf("  UID installs = %8d        dups = %8d    collisions = %d\n",
			p->installs,
			p->duplicate_install,
			p->install_collision);
	db_printf("  UID lookups  = %8d    failures = %8d    max depth  = %d\n",
			p->lookups,
			p->lookup_failures,
			p->max_lookup_depth);
	db_printf("  UID destroys = %8d   not found = %8d\n",
			p->destroys,
			p->destroys_no_uid);
	return (0);
}

void
db_show_dipc_uid( dipc_uid_t uid )
{
	ipc_port_t	port;

	if ( (port=dipc_port_lookup( uid )) == IP_NULL ) {
		db_printf("invalid uid 0x%x\n",uid);
		return;
	}
	ipc_port_print(port);
}

#endif	/* MACH_KDB */
