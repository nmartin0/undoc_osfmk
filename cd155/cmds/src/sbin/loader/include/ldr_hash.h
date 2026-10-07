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
/* ldr_hash.h
 * Common declarations for all loader hashing functions
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_LDR_HASH
#define _H_LDR_HASH


/* The following declaration describes the action to be performed when
 * searching a hash table.  The following combinations are useful:
 *
 * LDR_HASH_LOOKUP		look up the specified key.  Return success
 *				and value if present, error if not present
 * LDR_HASH_INSERT		insert the specified key/value.  No error
 *				if the key is already present (this may
 *				result in duplicate keys).
 * LDR_HASH_DELETE		delete the specified key/value.  Does not
 *				return the value.  Return error if not present.
 * LDR_HASH_LOOKUP|LDR_HASH_INSERT insert the specified key/value.  If key is
 *				already present, don't insert duplicate, but
 *				return success and current value.
 * LDR_HASH_LOOKUP|LDR_HASH_INSERT delete the specified key/value.  Return
 *				the value on success, or error if not present.
 * LDR_HASH_INSERT|LDR_HASH_DELETE unspecified results.
 */

typedef	int	ldr_hash_action;	/* hash operation to perform: */
#define	LDR_HASH_LOOKUP ((ldr_hash_action)1) /* lookup key (no error if present) */
#define	LDR_HASH_INSERT ((ldr_hash_action)2) /* insert key (no error if not present) */
#define	LDR_HASH_DELETE ((ldr_hash_action)4) /* delete key (no error if present) */

/* Declarations of functions to hash and compare keys */

/* Hash a key, returning a value in the range 0..modulus. */

typedef	unsigned (*ldr_hash_p) __((const univ_t key, unsigned modulus));

/* Compare two keys, returning 0 for equality, nonzero for inequality 
 * (NOTE: like strcmp or memcmp, not like == )
 */

typedef int	 (*ldr_hash_compare_p) __((const univ_t key1, const univ_t key2));

/* Hash a string into an unsigned integer that is modulus the
 * specified modulus value.
 */

extern unsigned
hash_string __((const char *string, unsigned modulus));

#endif /* _H_LDR_HASH */
