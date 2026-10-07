/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * $Id: dipc_uid.h,v 1.4 1995/03/28 02:58:09 stans Exp $
 *
 * HISTORY
 * $Log: dipc_uid.h,v $
 * Revision 1.4  1995/03/28  02:58:09  stans
 *  Changed DIPC_IS_SPECIAL() macro to include all special ports.
 *  Decreased non-kernel special uids from 40 --> 15. Unused uid range; reclaim
 *  wasted table space.
 *
 *  Reviewer:lenb
 *  Risk:medium-high
 *  Benefit or PTS #: 12734
 *  Testing:
 *         Installed & booted kernel on systems: 'a5', XPS150 and testjig.
 *         WW10 sats
 *         Developer tests:
 *                 Verified console logging works for kernel printf()'s at
 *                 interrupt and non-interrupt levels.
 *
 * Revision 1.3  1994/11/18  20:58:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:30:00  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.15  1994/07/11  16:49:46  rkl
 *  New and improved logging.
 *
 * Revision 1.1.2.14  1994/04/21  20:33:27  andyp
 * Added dipc_borg switch.
 *
 * Revision 1.1.2.13  1994/04/19  17:47:01  stans
 *   Name change: TRANSITS_FROM_UID --> EXTRACT_TRANSITS.
 * 	Return actual number of transits represented in the 'uid'.
 *
 * Revision 1.1.2.12  1994/04/01  04:09:43  stans
 * added DIPC_KOBJ_REPLY_PORT
 *
 * Revision 1.1.2.11  1994/03/28  18:24:38  rkl
 *  Fixed bug in INSERT_TRANSIT macro where '&&' was being used instead of '&'.
 *
 * Revision 1.1.2.10  1994/03/24  20:56:53  rkl
 *  More Transits-in-UID work
 *
 * Revision 1.1.2.9  1994/03/23  20:28:01  rkl
 *  Attempt to make lint(1) happy.
 *
 * Revision 1.1.2.8  1994/03/23  17:49:51  rkl
 *  Added support for UID to carry transits
 *
 * Revision 1.1.2.7  1994/03/15  00:33:40  rkl
 *  Made changes to support new norma logging capability.
 *
 * Revision 1.1.2.6  1994/03/10  19:04:35  stans
 *   Added:
 * 	typedef dipc_uid_seq_t
 * 	typedef dipc_node_t
 * 	MK_UID() to generate a uid for a node which is not the current node.
 * 	Kernel special uid support.
 *
 * Revision 1.1.2.5  1994/03/10  07:39:31  rkl
 *  Macro changes.
 *
 * Revision 1.1.2.4  1994/03/08  22:17:22  stans
 * added well-known UID
 *
 * Revision 1.1.2.3  1994/03/03  22:49:32  stans
 *   Removed "vi" introduced garbage into the file...
 *
 * Revision 1.1.2.2  1994/03/03  22:43:48  stans
 *   One of those "==" which should have been "!=" in DIPC_UID_VALID(p) macro.
 *
 * Revision 1.1.2.1  1994/03/03  22:39:35  stans
 *   Added DIPC_UID_NULL & DIPC_UID_VALID(port)
 *
 * Revision 1.1  1994/03/02  18:49:57  rkl
 * Initial revision
 *
 */

#ifndef	_DIPC_UID_H_
#define	_DIPC_UID_H_

#define	DIPC_SUCCESS	0
#define	DIPC_DUPLICATE	1
#define	DIPC_NO_UID	2
typedef	int		dipc_return_t;

typedef unsigned long   dipc_uid_t;
typedef unsigned long   dipc_uid_seq_t;
typedef unsigned long	dipc_node_t;
typedef unsigned long	dipc_transit_t;

/*
 *  The following defines only assume relative position of bits and try
 *  to be bit width independent.  It is assumed that the node number is
 *  in the MSBits, the sequence ID is in the LSBits, and transits are in
 *  the middle.
 *
 *    MSB                                                      LSB
 *    +------------------+----------+----------------------------+
 *    | NODE NUMBER BITS | TRANSITS |    SEQUENCE NUMBER BITS    |
 *    +------------------+----------+----------------------------+
 *
 */

/*
 *  Bit widths
 *
 *  12 bits for node             -> max 4096 nodes
 *   1 bit for transits          -> max 2
 *  19 bits for local id         -> max 524,288 exported ports per node
 */
#define	UID_BITS_TOTAL		(sizeof(dipc_uid_t) * 8)
#define	UID_BITS_IN_SEQID	19
#define	UID_BITS_IN_TRANSIT	1
#define	UID_BITS_IN_NODEID \
	(UID_BITS_TOTAL - UID_BITS_IN_TRANSIT - UID_BITS_IN_SEQID)

/*
 *  Shift widths
 */
#define	UID_NODE_SHIFT		(UID_BITS_TOTAL - UID_BITS_IN_NODEID)
#define	UID_TRANSIT_SHIFT	UID_BITS_IN_SEQID

/*
 *  Bit masks
 */
#define	UID_SEQID_MASK   \
	((dipc_uid_seq_t)(~0) >> (UID_BITS_TOTAL - UID_BITS_IN_SEQID))
