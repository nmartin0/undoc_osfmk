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
 * deltat_utils.c
 *
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: deltat_utils.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:10:49 $";
#endif
/*
 * Hash table and queueing utilities for the deltat-t transport protocol.
 */


#include	"netmsg.h"
#include	"nm_defs.h"

#if	USE_DELTAT

#define DELTAT_UTILS_DEBUG	(debug.deltat & 0x8)

#include <pthread.h>
#include <sys/errno.h>
#include <sys/time.h>

#include "nm_defs.h"
#include "debug.h"
#include "deltat.h"
#include "deltat_defs.h"
#include "lock_queue.h"
#include "ls_defs.h"
#include "mem.h"
#include "netmsg.h"
#include "nm_extra.h"
#include "sbuf.h"
#include "timer.h"


/*
 * Hash table definitions - just hash on the host_low field of a host address.
 */
typedef struct deltat_hash_entry_ {
    struct lock_queue	dh_queue;
    netaddr_t		dh_host_id;
    long		dh_last_seq_no;
} deltat_hash_t, *deltat_hash_ptr_t;

typedef struct {
    pthread_mutex_t	dt_ht_lock;
    int			dt_ht_size;
    int			dt_ht_inc;
    int			dt_ht_num_entries;
    int			dt_ht_limit;
    deltat_hash_ptr_t	*dt_ht_entries;
} deltat_hash_table_t;

static deltat_hash_table_t	deltat_hash_tables[2];

#define INIT_HASH_TABLE_SIZE	256
#define INIT_HASH_TABLE_LIMIT	200
#define HASH_TABLE_ENTRY_SIZE	(sizeof(deltat_hash_ptr_t))


/*
 * The current incarnation and event sequence number of this network server.
 */
static long current_incarnation = 0;
static long current_sequence_no = 0;


/*
 * deltat_utils_init
 *	Initialises:
 *	    the event hash tables and their locks,
 *	    the current incarnation and sequence numbers.
 *
 */
PUBLIC void deltat_utils_init()
BEGIN("deltat_utils_init")
    int			i;
    struct timeval	tp;
    struct timezone	tzp;
    deltat_hash_ptr_t	*new_hash_table;

    MEM_ALLOC(new_hash_table,deltat_hash_ptr_t *,	
			(INIT_HASH_TABLE_SIZE*HASH_TABLE_ENTRY_SIZE), FALSE);
    for (i = 0; i < INIT_HASH_TABLE_SIZE; i++) new_hash_table[i] = (deltat_hash_ptr_t)0;
    deltat_hash_tables[DELTAT_SEND_HASH_TABLE_ID].dt_ht_entries = new_hash_table;
    pthread_mutex_create(&deltat_hash_tables[DELTAT_SEND_HASH_TABLE_ID].dt_ht_lock, pthread_mutexattr_default);
    deltat_hash_tables[DELTAT_SEND_HASH_TABLE_ID].dt_ht_size = INIT_HASH_TABLE_SIZE;
    deltat_hash_tables[DELTAT_SEND_HASH_TABLE_ID].dt_ht_inc = 1;
    deltat_hash_tables[DELTAT_SEND_HASH_TABLE_ID].dt_ht_num_entries = 0;
    deltat_hash_tables[DELTAT_SEND_HASH_TABLE_ID].dt_ht_limit = INIT_HASH_TABLE_LIMIT;

    MEM_ALLOC(new_hash_table,deltat_hash_ptr_t *,	
			(INIT_HASH_TABLE_SIZE*HASH_TABLE_ENTRY_SIZE), FALSE);
    for (i = 0; i < INIT_HASH_TABLE_SIZE; i++) new_hash_table[i] = (deltat_hash_ptr_t)0;
    deltat_hash_tables[DELTAT_RECV_HASH_TABLE_ID].dt_ht_entries = new_hash_table;
    pthread_mutex_create(&deltat_hash_tables[DELTAT_RECV_HASH_TABLE_ID].dt_ht_lock,  pthread_mutexattr_default);
    deltat_hash_tables[DELTAT_RECV_HASH_TABLE_ID].dt_ht_size = INIT_HASH_TABLE_SIZE;
    deltat_hash_tables[DELTAT_RECV_HASH_TABLE_ID].dt_ht_inc = 1;
    deltat_hash_tables[DELTAT_RECV_HASH_TABLE_ID].dt_ht_num_entries = 0;
    deltat_hash_tables[DELTAT_RECV_HASH_TABLE_ID].dt_ht_limit = INIT_HASH_TABLE_LIMIT;

    (void)gettimeofday(&tp, &tzp);
    current_incarnation = (long)tp.tv_sec;
    current_sequence_no = 0; 

    RET;
