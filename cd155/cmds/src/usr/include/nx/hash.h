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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/hash.h,v 1.4 1994/11/19 02:44:42 mtm Exp $
 *
 */


/*
 * hash.h	- Don Cameron	3/92
 *
 * This file contains the exported interface of the hash module of the
 * allocator server.
 */
#ifndef _hash_h
#define _hash_h

#include <nx/list.h>
 
#define HASH_TBL_LENGTH 1009	/* Size of hash table: prime closest to 1024 */
#define BUCKET_SIZE     14	/* Number of slots in hash bucket */

/*
 *  Hash bucket, linked in case more than BUCKET_SIZE collisions.
 */
typedef struct bucket { 
        struct bucket		*prev;
        struct bucket		*next; 
	struct {
		void		*p;	/* Pointer to object */
		unsigned long	key;	/* Lookup key */
	} slot[BUCKET_SIZE];
} BUCKET_T; 

/*
 * Hash table entry.
 */
typedef BUCKET_T	*HASH_TBL_T;

/*
 * Exported routines.
 */
int hash_tbl_insert();
void * hash_tbl_lookup();
void hash_tbl_delete();
void hash_tbl_init();

#endif	/* ! _hash_h */
