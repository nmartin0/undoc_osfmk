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
 * $Log: radix.h,v $
 * Revision 1.5  1994/11/18  20:34:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:45  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:00:07  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:29  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:57  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:41:15  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:33:59  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:32:46  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:36  gm]
 * 
 * Revision 1.7  90/07/27  08:59:40  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:49:59  tmt]
 * 
 * Revision 1.6  90/04/27  19:13:36  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:32:17  tmt]
 * 
 * Revision 1.5  90/04/14  00:32:35  devrcs
 * 	Use _KERNEL.
 * 	[90/04/09  16:18:15  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:12  gm
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/02  20:10:56  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:46:55  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1988, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	radix.h	7.3 (Berkeley) 4/22/89
 *	Merged:	radix.h	7.4 (Berkeley) 6/28/90
 */

/*
 * Radix search tree node layout.
 */

struct radix_node {
	struct	radix_mask *rn_mklist;	/* list of masks contained in subtree */
	struct	radix_node *rn_p;	/* parent */
	short	rn_b;			/* bit offset; -1-index(netmask) */
	char	rn_bmask;		/* node: mask for bit test*/
	u_char	rn_flags;		/* enumerated next */
#define RNF_NORMAL	1		/* leaf contains normal route */
#define RNF_ROOT	2		/* leaf is root leaf for tree */
#define RNF_ACTIVE	4		/* This node is alive (for rtfree) */
	union {
		struct {			/* leaf only data: */
			caddr_t	rn_Key;	/* object of search */
			caddr_t	rn_Mask;	/* netmask, if present */
			struct	radix_node *rn_Dupedkey;
		} rn_leaf;
		struct {			/* node only data: */
			int	rn_Off;		/* where to start compare */
			struct	radix_node *rn_L;/* progeny */
			struct	radix_node *rn_R;/* progeny */
		}rn_node;
	}		rn_u;
#ifdef RN_DEBUG
	int rn_info;
	struct radix_node *rn_twin;
	struct radix_node *rn_ybro;
#endif
};

#define MAXKEYLEN 32

#define rn_dupedkey rn_u.rn_leaf.rn_Dupedkey
#define rn_key rn_u.rn_leaf.rn_Key
#define rn_mask rn_u.rn_leaf.rn_Mask
#define rn_off rn_u.rn_node.rn_Off
#define rn_l rn_u.rn_node.rn_L
#define rn_r rn_u.rn_node.rn_R

/*
 * Annotations to tree concerning potential routes applying to subtrees.
 */

extern struct radix_mask {
	short	rm_b;			/* bit offset; -1-index(netmask) */
	char	rm_unused;		/* cf. rn_bmask */
	u_char	rm_flags;		/* cf. rn_flags */
	struct	radix_mask *rm_mklist;	/* more masks to try */
	caddr_t	rm_mask;		/* the mask */
	int	rm_refs;		/* # of references to this struct */
} *rn_mkfreelist;

#define MKGet(m) {\
	if (rn_mkfreelist) {\
		m = rn_mkfreelist; \
		rn_mkfreelist = (m)->rm_mklist; \
	} else \
		R_Malloc(m, struct radix_mask *, sizeof (*(m))); }\

#define MKFree(m) { (m)->rm_mklist = rn_mkfreelist; rn_mkfreelist = (m);}

extern struct radix_node_head {
	struct	radix_node_head *rnh_next;
	struct	radix_node *rnh_treetop;
	int	rnh_af;
	struct	radix_node rnh_nodes[3];
} *radix_node_head;


#ifndef _KERNEL
#define Bcmp(a, b, n) bcmp(((char *)(a)), ((char *)(b)), (n))
#define Bzero(p, n) bzero((char *)(p), (int)(n));
#define R_Malloc(p, t, n) (p = (t) malloc((unsigned int)(n)))
#define Free(p) free((char *)p);
#else
#define Bcmp(a, b, n) bcmp(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define Bcopy(a, b, n) bcopy(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define Bzero(p, n) bzero((caddr_t)(p), (unsigned)(n));
#define R_Malloc(p, t, n) NET_MALLOC(p, t, n, M_RTABLE, M_NOWAIT)
#define Free(p) NET_FREE(p, M_RTABLE);
#endif