END



/*
 * deltat_grow_hash_table
 *	doubles the size of one of the hash tables.
 *
 * Parameters:
 *	hash_table_id	: which hash table to grow
 *
 * Side effects:
 *	a new hash table with the entries from the old one rehashed into it
 *
 * Design:
 *	Create a new hash table that is twice as big as the old one.
 *	Rehash the entries from the old table into the new one.
 *	Delete the old table.
 *
 * Note:
 *	We are responsible for our own locking;
 *	(assume that caller has not left the hash table locked).
 *
 */
PRIVATE void deltat_grow_hash_table(hash_table_id)
int	hash_table_id;
BEGIN("deltat_grow_hash_table")
    int			old_size, new_size, i, hash_table_inc;
    deltat_hash_ptr_t	*new_hash_table, *old_hash_table;

    pthread_mutex_lock(&deltat_hash_tables[hash_table_id].dt_ht_lock);

    old_size = deltat_hash_tables[hash_table_id].dt_ht_size;
    new_size = deltat_hash_tables[hash_table_id].dt_ht_size = 2 * old_size;
    hash_table_inc = deltat_hash_tables[hash_table_id].dt_ht_inc = old_size + 1;
    deltat_hash_tables[hash_table_id].dt_ht_limit *= 2;

    /*
     * Now create and initialise the new hash table.
     */
    old_hash_table = deltat_hash_tables[hash_table_id].dt_ht_entries;
    MEM_ALLOC(new_hash_table,deltat_hash_ptr_t *,	
				(new_size * HASH_TABLE_ENTRY_SIZE), FALSE);
    for (i = 0; i < new_size; i++) new_hash_table[i] = (deltat_hash_ptr_t)0;
    deltat_hash_tables[hash_table_id].dt_ht_entries = new_hash_table;

    /*
     * Rehash the entries from the old hash table into the new one.
     */
    for (i = 0; i < old_size; i++) {
	ip_addr_t		host_ip_addr;
	int			index;

	host_ip_addr.ia_netaddr = old_hash_table[i]->dh_host_id;;
	index = host_ip_addr.ia_bytes.ia_host_low;	

	while (new_hash_table[index] != (deltat_hash_ptr_t)0) {
	    index = (index + hash_table_inc) % new_size;
	}

	new_hash_table[index] = old_hash_table[i];
    }

    pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
    MEM_DEALLOC((pointer_t)old_hash_table, (old_size * HASH_TABLE_ENTRY_SIZE));
END



/*
 * deltat_hash_enter
 *
 * Parameters:
 *	host_id	: the host to create a new event list for in the hash table
 *
 * Results:
 *	A pointer to the event list for this host.
 *
 * Notes:
 *	We must lock the hash table for mutual exclusion.
 *	If the host already exists in the hash table,
 *	then just return a pointer to its event list.
 *
 */
