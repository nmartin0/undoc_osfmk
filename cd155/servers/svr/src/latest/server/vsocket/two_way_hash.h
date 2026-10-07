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
 * Copyright (c) 1994-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: two_way_hash.h,v $
 * Revision 1.3  1995/02/01  23:19:45  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.2  1994/11/18  20:51:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/04/27  23:31:24  yazz
 *  Reviewer: Charlie Johnson, Bob Yasi
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
 * TNC select rewrite.  This file implements generic two-way associative
 * hash tables based on the one-way hash tables in <uxkern/port_hash.h>.
 *
 */

/*
 *  two_way_hash.h ---
 *
 *  Generic implementation of two-way associative port name
 *  hash tables with locking, based on OSF/1 hash table code.
 *
 *  A two-way hash table is really two one-way hash tables (as implemented
 *  in uxkern/port_hash.[ch]).  A lookup using either of the two keys
 *  returns the address of a twh_entry_t structure, which contains the
 *  values of both keys plus some bytes of use-specific data.  You
 *  can use two-way hash tables to record one-to-one mappings between
 *  arbitrary 32-bit values (e.g. between select ids and reply port
 *  names), and you can also store additional data associated with
 *  such mappings.
 */

#include <uxkern/port_hash.h>
#include <kern/lock.h>
#include <net/net_globals.h>

#if	MACH_ASSERT
#  define	TWHDEBUG(s)	s
#else
#  define	TWHDEBUG(s)
#endif

/*
 *  The following definitions may be overridden by the file #including
 *  this one.
 */

#ifndef	TWH_MALLOC
#  define TWH_MALLOC(addr, cast, size) \
	VS_MALLOC((addr), cast, (size), VSM_TWHE)
#  define TWH_FREE(addr)	VS_FREE((addr), VSM_TWHE)
#endif	/* ! TWH_MALLOC */

#ifndef TWH_NBUCKETS
#define TWH_NBUCKETS	63
#endif

/*
 *  Structure definitions for two-way hash tables and their entries.
 */
typedef struct two_way_hash {
	port_hash_table_t	twh_key_to_data;
	port_hash_table_t	twh_data_to_key;
	lock_data_t		twh_lock;
	TWHDEBUG(int 		twh_entries;)
} two_way_hash_t;

#define TWH_LOCK_INIT(ht)	lock_init(&(ht).twh_lock, 1/*sleepable*/)
#define TWH_WRITE_LOCK(ht)	lock_write(&(ht).twh_lock)
#define TWH_READ_LOCK(ht)	lock_read(&(ht).twh_lock)
#define TWH_LOCK_UPGRADE(ht)	lock_read_to_write(&(ht).twh_lock)
#define TWH_UNLOCK(ht)		lock_done(&(ht).twh_lock)
#define TWH_ISLOCKED(ht)	lock_islocked(&(ht).twh_lock)

/*
 *  Both hash tables in a two_way_hash_t point to a twh_entry_t.
 */
typedef struct twh_entry_header {
	unsigned int	teh_key1;	/* primary key */
	unsigned int	teh_key2;	/* secondary key */
} twh_ehdr_t;

typedef struct twh_entry {
	twh_ehdr_t	te_header;
	char		te_data[1];	/* use-specific data */
} twh_entry_t;
#define	te_key1		te_header.teh_key1
#define	te_key2		te_header.teh_key2

/*
 *  Access macros.
 */

/* Initialization. */
#define TWH_INIT(x)						\
MACRO_BEGIN							\
	(x).twh_key_to_data = port_hash_init(TWH_NBUCKETS);	\
	(x).twh_data_to_key = port_hash_init(TWH_NBUCKETS);	\
	TWH_LOCK_INIT(x);					\
	TWHDEBUG((x).twh_entries = 0);				\
MACRO_END

/* Enter ep into hash tables, return TRUE if successful. */
#define TWH_ENTER(ht, ep, rc)						\
MACRO_BEGIN								\
	twh_entry_t	*twe = (twh_entry_t *)(ep);			\
									\
	LOCK_ASSERT("TWH_ENTER", TWH_ISLOCKED(ht));			\
	ASSERT(twe->te_key1 && twe->te_key2);				\
	(rc) = port_hash_enter((ht).twh_key_to_data, twe->te_key1, twe);\
	if (rc) {							\
		(rc) = port_hash_enter((ht).twh_data_to_key,		\
				       twe->te_key2, twe);		\
		TWHDEBUG((ht).twh_entries++);				\
	}								\
MACRO_END

/* Remove entry ep from hash tables. */
#define TWH_REMOVE_KEY(ht, ep)						\
MACRO_BEGIN								\
	int		rc;						\
	twh_entry_t	*twe = (twh_entry_t *)(ep);			\
									\
	LOCK_ASSERT("TWH_REMOVE_KEY", TWH_ISLOCKED(ht));		\
	rc = port_hash_remove((ht).twh_key_to_data, twe->te_key1);	\
	ASSERT(rc == TRUE);						\
	rc = port_hash_remove((ht).twh_data_to_key, twe->te_key2);	\
	ASSERT(rc != FALSE);						\
	TWHDEBUG((ht).twh_entries--);					\
MACRO_END

/*
 *  New style lookup macros return the entry pointer.
 */
#define TWH_LOOKUP_1ST_KEY(ht, ky, ep, cast) \
MACRO_BEGIN \
	(ep) = (cast) port_hash_lookup((ht).twh_key_to_data, (ky)); \
MACRO_END

#define TWH_LOOKUP_2ND_KEY(ht, dt, ep, cast) \
MACRO_BEGIN \
	(ep) = (cast) port_hash_lookup((ht).twh_data_to_key, (dt)); \
MACRO_END


#ifdef	NOTDEF
/*
 *  Backward compatibility.
 */
#define TWH_LOOKUP_BY_KEY(x, key, data)					\
MACRO_BEGIN								\
	twh_entry_t	*twe; \
	twe = (twh_entry_t *) port_hash_lookup((x).twh_key_to_data, (key)); \
	(data) = (twe ? (mach_port_t) twe->te_key2 : NULL); \
MACRO_END

#define TWH_LOOKUP_BY_DATA(x, data, key)				\
MACRO_BEGIN								\
	twh_entry_t	*twe; \
	twe = (twh_entry_t *) port_hash_lookup((x).twh_data_to_key, (data)); \
	(key) = (twe? (sel_id_t) twe->te_key1 : NULL); \
MACRO_END
#endif	/* NOTDEF */
