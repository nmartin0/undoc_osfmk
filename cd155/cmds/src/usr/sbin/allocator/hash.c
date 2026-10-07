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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/hash.c,v 1.5 1994/11/19 03:04:16 mtm Exp $
 *
 */

/*
 * hash.c	- Don Cameron	3/92
 *
 * This file contains has hash table lookup, insert, and delete routines used
 * by the allocator server.
 */
#include <nx/hash.h>
#include <nx/list.h>
#if OSF
#include <malloc.h>
#else
#include <libc.h>
#endif
 
/*
 * Straight from Knuth - Compute hash(K) by K mod size_of_hash_table, use
 * prime number for best spread.
 */
#define	HASH(k)	((int) ((unsigned long) (k) % (unsigned long) HASH_TBL_LENGTH))


/*
 * Forward references for internal routines.
 */
BUCKET_T *alloc_bucket();

/*
 * hash_tbl_init() - Initialize hash table.
 */
void
hash_tbl_init(hash_tbl)
HASH_TBL_T	*hash_tbl;	/* Hash table */
{
	int		i;

	for (i = 0; i < HASH_TBL_LENGTH; i++)
		hash_tbl[i] = (BUCKET_T *) 0;
}


/*
 * hash_tbl_insert() - Insert element in hash table.
 */
int
hash_tbl_insert(hash_tbl, key, p)
HASH_TBL_T	*hash_tbl;	/* Hash table */
unsigned long	key;		/* Key that will be used to do lookup */
void		*p;		/* Pointer to insert */
{
	int		i;
	BUCKET_T	*bucket;	/* Pointer to hash bucket */
	BUCKET_T	*b;

	/*
	 * Hash key to get pointer to first hash bucket.
	 */
	i = HASH(key);
	if (hash_tbl[i] == (BUCKET_T *)  0) {
		/* No bucket, allocate one */
		if ((hash_tbl[i] = alloc_bucket()) == 0) {
			return -1;
		}
	}
	bucket = hash_tbl[i];

	/*
	 * Find last bucket.
	 */
	while (bucket->next != (BUCKET_T *) 0) {
		bucket = bucket->next;
	}

	/* XXX */
	if (bucket->slot[BUCKET_SIZE - 1].p == (void *) 0) {
		/*
		 * No room in last bucket, allocate new bucket and insert
		 * in list.
		 */
		if ((b = alloc_bucket()) == 0) {
			return -1;
		}
		INSERT_AFTER(b, bucket);
		bucket = b;
	}
	/*
	 * Search for free slot and insert p and key.
	 */
	for (i = 0; bucket->slot[i].p != (void *) 0; i++);
	bucket->slot[i].p = p;
	bucket->slot[i].key = key;
	
	return 0;
}

/*
 * hash_tbl_lookup() - Return pointer to object associated with key,
 *                     or NULL pointer if not there.
 */
void *
hash_tbl_lookup(hash_tbl, key)
HASH_TBL_T	*hash_tbl;	/* Hash table */
unsigned long	key;		/* Lookup key */
{
	BUCKET_T	*bucket;	/* Pointer to hash bucket */
	int		i;

	/*
	 * Hash key to get pointer to first hash bucket.
	 */
	bucket = hash_tbl[HASH(key)];

	/*
	 * Search each bucket
	 */
	while (bucket != (BUCKET_T *) 0) {
		/*
		 * Search within bucket
		 */
		i = 0;
		while ((i < BUCKET_SIZE) &&
		       (bucket->slot[i].p != (void *) 0))
		{
			if (bucket->slot[i].key == key)
				return(bucket->slot[i].p);
			i++;
		}
		bucket = bucket->next;
	}

	/*
	 * If we got here there was no match, return null pointer.
	 */
	return((void *) 0);
}
	
/*
 * hash_tbl_delete() - Remove key and object pointer from hash table.
 */
void
hash_tbl_delete(hash_tbl, key)
HASH_TBL_T	*hash_tbl;	/* Hash table */
unsigned long	key;		/* Lookup key to remove */
{
	BUCKET_T	*match_bucket;	/* Hash bucket with matching key */
	int		match_slot;	/* Slot with matching key */
	BUCKET_T	*last_bucket;	/* Last hash bucket in list */
	BUCKET_T	*bucket;
	int		hash_value;
	int 		i;

	match_bucket = (BUCKET_T *) 0;

	/*
	 * Hash key to get pointer to first hash bucket.
	 */
	hash_value = HASH(key);
	bucket = hash_tbl[hash_value];

	/*
	 * Search each bucket
	 */
	while (bucket != (BUCKET_T *) 0) {
		/*
		 * Search within bucket
		 */
		i = 0;
		while ((i < BUCKET_SIZE) &&
		       (bucket->slot[i].p != (void *) 0))
		{
			if (bucket->slot[i].key == key) {
				match_bucket = bucket;
				match_slot = i;
			}
			i++;
		}
		last_bucket = bucket;
		bucket = bucket->next;
	}

	if (match_bucket == (BUCKET_T *) 0)
		/*
		 * No match, nothing to do.
		 */
		return;

	/*
	 * At this point:
	 *	match_bucket	- points to bucket with slot matching key
	 *	match_slot	- is slot of match_bucket which matches key
	 *	last_bucket	- points to last hash bucket in list
	 *	i		- is last slot in last_bucket
	 *
	 * Since keys are not ordered we can just copy last slot into
	 * slot we are vacating, and NULL out last slot.
	 */
	match_bucket->slot[match_slot].p = last_bucket->slot[i].p;
	match_bucket->slot[match_slot].key = last_bucket->slot[i].key;
	last_bucket->slot[i].p = (void *) 0;

	if (i == 0) {
		/*
		 * last_bucket is now empty, remove it from list and free it.
		 */
		REMOVE(hash_tbl[hash_value], last_bucket);
		free((void *) last_bucket);
	}
}

/*
 * alloc_bucket() - Allocate and initialize hash bucket.
 */
BUCKET_T *
alloc_bucket()
{
	BUCKET_T	*b;
	int		i;

	if ((b = (BUCKET_T *) malloc(sizeof(BUCKET_T)))) {
		for (i = 0; i < BUCKET_SIZE; i++) {
			b->slot[i].p = (void *) 0;
		}
		b->next = (BUCKET_T *) 0;
		b->prev = (BUCKET_T *) 0;
	}
	return b;
}