PUBLIC lock_queue_t deltat_hash_enter(host_id, hash_table_id)
register netaddr_t	host_id;
int			hash_table_id;
BEGIN("deltat_hash_enter")
    ip_addr_t			host_ip_addr;
    register int		index;
    int				original_index;
    register deltat_hash_ptr_t	*hash_table;
    int				hash_table_size, hash_table_inc;

    host_ip_addr.ia_netaddr = host_id;
    original_index = index = host_ip_addr.ia_bytes.ia_host_low;

    pthread_mutex_lock(&deltat_hash_tables[hash_table_id].dt_ht_lock);

    deltat_hash_tables[hash_table_id].dt_ht_num_entries ++;
    if (deltat_hash_tables[hash_table_id].dt_ht_num_entries
	> deltat_hash_tables[hash_table_id].dt_ht_limit)
    {
	pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
	deltat_grow_hash_table(hash_table_id);
	pthread_mutex_lock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
    }

    hash_table = deltat_hash_tables[hash_table_id].dt_ht_entries;
    hash_table_size = deltat_hash_tables[hash_table_id].dt_ht_size;
    hash_table_inc = deltat_hash_tables[hash_table_id].dt_ht_inc;

    while ((hash_table[index] != (deltat_hash_ptr_t)0)
		&& (hash_table[index]->dh_host_id != host_id))
    {
	index = (index + hash_table_inc) % hash_table_size;
	if (index == original_index) {
	    pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
	    LOG1(TRUE, 4, 1072, hash_table_id);
	    deltat_grow_hash_table(hash_table_id);
	    /*
	     * Retry this call to enter.
	     */
	    {
		register lock_queue_t	lq;
		lq = deltat_hash_enter(host_id, hash_table_id);
		RETURN(lq);
	    }
	}
    }

    if (hash_table[index] != (deltat_hash_ptr_t)0) {
	pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
	LOG0(TRUE, 2, 1073);
	RETURN(&hash_table[index]->dh_queue);
    }

    /*
     * Otherwise give this host the current record.
     */
    MEM_ALLOC(hash_table[index],deltat_hash_ptr_t,sizeof(deltat_hash_t), FALSE);
    lq_init(&hash_table[index]->dh_queue);
    hash_table[index]->dh_host_id = host_id;
    hash_table[index]->dh_last_seq_no = 0;

    pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
    RETURN(&hash_table[index]->dh_queue);

END



/*
 * deltat_hash_lookup
 *
 * Parameters:
 *	host_id	: the host to look up
 *
 * Results:
 *	A pointer to the event list for this host (0 if not found).
 *
 * Notes:
 *	Must lock the hash table for mutual exclusion.
 *
 */
PUBLIC lock_queue_t deltat_hash_lookup(host_id, hash_table_id)
register netaddr_t	host_id;
int			hash_table_id;
BEGIN("deltat_hash_lookup")
    ip_addr_t			host_ip_addr;
    register int		index;
    int				original_index;
    register deltat_hash_ptr_t	*hash_table;
    int				hash_table_size, hash_table_inc;

    host_ip_addr.ia_netaddr = host_id;
    original_index = index = host_ip_addr.ia_bytes.ia_host_low;

    hash_table = deltat_hash_tables[hash_table_id].dt_ht_entries;
    hash_table_size = deltat_hash_tables[hash_table_id].dt_ht_size;
    hash_table_inc = deltat_hash_tables[hash_table_id].dt_ht_inc;
    pthread_mutex_lock(&deltat_hash_tables[hash_table_id].dt_ht_lock);

    while ((hash_table[index] != (deltat_hash_ptr_t)0)
		&& (hash_table[index]->dh_host_id != host_id))
    {
	index = (index + hash_table_inc) % hash_table_size;
	if (index == original_index) {
	    pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);
	    panic("deltat_hash_lookup: hash table full");
	}
    }
    pthread_mutex_unlock(&deltat_hash_tables[hash_table_id].dt_ht_lock);

    if (hash_table[index] == (deltat_hash_ptr_t)0) {
	RETURN(LOCK_QUEUE_NULL);
    }
    else {
	RETURN(&hash_table[index]->dh_queue);
    }

END



