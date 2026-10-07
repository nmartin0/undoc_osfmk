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
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: vs_subr.h,v $
 * Revision 1.6  1995/02/01  23:34:16  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:52:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/05/19  21:50:54  chrisp
 * Change quotes for inclusion of two_way_hash.h from ""s to <>s to
 * correct compilation using gcc on i386 platforms
 *
 * Revision 1.3  1994/05/04  22:26:47  mjl
 * Merge revision 1.1.4.2 from R1_2 branch into main trunk.
 *
 * Revision 1.1.4.2  1994/04/27  23:10:13  yazz
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
 * TNC select rewrite.  Implement the select-id-to-drp associative hash
 * table; table entries hold a file port svrref and a vsocket ref.  New
 * VS_MALLOC/VS_FREE macros use NET_MALLOC.  A select-id-to-secondary-socket
 * hash table indicates whether a select is enqueued on a secondary.
 *
 * Revision 1.2  1993/12/07  17:46:43  mjl
 * Reimplement two-way hash tables as generic macros.
 *
 *
 *  Reviewer: cfj@ssd.intel.com, bhk@locus.com
 *  Risk: low
 *  Benefit or PTS #: 7272
 *  Testing: Locus network tests
 *  Module(s): server/vsocket/sys_vsocket.c, server/vsocket/vs_init.c,
 * 	server/vsocket/vs_subr.c, server/vsocket/vs_subr.h
 *
 * Revision 1.1  1993/09/01  01:55:26  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 3.4  93/08/23  00:26:58  mjl
 * [LCCbug #0370] Add extern decl for create_vs_opts().
 * 
 * Revision 3.3  93/08/22  09:41:46  bhk
 * Fixed the prototype for vs_soreadable()
 * 
 * Revision 3.2  93/08/19  15:27:48  bhk
 * added prototypes to new function calls [#349]
 * 
 * Revision 3.1  93/08/06  18:00:03  bhk
 * prototype declarations of vs_soreadable
 * 
 * Revision 3.0  93/08/03  16:54:24  mjl
 * Macros and definitions for support of networking domain virtual sockets.
 * 
 */

/*
 *  This file contains miscellaneous virtual socket related macro
 *  definitions.
 *
 *  XXX Only definitions relating to the virtual socket operation
 *  macros VSOP_*() themselves belong in vsocket.h; only definitions
 *  relating to keeping MiG happy belong in vs_types.h .  Lots of
 *  stuff now in vs_types.h belongs here instead.  Make it so!!!
 */

#ifndef _VS_SUBR_H_
#define _VS_SUBR_H_

#include <sys/types.h>
#include <kern/macro_help.h>
#include <kern/lock.h>
#include <uxkern/port_hash.h>
#ifndef	MACH_ASSERT
#include <mach_assert.h>
#endif
#include <net/net_malloc.h>

#include <vsocket/vs_types.h>


/*
 *  Heap allocation for virtual socket data structures.
 */

extern caddr_t	malloc();
extern void	free();

#define	VSM_OPT		0
#define VSM_OPTVAL	1
#define VSM_RVS		2
#define	VSM_NSRVDATA	3
#define VSM_IFINFO	4
#define VSM_MISOFTC	5
#define VSM_MIASSOC	6
#define VSM_IFCONF	7
#define VSM_TWHE	8
#define VSM_MAX		9

#if	MACH_ASSERT
extern void	increment_vsm_count(int);
extern void	decrement_vsm_count(int);
#else
#define	increment_vsm_count(x)
#define	decrement_vsm_count(x)
#endif

#define	VS_MALLOC(addr, cast, size, type)		\
MACRO_BEGIN						\
	NET_MALLOC(addr, cast, size, M_TEMP, M_WAITOK);	\
	if ((addr) != NULL) {				\
		increment_vsm_count(type);		\
	}						\
MACRO_END

#define	VS_FREE(addr, type)				\
MACRO_BEGIN						\
	NET_FREE(addr, M_TEMP);				\
	decrement_vsm_count(type);			\
MACRO_END


/*
 *  Hash tables for TNC select(2) support.
 *
 *  TNC uses hash tables to coordinate distributed select(2) system
 *  calls.  They map between "select ids" and other entities.  A
 *  select id is a cluster-unique 32-bit identifier assigned each time
 *  a delayed select is done on a socket.  TNC uses select ids to
 *  coordinate select activity across nodes.
 *
 *  ZZZ elaborate
 */

#define	VS_NBUCKETS	63

/*
 *  Select IDs are used to coordinate select operations across all
 *  secondary virtual sockets.  They are assigned each time a delayed
 *  select is done on a primary virtual socket.  The node number is
 *  encoded in the sel_id_t in anticipation of eventual support for
 *  primary socket migration.
 *
 *  XXX High 12 bits of select id is node number (because Paragon TNC
 *  supports 4096 == 2^12 nodes.  Remaining 20 bits are taken from a
 *  per-node counter.  Basically we want a 32-bit system-wide uniquifier.
 */

/*
 *  This NODESHIFT #define is from <tnc/dpvproc_struct.h>.
 *  (XXX I think fundamental TNC constants like this should be split
 *  out into a separate TNC header file.)
 */

#define NODESHIFT	16
#if	MACH_ASSERT
#define SELNODESHIFT	NODESHIFT
#define LOCSELIDMAX	0xFFF		/* fast wrap for debugging */
#else
#define SELNODESHIFT	20
#define LOCSELIDMAX	((1<<20)-1)	/* slow wrap for production */
#endif

typedef unsigned long	sel_id_t;
#define SEL_ID_NULL	((sel_id_t)0)

extern sel_id_t	get_select_id(void);

/*
 *  The selid/drp map is a two-way map between select ids and delayed
 *  reply port send-once rights.
 *
 *  Note that SELID_DRP_LOOKUP() and DRP_SELID_LOOKUP() macros take a
 *  reference on the map entry, and this reference must later be released
 *  with SDM_UNREF(), which see.
 */

#include <vsocket/two_way_hash.h>

typedef struct selid_drp_map_entry {
	twh_ehdr_t	sdm_ehdr;		/* keys: selid and drp */
	int		sdm_refcnt;		/* when 0, delete the entry */
	int		sdm_flags;		/* see below */
	struct socket	*sdm_so;		/* socket using this selid */
	struct file	*sdm_fp;		/* file pointing to socket */
	decl_simple_lock_data(,sdm_lock)	/* interlock on refcnt */
} selid_drp_map_t;
#define sdm_selid	sdm_ehdr.teh_key1
#define sdm_drp		sdm_ehdr.teh_key2

/* Selid/drp map entry flags */
#define	SDM_DEAD	0x01	/* Set when sel_wakeup() discovers drp dead */
#define SDM_SCRUBBED	0x02	/* Prevent lookups, it's about to be scrubbed */

/* Short duration lock to protect sdm_refcnt */
#define SDM_LOCK_INIT(sdm)	simple_lock_init
#define SDM_LOCK(sdm)		simple_lock(&(sdm)->sdm_lock)
#define SDM_UNLOCK(sdm)		simple_unlock(&(sdm)->sdm_lock)

#define	SDM_REF(sdm)		\
MACRO_BEGIN			\
	SDM_LOCK(sdm);		\
	sdm->sdm_refcnt++;	\
	SDM_UNLOCK(sdm);	\
MACRO_END

/*
 *  NB Care must be taken when calling SDM_UNREF when the SOCKET_LOCK is
 *  held on sdm->sdm_so , because DECREMENT_VSNET_REFCNT demands that
 *  the SOCKET_LOCK *not* be held.  So you must be sure you are not
 *  removing the last sdm referrence if you hold the SOCKET_LOCK.
 */
#define SDM_UNREF(sdm)							\
MACRO_BEGIN								\
	SDM_LOCK(sdm);							\
	ASSERT(sdm->sdm_refcnt > 0);					\
	if (sdm->sdm_refcnt > 1) {					\
		sdm->sdm_refcnt--;					\
		SDM_UNLOCK(sdm);					\
	} else {							\
		SDM_UNLOCK(sdm);					\
		TWH_WRITE_LOCK(ht_selid_drp);				\
		TWH_REMOVE_KEY(ht_selid_drp, sdm);			\
		TWH_UNLOCK(ht_selid_drp);				\
		DECREMENT_VSNET_REFCNT(sdm->sdm_so, "SDM_UNREF");	\
		fp_unref_port(sdm->sdm_fp, -1);				\
		TWH_FREE(sdm);						\
	}								\
MACRO_END

/*
 *  Like SDM_UNREF(), but is intended to be called under the SOCKET_LOCK
 *  when dequeueing a dead select.  Insures that after SOCKET_LOCK is
 *  released, no new lookups can find this entry and dequeue a dead
 *  select twice.
 */
#define SDM_SCRUB(sdm)				\
MACRO_BEGIN					\
	SDM_LOCK(sdm);				\
	ASSERT(sdm->sdm_refcnt > 1);		\
	sdm->sdm_flags |= SDM_SCRUBBED;		\
	sdm->sdm_refcnt--;			\
	SDM_UNLOCK(sdm);			\
MACRO_END

extern two_way_hash_t	ht_selid_drp;

#define	SELID_DRP_DECL	\
	two_way_hash_t		ht_selid_drp

#define SELID_DRP_INIT	TWH_INIT(ht_selid_drp)

#define SELID_DRP_INSERT(fp, drp, sdm) \
	(sdm) = do_selid_drp_insert((fp), (drp))

extern selid_drp_map_t	*do_selid_drp_insert(struct file *, mach_port_t);

#define	SELID_DRP_LOOKUP(selid, sdm)					\
MACRO_BEGIN								\
	TWH_READ_LOCK(ht_selid_drp);					\
	TWH_LOOKUP_1ST_KEY(ht_selid_drp, (selid), (sdm), selid_drp_map_t *); \
	if ((sdm) == NULL || ((sdm)->sdm_flags & SDM_SCRUBBED)) {	\
		(sdm) = NULL;						\
	} else {							\
		SDM_REF(sdm);						\
	}								\
	TWH_UNLOCK(ht_selid_drp);					\
MACRO_END

#define	DRP_SELID_LOOKUP(drp, sdm)					\
MACRO_BEGIN								\
	TWH_READ_LOCK(ht_selid_drp);					\
	TWH_LOOKUP_2ND_KEY(ht_selid_drp, (drp), (sdm), selid_drp_map_t *); \
	if ((sdm) == NULL || ((sdm)->sdm_flags & SDM_SCRUBBED)) {	\
		(sdm) = NULL;						\
	} else {							\
		SDM_REF(sdm);						\
	}								\
	TWH_UNLOCK(ht_selid_drp);					\
MACRO_END

#define SELID_DRP_LOOKUP_DONE(sdm)	SDM_UNREF(sdm)


/*
 *  One-way map from select id to secondary virtual socket.
 *  Used to decide if the select id needs to be requeued at
 *  this remote secondary node---an entry indicates that the
 *  select id is on some secondary socket's queue at this node.
 */

extern struct lock vs_2ary_lock;

#define VS_SECNDRY_LOCK_DECL	struct lock vs_2ary_lock
#define VS_SECNDRY_LOCK_INIT	lock_init(&vs_2ary_lock, 1/*sleepable*/)
#define VS_SECNDRY_LOCK		lock_write(&vs_2ary_lock)
#define VS_SECNDRY_READ_LOCK	lock_read(&vs_2ary_lock)
#define VS_SECNDRY_UNLOCK	lock_done(&vs_2ary_lock)
#define VS_SECNDRY_ISLOCKED	lock_islocked(&vs_2ary_lock)

extern port_hash_table_t	ht_selid_svs;

#define SELID_TO_SVS_DECL \
	port_hash_table_t	ht_selid_svs

#define SELID_TO_SVS_INIT \
	ht_selid_svs = port_hash_init(VS_NBUCKETS)

#define	SELID_TO_SVS_ENTER(selid, socket_port)			\
MACRO_BEGIN							\
	int	rc;						\
	ASSERT(VS_SECNDRY_ISLOCKED);				\
	rc = port_hash_enter(ht_selid_svs, (selid), (socket_port)); \
	ASSERT(rc == TRUE);					\
MACRO_END

#define SELID_TO_SVS_LOOKUP(selid, vs)				\
MACRO_BEGIN							\
	ASSERT(VS_SECNDRY_ISLOCKED);				\
	(vs) = (struct socket *) port_hash_lookup(ht_selid_svs, (selid)); \
MACRO_END

#define SELID_TO_SVS_REMOVE(selid)				\
MACRO_BEGIN							\
	int	rc;						\
	ASSERT(VS_SECNDRY_ISLOCKED);				\
	rc = (int) port_hash_remove(ht_selid_svs, (selid));	\
	ASSERT(rc == TRUE);					\
MACRO_END

/*
 *  Flag values for select queue entries (qp->selq_flags), typically
 *  set via uth->uu_sel_flags when an entry is enqueued.
 */
#define	SQ_VSOCK	0x01	/* Use TNC select enqueue, dequeue, etc. */
#define SQ_DRP		0x02	/* selq_delay_port is valid port right */
#define SQ_DEAD		0x04	/* delay port has become a dead name */
#define SQ_REMOTE_REPLY	0x08	/* bsd_sel_poll_reply() came from remote node */
#define SQ_DEQUEUED	0x10	/* Set if select_dequeue() dequeued something */

/*
 *  Translate address of socket select queue to address of socket.
 */

#define selq2so(sq) \
	(struct socket *) ((uint_t)(sq) - \
			   (uint_t)(&((struct socket *)0)->so_rcv.sb_selq))

/*
 *  Miscellaneous function prototypes
 */
extern vs_socket_t *	vs_soreadable(struct socket *vs, boolean_t nbio);
extern node_t		vs_rvs_node_number(vs_socket_t *rvs);
extern vs_socket_t * 	vs_get_new_rvs(struct socket *vs);
extern int create_vs_opts(struct socket *vs, int type, int level, int cmd, 
			  int len, char *data);
extern void selid2drp_print(two_way_hash_t *);

#if	MACH_ASSERT
#define	ASSERT_DRP_UNMAPPED(xdrp, islocked, args) \
MACRO_BEGIN \
	selid_drp_map_t	*sdm; \
	\
	DRP_SELID_LOOKUP((xdrp), sdm); \
	if (sdm) { \
		printf("selid/drp %x/%x STILL MAPPED (sdm 0x%x): ", \
		       sdm->sdm_selid, (xdrp), sdm); \
		printf args ; \
		print_port_info((xdrp), "delay port"); \
		panic("drp was mapped!\n"); \
	} \
MACRO_END
#else
#define	ASSERT_DRP_UNMAPPED(xdrp, islocked, args)
#endif

#endif  /* ! _VS_SUBR_H_ */
