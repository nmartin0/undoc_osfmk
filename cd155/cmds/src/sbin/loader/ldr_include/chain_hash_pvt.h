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
/* chain_hash_pvt.h
 * internal declarations for symbol table hashing functions
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_CHAIN_HASH_PVT
#define _H_CHAIN_HASH_PVT

/* Hash table; entries array is actually larger */

struct	hashtab_hdr	{
	int		hh_nelem;	/* number of slots */
	ldr_hash_p	hh_hasher;	/* function to hash a key */
	ldr_hash_compare_p hh_comper;	/* function to compare two keys */
};

typedef	struct	{			/* internal def for htab */
	struct hashtab_hdr	h_header; /* header of table */
	chain_hash_elem		*h_entries[1]; /* actually bigger */
} int_hashtab_t;

#define	h_nelem		h_header.hh_nelem
#define	h_hasher	h_header.hh_hasher
#define	h_comper	h_header.hh_comper

/* Compute the size of a hash table of a specified number of elements */

#define	hash_table_size(nelem)	(sizeof(struct hashtab_hdr) + ((nelem) * sizeof (chain_hash_elem *)))

#endif /* _H_CHAIN_HASH_PVT */