/*
 * queue_item_equal
 *	Checks to see whether the uid of the input queue entry is equal to the input uid.
 *
 * Parameters:
 *	uid_ptr		: a pointer to a uid
 *	queue_entry	: a pointer to a queue entry
 *
 * Results:
 *	TRUE if the uids are equal, FALSE otherwise.
 *
 */
#define queue_item_equal(queue_entry, uid_ptr) 				   \
    ((((deltat_event_ptr_t)queue_entry)->dte_uid.deltat_uid_incarnation == \
	((deltat_uid_t *)uid_ptr)->deltat_uid_incarnation) && 		   \
     (((deltat_event_ptr_t)queue_entry)->dte_uid.deltat_uid_sequence_no == \
	((deltat_uid_t *)uid_ptr)->deltat_uid_sequence_no))


#if	0
	XXX Not used right now. XXX
/*
 * queue_item_less_than
 *	Checks to see whether the uid of the input queue entry is less than to the input uid.
 *
 * Parameters:
 *	uid_ptr		: a pointer to a uid
 *	queue_entry	: a pointer to a queue entry
 *
 * Results:
 *	TRUE if the entry uid is less than the input uid, FALSE otherwise.
 *
 */
PRIVATE int queue_item_less_than(queue_entry, uid_ptr)
register deltat_event_ptr_t	queue_entry;
register deltat_uid_t		*uid_ptr;
BEGIN("queue_item_less_than")
    RETURN((queue_entry->dte_uid.deltat_uid_incarnation < uid_ptr->deltat_uid_incarnation)
	|| ((queue_entry->dte_uid.deltat_uid_incarnation == uid_ptr->deltat_uid_incarnation)
	    && (queue_entry->dte_uid.deltat_uid_sequence_no < uid_ptr->deltat_uid_sequence_no)))
END
#endif	0



/*
 * deltat_event_lookup
 *	Locate an event given a host id and an event uid.
 *
 * Parameters:
 *	host_id		: the host that we are interested in
 *	event_uid	: the uid of the event to locate
 *	hash_table_id	: in which hash table to do the event queue lookup
 *
 * Results:
 *	a pointer to the event (DELTAT_NULL_EVENT if the event was not found)
 *
 */
PUBLIC deltat_event_ptr_t deltat_event_lookup(host_id, event_uid, hash_table_id)
netaddr_t		host_id;
deltat_uid_t		event_uid;
int			hash_table_id;
BEGIN("deltat_event_lookup")
    register lock_queue_t		event_queue;
    register cthread_queue_item_t		dep;

    if ((event_queue = deltat_hash_lookup(host_id, hash_table_id)) == LOCK_QUEUE_NULL) {
	RETURN(DELTAT_NULL_EVENT);
    }
    else {
	lq_find_macro(event_queue, queue_item_equal, &event_uid, dep);
	RETURN((deltat_event_ptr_t)dep);
    }
END



/*
 * deltat_destroy_recv_event
 *	Removes a recv event from an event queue.
 *	Frees any storage associate with that event.
 *
 * Parameters:
 *	event_ptr	: a pointer to the event to be destroyed
 *
 */
PUBLIC void deltat_destroy_recv_event(event_ptr)
register deltat_event_ptr_t	event_ptr;
BEGIN("deltat_destroy_recv_event")
    register lock_queue_t	event_queue;
    int				tmp;

    DEBUG1(DELTAT_UTILS_DEBUG, 3, 1074, event_ptr);

    if ((event_queue = event_ptr->dte_event_q) == LOCK_QUEUE_NULL) {
	ERROR((msg, "deltat_destroy_recv_event.deltat_hash_lookup fails."));
	RET;
    }

    lq_remove_macro(event_queue, (cthread_queue_item_t)event_ptr, tmp);
    if (!tmp) {
	ERROR((msg, "deltat_destroy_recv_event.lq_remove_macro fails."))
    }

    /*
     * Clear the lock.  Deallocate the sbuf if it was allocated explicitly.
     * noop
     * mutex_clear(&event_ptr->dte_lock);
     */
    if (event_ptr->dte_data.size > 1) SBUF_FREE(event_ptr->dte_data);

    /*
     * Now deallocate the event itself.
     */
    MEM_DEALLOCOBJ(event_ptr, MEM_DTEVENT);

    RET;