#define	UID_NODE_MASK    \
	((dipc_node_t)   (~0) << UID_NODE_SHIFT)
#define	UID_TRANSIT_MASK \
	((dipc_transit_t)(~(UID_NODE_MASK | UID_SEQID_MASK)))

/*
 *  Maximum values
 */
#define	UID_SEQID_MAX		UID_SEQID_MASK
#define	UID_NODE_MAX		((dipc_node_t)(~0) >> UID_NODE_SHIFT))

/*
 *  Extraction macros
 */
#define CLEAN_UID(uid)		((dipc_uid_t)    (uid) & ~(UID_TRANSIT_MASK))

#define	SEQID_FROM_UID(uid)	((dipc_uid_seq_t)(uid) &  UID_SEQID_MASK)

#define	NODE_FROM_UID(uid)	((dipc_node_t)   (uid) >> UID_NODE_SHIFT)

#define	EXTRACT_TRANSITS(uid) \
	((((dipc_transit_t)(uid) & UID_TRANSIT_MASK) >> UID_TRANSIT_SHIFT)+1)

#define	INSERT_TRANSITS(uid, count) \
	((((count - 1) << UID_TRANSIT_SHIFT) & UID_TRANSIT_MASK) | uid)

#define CREATE_UID(seq) \
	(((dipc_node_t)(node_self()) << UID_NODE_SHIFT) | \
	 		((dipc_uid_seq_t)(seq) & UID_SEQID_MASK))
/*
 *  like CREATE_UID() except we supply the node number
 */
#define MK_UID(node, seq) \
	((((dipc_node_t)(node)) << UID_NODE_SHIFT) | \
			((dipc_uid_seq_t)(seq) & UID_SEQID_MASK))

#define	DIPC_UID_NULL		((dipc_uid_t)0)
#define DIPC_UID_DEAD		((dipc_uid_t)UID_SEQID_MAX)

#define	DIPC_UID_VALID(p) ((dipc_uid_t)((p)->dipc_uid) != DIPC_UID_NULL)

/*
 * from mach/norma_special_ports.h
 */
#define DIPC_MAX_SPECIAL_KERNEL_ID	10
#define DIPC_MAX_SPECIAL_ID		15

#define	DIPC_SPECIAL_UID(uid) \
	( (SEQID_FROM_UID((uid)) > 0) && \
		 (SEQID_FROM_UID((uid)) <= DIPC_MAX_SPECIAL_ID) )
/*
 * Reserved/special (well-known) UID's, indicies into host_special_port[]
 *
 * Provided by kernel
 */
#define DIPC_DEVICE_PORT	1
#define DIPC_HOST_PORT		2
#define DIPC_HOST_PRIV_PORT	3
#define DIPC_HOST_PAGING_PORT	4
#define	DIPC_KOBJ_PORT		5
#define	DIPC_KOBJ_REPLY_PORT	6

/*
 *  Stats structure.
 */
#define UID_STATS(a) dipc_uid_stats.a

typedef struct {
	unsigned long	allocations;
	unsigned long	duplicate_allocate;
	unsigned long	new_uid_collisions;

	unsigned long	installs;
	unsigned long	duplicate_install;
	unsigned long	install_collision;

	unsigned long	lookups;
	unsigned long	lookup_failures;
	unsigned long	max_lookup_depth;

	unsigned long	destroys;
	unsigned long	destroys_no_uid;
} dipc_uid_stats_t;

#define uid_log1	NORMA_LOG1
#define uid_log2	NORMA_LOG2
#define uid_log3	NORMA_LOG3
#define uid_log4	NORMA_LOG4
#define uid_log5	NORMA_LOG5
#define uid_log6	NORMA_LOG6

#define uid_entry0(func) \
	NORMA_ENTRY0(UID_LOG_ID, func)
#define uid_entry1(func, a)      \
	NORMA_ENTRY1(UID_LOG_ID, func, a)
#define uid_entry2(func, a, b) \
	NORMA_ENTRY2(UID_LOG_ID, func, a, b)
#define uid_entry3(func, a, b, c) \
	NORMA_ENTRY3(UID_LOG_ID, func, a, b, c)
#define uid_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(UID_LOG_ID, func, a, b, c, d)
#define uid_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(UID_LOG_ID, func, a, b, c, d, e)
#define uid_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(UID_LOG_ID, func, a, b, c, d, e, f)

#if __STDC__

extern	void		uid_init();
extern	dipc_return_t	dipc_uid_allocate( ipc_port_t );
extern	dipc_return_t	dipc_uid_install( ipc_port_t, dipc_uid_t );
extern	dipc_return_t	dipc_uid_destroy( ipc_port_t );
extern	ipc_port_t	dipc_port_lookup( dipc_uid_t );

#else

extern	void		uid_init();
extern	dipc_return_t	dipc_uid_allocate();
extern	dipc_return_t	dipc_uid_install();
extern	dipc_return_t	dipc_uid_destroy();
extern	ipc_port_t	dipc_port_lookup();

#endif 	/* __STDC__ */
#endif	/* _DIPC_UID_H_ */
