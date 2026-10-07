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
/* open_hash.h
 * external declarations for symbol table hashing functions
 * Depends on <standards.h>, "ldr_hash.h"
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_OPEN_HASH
#define _H_OPEN_HASH

typedef	univ_t	open_hashtab_t;		/* opaque def for hash table (should be void *) */

typedef	int	open_hash_element_index; /* index of element in hash table */

/* Hash table flags */

typedef	int	open_hash_flags_t;	/* flags for tables, from: */

#define	OPEN_HASH_REBALANCE	((open_hash_flags_t)1) /* rebalance table on insert */


/* Insert the specified (key, value) pair into the specified hash table.
 * No error if the key already exists in the table (this may result in
 * duplicate keys; use open_hash_search(LDR_HASH_INSERT|LDR_HASH_LOOKUP)
 * if duplicate keys are possible).  Return negative error status on error.
 * WARNING: it must be possible to take the address of "value".
 * extern int open_hash_insert(open_hashtab_t table, const univ_t key, univ_t value);
 */

#define	open_hash_insert(table, key, value) \
	open_hash_search((table), (key), &(value), LDR_HASH_INSERT)

/* Lookup the specified key in the specified hash table, returning the 
 * value in *value.  Returns negative error status if the key is not
 * in the table.
 * extern int open_hash_lookup(open_hashtab_t table, const univ_t key, univ_t *value);
 */

#define	open_hash_lookup(table, key, value) \
	open_hash_search((table), (key), (value), LDR_HASH_LOOKUP)


/* Create a hash table large enough to hold at least nelem elements.
 * Returns zero on sucess or a negative error status on error.
 */

#define open_hash_create(n, h, c, f, t) \
	open_hash_create_heap(ldr_process_heap, (n), (h), (c), (f), (t))

/* Destroy a hash table.  Returns zero on success or a negative error
 * status on error.  Free the storage to the specified heap.
 * Note that this procedure does not do anything about freeing the
 * key/value pairs; it assumes that the table is empty or that this
 * has been otherwise taken care of.
 */

#define open_hash_destroy(o) \
	open_hash_destroy_heap(ldr_process_heap, (o))

/* Grow the specified hash table to be big enough to handle at least
 * nelem elements, and return the pointer to the new table in *table.
 * Deallocates the old table.  Returns 0 on success or negative error
 * status on error.
 */

#define open_hash_resize(h, n) \
	open_hash_resize_heap(ldr_process_heap, (h), (n))


/* Create a hash table large enough to hold at least nelem elements.  Allocate
 * the table from the specified heap.
 * Return a negative error status on error.
 */

extern int
open_hash_create_heap __((ldr_heap_t heap, int nelem, ldr_hash_p hasher,
			  ldr_hash_compare_p comper, open_hash_flags_t flags,
			  open_hashtab_t *table));

/* Search a hash table for a specified key.  If HASH_INSERT
 * is specified and the key is not present, the (key, value) pair is
 * inserted into the table.  If HASH_LOOKUP is specified and the key is
 * present, the value is returned.  (See the descriptions of ldr_hash_action
 * in "hash.h" for a detailed description of the behavior).  Returns zero on
 * success or a negative error status on error.
 */

extern int
open_hash_search __((open_hashtab_t table, const univ_t key, univ_t *value,
		     ldr_hash_action action));

/* Iterate through the elements of a hash table in an unspecified order.
 * Set *index to 0 to initialize the iteration; hash_elements() will modify
 * it to identify the next element to be returned on each call.  On success,
 * *str is set to the key of the next element, *value is set to its value.
 * Returns 0 on success, LDR_EAGAIN when there are no more elements, or other
 * negative error status on error.
 */

extern int
open_hash_elements __((open_hashtab_t table, open_hash_element_index *ix,
		       univ_t *key, univ_t *value));

/* Destroy a hash table.  Returns zero on success or a negative error
 * status on error.  Free the storage to the specified heap.
 * Note that this procedure does not do anything about freeing the
 * key/value pairs; it assumes that the table is empty or that this
 * has been otherwise taken care of.
 */

extern int
open_hash_destroy_heap __((ldr_heap_t heap, open_hashtab_t table));

/* Grow the specified hash table to be big enough to handle at least
 * nelem elements, and return the pointer to the new table in *table.
 * Use the specified heap for allocations and deallocations.
 * Deallocates the old table.  Returns 0 on success or negative error
 * status on error.
 */

extern int
open_hash_resize_heap __((ldr_heap_t heap, open_hashtab_t *table, int nelem));

/* Try to inherit the specified open hash table, presumably from a
 * keep-on-exec region or mapped file.  Only thing to do is to verify
 * the addresses of the procedure pointers in the table.
 * Returns LDR_SUCCESS on success or negative error status on error.
 */

extern int
open_hash_inherit __((open_hashtab_t table, ldr_hash_p hasher,
		      ldr_hash_compare_p comper));

#endif /* _H_OPEN_HASH */