END



/*
 * deltat_destroy_send_event
 *	Removes a send event from an event queue.
 *	Frees any storage associate with that event.
 *
 * Parameters:
 *	event_ptr	: a pointer to the event to be destroyed
 *
 * Notes:
 *	Assumes that the event is already locked.
 *
 */
PUBLIC void deltat_destroy_send_event(event_ptr)
register deltat_event_ptr_t	event_ptr;
BEGIN("deltat_destroy_send_event")
    register lock_queue_t	event_queue;
    int				tmp;

    DEBUG1(DELTAT_UTILS_DEBUG, 3, 1075, event_ptr);

    if ((event_queue = event_ptr->dte_event_q) == LOCK_QUEUE_NULL) {
	ERROR((msg, "deltat_destroy_send_event.deltat_hash_lookup fails."));
	RET;
    }

    lq_remove_macro(event_queue, (cthread_queue_item_t)event_ptr, tmp);
    if (!tmp) {
	ERROR((msg, "deltat_destroy_send_event.lq_remove_macro fails."))
    }

    /*
     * Clear the lock and deallocate the event itself.
     */
    MEM_DEALLOCOBJ(event_ptr, MEM_DTEVENT);

    RET;

END



/*
 * deltat_get_recv_event
 *	Returns an event suitable for receiving data from a remote host.
 *
 * Parameters:
 *	host_id		: the remote host
 *	new_uid		: the incoming uid
 *
 * Results:
 *	a pointer to the new event (DELTAT_NULL_EVENT if failure)
 *
 * Side effects:
 *	May make a new event if a suitable event does not already exist.
 *	May make an entry in the recv hash table if this host is not already entered.
 *	Queues the new event in the event queue associated with the host.
 *
 */
PUBLIC deltat_event_ptr_t deltat_get_recv_event(host_id, new_uid)
register netaddr_t	host_id;
deltat_uid_t		new_uid;
BEGIN("deltat_get_recv_event")
    register lock_queue_t	event_queue;
    cthread_queue_item_t		event_found_ptr;
    deltat_event_ptr_t		new_event_ptr;

    if (((event_queue = deltat_hash_lookup(host_id, DELTAT_RECV_HASH_TABLE_ID)) == LOCK_QUEUE_NULL)
	&& ((event_queue = deltat_hash_enter(host_id, DELTAT_RECV_HASH_TABLE_ID)) == LOCK_QUEUE_NULL))
    {
	ERROR((msg, "deltat_get_recv_event.deltat_hash_enter fails."));
	RETURN(DELTAT_NULL_EVENT);
    }
    else {
	deltat_hash_ptr_t	hash_ptr = (deltat_hash_ptr_t)event_queue;
	if (new_uid.deltat_uid_sequence_no > hash_ptr->dh_last_seq_no) {
	    /*
	     * This is definitely a new event.
	     */
	    hash_ptr->dh_last_seq_no = new_uid.deltat_uid_sequence_no;
	}
	else {
		lq_find_macro(event_queue, queue_item_equal, 
					&new_uid, event_found_ptr);
		if ((deltat_event_ptr_t)event_found_ptr != DELTAT_NULL_EVENT)
		{
			    RETURN((deltat_event_ptr_t)event_found_ptr);
		}
	}
    }

    /*
     * Create a new event.
     */
    MEM_ALLOCOBJ(new_event_ptr,deltat_event_ptr_t,MEM_DTEVENT);
    pthread_mutex_create(&new_event_ptr->dte_lock, pthread_mutexattr_default);
    new_event_ptr->dte_host_id = host_id;
    new_event_ptr->dte_status = DELTAT_RECEIVING;

    new_event_ptr->dte_uid = new_uid;
    new_event_ptr->dte_pkt_seq_no = 0;

    SBUF_SEG_INIT(new_event_ptr->dte_data, &new_event_ptr->dte_seg);
    SBUF_SEEK(new_event_ptr->dte_data, new_event_ptr->dte_data_pos, 0);
    new_event_ptr->dte_in_packets = 0;

    /*
     * Initialise the receive time-out structure and place it on the deltat_recv_queue.
     */
    new_event_ptr->dte_recv_event.link = 0;
    new_event_ptr->dte_recv_event.dtre_event_ptr = new_event_ptr;
    new_event_ptr->dte_recv_event.dtre_active = TRUE;
    lq_prequeue(&deltat_recv_queue, (cthread_queue_item_t)&new_event_ptr->dte_recv_event.link);

    /*
     * Now actually insert the event into the event queue.
     */
    new_event_ptr->dte_event_q = event_queue;
    lq_prequeue(event_queue, (cthread_queue_item_t)new_event_ptr);

    DEBUG1(DELTAT_UTILS_DEBUG, 3, 1076, new_event_ptr);
    RETURN(new_event_ptr);

