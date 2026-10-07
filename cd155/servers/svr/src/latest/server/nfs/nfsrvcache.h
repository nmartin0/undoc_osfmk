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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: nfsrvcache.h,v $
 * Revision 1.5  1994/11/18  20:37:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:39:15  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:52  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:36:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:56  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:12  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:54:17  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:08  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:40:07  devrcs
 * 	Add RC_NOCACHE return code.
 * 	[90/10/02  17:51:47  tmt]
 * 
 * 	Make rc_saddr an mbuf pointer (and change name to rc_rnam)
 * 	to save entire requestor sockaddr in server cache.
 * 	[90/10/02  15:08:59  tmt]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:56  gm]
 * 
 * Revision 1.4  90/07/27  09:05:08  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:04:54  nags]
 * 
 * Revision 1.3  90/01/02  20:22:40  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:22:35  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)nfsrvcache.h	7.2 (Berkeley) 7/6/89
 */

/*
 * Definitions for the server recent request cache
 */

#define	NFSRVCACHESIZ	128
#define	NFSRCHSZ	32

struct nfsrvcache {
	struct	nfsrvcache *rc_chain[2];	/* Hash chain links */
	struct	nfsrvcache *rc_next;	/* Lru list */
	struct	nfsrvcache *rc_prev;
	int	rc_state;		/* Current state of request */
	int	rc_flag;		/* Flag bits */
	struct	mbuf  *rc_rnam;		/* Sockaddr of requestor */
	u_long	rc_xid;			/* rpc id number */
	int	rc_proc;		/* rpc proc number */
	long	rc_timestamp;		/* Time stamp */
	union {
		struct mbuf *rc_repmb;	/* Reply mbuf list OR */
		int rc_repstat;		/* Reply status */
	} rc_un;
	struct	rhead *rc_hash_chain;	/* associated hash chain */
	udecl_simple_lock_data(,rc_lock)
};

#define	rc_forw		rc_chain[0]
#define	rc_back		rc_chain[1]
#define	rc_status	rc_un.rc_repstat
#define	rc_reply	rc_un.rc_repmb

#define nfsrvhead_remove(rp) \
		(rp)->rc_prev->rc_next = (rp)->rc_next; \
		(rp)->rc_next->rc_prev = (rp)->rc_prev;

#define	nfsrvhead_add(rp) \
		(rp)->rc_next = nfsrvcachehead.rc_next; \
		(rp)->rc_next->rc_prev = (rp); \
		nfsrvcachehead.rc_next = (rp); \
		(rp)->rc_prev = &nfsrvcachehead;

#define	nfsrvhead_lruadd(rp) \
		nfsrvcachehead.rc_prev->rc_next = (rp); \
		(rp)->rc_prev = nfsrvcachehead.rc_prev; \
		(rp)->rc_next = &nfsrvcachehead; \
		nfsrvcachehead.rc_prev = (rp);

#define	put_at_head(rp) \
		nfsrvhead_remove(rp); \
		nfsrvhead_add(rp);

/* Cache entry states */
#define	RC_UNUSED	0
#define	RC_INPROG	1
#define	RC_DONE		2

/* Return values */
#define	RC_DROPIT	0
#define	RC_REPLY	1
#define	RC_DOIT		2
#define	RC_NOCACHE	3

/* Flag bits */
#define	RC_LOCKED	0x1
#define	RC_WANTED	0x2
#define	RC_REPSTATUS	0x4
#define	RC_REPMBUF	0x8

/* Delay time after completion that request is dropped */
#define	RC_DELAY	2		/* seconds */

#define	NSC_LOCK(rc)		usimple_lock(&(rc)->rc_lock)
#define	NSC_UNLOCK(rc)		usimple_unlock(&(rc)->rc_lock)
#define	NSC_LOCK_INIT(rc)	usimple_lock_init(&(rc)->rc_lock)

/*
 * Locking notes.  The fields in an nfsrvcache element
 * are protected in different ways, as listed below.
 * A "read-only" field is one initialized while the
 * cache element can't be found by another processor
 * and never changed while the element is "public".
 *
 * Field		Protected By
 * -----		------------
 * rc_chain[2]		rhead hash bucket lock
 * rc_next, rc_prev	nfsrvcachehead lock
 * rc_state		RC_LOCKED flag
 * rc_flag		cache element lock
 * rc_saddr		read-only
 * rc_xid		read-only
 * rc_proc		read-only
 * rc_timestamp		RC_LOCKED flag
 * rc_un		RC_LOCKED flag
 * rc_status		RC_LOCKED flag
 * rc_reply		RC_LOCKED flag
 * rc_hash_chain	read-only
 */