END



/*
 * deltat_make_send_event
 *	Makes and enters an event suitable for sending data to a remote host.
 *
 * Parameters:
 *	host_id		: the remote host
 *
 * Results:
 *	a pointer to the new event (DELTAT_NULL_EVENT if failure)
 *
 * Side effects:
 *	May make an entry in the send hash table if this host is not already entered.
 *	Queues the new event in the event queue associated with the host.
 *
 * Note:
 *	Assumes that the event does not already exist.
 *
 */
PUBLIC deltat_event_ptr_t deltat_make_send_event(host_id)
register netaddr_t	host_id;
BEGIN("deltat_make_send_event")
    register lock_queue_t	event_queue;
    register deltat_event_ptr_t	new_event_ptr;

    if (((event_queue = deltat_hash_lookup(host_id, DELTAT_SEND_HASH_TABLE_ID)) == LOCK_QUEUE_NULL)
	&& ((event_queue = deltat_hash_enter(host_id, DELTAT_SEND_HASH_TABLE_ID)) == LOCK_QUEUE_NULL))
    {
	ERROR((msg, "deltat_make_send_event.deltat_hash_enter fails."));
	RETURN(DELTAT_NULL_EVENT);
    }

    /*
     * Create a new event.
     */
    MEM_ALLOCOBJ(new_event_ptr,deltat_event_ptr_t,MEM_DTEVENT);
    pthread_mutex_create(&new_event_ptr->dte_lock, pthread_mutexattr_default);
    new_event_ptr->dte_host_id = host_id;
    new_event_ptr->dte_status = DELTAT_SENDING;

    current_sequence_no ++;
    new_event_ptr->dte_uid.deltat_uid_sequence_no = current_sequence_no;
    new_event_ptr->dte_uid.deltat_uid_incarnation = current_incarnation;
    new_event_ptr->dte_pkt_seq_no = 0;

    /* Initialiase the timer. */
    new_event_ptr->dte_timer.interval.tv_sec = param.deltat_retry_sec;
    new_event_ptr->dte_timer.interval.tv_usec = param.deltat_retry_usec;
    new_event_ptr->dte_timer.action = deltat_retry;
    new_event_ptr->dte_timer.info = (char *)new_event_ptr;

    /*
     * Now actually insert the event into the event queue.
     */
    new_event_ptr->dte_event_q = event_queue;
    lq_prequeue(event_queue, (cthread_queue_item_t)new_event_ptr);

    DEBUG1(DELTAT_UTILS_DEBUG, 3, 1077, new_event_ptr);
    RETURN(new_event_ptr);

END


#else	USE_DELTAT
	/*
	 * Just a dummy to keep the loader happy.
	 */
static int	dummy;
#endif	USE_DELTAT

